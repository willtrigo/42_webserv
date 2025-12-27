/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PermissionException.cpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/21 00:07:21 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/27 03:53:18 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "domain/filesystem/exceptions/PermissionException.hpp"

#include <sstream>

namespace domain {
namespace filesystem {
namespace exceptions {

const std::pair<PermissionException::ErrorCode, std::string>
    PermissionException::K_CODE_MSGS[] = {
        std::make_pair(PermissionException::EMPTY_STRING,
                       "Permission string cannot be empty"),
        std::make_pair(PermissionException::INVALID_FORMAT,
                       "Invalid permission format"),
        std::make_pair(PermissionException::INVALID_CHARACTER,
                       "Permission contains invalid characters"),
        std::make_pair(PermissionException::OUT_OF_RANGE,
                       "Permission value out of range"),
        std::make_pair(PermissionException::INVALID_OCTAL,
                       "Invalid octal permission"),
        std::make_pair(PermissionException::CONVERSION_FAILED,
                       "Failed to convert permission string")};

PermissionException::PermissionException(const std::string& msg, ErrorCode code)
    : BaseException("", static_cast<int>(code)) {
  std::ostringstream oss;
  oss << getErrorMsg(code) << ": " << msg;
  this->m_whatMsg = oss.str();
}

PermissionException::PermissionException(const PermissionException& other)
    : BaseException(other) {}

PermissionException::~PermissionException() throw() {}

PermissionException& PermissionException::operator=(
    const PermissionException& other) {
  if (this != &other) {
    BaseException::operator=(other);
  }
  return *this;
}

std::string PermissionException::getErrorMsg(
    PermissionException::ErrorCode code) {
  for (int index = 0; index < CODE_COUNT; ++index) {
    if (K_CODE_MSGS[index].first == code) {
      return K_CODE_MSGS[index].second;
    }
  }
  return "unknown permission error";
}

}  // namespace exceptions
}  // namespace filesystem
}  // namespace domain
