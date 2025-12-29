/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   UploadAccessException.hpp                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/28 20:13:55 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/28 20:13:57 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UPLOAD_ACCESS_EXCEPTION_HPP
#define UPLOAD_ACCESS_EXCEPTION_HPP

#include "shared/exceptions/BaseException.hpp"

namespace domain {
namespace filesystem {
namespace exceptions {

class UploadAccessException : public ::shared::exceptions::BaseException {
 public:
  enum ErrorCode {
    EMPTY_STRING,
    INVALID_FORMAT,
    INVALID_WHO,
    INVALID_PERMISSION,
    OUT_OF_RANGE,
    DUPLICATE_PERMISSION,
    CODE_COUNT
  };

  explicit UploadAccessException(const std::string& msg, ErrorCode code);
  UploadAccessException(const UploadAccessException& other);
  virtual ~UploadAccessException() throw();

  UploadAccessException& operator=(const UploadAccessException& other);

 private:
  static const std::pair<ErrorCode, std::string> K_CODE_MSGS[];

  static std::string getErrorMsg(ErrorCode code);
};

}  // namespace exceptions
}  // namespace filesystem
}  // namespace domain

#endif  // UPLOAD_ACCESS_EXCEPTION_HPP
