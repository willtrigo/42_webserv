/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParserException.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/30 14:08:03 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/30 17:09:21 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "infrastructure/config/exceptions/ParserException.hpp"

#include <sstream>

namespace infrastructure {
namespace config {
namespace exceptions {

const std::pair<ParserException::ErrorCode, std::string>
    ParserException::K_CODE_MSGS[] = {
        std::make_pair(ParserException::FILE_NOT_FOUND,
                       "config file not found or unreadable"),
        std::make_pair(ParserException::PERMISSION_DENIED,
                       "permission denied for config file"),
        std::make_pair(ParserException::INCLUDE_RECURSION,
                       "recursive include detected"),
        std::make_pair(ParserException::UNEXPECTED_ERROR,
                       "unexpected error during parsing")};

ParserException::ParserException(const std::string& msg, ErrorCode code)
    : BaseException("", static_cast<int>(code)) {
  std::ostringstream oss;
  oss << getErrorMsg(code) << ": " << msg;
  this->m_whatMsg = oss.str();
}

ParserException::ParserException(const ParserException& other)
    : BaseException(other) {}

ParserException::~ParserException() throw() {}

ParserException& ParserException::operator=(const ParserException& other) {
  if (this != &other) {
    BaseException::operator=(other);
  }
  return *this;
}

std::string ParserException::getErrorMsg(ErrorCode code) {
  for (int i = 0; i < CODE_COUNT; ++i) {
    if (K_CODE_MSGS[i].first == code) {
      return K_CODE_MSGS[i].second;
    }
  }
  return "unknown parse error";
}

}  // namespace exceptions
}  // namespace config
}  // namespace infrastructure
