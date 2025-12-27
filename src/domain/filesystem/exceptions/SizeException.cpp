/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SizeException.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/20 09:56:36 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/27 03:46:03 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "domain/filesystem/exceptions/SizeException.hpp"

#include <sstream>

namespace domain {
namespace filesystem {
namespace exceptions {

const std::pair<SizeException::ErrorCode, std::string>
    SizeException::K_CODE_MSGS[] = {
        std::make_pair(SizeException::EMPTY_STRING,
                       "Size string cannot be empty"),
        std::make_pair(SizeException::INVALID_FORMAT, "Invalid size format"),
        std::make_pair(SizeException::UNKNOWN_UNIT, "Unknown size unit"),
        std::make_pair(SizeException::NEGATIVE_VALUE,
                       "Size cannot be negative"),
        std::make_pair(SizeException::OVERFLOW, "Size value overflow"),
        std::make_pair(SizeException::CONVERSION_FAILED,
                       "Failed to convert size string")};

SizeException::SizeException(const std::string& msg, ErrorCode code)
    : BaseException("", static_cast<int>(code)) {
  std::ostringstream oss;
  oss << getErrorMsg(code) << ": " << msg;
  this->m_whatMsg = oss.str();
}

SizeException::SizeException(const SizeException& other)
    : BaseException(other) {}

SizeException::~SizeException() throw() {}

SizeException& SizeException::operator=(const SizeException& other) {
  if (this != &other) {
    BaseException::operator=(other);
  }
  return *this;
}

std::string SizeException::getErrorMsg(SizeException::ErrorCode code) {
  for (int i = 0; i < CODE_COUNT; ++i) {
    if (K_CODE_MSGS[i].first == code) {
      return K_CODE_MSGS[i].second;
    }
  }
  return "unknown size error";
}

}  // namespace exceptions
}  // namespace filesystem
}  // namespace domain
