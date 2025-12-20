/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ErrorCodeException.cpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/20 11:06:17 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/20 11:20:08 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shared/exceptions/ErrorCodeException.hpp"

#include <sstream>

namespace shared {
namespace exceptions {

const std::pair<ErrorCodeException::ErrorCode, std::string>
    ErrorCodeException::K_CODE_MSGS[] = {
        std::make_pair(ErrorCodeException::INVALID_CODE,
                       "Invalid HTTP error code"),
        std::make_pair(ErrorCodeException::OUT_OF_RANGE,
                       "Error code out of valid range"),
        std::make_pair(ErrorCodeException::NOT_AN_ERROR,
                       "Code is not an error code (must be 400-599)"),
        std::make_pair(ErrorCodeException::NOT_A_SUCCESS,
                       "Code is not a success code (must be 200-299)"),
        std::make_pair(ErrorCodeException::EMPTY_STRING,
                       "Error code string cannot be empty"),
        std::make_pair(ErrorCodeException::CONVERSION_FAILED,
                       "Failed to convert error code string")};

ErrorCodeException::ErrorCodeException(const std::string& msg, ErrorCode code)
    : BaseException("", static_cast<int>(code)) {
  std::ostringstream oss;
  oss << getErrorMsg(code) << ": " << msg;
  this->m_whatMsg = oss.str();
}

ErrorCodeException::ErrorCodeException(const ErrorCodeException& other)
    : BaseException(other) {}

ErrorCodeException::~ErrorCodeException() throw() {}

ErrorCodeException& ErrorCodeException::operator=(
    const ErrorCodeException& other) {
  if (this != &other) {
    BaseException::operator=(other);
  }
  return *this;
}

std::string ErrorCodeException::getErrorMsg(
    ErrorCodeException::ErrorCode code) {
  for (int index = 0; index < CODE_COUNT; ++index) {
    if (K_CODE_MSGS[index].first == code) {
      return K_CODE_MSGS[index].second;
    }
  }
  return "unknown error code error";
}

}  // namespace exceptions
}  // namespace shared
