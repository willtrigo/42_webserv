/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Size.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: umeneses <umeneses@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/20 10:04:57 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/27 20:12:13 by umeneses         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "domain/filesystem/exceptions/SizeException.hpp"
#include "domain/filesystem/value_objects/Size.hpp"

#include <cctype>
#include <cstdlib>
#include <sstream>

namespace domain {
namespace filesystem {
namespace value_objects {

const char* Size::UNIT_STRINGS[] = {"B", "K", "M", "G", "UNKNOWN"};

Size::Size() : m_bytes(0) {}

Size::Size(std::size_t bytes) : m_bytes(bytes) { validate(); }

Size::Size(const std::string& sizeString) {
  m_bytes = parseSizeString(sizeString);
  validate();
}

Size::Size(const Size& other) : m_bytes(other.m_bytes) {}

Size::~Size() {}

Size& Size::operator=(const Size& other) {
  if (this != &other) {
    m_bytes = other.m_bytes;
  }
  return *this;
}

std::size_t Size::getBytes() const { return m_bytes; }

std::size_t Size::getKilobytes() const { return m_bytes / BYTES_PER_KB; }

std::size_t Size::getMegabytes() const { return m_bytes / BYTES_PER_MB; }

std::size_t Size::getGigabytes() const { return m_bytes / BYTES_PER_GB; }

std::string Size::toString() const {
  if (m_bytes >= BYTES_PER_GB) {
    return toString(UNIT_GIGABYTES);
  }
  if (m_bytes >= BYTES_PER_MB) {
    return toString(UNIT_MEGABYTES);
  }
  if (m_bytes >= BYTES_PER_KB) {
    return toString(UNIT_KILOBYTES);
  }
  return toString(UNIT_BYTES);
}

std::string Size::toString(Unit unit) const {
  std::ostringstream oss;

  switch (unit) {
    case UNIT_BYTES:
      oss << m_bytes << unitToString(unit);
      break;
    case UNIT_KILOBYTES:
      oss << (m_bytes / BYTES_PER_KB) << unitToString(unit);
      break;
    case UNIT_MEGABYTES:
      oss << (m_bytes / BYTES_PER_MB) << unitToString(unit);
      break;
    case UNIT_GIGABYTES:
      oss << (m_bytes / BYTES_PER_GB) << unitToString(unit);
      break;
    default:
      oss << m_bytes << "B";
  }

  return oss.str();
}

bool Size::isValidSizeString(const std::string& sizeString) {
  try {
    parseSizeString(sizeString);
    return true;
  } catch (const exceptions::SizeException&) {
    return false;
  }
}

bool Size::isValidBytes(std::size_t bytes) { return bytes <= MAX_SIZE; }

void Size::validate() const {
  if (m_bytes > MAX_SIZE) {
    std::ostringstream oss;
    oss << "Size value overflow: " << m_bytes << " (max: " << MAX_SIZE << ")";
    throw exceptions::SizeException(oss.str(),
                                    exceptions::SizeException::OVERFLOW);
  }
}

bool Size::operator==(const Size& other) const {
  return m_bytes == other.m_bytes;
}

bool Size::operator!=(const Size& other) const {
  return m_bytes != other.m_bytes;
}

bool Size::operator<(const Size& other) const {
  return m_bytes < other.m_bytes;
}

bool Size::operator<=(const Size& other) const {
  return m_bytes <= other.m_bytes;
}

bool Size::operator>(const Size& other) const {
  return m_bytes > other.m_bytes;
}

bool Size::operator>=(const Size& other) const {
  return m_bytes >= other.m_bytes;
}

Size Size::operator+(const Size& other) const {
  if (m_bytes > MAX_SIZE - other.m_bytes) {
    throw exceptions::SizeException("Addition would cause overflow",
                                    exceptions::SizeException::OVERFLOW);
  }
  return Size(m_bytes + other.m_bytes);
}

Size Size::operator-(const Size& other) const {
  if (other.m_bytes > m_bytes) {
    throw exceptions::SizeException("Subtraction would result in negative size",
                                    exceptions::SizeException::NEGATIVE_VALUE);
  }
  return Size(m_bytes - other.m_bytes);
}

Size& Size::operator+=(const Size& other) {
  if (m_bytes > MAX_SIZE - other.m_bytes) {
    throw exceptions::SizeException("Addition would cause overflow",
                                    exceptions::SizeException::OVERFLOW);
  }
  m_bytes += other.m_bytes;
  return *this;
}

Size& Size::operator-=(const Size& other) {
  if (other.m_bytes > m_bytes) {
    throw exceptions::SizeException("Subtraction would result in negative size",
                                    exceptions::SizeException::NEGATIVE_VALUE);
  }
  m_bytes -= other.m_bytes;
  return *this;
}

Size Size::fromString(const std::string& sizeString) {
  return Size(sizeString);
}

Size Size::fromKilobytes(std::size_t kilobytes) {
  if (isOverflowMultiplication(kilobytes, BYTES_PER_KB)) {
    throw exceptions::SizeException(
        "Kilobytes to bytes conversion would overflow",
        exceptions::SizeException::OVERFLOW);
  }
  return Size(kilobytes * BYTES_PER_KB);
}

Size Size::fromMegabytes(std::size_t megabytes) {
  if (isOverflowMultiplication(megabytes, BYTES_PER_MB)) {
    throw exceptions::SizeException(
        "Megabytes to bytes conversion would overflow",
        exceptions::SizeException::OVERFLOW);
  }
  return Size(megabytes * BYTES_PER_MB);
}

Size Size::fromGigabytes(std::size_t gigabytes) {
  if (isOverflowMultiplication(gigabytes, BYTES_PER_GB)) {
    throw exceptions::SizeException(
        "Gigabytes to bytes conversion would overflow",
        exceptions::SizeException::OVERFLOW);
  }
  return Size(gigabytes * BYTES_PER_GB);
}

Size Size::zero() { return Size(0); }

Size Size::kilobyte() { return fromKilobytes(1); }

Size Size::megabyte() { return fromMegabytes(1); }

Size Size::gigabyte() { return fromGigabytes(1); }

bool Size::isZero() const { return m_bytes == 0; }

std::size_t Size::parseSizeString(const std::string& sizeString) {
  if (sizeString.empty()) {
    throw exceptions::SizeException("Size string cannot be empty",
                                    exceptions::SizeException::EMPTY_STRING);
  }

  std::string::size_type pos = 0;
  std::size_t number = extractNumber(sizeString, pos);
  Unit unit = extractUnit(sizeString, pos);

  return convertToBytes(number, unit);
}

// TODO: check if this func can use StringUtils
std::size_t Size::extractNumber(const std::string& sizeString,
                                std::string::size_type& pos) {
  while (pos < sizeString.size() &&
         (std::isspace(static_cast<unsigned char>(sizeString[pos])) != 0)) {
    ++pos;
  }

  if (pos >= sizeString.size()) {
    throw exceptions::SizeException(
        "No number found in size string: '" + sizeString + "'",
        exceptions::SizeException::INVALID_FORMAT);
  }

  std::string::size_type start = pos;
  while (pos < sizeString.size() &&
         (std::isdigit(static_cast<unsigned char>(sizeString[pos])) != 0)) {
    ++pos;
  }

  if (pos == start) {
    throw exceptions::SizeException(
        "No digits found in size string: '" + sizeString + "'",
        exceptions::SizeException::INVALID_FORMAT);
  }

  std::string numberStr = sizeString.substr(start, pos - start);

  char* endptr = NULL;
  unsigned long result = std::strtoul(numberStr.c_str(), &endptr, BASE_DECIMAL);

  if (endptr == numberStr.c_str() || *endptr != '\0') {
    throw exceptions::SizeException(
        "Failed to convert number: '" + numberStr + "'",
        exceptions::SizeException::CONVERSION_FAILED);
  }

  if (result > static_cast<unsigned long>(MAX_SIZE)) {
    throw exceptions::SizeException("Number too large: " + numberStr,
                                    exceptions::SizeException::OVERFLOW);
  }

  return static_cast<std::size_t>(result);
}

Size::Unit Size::extractUnit(const std::string& sizeString,
                             std::string::size_type pos) {
  while (pos < sizeString.size() &&
         (std::isspace(static_cast<unsigned char>(sizeString[pos])) != 0)) {
    ++pos;
  }

  if (pos >= sizeString.size()) {
    return UNIT_BYTES;
  }

  char unitChar = static_cast<char>(
      std::tolower(static_cast<unsigned char>(sizeString[pos])));

  Unit unit = charToUnit(unitChar);
  if (unit == UNIT_UNKNOWN) {
    std::ostringstream oss;
    oss << "Unknown unit: '" << sizeString[pos] << "' in size string: '"
        << sizeString + "'";
    throw exceptions::SizeException(oss.str(),
                                    exceptions::SizeException::UNKNOWN_UNIT);
  }

  if (pos + 1 < sizeString.size()) {
    char nextChar = sizeString[pos + 1];
    if (std::tolower(static_cast<unsigned char>(nextChar)) != 'b' &&
        (std::isspace(static_cast<unsigned char>(nextChar)) == 0)) {
      throw exceptions::SizeException(
          "Extra characters after unit in size string: '" + sizeString + "'",
          exceptions::SizeException::INVALID_FORMAT);
    }
  }

  return unit;
}

std::size_t Size::convertToBytes(std::size_t number, Unit unit) {
  switch (unit) {
    case UNIT_BYTES:
      return number;
    case UNIT_KILOBYTES:
      if (isOverflowMultiplication(number, BYTES_PER_KB)) {
        throw exceptions::SizeException(
            "Kilobytes to bytes conversion would overflow",
            exceptions::SizeException::OVERFLOW);
      }
      return number * BYTES_PER_KB;
    case UNIT_MEGABYTES:
      if (isOverflowMultiplication(number, BYTES_PER_MB)) {
        throw exceptions::SizeException(
            "Megabytes to bytes conversion would overflow",
            exceptions::SizeException::OVERFLOW);
      }
      return number * BYTES_PER_MB;
    case UNIT_GIGABYTES:
      if (isOverflowMultiplication(number, BYTES_PER_GB)) {
        throw exceptions::SizeException(
            "Gigabytes to bytes conversion would overflow",
            exceptions::SizeException::OVERFLOW);
      }
      return number * BYTES_PER_GB;
    default:
      return number;
  }
}

bool Size::isOverflowMultiplication(std::size_t multiplicand,
                                    std::size_t multiplier) {
  if (multiplicand == 0 || multiplier == 0) return false;
  return multiplicand > MAX_SIZE / multiplier;
}

std::string Size::unitToString(Unit unit) {
  if (unit >= UNIT_BYTES && unit <= UNIT_GIGABYTES) {
    return UNIT_STRINGS[unit];
  }
  return "B";
}

char Size::unitToChar(Unit unit) {
  switch (unit) {
    case UNIT_BYTES:
      return 'b';
    case UNIT_KILOBYTES:
      return 'k';
    case UNIT_MEGABYTES:
      return 'm';
    case UNIT_GIGABYTES:
      return 'g';
    default:
      return 'b';
  }
}

Size::Unit Size::charToUnit(char chr) {
  switch (std::tolower(static_cast<unsigned char>(chr))) {
    case 'b':
      return UNIT_BYTES;
    case 'k':
      return UNIT_KILOBYTES;
    case 'm':
      return UNIT_MEGABYTES;
    case 'g':
      return UNIT_GIGABYTES;
    default:
      return UNIT_UNKNOWN;
  }
}

}  // namespace value_objects
}  // namespace filesystem
}  // namespace domain
