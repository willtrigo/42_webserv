/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationConfigException.cpp                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/22 12:19:31 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/22 12:21:59 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shared/exceptions/LocationConfigException.hpp"

#include <sstream>

namespace shared {
namespace exceptions {

const std::pair<LocationConfigException::ErrorCode, std::string>
    LocationConfigException::K_CODE_MSGS[] = {
        std::make_pair(LocationConfigException::EMPTY_PATH,
                       "Location path cannot be empty"),
        std::make_pair(LocationConfigException::INVALID_PATH_FORMAT,
                       "Invalid location path format"),
        std::make_pair(LocationConfigException::INVALID_REGEX_PATTERN,
                       "Invalid regex pattern"),
        std::make_pair(LocationConfigException::REGEX_COMPILATION_FAILED,
                       "Failed to compile regex pattern"),
        std::make_pair(LocationConfigException::EMPTY_ROOT,
                       "Root path cannot be empty"),
        std::make_pair(LocationConfigException::INVALID_ROOT_PATH,
                       "Invalid root path"),
        std::make_pair(LocationConfigException::EMPTY_INDEX_FILE,
                       "Index file cannot be empty"),
        std::make_pair(LocationConfigException::DUPLICATE_INDEX_FILE,
                       "Duplicate index file"),
        std::make_pair(LocationConfigException::NO_ALLOWED_METHODS,
                       "No allowed HTTP methods specified"),
        std::make_pair(LocationConfigException::EMPTY_TRY_FILE,
                       "Try files entry cannot be empty"),
        std::make_pair(LocationConfigException::EMPTY_REDIRECT_URL,
                       "Redirect URL cannot be empty"),
        std::make_pair(LocationConfigException::INVALID_REDIRECT_CODE,
                       "Invalid redirect code"),
        std::make_pair(LocationConfigException::INVALID_UPLOAD_CONFIG,
                       "Invalid upload configuration"),
        std::make_pair(LocationConfigException::INVALID_CGI_CONFIG,
                       "Invalid CGI configuration"),
        std::make_pair(LocationConfigException::INVALID_ERROR_CODE,
                       "Invalid error code"),
        std::make_pair(LocationConfigException::EMPTY_ERROR_PAGE_URI,
                       "Error page URI cannot be empty"),
        std::make_pair(LocationConfigException::INVALID_ERROR_PAGE_URI,
                       "Invalid error page URI"),
        std::make_pair(LocationConfigException::BODY_SIZE_TOO_LARGE,
                       "Client body size too large"),
        std::make_pair(LocationConfigException::EMPTY_PROXY_PASS,
                       "Proxy pass URL cannot be empty"),
        std::make_pair(LocationConfigException::INVALID_PROXY_PASS,
                       "Invalid proxy pass URL"),
        std::make_pair(LocationConfigException::CONFLICTING_DIRECTIVES,
                       "Conflicting directives in location configuration")};

LocationConfigException::LocationConfigException(const std::string& msg,
                                                 ErrorCode code)
    : BaseException("", static_cast<int>(code)) {
  std::ostringstream oss;
  oss << getErrorMsg(code) << ": " << msg;
  this->m_whatMsg = oss.str();
}

LocationConfigException::LocationConfigException(
    const LocationConfigException& other)
    : BaseException(other) {}

LocationConfigException::~LocationConfigException() throw() {}

LocationConfigException& LocationConfigException::operator=(
    const LocationConfigException& other) {
  if (this != &other) {
    BaseException::operator=(other);
  }
  return *this;
}

std::string LocationConfigException::getErrorMsg(
    LocationConfigException::ErrorCode code) {
  for (int i = 0; i < CODE_COUNT; ++i) {
    if (K_CODE_MSGS[i].first == code) {
      return K_CODE_MSGS[i].second;
    }
  }
  return "unknown location configuration error";
}

}  // namespace exceptions
}  // namespace shared
