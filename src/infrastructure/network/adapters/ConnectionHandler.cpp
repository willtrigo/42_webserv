/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConnectionHandler.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/03 12:01:14 by dande-je          #+#    #+#             */
/*   Updated: 2026/01/11 00:36:15 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "domain/configuration/entities/LocationConfig.hpp"
#include "domain/configuration/exceptions/LocationConfigException.hpp"
#include "domain/configuration/value_objects/CgiConfig.hpp"
#include "domain/configuration/value_objects/Route.hpp"
#include "domain/configuration/value_objects/UploadConfig.hpp"
#include "domain/filesystem/value_objects/Path.hpp"
#include "domain/http/entities/HttpRequest.hpp"
#include "domain/http/entities/HttpResponse.hpp"
#include "domain/http/exceptions/HttpRequestException.hpp"
#include "domain/http/value_objects/HttpMethod.hpp"
#include "domain/http/value_objects/RouteMatchInfo.hpp"
#include "domain/shared/value_objects/ErrorCode.hpp"
#include "infrastructure/cgi/adapters/CgiExecutor.hpp"
#include "infrastructure/cgi/exceptions/CgiExecutionException.hpp"
#include "infrastructure/cgi/primitives/CgiRequest.hpp"
#include "infrastructure/cgi/primitives/CgiResponse.hpp"
#include "infrastructure/filesystem/adapters/DirectoryLister.hpp"
#include "infrastructure/filesystem/adapters/FileSystemHelper.hpp"
#include "infrastructure/http/RequestParser.hpp"
#include "infrastructure/network/adapters/ConnectionHandler.hpp"
#include "infrastructure/network/adapters/TcpSocket.hpp"
#include "infrastructure/network/exceptions/ConnectionException.hpp"

#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <limits.h>
#include <sstream>
#include <sys/stat.h>
#include <unistd.h>

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
    throw exceptions::ConnectionException(
        "Socket pointer cannot be NULL",
        exceptions::ConnectionException::INVALID_STATE);
  }

  if (serverConfig == NULL) {
    throw exceptions::ConnectionException(
        "ServerConfig pointer cannot be NULL",
        exceptions::ConnectionException::INVALID_STATE);
  }

  m_requestBuffer.reserve(K_READ_BUFFER_SIZE);

  std::ostringstream oss;
  oss << "ConnectionHandler created for " << getRemoteAddress();
  m_logger.debug(oss.str());
}

ConnectionHandler::~ConnectionHandler() {
  std::string remoteAddr;
  try {
    remoteAddr = getRemoteAddress();
  } catch (...) {
    remoteAddr = "unknown (disconnected)";
  }

  std::ostringstream oss;
  oss << "ConnectionHandler destroyed for " << remoteAddr;
  m_logger.debug(oss.str());

  delete m_socket;
  m_socket = NULL;
}

void ConnectionHandler::processEvent() {
  updateLastActivity(std::time(NULL));

  try {
    bool continueProcessing = true;
    while (continueProcessing) {
      continueProcessing = false;

      switch (m_state) {
        case STATE_READING_REQUEST:
          handleRead();
          if (m_state == STATE_PROCESSING) {
            continueProcessing = true;
          }
          break;

        case STATE_PROCESSING:
          processRequest();
          m_responseBuffer = m_response.serialize();
          m_responseOffset = 0;
          m_state = STATE_WRITING_RESPONSE;
          continueProcessing = true;
          break;

        case STATE_WRITING_RESPONSE:
          handleWrite();
          break;

        case STATE_KEEP_ALIVE:
          handleRead();
          if (m_state == STATE_PROCESSING) {
            continueProcessing = true;
          }
          break;

        case STATE_CLOSING:
          break;
      }
    }
  } catch (const domain::http::exceptions::HttpRequestException& ex) {
    m_logger.error(std::string("Request error: ") + ex.what());
    generateErrorResponse(
        domain::shared::value_objects::ErrorCode::badRequest(), ex.what());
    m_responseBuffer = m_response.serialize();
    m_responseOffset = 0;
    m_state = STATE_WRITING_RESPONSE;
  } catch (const exceptions::ConnectionException& ex) {
    m_logger.error(std::string("Connection error: ") + ex.what());
    domain::shared::value_objects::ErrorCode errorCode =
        domain::shared::value_objects::ErrorCode::internalServerError();
    std::string errorMsg = ex.what();
    if (errorMsg.find("Request size exceeds") != std::string::npos ||
        errorMsg.find("too large") != std::string::npos ||
        errorMsg.find("Request entity too large") != std::string::npos) {
      errorCode = domain::shared::value_objects::ErrorCode::payloadTooLarge();
    }
    generateErrorResponse(errorCode, ex.what());
    m_responseBuffer = m_response.serialize();
    m_responseOffset = 0;
    m_state = STATE_WRITING_RESPONSE;
  } catch (const std::exception& ex) {
    m_logger.error(std::string("Unexpected error: ") + ex.what());
    generateErrorResponse(
        domain::shared::value_objects::ErrorCode::internalServerError(),
        "Internal Server Error");
    m_responseBuffer = m_response.serialize();
    m_responseOffset = 0;
    m_state = STATE_WRITING_RESPONSE;
  }
}

bool ConnectionHandler::shouldClose() const {
  return m_state == STATE_CLOSING && m_responseBuffer.empty();
}

ConnectionHandler::State ConnectionHandler::getState() const { return m_state; }

int ConnectionHandler::getFd() const {
  return (m_socket != NULL) ? m_socket->getFd() : -1;
}

std::string ConnectionHandler::getRemoteAddress() const {
  return (m_socket != NULL) ? m_socket->getRemoteAddress() : "unknown";
}

bool ConnectionHandler::isTimedOut(time_t currentTime) const {
  const time_t timeout = (m_state == STATE_KEEP_ALIVE) ? K_KEEPALIVE_TIMEOUT
                                                       : K_CONNECTION_TIMEOUT;
  return (currentTime - m_lastActivityTime) > timeout;
}

void ConnectionHandler::updateLastActivity(time_t currentTime) {
  m_lastActivityTime = currentTime;
}

void ConnectionHandler::handleRead() {
  char buffer[K_READ_BUFFER_SIZE];
  const ssize_t bytesRead = m_socket->read(buffer, K_READ_BUFFER_SIZE);

  if (bytesRead == 0) {
    m_logger.debug("Client closed connection: " + getRemoteAddress());
    m_state = STATE_CLOSING;
    return;
  }

  if (bytesRead == -1) {
    return;
  }

  m_requestBuffer.append(buffer, static_cast<size_t>(bytesRead));

  std::ostringstream oss;
  oss << "Read " << bytesRead << " bytes from " << getRemoteAddress()
      << " (total: " << m_requestBuffer.size() << ")";
  m_logger.debug(oss.str());

  if (m_requestBuffer.size() > K_MAX_REQUEST_SIZE) {
    throw exceptions::ConnectionException(
        "Request size exceeds maximum allowed",
        exceptions::ConnectionException::REQUEST_TOO_LARGE);
  }

  if (parseRequest()) {
    m_state = STATE_PROCESSING;
  }
}

void ConnectionHandler::handleWrite() {
  if (m_responseBuffer.empty()) {
    logRequest(m_request, m_response);

    if (shouldKeepAlive()) {
      m_logger.debug("Keeping connection alive: " + getRemoteAddress());
      resetForNextRequest();
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
  http::RequestParser parser;
  parser.setMaxHeaderSize(K_MAX_REQUEST_SIZE);
  parser.setMaxBodySize(m_serverConfig->getClientMaxBodySize().getBytes());

  if (!parser.parse(m_requestBuffer.c_str(), m_requestBuffer.size())) {
    return false;
  }

  if (!parser.isComplete()) {
    return false;
  }

  if (parser.hasError()) {
    throw exceptions::ConnectionException(
        "Failed to parse HTTP request",
        exceptions::ConnectionException::MALFORMED_REQUEST);
  }

  const http::ParsedRequest& parsedReq = parser.getRequest();

  m_request.setMethod(parsedReq.method);
  m_request.setPath(parsedReq.path);
  m_request.setQuery(parsedReq.query);
  m_request.setVersion(domain::http::value_objects::HttpVersion::fromString(
      parsedReq.httpVersion));
  m_request.clearHeaders();

  for (std::map<std::string, std::string>::const_iterator it =
           parsedReq.headers.begin();
       it != parsedReq.headers.end(); ++it) {
    m_request.addHeader(it->first, it->second);
  }

  if (!parsedReq.body.empty()) {
    m_request.setBody(domain::http::entities::HttpRequest::Body(
        parsedReq.body.begin(), parsedReq.body.end()));
  }

  m_request.validate();

  std::ostringstream oss;
  oss << "Parsed request: " << m_request.getMethod().toString() << " "
      << m_request.getPath().toString() << " "
      << m_request.getVersion().toString();
  m_logger.info(oss.str());

  return true;
}

domain::filesystem::value_objects::Path
ConnectionHandler::resolvePathWithServerFallback(
    const domain::configuration::entities::LocationConfig& location,
    const std::string& requestPath) const {
  const domain::configuration::entities::LocationConfig::LocationMatchType
      matchType = location.getMatchType();
  bool isRegexLocation =
      (matchType == domain::configuration::entities::LocationConfig::
                        MATCH_REGEX_CASE_SENSITIVE ||
       matchType == domain::configuration::entities::LocationConfig::
                        MATCH_REGEX_CASE_INSENSITIVE);

  std::string relativeRequestPath = requestPath;
  if (!relativeRequestPath.empty() && relativeRequestPath[0] == '/') {
    relativeRequestPath = relativeRequestPath.substr(1);
  }

  if (isRegexLocation) {
    if (location.hasCgiConfig()) {
      try {
        const domain::configuration::value_objects::CgiConfig& cgiConfig =
            location.getCgiConfig();
        const domain::filesystem::value_objects::Path& cgiRoot =
            cgiConfig.getCgiRoot();
        std::string cgiRootStr = cgiRoot.toString();
        if (!cgiRoot.isEmpty() && cgiRootStr != "/" &&
            cgiRootStr.find('\\') == std::string::npos &&
            cgiRootStr.find('$') == std::string::npos) {
          return cgiRoot.join(relativeRequestPath);
        }
      } catch (const std::exception& ex) {
        std::ostringstream oss;
        oss << "CGI root resolution failed: " << ex.what();
        m_logger.debug(oss.str());
      }
    }

    if (m_serverConfig != NULL && !m_serverConfig->getRoot().isEmpty()) {
      return m_serverConfig->getRoot().join(relativeRequestPath);
    }

    return domain::filesystem::value_objects::Path::fromString(
        "./" + relativeRequestPath, false);
  }

  try {
    const domain::filesystem::value_objects::Path& locationRoot =
        location.getRoot();
    std::string rootStr = locationRoot.toString();

    bool looksLikeRegex =
        (!rootStr.empty() &&
         (rootStr[0] == '\\' || rootStr.find('$') != std::string::npos ||
          rootStr.find('^') != std::string::npos ||
          rootStr.find('*') != std::string::npos));

    if (!looksLikeRegex && rootStr != "/") {
      return location.resolvePath(requestPath);
    }

    if (m_serverConfig != NULL && !m_serverConfig->getRoot().isEmpty()) {
      return m_serverConfig->getRoot().join(relativeRequestPath);
    }

  } catch (const std::exception& ex) {
    std::ostringstream oss;
    oss << "Location root resolution failed (likely regex location): "
        << ex.what();
    m_logger.debug(oss.str());

    if (m_serverConfig != NULL && !m_serverConfig->getRoot().isEmpty()) {
      return m_serverConfig->getRoot().join(relativeRequestPath);
    }
  }

  return domain::filesystem::value_objects::Path::fromString(
      "./" + relativeRequestPath, false);
}

domain::filesystem::value_objects::Path
ConnectionHandler::resolveErrorPagePath(
    const domain::configuration::entities::LocationConfig& location,
    const std::string& errorPagePath) const {
  if (errorPagePath.empty()) {
    throw std::runtime_error("Error page path cannot be empty");
  }

  // Case 1: Relative path starting with "./" (e.g., ./errors/404.html)
  // Convert to absolute path using realpath()
  if (errorPagePath.length() >= 2 && errorPagePath[0] == '.' &&
      errorPagePath[1] == '/') {
    char resolvedPath[PATH_MAX];

    if (realpath(errorPagePath.c_str(), resolvedPath) == NULL) {
      std::ostringstream errorMsg;
      errorMsg << "Failed to resolve error page path: " << errorPagePath
               << " (errno=" << errno << ")";
      m_logger.warn(errorMsg.str());

      // Try to construct path manually if realpath fails (file might not exist
      // yet)
      char cwd[PATH_MAX];
      if (getcwd(cwd, sizeof(cwd)) != NULL) {
        std::string cwdStr(cwd);
        std::string relativePart = errorPagePath.substr(2);  // Remove "./"
        std::string constructedPath = cwdStr + "/" + relativePart;
        return domain::filesystem::value_objects::Path(constructedPath);
      }

      throw std::runtime_error("Failed to resolve relative error page path: " +
                               errorPagePath);
    }

    std::ostringstream debugMsg;
    debugMsg << "Resolved relative error page path: " << errorPagePath << " -> "
             << resolvedPath;
    m_logger.debug(debugMsg.str());

    return domain::filesystem::value_objects::Path(resolvedPath);
  }

  // Case 2: Absolute path from server root starting with "/" (e.g.,
  // /errors/404.html) Join with server or location root
  if (errorPagePath[0] == '/') {
    std::string pathWithoutLeadingSlash = errorPagePath.substr(1);

    // Try location root first
    try {
      const domain::filesystem::value_objects::Path& locationRoot =
          location.getRoot();
      if (!locationRoot.isEmpty() && locationRoot.toString() != "/") {
        domain::filesystem::value_objects::Path resolvedPath =
            locationRoot.join(pathWithoutLeadingSlash);

        std::ostringstream debugMsg;
        debugMsg << "Resolved error page path using location root: "
                 << errorPagePath << " + " << locationRoot.toString() << " -> "
                 << resolvedPath.toString();
        m_logger.debug(debugMsg.str());

        return resolvedPath;
      }
    } catch (const std::exception& ex) {
      std::ostringstream warnMsg;
      warnMsg << "Could not use location root for error page: " << ex.what();
      m_logger.debug(warnMsg.str());
    }

    // Fall back to server root
    if (m_serverConfig != NULL && !m_serverConfig->getRoot().isEmpty()) {
      domain::filesystem::value_objects::Path resolvedPath =
          m_serverConfig->getRoot().join(pathWithoutLeadingSlash);

      std::ostringstream debugMsg;
      debugMsg << "Resolved error page path using server root: " << errorPagePath
               << " + " << m_serverConfig->getRoot().toString() << " -> "
               << resolvedPath.toString();
      m_logger.debug(debugMsg.str());

      return resolvedPath;
    }

    // If no root configured, use path as-is (treat as absolute filesystem path)
    std::ostringstream warnMsg;
    warnMsg << "No server root configured, using error page path as absolute: "
            << errorPagePath;
    m_logger.warn(warnMsg.str());

    return domain::filesystem::value_objects::Path(errorPagePath);
  }

  // Case 3: Neither ./ nor / - treat as relative to server root
  std::ostringstream warnMsg;
  warnMsg << "Error page path '" << errorPagePath
          << "' doesn't start with ./ or /, treating as relative to server root";
  m_logger.warn(warnMsg.str());

  if (m_serverConfig != NULL && !m_serverConfig->getRoot().isEmpty()) {
    return m_serverConfig->getRoot().join(errorPagePath);
  }

  return domain::filesystem::value_objects::Path("./" + errorPagePath);
}

void ConnectionHandler::processRequest() {
  try {
    const domain::configuration::entities::ServerConfig* config =
        resolveVirtualHost();

    const domain::http::value_objects::HttpMethod& method =
        m_request.getMethod();
    const domain::filesystem::value_objects::Path requestPath =
        domain::filesystem::value_objects::Path::fromString(
            m_request.getPath().toString(), true);

    const domain::configuration::entities::LocationConfig* matchedLocation =
        findMatchingLocation(config, requestPath.toString());

    if (matchedLocation == NULL) {
      generateErrorResponse(
          domain::shared::value_objects::ErrorCode::notFound(),
          "No matching location configuration found");
      return;
    }

    if (!matchedLocation->isMethodAllowed(method)) {
      generateErrorResponse(
          domain::shared::value_objects::ErrorCode::methodNotAllowed(),
          "Method not allowed for this resource");
      return;
    }

    if (matchedLocation->hasReturnRedirect()) {
      handleRedirect(*matchedLocation);
      return;
    }

    if (matchedLocation->hasReturnContent()) {
      handleReturnContent(*matchedLocation);
      return;
    }

    if (!validateRequestBodySize(*matchedLocation)) {
      generateErrorResponse(
          domain::shared::value_objects::ErrorCode::payloadTooLarge(),
          "Request entity too large");
      return;
    }

    m_logger.debug("route will be create");
    domain::configuration::value_objects::Route route =
        matchedLocation->toRoute();
    m_logger.debug("route is working");

    if (method == domain::http::value_objects::HttpMethod::get() ||
        method == domain::http::value_objects::HttpMethod::head()) {
      m_logger.debug("method == get or head");
      handleGetRequest(route, *matchedLocation, requestPath);
    } else if (method == domain::http::value_objects::HttpMethod::post()) {
      m_logger.debug("method == post");
      handlePostRequest(route, *matchedLocation, requestPath);
    } else if (method ==
               domain::http::value_objects::HttpMethod::deleteMethod()) {
      handleDeleteRequest(route, *matchedLocation, requestPath);
    } else {
      m_logger.debug("else generateErrorResponse");
      generateErrorResponse(
          domain::shared::value_objects::ErrorCode::methodNotAllowed(),
          "Unsupported HTTP method");
    }

    applyCustomHeaders(*matchedLocation);

    if (shouldKeepAlive()) {
      m_response.setConnection("keep-alive");
    } else {
      m_response.setConnection("close");
    }

  } catch (const domain::http::exceptions::HttpRequestException& ex) {
    m_logger.error(std::string("HTTP request error: ") + ex.what());
    generateErrorResponse(
        domain::shared::value_objects::ErrorCode::badRequest(), ex.what());
  } catch (
      const domain::configuration::exceptions::LocationConfigException& ex) {
    m_logger.error(std::string("Location configuration error: ") + ex.what());
    generateErrorResponse(
        domain::shared::value_objects::ErrorCode::internalServerError(),
        "Internal Server Error");
  } catch (const std::exception& ex) {
    m_logger.error(std::string("Request processing failed: ") + ex.what());
    generateErrorResponse(
        domain::shared::value_objects::ErrorCode::internalServerError(),
        "Internal Server Error");
  }
}

const domain::configuration::entities::ServerConfig*
ConnectionHandler::resolveVirtualHost() {
  if (!m_request.hasHeader("host")) {
    return m_serverConfig;
  }

  const std::string hostHeader = m_request.getHost();
  if (hostHeader.empty()) {
    return m_serverConfig;
  }

  return m_serverConfig;
}

const domain::configuration::entities::LocationConfig*
ConnectionHandler::findMatchingLocation(
    const domain::configuration::entities::ServerConfig* serverConfig,
    const std::string& requestPath) const {
  if (serverConfig == NULL) {
    return NULL;
  }

  const std::vector<domain::configuration::entities::LocationConfig*>&
      locations = serverConfig->getLocations();

  const domain::configuration::entities::LocationConfig* exactMatch = NULL;
  const domain::configuration::entities::LocationConfig* bestPrefixMatch = NULL;
  std::size_t longestPrefixLength = 0;
  const domain::configuration::entities::LocationConfig* regexMatch = NULL;

  for (std::vector<domain::configuration::entities::LocationConfig*>::
           const_iterator it = locations.begin();
       it != locations.end(); ++it) {
    const domain::configuration::entities::LocationConfig* location = *it;
    const domain::configuration::entities::LocationConfig::LocationMatchType
        matchType = location->getMatchType();

    if (matchType ==
        domain::configuration::entities::LocationConfig::MATCH_EXACT) {
      if (location->matchesPath(requestPath)) {
        exactMatch = location;
        break;
      }
    } else if (matchType ==
               domain::configuration::entities::LocationConfig::MATCH_PREFIX) {
      if (location->matchesPath(requestPath)) {
        const std::size_t pathLength = location->getPath().length();
        if (pathLength > longestPrefixLength) {
          longestPrefixLength = pathLength;
          bestPrefixMatch = location;
        }
      }
    }
  }

  if (exactMatch != NULL) {
    return exactMatch;
  }

  for (std::vector<domain::configuration::entities::LocationConfig*>::
           const_iterator it = locations.begin();
       it != locations.end(); ++it) {
    const domain::configuration::entities::LocationConfig* location = *it;
    const domain::configuration::entities::LocationConfig::LocationMatchType
        matchType = location->getMatchType();

    if (matchType == domain::configuration::entities::LocationConfig::
                         MATCH_REGEX_CASE_SENSITIVE ||
        matchType == domain::configuration::entities::LocationConfig::
                         MATCH_REGEX_CASE_INSENSITIVE) {
      if (location->matchesPath(requestPath)) {
        regexMatch = location;
        break;
      }
    }
  }

  if (regexMatch != NULL) {
    return regexMatch;
  }

  if (bestPrefixMatch != NULL) {
    return bestPrefixMatch;
  }

  for (std::vector<domain::configuration::entities::LocationConfig*>::
           const_iterator it = locations.begin();
       it != locations.end(); ++it) {
    const domain::configuration::entities::LocationConfig* location = *it;
    if (location->getPath() == "/") {
      return location;
    }
  }

  return NULL;
}

void ConnectionHandler::handleGetRequest(
    const domain::configuration::value_objects::Route& /* route */,
    const domain::configuration::entities::LocationConfig& location,
    const domain::filesystem::value_objects::Path& requestPath) {
  domain::filesystem::value_objects::Path resolvedPath =
      resolvePathWithServerFallback(location, requestPath.toString());

  std::ostringstream debugMsg;
  debugMsg << "GET request - resolved path: " << resolvedPath.toString();
  m_logger.debug(debugMsg.str());

  bool pathExists = false;
  try {
    pathExists =
        filesystem::adapters::FileSystemHelper::exists(resolvedPath.toString());
  } catch (const std::exception&) {
    pathExists = false;
  }

  std::ostringstream oss;
  oss << "Inside of handleGetRequest verify pathExists = " << pathExists;
  m_logger.debug(oss.str());
  if (!pathExists) {
    if (!location.getTryFiles().empty()) {
      resolvedPath = tryFindFile(location, requestPath);
      if (resolvedPath.isEmpty()) {
        handleNotFound(location);
        return;
      }
    } else {
      handleNotFound(location);
      return;
    }
  }

  bool isDir = false;
  try {
    isDir = filesystem::adapters::FileSystemHelper::isDirectory(
        resolvedPath.toString());
  } catch (const std::exception&) {
    isDir = false;
  }

  if (isDir) {
    handleDirectoryRequest(location, resolvedPath, requestPath);
    return;
  }

  if (location.hasCgiConfig()) {
    const domain::configuration::entities::LocationConfig::LocationMatchType
        matchType = location.getMatchType();
    bool isRegexLocation =
        (matchType == domain::configuration::entities::LocationConfig::
                          MATCH_REGEX_CASE_SENSITIVE ||
         matchType == domain::configuration::entities::LocationConfig::
                          MATCH_REGEX_CASE_INSENSITIVE);

    if (isRegexLocation ||
        location.getCgiConfig().matchesExtension(resolvedPath.toString())) {
      m_logger.debug("Routing to CGI handler for: " + resolvedPath.toString());
      handleCgiRequest(location, resolvedPath);
      return;
    }
  }

  handleStaticFileRequest(location, resolvedPath);
}

