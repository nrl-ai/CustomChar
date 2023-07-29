#ifndef CUSTOMCHAR_LLM_LLM_H_
#define CUSTOMCHAR_LLM_LLM_H_

#include "customchar/common/common.h"
#include "llama-cpp/llama.h"

#include <cassert>
#include <cstdio>
#include <fstream>
#include <regex>
#include <string>
#include <thread>
#include <vector>

namespace CC {
namespace llm {

const std::string kPromptLlama =
    R"(Text transcript of a dialog, where {0} interacts with an AI assistant named {1}.
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
  std::string model_path_;
  std::string person_ = "User";
  std::string bot_name_ = "CustomChar";
  const std::string chat_symb_ = ":";
  bool verbose_prompt_ = false;
  bool need_to_save_session_ = false;

  int n_threads_ = std::min(4, (int)std::thread::hardware_concurrency());
  std::string path_session_;
  struct llama_context_params lparams_;
  struct llama_context* ctx_llama_;

  std::string prompt_llama_;
  std::string prompt_ = "";

  std::vector<llama_token> embd_inp_;
  std::vector<llama_token> session_tokens_;
  std::vector<std::string> antiprompts_;

  int n_keep_;
  int n_ctx_;
  int n_past_;
  int n_prev_ = 64;
  int n_session_consumed_;

  /// @brief Initialize prompt
  void InitPrompt();

 public:
  /// @brief Constructor
  /// @param model_path Path to the model
  /// @param path_session Path to the session
  LLM(const std::string& model_path, const std::string& path_session = "",
      const std::string& person = "User",
      const std::string& bot_name = "CustomChar");
  ~LLM();

  /// @brief Evaluate the model. Run this function before inference.
  void EvalModel();

  /// @brief Load session from file
  /// @param path_session Path to the session
  void LoadSession(const std::string& path_session);

  /// @brief Add new tokens to the sessions
  void AddTokensToCurrentSession(const std::vector<llama_token>& tokens);

  /// @brief Tokenize text
  std::vector<llama_token> Tokenize(const std::string& text, bool add_bos);

  /// @brief Get answer from LLM
  std::string GetAnswer(std::vector<llama_token>& embd);
};

}  // namespace llm
}  // namespace CC

#endif  // CUSTOMCHAR_LLM_LLM_H_
