#ifndef CUSTOMCHAR_SESSION_CHAT_MESSAGE_H_
#define CUSTOMCHAR_SESSION_CHAT_MESSAGE_H_

#include <ctime>
#include <string>

namespace CC {
namespace session {

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
  std::string message_;
  std::string sender_;
  std::string timestamp_;
};

}  // namespace session
}  // namespace CC

#endif  // CUSTOMCHAR_SESSION_CHAT_MESSAGE_H_
