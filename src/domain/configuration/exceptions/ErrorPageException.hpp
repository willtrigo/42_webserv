/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ErrorPageException.hpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/25 21:58:11 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/27 17:36:47 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ERROR_PAGE_EXCEPTION_HPP
#define ERROR_PAGE_EXCEPTION_HPP

#include "shared/exceptions/BaseException.hpp"

namespace domain {
namespace configuration {
namespace exceptions {

class ErrorPageException : public ::shared::exceptions::BaseException {
 public:
  enum ErrorCode {
    INVALID_FORMAT,
    NO_CONTENT,
    CONFLICTING_SOURCES,
    CONTENT_TOO_LARGE,
    INVALID_PATH,
    FILE_ERROR,
    INVALID_CONTENT_TYPE,
    CODE_COUNT
  };

  explicit ErrorPageException(const std::string& msg, ErrorCode code);
  ErrorPageException(const ErrorPageException& other);
  virtual ~ErrorPageException() throw();

  ErrorPageException& operator=(const ErrorPageException& other);

 private:
  static const std::pair<ErrorCode, std::string> K_CODE_MSGS[];

  static std::string getErrorMsg(ErrorCode code);
};

}  // namespace exceptions
}  // namespace configuration
}  // namespace domain

#endif  // ERROR_PAGE_EXCEPTION_HPP
