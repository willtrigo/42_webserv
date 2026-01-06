/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiExecutionException.hpp                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/05 17:38:18 by dande-je          #+#    #+#             */
/*   Updated: 2026/01/05 17:49:08 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGI_EXECUTION_EXCEPTION_HPP
#define CGI_EXECUTION_EXCEPTION_HPP

#include "shared/exceptions/BaseException.hpp"

namespace infrastructure {
namespace cgi {
namespace exceptions {

class CgiExecutionException : public ::shared::exceptions::BaseException {
 public:
  enum ErrorCode {
    FORK_FAILED,
    EXEC_FAILED,
    PIPE_FAILED,
    SCRIPT_NOT_FOUND,
    SCRIPT_NOT_EXECUTABLE,
    TIMEOUT,
    INVALID_OUTPUT,
    PROCESS_ERROR,
    ENVIRONMENT_ERROR,
    INTERPRETER_NOT_FOUND,
    CODE_COUNT
  };

  explicit CgiExecutionException(const std::string& message, ErrorCode code);
  CgiExecutionException(const CgiExecutionException& other);
  virtual ~CgiExecutionException() throw();

  CgiExecutionException& operator=(const CgiExecutionException& other);

 private:
  static const std::pair<ErrorCode, std::string> K_CODE_MSGS[];

  static std::string getErrorMsg(ErrorCode code);
};

}  // namespace exceptions
}  // namespace cgi
}  // namespace infrastructure

#endif  // CGI_EXECUTION_EXCEPTION_HPP
