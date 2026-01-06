/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpHeaderException.cpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/04 13:53:19 by dande-je          #+#    #+#             */
/*   Updated: 2026/01/04 13:54:07 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpHeaderException.hpp"

#include <sstream>

namespace domain {
namespace http {
namespace exceptions {

const std::pair<HttpHeaderException::ErrorCode, std::string>
    HttpHeaderException::K_CODE_MSGS[] = {
        std::make_pair(HttpHeaderException::INVALID_FORMAT,
                       "Invalid HTTP header format"),
        std::make_pair(HttpHeaderException::EMPTY_NAME,
                       "HTTP header name cannot be empty"),
        std::make_pair(HttpHeaderException::EMPTY_VALUE,
                       "HTTP header value cannot be empty"),
        std::make_pair(HttpHeaderException::NAME_TOO_LONG,
                       "HTTP header name too long"),
        std::make_pair(HttpHeaderException::VALUE_TOO_LONG,
                       "HTTP header value too long"),
        std::make_pair(HttpHeaderException::INVALID_CHARACTER_IN_NAME,
                       "Invalid character in HTTP header name"),
        std::make_pair(HttpHeaderException::INVALID_CHARACTER_IN_VALUE,
                       "Invalid character in HTTP header value"),
        std::make_pair(HttpHeaderException::MISSING_COLON_SEPARATOR,
                       "HTTP header missing colon separator"),
        std::make_pair(HttpHeaderException::DUPLICATE_HEADER,
                       "Duplicate HTTP header"),
        std::make_pair(HttpHeaderException::REQUIRED_HEADER_MISSING,
                       "Required HTTP header missing")};

HttpHeaderException::HttpHeaderException(const std::string& msg, ErrorCode code)
    : BaseException("", static_cast<int>(code)) {
  std::ostringstream oss;
  oss << getErrorMsg(code) << ": " << msg;
  this->m_whatMsg = oss.str();
}

HttpHeaderException::HttpHeaderException(const HttpHeaderException& other)
    : BaseException(other) {}

HttpHeaderException::~HttpHeaderException() throw() {}

HttpHeaderException& HttpHeaderException::operator=(
    const HttpHeaderException& other) {
  if (this != &other) {
    BaseException::operator=(other);
  }
  return *this;
}

std::string HttpHeaderException::getErrorMsg(
    HttpHeaderException::ErrorCode code) {
  for (int i = 0; i < CODE_COUNT; ++i) {
    if (K_CODE_MSGS[i].first == code) {
      return K_CODE_MSGS[i].second;
    }
  }
  return "Unknown HTTP header error";
}

}  // namespace exceptions
}  // namespace http
}  // namespace domain
