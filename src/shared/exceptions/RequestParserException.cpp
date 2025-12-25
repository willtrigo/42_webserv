/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParserException.cpp                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/24 20:51:11 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/24 20:57:22 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shared/exceptions/RequestParserException.hpp"

#include <sstream>

namespace shared {
namespace exceptions {

const std::pair<RequestParserException::ErrorCode, std::string>
    RequestParserException::K_CODE_MSGS[] = {
        std::make_pair(RequestParserException::MALFORMED_REQUEST,
                       "Malformed HTTP request"),
        std::make_pair(RequestParserException::UNSUPPORTED_METHOD,
                       "Unsupported HTTP method"),
        std::make_pair(RequestParserException::INVALID_HEADER,
                       "Invalid HTTP header"),
        std::make_pair(RequestParserException::INVALID_BODY,
                       "Invalid HTTP request body"),
        std::make_pair(RequestParserException::CHUNKED_ENCODING_ERROR,
                       "Chunked encoding error"),
        std::make_pair(RequestParserException::CONTENT_LENGTH_MISMATCH,
                       "Content-Length mismatch"),
        std::make_pair(RequestParserException::URI_TOO_LONG, "URI too long"),
        std::make_pair(RequestParserException::HEADER_TOO_LARGE,
                       "HTTP headers too large"),
        std::make_pair(RequestParserException::BODY_TOO_LARGE,
                       "Request body too large"),
        std::make_pair(RequestParserException::INVALID_HTTP_VERSION,
                       "Invalid or unsupported HTTP version"),
        std::make_pair(RequestParserException::TIMEOUT,
                       "Request parsing timeout")};

RequestParserException::RequestParserException(const std::string& msg,
                                               ErrorCode code)
    : BaseException("", static_cast<int>(code)) {
  std::ostringstream oss;
  oss << getErrorMsg(code) << ": " << msg;
  this->m_whatMsg = oss.str();
}

RequestParserException::RequestParserException(
    const RequestParserException& other)
    : BaseException(other) {}

RequestParserException::~RequestParserException() throw() {}

RequestParserException& RequestParserException::operator=(
    const RequestParserException& other) {
  if (this != &other) {
    BaseException::operator=(other);
  }
  return *this;
}

std::string RequestParserException::getErrorMsg(ErrorCode code) {
  for (int i = 0; i < CODE_COUNT; ++i) {
    if (K_CODE_MSGS[i].first == code) {
      return K_CODE_MSGS[i].second;
    }
  }
  return "unknown request parser error";
}

}  // namespace exceptions
}  // namespace shared
