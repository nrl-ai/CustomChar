#include "customchar/llm/llm.h"

using namespace CC;

LLM::LLM(const std::string& model_path, const std::string& path_session,
         const std::string& person, const std::string& bot_name)
    : model_path_(model_path),
      path_session_(path_session),
      person_(person),
      bot_name_(bot_name) {
  // Init prompt
  InitPrompt();

  // Init Llama
  llama_init_backend();
  lparams_ = llama_context_default_params();

  // Tune these to your liking
  lparams_.n_ctx = 2048;
  lparams_.seed = 1;
  lparams_.f16_kv = true;

  // Load model to ram
  ctx_llama_ = llama_init_from_file(model_path_.c_str(), lparams_);
  n_ctx_ = llama_n_ctx(ctx_llama_);
  embd_inp_ = Tokenize(prompt_llama_, true);
  n_keep_ = embd_inp_.size();
  n_past_ = n_keep_;

  // Reverse prompts for detecting when it's time to stop speaking
  antiprompts_ = {
      person + chat_symb_,
  };

  // Load session
  LoadSession(path_session);
}

LLM::~LLM() {
  llama_print_timings(ctx_llama_);
  llama_free(ctx_llama_);
}

void LLM::InitPrompt() {
  // Construct the initial prompt for LLaMA inference
  prompt_llama_ = prompt_.empty() ? kPromptLlama : prompt_;

  // Need to have leading ' '
  prompt_llama_.insert(0, 1, ' ');
  prompt_llama_ = common::Replace(prompt_llama_, "{0}", person_);
  prompt_llama_ = common::Replace(prompt_llama_, "{1}", bot_name_);

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
    prompt_llama_ = common::Replace(prompt_llama_, "{2}", time_str);
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
    prompt_llama_ = common::Replace(prompt_llama_, "{3}", year_str);
  }

  prompt_llama_ = common::Replace(prompt_llama_, "{4}", chat_symb_);
}

std::vector<llama_token> LLM::Tokenize(const std::string& text, bool add_bos) {
  // initialize to prompt numer of chars, since n_tokens <= n_prompt_chars
  std::vector<llama_token> res(text.size() + (int)add_bos);
  int n = ::llama_tokenize(ctx_llama_, text.c_str(), res.data(), res.size(),
                           add_bos);
  assert(n >= 0);
  res.resize(n);

  return res;
}

void LLM::AddTokensToCurrentSession(const std::vector<llama_token>& tokens) {
  // Append the new input tokens to the session_tokens vector
  if (!path_session_.empty()) {
    session_tokens_.insert(session_tokens_.end(), tokens.begin(), tokens.end());
  }
}

void LLM::LoadSession(const std::string& path_session) {
  path_session_ = path_session;
  std::vector<llama_token> session_tokens;
  fprintf(stderr, "%s: attempting to load saved session from %s\n", __func__,
          path_session_.c_str());
  // fopen to check for existing session
  FILE* fp = std::fopen(path_session_.c_str(), "rb");
  if (fp != NULL) {
    std::fclose(fp);

    session_tokens.resize(lparams_.n_ctx);
    size_t n_token_count_out = 0;
    if (!llama_load_session_file(
            ctx_llama_, path_session_.c_str(), session_tokens.data(),
            session_tokens.capacity(), &n_token_count_out)) {
      fprintf(stderr, "%s: error: failed to load session file '%s'\n", __func__,
              path_session_.c_str());
      exit(1);
    }
    session_tokens.resize(n_token_count_out);
    for (size_t i = 0; i < session_tokens.size(); i++) {
      embd_inp_[i] = session_tokens[i];
    }
    fprintf(stderr, "%s: loaded a session with prompt size of %d tokens\n",
            __func__, (int)session_tokens.size());
  } else {
    fprintf(stderr, "%s: session file does not exist, will create\n", __func__);
  }

  n_session_consumed_ = !path_session_.empty() && session_tokens.size() > 0
                            ? session_tokens.size()
                            : 0;
}

void LLM::EvalModel() {
  printf(
      "Initializing... This may take a few minutes, depending on the model "
      "size.\n");
  if (llama_eval(ctx_llama_, embd_inp_.data(), embd_inp_.size(), 0,
                 n_threads_)) {
    fprintf(stderr, "%s : failed to eval\n", __func__);
    exit(1);
  }

  if (verbose_prompt_) {
    fprintf(stdout, "\n");
    fprintf(stdout, "%s", prompt_llama_.c_str());
    fflush(stdout);
  }

  // debug message about similarity of saved session, if applicable
  size_t n_matching_session_tokens = 0;
  if (session_tokens_.size()) {
    for (llama_token id : session_tokens_) {
      if (n_matching_session_tokens >= embd_inp_.size() ||
          id != embd_inp_[n_matching_session_tokens]) {
        break;
      }
      n_matching_session_tokens++;
    }
    if (n_matching_session_tokens >= embd_inp_.size()) {
      fprintf(stderr, "%s: session file has exact match for prompt!\n",
              __func__);
    } else if (n_matching_session_tokens < (embd_inp_.size() / 2)) {
      fprintf(stderr,
              "%s: warning: session file has low similarity to prompt (%zu / "
              "%zu tokens); will mostly be reevaluated\n",
              __func__, n_matching_session_tokens, embd_inp_.size());
    } else {
      fprintf(stderr, "%s: session file matches %zu / %zu tokens of prompt\n",
              __func__, n_matching_session_tokens, embd_inp_.size());
    }
  }

  // HACK - because session saving incurs a non-negligible delay, for now skip
  // re-saving session if we loaded a session with at least 75% similarity.
  // It's currently just used to speed up the initial prompt so it doesn't
  // need to be an exact match.
  need_to_save_session_ =
      !path_session_.empty() &&
      n_matching_session_tokens < (embd_inp_.size() * 3 / 4);
}

