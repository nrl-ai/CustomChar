#pragma once

#include <string>
#include <vector>

#include "chat_message.h"

class ChatHistory {
 public:
  ChatHistory();
  ~ChatHistory();

  void add_message(std::string, std::string);
  std::vector<ChatMessage> get_char_history();
  bool has_new_message();

 private:
  std::vector<ChatMessage> hist_vec;
  std::vector<ChatMessage>::size_type old_size;
};