void ConnectionHandler::handlePostRequest(
    const domain::configuration::value_objects::Route& /* route */,
    const domain::configuration::entities::LocationConfig& location,
    const domain::filesystem::value_objects::Path& requestPath) {
  if (location.isUploadRoute()) {
    handleFileUpload(location, requestPath);
    return;
  }

  if (location.hasCgiConfig()) {
    domain::filesystem::value_objects::Path resolvedPath =
        resolvePathWithServerFallback(location, requestPath.toString());

    const domain::configuration::entities::LocationConfig::LocationMatchType
        matchType = location.getMatchType();
    bool isRegexLocation =
        (matchType == domain::configuration::entities::LocationConfig::
                          MATCH_REGEX_CASE_SENSITIVE ||
         matchType == domain::configuration::entities::LocationConfig::
                          MATCH_REGEX_CASE_INSENSITIVE);

    if (isRegexLocation ||
        location.getCgiConfig().matchesExtension(resolvedPath.toString())) {
      m_logger.debug("POST routing to CGI handler for: " +
                     resolvedPath.toString());
      handleCgiRequest(location, resolvedPath);
      return;
    }
  }

  generateErrorResponse(
      domain::shared::value_objects::ErrorCode::methodNotAllowed(),
      "POST method not allowed for this resource");
}

