/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ErrorCodeException.hpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/20 11:05:51 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/20 11:05:56 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ERROR_CODE_EXCEPTION_HPP
#define ERROR_CODE_EXCEPTION_HPP

#include "shared/exceptions/BaseException.hpp"

namespace shared {
namespace exceptions {

class ErrorCodeException : public BaseException {
 public:
  enum ErrorCode {
    INVALID_CODE,
    OUT_OF_RANGE,
    NOT_AN_ERROR,
    NOT_A_SUCCESS,
    EMPTY_STRING,
    CONVERSION_FAILED,
    CODE_COUNT
  };

  explicit ErrorCodeException(const std::string& msg, ErrorCode code);
  ErrorCodeException(const ErrorCodeException& other);
  virtual ~ErrorCodeException() throw();

  ErrorCodeException& operator=(const ErrorCodeException& other);

 private:
  static const std::pair<ErrorCode, std::string> K_CODE_MSGS[];

  static std::string getErrorMsg(ErrorCode code);
};

}  // namespace exceptions
}  // namespace shared

#endif  // ERROR_CODE_EXCEPTION_HPP
