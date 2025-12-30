/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SyntaxException.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/30 13:30:45 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/30 14:07:20 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "infrastructure/config/exceptions/SyntaxException.hpp"

#include <sstream>

namespace infrastructure {
namespace config {
namespace exceptions {

const std::pair<SyntaxException::ErrorCode, std::string>
    SyntaxException::K_CODE_MSGS[] = {
        std::make_pair(SyntaxException::UNEXPECTED_TOKEN, "unexpected token"),
        std::make_pair(SyntaxException::UNEXPECTED_EOF,
                       "unexpected end of file"),
        std::make_pair(SyntaxException::MISSING_SEMICOLON, "missing semicolon"),
        std::make_pair(SyntaxException::MISSING_BRACE, "missing brace"),
        std::make_pair(SyntaxException::INVALID_DIRECTIVE,
                       "invalid directive")};

SyntaxException::SyntaxException(const std::string& msg, ErrorCode code)
    : BaseException("", static_cast<int>(code)) {
  std::ostringstream oss;
  oss << getErrorMsg(code) << ": " << msg;
  this->m_whatMsg = oss.str();
}

SyntaxException::SyntaxException(const SyntaxException& other)
    : BaseException(other) {}

SyntaxException::~SyntaxException() throw() {}

SyntaxException& SyntaxException::operator=(const SyntaxException& other) {
  if (this != &other) {
    BaseException::operator=(other);
  }
  return *this;
}

std::string SyntaxException::getErrorMsg(ErrorCode code) {
  for (int i = 0; i < CODE_COUNT; ++i) {
    if (K_CODE_MSGS[i].first == code) {
      return K_CODE_MSGS[i].second;
    }
  }
  return "unknown syntax error";
}

}  // namespace exceptions
}  // namespace config
}  // namespace infrastructure
