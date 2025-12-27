/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HostException.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/24 14:46:04 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/27 03:33:14 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "domain/http/exceptions/HostException.hpp"

#include <sstream>

namespace domain {
namespace http {
namespace exceptions {

const std::pair<HostException::ErrorCode, std::string>
    HostException::K_CODE_MSGS[] = {
        std::make_pair(HostException::EMPTY_HOST, "Host cannot be empty"),
        std::make_pair(HostException::INVALID_FORMAT, "Invalid host format"),
        std::make_pair(HostException::TOO_LONG, "Host exceeds maximum length"),
        std::make_pair(HostException::INVALID_IPV4, "Invalid IPv4 address"),
        std::make_pair(HostException::INVALID_IPV6, "Invalid IPv6 address"),
        std::make_pair(HostException::INVALID_HOSTNAME, "Invalid hostname"),
        std::make_pair(HostException::MALFORMED,
                       "Malformed host specification")};

HostException::HostException(const std::string& msg, ErrorCode code)
    : BaseException("", static_cast<int>(code)) {
  std::ostringstream oss;
  oss << getErrorMsg(code) << ": " << msg;
  this->m_whatMsg = oss.str();
}

HostException::HostException(const HostException& other)
    : BaseException(other) {}

HostException::~HostException() throw() {}

HostException& HostException::operator=(const HostException& other) {
  if (this != &other) {
    BaseException::operator=(other);
  }
  return *this;
}

std::string HostException::getErrorMsg(HostException::ErrorCode code) {
  for (int i = 0; i < CODE_COUNT; ++i) {
    if (K_CODE_MSGS[i].first == code) {
      return K_CODE_MSGS[i].second;
    }
  }
  return "unknown host error";
}

}  // namespace exceptions
}  // namespace http
}  // namespace domain
