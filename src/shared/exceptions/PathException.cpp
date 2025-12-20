/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PathException.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/20 01:05:38 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/20 01:06:53 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shared/exceptions/PathException.hpp"

#include <sstream>

namespace shared {
namespace exceptions {

const std::pair<PathException::ErrorCode, std::string>
    PathException::K_CODE_MSGS[] = {
        std::make_pair(PathException::EMPTY_PATH, "Path cannot be empty"),
        std::make_pair(PathException::INVALID_CHARACTER,
                       "Path contains invalid characters"),
        std::make_pair(PathException::TOO_LONG, "Path exceeds maximum length"),
        std::make_pair(PathException::NOT_ABSOLUTE,
                       "Path must be absolute (start with '/')"),
        std::make_pair(PathException::NOT_RELATIVE,
                       "Path must be relative (not start with '/')"),
        std::make_pair(PathException::MALFORMED, "Path is malformed"),
        std::make_pair(PathException::TRAVERSAL_ATTEMPT,
                       "Path contains directory traversal attempt"),
        std::make_pair(PathException::INVALID_FILENAME,
                       "Filename contains invalid characters")};

PathException::PathException(const std::string& msg, ErrorCode code)
    : BaseException("", static_cast<int>(code)) {
  std::ostringstream oss;
  oss << getErrorMsg(code) << ": " << msg;
  this->m_whatMsg = oss.str();
}

PathException::PathException(const PathException& other)
    : BaseException(other) {}

PathException::~PathException() throw() {}

PathException& PathException::operator=(const PathException& other) {
  if (this != &other) {
    BaseException::operator=(other);
  }
  return *this;
}

std::string PathException::getErrorMsg(PathException::ErrorCode code) {
  for (int i = 0; i < CODE_COUNT; ++i) {
    if (K_CODE_MSGS[i].first == code) {
      return K_CODE_MSGS[i].second;
    }
  }
  return "unknown path error";
}

}  // namespace exceptions
}  // namespace shared
