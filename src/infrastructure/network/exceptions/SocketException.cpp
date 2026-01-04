/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SocketException.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/03 23:38:53 by dande-je          #+#    #+#             */
/*   Updated: 2026/01/03 23:38:57 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "SocketException.hpp"

#include <cerrno>
#include <cstring>
#include <sstream>

namespace infrastructure {
namespace network {
namespace exceptions {

const std::pair<SocketException::ErrorCode, std::string>
    SocketException::K_CODE_MSGS[] = {
        std::make_pair(SocketException::SOCKET_CREATION_FAILED,
                       "Failed to create socket"),
        std::make_pair(SocketException::BIND_FAILED,
                       "Failed to bind socket to address"),
        std::make_pair(SocketException::LISTEN_FAILED,
                       "Failed to listen on socket"),
        std::make_pair(SocketException::ACCEPT_FAILED,
                       "Failed to accept connection"),
        std::make_pair(SocketException::CONNECT_FAILED,
                       "Failed to connect socket"),
        std::make_pair(SocketException::READ_FAILED,
                       "Failed to read from socket"),
        std::make_pair(SocketException::WRITE_FAILED,
                       "Failed to write to socket"),
        std::make_pair(SocketException::CLOSE_FAILED, "Failed to close socket"),
        std::make_pair(SocketException::SETSOCKOPT_FAILED,
                       "Failed to set socket option"),
        std::make_pair(SocketException::FCNTL_FAILED,
                       "Failed to configure socket flags"),
        std::make_pair(SocketException::GETSOCKNAME_FAILED,
                       "Failed to get local socket address"),
        std::make_pair(SocketException::GETPEERNAME_FAILED,
                       "Failed to get remote peer address"),
        std::make_pair(SocketException::INET_NTOP_FAILED,
                       "Failed to convert network address to string"),
        std::make_pair(SocketException::INET_PTON_FAILED,
                       "Failed to convert string to network address"),
        std::make_pair(SocketException::GETADDRINFO_FAILED,
                       "Failed to resolve host address"),
        std::make_pair(SocketException::INVALID_FILE_DESCRIPTOR,
                       "Invalid file descriptor"),
        std::make_pair(SocketException::INVALID_PORT, "Invalid port number"),
        std::make_pair(SocketException::INVALID_HOST, "Invalid host address"),
        std::make_pair(SocketException::INVALID_BUFFER,
                       "Invalid buffer pointer"),
        std::make_pair(SocketException::INVALID_SIZE, "Invalid size parameter"),
        std::make_pair(SocketException::ADDRESS_IN_USE,
                       "Address already in use"),
        std::make_pair(SocketException::PERMISSION_DENIED, "Permission denied"),
        std::make_pair(SocketException::BROKEN_PIPE, "Broken pipe"),
        std::make_pair(SocketException::CONNECTION_RESET,
                       "Connection reset by peer"),
        std::make_pair(SocketException::TIMEOUT, "Operation timed out")};

SocketException::SocketException(const std::string& msg, ErrorCode code)
    : BaseException("", static_cast<int>(code)), m_systemError(0) {
  std::ostringstream oss;
  oss << getErrorMsg(code) << ": " << msg;
  this->m_whatMsg = oss.str();
}

SocketException::SocketException(const std::string& msg, ErrorCode code,
                                 int systemError)
    : BaseException("", static_cast<int>(code)), m_systemError(systemError) {
  std::ostringstream oss;
  oss << getErrorMsg(code) << ": " << msg;
  if (systemError != 0) {
    oss << " (" << formatSystemError(systemError) << ")";
  }
  this->m_whatMsg = oss.str();
}

SocketException::SocketException(const SocketException& other)
    : BaseException(other), m_systemError(other.m_systemError) {}

SocketException::~SocketException() throw() {}

SocketException& SocketException::operator=(const SocketException& other) {
  if (this != &other) {
    BaseException::operator=(other);
    m_systemError = other.m_systemError;
  }
  return *this;
}

int SocketException::getSystemError() const { return m_systemError; }

std::string SocketException::getErrorMsg(SocketException::ErrorCode code) {
  for (int i = 0; i < CODE_COUNT; ++i) {
    if (K_CODE_MSGS[i].first == code) {
      return K_CODE_MSGS[i].second;
    }
  }
  return "Unknown socket error";
}

std::string SocketException::formatSystemError(int systemError) {
  std::ostringstream oss;
  oss << std::strerror(systemError) << " [errno=" << systemError << "]";
  return oss.str();
}

}  // namespace exceptions
}  // namespace network
}  // namespace infrastructure
