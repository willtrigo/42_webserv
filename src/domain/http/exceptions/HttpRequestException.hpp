/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequestException.hpp                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/04 14:20:16 by dande-je          #+#    #+#             */
/*   Updated: 2026/01/04 14:55:17 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPREQUESTEXCEPTION_HPP
#define HTTPREQUESTEXCEPTION_HPP

#include "shared/exceptions/BaseException.hpp"

namespace domain {
namespace http {
namespace exceptions {

class HttpRequestException : public ::shared::exceptions::BaseException {
 public:
  enum ErrorCode {
    INVALID_REQUEST,
    INCOMPLETE_REQUEST,
    MALFORMED_REQUEST,
    MISSING_REQUIRED_HEADER,
    INVALID_CONTENT_LENGTH,
    BODY_SIZE_MISMATCH,
    UNSUPPORTED_TRANSFER_ENCODING,
    INVALID_CHUNK_SIZE,
    HEADER_NOT_FOUND,
    DUPLICATE_HEADER,
    INVALID_URI,
    INVALID_METHOD,
    INVALID_VERSION,
    CODE_COUNT
  };

  explicit HttpRequestException(const std::string& msg, ErrorCode code);
  HttpRequestException(const HttpRequestException& other);
  virtual ~HttpRequestException() throw();

  HttpRequestException& operator=(const HttpRequestException& other);

 private:
  static const std::pair<ErrorCode, std::string> K_CODE_MSGS[];

  static std::string getErrorMsg(ErrorCode code);
};

}  // namespace exceptions
}  // namespace http
}  // namespace domain

#endif  // HTTPREQUESTEXCEPTION_HPP
