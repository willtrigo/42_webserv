/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/04 02:03:54 by dande-je          #+#    #+#             */
/*   Updated: 2026/01/06 01:53:16 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpRequest.hpp"
#include "domain/http/exceptions/HttpRequestException.hpp"

#include <algorithm>
#include <sstream>

namespace domain {
namespace http {
namespace entities {

HttpRequest::HttpRequest()
    : m_method(value_objects::HttpMethod::get()),
      m_uri(value_objects::Uri::fromString("/")),
      m_path(filesystem::value_objects::Path::fromString("/", true)),
      m_version(value_objects::HttpVersion::http11()) {}

HttpRequest::HttpRequest(const HttpRequest& other)
    : m_method(other.m_method),
      m_uri(other.m_uri),
      m_path(other.m_path),
      m_query(other.m_query),
      m_version(other.m_version),
      m_headers(other.m_headers),
      m_body(other.m_body) {}

HttpRequest::~HttpRequest() {}

HttpRequest& HttpRequest::operator=(const HttpRequest& other) {
  if (this != &other) {
    m_method = other.m_method;
    m_uri = other.m_uri;
    m_path = other.m_path;
    m_query = other.m_query;
    m_version = other.m_version;
    m_headers = other.m_headers;
    m_body = other.m_body;
  }
  return *this;
}

value_objects::HttpMethod HttpRequest::getMethod() const { return m_method; }

value_objects::Uri HttpRequest::getUri() const { return m_uri; }

filesystem::value_objects::Path HttpRequest::getPath() const { return m_path; }

value_objects::QueryStringBuilder HttpRequest::getQuery() const {
  return m_query;
}

value_objects::HttpVersion HttpRequest::getVersion() const { return m_version; }

const HttpRequest::HeaderMap& HttpRequest::getHeaders() const {
  return m_headers;
}

const HttpRequest::Body& HttpRequest::getBody() const { return m_body; }

void HttpRequest::setMethod(const value_objects::HttpMethod& method) {
  m_method = method;
}

void HttpRequest::setUri(const value_objects::Uri& uri) { m_uri = uri; }

void HttpRequest::setPath(const filesystem::value_objects::Path& path) {
  m_path = path;
}

void HttpRequest::setQuery(const value_objects::QueryStringBuilder& query) {
  m_query = query;
}

void HttpRequest::setVersion(const value_objects::HttpVersion& version) {
  m_version = version;
}

void HttpRequest::setBody(const Body& body) { m_body = body; }

std::string HttpRequest::getHeader(const std::string& name) const {
  const std::string normalizedName = normalizeHeaderName(name);
  HeaderMap::const_iterator iter = m_headers.find(normalizedName);
  if (iter != m_headers.end()) {
    return iter->second;
  }
  return "";
}

bool HttpRequest::hasHeader(const std::string& name) const {
  const std::string normalizedName = normalizeHeaderName(name);
  return m_headers.find(normalizedName) != m_headers.end();
}

void HttpRequest::addHeader(const std::string& name, const std::string& value) {
  const std::string normalizedName = normalizeHeaderName(name);
  m_headers[normalizedName] = value;
}

void HttpRequest::removeHeader(const std::string& name) {
  const std::string normalizedName = normalizeHeaderName(name);
  m_headers.erase(normalizedName);
}

void HttpRequest::clearHeaders() { m_headers.clear(); }

std::size_t HttpRequest::getContentLength() const {
  const std::string contentLength = getHeader("content-length");
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

std::string HttpRequest::getContentType() const {
  return getHeader("content-type");
}

std::string HttpRequest::getHost() const { return getHeader("host"); }

std::string HttpRequest::getConnection() const {
  return getHeader("connection");
}

std::string HttpRequest::getTransferEncoding() const {
  return getHeader("transfer-encoding");
}

bool HttpRequest::isChunked() const {
  std::string transferEncoding = getTransferEncoding();
  std::transform(transferEncoding.begin(), transferEncoding.end(),
                 transferEncoding.begin(), toLowerCase);
  return transferEncoding.find("chunked") != std::string::npos;
}

bool HttpRequest::isKeepAlive() const {
  if (m_version.isHttp11()) {
    std::string connection = getConnection();
    std::transform(connection.begin(), connection.end(), connection.begin(),
                   toLowerCase);
    return connection != "close";
  }

  std::string connection = getConnection();
  std::transform(connection.begin(), connection.end(), connection.begin(),
                 toLowerCase);
  return connection == "keep-alive";
}

bool HttpRequest::hasBody() const { return !m_body.empty(); }

bool HttpRequest::isComplete() const {
  if (m_method.getMethod() == value_objects::HttpMethod::METHOD_UNKNOWN) {
    return false;
  }

  if (m_path.isEmpty()) {
    return false;
  }

  if (!m_version.isHttp10() && !m_version.isHttp11()) {
    return false;
  }

  if (getContentLength() > 0 && m_body.size() != getContentLength()) {
    return false;
  }

  return true;
}

bool HttpRequest::isValid() const {
  try {
    validate();
    return true;
  } catch (const exceptions::HttpRequestException&) {
    return false;
  }
}

void HttpRequest::validate() const {
  if (m_method.getMethod() == value_objects::HttpMethod::METHOD_UNKNOWN) {
    throw exceptions::HttpRequestException(
        "Request method is unknown or not set",
        exceptions::HttpRequestException::INVALID_METHOD);
  }

  if (m_path.isEmpty()) {
    throw exceptions::HttpRequestException(
        "Request path is empty", exceptions::HttpRequestException::INVALID_URI);
  }

  if (!m_version.isHttp10() && !m_version.isHttp11()) {
    throw exceptions::HttpRequestException(
        "Unsupported HTTP version: " + m_version.toString(),
        exceptions::HttpRequestException::INVALID_VERSION);
  }

  if (m_version.isHttp11()) {
    validateHostHeader();
  }

  validateContentLength();
  validateTransferEncoding();
}

bool HttpRequest::operator==(const HttpRequest& other) const {
  return m_method == other.m_method && m_uri == other.m_uri &&
         m_path == other.m_path && m_version == other.m_version &&
         m_headers == other.m_headers && m_body == other.m_body;
}

bool HttpRequest::operator!=(const HttpRequest& other) const {
  return !(*this == other);
}

std::string HttpRequest::normalizeHeaderName(const std::string& name) {
  std::string normalized;
  normalized.reserve(name.size());

  for (std::size_t i = 0; i < name.size(); ++i) {
    normalized += toLowerCase(name[i]);
  }

  return normalized;
}

bool HttpRequest::isWhitespace(char chr) { return chr == ' ' || chr == '\t'; }

char HttpRequest::toLowerCase(char chr) {
  if (chr >= 'A' && chr <= 'Z') {
    return static_cast<char>(static_cast<int>(chr) + ('a' - 'A'));
  }
  return chr;
}

void HttpRequest::validateContentLength() const {
  if (!hasHeader("content-length")) {
    return;
  }

  const std::size_t contentLength = getContentLength();
  if (contentLength == 0 && !getHeader("content-length").empty()) {
    const std::string value = getHeader("content-length");
    if (value != "0") {
      throw exceptions::HttpRequestException(
          "Invalid Content-Length value: '" + value + "'",
          exceptions::HttpRequestException::INVALID_CONTENT_LENGTH);
    }
  }

  if (hasBody() && m_body.size() != contentLength) {
    std::ostringstream oss;
    oss << "Body size (" << m_body.size() << ") does not match Content-Length ("
        << contentLength << ")";
    throw exceptions::HttpRequestException(
        oss.str(), exceptions::HttpRequestException::BODY_SIZE_MISMATCH);
  }

  if (contentLength > DEFAULT_MAX_BODY_SIZE) {
    std::ostringstream oss;
    oss << "Content-Length (" << contentLength << ") exceeds maximum allowed ("
        << DEFAULT_MAX_BODY_SIZE << ")";
    throw exceptions::HttpRequestException(
        oss.str(), exceptions::HttpRequestException::INVALID_CONTENT_LENGTH);
  }
}

void HttpRequest::validateHostHeader() const {
  if (!hasHeader("host")) {
    throw exceptions::HttpRequestException(
        "HTTP/1.1 requests must include Host header",
        exceptions::HttpRequestException::MISSING_REQUIRED_HEADER);
  }

  const std::string host = getHost();
  if (host.empty()) {
    throw exceptions::HttpRequestException(
        "Host header cannot be empty",
        exceptions::HttpRequestException::MISSING_REQUIRED_HEADER);
  }
}

void HttpRequest::validateTransferEncoding() const {
  if (!hasHeader("transfer-encoding")) {
    return;
  }

  std::string encoding = getTransferEncoding();
  std::transform(encoding.begin(), encoding.end(), encoding.begin(),
                 toLowerCase);

  if (encoding.find("chunked") == std::string::npos) {
    throw exceptions::HttpRequestException(
        "Unsupported Transfer-Encoding: '" + getTransferEncoding() + "'",
        exceptions::HttpRequestException::UNSUPPORTED_TRANSFER_ENCODING);
  }

  if (hasHeader("content-length")) {
    throw exceptions::HttpRequestException(
        "Request cannot have both Content-Length and Transfer-Encoding headers",
        exceptions::HttpRequestException::MALFORMED_REQUEST);
  }
}

}  // namespace entities
}  // namespace http
}  // namespace domain
