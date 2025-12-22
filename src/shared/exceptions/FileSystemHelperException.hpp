/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FileSystemHelperException.hpp                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/22 18:05:27 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/22 18:57:54 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FILE_SYSTEM_HELPER_EXCEPTION_HPP
#define FILE_SYSTEM_HELPER_EXCEPTION_HPP

#include "shared/exceptions/BaseException.hpp"

namespace shared {
namespace exceptions {

class FileSystemHelperException : public BaseException {
 public:
  enum ErrorCode {
    PATH_EMPTY,
    PATH_NOT_FOUND,
    PATH_NOT_DIRECTORY,
    PATH_NOT_FILE,
    PATH_NOT_SYMBOLIC_LINK,
    PATH_NOT_READABLE,
    PATH_NOT_WRITABLE,
    PATH_NOT_EXECUTABLE,
    CREATE_DIRECTORY_FAILED,
    REMOVE_DIRECTORY_FAILED,
    REMOVE_FILE_FAILED,
    RENAME_FILE_FAILED,
    COPY_FILE_FAILED,
    SET_PERMISSIONS_FAILED,
    GET_PERMISSIONS_FAILED,
    CHANGE_OWNER_FAILED,
    GET_CURRENT_DIR_FAILED,
    CHANGE_DIRECTORY_FAILED,
    GET_ABSOLUTE_PATH_FAILED,
    RESOLVE_SYMLINK_FAILED,
    INVALID_PATH,
    ACCESS_DENIED,
    IO_ERROR,
    CODE_COUNT
  };

  explicit FileSystemHelperException(const std::string& msg, ErrorCode code);
  FileSystemHelperException(const FileSystemHelperException& other);
  virtual ~FileSystemHelperException() throw();

  FileSystemHelperException& operator=(const FileSystemHelperException& other);

 private:
  static const std::pair<ErrorCode, std::string> K_CODE_MSGS[];

  static std::string getErrorMsg(ErrorCode code);
};

}  // namespace exceptions
}  // namespace shared

#endif  // FILE_SYSTEM_HELPER_EXCEPTION_HPP
