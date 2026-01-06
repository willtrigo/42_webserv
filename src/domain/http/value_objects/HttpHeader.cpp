/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpHeader.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/04 13:55:14 by dande-je          #+#    #+#             */
/*   Updated: 2026/01/04 13:58:17 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpHeader.hpp"
#include "domain/http/exceptions/HttpHeaderException.hpp"

#include <cctype>
#include <sstream>

namespace domain {
namespace http {
namespace value_objects {

const std::string HttpHeader::CONTENT_LENGTH = "content-length";
const std::string HttpHeader::CONTENT_TYPE = "content-type";
const std::string HttpHeader::HOST = "host";
const std::string HttpHeader::CONNECTION = "connection";
const std::string HttpHeader::TRANSFER_ENCODING = "transfer-encoding";
const std::string HttpHeader::ACCEPT = "accept";
const std::string HttpHeader::USER_AGENT = "user-agent";
const std::string HttpHeader::SERVER = "server";
const std::string HttpHeader::DATE = "date";
const std::string HttpHeader::LOCATION = "location";
const std::string HttpHeader::CACHE_CONTROL = "cache-control";
const std::string HttpHeader::SET_COOKIE = "set-cookie";
const std::string HttpHeader::COOKIE = "cookie";

const std::string HttpHeader::CONNECTION_CLOSE = "close";
const std::string HttpHeader::CONNECTION_KEEP_ALIVE = "keep-alive";
const std::string HttpHeader::TRANSFER_ENCODING_CHUNKED = "chunked";

HttpHeader::HttpHeader() {}

HttpHeader::HttpHeader(const std::string& name, const std::string& value)
    : m_name(name), m_value(value) {
  validate();
  updateNormalizedName();
}

HttpHeader::HttpHeader(const HttpHeader& other)
    : m_name(other.m_name),
      m_value(other.m_value),
      m_normalizedName(other.m_normalizedName) {}

HttpHeader::~HttpHeader() {}

HttpHeader& HttpHeader::operator=(const HttpHeader& other) {
  if (this != &other) {
    m_name = other.m_name;
    m_value = other.m_value;
    m_normalizedName = other.m_normalizedName;
  }
  return *this;
}

std::string HttpHeader::getName() const { return m_name; }

std::string HttpHeader::getValue() const { return m_value; }

std::string HttpHeader::getNormalizedName() const { return m_normalizedName; }

std::string HttpHeader::toString() const {
  std::ostringstream oss;
  oss << m_name << ": " << m_value;
  return oss.str();
}

bool HttpHeader::isValidHeaderName(const std::string& name) {
  if (name.empty() || name.length() > MAX_HEADER_NAME_LENGTH) {
    return false;
  }

  for (std::size_t i = 0; i < name.length(); ++i) {
    if (!isValidNameCharacter(name[i])) {
      return false;
    }
  }

  return true;
}

bool HttpHeader::isValidHeaderValue(const std::string& value) {
  if (value.length() > MAX_HEADER_VALUE_LENGTH) {
    return false;
  }

  for (std::size_t i = 0; i < value.length(); ++i) {
    if (!isValidValueCharacter(value[i])) {
      return false;
    }
  }

  return true;
}

bool HttpHeader::isValidHeaderLine(const std::string& line) {
  std::size_t colonPos = line.find(':');
  if (colonPos == std::string::npos) {
    return false;
  }

  std::string name = line.substr(0, colonPos);
  std::string value = line.substr(colonPos + 1);

  name = trimWhitespace(name);
  value = trimWhitespace(value);

  return isValidHeaderName(name) && isValidHeaderValue(value);
}

void HttpHeader::setValue(const std::string& value) {
  m_value = value;
  validate();
}

bool HttpHeader::operator==(const HttpHeader& other) const {
  return m_normalizedName == other.m_normalizedName && m_value == other.m_value;
}

bool HttpHeader::operator!=(const HttpHeader& other) const {
  return !(*this == other);
}

bool HttpHeader::operator<(const HttpHeader& other) const {
  if (m_normalizedName != other.m_normalizedName) {
    return m_normalizedName < other.m_normalizedName;
  }
  return m_value < other.m_value;
}

HttpHeader HttpHeader::fromString(const std::string& headerLine) {
  std::size_t colonPos = headerLine.find(':');
  if (colonPos == std::string::npos) {
    throw exceptions::HttpHeaderException(
        "Header line missing colon separator: '" + headerLine + "'",
        exceptions::HttpHeaderException::MISSING_COLON_SEPARATOR);
  }

  std::string name = headerLine.substr(0, colonPos);
  std::string value = headerLine.substr(colonPos + 1);

  name = trimWhitespace(name);
  value = trimWhitespace(value);

  return HttpHeader(name, value);
}

HttpHeader HttpHeader::contentLength(std::size_t length) {
  std::ostringstream oss;
  oss << length;
  return HttpHeader(CONTENT_LENGTH, oss.str());
}

HttpHeader HttpHeader::contentType(const std::string& type) {
  return HttpHeader(CONTENT_TYPE, type);
}

HttpHeader HttpHeader::host(const std::string& hostname) {
  return HttpHeader(HOST, hostname);
}

HttpHeader HttpHeader::connection(const std::string& value) {
  return HttpHeader(CONNECTION, value);
}

HttpHeader HttpHeader::transferEncoding(const std::string& encoding) {
  return HttpHeader(TRANSFER_ENCODING, encoding);
}

std::string HttpHeader::normalizeName(const std::string& name) {
  std::string normalized;
  normalized.reserve(name.length());

  for (std::size_t i = 0; i < name.length(); ++i) {
    normalized += toLowerCase(name[i]);
  }

  return normalized;
}

std::string HttpHeader::trimWhitespace(const std::string& str) {
  if (str.empty()) {
    return str;
  }

  std::size_t start = 0;
  while (start < str.length() && isWhitespace(str[start])) {
    ++start;
  }

  if (start == str.length()) {
    return "";
  }

  std::size_t end = str.length() - 1;
  while (end > start && isWhitespace(str[end])) {
    --end;
  }

  return str.substr(start, end - start + 1);
}

void HttpHeader::validate() const {
  if (m_name.empty()) {
    throw exceptions::HttpHeaderException(
        "Header name cannot be empty",
        exceptions::HttpHeaderException::EMPTY_NAME);
  }

  if (m_name.length() > MAX_HEADER_NAME_LENGTH) {
    std::ostringstream oss;
    oss << "Header name too long: " << m_name.length()
        << " characters (max: " << MAX_HEADER_NAME_LENGTH << ")";
    throw exceptions::HttpHeaderException(
        oss.str(), exceptions::HttpHeaderException::NAME_TOO_LONG);
  }

  if (m_value.length() > MAX_HEADER_VALUE_LENGTH) {
    std::ostringstream oss;
    oss << "Header value too long: " << m_value.length()
        << " characters (max: " << MAX_HEADER_VALUE_LENGTH << ")";
    throw exceptions::HttpHeaderException(
        oss.str(), exceptions::HttpHeaderException::VALUE_TOO_LONG);
  }

  for (std::size_t i = 0; i < m_name.length(); ++i) {
    if (!isValidNameCharacter(m_name[i])) {
      std::ostringstream oss;
      oss << "Invalid character in header name: '" << m_name[i] << "' (ASCII "
          << static_cast<int>(m_name[i]) << ")";
      throw exceptions::HttpHeaderException(
          oss.str(),
          exceptions::HttpHeaderException::INVALID_CHARACTER_IN_NAME);
    }
  }

  for (std::size_t i = 0; i < m_value.length(); ++i) {
    if (!isValidValueCharacter(m_value[i])) {
      std::ostringstream oss;
      oss << "Invalid character in header value: '" << m_value[i] << "' (ASCII "
          << static_cast<int>(m_value[i]) << ")";
      throw exceptions::HttpHeaderException(
          oss.str(),
          exceptions::HttpHeaderException::INVALID_CHARACTER_IN_VALUE);
    }
  }
}

void HttpHeader::updateNormalizedName() {
  m_normalizedName = normalizeName(m_name);
}

bool HttpHeader::isValidNameCharacter(char chr) {
  return (chr >= 'a' && chr <= 'z') || (chr >= 'A' && chr <= 'Z') ||
         (chr >= '0' && chr <= '9') || chr == '-' || chr == '_';
}

// TODO: maybe can be use StringUtils
bool HttpHeader::isValidValueCharacter(char chr) {
  return (chr >= 32 && chr <= 126) || chr == '\t';
}

bool HttpHeader::isWhitespace(char chr) { return chr == ' ' || chr == '\t'; }

char HttpHeader::toLowerCase(char chr) {
  if (chr >= 'A' && chr <= 'Z') {
    return chr + ('a' - 'A');
  }
  return chr;
}

}  // namespace value_objects
}  // namespace http
}  // namespace domain
