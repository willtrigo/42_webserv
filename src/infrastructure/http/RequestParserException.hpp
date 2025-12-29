/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParserException.hpp                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/24 20:50:49 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/24 20:50:55 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_PARSER_EXCEPTION_HPP
#define REQUEST_PARSER_EXCEPTION_HPP

#include "shared/exceptions/BaseException.hpp"

namespace shared {
namespace exceptions {

class RequestParserException : public BaseException {
 public:
  enum ErrorCode {
    MALFORMED_REQUEST,
    UNSUPPORTED_METHOD,
    INVALID_HEADER,
    INVALID_BODY,
    CHUNKED_ENCODING_ERROR,
    CONTENT_LENGTH_MISMATCH,
    URI_TOO_LONG,
    HEADER_TOO_LARGE,
    BODY_TOO_LARGE,
    INVALID_HTTP_VERSION,
    TIMEOUT,
    CODE_COUNT
  };

  explicit RequestParserException(const std::string& msg, ErrorCode code);
  RequestParserException(const RequestParserException& other);
  virtual ~RequestParserException() throw();

  RequestParserException& operator=(const RequestParserException& other);

 private:
  static const std::pair<ErrorCode, std::string> K_CODE_MSGS[];

  static std::string getErrorMsg(ErrorCode code);
};

}  // namespace exceptions
}  // namespace shared

#endif  // REQUEST_PARSER_EXCEPTION_HPP
