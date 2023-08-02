#include "customchar/common/common.h"

#include <regex>

using namespace CC;
using namespace CC::common;

std::string CC::common::replace(const std::string& s, const std::string& from,
                                const std::string& to) {
  std::string result = s;
  size_t pos = 0;
  while ((pos = result.find(from, pos)) != std::string::npos) {
    result.replace(pos, from.length(), to);
    pos += to.length();
  }
  return result;
}

std::string CC::common::trim(const std::string& s) {
  std::regex e("^\\s+|\\s+$");
  return std::regex_replace(s, e, "");
}
