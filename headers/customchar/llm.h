#pragma once

#include "llama-cpp/llama.h"

#include <cassert>
#include <cstdio>
#include <fstream>
#include <regex>
#include <string>
#include <thread>
#include <vector>

namespace CC {

class LLM {
 private:
  int n_threads = std::min(4, (int)std::thread::hardware_concurrency());
  std::string path_session =
      "";  // path to file for saving/loading model eval state
  struct llama_context_params lparams;
  struct llama_context* ctx_llama;

  const std::string k_prompt_llama =
      R"(Text transcript of a never ending dialog, where {0} interacts with an AI assistant named {1}.
{1} is helpful, kind, honest, friendly, good at writing and never fails to answer {0}’s requests immediately and with details and precision.
There are no annotations like (30 seconds passed...) or (to himself), just what {0} and {1} say aloud to each other.
The transcript only includes text, it does not include markup like HTML and Markdown.
{1} responds with short and concise answers.

{0}{4} Hello, {1}!
{1}{4} Hello {0}! How may I help you today?
{0}{4} What time is it?
{1}{4} It is {2} o'clock.
{0}{4} What year is it?
{1}{4} We are in {3}.
{0}{4} What is a cat?
{1}{4} A cat is a domestic species of small carnivorous mammal. It is the only domesticated species in the family Felidae.
{0}{4} Name a color.
{1}{4} Blue
{0}{4})";
  std::string prompt_llama;
  std::string prompt = "";
  std::string person = "User";
  std::string bot_name = "CustomChar";
  const std::string chat_symb = ":";

  bool verbose_prompt = false;
  bool need_to_save_session = false;

  std::vector<llama_token> embd_inp;

  void init_prompt() {
    // Construct the initial prompt for LLaMA inference
    prompt_llama = prompt.empty() ? k_prompt_llama : prompt;

    // Need to have leading ' '
    prompt_llama.insert(0, 1, ' ');
    prompt_llama = replace(prompt_llama, "{0}", person);
    prompt_llama = replace(prompt_llama, "{1}", bot_name);

    {
      // Get time string
      std::string time_str;
      {
        time_t t = time(0);
        struct tm* now = localtime(&t);
        char buf[128];
        strftime(buf, sizeof(buf), "%H:%M", now);
        time_str = buf;
      }
      prompt_llama = replace(prompt_llama, "{2}", time_str);
    }

    {
      // get year string
      std::string year_str;
      {
        time_t t = time(0);
        struct tm* now = localtime(&t);
        char buf[128];
        strftime(buf, sizeof(buf), "%Y", now);
        year_str = buf;
      }
      prompt_llama = replace(prompt_llama, "{3}", year_str);
    }

    prompt_llama = replace(prompt_llama, "{4}", chat_symb);
  }

 public:
  LLM() {
    // Init Llama
    llama_init_backend();
    lparams = llama_context_default_params();

    // Tune these to your liking
    lparams.n_ctx = 2048;
    lparams.seed = 1;
    lparams.f16_kv = true;

    std::string model_llama =
        "../models/llama-2-7b-chat.ggmlv3.q4_0.bin";  // TODO: Fixit
    ctx_llama = llama_init_from_file(model_llama.c_str(), lparams);

    init_prompt();

    embd_inp = tokenize(prompt_llama, true);
  }

  ~LLM() {
    llama_print_timings(ctx_llama);
    llama_free(ctx_llama);
  }

  std::vector<llama_token> tokenize(const std::string& text, bool add_bos) {
    // initialize to prompt numer of chars, since n_tokens <= n_prompt_chars
    std::vector<llama_token> res(text.size() + (int)add_bos);
    int n = ::llama_tokenize(ctx_llama, text.c_str(), res.data(), res.size(),
                             add_bos);
    assert(n >= 0);
    res.resize(n);

    return res;
  }

