/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RouteMatchInfoException.cpp                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/05 15:17:13 by dande-je          #+#    #+#             */
/*   Updated: 2026/01/05 15:17:27 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "domain/http/exceptions/RouteMatchInfoException.hpp"

#include <sstream>

namespace domain {
namespace http {
namespace exceptions {

const std::pair<RouteMatchInfoException::ErrorCode, std::string>
    RouteMatchInfoException::K_CODE_MSGS[] = {
        std::make_pair(RouteMatchInfoException::INVALID_RESOLVED_PATH,
                       "Invalid resolved path"),
        std::make_pair(RouteMatchInfoException::INVALID_FILE_TO_SERVE,
                       "Invalid file to serve"),
        std::make_pair(RouteMatchInfoException::INVALID_QUERY_PARAMS,
                       "Invalid query parameters"),
        std::make_pair(RouteMatchInfoException::RESOLUTION_ERROR,
                       "Route match resolution error")};

RouteMatchInfoException::RouteMatchInfoException(const std::string& message,
                                                 ErrorCode code)
    : BaseException("", static_cast<int>(code)) {
  std::ostringstream oss;
  oss << getErrorMsg(code) << ": " << message;
  this->m_whatMsg = oss.str();
}

RouteMatchInfoException::RouteMatchInfoException(
    const RouteMatchInfoException& other)
    : BaseException(other) {}

RouteMatchInfoException::~RouteMatchInfoException() throw() {}

RouteMatchInfoException& RouteMatchInfoException::operator=(
    const RouteMatchInfoException& other) {
  if (this != &other) {
    BaseException::operator=(other);
  }
  return *this;
}

std::string RouteMatchInfoException::getErrorMsg(ErrorCode code) {
  for (int i = 0; i < CODE_COUNT; ++i) {
    if (K_CODE_MSGS[i].first == code) {
      return K_CODE_MSGS[i].second;
    }
  }
  return "unknown route match info error";
}

}  // namespace exceptions
}  // namespace http
}  // namespace domain
