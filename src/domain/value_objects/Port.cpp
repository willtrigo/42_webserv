/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Port.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/19 19:13:44 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/20 00:40:29 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "domain/value_objects/Port.hpp"
#include "shared/exceptions/PortException.hpp"

#include <cctype>
#include <cstdlib>
#include <sstream>

namespace domain {
namespace value_objects {

Port::Port() : m_value(MIN_PORT) {}

Port::Port(unsigned int port) : m_value(port) { validate(); }

Port::Port(const std::string& portString) : m_value(MIN_PORT) {
  m_value = parsePortString(portString);
  validate();
}

Port::Port(const Port& other) : m_value(other.m_value) {}

Port::~Port() {}

Port& Port::operator=(const Port& other) {
  if (this != &other) {
    m_value = other.m_value;
  }
  return *this;
}

unsigned int Port::getValue() const { return m_value; }

bool Port::isValidPort(unsigned int port) { return port <= MAX_PORT; }

void Port::validate() const {
  if (isValidPort(m_value)) {
    std::ostringstream oss;
    oss << "Port value out of range: " << m_value
        << " (valid range: " << MIN_PORT << "-" << MAX_PORT << ")";
    throw shared::exceptions::PortException(
        oss.str(), shared::exceptions::PortException::OUT_OF_RANGE);
  }
}

bool Port::operator==(const Port& other) const {
  return m_value == other.m_value;
}

bool Port::operator!=(const Port& other) const {
  return m_value != other.m_value;
}

bool Port::operator<(const Port& other) const {
  return m_value < other.m_value;
}

bool Port::operator<=(const Port& other) const {
  return m_value <= other.m_value;
}

bool Port::operator>(const Port& other) const {
  return m_value > other.m_value;
}

bool Port::operator>=(const Port& other) const {
  return m_value >= other.m_value;
}

Port Port::httpPort() { return Port(HTTP_PORT); }

Port Port::httpsPort() { return Port(HTTPS_PORT); }

Port Port::ftpPort() { return Port(FTP_PORT); }

Port Port::sshPort() { return Port(SSH_PORT); }

Port Port::smtpPort() { return Port(SMTP_PORT); }

Port Port::dnsPort() { return Port(DNS_PORT); }

Port Port::fromString(const std::string& portString) {
  return Port(portString);
}

bool Port::isWellKnown() const { return m_value <= WELL_KNOWN_MAX; }

bool Port::isRegistered() const {
  return m_value >= REGISTERED_MIN && m_value <= REGISTERED_MAX;
}

bool Port::isDynamic() const {
  return m_value >= DYNAMIC_MIN && m_value <= DYNAMIC_MAX;
}

bool Port::isAllDigits(const std::string& str) {
  for (std::size_t i = 0; i < str.size(); ++i) {
    if (std::isdigit(static_cast<unsigned char>(str[i])) == 0) {
      return false;
    }
  }
  return true;
}

unsigned int Port::parsePortString(const std::string& portString) {
  if (portString.empty()) {
    throw shared::exceptions::PortException(
        "Port string cannot be empty",
        shared::exceptions::PortException::EMPTY_STRING);
  }

  if (!isAllDigits(portString)) {
    throw shared::exceptions::PortException(
        "Port contains non-digit characters: '" + portString + "'",
        shared::exceptions::PortException::NON_DIGIT_CHARACTER);
  }

  if (portString.size() > 1 && portString[0] == '0') {
    throw shared::exceptions::PortException(
        "Port cannot have leading zero: '" + portString + "'",
        shared::exceptions::PortException::INVALID_STRING);
  }

  char* endptr = NULL;
  unsigned long result =
      std::strtoul(portString.c_str(), &endptr, BASE_DECIMAL);

  if (endptr == portString.c_str() || *endptr != '\0') {
    throw shared::exceptions::PortException(
        "Failed to convert port string to number: '" + portString + "'",
        shared::exceptions::PortException::CONVERSION_FAILED);
  }

  if (result > static_cast<unsigned long>(MAX_PORT)) {
    std::ostringstream oss;
    oss << "Port value out of range: '" << portString << "' (max: " << MAX_PORT
        << ")";
    throw shared::exceptions::PortException(
        oss.str(), shared::exceptions::PortException::OUT_OF_RANGE);
  }

  return static_cast<unsigned int>(result);
}

}  // namespace value_objects
}  // namespace domain
