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

#ifdef __APPLE__
    std::string open_command = "open ";
#elif __linux__
    std::string open_command = "xdg-open ";
#elif _WIN32
    std::string open_command = "start ";
#endif

    if (app_name == "terminal") {
#ifdef __APPLE__
      open_command += "-a Terminal";
#elif __linux__
      open_command += "gnome-terminal";
#elif _WIN32
      open_command += "cmd";
#endif
    } else if (app_name == "facebook") {
      open_command += "https://www.facebook.com";
    } else if (app_name == "youtube") {
      open_command += "https://www.youtube.com";
    } else if (app_name == "google") {
      open_command += "https://www.google.com";
    } else {
      return false;
    }
    system(open_command.c_str());

    std::cout << ">> " << app_name << ": " << open_command << std::endl;
    response = "Sure. Opening " + app_name;
    finished = true;
    return true;
  }
};

}  // namespace executors
}  // namespace CC

#endif
