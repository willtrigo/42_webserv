/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfigException.hpp                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/22 11:46:58 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/26 16:45:34 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_CONFIG_EXCEPTION_HPP
#define SERVER_CONFIG_EXCEPTION_HPP

#include "shared/exceptions/BaseException.hpp"

namespace shared {
namespace exceptions {

class ServerConfigException : public BaseException {
 public:
  enum ErrorCode {
    INVALID_PORT,
    INVALID_ADDRESS,
    DUPLICATE_LISTEN,
    EMPTY_SERVER_NAME,
    INVALID_SERVER_NAME,
    DUPLICATE_SERVER_NAME,
    EMPTY_ROOT,
    INVALID_ROOT_PATH,
    MISSING_ROOT,
    EMPTY_INDEX_FILE,
    DUPLICATE_INDEX_FILE,
    INVALID_ERROR_CODE,
    EMPTY_ERROR_PAGE_URI,
    INVALID_ERROR_PAGE_URI,
    NULL_LOCATION,
    DUPLICATE_LOCATION_PATH,
    EMPTY_REDIRECT_URL,
    INVALID_REDIRECT_CODE,
    BODY_SIZE_TOO_LARGE,
    MISSING_LISTEN_DIRECTIVE,
    MULTIPLE_DEFAULT_SERVERS,
    MISSING_CONFIGURATION,
    INVALID_LISTEN_DIRECTIVE_FORMAT,
    LOCATION_VALIDATION_FAILED,
    CODE_COUNT
  };

  ServerConfigException(const std::string& msg, ErrorCode code);
  ServerConfigException(const ServerConfigException& other);
  virtual ~ServerConfigException() throw();

  ServerConfigException& operator=(const ServerConfigException& other);

 private:
  static const std::pair<ErrorCode, std::string> K_CODE_MSGS[];

  static std::string getErrorMsg(ErrorCode code);
};

}  // namespace exceptions
}  // namespace shared

#endif  // SERVER_CONFIG_EXCEPTION_HPP
