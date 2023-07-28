#include "chat_message.h"

#include <ctime>

ChatMessage::ChatMessage() {
  message_ = "";
  sender_ = "Unknown";
}

ChatMessage::ChatMessage(std::string in_message, std::string in_sender) {
  SetMessage(in_message, in_sender);
}

ChatMessage::~ChatMessage() {}

void ChatMessage::SetMessage(std::string input) { message_ = input; }

void ChatMessage::SetMessage(std::string in_message, std::string in_sender) {
  message_ = in_message;
  sender_ = in_sender;
}

void ChatMessage::SetSender(std::string input) { sender_ = input; }

void ChatMessage::SetTime() {
  time_t time_now = time(NULL);
  char* ct = ctime(&time_now);
  timestamp_ = ct;
}

std::string ChatMessage::GetMessage() { return message_; }

std::string ChatMessage::GetSender() { return sender_; }

std::string ChatMessage::GetTime() { return timestamp_; }
