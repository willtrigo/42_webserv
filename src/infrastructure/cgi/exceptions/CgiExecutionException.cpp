/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiExecutionException.cpp                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/05 17:38:34 by dande-je          #+#    #+#             */
/*   Updated: 2026/01/05 18:17:59 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "infrastructure/cgi/exceptions/CgiExecutionException.hpp"

#include <sstream>

namespace infrastructure {
namespace cgi {
namespace exceptions {

const std::pair<CgiExecutionException::ErrorCode, std::string>
    CgiExecutionException::K_CODE_MSGS[] = {
        std::make_pair(CgiExecutionException::FORK_FAILED,
                       "Failed to fork CGI process"),
        std::make_pair(CgiExecutionException::EXEC_FAILED,
                       "Failed to execute CGI script"),
        std::make_pair(CgiExecutionException::PIPE_FAILED,
                       "Failed to create pipe for CGI communication"),
        std::make_pair(CgiExecutionException::SCRIPT_NOT_FOUND,
                       "CGI script not found"),
        std::make_pair(CgiExecutionException::SCRIPT_NOT_EXECUTABLE,
                       "CGI script is not executable"),
        std::make_pair(CgiExecutionException::TIMEOUT,
                       "CGI script execution timeout"),
        std::make_pair(CgiExecutionException::INVALID_OUTPUT,
                       "CGI script produced invalid output"),
        std::make_pair(CgiExecutionException::PROCESS_ERROR,
                       "CGI process error"),
        std::make_pair(CgiExecutionException::ENVIRONMENT_ERROR,
                       "Failed to set CGI environment"),
        std::make_pair(CgiExecutionException::INTERPRETER_NOT_FOUND,
                       "CGI interpreter not found")};

CgiExecutionException::CgiExecutionException(const std::string& message,
                                             ErrorCode code)
    : BaseException("", static_cast<int>(code)) {
  std::ostringstream oss;
  oss << getErrorMsg(code) << ": " << message;
  this->m_whatMsg = oss.str();
}

CgiExecutionException::CgiExecutionException(const CgiExecutionException& other)
    : BaseException(other) {}

CgiExecutionException::~CgiExecutionException() throw() {}

CgiExecutionException& CgiExecutionException::operator=(
    const CgiExecutionException& other) {
  if (this != &other) {
    BaseException::operator=(other);
  }
  return *this;
}

std::string CgiExecutionException::getErrorMsg(ErrorCode code) {
  for (int i = 0; i < CODE_COUNT; ++i) {
    if (K_CODE_MSGS[i].first == code) {
      return K_CODE_MSGS[i].second;
    }
  }
  return "unknown CGI execution error";
}

}  // namespace exceptions
}  // namespace cgi
}  // namespace infrastructure
