#pragma once

#include <ctime>
#include <string>

class ChatMessage {
 public:
  ChatMessage();
  ChatMessage(std::string, std::string);
  ~ChatMessage();

  void set_message(std::string);
  void set_message(std::string, std::string);
  void set_sender(std::string);
  void set_time();

  std::string get_message();
  std::string get_sender();
  std::string get_time();

 private:
  std::string message;
  std::string sender;
  std::string timestamp;
};
