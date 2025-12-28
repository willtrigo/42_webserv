/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Uri.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: umeneses <umeneses@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/21 12:50:29 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/27 20:09:54 by umeneses         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "domain/http/exceptions/PortException.hpp"
#include "domain/http/exceptions/UriException.hpp"
#include "domain/http/value_objects/Uri.hpp"
#include "domain/shared/utils/StringUtils.hpp"

#include <algorithm>
#include <cctype>
#include <sstream>
#include <vector>

namespace domain {
namespace http {
namespace value_objects {

const std::string Uri::HTTP_SCHEME = "http";
const std::string Uri::HTTPS_SCHEME = "https";
const std::string Uri::FTP_SCHEME = "ftp";
const std::string Uri::FILE_SCHEME = "file";
const std::string Uri::WS_SCHEME = "ws";
const std::string Uri::WSS_SCHEME = "wss";

const Port Uri::HTTP_DEFAULT_PORT = Port::httpPort();
const Port Uri::HTTPS_DEFAULT_PORT = Port::httpsPort();
const Port Uri::FTP_DEFAULT_PORT = Port::ftpPort();

Uri::UriParameters::UriParameters() {}

Uri::UriParameters& Uri::UriParameters::withScheme(const std::string& scheme) {
  m_scheme = scheme;
  return *this;
}
Uri::UriParameters& Uri::UriParameters::withHost(const std::string& host) {
  m_host = host;
  return *this;
}
Uri::UriParameters& Uri::UriParameters::withPort(const Port& port) {
  m_port = port;
  return *this;
}
Uri::UriParameters& Uri::UriParameters::withPath(const std::string& path) {
  m_path = path;
  return *this;
}
Uri::UriParameters& Uri::UriParameters::withQuery(const std::string& query) {
  m_query = query;
  return *this;
}
Uri::UriParameters& Uri::UriParameters::withFragment(
    const std::string& fragment) {
  m_fragment = fragment;
  return *this;
}

Uri::Uri() : m_port(Port(0)), m_isAbsolute(false) {}

Uri::Uri(const std::string& uriString) : m_port(Port(0)), m_isAbsolute(false) {
  parseUriString(uriString);
  validate();
}

Uri::Uri(const UriParameters& param)
    : m_scheme(param.m_scheme),
      m_host(param.m_host),
      m_port(param.m_port),
      m_path(param.m_path),
      m_query(param.m_query),
      m_fragment(param.m_fragment),
      m_isAbsolute(!param.m_scheme.empty()) {
  validate();
}

Uri::Uri(const Uri& other)
    : m_scheme(other.m_scheme),
      m_host(other.m_host),
      m_port(other.m_port),
      m_path(other.m_path),
      m_query(other.m_query),
      m_fragment(other.m_fragment),
      m_isAbsolute(other.m_isAbsolute) {}

Uri::~Uri() {}

Uri& Uri::operator=(const Uri& other) {
  if (this != &other) {
    m_scheme = other.m_scheme;
    m_host = other.m_host;
    m_port = other.m_port;
    m_path = other.m_path;
    m_query = other.m_query;
    m_fragment = other.m_fragment;
    m_isAbsolute = other.m_isAbsolute;
  }
  return *this;
}

std::string Uri::getScheme() const { return m_scheme; }

std::string Uri::getHost() const { return m_host; }

Port Uri::getPort() const { return m_port; }

std::string Uri::getPath() const { return m_path; }

std::string Uri::getQuery() const { return m_query; }

std::string Uri::getFragment() const { return m_fragment; }

std::string Uri::getAuthority() const {
  if (m_host.empty()) {
    return "";
  }

  std::ostringstream oss;
  oss << m_host;

  if (m_port.getValue() > 0 && !isDefaultPort()) {
    oss << ":" << m_port.getValue();
  }

  return oss.str();
}

std::string Uri::toString() const {
  std::ostringstream oss;

  if (m_isAbsolute) {
    oss << m_scheme << ":";

    if (!m_host.empty()) {
      oss << "//" << getAuthority();
    }
  }

  if (!m_path.empty()) {
    if (m_isAbsolute && !m_host.empty() && !m_path.empty() &&
        m_path[0] != '/') {
      oss << "/";
    }
    oss << m_path;
  }

  if (!m_query.empty()) {
    oss << "?" << m_query;
  }

  if (!m_fragment.empty()) {
    oss << "#" << m_fragment;
  }

  return oss.str();
}

std::string Uri::toNormalizedString() const {
  std::ostringstream oss;

  if (m_isAbsolute) {
    oss << m_scheme << ":";

    if (!m_host.empty()) {
      oss << "//" << m_host;

      if (m_port.getValue() > 0 && !isDefaultPort()) {
        oss << ":" << m_port.getValue();
      }
    }
  }

  if (!m_path.empty()) {
    if (m_isAbsolute && !m_host.empty() && !m_path.empty() &&
        m_path[0] != '/') {
      oss << "/";
    }
    oss << normalizePath(m_path);
  }

  if (!m_query.empty()) {
    oss << "?" << m_query;
  }

  if (!m_fragment.empty()) {
    oss << "#" << m_fragment;
  }

  return oss.str();
}

bool Uri::isValidUri(const std::string& uriString) {
  try {
    Uri uri(uriString);
    return true;
  } catch (const exceptions::UriException&) {
    return false;
  }
}

bool Uri::isAbsoluteUri(const std::string& uriString) {
  if (uriString.empty()) {
    return false;
  }

  std::size_t colonPos = uriString.find(':');
  if (colonPos == std::string::npos || colonPos == 0) {
    return false;
  }

  std::string scheme = uriString.substr(0, colonPos);
  return isValidScheme(scheme);
}

bool Uri::isRelativeUri(const std::string& uriString) {
  return !isAbsoluteUri(uriString) && !uriString.empty();
}

void Uri::validate() const {
  if (m_isAbsolute) {
    if (m_scheme.empty()) {
      throw exceptions::UriException("Absolute URI must have a scheme",
                                     exceptions::UriException::MISSING_SCHEME);
    }

    validateUriComponent(m_scheme, "scheme");
    validateUriComponent(m_host, "host");

    if (m_scheme != FILE_SCHEME && m_host.empty()) {
      throw exceptions::UriException(
          "URI with scheme '" + m_scheme + "' must have a host",
          exceptions::UriException::MISSING_HOST);
    }
  }

  validateUriComponent(m_path, "path");
  validateUriComponent(m_query, "query");
  validateUriComponent(m_fragment, "fragment");
}

bool Uri::isEmpty() const {
  return m_scheme.empty() && m_host.empty() && m_port.getValue() == 0 &&
         m_path.empty() && m_query.empty() && m_fragment.empty();
}

bool Uri::isAbsolute() const { return m_isAbsolute; }

bool Uri::isRelative() const { return !m_isAbsolute; }

bool Uri::hasScheme() const { return !m_scheme.empty(); }

bool Uri::hasHost() const { return !m_host.empty(); }

bool Uri::hasPort() const { return m_port.getValue() > 0; }

bool Uri::hasPath() const { return !m_path.empty(); }

bool Uri::hasQuery() const { return !m_query.empty(); }

bool Uri::hasFragment() const { return !m_fragment.empty(); }

bool Uri::isDefaultPort() const {
  return isDefaultPortForScheme(m_scheme, m_port);
}

bool Uri::isHttp() const { return m_scheme == HTTP_SCHEME; }

bool Uri::isHttps() const { return m_scheme == HTTPS_SCHEME; }

bool Uri::isWebSocket() const { return m_scheme == WS_SCHEME; }

bool Uri::isSecureWebSocket() const { return m_scheme == WSS_SCHEME; }

bool Uri::isFile() const { return m_scheme == FILE_SCHEME; }

bool Uri::operator==(const Uri& other) const {
  return toString() == other.toString();
}

bool Uri::operator!=(const Uri& other) const { return !(*this == other); }

bool Uri::operator<(const Uri& other) const {
  return toString() < other.toString();
}

Uri Uri::resolve(const Uri& baseUri) const {
  if (m_isAbsolute) {
    return *this;
  }

  if (!baseUri.isAbsolute()) {
    throw exceptions::UriException("Base URI must be absolute for resolution",
                                   exceptions::UriException::INVALID_FORMAT);
  }

  if (!m_path.empty() && m_path[0] == '/') {
    return Uri(UriParameters()
                   .withScheme(baseUri.m_scheme)
                   .withHost(baseUri.m_host)
                   .withPort(baseUri.m_port)
                   .withPath(normalizePath(m_path))
                   .withQuery(m_query)
                   .withFragment(m_fragment));
  }
  std::string resolvedPath = baseUri.getBasePath() + m_path;
  return Uri(UriParameters()
                 .withScheme(baseUri.m_scheme)
                 .withHost(baseUri.m_host)
                 .withPort(baseUri.m_port)
                 .withPath(normalizePath(resolvedPath))
                 .withQuery(m_query)
                 .withFragment(m_fragment));
}

Uri Uri::normalize() const {
  return Uri(UriParameters()
                 .withScheme(m_scheme)
                 .withHost(m_host)
                 .withPort(m_port)
                 .withPath(normalizePath(m_path))
                 .withQuery(m_query)
                 .withFragment(m_fragment));
}

Uri Uri::withScheme(const std::string& scheme) const {
  return Uri(UriParameters()
                 .withScheme(scheme)
                 .withHost(m_host)
                 .withPort(m_port)
                 .withPath(m_path)
                 .withQuery(m_query)
                 .withFragment(m_fragment));
}

Uri Uri::withHost(const std::string& host) const {
  return Uri(UriParameters()
                 .withScheme(m_scheme)
                 .withHost(host)
                 .withPort(m_port)
                 .withPath(m_path)
                 .withQuery(m_query)
                 .withFragment(m_fragment));
}

Uri Uri::withPort(const Port& port) const {
  return Uri(UriParameters()
                 .withScheme(m_scheme)
                 .withHost(m_host)
                 .withPort(port)
                 .withPath(m_path)
                 .withQuery(m_query)
                 .withFragment(m_fragment));
}

Uri Uri::withPath(const std::string& path) const {
  return Uri(UriParameters()
                 .withScheme(m_scheme)
                 .withHost(m_host)
                 .withPort(m_port)
                 .withPath(path)
                 .withQuery(m_query)
                 .withFragment(m_fragment));
}

Uri Uri::withQuery(const std::string& query) const {
  return Uri(UriParameters()
                 .withScheme(m_scheme)
                 .withHost(m_host)
                 .withPort(m_port)
                 .withPath(m_path)
                 .withQuery(query)
                 .withFragment(m_fragment));
}

Uri Uri::withFragment(const std::string& fragment) const {
  return Uri(UriParameters()
                 .withScheme(m_scheme)
                 .withHost(m_host)
                 .withPort(m_port)
                 .withPath(m_path)
                 .withQuery(m_query)
                 .withFragment(fragment));
}

Uri Uri::fromString(const std::string& uriString) { return Uri(uriString); }

Uri Uri::parseAuthority(const std::string& authority) {
  std::string host;
  Port port(0);
  std::string remaining = parseAuthorityComponent(authority, host, port);

  if (!remaining.empty()) {
    throw exceptions::UriException(
        "Extra characters in authority string: '" + authority + "'",
        exceptions::UriException::INVALID_FORMAT);
  }

  return Uri(UriParameters()
                 .withScheme("")
                 .withHost(host)
                 .withPort(port)
                 .withPath("")
                 .withQuery("")
                 .withFragment(""));
}

Uri Uri::createHttp(const std::string& host, const Port& port,
                    const std::string& path) {
  return Uri(UriParameters()
                 .withScheme(HTTPS_SCHEME)
                 .withHost(host)
                 .withPort(port)
                 .withPath(path)
                 .withQuery("")
                 .withFragment(""));
}

Uri Uri::createHttps(const std::string& host, const Port& port,
                     const std::string& path) {
  return Uri(UriParameters()
                 .withScheme(HTTPS_SCHEME)
                 .withHost(host)
                 .withPort(port)
                 .withPath(path)
                 .withQuery("")
                 .withFragment(""));
}

Uri Uri::createFile(const std::string& path) {
  return Uri(UriParameters()
                 .withScheme(FILE_SCHEME)
                 .withHost("")
                 .withPort(Port(0))
                 .withPath(path)
                 .withQuery("")
                 .withFragment(""));
}

std::string Uri::encodeComponent(const std::string& component) {
  std::ostringstream oss;

  for (std::size_t i = 0; i < component.length(); ++i) {
    unsigned char chr = static_cast<unsigned char>(component[i]);

    if ((std::isalnum(chr) != 0) || chr == '-' || chr == '_' || chr == '.' ||
        chr == '~') {
      oss << chr;
    } else {
      oss << '%' << std::hex << std::uppercase << static_cast<int>(chr);
    }
  }

  return oss.str();
}

std::string Uri::decodeComponent(const std::string& component) {
  std::ostringstream oss;

  for (std::size_t i = 0; i < component.length(); ++i) {
    if (component[i] == '%' && i + 2 < component.length()) {
      std::string hex = component.substr(i + 1, 2);
      char* endPtr = NULL;
      long value = std::strtol(hex.c_str(), &endPtr, BASE_HEX);

      if (endPtr == hex.c_str() + HEX_DIGITS_PER_BYTE &&
          value >= MIN_BYTE_VALUE && value <= MAX_BYTE_VALUE) {
        oss << static_cast<char>(value);
        i += 2;
      } else {
        oss << component[i];
      }
    } else {
      oss << component[i];
    }
  }

  return oss.str();
}

std::string Uri::getBasePath() const {
  if (m_path.empty()) {
    return "/";
  }

  std::size_t lastSlash = m_path.find_last_of('/');
  if (lastSlash == std::string::npos) {
    return "/";
  }

  return m_path.substr(0, lastSlash + 1);
}

std::string Uri::getFilename() const {
  if (m_path.empty()) {
    return "";
  }

  std::size_t lastSlash = m_path.find_last_of('/');
  if (lastSlash == std::string::npos) {
    return m_path;
  }

  return m_path.substr(lastSlash + 1);
}

std::string Uri::getExtension() const {
  std::string filename = getFilename();
  if (filename.empty()) {
    return "";
  }

  std::size_t lastDot = filename.find_last_of('.');
  if (lastDot == std::string::npos || lastDot == 0) {
    return "";
  }

  return filename.substr(lastDot);
}

Uri Uri::getParentUri() const {
  std::string parentPath = getBasePath();

  if (parentPath.length() > 1 && parentPath[parentPath.length() - 1] == '/') {
    parentPath = parentPath.substr(0, parentPath.length() - 1);
  }

  return Uri(UriParameters()
                 .withScheme(m_scheme)
                 .withHost(m_host)
                 .withPort(m_port)
                 .withPath(parentPath)
                 .withQuery(m_query)
                 .withFragment(m_fragment));
}

std::string Uri::getQueryParameter(const std::string& queryString,
                                   const std::string& parameterName) {
  if (queryString.empty() || parameterName.empty()) {
    return "";
  }

  std::string searchFor = parameterName + "=";
  std::size_t startPos = queryString.find(searchFor);

  if (startPos == std::string::npos) {
    return "";
  }

  startPos += searchFor.length();
  std::size_t endPos = queryString.find('&', startPos);

  if (endPos == std::string::npos) {
    endPos = queryString.length();
  }

  std::string value = queryString.substr(startPos, endPos - startPos);
  return decodeComponent(value);
}

void Uri::parseUriString(const std::string& uriString) {
  if (uriString.empty()) {
    throw exceptions::UriException("URI string cannot be empty",
                                   exceptions::UriException::EMPTY_URI);
  }

  if (uriString.length() > MAX_URI_LENGTH) {
    std::ostringstream oss;
    oss << "URI too long: " << uriString.length()
        << " characters (max: " << MAX_URI_LENGTH << ")";
    throw exceptions::UriException(oss.str(),
                                   exceptions::UriException::INVALID_FORMAT);
  }

  std::size_t position = 0;

  std::size_t colonPos = findSchemeSeparator(uriString);
  if (colonPos != std::string::npos) {
    m_scheme = parseScheme(uriString, position);
    m_isAbsolute = true;

    std::size_t authorityStart = findAuthorityStart(uriString, position);
    if (authorityStart != std::string::npos) {
      std::size_t authorityEnd = findAuthorityEnd(uriString, authorityStart);
      if (authorityEnd > authorityStart) {
        std::string authority =
            uriString.substr(authorityStart, authorityEnd - authorityStart);
        parseAuthorityComponent(authority, m_host, m_port);
        position = authorityEnd;
      }
    }
  }

  m_path = parsePath(uriString, position);

  std::size_t queryStart = findQueryStart(uriString, position);
  if (queryStart != std::string::npos) {
    position = queryStart;
    m_query = parseQuery(uriString, position);
  }

  std::size_t fragmentStart = findFragmentStart(uriString, position);
  if (fragmentStart != std::string::npos) {
    position = fragmentStart;
    m_fragment = parseFragment(uriString, position);
  }

  if (m_path.empty() && (m_scheme == HTTP_SCHEME || m_scheme == HTTPS_SCHEME)) {
    m_path = "/";
  }
}

void Uri::validateUriComponent(const std::string& component,
                               const std::string& componentName) {
  for (std::size_t i = 0; i < component.length(); ++i) {
    unsigned char chr = static_cast<unsigned char>(component[i]);
    if (chr < ' ' || chr == 'W') {
      throw exceptions::UriException(
          "Invalid character in " + componentName + ": '" + component + "'",
          exceptions::UriException::INVALID_FORMAT);
    }
  }
}

bool Uri::isValidScheme(const std::string& scheme) {
  if (scheme.empty()) {
    return false;
  }

  if (std::isalpha(static_cast<unsigned char>(scheme[0])) == 0) {
    return false;
  }

  for (std::size_t i = 1; i < scheme.length(); ++i) {
    unsigned char chr = static_cast<unsigned char>(scheme[i]);
    if ((std::isalnum(chr) == 0) && chr != '+' && chr != '-' && chr != '.') {
      return false;
    }
  }

  return true;
}

bool Uri::isValidHost(const std::string& host) {
  if (host.empty()) {
    return true;
  }

  for (std::size_t i = 0; i < host.length(); ++i) {
    unsigned char chr = static_cast<unsigned char>(host[i]);
    if (chr < ' ' || chr == 'W') {
      return false;
    }
  }

  return true;
}

bool Uri::isValidPath(const std::string& path) {
  for (std::size_t i = 0; i < path.length(); ++i) {
    unsigned char chr = static_cast<unsigned char>(path[i]);
    if (chr < ' ' || chr == 'W') {
      return false;
    }
  }

  return true;
}

bool Uri::isValidQuery(const std::string& query) {
  for (std::size_t i = 0; i < query.length(); ++i) {
    unsigned char chr = static_cast<unsigned char>(query[i]);
    if (chr < ' ' || chr == 'W') {
      return false;
    }
  }

  return true;
}

bool Uri::isValidFragment(const std::string& fragment) {
  for (std::size_t i = 0; i < fragment.length(); ++i) {
    unsigned char chr = static_cast<unsigned char>(fragment[i]);
    if (chr < ' ' || chr == 'W') {
      return false;
    }
  }

  return true;
}

std::string Uri::normalizePath(const std::string& path) {
  if (path.empty()) {
    return "";
  }

  const char PATH_SEPARATOR = '/';

  const bool isAbsolute = (path[0] == PATH_SEPARATOR);

  std::vector<std::string> pathSegments;
  std::istringstream pathStream(path);

  processPathSegments(pathStream, pathSegments, isAbsolute);

  return buildNormalizedPath(pathSegments, isAbsolute, path);
}

void Uri::processPathSegments(std::istringstream& pathStream,
                              std::vector<std::string>& pathSegments,
                              bool isAbsolute) {
  std::string currentSegment;
  const std::string CURRENT_DIR = ".";
  const char PATH_SEPARATOR = '/';

  while (std::getline(pathStream, currentSegment, PATH_SEPARATOR) != 0) {
    if (currentSegment.empty() || currentSegment == CURRENT_DIR) {
      continue;
    }

    handlePathSegment(currentSegment, pathSegments, isAbsolute);
  }
}

void Uri::handlePathSegment(const std::string& segment,
                            std::vector<std::string>& pathSegments,
                            bool isAbsolute) {
  const std::string PARENT_DIR = "..";

  if (segment == PARENT_DIR) {
    handleParentDirectory(pathSegments, isAbsolute);
  } else {
    pathSegments.push_back(segment);
  }
}

void Uri::handleParentDirectory(std::vector<std::string>& pathSegments,
                                bool isAbsolute) {
  if (!pathSegments.empty() && pathSegments.back() != "..") {
    pathSegments.pop_back();
  } else if (!isAbsolute) {
    pathSegments.push_back("..");
  }
}

std::string Uri::buildNormalizedPath(
    const std::vector<std::string>& pathSegments, bool isAbsolute,
    const std::string& originalPath) {
  const char PATH_SEPARATOR = '/';

  if (isAbsolute && pathSegments.empty()) {
    return "/";
  }

  std::ostringstream normalizedPath;

  if (isAbsolute) {
    normalizedPath << PATH_SEPARATOR;
  }

  for (std::size_t i = 0; i < pathSegments.size(); ++i) {
    if (i > 0) {
      normalizedPath << PATH_SEPARATOR;
    }
    normalizedPath << pathSegments[i];
  }

  if (shouldAddTrailingSeparator(originalPath, pathSegments, isAbsolute)) {
    normalizedPath << PATH_SEPARATOR;
  }

  return normalizedPath.str();
}

bool Uri::shouldAddTrailingSeparator(
    const std::string& originalPath,
    const std::vector<std::string>& pathSegments, bool isAbsolute) {
  const char PATH_SEPARATOR = '/';

  if (originalPath.empty()) {
    return false;
  }

  bool originalEndsWithSlash =
      (originalPath[originalPath.length() - 1] == PATH_SEPARATOR);

  if (!originalEndsWithSlash) {
    return false;
  }

  return (isAbsolute && pathSegments.empty()) || !pathSegments.empty();
}

Port Uri::getDefaultPortForScheme(const std::string& scheme) {
  if (scheme == HTTP_SCHEME) {
    return HTTP_DEFAULT_PORT;
  }
  if (scheme == HTTPS_SCHEME) {
    return HTTPS_DEFAULT_PORT;
  }
  if (scheme == FTP_SCHEME) {
    return FTP_DEFAULT_PORT;
  }

  return Port(0);
}

bool Uri::isDefaultPortForScheme(const std::string& scheme, const Port& port) {
  if (scheme.empty() || port.getValue() == 0) {
    return false;
  }

  Port defaultPort = getDefaultPortForScheme(scheme);
  return defaultPort.getValue() > 0 && port == defaultPort;
}

std::size_t Uri::findSchemeSeparator(const std::string& uriString) {
  std::size_t colonPos = uriString.find(':');

  if (colonPos == std::string::npos || colonPos == 0) {
    return std::string::npos;
  }

  std::string potentialScheme = uriString.substr(0, colonPos);

  static const std::string SCHEME_CHARS =
      "abcdefghijklmnopqrstuvwxyz"
      "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
      "0123456789"
      "+-.";

  if (!shared::utils::StringUtils::containsOnly(potentialScheme,
                                                SCHEME_CHARS)) {
    return std::string::npos;
  }

  if (std::isalpha(static_cast<unsigned char>(uriString[0])) == 0) {
    return std::string::npos;
  }

  return colonPos;
}

std::size_t Uri::findAuthorityStart(const std::string& uriString,
                                    std::size_t schemeEnd) {
  if (schemeEnd + 2 >= uriString.length()) {
    return std::string::npos;
  }

  if (uriString[schemeEnd] == '/' && uriString[schemeEnd + 1] == '/') {
    return schemeEnd + 2;
  }

  return std::string::npos;
}

std::size_t Uri::findAuthorityEnd(const std::string& uriString,
                                  std::size_t authorityStart) {
  if (authorityStart >= uriString.length()) {
    return authorityStart;
  }

  std::size_t position = authorityStart;
  std::size_t length = uriString.length();
  static const std::string AUTHORITY_CHARS =
      "abcdefghijklmnopqrstuvwxyz"
      "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
      "0123456789"
      ".-:[]";

  bool inIpv6Brackets = false;
  bool foundPortSeparator = false;

  while (position < length) {
    char chr = uriString[position];

    if (chr == '[') {
      if (inIpv6Brackets) {
        return position;
      }
      inIpv6Brackets = true;
    } else if (chr == ']') {
      if (!inIpv6Brackets) {
        return position;
      }
      inIpv6Brackets = false;
    }

    bool isValidChar = AUTHORITY_CHARS.find(chr) != std::string::npos;

    if (!isValidChar && !inIpv6Brackets) {
      if (chr == '/' || chr == '?' || chr == '#' ||
          std::isspace(static_cast<unsigned char>(chr)) != 0) {
        return position;
      }
      if (chr == ':') {
        if (inIpv6Brackets) {
          position++;
          continue;
        }
        if (!foundPortSeparator) {
          foundPortSeparator = true;
          std::size_t nextPos = position + 1;
          if (nextPos < length) {
            position++;
            continue;
          }
        } else {
          return position;
        }
      }
      return position;
    }
    position++;
  }

  return length;
}

std::size_t Uri::findPathStart(const std::string& uriString,
                               std::size_t authorityEnd) {
  if (authorityEnd >= uriString.length()) {
    return authorityEnd;
  }

  if (uriString[authorityEnd] == ':') {
    return authorityEnd + 1;
  }

  return authorityEnd;
}

std::size_t Uri::findQueryStart(const std::string& uriString,
                                std::size_t pathStart) {
  if (pathStart >= uriString.length()) {
    return std::string::npos;
  }

  std::size_t position = pathStart;
  std::size_t length = uriString.length();

  while (position < length) {
    char chr = uriString[position];

    if (chr == '?') {
      if (position > pathStart && uriString[position - 1] == '%' &&
          position + 2 < length) {
        std::string hex = uriString.substr(position, 2);
        if (shared::utils::StringUtils::isAllHexDigits(hex)) {
          position += 3;  // Skip %3F
          continue;
        }
      }
      return position;
    }
    if (chr == '#') {
      return std::string::npos;
    }

    position++;
  }

  return std::string::npos;
}

std::size_t Uri::findFragmentStart(const std::string& uriString,
                                   std::size_t queryStart) {
  std::size_t startPos = (queryStart == std::string::npos) ? 0 : queryStart;

  if (startPos >= uriString.length()) {
    return std::string::npos;
  }

  std::size_t position = startPos;
  std::size_t length = uriString.length();

  while (position < length) {
    char chr = uriString[position];

    if (chr == '#') {
      if (position > startPos && uriString[position - 1] == '%' &&
          position + 2 < length) {
        std::string hex = uriString.substr(position, 2);
        if (shared::utils::StringUtils::isAllHexDigits(hex)) {
          position += 3;
          continue;
        }
      }
      return position;
    }

    position++;
  }

  return std::string::npos;
}

std::string Uri::parseScheme(const std::string& uriString,
                             std::size_t& position) {
  std::size_t colonPos = uriString.find(':');
  if (colonPos == std::string::npos) {
    return "";
  }

  std::string scheme = uriString.substr(0, colonPos);
  position = colonPos + 1;

  if (!isValidScheme(scheme)) {
    throw exceptions::UriException("Invalid scheme: '" + scheme + "'",
                                   exceptions::UriException::INVALID_SCHEME);
  }

  std::transform(scheme.begin(), scheme.end(), scheme.begin(), ::tolower);
  return scheme;
}

std::string Uri::parseAuthorityComponent(const std::string& authorityString,
                                         std::string& host, Port& port) {
  if (authorityString.empty()) {
    return "";
  }

  std::size_t colonPos = authorityString.find(':');

  if (colonPos == std::string::npos) {
    host = authorityString;
    port = Port(0);
    return "";
  }

  host = authorityString.substr(0, colonPos);

  std::string portStr = authorityString.substr(colonPos + 1);
  if (portStr.empty()) {
    throw exceptions::UriException(
        "Port cannot be empty after colon: '" + authorityString + "'",
        exceptions::UriException::INVALID_PORT);
  }

  try {
    port = Port::fromString(portStr);
  } catch (const exceptions::PortException& e) {
    throw exceptions::UriException(
        "Invalid port in authority: '" + authorityString + "' - " + e.what(),
        exceptions::UriException::INVALID_PORT);
  }

  return "";
}

std::string Uri::parsePath(const std::string& uriString,
                           std::size_t& position) {
  if (position >= uriString.length()) {
    return "";
  }

  std::size_t queryStart = uriString.find('?', position);
  std::size_t fragmentStart = uriString.find('#', position);

  std::size_t endPos = uriString.length();
  if (queryStart != std::string::npos && queryStart < endPos) {
    endPos = queryStart;
  }
  if (fragmentStart != std::string::npos && fragmentStart < endPos) {
    endPos = fragmentStart;
  }

  std::string path = uriString.substr(position, endPos - position);
  position = endPos;

  if (!isValidPath(path)) {
    throw exceptions::UriException("Invalid path: '" + path + "'",
                                   exceptions::UriException::INVALID_PATH);
  }

  return path;
}

std::string Uri::parseQuery(const std::string& uriString,
                            std::size_t& position) {
  if (position >= uriString.length() || uriString[position] != '?') {
    return "";
  }

  ++position;

  std::size_t fragmentStart = uriString.find('#', position);
  std::size_t endPos =
      (fragmentStart != std::string::npos) ? fragmentStart : uriString.length();

  std::string query = uriString.substr(position, endPos - position);
  position = endPos;

  if (!isValidQuery(query)) {
    throw exceptions::UriException("Invalid query: '" + query + "'",
                                   exceptions::UriException::INVALID_QUERY);
  }

  return query;
}

std::string Uri::parseFragment(const std::string& uriString,
                               std::size_t& position) {
  if (position >= uriString.length() || uriString[position] != '#') {
    return "";
  }

  ++position;

  std::string fragment = uriString.substr(position);
  position = uriString.length();

  if (!isValidFragment(fragment)) {
    throw exceptions::UriException("Invalid fragment: '" + fragment + "'",
                                   exceptions::UriException::INVALID_FRAGMENT);
  }

  return fragment;
}

}  // namespace value_objects
}  // namespace http
}  // namespace domain
