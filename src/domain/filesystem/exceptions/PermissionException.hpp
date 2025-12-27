/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PermissionException.hpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/21 00:05:27 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/27 03:52:35 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PERMISSION_EXCEPTION_HPP
#define PERMISSION_EXCEPTION_HPP

#include "shared/exceptions/BaseException.hpp"

namespace domain {
namespace filesystem {
namespace exceptions {

class PermissionException : public shared::exceptions::BaseException {
 public:
  enum ErrorCode {
    EMPTY_STRING,
    INVALID_FORMAT,
    INVALID_CHARACTER,
    OUT_OF_RANGE,
    INVALID_OCTAL,
    CONVERSION_FAILED,
    CODE_COUNT
  };

  explicit PermissionException(const std::string& msg, ErrorCode code);
  PermissionException(const PermissionException& other);
  virtual ~PermissionException() throw();

  PermissionException& operator=(const PermissionException& other);

 private:
  static const std::pair<ErrorCode, std::string> K_CODE_MSGS[];

  static std::string getErrorMsg(ErrorCode code);
};

}  // namespace exceptions
}  // namespace shared
}  // namespace shared

#endif  // PERMISSION_EXCEPTION_HPP