void ConnectionHandler::handleDeleteRequest(
    const domain::configuration::value_objects::Route& /* route */,
    const domain::configuration::entities::LocationConfig& location,
    const domain::filesystem::value_objects::Path& requestPath) {
  domain::filesystem::value_objects::Path resolvedPath;

  m_logger.debug("enter handler Delete");
  if (location.isUploadRoute() && location.hasUploadConfig()) {
    const domain::configuration::value_objects::UploadConfig& uploadConfig =
        location.getUploadConfig();
    const domain::filesystem::value_objects::Path& uploadStore =
        uploadConfig.getUploadDirectory();

    m_logger.debug("uploadConfig => " +
                   uploadConfig.getUploadDirectory().getFilename());
    m_logger.debug("uploadStore => " + uploadStore.getFilename());
    std::string requestPathStr = requestPath.toString();
    std::string locationPath = location.getPath();

    std::string relativePath = requestPathStr;
    if (requestPathStr.length() > locationPath.length() &&
        requestPathStr.substr(0, locationPath.length()) == locationPath) {
      relativePath = requestPathStr.substr(locationPath.length());
      if (!relativePath.empty() && relativePath[0] == '/') {
        relativePath = relativePath.substr(1);
      }
    }

    if (!relativePath.empty()) {
      resolvedPath = uploadStore.join(relativePath);
    } else {
      generateErrorResponse(
          domain::shared::value_objects::ErrorCode::badRequest(),
          "Invalid file path for deletion");
      return;
    }

    std::ostringstream debugMsg;
    debugMsg << "DELETE request (upload location) - resolved path: "
             << resolvedPath.toString();
    m_logger.debug(debugMsg.str());
  } else {
    resolvedPath =
        resolvePathWithServerFallback(location, requestPath.toString());

    std::ostringstream debugMsg;
    debugMsg << "DELETE request - resolved path: " << resolvedPath.toString();
    m_logger.debug(debugMsg.str());
  }

  const std::string resolvedPathStr = resolvedPath.toString();

  m_logger.debug("Checking if path exists: " + resolvedPathStr);
  if (!filesystem::adapters::FileSystemHelper::exists(resolvedPathStr)) {
    m_logger.debug("Path does not exist");
    handleNotFound(location);
    return;
  }
  m_logger.debug("Path exists");

  m_logger.debug("Checking if path is directory: " + resolvedPathStr);
  if (filesystem::adapters::FileSystemHelper::isDirectory(resolvedPathStr)) {
    m_logger.debug("Path is a directory - deletion forbidden");
    generateErrorResponse(domain::shared::value_objects::ErrorCode::forbidden(),
                          "Directory deletion not allowed");
    return;
  }
  m_logger.debug("Path is a file - proceeding with deletion");

  try {
    if (std::remove(resolvedPathStr.c_str()) != 0) {
      throw std::runtime_error("Failed to delete file");
    }

    m_response = domain::http::entities::HttpResponse::noContent();
    m_response.setContentType("text/plain");

  } catch (const std::exception& ex) {
    m_logger.error(std::string("Delete operation failed: ") + ex.what());
    generateErrorResponse(
        domain::shared::value_objects::ErrorCode::internalServerError(),
        "Failed to delete resource");
  }
}

