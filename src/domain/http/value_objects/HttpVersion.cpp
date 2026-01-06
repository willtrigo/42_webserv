/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpVersion.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/04 13:46:11 by dande-je          #+#    #+#             */
/*   Updated: 2026/01/04 13:47:15 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpVersion.hpp"
#include "domain/http/exceptions/HttpVersionException.hpp"

#include <sstream>

namespace domain {
namespace http {
namespace value_objects {

HttpVersion::HttpVersion() : m_major(HTTP_1_1_MAJOR), m_minor(HTTP_1_1_MINOR) {}

HttpVersion::HttpVersion(unsigned int major, unsigned int minor)
    : m_major(major), m_minor(minor) {
  validate();
}

HttpVersion::HttpVersion(const std::string& versionString)
    : m_major(HTTP_1_1_MAJOR), m_minor(HTTP_1_1_MINOR) {
  parseVersionString(versionString, m_major, m_minor);
  validate();
}

HttpVersion::HttpVersion(const HttpVersion& other)
    : m_major(other.m_major), m_minor(other.m_minor) {}

HttpVersion::~HttpVersion() {}

HttpVersion& HttpVersion::operator=(const HttpVersion& other) {
  if (this != &other) {
    m_major = other.m_major;
    m_minor = other.m_minor;
  }
  return *this;
}

unsigned int HttpVersion::getMajor() const { return m_major; }

unsigned int HttpVersion::getMinor() const { return m_minor; }

std::string HttpVersion::toString() const {
  std::ostringstream oss;
  oss << "HTTP/" << m_major << "." << m_minor;
  return oss.str();
}

bool HttpVersion::isValidVersion(unsigned int major, unsigned int minor) {
  if (major < MIN_MAJOR_VERSION || major > MAX_MAJOR_VERSION) {
    return false;
  }
  if (minor < MIN_MINOR_VERSION || minor > MAX_MINOR_VERSION) {
    return false;
  }

  if (major == 1 && (minor == 0 || minor == 1)) {
    return true;
  }
  if (major == 2 && minor == 0) {
    return true;
  }

  return false;
}

bool HttpVersion::isValidVersionString(const std::string& versionString) {
  try {
    unsigned int major = 0;
    unsigned int minor = 0;
    parseVersionString(versionString, major, minor);
    return isValidVersion(major, minor);
  } catch (const exceptions::HttpVersionException&) {
    return false;
  }
}

bool HttpVersion::isHttp10() const {
  return m_major == HTTP_1_0_MAJOR && m_minor == HTTP_1_0_MINOR;
}

bool HttpVersion::isHttp11() const {
  return m_major == HTTP_1_1_MAJOR && m_minor == HTTP_1_1_MINOR;
}

bool HttpVersion::isHttp20() const {
  return m_major == HTTP_2_0_MAJOR && m_minor == HTTP_2_0_MINOR;
}

bool HttpVersion::supportsKeepAlive() const {
  return m_major >= HTTP_1_1_MAJOR;
}

bool HttpVersion::supportsPipelining() const {
  return m_major >= HTTP_1_1_MAJOR;
}

bool HttpVersion::supportsChunkedEncoding() const {
  return m_major >= HTTP_1_1_MAJOR;
}

bool HttpVersion::operator==(const HttpVersion& other) const {
  return m_major == other.m_major && m_minor == other.m_minor;
}

bool HttpVersion::operator!=(const HttpVersion& other) const {
  return !(*this == other);
}

bool HttpVersion::operator<(const HttpVersion& other) const {
  if (m_major != other.m_major) {
    return m_major < other.m_major;
  }
  return m_minor < other.m_minor;
}

bool HttpVersion::operator<=(const HttpVersion& other) const {
  return *this < other || *this == other;
}

bool HttpVersion::operator>(const HttpVersion& other) const {
  return !(*this <= other);
}

bool HttpVersion::operator>=(const HttpVersion& other) const {
  return !(*this < other);
}

HttpVersion HttpVersion::http10() {
  return HttpVersion(HTTP_1_0_MAJOR, HTTP_1_0_MINOR);
}

HttpVersion HttpVersion::http11() {
  return HttpVersion(HTTP_1_1_MAJOR, HTTP_1_1_MINOR);
}

HttpVersion HttpVersion::http20() {
  return HttpVersion(HTTP_2_0_MAJOR, HTTP_2_0_MINOR);
}

HttpVersion HttpVersion::fromString(const std::string& versionString) {
  return HttpVersion(versionString);
}

void HttpVersion::validate() const {
  if (!isValidVersion(m_major, m_minor)) {
    std::ostringstream oss;
    oss << "Unsupported HTTP version: " << m_major << "." << m_minor;
    throw exceptions::HttpVersionException(
        oss.str(), exceptions::HttpVersionException::UNSUPPORTED_VERSION);
  }
}

// TODO: refactor this func
void HttpVersion::parseVersionString(const std::string& versionString,
                                     unsigned int& major, unsigned int& minor) {
  if (versionString.empty()) {
    throw exceptions::HttpVersionException(
        "Version string cannot be empty",
        exceptions::HttpVersionException::EMPTY_STRING);
  }

  if (versionString.length() < MIN_VERSION_STRING_LENGTH ||
      versionString.length() > MAX_VERSION_STRING_LENGTH) {
    throw exceptions::HttpVersionException(
        "Invalid version string length: '" + versionString + "'",
        exceptions::HttpVersionException::INVALID_FORMAT);
  }

  if (versionString.substr(0, VERSION_STRING_PREFIX_LENGTH) != "HTTP/") {
    throw exceptions::HttpVersionException(
        "Version string must start with 'HTTP/': '" + versionString + "'",
        exceptions::HttpVersionException::INVALID_FORMAT);
  }

  std::string versionNumbers =
      versionString.substr(VERSION_STRING_PREFIX_LENGTH);

  std::size_t dotPos = versionNumbers.find('.');
  if (dotPos == std::string::npos) {
    throw exceptions::HttpVersionException(
        "Version string missing dot separator: '" + versionString + "'",
        exceptions::HttpVersionException::INVALID_FORMAT);
  }

  std::string majorStr = versionNumbers.substr(0, dotPos);
  std::string minorStr = versionNumbers.substr(dotPos + 1);

  if (majorStr.empty() || minorStr.empty()) {
    throw exceptions::HttpVersionException(
        "Version numbers cannot be empty: '" + versionString + "'",
        exceptions::HttpVersionException::INVALID_FORMAT);
  }

  for (std::size_t i = 0; i < majorStr.length(); ++i) {
    if (!isDigit(majorStr[i])) {
      throw exceptions::HttpVersionException(
          "Invalid major version number: '" + majorStr + "'",
          exceptions::HttpVersionException::INVALID_MAJOR_VERSION);
    }
  }

  for (std::size_t i = 0; i < minorStr.length(); ++i) {
    if (!isDigit(minorStr[i])) {
      throw exceptions::HttpVersionException(
          "Invalid minor version number: '" + minorStr + "'",
          exceptions::HttpVersionException::INVALID_MINOR_VERSION);
    }
  }

  std::istringstream majorStream(majorStr);
  std::istringstream minorStream(minorStr);

  majorStream >> major;
  minorStream >> minor;

  if (majorStream.fail() || minorStream.fail()) {
    throw exceptions::HttpVersionException(
        "Failed to parse version numbers: '" + versionString + "'",
        exceptions::HttpVersionException::INVALID_FORMAT);
  }
}

bool HttpVersion::isDigit(char chr) { return chr >= '0' && chr <= '9'; }

}  // namespace value_objects
}  // namespace http
}  // namespace domain
