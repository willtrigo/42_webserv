/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ErrorPageException.cpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/25 21:58:38 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/25 21:58:58 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shared/exceptions/ErrorPageException.hpp"

#include <sstream>

namespace shared {
namespace exceptions {

const std::pair<ErrorPageException::ErrorCode, std::string>
    ErrorPageException::K_CODE_MSGS[] = {
        std::make_pair(ErrorPageException::INVALID_FORMAT,
                       "Invalid error page format"),
        std::make_pair(ErrorPageException::NO_CONTENT,
                       "Error page has no content or file"),
        std::make_pair(ErrorPageException::CONFLICTING_SOURCES,
                       "Error page cannot have both file and content"),
        std::make_pair(ErrorPageException::CONTENT_TOO_LARGE,
                       "Error page content too large"),
        std::make_pair(ErrorPageException::INVALID_PATH,
                       "Invalid error page file path"),
        std::make_pair(ErrorPageException::FILE_ERROR,
                       "Error page file operation failed"),
        std::make_pair(ErrorPageException::INVALID_CONTENT_TYPE,
                       "Invalid error page content type")};

ErrorPageException::ErrorPageException(const std::string& msg, ErrorCode code)
    : BaseException("", static_cast<int>(code)) {
  std::ostringstream oss;
  oss << getErrorMsg(code) << ": " << msg;
  this->m_whatMsg = oss.str();
}

ErrorPageException::ErrorPageException(const ErrorPageException& other)
    : BaseException(other) {}

ErrorPageException::~ErrorPageException() throw() {}

ErrorPageException& ErrorPageException::operator=(
    const ErrorPageException& other) {
  if (this != &other) {
    BaseException::operator=(other);
  }
  return *this;
}

std::string ErrorPageException::getErrorMsg(
    ErrorPageException::ErrorCode code) {
  for (int index = 0; index < CODE_COUNT; ++index) {
    if (K_CODE_MSGS[index].first == code) {
      return K_CODE_MSGS[index].second;
    }
  }
  return "unknown error page error";
}

}  // namespace exceptions
}  // namespace shared
