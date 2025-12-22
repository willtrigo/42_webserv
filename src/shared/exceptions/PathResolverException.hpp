/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PathResolverException.hpp                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/22 19:26:21 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/22 19:26:31 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PATHRESOLVEREXCEPTION_HPP
#define PATHRESOLVEREXCEPTION_HPP

#include "shared/exceptions/BaseException.hpp"

namespace shared {
namespace exceptions {

class PathResolverException : public BaseException {
 public:
  enum ErrorCode {
    INVALID_DEPENDENCY,
    INVALID_ROOT,
    ROOT_NOT_FOUND,
    ROOT_NOT_DIRECTORY,
    INVALID_REQUEST,
    PATH_NOT_FOUND,
    PATH_TRAVERSAL,
    INVALID_MAPPING,
    DIRECTORY_LISTING_DISABLED,
    PERMISSION_DENIED,
    CODE_COUNT
  };

  explicit PathResolverException(const std::string& msg, ErrorCode code);
  PathResolverException(const PathResolverException& other);
  virtual ~PathResolverException() throw();

  PathResolverException& operator=(const PathResolverException& other);

 private:
  static const std::pair<ErrorCode, std::string> K_CODE_MSGS[];

  static std::string getErrorMsg(ErrorCode code);
};

}  // namespace exceptions
}  // namespace shared

#endif  // PATHRESOLVEREXCEPTION_HPP
