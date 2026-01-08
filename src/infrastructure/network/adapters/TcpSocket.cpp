/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TcpSocket.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/03 08:52:55 by dande-je          #+#    #+#             */
/*   Updated: 2026/01/08 04:11:49 by dande-je         ###   ########.fr       */
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
  const int addressFamily = m_host.isIpv6() ? AF_INET6 : AF_INET;

  m_fd = socket(addressFamily, SOCK_STREAM, 0);
  if (m_fd == K_INVALID_FD) {
    throw exceptions::SocketException(
        "Failed to create socket",
        exceptions::SocketException::SOCKET_CREATION_FAILED, errno);
  }

  m_address = new sockaddr_storage;
  std::memset(m_address, 0, sizeof(sockaddr_storage));
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

  m_address = new sockaddr_storage;
  std::memset(m_address, 0, sizeof(sockaddr_storage));

  socklen_t addrLen = sizeof(sockaddr_storage);
  if (getpeername(m_fd, reinterpret_cast<sockaddr*>(m_address), &addrLen) ==
      K_SOCKET_ERROR) {
    const int savedErrno = errno;
    delete reinterpret_cast<sockaddr_storage*>(m_address);
    m_address = NULL;
    throw exceptions::SocketException(
        "Failed to get peer address",
        exceptions::SocketException::GETPEERNAME_FAILED, savedErrno);
  }

  sockaddr_storage* addr = reinterpret_cast<sockaddr_storage*>(m_address);

  if (addr->ss_family == AF_INET) {
    sockaddr_in* addr4 = reinterpret_cast<sockaddr_in*>(addr);
    m_port = domain::http::value_objects::Port(ntohs(addr4->sin_port));

    char hostBuffer[K_IPV4_ADDR_STRLEN];
    if (inet_ntop(AF_INET, &addr4->sin_addr, hostBuffer, K_IPV4_ADDR_STRLEN) !=
        NULL) {
      m_host = domain::http::value_objects::Host(hostBuffer);
    }
  } else if (addr->ss_family == AF_INET6) {
    sockaddr_in6* addr6 = reinterpret_cast<sockaddr_in6*>(addr);
    m_port = domain::http::value_objects::Port(ntohs(addr6->sin6_port));

    char hostBuffer[K_IPV6_ADDR_STRLEN];
    if (inet_ntop(AF_INET6, &addr6->sin6_addr, hostBuffer,
                  K_IPV6_ADDR_STRLEN) != NULL) {
      m_host = domain::http::value_objects::Host(hostBuffer);
    }
  }
}