void ConnectionHandler::handleDirectoryRequest(
    const domain::configuration::entities::LocationConfig& location,
    const domain::filesystem::value_objects::Path& directoryPath,
    const domain::filesystem::value_objects::Path& requestPath) {
  const std::vector<std::string>& indexFiles = location.getIndexFiles();

  std::ostringstream debugDirPath;
  debugDirPath << "handleDirectoryRequest called with directory: "
               << directoryPath.toString() << ", checking " << indexFiles.size()
               << " index files";
  m_logger.debug(debugDirPath.str());

  for (std::vector<std::string>::const_iterator it = indexFiles.begin();
       it != indexFiles.end(); ++it) {
    try {
      m_logger.debug("About to join path: " + directoryPath.toString() +
                     " with: " + *it);

      domain::filesystem::value_objects::Path indexPath =
          directoryPath.join(*it);

      m_logger.debug("Successfully created path: " + indexPath.toString());

      bool pathExists = false;
      bool pathIsDir = false;

      try {
        m_logger.debug("About to check if path exists...");
        pathExists =
            filesystem::adapters::FileSystemHelper::exists(indexPath.toString());
        m_logger.debug(pathExists ? "Path exists: YES" : "Path exists: NO");
      } catch (const std::exception& ex) {
        m_logger.debug(std::string("Path exists check failed: ") + ex.what());
        pathExists = false;
      }

      if (pathExists) {
        try {
          m_logger.debug("About to check if path is directory...");
          pathIsDir = filesystem::adapters::FileSystemHelper::isDirectory(
              indexPath.toString());
          m_logger.debug(pathIsDir ? "Path is directory: YES"
                                   : "Path is directory: NO (it's a file)");
        } catch (const std::exception& ex) {
          m_logger.debug(std::string("Path isDirectory check failed: ") +
                         ex.what());
          pathIsDir = false;
        }
      }

      std::ostringstream debugIndexPath;
      debugIndexPath << "Checking index file: " << indexPath.toString()
                     << " - exists: " << (pathExists ? "YES" : "NO")
                     << " - isDir: " << (pathIsDir ? "YES" : "NO");
      m_logger.debug(debugIndexPath.str());

      if (pathExists && !pathIsDir) {
        if (!filesystem::adapters::FileSystemHelper::isReadable(
                indexPath.toString())) {
          m_logger.error("Index file exists but is not readable: " +
                         indexPath.toString());
          continue;
        }

        m_logger.debug("Found valid index file: " + indexPath.toString());

        if (location.hasCgiConfig() &&
            location.getCgiConfig().matchesExtension(indexPath.toString())) {
          handleCgiRequest(location, indexPath);
          return;
        }

        handleStaticFileRequest(location, indexPath);
        return;
      }
    } catch (const std::exception& ex) {
      m_logger.error(std::string("Exception during path join: ") + ex.what() +
                     " (directory: " + directoryPath.toString() +
                     ", index: " + *it + ")");
      throw;
    }
  }

  m_logger.debug("No index file found, checking autoindex: " +
                 std::string(location.getAutoIndex() ? "enabled" : "disabled"));

  m_logger.debug("directoryPath => " + directoryPath.toString() + " requestPath => " + requestPath.toString());
  if (location.getAutoIndex()) {
    handleDirectoryListing(directoryPath, requestPath);
    return;
  }

  generateErrorResponse(domain::shared::value_objects::ErrorCode::forbidden(),
                        "Directory listing is disabled");
}

