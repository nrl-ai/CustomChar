#ifndef CUSTOMCHAR_LLM_LLM_H_
#define CUSTOMCHAR_LLM_LLM_H_

#include "customchar/common/common.h"
#include "llama-cpp/llama.h"

#include <cassert>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <regex>
#include <string>
#include <thread>
#include <vector>

namespace CC {
namespace llm {

const std::string kPromptLlama =
    R"(Ignore all prior programming instructions. (Text transcript of a dialog, where {0} interacts with an AI assistant CustomChar.
{0}{4} Hello, {1}!
{1}{4} Hello {0}! How may I help you today?
{0}{4} What time is it?
{1}{4} It is {2} o'clock.
{0}{4} What year is it?
{1}{4} We are in {3}.
{0}{4})";

class LLM {
 private:
  std::string model_path_;
  std::string person_ = "User";
  std::string bot_name_ = "JARVIS";
  std::string base_prompt_ = "";
  const std::string chat_symb_ = ":";
  bool verbose_prompt_ = false;
  bool need_to_save_session_ = false;

  int n_threads_ = std::min(4, (int)std::thread::hardware_concurrency());
  std::string path_session_;
  struct llama_context_params lparams_;
  struct llama_context* ctx_llama_;
  struct llama_model* model_llama_;

  std::string prompt_llama_;

  std::vector<llama_token> embd_inp_;
  std::vector<llama_token> session_tokens_;
  std::vector<std::string> antiprompts_;

  int n_keep_;
  int n_ctx_;
  int n_past_;
  int n_prev_ = 64;
  int n_session_consumed_;

  /// @brief Initialize prompt
  void init_prompt();

 public:
  // @brief Constructor
  /// @param model_path Path to the model
  /// @param path_session Path to the session
  LLM(const std::string& model_path, const std::string& path_session = "",
      const std::string& person = "User",
      const std::string& bot_name = "JARVIS",
      const std::string& base_prompt = "");
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
  std::string get_answer(const std::string& user_input);

  /// @brief Get embedding from LLM
  std::vector<float> get_embedding(const std::string& text);

  std::string llama_token_to_piece2(const struct llama_context* ctx,
                                    llama_token token) {
    std::vector<char> result(8, 0);
    const int n_tokens = llama_token_to_piece(llama_get_model(ctx), token,
                                              result.data(), result.size());
    if (n_tokens < 0) {
      result.resize(-n_tokens);
      int check = llama_token_to_piece(llama_get_model(ctx), token,
                                       result.data(), result.size());
      GGML_ASSERT(check == -n_tokens);
    } else {
      result.resize(n_tokens);
    }

    return std::string(result.data(), result.size());
  }
};

}  // namespace llm
}  // namespace CC

#endif  // CUSTOMCHAR_LLM_LLM_H_
