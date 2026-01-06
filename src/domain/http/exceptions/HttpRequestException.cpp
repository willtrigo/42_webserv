/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequestException.cpp                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/04 14:24:40 by dande-je          #+#    #+#             */
/*   Updated: 2026/01/04 14:24:43 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpRequestException.hpp"

#include <sstream>

namespace domain {
namespace http {
namespace exceptions {

const std::pair<HttpRequestException::ErrorCode, std::string>
    HttpRequestException::K_CODE_MSGS[] = {
        std::make_pair(HttpRequestException::INVALID_REQUEST,
                       "Invalid HTTP request"),
        std::make_pair(HttpRequestException::INCOMPLETE_REQUEST,
                       "Incomplete HTTP request"),
        std::make_pair(HttpRequestException::MALFORMED_REQUEST,
                       "Malformed HTTP request"),
        std::make_pair(HttpRequestException::MISSING_REQUIRED_HEADER,
                       "Missing required HTTP header"),
        std::make_pair(HttpRequestException::INVALID_CONTENT_LENGTH,
                       "Invalid Content-Length header"),
        std::make_pair(HttpRequestException::BODY_SIZE_MISMATCH,
                       "Request body size mismatch"),
        std::make_pair(HttpRequestException::UNSUPPORTED_TRANSFER_ENCODING,
                       "Unsupported Transfer-Encoding"),
        std::make_pair(HttpRequestException::INVALID_CHUNK_SIZE,
                       "Invalid chunk size"),
        std::make_pair(HttpRequestException::HEADER_NOT_FOUND,
                       "HTTP header not found"),
        std::make_pair(HttpRequestException::DUPLICATE_HEADER,
                       "Duplicate HTTP header"),
        std::make_pair(HttpRequestException::INVALID_URI,
                       "Invalid request URI"),
        std::make_pair(HttpRequestException::INVALID_METHOD,
                       "Invalid HTTP method"),
        std::make_pair(HttpRequestException::INVALID_VERSION,
                       "Invalid HTTP version")};

HttpRequestException::HttpRequestException(const std::string& msg,
                                           ErrorCode code)
    : BaseException("", static_cast<int>(code)) {
  std::ostringstream oss;
  oss << getErrorMsg(code) << ": " << msg;
  this->m_whatMsg = oss.str();
}

HttpRequestException::HttpRequestException(const HttpRequestException& other)
    : BaseException(other) {}

HttpRequestException::~HttpRequestException() throw() {}

HttpRequestException& HttpRequestException::operator=(
    const HttpRequestException& other) {
  if (this != &other) {
    BaseException::operator=(other);
  }
  return *this;
}

std::string HttpRequestException::getErrorMsg(
    HttpRequestException::ErrorCode code) {
  for (int i = 0; i < CODE_COUNT; ++i) {
    if (K_CODE_MSGS[i].first == code) {
      return K_CODE_MSGS[i].second;
    }
  }
  return "Unknown HTTP request error";
}

}  // namespace exceptions
}  // namespace http
}  // namespace domain
