/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HostException.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/24 14:45:49 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/27 03:32:08 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HOST_EXCEPTION_HPP
#define HOST_EXCEPTION_HPP

#include "shared/exceptions/BaseException.hpp"

namespace domain {
namespace http {
namespace exceptions {

class HostException : public shared::exceptions::BaseException {
 public:
  enum ErrorCode {
    EMPTY_HOST,
    INVALID_FORMAT,
    TOO_LONG,
    INVALID_IPV4,
    INVALID_IPV6,
    INVALID_HOSTNAME,
    MALFORMED,
    CODE_COUNT
  };

  explicit HostException(const std::string& msg, ErrorCode code);
  HostException(const HostException& other);
  virtual ~HostException() throw();

  HostException& operator=(const HostException& other);

 private:
  static const std::pair<ErrorCode, std::string> K_CODE_MSGS[];

  static std::string getErrorMsg(ErrorCode code);
};

}  // namespace exceptions
}  // namespace http
}  // namespace domain

#endif  // HOST_EXCEPTION_HPP
