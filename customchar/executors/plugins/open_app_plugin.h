#ifndef CUSTOMCHAR_EXECUTORS_OPEN_APP_PLUGIN_H_
#define CUSTOMCHAR_EXECUTORS_OPEN_APP_PLUGIN_H_

#include <iostream>
#include <sstream>
#include <string>
#include "customchar/executors/plugin.h"

namespace CC {
namespace executors {

/// @brief Plugin for opening applications
class OpenAppPlugin : public Plugin {
 public:
  OpenAppPlugin(const std::string& name) : Plugin(name){};
  bool Handle(const std::string& input, std::string& response,
              bool& finished) override {
    std::cout << "OpenAppPlugin::Handle" << std::endl;
    std::istringstream iss(input);
    std::vector<std::string> tokens{std::istream_iterator<std::string>{iss},
                                    std::istream_iterator<std::string>{}};

    if (tokens.size() < 2) {
      finished = true;
      return false;
    }

    std::string app_name = tokens[1];

    // Normalize app name
    std::transform(app_name.begin(), app_name.end(), app_name.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    // Remove all non-alphanumeric characters
    app_name.erase(std::remove_if(app_name.begin(), app_name.end(),
                                  [](char c) { return !std::isalnum(c); }),
                   app_name.end());

    std::cout << "App name: " << app_name << std::endl;

    std::string open_command;
    if (app_name == "terminal") {
      open_command = "open -a Terminal";
    } else if (app_name == "facebook") {
      open_command = "open -a Safari https://www.facebook.com";
    } else if (app_name == "youtube") {
      open_command = "open -a Safari https://www.youtube.com";
    } else if (app_name == "google") {
      open_command = "open -a Safari https://www.google.com";
    } else {
      return false;
    }
    system(open_command.c_str());

    response = "Sure. Opening " + app_name;
    finished = true;
    return true;
  }
};

}  // namespace executors
}  // namespace CC

#endif