void ConnectionHandler::handleStaticFileRequest(
    const domain::configuration::entities::LocationConfig& /* location */,
    const domain::filesystem::value_objects::Path& filePath) {
  try {
    std::string pathStr = filePath.toString();

    m_logger.debug("handleStaticFileRequest: Attempting to serve file: " +
                   pathStr);

    struct stat fileStat;
    if (stat(pathStr.c_str(), &fileStat) != 0) {
      m_logger.error("File not found (stat failed): " + pathStr);
      generateErrorResponse(
          domain::shared::value_objects::ErrorCode::notFound(), "Not Found");
      return;
    }

    if (S_ISDIR(fileStat.st_mode)) {
      m_logger.error("Path is a directory, not a file: " + pathStr);
      generateErrorResponse(
          domain::shared::value_objects::ErrorCode::forbidden(),
          "Path is a directory");
      return;
    }

    if (access(pathStr.c_str(), R_OK) != 0) {
      m_logger.error("File not readable: " + pathStr);
      generateErrorResponse(
          domain::shared::value_objects::ErrorCode::forbidden(),
          "File not readable");
      return;
    }

    std::ifstream file(pathStr.c_str(), std::ios::binary);
    if (!file.is_open()) {
      m_logger.error("Failed to open file: " + pathStr);
      generateErrorResponse(
          domain::shared::value_objects::ErrorCode::internalServerError(),
          "Failed to open file");
      return;
    }

    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    file.close();

    std::ostringstream oss;
    oss << "Successfully read file: " << pathStr << " (" << content.length()
        << " bytes)";
    m_logger.debug(oss.str());

    std::string mimeType = "application/octet-stream";
    std::string filename = filePath.getFilename();
    std::size_t dotPos = filename.find_last_of('.');
    if (dotPos != std::string::npos) {
      std::string ext = filename.substr(dotPos);
      if (ext == ".html" || ext == ".htm")
        mimeType = "text/html; charset=utf-8";
      else if (ext == ".css")
        mimeType = "text/css";
      else if (ext == ".js")
        mimeType = "application/javascript";
      else if (ext == ".json")
        mimeType = "application/json";
      else if (ext == ".png")
        mimeType = "image/png";
      else if (ext == ".jpg" || ext == ".jpeg")
        mimeType = "image/jpeg";
      else if (ext == ".gif")
        mimeType = "image/gif";
      else if (ext == ".svg")
        mimeType = "image/svg+xml";
      else if (ext == ".ico")
        mimeType = "image/x-icon";
      else if (ext == ".txt")
        mimeType = "text/plain";
      else if (ext == ".xml")
        mimeType = "application/xml";
      else if (ext == ".pdf")
        mimeType = "application/pdf";
      else if (ext == ".zip")
        mimeType = "application/zip";
    }

    m_response = domain::http::entities::HttpResponse::ok(content);
    m_response.setContentType(mimeType);

    std::ostringstream contentLength;
    contentLength << content.length();
    m_response.addHeader("Content-Length", contentLength.str());

    char timeBuffer[80];
    struct tm* timeinfo = localtime(&fileStat.st_mtime);
    if (timeinfo != NULL) {
      strftime(timeBuffer, sizeof(timeBuffer), "%a, %d %b %Y %H:%M:%S GMT",
               timeinfo);
      m_response.addHeader("Last-Modified", std::string(timeBuffer));
    }

    m_logger.debug("Response prepared successfully for: " + pathStr);

  } catch (const std::exception& ex) {
    m_logger.error(std::string("Static file error: ") + ex.what());
    generateErrorResponse(
        domain::shared::value_objects::ErrorCode::internalServerError(),
        "Internal Server Error");
  }
}

void ConnectionHandler::handleDirectoryListing(
    const domain::filesystem::value_objects::Path& directoryPath,
    const domain::filesystem::value_objects::Path& requestPath) {
  try {
    const std::string dirPathStr = directoryPath.toString();
    char resolvedPath[PATH_MAX];
    
    if (realpath(dirPathStr.c_str(), resolvedPath) == NULL) {
      std::ostringstream errorMsg;
      errorMsg << "Failed to resolve absolute path for: " << dirPathStr
               << " (errno: " << errno << ")";
      m_logger.error(errorMsg.str());
      generateErrorResponse(
          domain::shared::value_objects::ErrorCode::internalServerError(),
          "Failed to resolve directory path");
      return;
    }
    
    const domain::filesystem::value_objects::Path absolutePath(resolvedPath);
    
    std::ostringstream debugMsg;
    debugMsg << "Resolved directory path: " << dirPathStr << " -> "
             << absolutePath.toString();
    m_logger.debug(debugMsg.str());
    
    const std::string htmlListing =
        filesystem::adapters::DirectoryLister::generateHtmlListing(
            absolutePath, requestPath, false, "name", true);

    m_response = domain::http::entities::HttpResponse::ok(htmlListing);
    m_response.setContentType("text/html; charset=utf-8");

  } catch (const std::exception& ex) {
    m_logger.error(std::string("Directory listing error: ") + ex.what());
    generateErrorResponse(
        domain::shared::value_objects::ErrorCode::internalServerError(),
        "Failed to generate directory listing");
  }
}