TcpSocket::~TcpSocket() {
  close();
  delete reinterpret_cast<sockaddr_storage*>(m_address);
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

  if (m_host.isIpv6()) {
    const int ipv6Only = 1;
    if (setsockopt(m_fd, IPPROTO_IPV6, IPV6_V6ONLY, &ipv6Only,
                   sizeof(ipv6Only)) == K_SOCKET_ERROR) {
      std::ostringstream oss;
      oss << "Warning: Failed to set IPV6_V6ONLY for " << m_host.getValue()
          << ":" << m_port.getValue() << " (errno=" << errno << ")";
      m_logger.warn(oss.str());
    }
  }

  sockaddr_storage* addr = reinterpret_cast<sockaddr_storage*>(m_address);
  socklen_t addrLen;

  if (m_host.isIpv6()) {
    addrLen = sizeof(sockaddr_in6);
  } else {
    addrLen = sizeof(sockaddr_in);
  }

  if (::bind(m_fd, reinterpret_cast<const sockaddr*>(addr), addrLen) ==
      K_SOCKET_ERROR) {
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

  sockaddr_storage clientAddr;
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

  sockaddr_storage addr;
  socklen_t addrLen = sizeof(addr);
  std::memset(&addr, 0, sizeof(addr));

  if (getsockname(m_fd, reinterpret_cast<sockaddr*>(&addr), &addrLen) ==
      K_SOCKET_ERROR) {
    throw exceptions::SocketException(
        "Failed to get local socket address",
        exceptions::SocketException::GETSOCKNAME_FAILED, errno);
  }

  return formatAddress(&addr);
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

  sockaddr_storage addr;
  socklen_t addrLen = sizeof(addr);
  std::memset(&addr, 0, sizeof(addr));

  if (getpeername(m_fd, reinterpret_cast<sockaddr*>(&addr), &addrLen) ==
      K_SOCKET_ERROR) {
    throw exceptions::SocketException(
        "Failed to get remote peer address",
        exceptions::SocketException::GETPEERNAME_FAILED, errno);
  }

  return formatAddress(&addr);
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
  sockaddr_storage* addr = reinterpret_cast<sockaddr_storage*>(m_address);
  const std::string hostValue = m_host.getValue();

  if (m_host.isIpv6()) {
    sockaddr_in6* addr6 = reinterpret_cast<sockaddr_in6*>(addr);
    addr6->sin6_family = AF_INET6;
    addr6->sin6_port = htons(static_cast<uint16_t>(m_port.getValue()));

    if (m_host.isWildcard()) {
      addr6->sin6_addr = in6addr_any;
    } else if (m_host.isLocalhost()) {
      addr6->sin6_addr = in6addr_loopback;
    } else {
      if (inet_pton(AF_INET6, hostValue.c_str(), &addr6->sin6_addr) != 1) {
        throw exceptions::SocketException(
            "Failed to parse IPv6 address: " + hostValue,
            exceptions::SocketException::INET_PTON_FAILED, errno);
      }
    }
  } else {
    sockaddr_in* addr4 = reinterpret_cast<sockaddr_in*>(addr);
    addr4->sin_family = AF_INET;
    addr4->sin_port = htons(static_cast<uint16_t>(m_port.getValue()));

    if (m_host.isWildcard()) {
      addr4->sin_addr.s_addr = INADDR_ANY;
    } else if (m_host.isLocalhost()) {
      addr4->sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    } else if (m_host.isIpv4()) {
      if (inet_pton(AF_INET, hostValue.c_str(), &addr4->sin_addr) != 1) {
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
        std::memcpy(
            &addr4->sin_addr,
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
}

void TcpSocket::setSocketOption(int level, int optname, const void* optval,
                                socklen_t optlen) const {
  if (setsockopt(m_fd, level, optname, optval, optlen) == K_SOCKET_ERROR) {
    throw exceptions::SocketException(
        "Failed to set socket option",
        exceptions::SocketException::SETSOCKOPT_FAILED, errno);
  }
}

std::string TcpSocket::formatAddress(const sockaddr_storage* addr) {
  std::ostringstream oss;

  if (addr->ss_family == AF_INET) {
    const sockaddr_in* addr4 = reinterpret_cast<const sockaddr_in*>(addr);
    char hostBuffer[K_IPV4_ADDR_STRLEN];

    if (inet_ntop(AF_INET, &addr4->sin_addr, hostBuffer, K_IPV4_ADDR_STRLEN) ==
        NULL) {
      throw exceptions::SocketException(
          "Failed to convert IPv4 address to string",
          exceptions::SocketException::INET_NTOP_FAILED, errno);
    }

    oss << hostBuffer << ":" << ntohs(addr4->sin_port);
  } else if (addr->ss_family == AF_INET6) {
    const sockaddr_in6* addr6 = reinterpret_cast<const sockaddr_in6*>(addr);
    char hostBuffer[K_IPV6_ADDR_STRLEN];

    if (inet_ntop(AF_INET6, &addr6->sin6_addr, hostBuffer,
                  K_IPV6_ADDR_STRLEN) == NULL) {
      throw exceptions::SocketException(
          "Failed to convert IPv6 address to string",
          exceptions::SocketException::INET_NTOP_FAILED, errno);
    }

    oss << "[" << hostBuffer << "]:" << ntohs(addr6->sin6_port);
  }

  return oss.str();
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
