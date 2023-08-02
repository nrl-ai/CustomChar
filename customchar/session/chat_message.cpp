#include "chat_message.h"

#include <ctime>

using namespace CC;
using namespace CC::session;

ChatMessage::ChatMessage() {
  message_ = "";
  sender_ = "Unknown";
}

ChatMessage::ChatMessage(std::string in_message, std::string in_sender) {
  set_message(in_message, in_sender);
}

ChatMessage::~ChatMessage() {}

void ChatMessage::set_message(std::string input) { message_ = input; }

void ChatMessage::set_message(std::string in_message, std::string in_sender) {
  message_ = in_message;
  sender_ = in_sender;
}

void ChatMessage::set_sender(std::string input) { sender_ = input; }

void ChatMessage::set_time() {
  time_t time_now = time(NULL);
  char* ct = ctime(&time_now);
  timestamp_ = ct;
}

std::string ChatMessage::get_message() { return message_; }

std::string ChatMessage::get_sender() { return sender_; }

std::string ChatMessage::get_time() { return timestamp_; }
