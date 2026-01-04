/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RouteMatcherException.hpp                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/24 20:37:18 by dande-je          #+#    #+#             */
/*   Updated: 2026/01/03 06:42:26 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// TODO: not using yet
#ifndef ROUTER_MATCHER_EXCEPTION_HPP
#define ROUTER_MATCHER_EXCEPTION_HPP

#include "shared/exceptions/BaseException.hpp"

namespace shared {
namespace exceptions {

class RouteMatcherException : public BaseException {
 public:
  enum ErrorCode {
    INVALID_ROUTE_PATTERN,
    ROUTE_NOT_FOUND,
    METHOD_NOT_ALLOWED,
    DUPLICATE_ROUTE,
    INVALID_CONFIGURATION,
    MALFORMED_PATH,
    CODE_COUNT
  };

  explicit RouteMatcherException(const std::string& msg, ErrorCode code);
  RouteMatcherException(const RouteMatcherException& other);
  virtual ~RouteMatcherException() throw();

  RouteMatcherException& operator=(const RouteMatcherException& other);

 private:
  static const std::pair<ErrorCode, std::string> K_CODE_MSGS[];

  static std::string getErrorMsg(ErrorCode code);
};

}  // namespace exceptions
}  // namespace shared

#endif  // ROUTER_MATCHER_EXCEPTION_HPP
