/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpVersionException.cpp                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/04 13:41:49 by dande-je          #+#    #+#             */
/*   Updated: 2026/01/04 13:42:27 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpVersionException.hpp"

#include <sstream>

namespace domain {
namespace http {
namespace exceptions {

const std::pair<HttpVersionException::ErrorCode, std::string>
    HttpVersionException::K_CODE_MSGS[] = {
        std::make_pair(HttpVersionException::INVALID_FORMAT,
                       "Invalid HTTP version format"),
        std::make_pair(HttpVersionException::UNSUPPORTED_VERSION,
                       "Unsupported HTTP version"),
        std::make_pair(HttpVersionException::EMPTY_STRING,
                       "HTTP version string is empty"),
        std::make_pair(HttpVersionException::INVALID_MAJOR_VERSION,
                       "Invalid major version number"),
        std::make_pair(HttpVersionException::INVALID_MINOR_VERSION,
                       "Invalid minor version number"),
        std::make_pair(HttpVersionException::VERSION_TOO_OLD,
                       "HTTP version too old"),
        std::make_pair(HttpVersionException::VERSION_TOO_NEW,
                       "HTTP version too new")};

HttpVersionException::HttpVersionException(const std::string& msg,
                                           ErrorCode code)
    : BaseException("", static_cast<int>(code)) {
  std::ostringstream oss;
  oss << getErrorMsg(code) << ": " << msg;
  this->m_whatMsg = oss.str();
}

HttpVersionException::HttpVersionException(const HttpVersionException& other)
    : BaseException(other) {}

HttpVersionException::~HttpVersionException() throw() {}

HttpVersionException& HttpVersionException::operator=(
    const HttpVersionException& other) {
  if (this != &other) {
    BaseException::operator=(other);
  }
  return *this;
}

std::string HttpVersionException::getErrorMsg(
    HttpVersionException::ErrorCode code) {
  for (int i = 0; i < CODE_COUNT; ++i) {
    if (K_CODE_MSGS[i].first == code) {
      return K_CODE_MSGS[i].second;
    }
  }
  return "Unknown HTTP version error";
}

}  // namespace exceptions
}  // namespace http
}  // namespace domain
