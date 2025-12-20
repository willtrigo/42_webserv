/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PathException.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/20 01:03:46 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/20 01:04:59 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PATH_EXCEPTION_HPP
#define PATH_EXCEPTION_HPP

#include "shared/exceptions/BaseException.hpp"

namespace shared {
namespace exceptions {

class PathException : public BaseException {
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
}  // namespace shared

#endif  // PATH_EXCEPTION_HPP