  void add_tokens_to_current_session(const std::vector<llama_token>& tokens) {
    // Append the new input tokens to the session_tokens vector
    if (!path_session.empty()) {
      session_tokens.insert(session_tokens.end(), tokens.begin(), tokens.end());
    }
  }

  void load_session(std::string path_session) {
    this->path_session = path_session;
    std::vector<llama_token> session_tokens;
    fprintf(stderr, "%s: attempting to load saved session from %s\n", __func__,
            path_session.c_str());
    // fopen to check for existing session
    FILE* fp = std::fopen(path_session.c_str(), "rb");
    if (fp != NULL) {
      std::fclose(fp);

      session_tokens.resize(lparams.n_ctx);
      size_t n_token_count_out = 0;
      if (!llama_load_session_file(
              ctx_llama, path_session.c_str(), session_tokens.data(),
              session_tokens.capacity(), &n_token_count_out)) {
        fprintf(stderr, "%s: error: failed to load session file '%s'\n",
                __func__, path_session.c_str());
        exit(1);
      }
      session_tokens.resize(n_token_count_out);
      for (size_t i = 0; i < session_tokens.size(); i++) {
        embd_inp[i] = session_tokens[i];
      }
      fprintf(stderr, "%s: loaded a session with prompt size of %d tokens\n",
              __func__, (int)session_tokens.size());
    } else {
      fprintf(stderr, "%s: session file does not exist, will create\n",
              __func__);
    }
  }

  void eval_model() {
    printf(
        "Initializing... This may take a few minutes, depending on the model "
        "size.\n");
    if (llama_eval(ctx_llama, embd_inp.data(), embd_inp.size(), 0, n_threads)) {
      fprintf(stderr, "%s : failed to eval\n", __func__);
      exit(1);
    }

    if (verbose_prompt) {
      fprintf(stdout, "\n");
      fprintf(stdout, "%s", prompt_llama.c_str());
      fflush(stdout);
    }

    // debug message about similarity of saved session, if applicable
    size_t n_matching_session_tokens = 0;
    if (session_tokens.size()) {
      for (llama_token id : session_tokens) {
        if (n_matching_session_tokens >= embd_inp.size() ||
            id != embd_inp[n_matching_session_tokens]) {
          break;
        }
        n_matching_session_tokens++;
      }
      if (n_matching_session_tokens >= embd_inp.size()) {
        fprintf(stderr, "%s: session file has exact match for prompt!\n",
                __func__);
      } else if (n_matching_session_tokens < (embd_inp.size() / 2)) {
        fprintf(stderr,
                "%s: warning: session file has low similarity to prompt (%zu / "
                "%zu tokens); will mostly be reevaluated\n",
                __func__, n_matching_session_tokens, embd_inp.size());
      } else {
        fprintf(stderr, "%s: session file matches %zu / %zu tokens of prompt\n",
                __func__, n_matching_session_tokens, embd_inp.size());
      }
    }

    // HACK - because session saving incurs a non-negligible delay, for now skip
    // re-saving session if we loaded a session with at least 75% similarity.
    // It's currently just used to speed up the initial prompt so it doesn't
    // need to be an exact match.
    need_to_save_session =
        !path_session.empty() &&
        n_matching_session_tokens < (embd_inp.size() * 3 / 4);
  }

