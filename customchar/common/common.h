#ifndef CUSTOMCHAR_COMMON_COMMON_H_
#define CUSTOMCHAR_COMMON_COMMON_H_

#include <string>

namespace CC {
namespace common {

/// @brief Replace all occurrences of a substring with another substring.
/// @param s Original string.
/// @param from Replace this substring.
/// @param to With this substring.
/// @return New string with all occurrences of `from` replaced with `to`.
std::string replace(const std::string& s, const std::string& from,
                    const std::string& to);

/// @brief Trim whitespace from both ends of a string.
/// @param s Original string.
/// @return New string with whitespace removed from both ends.
std::string trim(const std::string& s);

}  // namespace common
}  // namespace CC

#endif  // CUSTOMCHAR_COMMON_COMMON_H_
