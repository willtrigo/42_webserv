/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpVersionException.hpp                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/04 13:41:14 by dande-je          #+#    #+#             */
/*   Updated: 2026/01/04 13:45:18 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPVERSIONEXCEPTION_HPP
#define HTTPVERSIONEXCEPTION_HPP

#include "shared/exceptions/BaseException.hpp"

namespace domain {
namespace http {
namespace exceptions {

class HttpVersionException : public ::shared::exceptions::BaseException {
 public:
  enum ErrorCode {
    INVALID_FORMAT,
    UNSUPPORTED_VERSION,
    EMPTY_STRING,
    INVALID_MAJOR_VERSION,
    INVALID_MINOR_VERSION,
    VERSION_TOO_OLD,
    VERSION_TOO_NEW,
    CODE_COUNT
  };

  explicit HttpVersionException(const std::string& msg, ErrorCode code);
  HttpVersionException(const HttpVersionException& other);
  virtual ~HttpVersionException() throw();

  HttpVersionException& operator=(const HttpVersionException& other);

 private:
  static const std::pair<ErrorCode, std::string> K_CODE_MSGS[];

  static std::string getErrorMsg(ErrorCode code);
};

}  // namespace exceptions
}  // namespace http
}  // namespace domain

#endif  // HTTPVERSIONEXCEPTION_HPP
