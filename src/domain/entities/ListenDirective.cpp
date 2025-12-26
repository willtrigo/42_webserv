/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ListenDirective.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/26 03:21:29 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/26 04:09:41 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "domain/entities/ListenDirective.hpp"
#include "domain/value_objects/Host.hpp"
#include "domain/value_objects/Port.hpp"
#include "shared/exceptions/ListenDirectiveException.hpp"
#include "shared/utils/StringUtils.hpp"

#include <cstdlib>
#include <sstream>

namespace domain {
namespace entities {

const std::string ListenDirective::DEFAULT_HOST =
    value_objects::Host::WILDCARD_IPV4;
const value_objects::Port ListenDirective::DEFAULT_PORT =
    value_objects::Port::httpPort();

ListenDirective::ListenDirective()
    : m_host(value_objects::Host::wildcard()), m_port(DEFAULT_PORT) {}

ListenDirective::ListenDirective(const value_objects::Host& host,
                                 const value_objects::Port& port)
    : m_host(host), m_port(port) {
  validate();
}

ListenDirective::ListenDirective(const std::string& directiveString)
    : m_host(value_objects::Host::wildcard()), m_port(DEFAULT_PORT) {
  validateDirectiveString(directiveString);

  std::pair<std::string, std::string> parts = splitDirective(directiveString);

  std::string hostStr = normalizeHostString(parts.first);
  std::string portStr = normalizePortString(parts.second);

  m_host = value_objects::Host::fromString(hostStr);

  if (!portStr.empty()) {
    m_port = value_objects::Port::fromString(portStr);
  }

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

value_objects::Host ListenDirective::getHost() const { return m_host; }

value_objects::Port ListenDirective::getPort() const { return m_port; }

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

  try {
    validateDirectiveString(directiveString);
    std::pair<std::string, std::string> parts = splitDirective(directiveString);

    std::string hostStr = normalizeHostString(parts.first);
    std::string portStr = normalizePortString(parts.second);

    if (!value_objects::Host::isValidHost(hostStr)) {
      return false;
    }

    if (!portStr.empty()) {
      try {
        value_objects::Port::fromString(portStr);
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
    const value_objects::Port& port) {
  return ListenDirective(value_objects::Host::wildcard(), port);
}

ListenDirective ListenDirective::createLocalhost(
    const value_objects::Port& port) {
  return ListenDirective(value_objects::Host::localhost(), port);
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
      throw shared::exceptions::ListenDirectiveException(
          "Missing closing bracket for IPv6 address: " + directiveString,
          shared::exceptions::ListenDirectiveException::INVALID_FORMAT);
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
  // Validation is done in constructors, but we can add additional checks here
}
/*
void ListenDirective::validate() const {
  // Additional consistency checks beyond basic constructor validation
  
  // 1. Check for invalid host/port combinations
  if (m_host.isWildcard() && m_port.getValue() == 0) {
    // Port 0 is a special case that means "any available port"
    // This is usually not valid for server listening directives
    throw shared::exceptions::ListenDirectiveException(
        "Wildcard host with port 0 is not valid for server listening",
        shared::exceptions::ListenDirectiveException::INVALID_FORMAT);
  }
  
  // 2. Check for IPv6-specific validations
  if (m_host.isIpv6() && m_host.getValue().empty()) {
    throw shared::exceptions::ListenDirectiveException(
        "IPv6 address cannot be empty",
        shared::exceptions::ListenDirectiveException::INVALID_HOST);
  }
  
  // 3. Check for localhost with non-standard ports (warning in logs, not exception)
  // This would typically be logged rather than throwing
  if (m_host.isLocalhost() && !m_port.isWellKnown()) {
    // Log a warning: "Listening on localhost with non-standard port X"
  }
  
  // 4. Validate that hostname doesn't contain port-like patterns
  if (m_host.isHostname()) {
    const std::string& hostname = m_host.getValue();
    
    // Check for common mistakes like "hostname:80" in host part
    if (hostname.find(':') != std::string::npos) {
      throw shared::exceptions::ListenDirectiveException(
          "Hostname contains colon character: " + hostname,
          shared::exceptions::ListenDirectiveException::INVALID_HOST);
    }
    
    // Check for brackets in hostname (IPv6 syntax mistake)
    if (hostname.find('[') != std::string::npos || 
        hostname.find(']') != std::string::npos) {
      throw shared::exceptions::ListenDirectiveException(
          "Hostname contains IPv6 brackets: " + hostname,
          shared::exceptions::ListenDirectiveException::INVALID_HOST);
    }
  }
  
  // 5. Check for reserved ports requiring special permissions
  if (m_port.isWellKnown() && m_port.getValue() < 1024) {
    // Ports below 1024 require root privileges on Unix-like systems
    // This is a validation that might be important for your server
    if (m_port.getValue() != Port::HTTP_PORT && 
        m_port.getValue() != Port::HTTPS_PORT &&
        m_port.getValue() != Port::FTP_PORT) {
      // Log a warning: "Port X requires root privileges"
    }
  }
  
  // 6. Validate against system-specific limitations
  // For example, check if port is already in use (though this is runtime, not compile-time)
  // This would typically be done elsewhere, but could be noted here
  
  // 7. Check for duplicate host:port combinations if this is part of a collection
  // This would require context, so might be better in ServerConfig
  
  // 8. Validate hostname length and format more strictly
  if (m_host.isHostname()) {
    const std::string& hostname = m_host.getValue();
    
    // Check total length (RFC 1035)
    if (hostname.length() > 253) {
      throw shared::exceptions::ListenDirectiveException(
          "Hostname too long (max 253 chars): " + hostname,
          shared::exceptions::ListenDirectiveException::INVALID_HOST);
    }
    
    // Check for consecutive dots
    if (hostname.find("..") != std::string::npos) {
      throw shared::exceptions::ListenDirectiveException(
          "Hostname contains consecutive dots: " + hostname,
          shared::exceptions::ListenDirectiveException::INVALID_HOST);
    }
    
    // Check for leading or trailing dots
    if (!hostname.empty() && 
        (hostname[0] == '.' || hostname[hostname.length() - 1] == '.')) {
      throw shared::exceptions::ListenDirectiveException(
          "Hostname cannot start or end with dot: " + hostname,
          shared::exceptions::ListenDirectiveException::INVALID_HOST);
    }
  }
  
  // 9. Check for IPv4-mapped IPv6 addresses if you want to normalize them
  if (m_host.isIpv6()) {
    const std::string& ipv6 = m_host.getValue();
    
    // Check for IPv4-mapped IPv6 format (::ffff:192.168.1.1)
    if (ipv6.find("::ffff:") == 0) {
      // Could optionally normalize to IPv4
      // This is more of a note than a validation error
    }
  }
  
  // 10. Ensure port is within valid range (though Port class should handle this)
  // Double-check for consistency
  if (!Port::isValidPort(m_port.getValue())) {
    throw shared::exceptions::ListenDirectiveException(
        "Port value is invalid: " + m_port.toString(),
        shared::exceptions::ListenDirectiveException::INVALID_PORT);
  }
}

maybe implement add a separate method for warnings vs errors
std::vector<std::string> ListenDirective::getWarnings() const {
  std::vector<std::string> warnings;
  
  // Localhost with non-standard port
  if (m_host.isLocalhost() && !m_port.isWellKnown()) {
    warnings.push_back("Listening on localhost with non-standard port " + 
                       m_port.toString() + 
                       " - may not be accessible from other machines");
  }
  
  // Port requiring root privileges
  if (m_port.isWellKnown() && m_port.getValue() < 1024) {
    warnings.push_back("Port " + m_port.toString() + 
                       " requires root privileges on Unix-like systems");
  }
  
  // IPv6 address (some clients might not support it)
  if (m_host.isIpv6()) {
    warnings.push_back("IPv6 address " + m_host.getValue() + 
                       " may not be supported by all clients");
  }
  
  return warnings;
}
*/

void ListenDirective::validateDirectiveString(
    const std::string& directiveString) {
  if (directiveString.empty()) {
    throw shared::exceptions::ListenDirectiveException(
        "Directive string cannot be empty",
        shared::exceptions::ListenDirectiveException::EMPTY_STRING);
  }

  static const std::string VALID_CHARS =
      "abcdefghijklmnopqrstuvwxyz"
      "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
      "0123456789"
      ".:[]-_";

  if (!shared::utils::StringUtils::containsOnly(directiveString, VALID_CHARS)) {
    throw shared::exceptions::ListenDirectiveException(
        "Invalid character in listen directive: " + directiveString,
        shared::exceptions::ListenDirectiveException::INVALID_FORMAT);
  }
}

std::string ListenDirective::normalizeHostString(
    const std::string& hostString) {
  if (hostString.empty()) {
    return DEFAULT_HOST;
  }

  if (hostString == "*") {
    return value_objects::Host::WILDCARD_IPV4;
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
    return port <= value_objects::Port::MAX_PORT;
  } catch (...) {
    return false;
  }
}

}  // namespace entities
}  // namespace domain
