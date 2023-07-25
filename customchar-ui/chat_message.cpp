#include "chat_message.h"

#include <ctime>

ChatMessage::ChatMessage() {
  message = "";
  sender = "Unknown";
}

ChatMessage::ChatMessage(std::string in_message, std::string in_sender) {
  set_message(in_message, in_sender);
}

ChatMessage::~ChatMessage() {}

void ChatMessage::set_message(std::string input) { message = input; }

void ChatMessage::set_message(std::string in_message, std::string in_sender) {
  message = in_message;
  sender = in_sender;
}

void ChatMessage::set_sender(std::string input) { sender = input; }

void ChatMessage::set_time() {
  time_t time_now = time(NULL);
  char* ct = ctime(&time_now);
  timestamp = ct;
}

std::string ChatMessage::get_message() { return message; }

std::string ChatMessage::get_sender() { return sender; }

std::string ChatMessage::get_time() { return timestamp; }
