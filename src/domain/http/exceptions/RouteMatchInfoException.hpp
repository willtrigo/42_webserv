/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RouteMatchInfoException.hpp                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/05 15:16:30 by dande-je          #+#    #+#             */
/*   Updated: 2026/01/05 15:16:51 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ROUTE_MATCH_INFO_EXCEPTION_HPP
#define ROUTE_MATCH_INFO_EXCEPTION_HPP

#include "shared/exceptions/BaseException.hpp"

namespace domain {
namespace http {
namespace exceptions {

class RouteMatchInfoException : public ::shared::exceptions::BaseException {
 public:
  enum ErrorCode {
    INVALID_RESOLVED_PATH,
    INVALID_FILE_TO_SERVE,
    INVALID_QUERY_PARAMS,
    RESOLUTION_ERROR,
    CODE_COUNT
  };

  explicit RouteMatchInfoException(const std::string& message, ErrorCode code);
  RouteMatchInfoException(const RouteMatchInfoException& other);
  virtual ~RouteMatchInfoException() throw();

  RouteMatchInfoException& operator=(const RouteMatchInfoException& other);

 private:
  static const std::pair<ErrorCode, std::string> K_CODE_MSGS[];

  static std::string getErrorMsg(ErrorCode code);
};

}  // namespace exceptions
}  // namespace http
}  // namespace domain

#endif  // ROUTE_MATCH_INFO_EXCEPTION_HPP
