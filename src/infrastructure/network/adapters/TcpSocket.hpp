/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TcpSocket.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/03 06:58:42 by dande-je          #+#    #+#             */
/*   Updated: 2026/01/07 00:50:19 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TCPSOCKET_HPP
#define TCPSOCKET_HPP

#include "application/ports/ILogger.hpp"
#include "domain/http/value_objects/Host.hpp"
#include "domain/http/value_objects/Port.hpp"
#include "infrastructure/network/exceptions/SocketException.hpp"

#include <cstdio>
#include <string>
#include <unistd.h>

namespace infrastructure {
namespace network {
namespace adapters {

class TcpSocket {
 public:
  static const int K_INVALID_FD = -1;
  static const int K_SOCKET_ERROR = -1;
  static const int K_DEFAULT_BACKLOG = 128;
  static const size_t K_READ_BUFFER_SIZE = 8192;
  static const size_t K_IPV4_ADDR_STRLEN = 16;

  TcpSocket(const domain::http::value_objects::Host& host,
            const domain::http::value_objects::Port& port,
            application::ports::ILogger& logger);

  TcpSocket(int fileDescriptor, application::ports::ILogger& logger);

  ~TcpSocket();

  void bind();
  void listen(int backlog);

  TcpSocket* accept();

  void setNonBlocking(bool nonBlocking) const;

  ssize_t read(char* buffer, size_t maxBytes) const;
  ssize_t write(const char* data, size_t dataSize);

  int getFd() const;
  std::string getLocalAddress() const;
  std::string getRemoteAddress() const;

  bool isValid() const;

  void close();

 private:
  TcpSocket(const TcpSocket&);
  TcpSocket& operator=(const TcpSocket&);

  void initializeAddress();

  void setSocketOption(int level, int optname, const void* optval,
                       socklen_t optlen) const;

  static exceptions::SocketException::ErrorCode classifyBindError(
      int errorNumber);

  application::ports::ILogger& m_logger;
  int m_fd;
  domain::http::value_objects::Host m_host;
  domain::http::value_objects::Port m_port;
  bool m_isServerSocket;
  void* m_address;
};

}  // namespace adapters
}  // namespace network
}  // namespace infrastructure

#endif  // TCPSOCKET_HPP
