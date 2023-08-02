#include "customchar/executors/plugin_executor.h"

using namespace CC::executors;

std::string PluginExecutor::reflect(const std::string& word) {
  auto it = reflections_.find(word);
  if (it != reflections_.end()) {
    return it->second;
  }
  return word;
}

bool PluginExecutor::parse_and_execute(const std::string& input,
                                       std::string& response) {
  // Continue with current plugin
  if (current_plugin_) {
    bool finished = false;
    bool handled = current_plugin_->handle(input, response, finished);
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

      // Run executor here
      for (const auto& plugin : plugins_) {
        if (plugin->get_name() == plugin_name) {
          current_plugin_ = plugin;
          bool finished = false;
          bool handled = current_plugin_->handle(input, response, finished);
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
