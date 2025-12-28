/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FileHandlerException.cpp                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/23 12:50:46 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/27 23:08:57 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "infrastructure/filesystem/exceptions/FileHandlerException.hpp"

#include <sstream>

namespace infrastructure {
namespace filesystem {
namespace exceptions {

const std::pair<FileHandlerException::ErrorCode, std::string>
    FileHandlerException::K_CODE_MSGS[] = {
        std::make_pair(FileHandlerException::FILE_NOT_FOUND, "File not found"),
        std::make_pair(FileHandlerException::FILE_NOT_READABLE,
                       "File is not readable"),
        std::make_pair(FileHandlerException::FILE_NOT_WRITABLE,
                       "File is not writable"),
        std::make_pair(FileHandlerException::FILE_TOO_LARGE,
                       "File is too large"),
        std::make_pair(FileHandlerException::DISK_SPACE_INSUFFICIENT,
                       "Insufficient disk space"),
        std::make_pair(FileHandlerException::INVALID_FILE_TYPE,
                       "Invalid file type"),
        std::make_pair(FileHandlerException::UPLOAD_SIZE_EXCEEDED,
                       "Upload size exceeds limit"),
        std::make_pair(FileHandlerException::INVALID_FILENAME,
                       "Invalid filename"),
        std::make_pair(FileHandlerException::PERMISSION_DENIED,
                       "Permission denied"),
        std::make_pair(FileHandlerException::IO_ERROR, "I/O error occurred"),
        std::make_pair(FileHandlerException::LOCK_FAILED,
                       "Failed to lock file"),
        std::make_pair(FileHandlerException::UNLOCK_FAILED,
                       "Failed to unlock file"),
        std::make_pair(FileHandlerException::INVALID_OFFSET,
                       "Invalid file offset"),
        std::make_pair(FileHandlerException::INVALID_SIZE,
                       "Invalid size specified"),
        std::make_pair(FileHandlerException::CHECKSUM_MISMATCH,
                       "Checksum mismatch"),
        std::make_pair(FileHandlerException::COMPRESSION_FAILED,
                       "Compression failed"),
        std::make_pair(FileHandlerException::DECOMPRESSION_FAILED,
                       "Decompression failed"),
        std::make_pair(FileHandlerException::ENCRYPTION_FAILED,
                       "Encryption failed"),
        std::make_pair(FileHandlerException::DECRYPTION_FAILED,
                       "Decryption failed"),
        std::make_pair(FileHandlerException::TEMPORARY_FILE_CREATION_FAILED,
                       "Failed to create temporary file"),
        std::make_pair(FileHandlerException::FILE_ALREADY_EXISTS,
                       "File already exists"),
        std::make_pair(FileHandlerException::DIRECTORY_NOT_EMPTY,
                       "Directory is not empty"),
        std::make_pair(FileHandlerException::INVALID_CHUNK_SIZE,
                       "Invalid chunk size")};

FileHandlerException::FileHandlerException(const std::string& msg,
                                           ErrorCode code)
    : BaseException("", static_cast<int>(code)) {
  std::ostringstream oss;
  oss << getErrorMsg(code) << ": " << msg;
  this->m_whatMsg = oss.str();
}

FileHandlerException::FileHandlerException(const FileHandlerException& other)
    : BaseException(other) {}

FileHandlerException::~FileHandlerException() throw() {}

FileHandlerException& FileHandlerException::operator=(
    const FileHandlerException& other) {
  if (this != &other) {
    BaseException::operator=(other);
  }
  return *this;
}

std::string FileHandlerException::getErrorMsg(
    FileHandlerException::ErrorCode code) {
  for (int index = 0; index < CODE_COUNT; ++index) {
    if (K_CODE_MSGS[index].first == code) {
      return K_CODE_MSGS[index].second;
    }
  }
  return "unknown file handler error";
}

}  // namespace exceptions
}  // namespace filesystem
}  // namespace infrastructure
