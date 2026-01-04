/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MultiplexerException.cpp                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/04 00:09:28 by dande-je          #+#    #+#             */
/*   Updated: 2026/01/04 00:19:40 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "MultiplexerException.hpp"

#include <cerrno>
#include <cstring>
#include <sstream>

namespace infrastructure {
namespace network {
namespace exceptions {

const std::pair<MultiplexerException::ErrorCode, std::string>
    MultiplexerException::K_CODE_MSGS[] = {
        std::make_pair(MultiplexerException::EPOLL_CREATE_FAILED,
                       "Failed to create epoll instance"),
        std::make_pair(MultiplexerException::EPOLL_CTL_ADD_FAILED,
                       "Failed to add file descriptor to epoll"),
        std::make_pair(MultiplexerException::EPOLL_CTL_MOD_FAILED,
                       "Failed to modify epoll registration"),
        std::make_pair(MultiplexerException::EPOLL_CTL_DEL_FAILED,
                       "Failed to delete file descriptor from epoll"),
        std::make_pair(MultiplexerException::EPOLL_WAIT_FAILED,
                       "Failed to wait for epoll events"),
        std::make_pair(MultiplexerException::INVALID_FILE_DESCRIPTOR,
                       "Invalid file descriptor"),
        std::make_pair(MultiplexerException::ALREADY_REGISTERED,
                       "File descriptor already registered"),
        std::make_pair(MultiplexerException::NOT_REGISTERED,
                       "File descriptor not registered"),
        std::make_pair(MultiplexerException::INVALID_EVENT_MASK,
                       "Invalid event mask"),
        std::make_pair(MultiplexerException::REGISTRATION_LIMIT_EXCEEDED,
                       "Maximum registration limit exceeded"),
        std::make_pair(MultiplexerException::TIMEOUT, "Operation timed out")};

MultiplexerException::MultiplexerException(const std::string& msg,
                                           ErrorCode code)
    : BaseException("", static_cast<int>(code)), m_systemError(0) {
  std::ostringstream oss;
  oss << getErrorMsg(code) << ": " << msg;
  this->m_whatMsg = oss.str();
}

MultiplexerException::MultiplexerException(const std::string& msg,
                                           ErrorCode code, int systemError)
    : BaseException("", static_cast<int>(code)), m_systemError(systemError) {
  std::ostringstream oss;
  oss << getErrorMsg(code) << ": " << msg;
  if (systemError != 0) {
    oss << " (" << formatSystemError(systemError) << ")";
  }
  this->m_whatMsg = oss.str();
}

MultiplexerException::MultiplexerException(const MultiplexerException& other)
    : BaseException(other), m_systemError(other.m_systemError) {}

MultiplexerException::~MultiplexerException() throw() {}

MultiplexerException& MultiplexerException::operator=(
    const MultiplexerException& other) {
  if (this != &other) {
    BaseException::operator=(other);
    m_systemError = other.m_systemError;
  }
  return *this;
}

int MultiplexerException::getSystemError() const { return m_systemError; }

std::string MultiplexerException::getErrorMsg(
    MultiplexerException::ErrorCode code) {
  for (int i = 0; i < CODE_COUNT; ++i) {
    if (K_CODE_MSGS[i].first == code) {
      return K_CODE_MSGS[i].second;
    }
  }
  return "Unknown multiplexer error";
}

std::string MultiplexerException::formatSystemError(int systemError) {
  std::ostringstream oss;
  oss << std::strerror(systemError) << " [errno=" << systemError << "]";
  return oss.str();
}

}  // namespace exceptions
}  // namespace network
}  // namespace infrastructure
