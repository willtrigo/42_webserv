/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PortException.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/19 23:40:45 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/27 02:26:50 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PORT_EXCEPTION_HPP
#define PORT_EXCEPTION_HPP

#include "shared/exceptions/BaseException.hpp"

namespace domain {
namespace http {
namespace exceptions {

class PortException : public shared::exceptions::BaseException {
 public:
  enum ErrorCode {
    INVALID_STRING,
    OUT_OF_RANGE,
    NON_DIGIT_CHARACTER,
    CONVERSION_FAILED,
    EMPTY_STRING,
    CODE_COUNT
  };

  explicit PortException(const std::string& msg, ErrorCode code);
  PortException(const PortException& other);
  virtual ~PortException() throw();

  PortException& operator=(const PortException& other);

 private:
  static const std::pair<ErrorCode, std::string> K_CODE_MSGS[];

  static std::string getErrorMsg(ErrorCode code);
};

}  // namespace exceptions
}  // namespace http
}  // namespace domain

#endif  // PORT_EXCEPTION_HPP
