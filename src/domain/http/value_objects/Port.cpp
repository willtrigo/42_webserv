/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Port.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: umeneses <umeneses@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/19 19:13:44 by dande-je          #+#    #+#             */
/*   Updated: 2026/01/02 22:49:34 by umeneses         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "domain/http/value_objects/Port.hpp"
#include "domain/http/exceptions/PortException.hpp"
#include "domain/shared/utils/StringUtils.hpp"

#include <cctype>
#include <cstdlib>
#include <sstream>

namespace domain {
namespace http {
namespace value_objects {

Port::Port() : m_value(DEFAULT_PORT) {}

Port::Port(unsigned int port) : m_value(port) {
  // Special case: Port(0) is allowed as a sentinel value for "no port specified"
  // Used internally by Uri and other components
  if (port != 0 && port > MAX_PORT) {
    std::ostringstream oss;
    oss << "Port must be between " << MIN_PORT << " and " << MAX_PORT;
    throw exceptions::PortException(
        oss.str(),
        exceptions::PortException::OUT_OF_RANGE);
  }
  // Only validate if not using sentinel value
  if (port != 0) {
    validate();
  }
}

Port::Port(const std::string& portString) : m_value(DEFAULT_PORT) {
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

bool Port::isValidPort(unsigned int port) {
  return port >= MIN_PORT && port <= MAX_PORT;
}

void Port::validate() const {
  if (!isValidPort(m_value)) {
    std::ostringstream oss;
    oss << "Port value out of range: " << m_value
        << " (valid range: " << MIN_PORT << "-" << MAX_PORT << ")";
    throw exceptions::PortException(
        oss.str(), exceptions::PortException::OUT_OF_RANGE);
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
  return shared::utils::StringUtils::isAllDigits(str);
}

unsigned int Port::parsePortString(const std::string& portString) {
  if (portString.empty()) {
    throw exceptions::PortException(
        "Port string cannot be empty",
        exceptions::PortException::EMPTY_STRING);
  }

  if (!isAllDigits(portString)) {
    throw exceptions::PortException(
        "Port contains non-digit characters: '" + portString + "'",
        exceptions::PortException::NON_DIGIT_CHARACTER);
  }

  try {
    unsigned long result = shared::utils::StringUtils::toUnsignedLong(
        portString, shared::utils::StringUtils::BASE_DECIMAL);

    if (result == 0 || result > static_cast<unsigned long>(MAX_PORT)) {
      std::ostringstream oss;
      oss << "Port value out of range: '" << portString
          << "' (valid range: " << MIN_PORT << "-" << MAX_PORT << ")";
      throw exceptions::PortException(
          oss.str(), exceptions::PortException::OUT_OF_RANGE);
    }

    return static_cast<unsigned int>(result);
  } catch (const std::invalid_argument& exception) {
    throw exceptions::PortException(
        "Failed to convert port string to number: '" + portString + "'",
        exceptions::PortException::CONVERSION_FAILED);
  } catch (const std::out_of_range& exception) {
    std::ostringstream oss;
    oss << "Port value out of range: '" << portString << "' (max: " << MAX_PORT
        << ")";
    throw exceptions::PortException(
        oss.str(), exceptions::PortException::OUT_OF_RANGE);
  }
}

}  // namespace value_objects
}  // namespace http
}  // namespace domain
