/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponseException.hpp                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/04 15:07:25 by dande-je          #+#    #+#             */
/*   Updated: 2026/01/04 15:10:05 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPRESPONSEEXCEPTION_HPP
#define HTTPRESPONSEEXCEPTION_HPP

#include "shared/exceptions/BaseException.hpp"

namespace domain {
namespace http {
namespace exceptions {

class HttpResponseException : public ::shared::exceptions::BaseException {
 public:
  enum ErrorCode {
    INVALID_RESPONSE,
    INVALID_STATUS_CODE,
    INVALID_HEADER,
    BODY_TOO_LARGE,
    SERIALIZATION_FAILED,
    MISSING_REQUIRED_HEADER,
    DUPLICATE_HEADER,
    INVALID_CONTENT_LENGTH,
    CODE_COUNT
  };

  explicit HttpResponseException(const std::string& msg, ErrorCode code);
  HttpResponseException(const HttpResponseException& other);
  virtual ~HttpResponseException() throw();

  HttpResponseException& operator=(const HttpResponseException& other);

 private:
  static const std::pair<ErrorCode, std::string> K_CODE_MSGS[];

  static std::string getErrorMsg(ErrorCode code);
};

}  // namespace exceptions
}  // namespace http
}  // namespace domain

#endif  // HTTPRESPONSEEXCEPTION_HPP
