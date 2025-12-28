/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FileSystemHelperException.cpp                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/22 18:05:56 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/27 23:18:25 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "infrastructure/filesystem/exceptions/FileSystemHelperException.hpp"

#include <sstream>

namespace infrastructure {
namespace filesystem {
namespace exceptions {

const std::pair<FileSystemHelperException::ErrorCode, std::string>
    FileSystemHelperException::K_CODE_MSGS[] = {
        std::make_pair(FileSystemHelperException::PATH_EMPTY,
                       "Path cannot be empty"),
        std::make_pair(FileSystemHelperException::PATH_NOT_FOUND,
                       "Path not found"),
        std::make_pair(FileSystemHelperException::PATH_NOT_DIRECTORY,
                       "Path is not a directory"),
        std::make_pair(FileSystemHelperException::PATH_NOT_FILE,
                       "Path is not a file"),
        std::make_pair(FileSystemHelperException::PATH_NOT_SYMBOLIC_LINK,
                       "Path is not a symbolic link"),
        std::make_pair(FileSystemHelperException::PATH_NOT_READABLE,
                       "Path is not readable"),
        std::make_pair(FileSystemHelperException::PATH_NOT_WRITABLE,
                       "Path is not writable"),
        std::make_pair(FileSystemHelperException::PATH_NOT_EXECUTABLE,
                       "Path is not executable"),
        std::make_pair(FileSystemHelperException::CREATE_DIRECTORY_FAILED,
                       "Failed to create directory"),
        std::make_pair(FileSystemHelperException::REMOVE_DIRECTORY_FAILED,
                       "Failed to remove directory"),
        std::make_pair(FileSystemHelperException::REMOVE_FILE_FAILED,
                       "Failed to remove file"),
        std::make_pair(FileSystemHelperException::RENAME_FILE_FAILED,
                       "Failed to rename file"),
        std::make_pair(FileSystemHelperException::COPY_FILE_FAILED,
                       "Failed to copy file"),
        std::make_pair(FileSystemHelperException::SET_PERMISSIONS_FAILED,
                       "Failed to set permissions"),
        std::make_pair(FileSystemHelperException::GET_PERMISSIONS_FAILED,
                       "Failed to get permissions"),
        std::make_pair(FileSystemHelperException::CHANGE_OWNER_FAILED,
                       "Failed to change owner"),
        std::make_pair(FileSystemHelperException::GET_CURRENT_DIR_FAILED,
                       "Failed to get current working directory"),
        std::make_pair(FileSystemHelperException::CHANGE_DIRECTORY_FAILED,
                       "Failed to change directory"),
        std::make_pair(FileSystemHelperException::GET_ABSOLUTE_PATH_FAILED,
                       "Failed to get absolute path"),
        std::make_pair(FileSystemHelperException::RESOLVE_SYMLINK_FAILED,
                       "Failed to resolve symbolic link"),
        std::make_pair(FileSystemHelperException::INVALID_PATH, "Invalid path"),
        std::make_pair(FileSystemHelperException::ACCESS_DENIED,
                       "Access denied"),
        std::make_pair(FileSystemHelperException::IO_ERROR,
                       "I/O error occurred")};

FileSystemHelperException::FileSystemHelperException(const std::string& msg,
                                                     ErrorCode code)
    : BaseException("", static_cast<int>(code)) {
  std::ostringstream oss;
  oss << getErrorMsg(code) << ": " << msg;
  this->m_whatMsg = oss.str();
}

FileSystemHelperException::FileSystemHelperException(
    const FileSystemHelperException& other)
    : BaseException(other) {}

FileSystemHelperException::~FileSystemHelperException() throw() {}

FileSystemHelperException& FileSystemHelperException::operator=(
    const FileSystemHelperException& other) {
  if (this != &other) {
    BaseException::operator=(other);
  }
  return *this;
}

std::string FileSystemHelperException::getErrorMsg(
    FileSystemHelperException::ErrorCode code) {
  for (int i = 0; i < CODE_COUNT; ++i) {
    if (K_CODE_MSGS[i].first == code) {
      return K_CODE_MSGS[i].second;
    }
  }
  return "unknown filesystem error";
}

}  // namespace exceptions
}  // namespace filesystem
}  // namespace infrastructure