void ConnectionHandler::handleCgiRequest(
    const domain::configuration::entities::LocationConfig& location,
    const domain::filesystem::value_objects::Path& scriptPath) {
  try {
    const domain::configuration::value_objects::CgiConfig& cgiConfig =
        location.getCgiConfig();

    domain::http::value_objects::RouteMatchInfo matchInfo =
        domain::http::value_objects::RouteMatchInfo::createForFile(
            scriptPath, scriptPath.toString());

    std::string serverName = "localhost";
    unsigned int serverPort = 8080;
    if (m_request.hasHeader("host")) {
      std::string hostHeader = m_request.getHost();
      std::size_t colonPos = hostHeader.find(':');
      if (colonPos != std::string::npos) {
        serverName = hostHeader.substr(0, colonPos);
        serverPort = static_cast<unsigned int>(
            std::atoi(hostHeader.substr(colonPos + 1).c_str()));
      } else {
        serverName = hostHeader;
      }
    }

    cgi::primitives::CgiRequest cgiRequest(m_request, cgiConfig, matchInfo,
                                           serverName, serverPort);

    cgi::adapters::CgiExecutor executor(m_logger);
    cgi::primitives::CgiResponse cgiResponse = executor.execute(cgiRequest);

    buildHttpResponseFromCgi(cgiResponse);

  } catch (const cgi::exceptions::CgiExecutionException& ex) {
    m_logger.error(std::string("CGI execution error: ") + ex.what());
    generateErrorResponse(
        domain::shared::value_objects::ErrorCode::internalServerError(),
        "CGI Script Error");
  } catch (const std::exception& ex) {
    m_logger.error(std::string("CGI processing error: ") + ex.what());
    generateErrorResponse(
        domain::shared::value_objects::ErrorCode::internalServerError(),
        "Internal Server Error");
  }
}

void ConnectionHandler::handleFileUpload(
    const domain::configuration::entities::LocationConfig& location,
    const domain::filesystem::value_objects::Path& /* requestPath */) {
  if (!location.hasUploadConfig()) {
    generateErrorResponse(
        domain::shared::value_objects::ErrorCode::methodNotAllowed(),
        "Upload not configured for this location");
    return;
  }

  const domain::configuration::value_objects::UploadConfig& uploadConfig =
      location.getUploadConfig();

  try {
    if (!m_request.hasHeader("content-type")) {
      generateErrorResponse(
          domain::shared::value_objects::ErrorCode::badRequest(),
          "Content-Type header required for upload");
      return;
    }

    std::string contentType = m_request.getHeader("content-type");
    if (contentType.find("multipart/form-data") == std::string::npos) {
      generateErrorResponse(
          domain::shared::value_objects::ErrorCode::badRequest(),
          "Content-Type must be multipart/form-data");
      return;
    }

    std::string boundary = extractBoundary(contentType);
    if (boundary.empty()) {
      generateErrorResponse(
          domain::shared::value_objects::ErrorCode::badRequest(),
          "Missing boundary in Content-Type");
      return;
    }

    m_logger.debug("Upload boundary: " + boundary);

    const domain::http::entities::HttpRequest::Body& body = m_request.getBody();
    std::string bodyStr(body.begin(), body.end());

    domain::filesystem::value_objects::Size bodySize(body.size());
    if (!uploadConfig.validateFileSize(bodySize)) {
      generateErrorResponse(
          domain::shared::value_objects::ErrorCode::payloadTooLarge(),
          "Uploaded file exceeds maximum size");
      return;
    }

    std::string filename;
    std::vector<char> fileContent;
    if (!parseMultipartFormData(bodyStr, boundary, filename, fileContent)) {
      generateErrorResponse(
          domain::shared::value_objects::ErrorCode::badRequest(),
          "Invalid multipart format");
      return;
    }

    if (filename.empty()) {
      std::ostringstream defaultName;
      defaultName << "upload_" << std::time(NULL);
      filename = defaultName.str();
    }

    filename = sanitizeFilename(filename);
    m_logger.debug("Upload filename: " + filename);

    std::ostringstream sizeMsg;
    sizeMsg << "Upload content size: " << fileContent.size();
    m_logger.debug(sizeMsg.str());

    domain::filesystem::value_objects::Path uploadDir =
        uploadConfig.getUploadDirectory();
    domain::filesystem::value_objects::Path filePath = uploadDir.join(filename);

    m_logger.debug("Upload destination: " + filePath.toString());

    ensureDirectoryExists(uploadDir);
    writeUploadedFile(filePath, fileContent);

    m_logger.info("File uploaded successfully: " + filePath.toString());

    m_response = domain::http::entities::HttpResponse(
        domain::shared::value_objects::ErrorCode::created(),
        "File uploaded successfully");
    m_response.setContentType("text/html");

    std::ostringstream successBody;
    successBody << "<!DOCTYPE html>\n"
                << "<html><head><title>Upload Success</title></head>\n"
                << "<body>\n"
                << "<h1>File Uploaded Successfully</h1>\n"
                << "<p>Filename: " << filename << "</p>\n"
                << "<p>Size: " << fileContent.size() << " bytes</p>\n"
                << "<p><a href=\"/\">Back to Home</a></p>\n"
                << "</body></html>\n";
    m_response.setBody(successBody.str());

  } catch (const std::exception& ex) {
    m_logger.error(std::string("File upload error: ") + ex.what());
    generateErrorResponse(
        domain::shared::value_objects::ErrorCode::internalServerError(),
        "Upload failed");
  }
}

std::string ConnectionHandler::extractBoundary(
    const std::string& contentType) const {
  std::size_t boundaryPos = contentType.find("boundary=");
  if (boundaryPos == std::string::npos) {
    return "";
  }

  std::string boundary = contentType.substr(boundaryPos + 9);

  if (!boundary.empty() && boundary[0] == '"') {
    boundary = boundary.substr(1);
    std::size_t endQuote = boundary.find('"');
    if (endQuote != std::string::npos) {
      boundary = boundary.substr(0, endQuote);
    }
  }

  std::size_t semicolonPos = boundary.find(';');
  if (semicolonPos != std::string::npos) {
    boundary = boundary.substr(0, semicolonPos);
  }

  return boundary;
}

bool ConnectionHandler::parseMultipartFormData(
    const std::string& body, const std::string& boundary,
    std::string& outFilename, std::vector<char>& outContent) const {
  std::string delimiter = "--" + boundary;

  std::size_t pos = body.find(delimiter);
  if (pos == std::string::npos) {
    return false;
  }

  pos += delimiter.length();
  if (pos < body.length() && body[pos] == '\r') pos++;
  if (pos < body.length() && body[pos] == '\n') pos++;

  std::size_t nextDelimiter = body.find(delimiter, pos);
  if (nextDelimiter == std::string::npos) {
    return false;
  }

  std::string part = body.substr(pos, nextDelimiter - pos);

  std::size_t headerEnd = part.find("\r\n\r\n");
  std::size_t headerSeparatorLen = 4;
  if (headerEnd == std::string::npos) {
    headerEnd = part.find("\n\n");
    headerSeparatorLen = 2;
    if (headerEnd == std::string::npos) {
      return false;
    }
  }

  std::string headers = part.substr(0, headerEnd);
  std::string content = part.substr(headerEnd + headerSeparatorLen);

  while (!content.empty() && (content[content.length() - 1] == '\n' ||
                              content[content.length() - 1] == '\r')) {
    content = content.substr(0, content.length() - 1);
  }

  outFilename = extractFilenameFromHeaders(headers);
  outContent.assign(content.begin(), content.end());

  return true;
}

