#pragma once

#include "llama-cpp/llama.h"
#include "whisper-cpp/examples/common-sdl.h"  // TODO Remove this dependency

#include "customchar/common.h"

#include <cassert>
#include <cstdio>
#include <fstream>
#include <regex>
#include <string>
#include <thread>
#include <vector>

namespace CC {

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

class LLM {
 private:
  std::string model_path;
  std::string person = "User";
  std::string bot_name = "CustomChar";
  const std::string chat_symb = ":";
  bool verbose_prompt = false;
  bool need_to_save_session = false;

  int n_threads = std::min(4, (int)std::thread::hardware_concurrency());
  std::string path_session;
  struct llama_context_params lparams;
  struct llama_context* ctx_llama;

  std::string prompt_llama;
  std::string prompt = "";

  std::vector<llama_token> embd_inp;
  std::vector<llama_token> session_tokens;
  std::vector<std::string> antiprompts;

  int n_keep;
  int n_ctx;
  int n_past;
  int n_prev = 64;
  int n_session_consumed;

  /// @brief Initialize prompt
  void init_prompt();

 public:
  /// @brief Constructor
  /// @param model_path Path to the model
  /// @param path_session Path to the session
  LLM(const std::string& model_path, const std::string& path_session = "",
      const std::string& person = "User",
      const std::string& bot_name = "CustomChar");
  ~LLM();

  /// @brief Evaluate the model. Run this function before inference.
  void eval_model();

  /// @brief Load session from file
  /// @param path_session Path to the session
  void load_session(const std::string& path_session);

  /// @brief Add new tokens to the sessions
  void add_tokens_to_current_session(const std::vector<llama_token>& tokens);

  /// @brief Tokenize text
  std::vector<llama_token> tokenize(const std::string& text, bool add_bos);

  /// @brief Get answer from LLM
  std::string get_answer(std::vector<llama_token>& embd);
};

}  // namespace CC
