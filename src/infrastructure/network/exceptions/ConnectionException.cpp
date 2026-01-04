/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConnectionException.cpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/04 01:50:44 by dande-je          #+#    #+#             */
/*   Updated: 2026/01/04 01:51:02 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConnectionException.hpp"

#include <cerrno>
#include <cstring>
#include <sstream>

namespace infrastructure {
namespace network {
namespace exceptions {

const std::pair<ConnectionException::ErrorCode, std::string>
    ConnectionException::K_CODE_MSGS[] = {
        std::make_pair(ConnectionException::INVALID_STATE,
                       "Invalid connection state"),
        std::make_pair(ConnectionException::BUFFER_OVERFLOW,
                       "Buffer overflow detected"),
        std::make_pair(ConnectionException::MALFORMED_REQUEST,
                       "Malformed HTTP request"),
        std::make_pair(ConnectionException::REQUEST_TIMEOUT,
                       "Request timeout exceeded"),
        std::make_pair(ConnectionException::REQUEST_TOO_LARGE,
                       "Request entity too large"),
        std::make_pair(ConnectionException::RESPONSE_GENERATION_FAILED,
                       "Failed to generate HTTP response"),
        std::make_pair(ConnectionException::READ_ERROR,
                       "Failed to read from connection"),
        std::make_pair(ConnectionException::WRITE_ERROR,
                       "Failed to write to connection"),
        std::make_pair(ConnectionException::PROTOCOL_VIOLATION,
                       "HTTP protocol violation"),
        std::make_pair(ConnectionException::HEADER_PARSING_FAILED,
                       "Failed to parse HTTP headers"),
        std::make_pair(ConnectionException::BODY_PARSING_FAILED,
                       "Failed to parse HTTP body"),
        std::make_pair(ConnectionException::INCOMPLETE_REQUEST,
                       "Incomplete HTTP request"),
        std::make_pair(ConnectionException::INCOMPLETE_RESPONSE,
                       "Incomplete HTTP response"),
        std::make_pair(ConnectionException::INVALID_HTTP_VERSION,
                       "Invalid HTTP version"),
        std::make_pair(ConnectionException::INVALID_METHOD,
                       "Invalid HTTP method"),
        std::make_pair(ConnectionException::INVALID_URI, "Invalid request URI"),
        std::make_pair(ConnectionException::INVALID_HEADER,
                       "Invalid HTTP header"),
        std::make_pair(ConnectionException::CONTENT_LENGTH_MISMATCH,
                       "Content-Length mismatch"),
        std::make_pair(ConnectionException::CHUNK_ENCODING_ERROR,
                       "Chunked transfer encoding error"),
        std::make_pair(ConnectionException::KEEPALIVE_TIMEOUT,
                       "Keep-alive timeout exceeded"),
        std::make_pair(ConnectionException::PIPELINE_ERROR,
                       "HTTP pipeline error")};

ConnectionException::ConnectionException(const std::string& msg, ErrorCode code)
    : BaseException("", static_cast<int>(code)), m_systemError(0) {
  std::ostringstream oss;
  oss << getErrorMsg(code) << ": " << msg;
  this->m_whatMsg = oss.str();
}

ConnectionException::ConnectionException(const std::string& msg, ErrorCode code,
                                         int systemError)
    : BaseException("", static_cast<int>(code)), m_systemError(systemError) {
  std::ostringstream oss;
  oss << getErrorMsg(code) << ": " << msg;
  if (systemError != 0) {
    oss << " (" << formatSystemError(systemError) << ")";
  }
  this->m_whatMsg = oss.str();
}

ConnectionException::ConnectionException(const ConnectionException& other)
    : BaseException(other), m_systemError(other.m_systemError) {}

ConnectionException::~ConnectionException() throw() {}

ConnectionException& ConnectionException::operator=(
    const ConnectionException& other) {
  if (this != &other) {
    BaseException::operator=(other);
    m_systemError = other.m_systemError;
  }
  return *this;
}

int ConnectionException::getSystemError() const { return m_systemError; }

std::string ConnectionException::getErrorMsg(
    ConnectionException::ErrorCode code) {
  for (int i = 0; i < CODE_COUNT; ++i) {
    if (K_CODE_MSGS[i].first == code) {
      return K_CODE_MSGS[i].second;
    }
  }
  return "Unknown connection error";
}

std::string ConnectionException::formatSystemError(int systemError) {
  std::ostringstream oss;
  oss << std::strerror(systemError) << " [errno=" << systemError << "]";
  return oss.str();
}

}  // namespace exceptions
}  // namespace network
}  // namespace infrastructure
