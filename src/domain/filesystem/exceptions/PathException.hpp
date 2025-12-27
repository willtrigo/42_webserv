/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PathException.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: umeneses <umeneses@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/20 01:03:46 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/27 20:11:22 by umeneses         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PATH_EXCEPTION_HPP
#define PATH_EXCEPTION_HPP

#include "shared/exceptions/BaseException.hpp"

namespace domain {
namespace filesystem {
namespace exceptions {

class PathException : public ::shared::exceptions::BaseException {
 public:
  enum ErrorCode {
    EMPTY_PATH,
    INVALID_CHARACTER,
    TOO_LONG,
    NOT_ABSOLUTE,
    NOT_RELATIVE,
    MALFORMED,
    TRAVERSAL_ATTEMPT,
    INVALID_FILENAME,
    CODE_COUNT
  };

  explicit PathException(const std::string& msg, ErrorCode code);
  PathException(const PathException& other);
  virtual ~PathException() throw();

  PathException& operator=(const PathException& other);

 private:
  static const std::pair<ErrorCode, std::string> K_CODE_MSGS[];

  static std::string getErrorMsg(ErrorCode code);
};

}  // namespace exceptions
}  // namespace filesystem
}  // namespace domain

#endif  // PATH_EXCEPTION_HPP