  std::string get_answer(std::vector<llama_token>& embd) {
    // Append the new input tokens to the session_tokens vector
    add_tokens_to_current_session(embd);

    bool done = false;
    std::string text_to_speak;
    while (true) {
      if (embd.size() > 0) {
        if (n_past + (int)embd.size() > n_ctx) {
          n_past = n_keep;
          // Insert n_left/2 tokens at the start of embd from last_n_tokens
          embd.insert(embd.begin(), embd_inp.begin() + embd_inp.size() - n_prev,
                      embd_inp.end());
          // Stop saving session if we run out of context
          path_session = "";
        }

        // Try to reuse a matching prefix from the loaded session instead of
        // re-eval (via n_past) REVIEW
        if (n_session_consumed < (int)session_tokens.size()) {
          size_t i = 0;
          for (; i < embd.size(); i++) {
            if (embd[i] != session_tokens[n_session_consumed]) {
              session_tokens.resize(n_session_consumed);
              break;
            }

            n_past++;
            n_session_consumed++;

            if (n_session_consumed >= (int)session_tokens.size()) {
              i++;
              break;
            }
          }
          if (i > 0) {
            embd.erase(embd.begin(), embd.begin() + i);
          }
        }

        if (embd.size() > 0 && !path_session.empty()) {
          session_tokens.insert(session_tokens.end(), embd.begin(), embd.end());
          n_session_consumed = session_tokens.size();
        }

        if (llama_eval(ctx_llama, embd.data(), embd.size(), n_past,
                       params.n_threads)) {
          fprintf(stderr, "%s : failed to eval\n", __func__);
          return 1;
        }
      }

      embd_inp.insert(embd_inp.end(), embd.begin(), embd.end());
      n_past += embd.size();

      embd.clear();

      if (done) break;

      {
        // out of user input, sample next token
        const float top_k = 10000;
        const float top_p = 0.80f;
        const float temp = 0.20f;
        const float repeat_penalty = 1.1f;

        const int repeat_last_n = 256;

        if (!path_session.empty() && need_to_save_session) {
          need_to_save_session = false;
          llama_save_session_file(ctx_llama, path_session.c_str(),
                                  session_tokens.data(), session_tokens.size());
        }

        llama_token id = 0;

        {
          auto logits = llama_get_logits(ctx_llama);
          auto n_vocab = llama_n_vocab(ctx_llama);

          logits[llama_token_eos()] = 0;

          std::vector<llama_token_data> candidates;
          candidates.reserve(n_vocab);
          for (llama_token token_id = 0; token_id < n_vocab; token_id++) {
            candidates.emplace_back(
                llama_token_data{token_id, logits[token_id], 0.0f});
          }

          llama_token_data_array candidates_p = {candidates.data(),
                                                 candidates.size(), false};

          // apply repeat penalty
          const float nl_logit = logits[llama_token_nl()];

          llama_sample_repetition_penalty(
              ctx_llama, &candidates_p,
              embd_inp.data() + std::max(0, n_past - repeat_last_n),
              repeat_last_n, repeat_penalty);

          logits[llama_token_nl()] = nl_logit;

          if (temp <= 0) {
            // Greedy sampling
            id = llama_sample_token_greedy(ctx_llama, &candidates_p);
          } else {
            // Temperature sampling
            llama_sample_top_k(ctx_llama, &candidates_p, top_k, 1);
            llama_sample_top_p(ctx_llama, &candidates_p, top_p, 1);
            llama_sample_temperature(ctx_llama, &candidates_p, temp);
            id = llama_sample_token(ctx_llama, &candidates_p);
          }
        }

        if (id != llama_token_eos()) {
          // add it to the context
          embd.push_back(id);
          text_to_speak += llama_token_to_str(ctx_llama, id);
          printf("%s", llama_token_to_str(ctx_llama, id));
        }
      }

      {
        std::string last_output;
        for (int i = embd_inp.size() - 16; i < (int)embd_inp.size(); i++) {
          last_output += llama_token_to_str(ctx_llama, embd_inp[i]);
        }
        last_output += llama_token_to_str(ctx_llama, embd[0]);

        for (std::string& antiprompt : antiprompts) {
          if (last_output.find(antiprompt.c_str(),
                               last_output.length() - antiprompt.length(),
                               antiprompt.length()) != std::string::npos) {
            done = true;
            text_to_speak = ::replace(text_to_speak, antiprompt, "");
            fflush(stdout);
            need_to_save_session = true;
            break;
          }
        }
      }

      is_running = sdl_poll_events();

      if (!is_running) {
        break;
      }
    }
  }
};

}  // namespace CC