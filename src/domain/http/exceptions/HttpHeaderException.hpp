/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpHeaderException.hpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/04 13:52:14 by dande-je          #+#    #+#             */
/*   Updated: 2026/01/04 13:54:04 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPHEADEREXCEPTION_HPP
#define HTTPHEADEREXCEPTION_HPP

#include "shared/exceptions/BaseException.hpp"

namespace domain {
namespace http {
namespace exceptions {

class HttpHeaderException : public ::shared::exceptions::BaseException {
 public:
  enum ErrorCode {
    INVALID_FORMAT,
    EMPTY_NAME,
    EMPTY_VALUE,
    NAME_TOO_LONG,
    VALUE_TOO_LONG,
    INVALID_CHARACTER_IN_NAME,
    INVALID_CHARACTER_IN_VALUE,
    MISSING_COLON_SEPARATOR,
    DUPLICATE_HEADER,
    REQUIRED_HEADER_MISSING,
    CODE_COUNT
  };

  explicit HttpHeaderException(const std::string& msg, ErrorCode code);
  HttpHeaderException(const HttpHeaderException& other);
  virtual ~HttpHeaderException() throw();

  HttpHeaderException& operator=(const HttpHeaderException& other);

 private:
  static const std::pair<ErrorCode, std::string> K_CODE_MSGS[];

  static std::string getErrorMsg(ErrorCode code);
};

}  // namespace exceptions
}  // namespace http
}  // namespace domain

#endif  // HTTPHEADEREXCEPTION_HPP