std::string LLM::GetAnswer(std::vector<llama_token>& embd) {
  bool done = false;
  int last_length = 0;
  int loop_count = 0;
  std::string text_to_speak;
  while (true) {
    if (embd.size() > 0) {
      if (n_past_ + (int)embd.size() > n_ctx_) {
        n_past_ = n_keep_;
        // Insert n_left/2 tokens at the start of embd from last_n_tokens
        embd.insert(embd.begin(),
                    embd_inp_.begin() + embd_inp_.size() - n_prev_,
                    embd_inp_.end());
        // Stop saving session if we run out of context
        path_session_ = "";
      }

      // Try to reuse a matching prefix from the loaded session instead of
      // re-eval (via n_past) REVIEW
      if (n_session_consumed_ < (int)session_tokens_.size()) {
        size_t i = 0;
        for (; i < embd.size(); i++) {
          if (embd[i] != session_tokens_[n_session_consumed_]) {
            session_tokens_.resize(n_session_consumed_);
            break;
          }

          n_past_++;
          n_session_consumed_++;

          if (n_session_consumed_ >= (int)session_tokens_.size()) {
            i++;
            break;
          }
        }
        if (i > 0) {
          embd.erase(embd.begin(), embd.begin() + i);
        }
      }

      if (embd.size() > 0 && !path_session_.empty()) {
        session_tokens_.insert(session_tokens_.end(), embd.begin(), embd.end());
        n_session_consumed_ = session_tokens_.size();
      }

      if (llama_eval(ctx_llama_, embd.data(), embd.size(), n_past_,
                     n_threads_)) {
        fprintf(stderr, "%s : failed to eval\n", __func__);
        exit(1);
      }
    }

    embd_inp_.insert(embd_inp_.end(), embd.begin(), embd.end());
    n_past_ += embd.size();

    embd.clear();

    if (done) break;

    {
      // out of user input, sample next token
      const float top_k = 10000;
      const float top_p = 0.80f;
      const float temp = 0.20f;
      const float repeat_penalty = 1.1f;
      const int repeat_last_n = 256;

      if (!path_session_.empty() && need_to_save_session_) {
        need_to_save_session_ = false;
        llama_save_session_file(ctx_llama_, path_session_.c_str(),
                                session_tokens_.data(), session_tokens_.size());
      }

      llama_token id = 0;

      {
        auto logits = llama_get_logits(ctx_llama_);
        auto n_vocab = llama_n_vocab(ctx_llama_);

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
            ctx_llama_, &candidates_p,
            embd_inp_.data() + std::max(0, n_past_ - repeat_last_n),
            repeat_last_n, repeat_penalty);

        logits[llama_token_nl()] = nl_logit;

        if (temp <= 0) {
          // Greedy sampling
          id = llama_sample_token_greedy(ctx_llama_, &candidates_p);
        } else {
          // Temperature sampling
          llama_sample_top_k(ctx_llama_, &candidates_p, top_k, 1);
          llama_sample_top_p(ctx_llama_, &candidates_p, top_p, 1);
          llama_sample_temperature(ctx_llama_, &candidates_p, temp);
          id = llama_sample_token(ctx_llama_, &candidates_p);
        }
      }

      if (id != llama_token_eos()) {
        // add it to the context
        embd.push_back(id);
        text_to_speak += llama_token_to_str(ctx_llama_, id);
        printf("%s", llama_token_to_str(ctx_llama_, id));
      }
    }

    {
      std::string last_output;
      for (int i = embd_inp_.size() - 16; i < (int)embd_inp_.size(); i++) {
        last_output += llama_token_to_str(ctx_llama_, embd_inp_[i]);
      }
      last_output += llama_token_to_str(ctx_llama_, embd[0]);

      for (std::string& antiprompt : antiprompts_) {
        if (last_output.find(antiprompt.c_str(),
                             last_output.length() - antiprompt.length(),
                             antiprompt.length()) != std::string::npos) {
          done = true;
          text_to_speak = common::Replace(text_to_speak, antiprompt, "");
          fflush(stdout);
          need_to_save_session_ = true;
          break;
        }
      }
    }

    // Break to avoid infinite loop
    // TODO: Fix this bug
    if ((int)text_to_speak.length() == last_length + 1 &&
        text_to_speak[text_to_speak.length() - 1] == '\n') {
      ++loop_count;
    } else {
      loop_count = 0;
    }
    if (loop_count > 5) {
      break;
    }
    last_length = text_to_speak.length();
  }

  return text_to_speak;
}
