/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParser.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/24 20:53:07 by dande-je          #+#    #+#             */
/*   Updated: 2026/01/06 19:53:13 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "domain/filesystem/value_objects/Path.hpp"
#include "domain/http/value_objects/HttpMethod.hpp"
#include "domain/http/value_objects/QueryStringBuilder.hpp"
#include "infrastructure/http/RequestParser.hpp"
#include "infrastructure/http/RequestParserException.hpp"

#include <algorithm>
#include <cctype>
#include <sstream>

// TODO: refactor this class
namespace infrastructure {
namespace http {

using shared::exceptions::RequestParserException;

ParsedRequest::ParsedRequest()
    : method(domain::http::value_objects::HttpMethod::METHOD_UNKNOWN),
      path(domain::filesystem::value_objects::Path::fromString("/", true)),
      httpVersion("HTTP/1.1") {}

bool ParsedRequest::isComplete() const {
  return !hasError() &&
         method.getMethod() !=
             domain::http::value_objects::HttpMethod::METHOD_UNKNOWN;
}

bool ParsedRequest::hasError() const {
  return method.getMethod() ==
             domain::http::value_objects::HttpMethod::METHOD_UNKNOWN ||
         path.isEmpty();
}

std::string ParsedRequest::getHeader(const std::string& name) const {
  std::string lowerName = name;
  std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(),
                 ::tolower);

  std::map<std::string, std::string>::const_iterator it =
      headers.find(lowerName);
  if (it != headers.end()) {
    return it->second;
  }
  return "";
}

bool ParsedRequest::hasHeader(const std::string& name) const {
  std::string lowerName = name;
  std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(),
                 ::tolower);
  return headers.find(lowerName) != headers.end();
}

std::size_t ParsedRequest::getContentLength() const {
  std::string contentLength = getHeader("content-length");
  if (contentLength.empty()) {
    return 0;
  }

  std::istringstream iss(contentLength);
  std::size_t length;
  if (iss >> length) {
    return length;
  }
  return 0;
}

bool ParsedRequest::isChunked() const {
  std::string transferEncoding = getHeader("transfer-encoding");
  std::transform(transferEncoding.begin(), transferEncoding.end(),
                 transferEncoding.begin(), ::tolower);
  return transferEncoding.find("chunked") != std::string::npos;
}

RequestParser::RequestParser()
    : m_maxHeaderSize(8192),
      m_maxBodySize(10 * 1024 * 1024),
      m_state(START_LINE),
      m_bodyBytesRead(0) {
  reset();
}

RequestParser::RequestParser(std::size_t maxHeaderSize, std::size_t maxBodySize)
    : m_maxHeaderSize(maxHeaderSize),
      m_maxBodySize(maxBodySize),
      m_state(START_LINE),
      m_bodyBytesRead(0) {
  reset();
}

bool RequestParser::parse(const char* data, std::size_t length) {
  if (length == 0) {
    return false;
  }

  if (m_state == ERROR) {
    return false;
  }

  m_buffer.append(data, length);

  try {
    bool progress = false;
    while (true) {
      switch (m_state) {
        case START_LINE:
          progress = parseStartLine();
          if (!progress) return false;
          m_state = HEADERS;
          break;

        case HEADERS:
          progress = parseHeaders();
          if (!progress) return false;
          if (m_request.isChunked()) {
            m_state = CHUNKED_BODY;
          } else if (m_request.getContentLength() > 0) {
            m_state = BODY;
          } else {
            m_state = COMPLETE;
            return true;
          }
          break;

        case BODY:
          progress = parseBody();
          if (!progress) return false;
          if (m_state == COMPLETE) return true;
          break;

        case CHUNKED_BODY:
          progress = parseChunkedBody();
          if (!progress) return false;
          if (m_state == COMPLETE) return true;
          break;

        case COMPLETE:
          return true;

        case ERROR:
          return false;
      }
    }
  } catch (const std::exception& e) {
    m_state = ERROR;
    return false;
  }

  return false;
}

bool RequestParser::parse(const std::vector<char>& data) {
  return parse(&data[0], data.size());
}

const ParsedRequest& RequestParser::getRequest() const { return m_request; }

bool RequestParser::isComplete() const { return m_state == COMPLETE; }

bool RequestParser::hasError() const { return m_state == ERROR; }

void RequestParser::reset() {
  m_request = ParsedRequest();
  m_buffer.clear();
  m_state = START_LINE;
  m_bodyBytesRead = 0;
}

void RequestParser::setMaxHeaderSize(std::size_t size) {
  m_maxHeaderSize = size;
}

void RequestParser::setMaxBodySize(std::size_t size) { m_maxBodySize = size; }

std::size_t RequestParser::getMaxHeaderSize() const { return m_maxHeaderSize; }

std::size_t RequestParser::getMaxBodySize() const { return m_maxBodySize; }

bool RequestParser::parseStartLine() {
  std::size_t lineEnd = findLineEnd();
  if (lineEnd == std::string::npos) {
    return false;
  }

  std::string line = m_buffer.substr(0, lineEnd);
  m_buffer.erase(0, lineEnd + 2);

  processStartLine(line);
  return true;
}

bool RequestParser::parseHeaders() {
  while (true) {
    std::size_t lineEnd = findLineEnd();
    if (lineEnd == std::string::npos) {
      return false;
    }

    std::string line = m_buffer.substr(0, lineEnd);
    m_buffer.erase(0, lineEnd + 2);

    if (line.empty()) {
      break;
    }

    processHeaderLine(line);

    if (m_buffer.size() > m_maxHeaderSize) {
      std::ostringstream oss;
      oss << "Headers size " << m_buffer.size() << " exceeds maximum of "
          << m_maxHeaderSize;
      throw RequestParserException(oss.str(),
                                   RequestParserException::HEADER_TOO_LARGE);
    }
  }

  return true;
}

bool RequestParser::parseBody() {
  std::size_t contentLength = m_request.getContentLength();

  if (contentLength == 0) {
    m_state = COMPLETE;
    return true;
  }

  if (contentLength > m_maxBodySize) {
    std::ostringstream oss;
    oss << "Content-Length " << contentLength
        << " exceeds maximum body size of " << m_maxBodySize;
    throw RequestParserException(oss.str(),
                                 RequestParserException::BODY_TOO_LARGE);
  }

  std::size_t bytesNeeded = contentLength - m_bodyBytesRead;

  if (bytesNeeded == 0) {
    m_state = COMPLETE;
    return true;
  }

  std::size_t bytesToRead = std::min(bytesNeeded, m_buffer.size());
  m_request.body.insert(m_request.body.end(), m_buffer.begin(),
                        m_buffer.begin() + bytesToRead);
  m_bodyBytesRead += bytesToRead;
  m_buffer.erase(0, bytesToRead);

  if (m_bodyBytesRead >= contentLength) {
    m_state = COMPLETE;
    return true;
  }

  return false;
}

bool RequestParser::parseChunkedBody() {
  std::size_t endPos = m_buffer.find("0\r\n\r\n");
  if (endPos == std::string::npos) {
    return false;
  }

  m_buffer.erase(0, endPos + 5);
  m_state = COMPLETE;
  return true;
}

std::size_t RequestParser::findLineEnd() const {
  for (std::size_t i = 0; i + 1 < m_buffer.size(); ++i) {
    if (m_buffer[i] == '\r' && m_buffer[i + 1] == '\n') {
      return i;
    }
  }
  return std::string::npos;
}

std::string RequestParser::extractLine() {
  std::size_t lineEnd = findLineEnd();
  if (lineEnd == std::string::npos) {
    return "";
  }

  std::string line = m_buffer.substr(0, lineEnd);
  m_buffer.erase(0, lineEnd + 2);
  return line;
}

void RequestParser::processStartLine(const std::string& line) {
  std::istringstream iss(line);
  std::string methodStr, uri, version;

  if (!(iss >> methodStr >> uri >> version)) {
    throw RequestParserException("Invalid start line: " + line,
                                 RequestParserException::MALFORMED_REQUEST);
  }

  if (!validateMethod(methodStr)) {
    throw RequestParserException("Unsupported HTTP method: " + methodStr,
                                 RequestParserException::UNSUPPORTED_METHOD);
  }
  m_request.method =
      domain::http::value_objects::HttpMethod::fromString(methodStr);

  std::size_t queryPos = uri.find('?');
  std::string pathStr;
  if (queryPos != std::string::npos) {
    pathStr = uri.substr(0, queryPos);
    std::string queryStr = uri.substr(queryPos + 1);
    m_request.query =
        domain::http::value_objects::QueryStringBuilder::parseQueryString(
            queryStr);
  } else {
    pathStr = uri;
  }

  if (pathStr.length() > 8192) {
    std::ostringstream oss;
    oss << "URI length " << pathStr.length()
        << " exceeds maximum of 8192 characters";
    throw RequestParserException(oss.str(),
                                 RequestParserException::URI_TOO_LONG);
  }

  // Validate and parse path
  if (!validatePath(pathStr)) {
    std::ostringstream oss;
    oss << "Invalid path format: " << pathStr;
    throw RequestParserException(oss.str(),
                                 RequestParserException::MALFORMED_REQUEST);
  }

  try {
    m_request.path =
        domain::filesystem::value_objects::Path::fromString(pathStr, true);
  } catch (const std::exception& e) {
    std::ostringstream oss;
    oss << "Invalid path: " << e.what();
    throw RequestParserException(oss.str(),
                                 RequestParserException::MALFORMED_REQUEST);
  }

  if (!validateHttpVersion(version)) {
    throw RequestParserException(
        "Invalid or unsupported HTTP version: " + version,
        RequestParserException::INVALID_HTTP_VERSION);
  }
  m_request.httpVersion = version;
}

void RequestParser::processHeaderLine(const std::string& line) {
  std::size_t colonPos = line.find(':');
  if (colonPos == std::string::npos) {
    throw RequestParserException("Invalid header line (missing colon): " + line,
                                 RequestParserException::INVALID_HEADER);
  }

  std::string name = line.substr(0, colonPos);
  std::string value = line.substr(colonPos + 1);

  name.erase(0, name.find_first_not_of(" \t"));
  name.erase(name.find_last_not_of(" \t") + 1);
  value.erase(0, value.find_first_not_of(" \t"));
  value.erase(value.find_last_not_of(" \t") + 1);

  if (name.empty()) {
    throw RequestParserException("Empty header name",
                                 RequestParserException::INVALID_HEADER);
  }

  std::transform(name.begin(), name.end(), name.begin(), ::tolower);

  m_request.headers[name] = value;
}

bool RequestParser::validateMethod(const std::string& method) const {
  return domain::http::value_objects::HttpMethod::isValidMethodString(method);
}

bool RequestParser::validatePath(const std::string& path) const {
  if (path.empty()) {
    return false;
  }

  if (path[0] != '/') {
    return false;
  }

  for (std::size_t i = 0; i < path.length(); ++i) {
    if (iscntrl(static_cast<unsigned char>(path[i]))) {
      return false;
    }
  }

  return true;
}

bool RequestParser::validateHttpVersion(const std::string& version) const {
  if (version.find("HTTP/") != 0) {
    return false;
  }

  std::string verNum = version.substr(5);
  if (verNum == "1.0" || verNum == "1.1") {
    return true;
  }

  return false;
}

}  // namespace http
}  // namespace infrastructure
