/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RouteMatcherException.cpp                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/24 20:37:40 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/25 15:01:09 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shared/exceptions/RouteMatcherException.hpp"

#include <sstream>

namespace shared {
namespace exceptions {

const std::pair<RouteMatcherException::ErrorCode, std::string>
    RouteMatcherException::K_CODE_MSGS[] = {
        std::make_pair(RouteMatcherException::INVALID_ROUTE_PATTERN,
                       "Invalid route pattern"),
        std::make_pair(RouteMatcherException::ROUTE_NOT_FOUND,
                       "Route not found"),
        std::make_pair(RouteMatcherException::METHOD_NOT_ALLOWED,
                       "HTTP method not allowed for this route"),
        std::make_pair(RouteMatcherException::DUPLICATE_ROUTE,
                       "Duplicate route definition"),
        std::make_pair(RouteMatcherException::INVALID_CONFIGURATION,
                       "Invalid router configuration"),
        std::make_pair(RouteMatcherException::MALFORMED_PATH,
                       "Malformed request path")};

RouteMatcherException::RouteMatcherException(const std::string& msg,
                                             ErrorCode code)
    : BaseException("", static_cast<int>(code)) {
  std::ostringstream oss;
  oss << getErrorMsg(code) << ": " << msg;
  this->m_whatMsg = oss.str();
}

RouteMatcherException::RouteMatcherException(const RouteMatcherException& other)
    : BaseException(other) {}

RouteMatcherException::~RouteMatcherException() throw() {}

RouteMatcherException& RouteMatcherException::operator=(
    const RouteMatcherException& other) {
  if (this != &other) {
    BaseException::operator=(other);
  }
  return *this;
}

std::string RouteMatcherException::getErrorMsg(ErrorCode code) {
  for (int i = 0; i < CODE_COUNT; ++i) {
    if (K_CODE_MSGS[i].first == code) {
      return K_CODE_MSGS[i].second;
    }
  }
  return "unknown router matcher error";
}

}  // namespace exceptions
}  // namespace shared
