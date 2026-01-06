/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/04 15:12:01 by dande-je          #+#    #+#             */
/*   Updated: 2026/01/06 02:01:02 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "domain/http/entities/HttpResponse.hpp"
#include "domain/http/exceptions/HttpResponseException.hpp"

#include <ctime>
#include <sstream>

namespace domain {
namespace http {
namespace entities {

const std::string HttpResponse::CRLF = "\r\n";
const std::string HttpResponse::SERVER_NAME = "webserv/1.0";
const std::string HttpResponse::DEFAULT_CONTENT_TYPE = "text/html";

HttpResponse::HttpResponse()
    : m_statusCode(shared::value_objects::ErrorCode::ok()),
      m_version(value_objects::HttpVersion::http11()) {}

HttpResponse::HttpResponse(const shared::value_objects::ErrorCode& statusCode)
    : m_statusCode(statusCode),
      m_version(value_objects::HttpVersion::http11()) {}

HttpResponse::HttpResponse(const shared::value_objects::ErrorCode& statusCode,
                           const std::string& body)
    : m_statusCode(statusCode),
      m_version(value_objects::HttpVersion::http11()) {
  setBody(body);
}

HttpResponse::HttpResponse(const HttpResponse& other)
    : m_statusCode(other.m_statusCode),
      m_version(other.m_version),
      m_headers(other.m_headers),
      m_body(other.m_body) {}

HttpResponse::~HttpResponse() {}

HttpResponse& HttpResponse::operator=(const HttpResponse& other) {
  if (this != &other) {
    m_statusCode = other.m_statusCode;
    m_version = other.m_version;
    m_headers = other.m_headers;
    m_body = other.m_body;
  }
  return *this;
}

shared::value_objects::ErrorCode HttpResponse::getStatusCode() const {
  return m_statusCode;
}

value_objects::HttpVersion HttpResponse::getVersion() const {
  return m_version;
}

const HttpResponse::HeaderMap& HttpResponse::getHeaders() const {
  return m_headers;
}

const HttpResponse::Body& HttpResponse::getBody() const { return m_body; }

std::string HttpResponse::getBodyAsString() const {
  return std::string(m_body.begin(), m_body.end());
}

void HttpResponse::setStatusCode(
    const shared::value_objects::ErrorCode& statusCode) {
  m_statusCode = statusCode;
}

void HttpResponse::setVersion(const value_objects::HttpVersion& version) {
  m_version = version;
}

void HttpResponse::setBody(const std::string& body) {
  m_body.assign(body.begin(), body.end());
  updateContentLength();
}

void HttpResponse::setBody(const Body& body) {
  m_body = body;
  updateContentLength();
}

void HttpResponse::appendBody(const std::string& data) {
  m_body.insert(m_body.end(), data.begin(), data.end());
  updateContentLength();
}

void HttpResponse::appendBody(const Body& data) {
  m_body.insert(m_body.end(), data.begin(), data.end());
  updateContentLength();
}

void HttpResponse::clearBody() {
  m_body.clear();
  updateContentLength();
}

std::string HttpResponse::getHeader(const std::string& name) const {
  const std::string normalizedName = normalizeHeaderName(name);
  HeaderMap::const_iterator iter = m_headers.find(normalizedName);
  if (iter != m_headers.end()) {
    return iter->second;
  }
  return "";
}

bool HttpResponse::hasHeader(const std::string& name) const {
  const std::string normalizedName = normalizeHeaderName(name);
  return m_headers.find(normalizedName) != m_headers.end();
}

void HttpResponse::setHeader(const std::string& name,
                             const std::string& value) {
  const std::string normalizedName = normalizeHeaderName(name);
  m_headers[normalizedName] = value;
}

void HttpResponse::addHeader(const std::string& name,
                             const std::string& value) {
  setHeader(name, value);
}

void HttpResponse::removeHeader(const std::string& name) {
  const std::string normalizedName = normalizeHeaderName(name);
  m_headers.erase(normalizedName);
}

void HttpResponse::clearHeaders() { m_headers.clear(); }

void HttpResponse::setContentType(const std::string& contentType) {
  setHeader("Content-Type", contentType);
}

void HttpResponse::setContentLength(std::size_t length) {
  std::ostringstream oss;
  oss << length;
  setHeader("Content-Length", oss.str());
}

void HttpResponse::setConnection(const std::string& connection) {
  setHeader("Connection", connection);
}

void HttpResponse::setServer(const std::string& serverName) {
  setHeader("Server", serverName);
}

void HttpResponse::setDate() { setHeader("Date", getCurrentDateTime()); }

void HttpResponse::setLocation(const std::string& location) {
  setHeader("Location", location);
}

std::string HttpResponse::getContentType() const {
  return getHeader("Content-Type");
}

std::size_t HttpResponse::getContentLength() const {
  const std::string contentLength = getHeader("Content-Length");
  if (contentLength.empty()) {
    return 0;
  }

  std::istringstream iss(contentLength);
  std::size_t length = 0;
  if ((iss >> length) != 0) {
    return length;
  }
  return 0;
}

std::string HttpResponse::getConnection() const {
  return getHeader("Connection");
}

std::string HttpResponse::serialize() const {
  std::ostringstream response;

  response << buildStatusLine();
  response << buildHeaders();
  response << CRLF;

  if (!m_body.empty()) {
    response.write(&m_body[0], static_cast<std::streamsize>(m_body.size()));
  }

  return response.str();
}

std::string HttpResponse::buildStatusLine() const {
  std::ostringstream statusLine;
  statusLine << m_version.toString() << " " << m_statusCode.toStatusLine()
             << CRLF;
  return statusLine.str();
}

std::string HttpResponse::buildHeaders() const {
  std::ostringstream headers;

  for (HeaderMap::const_iterator it = m_headers.begin(); it != m_headers.end();
       ++it) {
    headers << it->first << ": " << it->second << CRLF;
  }

  return headers.str();
}

bool HttpResponse::isValid() const {
  try {
    validate();
    return true;
  } catch (const exceptions::HttpResponseException&) {
    return false;
  }
}

void HttpResponse::validate() const {
  if (m_body.size() > DEFAULT_MAX_BODY_SIZE) {
    std::ostringstream oss;
    oss << "Response body size (" << m_body.size()
        << ") exceeds maximum allowed (" << DEFAULT_MAX_BODY_SIZE << ")";
    throw exceptions::HttpResponseException(
        oss.str(), exceptions::HttpResponseException::BODY_TOO_LARGE);
  }

  if (hasHeader("Content-Length")) {
    const std::size_t headerLength = getContentLength();
    if (headerLength != m_body.size()) {
      std::ostringstream oss;
      oss << "Content-Length (" << headerLength
          << ") does not match body size (" << m_body.size() << ")";
      throw exceptions::HttpResponseException(
          oss.str(), exceptions::HttpResponseException::INVALID_CONTENT_LENGTH);
    }
  }
}

bool HttpResponse::operator==(const HttpResponse& other) const {
  return m_statusCode == other.m_statusCode && m_version == other.m_version &&
         m_headers == other.m_headers && m_body == other.m_body;
}

bool HttpResponse::operator!=(const HttpResponse& other) const {
  return !(*this == other);
}

HttpResponse HttpResponse::ok(const std::string& body) {
  HttpResponse response(shared::value_objects::ErrorCode::ok());
  response.setBody(body.empty() ? "<html><body><h1>200 OK</h1></body></html>"
                                : body);
  response.ensureDefaultHeaders();
  return response;
}

HttpResponse HttpResponse::created(const std::string& location) {
  HttpResponse response(shared::value_objects::ErrorCode::created());
  if (!location.empty()) {
    response.setLocation(location);
  }
  response.ensureDefaultHeaders();
  return response;
}

HttpResponse HttpResponse::noContent() {
  HttpResponse response(shared::value_objects::ErrorCode::noContent());
  response.ensureDefaultHeaders();
  return response;
}

HttpResponse HttpResponse::movedPermanently(const std::string& location) {
  HttpResponse response(shared::value_objects::ErrorCode::movedPermanently());
  response.setLocation(location);
  response.ensureDefaultHeaders();
  return response;
}

HttpResponse HttpResponse::found(const std::string& location) {
  HttpResponse response(shared::value_objects::ErrorCode::found());
  response.setLocation(location);
  response.ensureDefaultHeaders();
  return response;
}

HttpResponse HttpResponse::notModified() {
  HttpResponse response(shared::value_objects::ErrorCode::notModified());
  response.ensureDefaultHeaders();
  return response;
}

HttpResponse HttpResponse::badRequest(const std::string& message) {
  HttpResponse response(shared::value_objects::ErrorCode::badRequest());
  response.setBody(response.buildDefaultErrorBody(message));
  response.ensureDefaultHeaders();
  return response;
}

HttpResponse HttpResponse::unauthorized(const std::string& message) {
  HttpResponse response(shared::value_objects::ErrorCode::unauthorized());
  response.setBody(response.buildDefaultErrorBody(message));
  response.ensureDefaultHeaders();
  return response;
}

HttpResponse HttpResponse::forbidden(const std::string& message) {
  HttpResponse response(shared::value_objects::ErrorCode::forbidden());
  response.setBody(response.buildDefaultErrorBody(message));
  response.ensureDefaultHeaders();
  return response;
}

HttpResponse HttpResponse::notFound(const std::string& message) {
  HttpResponse response(shared::value_objects::ErrorCode::notFound());
  response.setBody(response.buildDefaultErrorBody(message));
  response.ensureDefaultHeaders();
  return response;
}

HttpResponse HttpResponse::methodNotAllowed(const std::string& message) {
  HttpResponse response(shared::value_objects::ErrorCode::methodNotAllowed());
  response.setBody(response.buildDefaultErrorBody(message));
  response.ensureDefaultHeaders();
  return response;
}

HttpResponse HttpResponse::payloadTooLarge(const std::string& message) {
  HttpResponse response(shared::value_objects::ErrorCode::payloadTooLarge());
  response.setBody(response.buildDefaultErrorBody(message));
  response.ensureDefaultHeaders();
  return response;
}

HttpResponse HttpResponse::internalServerError(const std::string& message) {
  HttpResponse response(
      shared::value_objects::ErrorCode::internalServerError());
  response.setBody(response.buildDefaultErrorBody(message));
  response.ensureDefaultHeaders();
  return response;
}

HttpResponse HttpResponse::notImplemented(const std::string& message) {
  HttpResponse response((shared::value_objects::ErrorCode(
      shared::value_objects::ErrorCode::STATUS_NOT_IMPLEMENTED)));
  response.setBody(response.buildDefaultErrorBody(message));
  response.ensureDefaultHeaders();
  return response;
}

HttpResponse HttpResponse::serviceUnavailable(const std::string& message) {
  HttpResponse response(shared::value_objects::ErrorCode::serviceUnavailable());
  response.setBody(response.buildDefaultErrorBody(message));
  response.ensureDefaultHeaders();
  return response;
}

std::string HttpResponse::normalizeHeaderName(const std::string& name) {
  std::string normalized;
  normalized.reserve(name.size());

  for (std::size_t i = 0; i < name.size(); ++i) {
    normalized += toLowerCase(name[i]);
  }

  return normalized;
}

char HttpResponse::toLowerCase(char chr) {
  if (chr >= 'A' && chr <= 'Z') {
    return static_cast<char>(static_cast<int>(chr) + ('a' - 'A'));
  }
  return chr;
}

// TODO: maybe create a TimeUtils
std::string HttpResponse::getCurrentDateTime() {
  std::time_t now = std::time(NULL);
  char buffer[100];
  std::strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S GMT",
                std::gmtime(&now));
  return std::string(buffer);
}

void HttpResponse::updateContentLength() { setContentLength(m_body.size()); }

void HttpResponse::ensureDefaultHeaders() {
  if (!hasHeader("Content-Type")) {
    setContentType(DEFAULT_CONTENT_TYPE);
  }

  if (!hasHeader("Server")) {
    setServer(SERVER_NAME);
  }

  if (!hasHeader("Date")) {
    setDate();
  }

  updateContentLength();
}

std::string HttpResponse::buildDefaultErrorBody(
    const std::string& message) const {
  std::ostringstream body;
  body << "<html><head><title>" << m_statusCode.toStatusLine()
       << "</title></head>";
  body << "<body><h1>" << m_statusCode.toStatusLine() << "</h1>";
  if (!message.empty()) {
    body << "<p>" << message << "</p>";
  }
  body << "<hr><p><em>" << SERVER_NAME << "</em></p>";
  body << "</body></html>";
  return body.str();
}

}  // namespace entities
}  // namespace http
}  // namespace domain
