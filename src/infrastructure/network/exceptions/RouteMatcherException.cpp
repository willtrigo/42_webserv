/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RouteMatcherException.cpp                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/24 20:37:40 by dande-je          #+#    #+#             */
/*   Updated: 2026/01/04 16:41:01 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RouteMatcherException.hpp"

#include <sstream>

namespace infrastructure {
namespace network {
namespace exceptions {

const std::pair<RouteMatcherException::ErrorCode, std::string>
    RouteMatcherException::K_CODE_MSGS[] = {
        std::make_pair(RouteMatcherException::ROUTE_NOT_FOUND,
                       "No matching route found"),
        std::make_pair(RouteMatcherException::METHOD_NOT_ALLOWED,
                       "HTTP method not allowed for matched route"),
        std::make_pair(RouteMatcherException::NO_MATCHING_ROUTE,
                       "No route matches the request"),
        std::make_pair(RouteMatcherException::INVALID_REQUEST_PATH,
                       "Invalid request path for matching"),
        std::make_pair(RouteMatcherException::MATCHING_FAILED,
                       "Route matching operation failed")};

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

std::string RouteMatcherException::getErrorMsg(
    RouteMatcherException::ErrorCode code) {
  for (int i = 0; i < CODE_COUNT; ++i) {
    if (K_CODE_MSGS[i].first == code) {
      return K_CODE_MSGS[i].second;
    }
  }
  return "Unknown route matcher error";
}

}  // namespace exceptions
}  // namespace network
}  // namespace infrastructure
