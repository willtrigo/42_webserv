/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TcpSocket.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/03 08:52:55 by dande-je          #+#    #+#             */
/*   Updated: 2026/01/07 00:50:24 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "TcpSocket.hpp"
#include "infrastructure/network/exceptions/SocketException.hpp"

#include <arpa/inet.h>
#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sstream>
#include <sys/socket.h>
#include <unistd.h>

namespace infrastructure {
namespace network {
namespace adapters {

TcpSocket::TcpSocket(const domain::http::value_objects::Host& host,
                     const domain::http::value_objects::Port& port,
                     application::ports::ILogger& logger)
    : m_logger(logger),
      m_fd(K_INVALID_FD),
      m_host(host),
      m_port(port),
      m_isServerSocket(true),
      m_address(NULL) {
  m_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (m_fd == K_INVALID_FD) {
    throw exceptions::SocketException(
        "Failed to create socket",
        exceptions::SocketException::SOCKET_CREATION_FAILED, errno);
  }

  m_address = new sockaddr_in;
  std::memset(m_address, 0, sizeof(sockaddr_in));
  initializeAddress();
}

TcpSocket::TcpSocket(int fileDescriptor, application::ports::ILogger& logger)
    : m_logger(logger),
      m_fd(fileDescriptor),
      m_isServerSocket(false),
      m_address(NULL) {
  if (fileDescriptor < 0) {
    throw exceptions::SocketException(
        "File descriptor must be non-negative",
        exceptions::SocketException::INVALID_FILE_DESCRIPTOR);
  }

  m_address = new sockaddr_in;
  std::memset(m_address, 0, sizeof(sockaddr_in));

  socklen_t addrLen = sizeof(sockaddr_in);
  if (getpeername(m_fd, reinterpret_cast<sockaddr*>(m_address), &addrLen) ==
      K_SOCKET_ERROR) {
    const int savedErrno = errno;
    delete reinterpret_cast<sockaddr_in*>(m_address);
    m_address = NULL;
    throw exceptions::SocketException(
        "Failed to get peer address",
        exceptions::SocketException::GETPEERNAME_FAILED, savedErrno);
  }

  sockaddr_in* addr = reinterpret_cast<sockaddr_in*>(m_address);
  m_port = domain::http::value_objects::Port(ntohs(addr->sin_port));

  char hostBuffer[K_IPV4_ADDR_STRLEN];
  if (inet_ntop(AF_INET, &addr->sin_addr, hostBuffer, K_IPV4_ADDR_STRLEN) !=
      NULL) {
    m_host = domain::http::value_objects::Host(hostBuffer);
  }
}

TcpSocket::~TcpSocket() {
  close();
  delete reinterpret_cast<sockaddr_in*>(m_address);
  m_address = NULL;
}

void TcpSocket::bind() {
  if (!isValid()) {
    throw exceptions::SocketException(
        "Cannot bind invalid socket",
        exceptions::SocketException::INVALID_FILE_DESCRIPTOR);
  }

  const int enableReuseAddr = 1;
  setSocketOption(SOL_SOCKET, SO_REUSEADDR, &enableReuseAddr, sizeof(int));

  sockaddr_in* addr = reinterpret_cast<sockaddr_in*>(m_address);
  if (::bind(m_fd, reinterpret_cast<const sockaddr*>(addr),
             sizeof(sockaddr_in)) == K_SOCKET_ERROR) {
    const exceptions::SocketException::ErrorCode errorCode =
        classifyBindError(errno);

    std::ostringstream oss;
    oss << m_host.getValue() << ":" << m_port.getValue();
    throw exceptions::SocketException(oss.str(), errorCode, errno);
  }

  std::ostringstream oss;
  oss << "Socket bound to " << m_host.getValue() << ":" << m_port.getValue();
  m_logger.debug(oss.str());
}

void TcpSocket::listen(int backlog) {
  if (!isValid()) {
    throw exceptions::SocketException(
        "Cannot listen on invalid socket",
        exceptions::SocketException::INVALID_FILE_DESCRIPTOR);
  }

  if (::listen(m_fd, backlog) == K_SOCKET_ERROR) {
    std::ostringstream oss;
    oss << m_host.getValue() << ":" << m_port.getValue();
    throw exceptions::SocketException(
        oss.str(), exceptions::SocketException::LISTEN_FAILED, errno);
  }

  std::ostringstream oss;
  oss << "Socket listening on " << m_host.getValue() << ":" << m_port.getValue()
      << " (backlog=" << backlog << ")";
  m_logger.debug(oss.str());
}

TcpSocket* TcpSocket::accept() {
  if (!isValid()) {
    throw exceptions::SocketException(
        "Cannot accept on invalid socket",
        exceptions::SocketException::INVALID_FILE_DESCRIPTOR);
  }

  sockaddr_in clientAddr;
  socklen_t clientAddrLen = sizeof(clientAddr);
  std::memset(&clientAddr, 0, sizeof(clientAddr));

  const int clientFd =
      ::accept(m_fd, reinterpret_cast<sockaddr*>(&clientAddr), &clientAddrLen);

  if (clientFd == K_INVALID_FD) {
    if (errno == EAGAIN || errno == EWOULDBLOCK) {
      return NULL;
    }
    throw exceptions::SocketException(
        "Failed to accept connection",
        exceptions::SocketException::ACCEPT_FAILED, errno);
  }

  try {
    TcpSocket* clientSocket = new TcpSocket(clientFd, m_logger);

    std::ostringstream oss;
    oss << "Accepted connection from " << clientSocket->getRemoteAddress()
        << " (FD=" << clientFd << ")";
    m_logger.debug(oss.str());

    return clientSocket;
  } catch (const std::exception& ex) {
    ::close(clientFd);
    throw;
  }
}

void TcpSocket::setNonBlocking(bool nonBlocking) const {
  if (!isValid()) {
    throw exceptions::SocketException(
        "Cannot configure invalid socket",
        exceptions::SocketException::INVALID_FILE_DESCRIPTOR);
  }

  int flags = fcntl(m_fd, F_GETFL, 0);
  if (flags == K_SOCKET_ERROR) {
    throw exceptions::SocketException("Failed to get socket flags",
                                      exceptions::SocketException::FCNTL_FAILED,
                                      errno);
  }

  if (nonBlocking) {
    flags |= O_NONBLOCK;
  } else {
    flags &= ~O_NONBLOCK;
  }

  if (fcntl(m_fd, F_SETFL, flags) == K_SOCKET_ERROR) {
    throw exceptions::SocketException("Failed to set socket flags",
                                      exceptions::SocketException::FCNTL_FAILED,
                                      errno);
  }
}

ssize_t TcpSocket::read(char* buffer, size_t maxBytes) const {
  if (!isValid()) {
    throw exceptions::SocketException(
        "Cannot read from invalid socket",
        exceptions::SocketException::INVALID_FILE_DESCRIPTOR);
  }

  if (buffer == NULL) {
    throw exceptions::SocketException(
        "Buffer pointer cannot be NULL",
        exceptions::SocketException::INVALID_BUFFER);
  }

  if (maxBytes == 0) {
    throw exceptions::SocketException(
        "Read size must be greater than zero",
        exceptions::SocketException::INVALID_SIZE);
  }

  const ssize_t bytesRead = ::recv(m_fd, buffer, maxBytes, 0);

  if (bytesRead == K_SOCKET_ERROR) {
    if (errno == EAGAIN || errno == EWOULDBLOCK) {
      return -1;
    }
    throw exceptions::SocketException("Failed to read from socket",
                                      exceptions::SocketException::READ_FAILED,
                                      errno);
  }

  return bytesRead;
}

ssize_t TcpSocket::write(const char* data, size_t dataSize) {
  if (!isValid()) {
    throw exceptions::SocketException(
        "Cannot write to invalid socket",
        exceptions::SocketException::INVALID_FILE_DESCRIPTOR);
  }

  if (data == NULL) {
    throw exceptions::SocketException(
        "Data pointer cannot be NULL",
        exceptions::SocketException::INVALID_BUFFER);
  }

  if (dataSize == 0) {
    throw exceptions::SocketException(
        "Write size must be greater than zero",
        exceptions::SocketException::INVALID_SIZE);
  }

  const ssize_t bytesWritten = ::send(m_fd, data, dataSize, MSG_NOSIGNAL);

  if (bytesWritten == K_SOCKET_ERROR) {
    if (errno == EAGAIN || errno == EWOULDBLOCK) {
      return -1;
    }
    if (errno == EPIPE) {
      m_logger.debug("Socket write encountered broken pipe");
      throw exceptions::SocketException(
          "Broken pipe detected", exceptions::SocketException::BROKEN_PIPE,
          errno);
    }
    throw exceptions::SocketException("Failed to write to socket",
                                      exceptions::SocketException::WRITE_FAILED,
                                      errno);
  }

  return bytesWritten;
}

int TcpSocket::getFd() const { return m_fd; }

std::string TcpSocket::getLocalAddress() const {
  if (!isValid()) {
    throw exceptions::SocketException(
        "Cannot get address of invalid socket",
        exceptions::SocketException::INVALID_FILE_DESCRIPTOR);
  }

  sockaddr_in addr;
  socklen_t addrLen = sizeof(addr);
  std::memset(&addr, 0, sizeof(addr));

  if (getsockname(m_fd, reinterpret_cast<sockaddr*>(&addr), &addrLen) ==
      K_SOCKET_ERROR) {
    throw exceptions::SocketException(
        "Failed to get local socket address",
        exceptions::SocketException::GETSOCKNAME_FAILED, errno);
  }

  char hostBuffer[K_IPV4_ADDR_STRLEN];
  if (inet_ntop(AF_INET, &addr.sin_addr, hostBuffer, K_IPV4_ADDR_STRLEN) ==
      NULL) {
    throw exceptions::SocketException(
        "Failed to convert address to string",
        exceptions::SocketException::INET_NTOP_FAILED, errno);
  }

  std::ostringstream oss;
  oss << hostBuffer << ":" << ntohs(addr.sin_port);
  return oss.str();
}

std::string TcpSocket::getRemoteAddress() const {
  if (!isValid()) {
    throw exceptions::SocketException(
        "Cannot get address of invalid socket",
        exceptions::SocketException::INVALID_FILE_DESCRIPTOR);
  }

  if (m_isServerSocket) {
    throw exceptions::SocketException(
        "Remote address not available for server sockets",
        exceptions::SocketException::INVALID_FILE_DESCRIPTOR);
  }

  sockaddr_in addr;
  socklen_t addrLen = sizeof(addr);
  std::memset(&addr, 0, sizeof(addr));

  if (getpeername(m_fd, reinterpret_cast<sockaddr*>(&addr), &addrLen) ==
      K_SOCKET_ERROR) {
    throw exceptions::SocketException(
        "Failed to get remote peer address",
        exceptions::SocketException::GETPEERNAME_FAILED, errno);
  }

  char hostBuffer[K_IPV4_ADDR_STRLEN];
  if (inet_ntop(AF_INET, &addr.sin_addr, hostBuffer, K_IPV4_ADDR_STRLEN) ==
      NULL) {
    throw exceptions::SocketException(
        "Failed to convert address to string",
        exceptions::SocketException::INET_NTOP_FAILED, errno);
  }

  std::ostringstream oss;
  oss << hostBuffer << ":" << ntohs(addr.sin_port);
  return oss.str();
}

bool TcpSocket::isValid() const { return m_fd >= 0; }

void TcpSocket::close() {
  if (isValid()) {
    if (::close(m_fd) == K_SOCKET_ERROR) {
      std::ostringstream oss;
      oss << "Failed to close socket FD=" << m_fd;
      m_logger.warn(oss.str());
    }
    m_fd = K_INVALID_FD;
  }
}

void TcpSocket::initializeAddress() {
  sockaddr_in* addr = reinterpret_cast<sockaddr_in*>(m_address);
  addr->sin_family = AF_INET;
  addr->sin_port = htons(static_cast<uint16_t>(m_port.getValue()));

  const std::string hostValue = m_host.getValue();

  if (m_host.isWildcard()) {
    addr->sin_addr.s_addr = INADDR_ANY;
  } else if (m_host.isLocalhost()) {
    addr->sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  } else if (m_host.isIpv4()) {
    if (inet_pton(AF_INET, hostValue.c_str(), &addr->sin_addr) != 1) {
      throw exceptions::SocketException(
          "Failed to parse IPv4 address: " + hostValue,
          exceptions::SocketException::INET_PTON_FAILED, errno);
    }
  } else {
    struct addrinfo hints;
    struct addrinfo* result = NULL;
    std::memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    const int status = getaddrinfo(hostValue.c_str(), NULL, &hints, &result);
    if (status != 0) {
      std::ostringstream oss;
      oss << "Failed to resolve hostname: " << hostValue << " ("
          << gai_strerror(status) << ")";
      throw exceptions::SocketException(
          oss.str(), exceptions::SocketException::GETADDRINFO_FAILED);
    }

    if (result != NULL) {
      std::memcpy(&addr->sin_addr,
                  &(reinterpret_cast<sockaddr_in*>(result->ai_addr))->sin_addr,
                  sizeof(struct in_addr));
      freeaddrinfo(result);
    } else {
      throw exceptions::SocketException(
          "Failed to resolve host: " + hostValue,
          exceptions::SocketException::GETADDRINFO_FAILED);
    }
  }
}

void TcpSocket::setSocketOption(int level, int optname, const void* optval,
                                socklen_t optlen) const {
  if (setsockopt(m_fd, level, optname, optval, optlen) == K_SOCKET_ERROR) {
    throw exceptions::SocketException(
        "Failed to set socket option",
        exceptions::SocketException::SETSOCKOPT_FAILED, errno);
  }
}

exceptions::SocketException::ErrorCode TcpSocket::classifyBindError(
    int errorNumber) {
  switch (errorNumber) {
    case EADDRINUSE:
      return exceptions::SocketException::ADDRESS_IN_USE;
    case EACCES:
      return exceptions::SocketException::PERMISSION_DENIED;
    default:
      return exceptions::SocketException::BIND_FAILED;
  }
}

}  // namespace adapters
}  // namespace network
}  // namespace infrastructure
