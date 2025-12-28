/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FileHandlerException.hpp                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/23 12:50:22 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/27 23:08:06 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FILEHANDLEREXCEPTION_HPP
#define FILEHANDLEREXCEPTION_HPP

#include "shared/exceptions/BaseException.hpp"

namespace infrastructure {
namespace filesystem {
namespace exceptions {

class FileHandlerException : public ::shared::exceptions::BaseException {
 public:
  enum ErrorCode {
    FILE_NOT_FOUND,
    FILE_NOT_READABLE,
    FILE_NOT_WRITABLE,
    FILE_TOO_LARGE,
    DISK_SPACE_INSUFFICIENT,
    INVALID_FILE_TYPE,
    UPLOAD_SIZE_EXCEEDED,
    INVALID_FILENAME,
    PERMISSION_DENIED,
    IO_ERROR,
    LOCK_FAILED,
    UNLOCK_FAILED,
    INVALID_OFFSET,
    INVALID_SIZE,
    CHECKSUM_MISMATCH,
    COMPRESSION_FAILED,
    DECOMPRESSION_FAILED,
    ENCRYPTION_FAILED,
    DECRYPTION_FAILED,
    TEMPORARY_FILE_CREATION_FAILED,
    FILE_ALREADY_EXISTS,
    DIRECTORY_NOT_EMPTY,
    INVALID_CHUNK_SIZE,
    CODE_COUNT
  };

  explicit FileHandlerException(const std::string& msg, ErrorCode code);
  FileHandlerException(const FileHandlerException& other);
  virtual ~FileHandlerException() throw();

  FileHandlerException& operator=(const FileHandlerException& other);

 private:
  static const std::pair<ErrorCode, std::string> K_CODE_MSGS[];

  static std::string getErrorMsg(ErrorCode code);
};

}  // namespace exceptions
}  // namespace filesystem
}  // namespace infrastructure

#endif  // FILEHANDLEREXCEPTION_HPP
