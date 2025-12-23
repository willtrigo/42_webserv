/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   DirectoryListerException.cpp                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/22 21:33:33 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/22 21:33:39 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shared/exceptions/DirectoryListerException.hpp"

#include <sstream>

namespace shared {
namespace exceptions {

const std::pair<DirectoryListerException::ErrorCode, std::string>
    DirectoryListerException::K_CODE_MSGS[] = {
        std::make_pair(DirectoryListerException::INVALID_DEPENDENCY,
                       "Invalid dependency in DirectoryLister"),
        std::make_pair(DirectoryListerException::INVALID_DIRECTORY,
                       "Invalid directory for listing"),
        std::make_pair(DirectoryListerException::DIRECTORY_NOT_FOUND,
                       "Directory not found"),
        std::make_pair(DirectoryListerException::NOT_A_DIRECTORY,
                       "Path is not a directory"),
        std::make_pair(DirectoryListerException::PERMISSION_DENIED,
                       "Permission denied for directory access"),
        std::make_pair(DirectoryListerException::CANNOT_OPEN_DIRECTORY,
                       "Cannot open directory"),
        std::make_pair(DirectoryListerException::LISTING_DISABLED,
                       "Directory listing is disabled"),
        std::make_pair(DirectoryListerException::INVALID_SORT_CRITERIA,
                       "Invalid sort criteria"),
        std::make_pair(DirectoryListerException::CANNOT_READ_ENTRIES,
                       "Cannot read directory entries"),
        std::make_pair(DirectoryListerException::GENERATION_FAILED,
                       "Failed to generate directory listing")};

DirectoryListerException::DirectoryListerException(const std::string& msg,
                                                   ErrorCode code)
    : BaseException("", static_cast<int>(code)) {
  std::ostringstream oss;
  oss << getErrorMsg(code) << ": " << msg;
  this->m_whatMsg = oss.str();
}

DirectoryListerException::DirectoryListerException(
    const DirectoryListerException& other)
    : BaseException(other) {}

DirectoryListerException::~DirectoryListerException() throw() {}

DirectoryListerException& DirectoryListerException::operator=(
    const DirectoryListerException& other) {
  if (this != &other) {
    BaseException::operator=(other);
  }
  return *this;
}

std::string DirectoryListerException::getErrorMsg(
    DirectoryListerException::ErrorCode code) {
  for (int i = 0; i < CODE_COUNT; ++i) {
    if (K_CODE_MSGS[i].first == code) {
      return K_CODE_MSGS[i].second;
    }
  }
  return "unknown directory lister error";
}

}  // namespace exceptions
}  // namespace shared
