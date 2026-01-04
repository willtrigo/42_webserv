/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConnectionException.hpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/04 01:48:42 by dande-je          #+#    #+#             */
/*   Updated: 2026/01/04 01:50:25 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONNECTION_EXCEPTION_HPP
#define CONNECTION_EXCEPTION_HPP

#include "shared/exceptions/BaseException.hpp"

namespace infrastructure {
namespace network {
namespace exceptions {

class ConnectionException : public shared::exceptions::BaseException {
 public:
  enum ErrorCode {
    INVALID_STATE,
    BUFFER_OVERFLOW,
    MALFORMED_REQUEST,
    REQUEST_TIMEOUT,
    REQUEST_TOO_LARGE,
    RESPONSE_GENERATION_FAILED,
    READ_ERROR,
    WRITE_ERROR,
    PROTOCOL_VIOLATION,
    HEADER_PARSING_FAILED,
    BODY_PARSING_FAILED,
    INCOMPLETE_REQUEST,
    INCOMPLETE_RESPONSE,
    INVALID_HTTP_VERSION,
    INVALID_METHOD,
    INVALID_URI,
    INVALID_HEADER,
    CONTENT_LENGTH_MISMATCH,
    CHUNK_ENCODING_ERROR,
    KEEPALIVE_TIMEOUT,
    PIPELINE_ERROR,
    CODE_COUNT
  };

  explicit ConnectionException(const std::string& msg, ErrorCode code);
  explicit ConnectionException(const std::string& msg, ErrorCode code,
                               int systemError);
  ConnectionException(const ConnectionException& other);
  virtual ~ConnectionException() throw();

  ConnectionException& operator=(const ConnectionException& other);

  int getSystemError() const;

 private:
  static const std::pair<ErrorCode, std::string> K_CODE_MSGS[];

  int m_systemError;

  static std::string getErrorMsg(ErrorCode code);
  static std::string formatSystemError(int systemError);
};

}  // namespace exceptions
}  // namespace network
}  // namespace infrastructure

#endif  // CONNECTION_EXCEPTION_HPP
