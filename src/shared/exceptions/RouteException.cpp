/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RouteException.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/24 21:51:38 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/24 21:52:42 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shared/exceptions/RouteException.hpp"

#include <sstream>

namespace shared {
namespace exceptions {

const std::pair<RouteException::ErrorCode, std::string>
    RouteException::K_CODE_MSGS[] = {
        std::make_pair(RouteException::INVALID_PATH, "Invalid route path"),
        std::make_pair(RouteException::INVALID_METHOD, "Invalid HTTP method"),
        std::make_pair(RouteException::INVALID_HANDLER,
                       "Invalid route handler type"),
        std::make_pair(RouteException::DUPLICATE_ROUTE,
                       "Duplicate route configuration"),
        std::make_pair(RouteException::CONFIGURATION_ERROR,
                       "Route configuration error"),
        std::make_pair(RouteException::ROUTE_NOT_FOUND, "Route not found"),
        std::make_pair(RouteException::METHOD_NOT_ALLOWED,
                       "HTTP method not allowed for this route"),
        std::make_pair(RouteException::FILE_NOT_FOUND, "File not found"),
        std::make_pair(RouteException::PERMISSION_DENIED, "Permission denied"),
        std::make_pair(RouteException::BODY_TOO_LARGE,
                       "Request body too large")};

RouteException::RouteException(const std::string& message, ErrorCode code)
    : BaseException("", static_cast<int>(code)) {
  std::ostringstream oss;
  oss << getErrorMsg(code) << ": " << message;
  this->m_whatMsg = oss.str();
}

RouteException::RouteException(const RouteException& other)
    : BaseException(other) {}

RouteException::~RouteException() throw() {}

RouteException& RouteException::operator=(const RouteException& other) {
  if (this != &other) {
    BaseException::operator=(other);
  }
  return *this;
}

std::string RouteException::getErrorMsg(ErrorCode code) {
  for (int i = 0; i < CODE_COUNT; ++i) {
    if (K_CODE_MSGS[i].first == code) {
      return K_CODE_MSGS[i].second;
    }
  }
  return "unknown route error";
}

}  // namespace exceptions
}  // namespace shared
