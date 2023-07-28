#ifndef CUSTOMCHAR_SESSION_CHAT_MESSAGE_H_
#define CUSTOMCHAR_SESSION_CHAT_MESSAGE_H_

#include <ctime>
#include <string>

class ChatMessage {
 public:
  ChatMessage();
  ChatMessage(std::string, std::string);
  ~ChatMessage();

  void SetMessage(std::string);
  void SetMessage(std::string, std::string);
  void SetSender(std::string);
  void SetTime();

  std::string GetMessage();
  std::string GetSender();
  std::string GetTime();

 private:
  std::string message_;
  std::string sender_;
  std::string timestamp_;
};

#endif  // CUSTOMCHAR_SESSION_CHAT_MESSAGE_H_
