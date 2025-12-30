/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ValidationException.hpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/30 13:30:22 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/30 14:04:02 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef VALIDATION_EXCEPTION_HPP
#define VALIDATION_EXCEPTION_HPP

#include "shared/exceptions/BaseException.hpp"

namespace infrastructure {
namespace config {
namespace exceptions {

class ValidationException : public ::shared::exceptions::BaseException {
 public:
  enum ErrorCode {
    DUPLICATE_PORT,
    INVALID_PATH,
    MISSING_DIRECTIVE,
    PORT_CONFLICT,
    INVALID_METHOD,
    CODE_COUNT
  };

  explicit ValidationException(const std::string& msg, ErrorCode code);
  ValidationException(const ValidationException& other);
  virtual ~ValidationException() throw();

  ValidationException& operator=(const ValidationException& other);

 private:
  static const std::pair<ErrorCode, std::string> K_CODE_MSGS[];

  static std::string getErrorMsg(ErrorCode code);
};

}  // namespace exceptions
}  // namespace config
}  // namespace infrastructure

#endif  // VALIDATION_EXCEPTION_HPP
