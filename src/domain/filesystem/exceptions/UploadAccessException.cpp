/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   UploadAccessException.cpp                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/28 20:14:15 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/28 20:41:45 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "domain/filesystem/exceptions/UploadAccessException.hpp"

#include <sstream>

namespace domain {
namespace filesystem {
namespace exceptions {

const std::pair<UploadAccessException::ErrorCode, std::string>
    UploadAccessException::K_CODE_MSGS[] = {
        std::make_pair(UploadAccessException::EMPTY_STRING,
                       "Upload access string cannot be empty"),
        std::make_pair(UploadAccessException::INVALID_FORMAT,
                       "Invalid upload access format"),
        std::make_pair(
            UploadAccessException::INVALID_WHO,
            "Invalid who specifier (must be 'user', 'group', or 'all')"),
        std::make_pair(UploadAccessException::INVALID_PERMISSION,
                       "Invalid permission character (must be 'r' or 'w')"),
        std::make_pair(UploadAccessException::OUT_OF_RANGE,
                       "Upload access bits out of range"),
        std::make_pair(UploadAccessException::DUPLICATE_PERMISSION,
                       "Duplicate permission character in token")};

UploadAccessException::UploadAccessException(const std::string& msg,
                                             ErrorCode code)
    : BaseException("", static_cast<int>(code)) {
  std::ostringstream oss;
  oss << getErrorMsg(code) << ": " << msg;
  this->m_whatMsg = oss.str();
}

UploadAccessException::UploadAccessException(const UploadAccessException& other)
    : BaseException(other) {}

UploadAccessException::~UploadAccessException() throw() {}

UploadAccessException& UploadAccessException::operator=(
    const UploadAccessException& other) {
  if (this != &other) {
    BaseException::operator=(other);
  }
  return *this;
}

std::string UploadAccessException::getErrorMsg(
    UploadAccessException::ErrorCode code) {
  for (int index = 0; index < CODE_COUNT; ++index) {
    if (K_CODE_MSGS[index].first == code) {
      return K_CODE_MSGS[index].second;
    }
  }
  return "unknown upload access error";
}

}  // namespace exceptions
}  // namespace filesystem
}  // namespace domain
