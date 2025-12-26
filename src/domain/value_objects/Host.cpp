/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Host.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/24 14:45:29 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/24 14:56:55 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "domain/value_objects/Host.hpp"
#include "shared/exceptions/HostException.hpp"

#include <cctype>
#include <sstream>
#include <vector>

namespace domain {
namespace value_objects {

const std::string Host::LOCALHOST_IPV4 = "127.0.0.1";
const std::string Host::LOCALHOST_IPV6 = "::1";
const std::string Host::WILDCARD_IPV4 = "0.0.0.0";
const std::string Host::WILDCARD_IPV6 = "::";
const std::string Host::WILDCARD_SHORT = "*";

const std::string Host::VALID_HOSTNAME_CHARS =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789"
    "-";

Host::Host() : m_type(TYPE_UNKNOWN) {}

Host::Host(const std::string& host) : m_value(host), m_type(TYPE_UNKNOWN) {
  validateHostString(host);
  parseHost(host);
}

Host::Host(const Host& other) : m_value(other.m_value), m_type(other.m_type) {}

Host::~Host() {}

Host& Host::operator=(const Host& other) {
  if (this != &other) {
    m_value = other.m_value;
    m_type = other.m_type;
  }
  return *this;
}

std::string Host::getValue() const { return m_value; }

Host::HostType Host::getType() const { return m_type; }

std::string Host::getTypeString() const {
  switch (m_type) {
    case TYPE_IPV4:
      return "IPv4";
    case TYPE_IPV6:
      return "IPv6";
    case TYPE_HOSTNAME:
      return "Hostname";
    case TYPE_WILDCARD:
      return "Wildcard";
    default:
      return "Unknown";
  }
}

bool Host::isValidHost(const std::string& host) {
  try {
    validateHostString(host);
    return true;
  } catch (const shared::exceptions::HostException&) {
    return false;
  }
}

bool Host::isValidIpv4(const std::string& ipv4) { return parseIpv4(ipv4); }

bool Host::isValidIpv6(const std::string& ipv6) { return parseIpv6(ipv6); }

bool Host::isValidHostname(const std::string& hostname) {
  return parseHostname(hostname);
}

bool Host::isIpv4() const { return m_type == TYPE_IPV4; }

bool Host::isIpv6() const { return m_type == TYPE_IPV6; }

bool Host::isLocalhost() const {
  return (m_value == LOCALHOST_IPV4 || m_value == LOCALHOST_IPV6 ||
          m_value == "localhost");
}

bool Host::isWildcard() const {
  return (m_type == TYPE_WILDCARD || m_value == WILDCARD_IPV4 ||
          m_value == WILDCARD_IPV6 || m_value == WILDCARD_SHORT);
}

bool Host::isHostname() const { return m_type == TYPE_HOSTNAME; }

bool Host::operator==(const Host& other) const {
  return m_value == other.m_value;
}

bool Host::operator!=(const Host& other) const { return !(*this == other); }

bool Host::operator<(const Host& other) const {
  return m_value < other.m_value;
}

Host Host::fromString(const std::string& host) { return Host(host); }

Host Host::localhost() { return Host(LOCALHOST_IPV4); }

Host Host::wildcard() { return Host(WILDCARD_IPV4); }

void Host::parseHost(const std::string& host) { m_type = determineType(host); }

void Host::validate() const { validateHostString(m_value); }

void Host::validateHostString(const std::string& host) {
  if (host.empty()) {
    throw shared::exceptions::HostException(
        "Host cannot be empty", shared::exceptions::HostException::EMPTY_HOST);
  }

  if (host.length() > MAX_HOST_LENGTH) {
    std::ostringstream oss;
    oss << "Host too long: " << host.length()
        << " characters (max: " << MAX_HOST_LENGTH << ")";
    throw shared::exceptions::HostException(
        oss.str(), shared::exceptions::HostException::TOO_LONG);
  }

  HostType type = determineType(host);
  if (type == TYPE_UNKNOWN) {
    throw shared::exceptions::HostException(
        "Invalid host format: '" + host + "'",
        shared::exceptions::HostException::INVALID_FORMAT);
  }
}

Host::HostType Host::determineType(const std::string& host) {
  if (host == WILDCARD_SHORT) {
    return TYPE_WILDCARD;
  }

  if (parseIpv4(host)) {
    return TYPE_IPV4;
  }

  if (parseIpv6(host)) {
    return TYPE_IPV6;
  }

  if (parseHostname(host)) {
    return TYPE_HOSTNAME;
  }

  return TYPE_UNKNOWN;
}

bool Host::parseIpv4(const std::string& ipv4) {
  if (ipv4.empty()) return false;

  std::vector<std::string> octets;
  std::string octet;
  std::istringstream iss(ipv4);
  int octetCount = 0;

  while (std::getline(iss, octet, '.') != 0) {
    if (octetCount >= 4) return false;
    if (!isValidIpv4Octet(octet)) return false;
    ++octetCount;
  }

  return (octetCount == 4);
}

bool Host::parseIpv6(const std::string& ipv6) {
  if (ipv6.empty() || ipv6.length() > MAX_IPV6_LENGTH) return false;

  size_t colonCount = 0;
  size_t doubleColonCount = 0;
  bool prevWasColon = false;

  for (std::size_t i = 0; i < ipv6.length(); ++i) {
    char chr = ipv6[i];

    if (chr == ':') {
      ++colonCount;
      if (prevWasColon) {
        ++doubleColonCount;
        if (doubleColonCount > MAX_DOUBLE_COLONS) return false;
      }
      prevWasColon = true;
    } else if (std::isxdigit(chr) != 0) {
      prevWasColon = false;
    } else {
      return false;
    }
  }

  return (colonCount >= MIN_IPV6_COLONS && colonCount <= MAX_IPV6_COLONS &&
          doubleColonCount <= MAX_DOUBLE_COLONS);
}

bool Host::parseHostname(const std::string& hostname) {
  if (hostname.empty() || hostname.length() > MAX_HOST_LENGTH) {
    return false;
  }

  std::vector<std::string> labels;
  std::string label;
  std::istringstream iss(hostname);

  while (std::getline(iss, label, LABEL_SEPARATOR) != 0) {
    if (label.empty()) return false;
    if (label.length() > MAX_LABEL_LENGTH) return false;
    if (!isValidHostnameLabel(label)) return false;
    labels.push_back(label);
  }

  if (labels.empty()) return false;

  std::string lastLabel = labels.back();
  return lastLabel.find_first_not_of("0123456789") != std::string::npos;
}

bool Host::isValidIpv4Octet(const std::string& octet) {
  if (octet.empty() || octet.length() > 3) return false;

  for (std::size_t i = 0; i < octet.length(); ++i) {
    if (std::isdigit(octet[i]) == 0) return false;
  }

  if (octet.length() > 1 && octet[0] == '0') return false;

  std::istringstream iss(octet);
  size_t value;
  iss >> value;

  return (value >= MIN_IPV4_OCTET && value <= MAX_IPV4_OCTET);
}

bool Host::isValidHostnameLabel(const std::string& label) {
  if (label.empty() || label.length() > MAX_LABEL_LENGTH) return false;

  for (std::size_t i = 0; i < label.length(); ++i) {
    char chr = label[i];

    if (VALID_HOSTNAME_CHARS.find(chr) == std::string::npos) {
      return false;
    }

    if (chr == '-' && (i == 0 || i == label.length() - 1)) {
      return false;
    }
  }

  return true;
}

bool Host::isAllDigits(const std::string& str) {
  if (str.empty()) return false;

  for (std::size_t i = 0; i < str.length(); ++i) {
    if (std::isdigit(str[i]) == 0) return false;
  }

  return true;
}

}  // namespace value_objects
}  // namespace domain
