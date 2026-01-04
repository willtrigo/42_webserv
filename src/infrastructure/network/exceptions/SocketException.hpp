/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SocketException.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/03 23:38:20 by dande-je          #+#    #+#             */
/*   Updated: 2026/01/04 00:09:02 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SOCKET_EXCEPTION_HPP
#define SOCKET_EXCEPTION_HPP

#include "shared/exceptions/BaseException.hpp"

namespace infrastructure {
namespace network {
namespace exceptions {

class SocketException : public ::shared::exceptions::BaseException {
 public:
  enum ErrorCode {
    SOCKET_CREATION_FAILED,
    BIND_FAILED,
    LISTEN_FAILED,
    ACCEPT_FAILED,
    CONNECT_FAILED,
    READ_FAILED,
    WRITE_FAILED,
    CLOSE_FAILED,
    SETSOCKOPT_FAILED,
    FCNTL_FAILED,
    GETSOCKNAME_FAILED,
    GETPEERNAME_FAILED,
    INET_NTOP_FAILED,
    INET_PTON_FAILED,
    GETADDRINFO_FAILED,
    INVALID_FILE_DESCRIPTOR,
    INVALID_PORT,
    INVALID_HOST,
    INVALID_BUFFER,
    INVALID_SIZE,
    ADDRESS_IN_USE,
    PERMISSION_DENIED,
    BROKEN_PIPE,
    CONNECTION_RESET,
    TIMEOUT,
    CODE_COUNT
  };

  explicit SocketException(const std::string& msg, ErrorCode code);
  explicit SocketException(const std::string& msg, ErrorCode code,
                           int systemError);
  SocketException(const SocketException& other);
  virtual ~SocketException() throw();

  SocketException& operator=(const SocketException& other);

  int getSystemError() const;

 private:
  static const std::pair<ErrorCode, std::string> K_CODE_MSGS[];

  int m_systemError;

  static std::string getErrorMsg(ErrorCode code);
  static std::string formatSystemError(int systemError);
};

}  // namespace exceptions
}  // namespace network
}  // namespace infrastructure

#endif  // SOCKET_EXCEPTION_HPP
