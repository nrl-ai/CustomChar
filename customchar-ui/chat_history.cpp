#include "chat_history.h"

ChatHistory::ChatHistory() { old_size = 0; }

ChatHistory::~ChatHistory() {}

void ChatHistory::add_message(std::string message, std::string sender) {
  ChatMessage new_message;
  new_message.set_time();
  new_message.set_message(message, sender);
  hist_vec.push_back(new_message);
}

std::vector<ChatMessage> ChatHistory::get_char_history() { return hist_vec; }

bool ChatHistory::has_new_message() {
  if (old_size != hist_vec.size()) {
    old_size = hist_vec.size();
    return true;
  }

  return false;
}
