/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpMethodException.cpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/20 23:54:03 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/27 03:24:55 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "domain/http/exceptions/HttpMethodException.hpp"

#include <sstream>

namespace domain {
namespace http {
namespace exceptions {

const std::pair<HttpMethodException::ErrorCode, std::string>
    HttpMethodException::K_CODE_MSGS[] = {
        std::make_pair(HttpMethodException::EMPTY_STRING,
                       "HTTP method string cannot be empty"),
        std::make_pair(HttpMethodException::INVALID_METHOD,
                       "Invalid HTTP method format"),
        std::make_pair(HttpMethodException::UNKNOWN_METHOD,
                       "Unknown HTTP method")};

HttpMethodException::HttpMethodException(const std::string& msg, ErrorCode code)
    : BaseException("", static_cast<int>(code)) {
  std::ostringstream oss;
  oss << getErrorMsg(code) << ": " << msg;
  this->m_whatMsg = oss.str();
}

HttpMethodException::HttpMethodException(const HttpMethodException& other)
    : BaseException(other) {}

HttpMethodException::~HttpMethodException() throw() {}

HttpMethodException& HttpMethodException::operator=(
    const HttpMethodException& other) {
  if (this != &other) {
    BaseException::operator=(other);
  }
  return *this;
}

std::string HttpMethodException::getErrorMsg(
    HttpMethodException::ErrorCode code) {
  for (int index = 0; index < CODE_COUNT; ++index) {
    if (K_CODE_MSGS[index].first == code) {
      return K_CODE_MSGS[index].second;
    }
  }
  return "unknown HTTP method error";
}

}  // namespace exceptions
}  // namespace http
}  // namespace domain
