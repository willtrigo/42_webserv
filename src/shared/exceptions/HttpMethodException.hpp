/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HtppMethodException.hpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/20 23:53:11 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/20 23:53:15 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTP_METHOD_EXCEPTION_HPP
#define HTTP_METHOD_EXCEPTION_HPP

#include "shared/exceptions/BaseException.hpp"

namespace shared {
namespace exceptions {

class HttpMethodException : public BaseException {
 public:
  enum ErrorCode {
    EMPTY_STRING,
    INVALID_METHOD,
    UNKNOWN_METHOD,
    CODE_COUNT
  };

  explicit HttpMethodException(const std::string& msg, ErrorCode code);
  HttpMethodException(const HttpMethodException& other);
  virtual ~HttpMethodException() throw();

  HttpMethodException& operator=(const HttpMethodException& other);

 private:
  static const std::pair<ErrorCode, std::string> K_CODE_MSGS[];

  static std::string getErrorMsg(ErrorCode code);
};

}  // namespace exceptions
}  // namespace shared

#endif  // HTTP_METHOD_EXCEPTION_HPP
