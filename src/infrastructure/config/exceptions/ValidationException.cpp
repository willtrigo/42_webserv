/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ValidationException.cpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/30 14:04:20 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/30 14:06:28 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "infrastructure/config/exceptions/ValidationException.hpp"

#include <sstream>

namespace infrastructure {
namespace config {
namespace exceptions {

const std::pair<ValidationException::ErrorCode, std::string>
    ValidationException::K_CODE_MSGS[] = {
        std::make_pair(ValidationException::DUPLICATE_PORT,
                       "duplicate listen port"),
        std::make_pair(ValidationException::INVALID_PATH, "invalid path"),
        std::make_pair(ValidationException::MISSING_DIRECTIVE,
                       "missing required directive"),
        std::make_pair(ValidationException::PORT_CONFLICT, "port conflict"),
        std::make_pair(ValidationException::INVALID_METHOD,
                       "invalid HTTP method")};

ValidationException::ValidationException(const std::string& msg, ErrorCode code)
    : BaseException("", static_cast<int>(code)) {
  std::ostringstream oss;
  oss << getErrorMsg(code) << ": " << msg;
  this->m_whatMsg = oss.str();
}

ValidationException::ValidationException(const ValidationException& other)
    : BaseException(other) {}

ValidationException::~ValidationException() throw() {}

ValidationException& ValidationException::operator=(
    const ValidationException& other) {
  if (this != &other) {
    BaseException::operator=(other);
  }
  return *this;
}

std::string ValidationException::getErrorMsg(ErrorCode code) {
  for (int i = 0; i < CODE_COUNT; ++i) {
    if (K_CODE_MSGS[i].first == code) {
      return K_CODE_MSGS[i].second;
    }
  }
  return "unknown validation error";
}

}  // namespace exceptions
}  // namespace config
}  // namespace infrastructure
