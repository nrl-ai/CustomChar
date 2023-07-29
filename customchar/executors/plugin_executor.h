#ifndef CUSTOMCHAR_EXECUTORS_PLUGIN_EXECUTOR_H_
#define CUSTOMCHAR_EXECUTORS_PLUGIN_EXECUTOR_H_

#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <map>
#include <regex>
#include <sstream>
#include <string>
#include <vector>
#include "customchar/executors/plugin.h"
#include "customchar/executors/plugins/open_app_plugin.h"

namespace CC {
namespace executors {

class PluginExecutor {
 private:
  std::map<std::string, std::string> reflections_ = {
      {"i am", "you are"},   {"i was", "you were"}, {"i", "you"},
      {"i'm", "you are"},    {"i'd", "you would"},  {"i've", "you have"},
      {"i'll", "you will"},  {"my", "your"},        {"you are", "I am"},
      {"you were", "I was"}, {"you've", "I have"},  {"you'll", "I will"},
      {"your", "my"},        {"yours", "mine"},     {"you", "me"},
      {"me", "you"}};
  std::vector<std::vector<std::string>> pairs_ = {
      {"open (.*)", "OPEN_APP"},
      {"open (.*) app", "OPEN_APP"},
      {"(.*)who\\s+.*\\s+created\\s+.*\\?",
       "I was created by CustomChar team."},
  };

  std::string Reflect(const std::string& word) {
    auto it = reflections_.find(word);
    if (it != reflections_.end()) {
      return it->second;
    }
    return word;
  }

  std::vector<std::shared_ptr<Plugin>> plugins_ = {
      std::make_shared<OpenAppPlugin>("OPEN_APP"),
  };
  std::shared_ptr<Plugin> current_plugin_;

 public:
  bool ParseAndExecute(const std::string& input, std::string& response) {
    // Continue with current plugin
    if (current_plugin_) {
      bool finished = false;
      bool handled = current_plugin_->Handle(input, response, finished);
      if (finished) {
        current_plugin_ = nullptr;
      }
      if (handled) {
        return true;
      }
    }

    // Check if input matches any pattern
    bool pattern_matched = false;
    for (const auto& pair : pairs_) {
      std::regex pattern(pair[0], std::regex_constants::icase);
      std::smatch matches;
      if (std::regex_match(input, matches, pattern)) {
        pattern_matched = true;
        std::string plugin_name = pair[1];

        // std::regex word_regex("(%\\d+)");
        // std::sregex_iterator it(raw_response.begin(), raw_response.end(),
        //                         word_regex);
        // std::sregex_iterator end;
        // while (it != end) {
        //   std::smatch match = *it;
        //   std::string match_str = match.str();
        //   int match_index = std::stoi(match_str.substr(1));
        //   std::string reflection = Reflect(matches.str(match_index));
        //   raw_response = std::regex_replace(raw_response,
        //   std::regex(match_str),
        //                                     reflection);
        //   ++it;
        // }

        // Run executor here
        for (const auto& plugin : plugins_) {
          if (plugin->GetName() == plugin_name) {
            current_plugin_ = plugin;
            bool finished = false;
            bool handled = current_plugin_->Handle(input, response, finished);
            if (finished) {
              current_plugin_ = nullptr;
            }
            if (handled) {
              return true;
            }
          }
        }

        response = plugin_name;
        return true;
      }
    }

    return false;
  }
};

}  // namespace executors
}  // namespace CC

#endif  // CUSTOMCHAR_EXECUTORS_COMMAND_PARSER_H_
