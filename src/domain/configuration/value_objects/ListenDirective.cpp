/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ListenDirective.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: umeneses <umeneses@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/26 03:21:29 by dande-je          #+#    #+#             */
/*   Updated: 2026/01/05 21:04:51 by umeneses         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "domain/configuration/exceptions/ListenDirectiveException.hpp"
#include "domain/configuration/value_objects/ListenDirective.hpp"
#include "domain/http/exceptions/HostException.hpp"
#include "domain/http/exceptions/PortException.hpp"
#include "domain/http/value_objects/Host.hpp"
#include "domain/http/value_objects/Port.hpp"
#include "domain/shared/utils/StringUtils.hpp"

#include <cstdlib>
#include <sstream>

namespace domain {
namespace configuration {
namespace entities {

const std::string ListenDirective::DEFAULT_HOST = "0.0.0.0";
const http::value_objects::Port ListenDirective::DEFAULT_PORT =
    http::value_objects::Port::httpPort();

ListenDirective::ListenDirective()
    : m_host(http::value_objects::Host::wildcard()), m_port(DEFAULT_PORT) {}

ListenDirective::ListenDirective(const http::value_objects::Host& host,
                                 const http::value_objects::Port& port)
    : m_host(host), m_port(port) {
  validate();
}

ListenDirective::ListenDirective(const std::string& directiveString)
    : m_host(http::value_objects::Host::wildcard()), m_port(DEFAULT_PORT) {
  validateDirectiveString(directiveString);
  validateDirectiveFormat(directiveString);

  std::pair<std::string, std::string> parts = splitDirective(directiveString);
  std::string hostStr = normalizeHostString(parts.first);
  std::string portStr = normalizePortString(parts.second);

  validatePortStringFormat(portStr, directiveString);
  parseAndSetHostPort(hostStr, portStr, directiveString);
  validate();
}

ListenDirective::ListenDirective(const ListenDirective& other)
    : m_host(other.m_host), m_port(other.m_port) {}

ListenDirective::~ListenDirective() {}

ListenDirective& ListenDirective::operator=(const ListenDirective& other) {
  if (this != &other) {
    m_host = other.m_host;
    m_port = other.m_port;
  }
  return *this;
}

http::value_objects::Host ListenDirective::getHost() const { return m_host; }

http::value_objects::Port ListenDirective::getPort() const { return m_port; }

std::string ListenDirective::toString() const {
  std::ostringstream oss;

  if (m_host.isWildcard() && m_port.getValue() == DEFAULT_PORT.getValue()) {
    oss << m_port.getValue();
  } else if (m_host.isWildcard()) {
    oss << ":" << m_port.getValue();
  } else if (m_host.isIpv6()) {
    oss << "[" << m_host.getValue() << "]";
    if (m_port.getValue() != DEFAULT_PORT.getValue()) {
      oss << ":" << m_port.getValue();
    }
  } else {
    oss << m_host.getValue();
    if (m_port.getValue() != DEFAULT_PORT.getValue()) {
      oss << ":" << m_port.getValue();
    }
  }

  return oss.str();
}

std::string ListenDirective::toCanonicalString() const {
  std::ostringstream oss;

  if (m_host.isIpv6()) {
    oss << "[" << m_host.getValue() << "]:" << m_port.getValue();
  } else {
    oss << m_host.getValue() << ":" << m_port.getValue();
  }

  return oss.str();
}

bool ListenDirective::isValidDirective(const std::string& directiveString) {
  if (directiveString.empty()) {
    return false;
  }

  std::string trimmed = shared::utils::StringUtils::trim(directiveString);
  if (trimmed.empty()) {
    return false;
  }

  if (directiveString.find(':') == std::string::npos) {
    return false;
  }

  if (directiveString[directiveString.length() - 1] == ':') {
    return false;
  }

  try {
    validateDirectiveString(directiveString);
    std::pair<std::string, std::string> parts = splitDirective(directiveString);

    std::string hostStr = normalizeHostString(parts.first);
    std::string portStr = normalizePortString(parts.second);

    if (!http::value_objects::Host::isValidHost(hostStr)) {
      return false;
    }

    if (!portStr.empty()) {
      try {
        http::value_objects::Port::fromString(portStr);
      } catch (...) {
        return false;
      }
    }

    return true;
  } catch (...) {
    return false;
  }
}

bool ListenDirective::isDefaultListen(const ListenDirective& directive) {
  return directive.m_host.isWildcard() &&
         directive.m_port.getValue() == DEFAULT_PORT.getValue();
}

bool ListenDirective::operator==(const ListenDirective& other) const {
  return m_host == other.m_host && m_port == other.m_port;
}

bool ListenDirective::operator!=(const ListenDirective& other) const {
  return !(*this == other);
}

bool ListenDirective::operator<(const ListenDirective& other) const {
  if (m_host < other.m_host) return true;
  if (other.m_host < m_host) return false;
  return m_port < other.m_port;
}

bool ListenDirective::isEmpty() const { return m_host.getValue().empty(); }

bool ListenDirective::isWildcard() const { return m_host.isWildcard(); }

bool ListenDirective::isLocalhost() const { return m_host.isLocalhost(); }

bool ListenDirective::isDefaultPort() const {
  return m_port.getValue() == DEFAULT_PORT.getValue();
}

ListenDirective ListenDirective::fromString(
    const std::string& directiveString) {
  return ListenDirective(directiveString);
}

ListenDirective ListenDirective::createDefault() { return ListenDirective(); }

ListenDirective ListenDirective::createWildcard(
    const http::value_objects::Port& port) {
  return ListenDirective(http::value_objects::Host::wildcard(), port);
}

ListenDirective ListenDirective::createLocalhost(
    const http::value_objects::Port& port) {
  return ListenDirective(http::value_objects::Host::localhost(), port);
}

std::pair<std::string, std::string> ListenDirective::splitDirective(
    const std::string& directiveString) {
  std::string hostPart;
  std::string portPart;

  const std::size_t length = directiveString.length();
  std::size_t currentPosition = 0;

  if (directiveString[0] == '[') {
    const std::size_t closingBracket = directiveString.find(']', 1);
    if (closingBracket == std::string::npos) {
      throw exceptions::ListenDirectiveException(
          "Missing closing bracket for IPv6 address: " + directiveString,
          exceptions::ListenDirectiveException::INVALID_FORMAT);
    }

    hostPart = directiveString.substr(1, closingBracket - 1);
    currentPosition = closingBracket + 1;

    if (currentPosition < length && directiveString[currentPosition] == ':') {
      currentPosition++;
      portPart = directiveString.substr(currentPosition);
    }
  } else {
    const std::size_t colonPosition = directiveString.find(':');

    if (colonPosition == std::string::npos) {
      if (hasPort(directiveString)) {
        portPart = directiveString;
      } else {
        hostPart = directiveString;
      }
    } else {
      hostPart = directiveString.substr(0, colonPosition);
      portPart = directiveString.substr(colonPosition + 1);
    }
  }

  return std::make_pair(hostPart, portPart);
}

void ListenDirective::validate() const {
  validateHostPortCombination();
  validateIpv6Specifics();
  validateHostnameFormat();
  validatePortRange();
}

void ListenDirective::validateHostPortCombination() const {
  if (m_host.isWildcard() && m_port.getValue() == 0) {
    throw exceptions::ListenDirectiveException(
        "Wildcard host with port 0 is not valid for server listening",
        exceptions::ListenDirectiveException::INVALID_FORMAT);
  }
}

void ListenDirective::validateIpv6Specifics() const {
  if (!m_host.isIpv6()) {
    return;
  }

  if (m_host.getValue().empty()) {
    throw exceptions::ListenDirectiveException(
        "IPv6 address cannot be empty",
        exceptions::ListenDirectiveException::INVALID_HOST);
  }
}

void ListenDirective::validateHostnameFormat() const {
  if (!m_host.isHostname()) {
    return;
  }

  const std::string& hostname = m_host.getValue();

  validateHostnameColonPresence(hostname);
  validateHostnameBrackets(hostname);
  validateHostnameLength(hostname);
  validateHostnameDotsPattern(hostname);
}

void ListenDirective::validateHostnameColonPresence(
    const std::string& hostname) {
  if (hostname.find(':') != std::string::npos) {
    throw exceptions::ListenDirectiveException(
        "Hostname contains colon character: " + hostname,
        exceptions::ListenDirectiveException::INVALID_HOST);
  }
}

void ListenDirective::validateHostnameBrackets(const std::string& hostname) {
  const bool hasBracketOpen = hostname.find('[') != std::string::npos;
  const bool hasBracketClose = hostname.find(']') != std::string::npos;

  if (hasBracketOpen || hasBracketClose) {
    throw exceptions::ListenDirectiveException(
        "Hostname contains IPv6 brackets: " + hostname,
        exceptions::ListenDirectiveException::INVALID_HOST);
  }
}

void ListenDirective::validateHostnameLength(const std::string& hostname) {
  static const std::size_t MAX_HOSTNAME_LENGTH = 253;

  if (hostname.length() > MAX_HOSTNAME_LENGTH) {
    throw exceptions::ListenDirectiveException(
        "Hostname too long (max 253 chars): " + hostname,
        exceptions::ListenDirectiveException::INVALID_HOST);
  }
}

void ListenDirective::validateHostnameDotsPattern(const std::string& hostname) {
  if (hostname.find("..") != std::string::npos) {
    throw exceptions::ListenDirectiveException(
        "Hostname contains consecutive dots: " + hostname,
        exceptions::ListenDirectiveException::INVALID_HOST);
  }

  if (hostname.empty()) {
    return;
  }

  const bool startsWithDot = hostname[0] == '.';
  const bool endsWithDot = hostname[hostname.length() - 1] == '.';

  if (startsWithDot || endsWithDot) {
    throw exceptions::ListenDirectiveException(
        "Hostname cannot start or end with dot: " + hostname,
        exceptions::ListenDirectiveException::INVALID_HOST);
  }
}

void ListenDirective::validatePortRange() const {
  if (!http::value_objects::Port::isValidPort(m_port.getValue())) {
    std::ostringstream oss;
    oss << "Port value is invalid: " << m_port.getValue();
    throw exceptions::ListenDirectiveException(
        oss.str(), exceptions::ListenDirectiveException::INVALID_PORT);
  }
}

bool ListenDirective::isStandardPrivilegedPort() const {
  const unsigned int portValue = m_port.getValue();

  return portValue == http::value_objects::Port::HTTP_PORT ||
         portValue == http::value_objects::Port::HTTPS_PORT ||
         portValue == http::value_objects::Port::FTP_PORT;
}

void ListenDirective::validateDirectiveString(
    const std::string& directiveString) {
  if (directiveString.empty()) {
    throw exceptions::ListenDirectiveException(
        "Directive string cannot be empty",
        exceptions::ListenDirectiveException::EMPTY_STRING);
  }

  static const std::string VALID_CHARS =
      "abcdefghijklmnopqrstuvwxyz"
      "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
      "0123456789"
      ".:[]-_";

  if (!shared::utils::StringUtils::containsOnly(directiveString, VALID_CHARS)) {
    throw exceptions::ListenDirectiveException(
        "Invalid character in listen directive: " + directiveString,
        exceptions::ListenDirectiveException::INVALID_FORMAT);
  }
}

std::string ListenDirective::normalizeHostString(
    const std::string& hostString) {
  if (hostString.empty()) {
    return DEFAULT_HOST;
  }

  if (hostString == "*") {
    return http::value_objects::Host::WILDCARD_IPV4;
  }

  return hostString;
}

std::string ListenDirective::normalizePortString(
    const std::string& portString) {
  if (portString.empty()) {
    return "";
  }

  return portString;
}

bool ListenDirective::hasPort(const std::string& directiveString) {
  if (directiveString.empty()) {
    return false;
  }

  if (!shared::utils::StringUtils::isAllDigits(directiveString)) {
    return false;
  }

  try {
    unsigned long port = shared::utils::StringUtils::toUnsignedLong(
        directiveString, shared::utils::StringUtils::BASE_DECIMAL);
    return port <= http::value_objects::Port::MAX_PORT;
  } catch (...) {
    return false;
  }
}

void ListenDirective::validateDirectiveFormat(
    const std::string& directiveString) {
  if (directiveString.find(':') != std::string::npos &&
      directiveString[directiveString.length() - 1] == ':') {
    throw exceptions::ListenDirectiveException(
        "Invalid listen directive (trailing colon): " + directiveString,
        exceptions::ListenDirectiveException::INVALID_FORMAT);
  }

  if (directiveString.find(':') == std::string::npos) {
    throw exceptions::ListenDirectiveException(
        "Invalid listen directive (missing colon separator): " +
            directiveString,
        exceptions::ListenDirectiveException::INVALID_FORMAT);
  }
}

void ListenDirective::validatePortStringFormat(
    const std::string& portStr, const std::string& directiveString) {
  if (!portStr.empty()) {
    if (!shared::utils::StringUtils::isAllDigits(portStr)) {
      throw exceptions::ListenDirectiveException(
          "Invalid listen directive format: " + directiveString,
          exceptions::ListenDirectiveException::INVALID_FORMAT);
    }
  }
}

void ListenDirective::parseAndSetHostPort(const std::string& hostStr,
                                          const std::string& portStr,
                                          const std::string& directiveString) {
  try {
    if (hostStr.empty()) {
      m_host = http::value_objects::Host::wildcard();
    } else {
      m_host = http::value_objects::Host::fromString(hostStr);
    }

    if (!portStr.empty()) {
      m_port = http::value_objects::Port::fromString(portStr);
    }
  } catch (const http::exceptions::PortException&) {
    throw;
  } catch (const http::exceptions::HostException&) {
    throw;
  } catch (const std::exception& e) {
    throw exceptions::ListenDirectiveException(
        "Invalid listen directive: " + directiveString + " (" + e.what() + ")",
        exceptions::ListenDirectiveException::INVALID_FORMAT);
  }
}

}  // namespace entities
}  // namespace configuration
}  // namespace domain
