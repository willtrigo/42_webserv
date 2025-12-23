/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   DirectoryListerException.hpp                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/22 21:33:03 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/22 21:33:13 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DIRECTORYLISTEREXCEPTION_HPP
#define DIRECTORYLISTEREXCEPTION_HPP

#include "shared/exceptions/BaseException.hpp"

namespace shared {
namespace exceptions {

class DirectoryListerException : public BaseException {
 public:
  enum ErrorCode {
    INVALID_DEPENDENCY,
    INVALID_DIRECTORY,
    DIRECTORY_NOT_FOUND,
    NOT_A_DIRECTORY,
    PERMISSION_DENIED,
    CANNOT_OPEN_DIRECTORY,
    LISTING_DISABLED,
    INVALID_SORT_CRITERIA,
    CANNOT_READ_ENTRIES,
    GENERATION_FAILED,
    CODE_COUNT
  };

  explicit DirectoryListerException(const std::string& msg, ErrorCode code);
  DirectoryListerException(const DirectoryListerException& other);
  virtual ~DirectoryListerException() throw();

  DirectoryListerException& operator=(const DirectoryListerException& other);

 private:
  static const std::pair<ErrorCode, std::string> K_CODE_MSGS[];

  static std::string getErrorMsg(ErrorCode code);
};

}  // namespace exceptions
}  // namespace shared

#endif  // DIRECTORYLISTEREXCEPTION_HPP
