/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RegexPatternException.cpp                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/21 10:51:02 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/21 10:52:51 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shared/exceptions/RegexPatternException.hpp"

#include <sstream>

namespace shared {
namespace exceptions {

const std::pair<RegexPatternException::ErrorCode, std::string>
    RegexPatternException::K_CODE_MSGS[] = {
        std::make_pair(RegexPatternException::EMPTY_PATTERN,
                       "Regular expression pattern cannot be empty"),
        std::make_pair(RegexPatternException::INVALID_CHARACTER,
                       "Regular expression contains invalid characters"),
        std::make_pair(RegexPatternException::UNBALANCED_BRACKETS,
                       "Unbalanced brackets in regular expression"),
        std::make_pair(RegexPatternException::UNBALANCED_PARENTHESES,
                       "Unbalanced parentheses in regular expression"),
        std::make_pair(RegexPatternException::UNBALANCED_BRACES,
                       "Unbalanced braces in regular expression"),
        std::make_pair(RegexPatternException::UNESCAPED_SPECIAL,
                       "Unescaped special character in regular expression"),
        std::make_pair(RegexPatternException::INVALID_ESCAPE,
                       "Invalid escape sequence in regular expression"),
        std::make_pair(RegexPatternException::PATTERN_TOO_LONG,
                       "Regular expression pattern too long"),
        std::make_pair(RegexPatternException::COMPILATION_FAILED,
                       "Failed to compile regular expression")};

RegexPatternException::RegexPatternException(const std::string& msg,
                                             ErrorCode code)
    : BaseException("", static_cast<int>(code)) {
  std::ostringstream oss;
  oss << getErrorMsg(code) << ": " << msg;
  this->m_whatMsg = oss.str();
}

RegexPatternException::RegexPatternException(const RegexPatternException& other)
    : BaseException(other) {}

RegexPatternException::~RegexPatternException() throw() {}

RegexPatternException& RegexPatternException::operator=(
    const RegexPatternException& other) {
  if (this != &other) {
    BaseException::operator=(other);
  }
  return *this;
}

std::string RegexPatternException::getErrorMsg(
    RegexPatternException::ErrorCode code) {
  for (int index = 0; index < CODE_COUNT; ++index) {
    if (K_CODE_MSGS[index].first == code) {
      return K_CODE_MSGS[index].second;
    }
  }
  return "unknown regular expression error";
}

}  // namespace exceptions
}  // namespace shared
