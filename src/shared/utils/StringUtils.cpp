/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   StringUtils.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/26 03:34:16 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/26 03:39:35 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shared/utils/StringUtils.hpp"

#include <cctype>
#include <climits>
#include <cstdlib>
#include <sstream>
#include <stdexcept>

namespace shared {
namespace utils {

const int StringUtils::BASE_DECIMAL;
const int StringUtils::BASE_HEXADECIMAL;
const int StringUtils::BASE_OCTAL;

StringUtils::StringUtils(const StringUtils& /*unused*/) {}

StringUtils::~StringUtils() {}

StringUtils& StringUtils::operator=(const StringUtils& /*unused*/) {
  throw std::runtime_error("TerminalColor assignment is not allowed");
}

bool StringUtils::isAllDigits(const std::string& str) {
  if (str.empty()) return false;

  for (std::size_t i = 0; i < str.size(); ++i) {
    if (std::isdigit(static_cast<unsigned char>(str[i])) == 0) {
      return false;
    }
  }
  return true;
}

bool StringUtils::isAllHexDigits(const std::string& str) {
  if (str.empty()) return false;

  for (std::size_t i = 0; i < str.size(); ++i) {
    char chr = str[i];
    if ((std::isdigit(static_cast<unsigned char>(chr)) == 0) &&
        (chr < 'a' || chr > 'f') && (chr < 'A' || chr > 'F')) {
      return false;
    }
  }
  return true;
}

bool StringUtils::isAllOctalDigits(const std::string& str) {
  if (str.empty()) return false;

  for (std::size_t i = 0; i < str.size(); ++i) {
    char chr = str[i];
    if (chr < '0' || chr > '7') {
      return false;
    }
  }
  return true;
}

bool StringUtils::isAllAlphanumeric(const std::string& str) {
  if (str.empty()) return false;

  for (std::size_t i = 0; i < str.size(); ++i) {
    if (std::isalnum(static_cast<unsigned char>(str[i])) == 0) {
      return false;
    }
  }
  return true;
}

bool StringUtils::containsOnly(const std::string& str,
                               const std::string& validChars) {
  if (str.empty()) return true;

  for (std::size_t i = 0; i < str.size(); ++i) {
    if (validChars.find(str[i]) == std::string::npos) {
      return false;
    }
  }
  return true;
}

unsigned long StringUtils::toUnsignedLong(const std::string& str, int base) {
  if (!isValidBase(base)) {
    std::ostringstream oss;
    oss << "Invalid base: " << base;
    throw std::invalid_argument(oss.str());
  }

  char* endptr = NULL;
  unsigned long result = std::strtoul(str.c_str(), &endptr, base);

  if (endptr == str.c_str() || *endptr != '\0') {
    std::ostringstream oss;
    oss << "Failed to convert string to unsigned long: '" << str << "'";
    throw std::invalid_argument(oss.str());
  }

  return result;
}

long StringUtils::toLong(const std::string& str, int base) {
  if (!isValidBase(base)) {
    std::ostringstream oss;
    oss << "Invalid base: " << base;
    throw std::invalid_argument(oss.str());
  }

  char* endptr = NULL;
  long result = std::strtol(str.c_str(), &endptr, base);

  if (endptr == str.c_str() || *endptr != '\0') {
    std::ostringstream oss;
    oss << "Failed to convert string to long: '" << str << "'";
    throw std::invalid_argument(oss.str());
  }

  return result;
}

int StringUtils::toInt(const std::string& str, int base) {
  long result = toLong(str, base);

  if (result < static_cast<long>(INT_MIN) ||
      result > static_cast<long>(INT_MAX)) {
    std::ostringstream oss;
    oss << "Integer overflow/underflow for value: " << result;
    throw std::out_of_range(oss.str());
  }

  return static_cast<int>(result);
}

std::string StringUtils::trim(const std::string& str) {
  return trimRight(trimLeft(str));
}

std::string StringUtils::trimLeft(const std::string& str) {
  std::size_t start = str.find_first_not_of(" \t\n\r\f\v");
  return (start == std::string::npos) ? "" : str.substr(start);
}

std::string StringUtils::trimRight(const std::string& str) {
  std::size_t end = str.find_last_not_of(" \t\n\r\f\v");
  return (end == std::string::npos) ? "" : str.substr(0, end + 1);
}

bool StringUtils::startsWith(const std::string& str,
                             const std::string& prefix) {
  if (prefix.length() > str.length()) return false;
  return str.compare(0, prefix.length(), prefix) == 0;
}

bool StringUtils::endsWith(const std::string& str, const std::string& suffix) {
  if (suffix.length() > str.length()) return false;
  return str.compare(str.length() - suffix.length(), suffix.length(), suffix) ==
         0;
}

std::string StringUtils::toUpperCase(const std::string& str) {
  std::string result = str;
  for (std::size_t i = 0; i < result.length(); ++i) {
    result[i] =
        static_cast<char>(std::toupper(static_cast<unsigned char>(result[i])));
  }
  return result;
}

std::string StringUtils::toLowerCase(const std::string& str) {
  std::string result = str;
  for (std::size_t i = 0; i < result.length(); ++i) {
    result[i] =
        static_cast<char>(std::tolower(static_cast<unsigned char>(result[i])));
  }
  return result;
}

bool StringUtils::isValidBase(int base) {
  return base == BASE_DECIMAL || base == BASE_HEXADECIMAL || base == BASE_OCTAL;
}

}  // namespace utils
}  // namespace shared
