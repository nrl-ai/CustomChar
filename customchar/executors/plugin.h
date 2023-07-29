#ifndef CUSTOMCHAR_EXECUTORS_PLUGIN_H_
#define CUSTOMCHAR_EXECUTORS_PLUGIN_H_

#include <string>

namespace CC {
namespace executors {

class Plugin {
 private:
  std::string name_;

 public:
  Plugin(const std::string& name) : name_(name){};
  std::string& GetName() { return name_; };

  /// @brief Handle input and return response
  /// @param input user input string
  /// @param response response string
  /// @param finished true if finished, false otherwise
  /// @return true if handled, false otherwise
  virtual bool Handle(const std::string& input, std::string& response,
                      bool& finished) = 0;
};

}  // namespace executors
}  // namespace CC

#endif
