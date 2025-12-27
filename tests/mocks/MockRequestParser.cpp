/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MockRequestParser.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: umeneses <umeneses@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/27 13:01:59 by umeneses          #+#    #+#             */
/*   Updated: 2025/12/27 14:46:49 by umeneses         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "MockRequestParser.hpp"
#include <sstream>

namespace mocks {

MockRequestParser::MockRequestParser() 
  : m_method("GET"),
    m_uri("/"),
    m_httpVersion("HTTP/1.1"),
    m_isComplete(false),
    m_hasError(false),
    m_bodySize(0) {}

MockRequestParser::~MockRequestParser() {}

// ============================================================================
// Parsing Control
// ============================================================================

bool MockRequestParser::parse(const std::string& rawRequest) {
  m_rawData = rawRequest;
  if (!m_hasError) {
    m_isComplete = true;
  }
  return !m_hasError;
}

bool MockRequestParser::parse(const char* data, std::size_t length) {
  return parse(std::string(data, length));
}

void MockRequestParser::reset() {
  m_method = "GET";
  m_uri = "/";
  m_httpVersion = "HTTP/1.1";
  m_headers.clear();
  m_body.clear();
  m_rawData = "";
  m_isComplete = false;
  m_hasError = false;
  m_bodySize = 0;
}

// ============================================================================
// State Query
// ============================================================================

bool MockRequestParser::isComplete() const {
  return m_isComplete;
}

bool MockRequestParser::hasError() const {
  return m_hasError;
}

bool MockRequestParser::isValid() const {
  return !m_hasError && m_isComplete;
}

// ============================================================================
// Request Line
// ============================================================================

void MockRequestParser::setMethod(const std::string& method) {
  m_method = method;
}

std::string MockRequestParser::getMethod() const {
  return m_method;
}

void MockRequestParser::setUri(const std::string& uri) {
  m_uri = uri;
}

std::string MockRequestParser::getUri() const {
  return m_uri;
}

void MockRequestParser::setHttpVersion(const std::string& version) {
  m_httpVersion = version;
}

std::string MockRequestParser::getHttpVersion() const {
  return m_httpVersion;
}

// ============================================================================
// Headers
// ============================================================================

void MockRequestParser::addHeader(const std::string& key, const std::string& value) {
  m_headers[key] = value;
}

std::string MockRequestParser::getHeader(const std::string& key) const {
  std::map<std::string, std::string>::const_iterator it = m_headers.find(key);
  if (it != m_headers.end()) {
    return it->second;
  }
  return "";
}

bool MockRequestParser::hasHeader(const std::string& key) const {
  return m_headers.find(key) != m_headers.end();
}

std::map<std::string, std::string> MockRequestParser::getAllHeaders() const {
  return m_headers;
}

void MockRequestParser::clearHeaders() {
  m_headers.clear();
}

// ============================================================================
// Body
// ============================================================================

void MockRequestParser::setBody(const std::string& body) {
  m_body = body;
  m_bodySize = body.length();
  std::ostringstream oss;
  oss << m_bodySize;
  addHeader("Content-Length", oss.str());
}

std::string MockRequestParser::getBody() const {
  return m_body;
}

std::size_t MockRequestParser::getBodySize() const {
  return m_bodySize;
}

bool MockRequestParser::hasBody() const {
  return !m_body.empty();
}

// ============================================================================
// Error Simulation
// ============================================================================

void MockRequestParser::setParseError(bool hasError) {
  m_hasError = hasError;
  if (hasError) {
    m_isComplete = false;
  }
}

void MockRequestParser::setComplete(bool complete) {
  m_isComplete = complete;
}

// ============================================================================
// Test Helpers
// ============================================================================

void MockRequestParser::setupGetRequest(const std::string& uri) {
  setMethod("GET");
  setUri(uri);
  setComplete(true);
}

void MockRequestParser::setupPostRequest(const std::string& uri, const std::string& body) {
  setMethod("POST");
  setUri(uri);
  setBody(body);
  addHeader("Content-Type", "application/x-www-form-urlencoded");
  setComplete(true);
}

void MockRequestParser::setupDeleteRequest(const std::string& uri) {
  setMethod("DELETE");
  setUri(uri);
  setComplete(true);
}

}  // namespace mocks
