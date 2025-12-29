/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpConfigException.hpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/26 16:27:35 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/29 04:43:48 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTP_CONFIG_EXCEPTION_HPP
#define HTTP_CONFIG_EXCEPTION_HPP

#include "shared/exceptions/BaseException.hpp"

namespace domain {
namespace configuration {
namespace exceptions {

class HttpConfigException : public ::shared::exceptions::BaseException {
 public:
  enum ErrorCode {
    EMPTY_CONFIGURATION,
    DUPLICATE_SERVER,
    INVALID_SERVER_CONFIG,
    NO_DEFAULT_SERVER,
    MULTIPLE_DEFAULT_SERVERS,
    INVALID_WORKER_PROCESSES,
    INVALID_WORKER_CONNECTIONS,
    INVALID_KEEPALIVE_TIMEOUT,
    INVALID_SEND_TIMEOUT,
    INVALID_CLIENT_MAX_BODY_SIZE,
    INVALID_ERROR_LOG_PATH,
    INVALID_ACCESS_LOG_PATH,
    INVALID_MIME_TYPES_PATH,
    INVALID_CONFIG_FILE,
    INVALID_ERROR_PAGE,
    FILE_NOT_FOUND,
    FILE_READ_ERROR,
    PARSE_ERROR,
    MISSING_REQUIRED_DIRECTIVE,
    CONFIG_VALIDATION_FAILED,
    SERVER_SELECTION_FAILED,
    PORT_CONFLICT,
    ADDRESS_CONFLICT,
    CODE_COUNT
  };

  HttpConfigException(const std::string& msg, ErrorCode code);
  HttpConfigException(const HttpConfigException& other);
  virtual ~HttpConfigException() throw();

  HttpConfigException& operator=(const HttpConfigException& other);

 private:
  static const std::pair<ErrorCode, std::string> K_CODE_MSGS[];

  static std::string getErrorMsg(ErrorCode code);
};

}  // namespace exceptions
}  // namespace configuration
}  // namespace domain

#endif  // HTTP_CONFIG_EXCEPTION_HPP
