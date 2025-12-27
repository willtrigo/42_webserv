/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SizeException.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/20 09:55:17 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/27 17:43:49 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SIZE_EXCEPTION_HPP
#define SIZE_EXCEPTION_HPP

#include "shared/exceptions/BaseException.hpp"

namespace domain {
namespace filesystem {
namespace exceptions {

class SizeException : public ::shared::exceptions::BaseException {
 public:
  enum ErrorCode {
    EMPTY_STRING,
    INVALID_FORMAT,
    UNKNOWN_UNIT,
    NEGATIVE_VALUE,
    OVERFLOW,
    CONVERSION_FAILED,
    CODE_COUNT
  };

  explicit SizeException(const std::string& msg, ErrorCode code);
  SizeException(const SizeException& other);
  virtual ~SizeException() throw();

  SizeException& operator=(const SizeException& other);

 private:
  static const std::pair<ErrorCode, std::string> K_CODE_MSGS[];

  static std::string getErrorMsg(ErrorCode code);
};

}  // namespace exceptions
}  // namespace filesystem
}  // namespace domain

#endif  // SIZE_EXCEPTION_HPP
