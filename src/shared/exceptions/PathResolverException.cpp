/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PathResolverException.cpp                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/22 19:26:46 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/22 19:34:11 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shared/exceptions/PathResolverException.hpp"

#include <sstream>

namespace shared {
namespace exceptions {

const std::pair<PathResolverException::ErrorCode, std::string>
    PathResolverException::K_CODE_MSGS[] = {
        std::make_pair(PathResolverException::INVALID_DEPENDENCY,
                       "Invalid dependency in PathResolver"),
        std::make_pair(PathResolverException::INVALID_ROOT,
                       "Invalid root directory configuration"),
        std::make_pair(PathResolverException::ROOT_NOT_FOUND,
                       "Root directory not found"),
        std::make_pair(PathResolverException::ROOT_NOT_DIRECTORY,
                       "Root path is not a directory"),
        std::make_pair(PathResolverException::INVALID_REQUEST,
                       "Invalid request path"),
        std::make_pair(PathResolverException::PATH_NOT_FOUND,
                       "Requested path not found"),
        std::make_pair(PathResolverException::PATH_TRAVERSAL,
                       "Path traversal attempt detected"),
        std::make_pair(PathResolverException::INVALID_MAPPING,
                       "Invalid location mapping"),
        std::make_pair(PathResolverException::DIRECTORY_LISTING_DISABLED,
                       "Directory listing is disabled"),
        std::make_pair(PathResolverException::PERMISSION_DENIED,
                       "Permission denied for path")};

PathResolverException::PathResolverException(const std::string& msg,
                                             ErrorCode code)
    : BaseException("", static_cast<int>(code)) {
  std::ostringstream oss;
  oss << getErrorMsg(code) << ": " << msg;
  this->m_whatMsg = oss.str();
}

PathResolverException::PathResolverException(const PathResolverException& other)
    : BaseException(other) {}

PathResolverException::~PathResolverException() throw() {}

PathResolverException& PathResolverException::operator=(
    const PathResolverException& other) {
  if (this != &other) {
    BaseException::operator=(other);
  }
  return *this;
}

std::string PathResolverException::getErrorMsg(
    PathResolverException::ErrorCode code) {
  for (int i = 0; i < CODE_COUNT; ++i) {
    if (K_CODE_MSGS[i].first == code) {
      return K_CODE_MSGS[i].second;
    }
  }
  return "unknown path resolver error";
}

}  // namespace exceptions
}  // namespace shared
