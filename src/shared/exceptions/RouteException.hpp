/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RouteException.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/24 21:51:11 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/24 21:52:21 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ROUTE_EXCEPTION_HPP
#define ROUTE_EXCEPTION_HPP

#include "shared/exceptions/BaseException.hpp"

namespace shared {
namespace exceptions {

class RouteException : public BaseException {
 public:
  enum ErrorCode {
    INVALID_PATH,
    INVALID_METHOD,
    INVALID_HANDLER,
    DUPLICATE_ROUTE,
    CONFIGURATION_ERROR,
    ROUTE_NOT_FOUND,
    METHOD_NOT_ALLOWED,
    FILE_NOT_FOUND,
    PERMISSION_DENIED,
    BODY_TOO_LARGE,
    CODE_COUNT
  };

  explicit RouteException(const std::string& message, ErrorCode code);
  RouteException(const RouteException& other);
  virtual ~RouteException() throw();

  RouteException& operator=(const RouteException& other);

 private:
  static const std::pair<ErrorCode, std::string> K_CODE_MSGS[];

  static std::string getErrorMsg(ErrorCode code);
};

}  // namespace exceptions
}  // namespace shared

#endif  // ROUTE_EXCEPTION_HPP
