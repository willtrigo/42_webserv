/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationConfigException.hpp                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/22 12:18:43 by dande-je          #+#    #+#             */
/*   Updated: 2026/01/01 20:20:50 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOCATION_CONFIG_EXCEPTION_HPP
#define LOCATION_CONFIG_EXCEPTION_HPP

#include "shared/exceptions/BaseException.hpp"

namespace domain {
namespace configuration {
namespace exceptions {

class LocationConfigException : public ::shared::exceptions::BaseException {
 public:
  enum ErrorCode {
    EMPTY_PATH,
    INVALID_PATH_FORMAT,
    INVALID_REGEX_PATTERN,
    REGEX_COMPILATION_FAILED,
    EMPTY_ROOT,
    INVALID_ROOT_PATH,
    EMPTY_INDEX_FILE,
    DUPLICATE_INDEX_FILE,
    NO_ALLOWED_METHODS,
    EMPTY_TRY_FILE,
    EMPTY_REDIRECT_URL,
    INVALID_REDIRECT_CODE,
    INVALID_UPLOAD_CONFIG,
    INVALID_CGI_CONFIG,
    INVALID_ERROR_CODE,
    EMPTY_ERROR_PAGE_URI,
    INVALID_ERROR_PAGE_URI,
    BODY_SIZE_TOO_LARGE,
    EMPTY_PROXY_PASS,
    INVALID_PROXY_PASS,
    CONFLICTING_DIRECTIVES,
    INVALID_CUSTOM_HEADER,
    INVALID_RETURN_CODE,
    RESERVED_HEADER,
    CODE_COUNT
  };

  explicit LocationConfigException(const std::string& msg, ErrorCode code);
  LocationConfigException(const LocationConfigException& other);
  virtual ~LocationConfigException() throw();

  LocationConfigException& operator=(const LocationConfigException& other);

 private:
  static const std::pair<ErrorCode, std::string> K_CODE_MSGS[];

  static std::string getErrorMsg(ErrorCode code);
};

}  // namespace exceptions
}  // namespace configuration
}  // namespace domain

#endif  // LOCATION_CONFIG_EXCEPTION_HPP
