#include "chat_history.h"

using namespace CC;
using namespace CC::session;

ChatHistory::ChatHistory() { old_size_ = 0; }

ChatHistory::~ChatHistory() {}

void ChatHistory::add_message(std::string message, std::string sender) {
  ChatMessage new_message;
  new_message.set_time();
  new_message.set_message(message, sender);
  hist_vec_.push_back(new_message);
}

std::vector<ChatMessage> ChatHistory::get_chat_history() { return hist_vec_; }

bool ChatHistory::has_new_message() {
  if (old_size_ != hist_vec_.size()) {
    old_size_ = hist_vec_.size();
    return true;
  }

  return false;
}
