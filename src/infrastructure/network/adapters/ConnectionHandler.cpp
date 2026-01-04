/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConnectionHandler.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/03 12:01:14 by dande-je          #+#    #+#             */
/*   Updated: 2026/01/04 01:10:53 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "infrastructure/network/adapters/ConnectionHandler.hpp"
#include "infrastructure/network/adapters/TcpSocket.hpp"

#include <ctime>
#include <sstream>
#include <stdexcept>

namespace infrastructure {
namespace network {
namespace adapters {

ConnectionHandler::ConnectionHandler(
    TcpSocket* socket,
    const domain::configuration::entities::ServerConfig* serverConfig,
    application::ports::ILogger& logger,
    application::ports::IConfigProvider& configProvider)
    : m_logger(logger),
      m_configProvider(configProvider),
      m_socket(socket),
      m_serverConfig(serverConfig),
      m_state(STATE_READING_REQUEST),
      m_lastActivityTime(std::time(NULL)),
      m_responseOffset(0) {
  if (socket == NULL) {
    throw std::invalid_argument("ConnectionHandler requires non-NULL socket");
  }

  if (serverConfig == NULL) {
    throw std::invalid_argument(
        "ConnectionHandler requires non-NULL serverConfig");
  }

  m_requestBuffer.reserve(K_READ_BUFFER_SIZE);

  std::ostringstream oss;
  oss << "ConnectionHandler created for " << getRemoteAddress();
  m_logger.debug(oss.str());
}

ConnectionHandler::~ConnectionHandler() {
  std::ostringstream oss;
  oss << "ConnectionHandler destroyed for " << getRemoteAddress();
  m_logger.debug(oss.str());

  delete m_socket;
  m_socket = NULL;
}

void ConnectionHandler::processEvent() {
  updateLastActivity(std::time(NULL));

  try {
    switch (m_state) {
      case STATE_READING_REQUEST:
        handleRead();
        break;

      case STATE_PROCESSING:
        processRequest();
        m_state = STATE_WRITING_RESPONSE;
        break;

      case STATE_WRITING_RESPONSE:
        handleWrite();
        break;

      case STATE_KEEP_ALIVE:
        // Transition back to reading if data available
        handleRead();
        break;

      case STATE_CLOSING:
        // No-op; awaiting closure from orchestrator
        break;
    }
  } catch (const std::exception& ex) {
    m_logger.error(std::string("ConnectionHandler::processEvent() failed: ") +
                   ex.what());
    generateErrorResponse(500, "Internal Server Error");
    m_state = STATE_CLOSING;
  }
}

bool ConnectionHandler::shouldClose() const {
  return m_state == STATE_CLOSING && m_responseBuffer.empty();
}

ConnectionHandler::State ConnectionHandler::getState() const { return m_state; }

int ConnectionHandler::getFd() const {
  return m_socket ? m_socket->getFd() : -1;
}

std::string ConnectionHandler::getRemoteAddress() const {
  return m_socket ? m_socket->getRemoteAddress() : "unknown";
}

bool ConnectionHandler::isTimedOut(time_t currentTime) const {
  return (currentTime - m_lastActivityTime) > K_CONNECTION_TIMEOUT;
}

void ConnectionHandler::updateLastActivity(time_t currentTime) {
  m_lastActivityTime = currentTime;
}

void ConnectionHandler::handleRead() {
  char buffer[K_READ_BUFFER_SIZE];
  const ssize_t bytesRead = m_socket->read(buffer, K_READ_BUFFER_SIZE);

  if (bytesRead == 0) {
    // Client closed connection
    m_logger.debug("Client closed connection: " + getRemoteAddress());
    m_state = STATE_CLOSING;
    return;
  }

  if (bytesRead == -1) {
    // EAGAIN/EWOULDBLOCK - no data available
    return;
  }

  m_requestBuffer.append(buffer, static_cast<size_t>(bytesRead));

  std::ostringstream oss;
  oss << "Read " << bytesRead << " bytes from " << getRemoteAddress()
      << " (total: " << m_requestBuffer.size() << ")";
  m_logger.debug(oss.str());

  // Attempt to parse complete request
  if (parseRequest()) {
    m_state = STATE_PROCESSING;
  }

  // Check request size limit
  const size_t maxBodySize =
      m_serverConfig->getClientMaxBodySize();  // Assuming this method exists
  if (m_requestBuffer.size() > maxBodySize) {
    generateErrorResponse(413, "Request Entity Too Large");
    m_state = STATE_CLOSING;
  }
}

void ConnectionHandler::handleWrite() {
  if (m_responseBuffer.empty()) {
    // Response fully transmitted
    if (shouldKeepAlive()) {
      m_logger.debug("Keeping connection alive: " + getRemoteAddress());
      m_requestBuffer.clear();
      m_responseBuffer.clear();
      m_responseOffset = 0;
      m_state = STATE_KEEP_ALIVE;
    } else {
      m_logger.debug("Closing connection: " + getRemoteAddress());
      m_state = STATE_CLOSING;
    }
    return;
  }

  const size_t remaining = m_responseBuffer.size() - m_responseOffset;
  const ssize_t bytesWritten =
      m_socket->write(m_responseBuffer.c_str() + m_responseOffset, remaining);

  if (bytesWritten == -1) {
    // EAGAIN/EWOULDBLOCK - socket not ready
    return;
  }

  m_responseOffset += static_cast<size_t>(bytesWritten);

  std::ostringstream oss;
  oss << "Wrote " << bytesWritten << " bytes to " << getRemoteAddress() << " ("
      << m_responseOffset << "/" << m_responseBuffer.size() << ")";
  m_logger.debug(oss.str());

  if (m_responseOffset >= m_responseBuffer.size()) {
    m_responseBuffer.clear();
    m_responseOffset = 0;
  }
}

bool ConnectionHandler::parseRequest() {
  // Simple HTTP request line parser (to be replaced with proper RequestParser)
  const size_t headerEndPos = m_requestBuffer.find("\r\n\r\n");
  if (headerEndPos == std::string::npos) {
    // Incomplete headers
    return false;
  }

  // Extract request line
  const size_t requestLineEnd = m_requestBuffer.find("\r\n");
  if (requestLineEnd == std::string::npos) {
    throw std::runtime_error("Malformed request: No request line");
  }

  const std::string requestLine = m_requestBuffer.substr(0, requestLineEnd);

  // Parse: METHOD URI HTTP/VERSION
  std::istringstream iss(requestLine);
  if (!(iss >> m_requestMethod >> m_requestUri >> m_requestProtocol)) {
    throw std::runtime_error("Malformed request line");
  }

  std::ostringstream oss;
  oss << "Parsed request: " << m_requestMethod << " " << m_requestUri << " "
      << m_requestProtocol;
  m_logger.info(oss.str());

  // TODO: Parse headers, check Content-Length, handle request body
  // For now, assume request complete after headers

  return true;
}

void ConnectionHandler::processRequest() {
  try {
    // Virtual host resolution
    const domain::configuration::entities::ServerConfig* config =
        resolveVirtualHost();

    // TODO: Integrate with proper request routing/handling pipeline
    // 1. Match LocationConfig using RouteMatcher
    // 2. Check HTTP method permissions
    // 3. Resolve file path or CGI handler
    // 4. Execute handler (FileHandler, DirectoryLister, CGI processor)
    // 5. Generate response

    // Placeholder: Generate simple 200 OK response
    std::ostringstream response;
    response << "HTTP/1.1 200 OK\r\n";
    response << "Content-Type: text/html\r\n";
    response << "Connection: " << (shouldKeepAlive() ? "keep-alive" : "close")
             << "\r\n";

    const std::string body =
        "<html><body><h1>42 WebServ</h1><p>Request "
        "received successfully</p></body></html>";

    response << "Content-Length: " << body.size() << "\r\n";
    response << "\r\n";
    response << body;

    m_responseBuffer = response.str();

    logRequest(m_requestMethod, m_requestUri, 200);

  } catch (const std::exception& ex) {
    m_logger.error(std::string("Request processing failed: ") + ex.what());
    generateErrorResponse(500, "Internal Server Error");
    logRequest(m_requestMethod, m_requestUri, 500);
  }
}

const domain::configuration::entities::ServerConfig*
ConnectionHandler::resolveVirtualHost() {
  // TODO: Parse Host header and match against server_names
  // For now, return configured server
  return m_serverConfig;
}

void ConnectionHandler::generateErrorResponse(int statusCode,
                                              const std::string& message) {
  std::ostringstream response;
  response << "HTTP/1.1 " << statusCode << " " << message << "\r\n";
  response << "Content-Type: text/html\r\n";
  response << "Connection: close\r\n";

  std::ostringstream body;
  body << "<html><body><h1>" << statusCode << " " << message
       << "</h1></body></html>";

  response << "Content-Length: " << body.str().size() << "\r\n";
  response << "\r\n";
  response << body.str();

  m_responseBuffer = response.str();
  m_responseOffset = 0;

  logRequest(m_requestMethod, m_requestUri, statusCode);
}

bool ConnectionHandler::shouldKeepAlive() const {
  // HTTP/1.1 defaults to keep-alive unless explicitly closed
  if (m_requestProtocol == "HTTP/1.1") {
    // TODO: Check Connection header for "close"
    return true;
  }

  // HTTP/1.0 requires explicit Connection: keep-alive
  // TODO: Parse Connection header
  return false;
}

std::string ConnectionHandler::formatState() const {
  switch (m_state) {
    case STATE_READING_REQUEST:
      return "READING_REQUEST";
    case STATE_PROCESSING:
      return "PROCESSING";
    case STATE_WRITING_RESPONSE:
      return "WRITING_RESPONSE";
    case STATE_KEEP_ALIVE:
      return "KEEP_ALIVE";
    case STATE_CLOSING:
      return "CLOSING";
    default:
      return "UNKNOWN";
  }
}

void ConnectionHandler::logRequest(const std::string& method,
                                   const std::string& uri, int statusCode) {
  std::ostringstream oss;
  oss << getRemoteAddress() << " - \"" << method << " " << uri << "\" "
      << statusCode;
  m_logger.info(oss.str());
}

}  // namespace adapters
}  // namespace network
}  // namespace infrastructure
