#include "chat_history.h"

ChatHistory::ChatHistory() { old_size_ = 0; }

ChatHistory::~ChatHistory() {}

void ChatHistory::AddMessage(std::string message, std::string sender) {
  ChatMessage new_message;
  new_message.SetTime();
  new_message.SetMessage(message, sender);
  hist_vec_.push_back(new_message);
}

std::vector<ChatMessage> ChatHistory::GetCharHistory() { return hist_vec_; }

bool ChatHistory::HasNewMessage() {
  if (old_size_ != hist_vec_.size()) {
    old_size_ = hist_vec_.size();
    return true;
  }

  return false;
}
