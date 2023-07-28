#ifndef CUSTOMCHAR_SESSION_CHAT_HISTORY_H_
#define CUSTOMCHAR_SESSION_CHAT_HISTORY_H_

#include <string>
#include <vector>

#include "chat_message.h"

class ChatHistory {
 public:
  ChatHistory();
  ~ChatHistory();

  void AddMessage(std::string, std::string);
  std::vector<ChatMessage> GetCharHistory();
  bool HasNewMessage();

 private:
  std::vector<ChatMessage> hist_vec_;
  std::vector<ChatMessage>::size_type old_size_;
};

#endif  // CUSTOMCHAR_SESSION_CHAT_HISTORY_H_
