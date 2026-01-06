/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponseException.cpp                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/04 15:09:44 by dande-je          #+#    #+#             */
/*   Updated: 2026/01/04 15:10:29 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "domain/http/exceptions/HttpResponseException.hpp"

#include <sstream>

namespace domain {
namespace http {
namespace exceptions {

const std::pair<HttpResponseException::ErrorCode, std::string>
    HttpResponseException::K_CODE_MSGS[] = {
        std::make_pair(HttpResponseException::INVALID_RESPONSE,
                       "Invalid HTTP response"),
        std::make_pair(HttpResponseException::INVALID_STATUS_CODE,
                       "Invalid HTTP status code"),
        std::make_pair(HttpResponseException::INVALID_HEADER,
                       "Invalid HTTP header"),
        std::make_pair(HttpResponseException::BODY_TOO_LARGE,
                       "Response body too large"),
        std::make_pair(HttpResponseException::SERIALIZATION_FAILED,
                       "Failed to serialize HTTP response"),
        std::make_pair(HttpResponseException::MISSING_REQUIRED_HEADER,
                       "Missing required HTTP header"),
        std::make_pair(HttpResponseException::DUPLICATE_HEADER,
                       "Duplicate HTTP header"),
        std::make_pair(HttpResponseException::INVALID_CONTENT_LENGTH,
                       "Invalid Content-Length header")};

HttpResponseException::HttpResponseException(const std::string& msg,
                                             ErrorCode code)
    : BaseException("", static_cast<int>(code)) {
  std::ostringstream oss;
  oss << getErrorMsg(code) << ": " << msg;
  this->m_whatMsg = oss.str();
}

HttpResponseException::HttpResponseException(const HttpResponseException& other)
    : BaseException(other) {}

HttpResponseException::~HttpResponseException() throw() {}

HttpResponseException& HttpResponseException::operator=(
    const HttpResponseException& other) {
  if (this != &other) {
    BaseException::operator=(other);
  }
  return *this;
}

std::string HttpResponseException::getErrorMsg(
    HttpResponseException::ErrorCode code) {
  for (int i = 0; i < CODE_COUNT; ++i) {
    if (K_CODE_MSGS[i].first == code) {
      return K_CODE_MSGS[i].second;
    }
  }
  return "Unknown HTTP response error";
}

}  // namespace exceptions
}  // namespace http
}  // namespace domain
