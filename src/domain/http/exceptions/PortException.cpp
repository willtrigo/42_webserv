/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PortException.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/19 23:50:27 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/27 02:27:43 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "domain/http/exceptions/PortException.hpp"

#include <sstream>

namespace domain {
namespace http {
namespace exceptions {

const std::pair<PortException::ErrorCode, std::string>
    PortException::K_CODE_MSGS[] = {
        std::make_pair(PortException::INVALID_STRING,
                       "Invalid port string format"),
        std::make_pair(PortException::OUT_OF_RANGE,
                       "Port value out of valid range (0-65535)"),
        std::make_pair(PortException::NON_DIGIT_CHARACTER,
                       "Port string contains non-digit characters"),
        std::make_pair(PortException::CONVERSION_FAILED,
                       "Failed to convert port string to number"),
        std::make_pair(PortException::EMPTY_STRING, "Port string is empty")};

PortException::PortException(const std::string& msg, ErrorCode code)
    : BaseException("", static_cast<int>(code)) {
  std::ostringstream oss;
  oss << getErrorMsg(code) << ": " << msg;
  this->m_whatMsg = oss.str();
}

PortException::PortException(const PortException& other)
    : BaseException(other) {}

PortException::~PortException() throw() {}

PortException& PortException::operator=(const PortException& other) {
  if (this != &other) {
    BaseException::operator=(other);
  }
  return *this;
}

std::string PortException::getErrorMsg(PortException::ErrorCode code) {
  for (int i = 0; i < CODE_COUNT; ++i) {
    if (K_CODE_MSGS[i].first == code) {
      return K_CODE_MSGS[i].second;
    }
  }
  return "unknown config error";
}

}  // namespace exceptions
}  // namespace http
}  // namespace domain
