/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiConfigException.cpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/22 13:10:48 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/27 18:26:26 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "domain/configuration/exceptions/CgiConfigException.hpp"

#include <sstream>

namespace domain {
namespace configuration {
namespace exceptions {

const std::pair<CgiConfigException::ErrorCode, std::string>
    CgiConfigException::K_CODE_MSGS[] = {
        std::make_pair(CgiConfigException::EMPTY_SCRIPT_PATH,
                       "CGI script path cannot be empty"),
        std::make_pair(CgiConfigException::INVALID_SCRIPT_PATH,
                       "Invalid CGI script path"),
        std::make_pair(CgiConfigException::EMPTY_CGI_ROOT,
                       "CGI root directory cannot be empty"),
        std::make_pair(CgiConfigException::INVALID_CGI_ROOT,
                       "Invalid CGI root directory"),
        std::make_pair(CgiConfigException::INVALID_EXTENSION_PATTERN,
                       "Invalid CGI extension pattern"),
        std::make_pair(CgiConfigException::DUPLICATE_CGI_PARAM,
                       "Duplicate CGI parameter"),
        std::make_pair(CgiConfigException::INVALID_CGI_PARAM,
                       "Invalid CGI parameter"),
        std::make_pair(CgiConfigException::MISSING_REQUIRED_PARAMS,
                       "Missing required CGI parameters")};

CgiConfigException::CgiConfigException(const std::string& msg, ErrorCode code)
    : BaseException("", static_cast<int>(code)) {
  std::ostringstream oss;
  oss << getErrorMsg(code) << ": " << msg;
  this->m_whatMsg = oss.str();
}

CgiConfigException::CgiConfigException(const CgiConfigException& other)
    : BaseException(other) {}

CgiConfigException::~CgiConfigException() throw() {}

CgiConfigException& CgiConfigException::operator=(
    const CgiConfigException& other) {
  if (this != &other) {
    BaseException::operator=(other);
  }
  return *this;
}

std::string CgiConfigException::getErrorMsg(
    CgiConfigException::ErrorCode code) {
  for (int i = 0; i < CODE_COUNT; ++i) {
    if (K_CODE_MSGS[i].first == code) {
      return K_CODE_MSGS[i].second;
    }
  }
  return "unknown CGI configuration error";
}

}  // namespace exceptions
}  // namespace configuration
}  // namespace domain