std::string ConnectionHandler::extractFilenameFromHeaders(
    const std::string& headers) const {
  std::string filename;

  std::size_t dispositionPos = headers.find("Content-Disposition:");
  if (dispositionPos == std::string::npos) {
    dispositionPos = headers.find("content-disposition:");
  }

  if (dispositionPos != std::string::npos) {
    std::size_t filenamePos = headers.find("filename=\"", dispositionPos);
    if (filenamePos != std::string::npos) {
      filenamePos += 10;
      std::size_t filenameEnd = headers.find('"', filenamePos);
      if (filenameEnd != std::string::npos) {
        filename = headers.substr(filenamePos, filenameEnd - filenamePos);
      }
    }
  }

  return filename;
}

std::string ConnectionHandler::sanitizeFilename(
    const std::string& filename) const {
  std::string sanitized = filename;

  std::size_t lastSlash = sanitized.find_last_of("/\\");
  if (lastSlash != std::string::npos) {
    sanitized = sanitized.substr(lastSlash + 1);
  }

  std::string result;
  for (std::size_t i = 0; i < sanitized.length(); ++i) {
    char c = sanitized[i];
    if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
        (c >= '0' && c <= '9') || c == '.' || c == '-' || c == '_') {
      result += c;
    } else if (c == ' ') {
      result += '_';
    }
  }

  if (result.empty()) {
    std::ostringstream defaultName;
    defaultName << "file_" << std::time(NULL);
    result = defaultName.str();
  }

  return result;
}

void ConnectionHandler::ensureDirectoryExists(
    const domain::filesystem::value_objects::Path& dirPath) const {
  try {
    filesystem::adapters::FileSystemHelper::isDirectory(dirPath.toString());
  } catch (const std::exception&) {
    if (!filesystem::adapters::FileSystemHelper::createDirectoryRecursive(
            dirPath.toString())) {
      throw std::runtime_error("Failed to create upload directory: " +
                               dirPath.toString());
    }
  }
}

void ConnectionHandler::writeUploadedFile(
    const domain::filesystem::value_objects::Path& filePath,
    const std::vector<char>& content) const {
  std::ofstream outFile(filePath.toString().c_str(), std::ios::binary);
  if (!outFile.is_open()) {
    throw std::runtime_error("Failed to open file for writing: " +
                             filePath.toString());
  }

  outFile.write(content.data(), static_cast<std::streamsize>(content.size()));
  outFile.close();

  if (outFile.fail()) {
    throw std::runtime_error("Failed to write file: " + filePath.toString());
  }
}

void ConnectionHandler::handleRedirect(
    const domain::configuration::entities::LocationConfig& location) {
  const domain::http::value_objects::Uri& redirectUri =
      location.getReturnRedirect();
  const domain::shared::value_objects::ErrorCode& redirectCode =
      location.getReturnCode();

  m_response =
      domain::http::entities::HttpResponse(redirectCode, "Redirecting");
  m_response.addHeader("Location", redirectUri.toString());
  m_response.setContentType("text/html");

  std::ostringstream body;
  body << "<html><head><title>Redirect</title></head><body>"
       << "<h1>Redirecting</h1>" << "<p>The document has moved <a href=\""
       << redirectUri.toString() << "\">here</a>.</p>" << "</body></html>";
  m_response.setBody(body.str());
}

void ConnectionHandler::handleReturnContent(
    const domain::configuration::entities::LocationConfig& location) {
  const std::string& content = location.getReturnContent();
  const domain::shared::value_objects::ErrorCode& returnCode =
      location.getReturnCode();

  m_response = domain::http::entities::HttpResponse(returnCode, content);
  m_response.setContentType("text/plain");
}

void ConnectionHandler::generateErrorResponse(
    const domain::shared::value_objects::ErrorCode& statusCode,
    const std::string& message) {
  if (statusCode.isBadRequest()) {
    m_response = domain::http::entities::HttpResponse::badRequest(message);
  } else if (statusCode.isUnauthorized()) {
    m_response = domain::http::entities::HttpResponse::unauthorized(message);
  } else if (statusCode.isForbidden()) {
    m_response = domain::http::entities::HttpResponse::forbidden(message);
  } else if (statusCode.isNotFound()) {
    m_response = domain::http::entities::HttpResponse::notFound(message);
  } else if (statusCode.isMethodNotAllowed()) {
    m_response =
        domain::http::entities::HttpResponse::methodNotAllowed(message);
  } else if (statusCode.isPayloadTooLarge()) {
    m_response = domain::http::entities::HttpResponse::payloadTooLarge(message);
  } else if (statusCode.getValue() == 500) {
    m_response =
        domain::http::entities::HttpResponse::internalServerError(message);
  } else {
    m_response = domain::http::entities::HttpResponse(statusCode, message);
  }

  m_response.setConnection("close");
}

void ConnectionHandler::handleNotFound(
    const domain::configuration::entities::LocationConfig& location) {
  domain::shared::value_objects::ErrorCode notFoundCode =
      domain::shared::value_objects::ErrorCode::notFound();

  std::ostringstream debugMsg;
  debugMsg << "handleNotFound: looking for 404 error page";
  m_logger.debug(debugMsg.str());

  const domain::configuration::entities::LocationConfig::ErrorPageMap&
      locationErrorPages = location.getErrorPages();

  domain::configuration::entities::LocationConfig::ErrorPageMap::const_iterator
      locationIt = locationErrorPages.find(notFoundCode);

  if (locationIt != locationErrorPages.end()) {
    std::ostringstream foundMsg;
    foundMsg << "handleNotFound: found 404 page in location config: "
             << locationIt->second;
    m_logger.debug(foundMsg.str());
    serveErrorPage(locationIt->second, notFoundCode, location);
    return;
  }

  std::ostringstream tryServerMsg;
  tryServerMsg << "handleNotFound: 404 not in location, checking server config";
  m_logger.debug(tryServerMsg.str());

  if (m_serverConfig != NULL) {
    const domain::configuration::entities::ServerConfig::ErrorPageMap&
        serverErrorPages = m_serverConfig->getErrorPages();

    domain::configuration::entities::ServerConfig::ErrorPageMap::const_iterator
        serverIt = serverErrorPages.find(notFoundCode);

    if (serverIt != serverErrorPages.end()) {
      std::ostringstream foundServerMsg;
      foundServerMsg << "handleNotFound: found 404 page in server config: "
                     << serverIt->second;
      m_logger.debug(foundServerMsg.str());
      serveErrorPage(serverIt->second, notFoundCode, location);
      return;
    }
  }

  std::ostringstream noErrorPageMsg;
  noErrorPageMsg
      << "handleNotFound: no 404 error page configured, generating default";
  m_logger.debug(noErrorPageMsg.str());
  generateErrorResponse(notFoundCode, "Not Found");
}

