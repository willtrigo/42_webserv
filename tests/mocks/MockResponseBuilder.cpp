/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MockResponseBuilder.cpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: umeneses <umeneses@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/27 13:00:00 by umeneses          #+#    #+#             */
/*   Updated: 2025/12/27 14:02:57 by umeneses         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "MockResponseBuilder.hpp"
#include <sstream>

namespace mocks {

MockResponseBuilder::MockResponseBuilder() 
  : m_statusCode(200),
    m_statusMessage("OK"),
    m_httpVersion("HTTP/1.1"),
    m_body("") {}

MockResponseBuilder::~MockResponseBuilder() {}

// ============================================================================
// Status Line
// ============================================================================

void MockResponseBuilder::setStatus(int code, const std::string& message) {
  m_statusCode = code;
  m_statusMessage = message;
}

void MockResponseBuilder::setStatus(int code) {
  m_statusCode = code;
  m_statusMessage = getDefaultMessage(code);
}

int MockResponseBuilder::getStatusCode() const {
  return m_statusCode;
}

std::string MockResponseBuilder::getStatusMessage() const {
  return m_statusMessage;
}

void MockResponseBuilder::setHttpVersion(const std::string& version) {
  m_httpVersion = version;
}

std::string MockResponseBuilder::getHttpVersion() const {
  return m_httpVersion;
}

// ============================================================================
// Headers
// ============================================================================

void MockResponseBuilder::addHeader(const std::string& key, const std::string& value) {
  m_headers[key] = value;
}

std::string MockResponseBuilder::getHeader(const std::string& key) const {
  std::map<std::string, std::string>::const_iterator it = m_headers.find(key);
  if (it != m_headers.end()) {
    return it->second;
  }
  return "";
}

bool MockResponseBuilder::hasHeader(const std::string& key) const {
  return m_headers.find(key) != m_headers.end();
}

std::map<std::string, std::string> MockResponseBuilder::getAllHeaders() const {
  return m_headers;
}

void MockResponseBuilder::removeHeader(const std::string& key) {
  m_headers.erase(key);
}

void MockResponseBuilder::clearHeaders() {
  m_headers.clear();
}

// ============================================================================
// Body
// ============================================================================

void MockResponseBuilder::setBody(const std::string& body) {
  m_body = body;
  updateContentLength();
}

void MockResponseBuilder::appendBody(const std::string& data) {
  m_body += data;
  updateContentLength();
}

std::string MockResponseBuilder::getBody() const {
  return m_body;
}

std::size_t MockResponseBuilder::getBodySize() const {
  return m_body.length();
}

bool MockResponseBuilder::hasBody() const {
  return !m_body.empty();
}

void MockResponseBuilder::clearBody() {
  m_body = "";
  updateContentLength();
}

// ============================================================================
// Response Building
// ============================================================================

std::string MockResponseBuilder::build() const {
  std::ostringstream oss;
  
  oss << m_httpVersion << " " << m_statusCode << " " 
      << m_statusMessage << "\r\n";
  
  for (std::map<std::string, std::string>::const_iterator it = m_headers.begin();
       it != m_headers.end(); ++it) {
    oss << it->first << ": " << it->second << "\r\n";
  }
  
  oss << "\r\n";
  
  if (!m_body.empty()) {
    oss << m_body;
  }
  
  return oss.str();
}

std::string MockResponseBuilder::buildStatusLine() const {
  std::ostringstream oss;
  oss << m_httpVersion << " " << m_statusCode << " " << m_statusMessage;
  return oss.str();
}

// ============================================================================
// Reset
// ============================================================================

void MockResponseBuilder::reset() {
  m_statusCode = 200;
  m_statusMessage = "OK";
  m_httpVersion = "HTTP/1.1";
  m_headers.clear();
  m_body = "";
}

// ============================================================================
// Convenience Methods
// ============================================================================

void MockResponseBuilder::buildOkResponse(const std::string& body, const std::string& contentType) {
  setStatus(200, "OK");
  addHeader("Content-Type", contentType);
  setBody(body);
}

void MockResponseBuilder::buildNotFoundResponse(const std::string& message) {
  setStatus(404, "Not Found");
  addHeader("Content-Type", "text/html");
  setBody("<html><body><h1>404 Not Found</h1><p>" + message + "</p></body></html>");
}

void MockResponseBuilder::buildServerErrorResponse(const std::string& message) {
  setStatus(500, "Internal Server Error");
  addHeader("Content-Type", "text/html");
  setBody("<html><body><h1>500 Server Error</h1><p>" + message + "</p></body></html>");
}

void MockResponseBuilder::buildBadRequestResponse(const std::string& message) {
  setStatus(400, "Bad Request");
  addHeader("Content-Type", "text/html");
  setBody("<html><body><h1>400 Bad Request</h1><p>" + message + "</p></body></html>");
}

// ============================================================================
// Test Helpers
// ============================================================================

bool MockResponseBuilder::isSuccessResponse() const {
  return m_statusCode >= 200 && m_statusCode < 300;
}

bool MockResponseBuilder::isErrorResponse() const {
  return m_statusCode >= 400;
}

bool MockResponseBuilder::isRedirectResponse() const {
  return m_statusCode >= 300 && m_statusCode < 400;
}

// ============================================================================
// Private Methods
// ============================================================================

void MockResponseBuilder::updateContentLength() {
  std::ostringstream oss;
  oss << m_body.length();
  addHeader("Content-Length", oss.str());
}

std::string MockResponseBuilder::getDefaultMessage(int code) const {
  switch (code) {
    case 200: return "OK";
    case 201: return "Created";
    case 204: return "No Content";
    case 301: return "Moved Permanently";
    case 302: return "Found";
    case 400: return "Bad Request";
    case 403: return "Forbidden";
    case 404: return "Not Found";
    case 405: return "Method Not Allowed";
    case 500: return "Internal Server Error";
    case 501: return "Not Implemented";
    case 503: return "Service Unavailable";
    default: return "Unknown";
  }
}

}  // namespace mocks