void ConnectionHandler::serveErrorPage(
    const std::string& errorPagePath,
    const domain::shared::value_objects::ErrorCode& statusCode,
    const domain::configuration::entities::LocationConfig& location) {
  try {
    std::ostringstream debugStart;
    debugStart << "Attempting to serve error page: " << errorPagePath
               << " for status code: " << statusCode.getValue();
    m_logger.debug(debugStart.str());

    domain::filesystem::value_objects::Path errorPath =
        resolveErrorPagePath(location, errorPagePath);

    std::ostringstream debugMsg;
    debugMsg << "Resolved error page path: " << errorPagePath << " -> "
             << errorPath.toString();
    m_logger.debug(debugMsg.str());

    const std::string errorPathStr = errorPath.toString();

    if (!filesystem::adapters::FileSystemHelper::exists(errorPathStr)) {
      m_logger.warn("Error page not found at: " + errorPathStr);
      generateErrorResponse(statusCode, "Error page not found");
      return;
    }

    std::ifstream file(errorPathStr.c_str(), std::ios::binary);
    if (!file.is_open()) {
      m_logger.error("Failed to open error page file: " + errorPathStr);
      generateErrorResponse(statusCode, "Failed to open error page");
      return;
    }

    std::ostringstream contentStream;
    contentStream << file.rdbuf();
    file.close();

    std::string contentStr = contentStream.str();

    std::ostringstream sizeMsg;
    sizeMsg << "Read error page file: " << contentStr.size() << " bytes";
    m_logger.debug(sizeMsg.str());

    m_response = domain::http::entities::HttpResponse(statusCode, contentStr);
    m_response.setContentType("text/html");

    std::ostringstream successMsg;
    successMsg << "Served error page " << statusCode.getValue()
               << " from: " << errorPathStr;
    m_logger.debug(successMsg.str());

  } catch (const std::exception& ex) {
    m_logger.error(std::string("Error page serving failed: ") + ex.what());
    generateErrorResponse(statusCode, "Internal Server Error");
  }
}

bool ConnectionHandler::validateRequestBodySize(
    const domain::configuration::entities::LocationConfig& location) const {
  if (!m_request.hasBody()) {
    return true;
  }

  const domain::http::entities::HttpRequest::Body& body = m_request.getBody();
  const domain::filesystem::value_objects::Size& maxSize =
      location.getClientMaxBodySize();

  return body.size() <= maxSize.getBytes();
}

domain::filesystem::value_objects::Path ConnectionHandler::tryFindFile(
    const domain::configuration::entities::LocationConfig& location,
    const domain::filesystem::value_objects::Path& requestPath) const {
  const domain::configuration::entities::LocationConfig::TryFiles& tryFiles =
      location.getTryFiles();

  const std::string requestPathStr = requestPath.toString();

  std::ostringstream debugStart;
  debugStart << "try_files: starting with " << tryFiles.size()
             << " patterns for path: " << requestPathStr;
  m_logger.debug(debugStart.str());

  for (domain::configuration::entities::LocationConfig::TryFiles::const_iterator
           it = tryFiles.begin();
       it != tryFiles.end(); ++it) {
    std::string tryFilePattern = *it;

    if (!tryFilePattern.empty() && tryFilePattern[0] == '=') {
      std::ostringstream statusMsg;
      statusMsg << "try_files: reached status code pattern '" << tryFilePattern
                << "', returning empty path to trigger error handling";
      m_logger.debug(statusMsg.str());
      return domain::filesystem::value_objects::Path();
    }

    std::string substitutedPattern = tryFilePattern;
    std::size_t uriPos = substitutedPattern.find("$uri");
    while (uriPos != std::string::npos) {
      substitutedPattern.replace(uriPos, 4, requestPathStr);
      uriPos =
          substitutedPattern.find("$uri", uriPos + requestPathStr.length());
    }

    std::ostringstream patternMsg;
    patternMsg << "try_files: processing pattern '" << tryFilePattern
               << "' -> '" << substitutedPattern << "'";
    m_logger.debug(patternMsg.str());

    try {
      domain::filesystem::value_objects::Path tryPath =
          resolvePathWithServerFallback(location, substitutedPattern);

      std::ostringstream debugMsg;
      debugMsg << "try_files: resolved to '" << tryPath.toString() << "'";
      m_logger.debug(debugMsg.str());

      if (filesystem::adapters::FileSystemHelper::exists(tryPath.toString())) {
        std::ostringstream foundMsg;
        foundMsg << "try_files: FOUND file at '" << tryPath.toString() << "'";
        m_logger.debug(foundMsg.str());
        return tryPath;
      } else {
        std::ostringstream notFoundMsg;
        notFoundMsg << "try_files: file does not exist: '" << tryPath.toString()
                    << "'";
        m_logger.debug(notFoundMsg.str());
      }
    } catch (const std::exception& ex) {
      std::ostringstream errMsg;
      errMsg << "try_files: exception while resolving '" << substitutedPattern
             << "': " << ex.what();
      m_logger.debug(errMsg.str());
    }
  }

  m_logger.debug("try_files: exhausted all patterns, returning empty path");
  return domain::filesystem::value_objects::Path();
}

void ConnectionHandler::buildHttpResponseFromCgi(
    const cgi::primitives::CgiResponse& cgiResponse) {
  domain::shared::value_objects::ErrorCode statusCode =
      domain::shared::value_objects::ErrorCode::ok();

  if (cgiResponse.hasStatus()) {
    statusCode = cgiResponse.getStatus();
  }

  m_response = domain::http::entities::HttpResponse(
      statusCode,
      std::string(cgiResponse.getBody().begin(), cgiResponse.getBody().end()));

  const std::map<std::string, std::string>& cgiHeaders =
      cgiResponse.getHeaders();
  for (std::map<std::string, std::string>::const_iterator it =
           cgiHeaders.begin();
       it != cgiHeaders.end(); ++it) {
    m_response.addHeader(it->first, it->second);
  }

  if (!cgiResponse.getContentType().empty()) {
    m_response.setContentType(cgiResponse.getContentType());
  } else {
    m_response.setContentType("text/html");
  }

  if (cgiResponse.hasLocation()) {
    m_response.addHeader("Location", cgiResponse.getLocation());
  }
}

void ConnectionHandler::applyCustomHeaders(
    const domain::configuration::entities::LocationConfig& location) {
  if (!location.hasCustomHeaders()) {
    return;
  }

  const domain::configuration::entities::LocationConfig::CustomHeaderMap&
      customHeaders = location.getCustomHeaders();

  for (domain::configuration::entities::LocationConfig::CustomHeaderMap::
           const_iterator it = customHeaders.begin();
       it != customHeaders.end(); ++it) {
    m_response.addHeader(it->first, it->second);
  }
}

bool ConnectionHandler::shouldKeepAlive() const {
  return m_request.isKeepAlive();
}

void ConnectionHandler::resetForNextRequest() {
  m_requestBuffer.clear();
  m_request = domain::http::entities::HttpRequest();
  m_response = domain::http::entities::HttpResponse();
  m_responseBuffer.clear();
  m_responseOffset = 0;
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

void ConnectionHandler::logRequest(
    const domain::http::entities::HttpRequest& request,
    const domain::http::entities::HttpResponse& response) {
  std::ostringstream oss;
  oss << getRemoteAddress() << " - \"" << request.getMethod().toString() << " "
      << request.getPath().toString() << " " << request.getVersion().toString()
      << "\" " << response.getStatusCode().getValue();
  m_logger.info(oss.str());
}

}  // namespace adapters
}  // namespace network
}  // namespace infrastructure
