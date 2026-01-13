/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FileSystemHelper.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: umeneses <umeneses@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/22 14:13:01 by dande-je          #+#    #+#             */
/*   Updated: 2026/01/08 23:04:56 by umeneses         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "domain/filesystem/value_objects/Path.hpp"
#include "domain/filesystem/value_objects/Permission.hpp"
#include "infrastructure/filesystem/adapters/FileSystemHelper.hpp"
#include "infrastructure/filesystem/exceptions/FileSystemHelperException.hpp"

#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <sstream>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <utime.h>

namespace infrastructure {
namespace filesystem {
namespace adapters {

FileSystemHelper::FileSystemHelper() {}

FileSystemHelper::FileSystemHelper(const FileSystemHelper& /*unused*/) {}

FileSystemHelper::~FileSystemHelper() {}

FileSystemHelper& FileSystemHelper::operator=(
    const FileSystemHelper& /*unused*/) {
  return *this;
}

bool FileSystemHelper::isPathEmpty(const std::string& path) {
  return path.empty();
}

std::string FileSystemHelper::getParentDirectory(const std::string& path) {
  if (path.empty() || path == "/") {
    return "";
  }

  std::size_t lastSlash = path.find_last_of('/');
  if (lastSlash == std::string::npos) {
    return ".";
  }

  if (lastSlash == 0) {
    return "/";
  }

  return path.substr(0, lastSlash);
}

bool FileSystemHelper::isRootDirectory(const std::string& path) {
  return path == "/";
}

FileSystemHelper* FileSystemHelper::getInstance() {
  static FileSystemHelper instance;
  return &instance;
}

bool FileSystemHelper::exists(const std::string& path) {
  if (isPathEmpty(path)) {
    return false;
  }

  struct stat pathStat;
  if (stat(path.c_str(), &pathStat) != 0) {
    return false;
  }

  return true;
}

bool FileSystemHelper::isDirectory(const std::string& path) {
  if (isPathEmpty(path)) {
    return false;
  }

  struct stat pathStat;
  if (stat(path.c_str(), &pathStat) != 0) {
    return false;
  }

  return S_ISDIR(pathStat.st_mode) != 0;
}

bool FileSystemHelper::isFile(const std::string& path) {
  if (isPathEmpty(path)) {
    return false;
  }

  struct stat pathStat;
  if (stat(path.c_str(), &pathStat) != 0) {
    return false;
  }

  return S_ISREG(pathStat.st_mode) != 0;
}

bool FileSystemHelper::isSymbolicLink(const std::string& path) {
  if (isPathEmpty(path)) {
    throw exceptions::FileSystemHelperException(
        "Path cannot be empty",
        exceptions::FileSystemHelperException::PATH_EMPTY);
  }

  struct stat pathStat;
  if (lstat(path.c_str(), &pathStat) != 0) {
    std::ostringstream oss;
    oss << "Path not found: '" << path << "'";
    throw exceptions::FileSystemHelperException(
        oss.str(), exceptions::FileSystemHelperException::PATH_NOT_FOUND);
  }

  if (S_ISLNK(pathStat.st_mode) == 0) {
    std::ostringstream oss;
    oss << "Path is not a symbolic link: '" << path << "'";
    throw exceptions::FileSystemHelperException(
        oss.str(),
        exceptions::FileSystemHelperException::PATH_NOT_SYMBOLIC_LINK);
  }

  return true;
}

bool FileSystemHelper::isReadable(const std::string& path) {
  if (isPathEmpty(path)) {
    throw exceptions::FileSystemHelperException(
        "Path cannot be empty",
        exceptions::FileSystemHelperException::PATH_EMPTY);
  }

  if (access(path.c_str(), R_OK) != 0) {
    std::ostringstream oss;
    oss << "Path is not readable: '" << path << "' - " << strerror(errno);
    throw exceptions::FileSystemHelperException(
        oss.str(), exceptions::FileSystemHelperException::PATH_NOT_READABLE);
  }

  return true;
}

bool FileSystemHelper::isWritable(const std::string& path) {
  if (isPathEmpty(path)) {
    throw exceptions::FileSystemHelperException(
        "Path cannot be empty",
        exceptions::FileSystemHelperException::PATH_EMPTY);
  }

  if (access(path.c_str(), W_OK) != 0) {
    std::ostringstream oss;
    oss << "Path is not writable: '" << path << "' - " << strerror(errno);
    throw exceptions::FileSystemHelperException(
        oss.str(), exceptions::FileSystemHelperException::PATH_NOT_WRITABLE);
  }

  return true;
}

bool FileSystemHelper::isExecutable(const std::string& path) {
  if (isPathEmpty(path)) {
    throw exceptions::FileSystemHelperException(
        "Path cannot be empty",
        exceptions::FileSystemHelperException::PATH_EMPTY);
  }

  if (access(path.c_str(), X_OK) != 0) {
    std::ostringstream oss;
    oss << "Path is not executable: '" << path << "' - " << strerror(errno);
    throw exceptions::FileSystemHelperException(
        oss.str(), exceptions::FileSystemHelperException::PATH_NOT_EXECUTABLE);
  }

  return true;
}

std::size_t FileSystemHelper::getFileSize(const std::string& path) {
  if (isPathEmpty(path)) {
    throw exceptions::FileSystemHelperException(
        "Path cannot be empty",
        exceptions::FileSystemHelperException::PATH_EMPTY);
  }

  struct stat pathStat;
  if (stat(path.c_str(), &pathStat) != 0) {
    std::ostringstream oss;
    oss << "Cannot get file size: path not found: '" << path << "'";
    throw exceptions::FileSystemHelperException(
        oss.str(), exceptions::FileSystemHelperException::PATH_NOT_FOUND);
  }

  if (S_ISREG(pathStat.st_mode) == 0) {
    std::ostringstream oss;
    oss << "Cannot get file size: path is not a file: '" << path << "'";
    throw exceptions::FileSystemHelperException(
        oss.str(), exceptions::FileSystemHelperException::PATH_NOT_FILE);
  }

  return static_cast<std::size_t>(pathStat.st_size);
}

bool FileSystemHelper::createDirectory(const std::string& path) {
  if (isPathEmpty(path)) {
    throw exceptions::FileSystemHelperException(
        "Path cannot be empty",
        exceptions::FileSystemHelperException::PATH_EMPTY);
  }

  if (isDirectory(path)) {
    return true;
  }

  mode_t mode = S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH;
  if (mkdir(path.c_str(), mode) != 0) {
    std::ostringstream oss;
    oss << "Failed to create directory: '" << path << "' - " << strerror(errno);
    throw exceptions::FileSystemHelperException(
        oss.str(),
        exceptions::FileSystemHelperException::CREATE_DIRECTORY_FAILED);
  }

  return true;
}

bool FileSystemHelper::createDirectoryRecursive(const std::string& path) {
  if (isPathEmpty(path)) {
    throw exceptions::FileSystemHelperException(
        "Path cannot be empty",
        exceptions::FileSystemHelperException::PATH_EMPTY);
  }

  if (isDirectory(path)) {
    return true;
  }

  std::string parent = getParentDirectory(path);
  if (!parent.empty() && parent != "." && parent != path) {
    try {
      createDirectoryRecursive(parent);
    } catch (const exceptions::FileSystemHelperException& e) {
      std::ostringstream oss;
      oss << "Failed to create parent directory for '" << path
          << "': " << e.what();
      throw exceptions::FileSystemHelperException(
          oss.str(),
          exceptions::FileSystemHelperException::CREATE_DIRECTORY_FAILED);
    }
  }

  return createDirectory(path);
}

bool FileSystemHelper::removeDirectory(const std::string& path) {
  if (isPathEmpty(path)) {
    throw exceptions::FileSystemHelperException(
        "Path cannot be empty",
        exceptions::FileSystemHelperException::PATH_EMPTY);
  }

  if (!isDirectory(path)) {
    std::ostringstream oss;
    oss << "Cannot remove directory: path is not a directory: '" << path << "'";
    throw exceptions::FileSystemHelperException(
        oss.str(), exceptions::FileSystemHelperException::PATH_NOT_DIRECTORY);
  }

  if (rmdir(path.c_str()) != 0) {
    std::ostringstream oss;
    oss << "Failed to remove directory: '" << path << "' - " << strerror(errno);
    throw exceptions::FileSystemHelperException(
        oss.str(),
        exceptions::FileSystemHelperException::REMOVE_DIRECTORY_FAILED);
  }

  return true;
}

bool FileSystemHelper::removeDirectoryRecursive(const std::string& path) {
  if (isPathEmpty(path)) {
    throw exceptions::FileSystemHelperException(
        "Path cannot be empty",
        exceptions::FileSystemHelperException::PATH_EMPTY);
  }

  if (!isDirectory(path)) {
    std::ostringstream oss;
    oss << "Cannot remove directory recursively: path is not a directory: '"
        << path << "'";
    throw exceptions::FileSystemHelperException(
        oss.str(), exceptions::FileSystemHelperException::PATH_NOT_DIRECTORY);
  }

  std::string command = "rm -rf \"" + path + "\"";
  if (system(command.c_str()) != 0) {
    std::ostringstream oss;
    oss << "Failed to remove directory recursively: '" << path << "'";
    throw exceptions::FileSystemHelperException(
        oss.str(),
        exceptions::FileSystemHelperException::REMOVE_DIRECTORY_FAILED);
  }

  return true;
}

bool FileSystemHelper::removeFile(const std::string& path) {
  if (isPathEmpty(path)) {
    throw exceptions::FileSystemHelperException(
        "Path cannot be empty",
        exceptions::FileSystemHelperException::PATH_EMPTY);
  }

  if (!isFile(path)) {
    std::ostringstream oss;
    oss << "Cannot remove file: path is not a file: '" << path << "'";
    throw exceptions::FileSystemHelperException(
        oss.str(), exceptions::FileSystemHelperException::PATH_NOT_FILE);
  }

  if (unlink(path.c_str()) != 0) {
    std::ostringstream oss;
    oss << "Failed to remove file: '" << path << "' - " << strerror(errno);
    throw exceptions::FileSystemHelperException(
        oss.str(), exceptions::FileSystemHelperException::REMOVE_FILE_FAILED);
  }

  return true;
}

bool FileSystemHelper::renameFile(const std::string& oldPath,
                                  const std::string& newPath) {
  if (isPathEmpty(oldPath) || isPathEmpty(newPath)) {
    throw exceptions::FileSystemHelperException(
        "Path cannot be empty",
        exceptions::FileSystemHelperException::PATH_EMPTY);
  }

  if (!isFile(oldPath)) {
    std::ostringstream oss;
    oss << "Cannot rename: source path is not a file: '" << oldPath << "'";
    throw exceptions::FileSystemHelperException(
        oss.str(), exceptions::FileSystemHelperException::PATH_NOT_FILE);
  }

  if (rename(oldPath.c_str(), newPath.c_str()) != 0) {
    std::ostringstream oss;
    oss << "Failed to rename file from '" << oldPath << "' to '" << newPath
        << "': " << strerror(errno);
    throw exceptions::FileSystemHelperException(
        oss.str(), exceptions::FileSystemHelperException::RENAME_FILE_FAILED);
  }

  return true;
}

bool FileSystemHelper::copyFile(const std::string& source,
                                const std::string& destination) {
  validateCopyFilePaths(source, destination);
  validateSourceIsFile(source);

  FILE* src = openSourceFileForCopying(source);
  FILE* dst = openDestinationFileForCopying(destination, src);

  char buffer[domain::filesystem::value_objects::Path::MAX_PATH_LENGTH];
  std::size_t bytesRead;
  bool success = true;

  while ((bytesRead = fread(buffer, 1, sizeof(buffer), src)) > 0) {
    if (fwrite(buffer, 1, bytesRead, dst) != bytesRead) {
      success = false;
      break;
    }
  }

  fclose(src);
  fclose(dst);

  if (!success) {
    try {
      removeFile(destination);
    } catch (const exceptions::FileSystemHelperException&) {
      throw;
    }

    std::ostringstream oss;
    oss << "Failed to copy file from '" << source << "' to '" << destination
        << "'";
    throw exceptions::FileSystemHelperException(
        oss.str(), exceptions::FileSystemHelperException::COPY_FILE_FAILED);
  }

  return true;
}

void FileSystemHelper::validateCopyFilePaths(const std::string& source,
                                             const std::string& destination) {
  if (isPathEmpty(source) || isPathEmpty(destination)) {
    throw exceptions::FileSystemHelperException(
        "Path cannot be empty",
        exceptions::FileSystemHelperException::PATH_EMPTY);
  }
}

void FileSystemHelper::validateSourceIsFile(const std::string& source) {
  if (!isFile(source)) {
    std::ostringstream oss;
    oss << "Cannot copy: source path is not a file: '" << source << "'";
    throw exceptions::FileSystemHelperException(
        oss.str(), exceptions::FileSystemHelperException::PATH_NOT_FILE);
  }
}

FILE* FileSystemHelper::openSourceFileForCopying(const std::string& source) {
  FILE* src = fopen(source.c_str(), "rb");
  if (src == 0) {
    std::ostringstream oss;
    oss << "Failed to open source file for copying: '" << source << "' - "
        << strerror(errno);
    throw exceptions::FileSystemHelperException(
        oss.str(), exceptions::FileSystemHelperException::COPY_FILE_FAILED);
  }
  return src;
}

FILE* FileSystemHelper::openDestinationFileForCopying(
    const std::string& destination, FILE* sourceHandle) {
  FILE* dst = fopen(destination.c_str(), "wb");
  if (dst == 0) {
    fclose(sourceHandle);
    std::ostringstream oss;
    oss << "Failed to open destination file for copying: '" << destination
        << "' - " << strerror(errno);
    throw exceptions::FileSystemHelperException(
        oss.str(), exceptions::FileSystemHelperException::COPY_FILE_FAILED);
  }
  return dst;
}

bool FileSystemHelper::setPermissions(const std::string& path, mode_t mode) {
  if (isPathEmpty(path)) {
    throw exceptions::FileSystemHelperException(
        "Path cannot be empty",
        exceptions::FileSystemHelperException::PATH_EMPTY);
  }

  if (!exists(path)) {
    std::ostringstream oss;
    oss << "Cannot set permissions: path not found: '" << path << "'";
    throw exceptions::FileSystemHelperException(
        oss.str(), exceptions::FileSystemHelperException::PATH_NOT_FOUND);
  }

  if (chmod(path.c_str(), mode) != 0) {
    std::ostringstream oss;
    oss << "Failed to set permissions for path: '" << path << "' - "
        << strerror(errno);
    throw exceptions::FileSystemHelperException(
        oss.str(),
        exceptions::FileSystemHelperException::SET_PERMISSIONS_FAILED);
  }

  return true;
}

mode_t FileSystemHelper::getPermissions(const std::string& path) {
  if (isPathEmpty(path)) {
    throw exceptions::FileSystemHelperException(
        "Path cannot be empty",
        exceptions::FileSystemHelperException::PATH_EMPTY);
  }

  if (!exists(path)) {
    std::ostringstream oss;
    oss << "Cannot get permissions: path not found: '" << path << "'";
    throw exceptions::FileSystemHelperException(
        oss.str(), exceptions::FileSystemHelperException::PATH_NOT_FOUND);
  }

  struct stat pathStat;
  if (stat(path.c_str(), &pathStat) != 0) {
    std::ostringstream oss;
    oss << "Failed to get permissions for path: '" << path << "' - "
        << strerror(errno);
    throw exceptions::FileSystemHelperException(
        oss.str(),
        exceptions::FileSystemHelperException::GET_PERMISSIONS_FAILED);
  }

  return pathStat.st_mode &
         domain::filesystem::value_objects::Permission::MAX_PERMISSION;
}

bool FileSystemHelper::changeOwner(const std::string& path, uid_t owner,
                                   gid_t group) {
  if (isPathEmpty(path)) {
    throw exceptions::FileSystemHelperException(
        "Path cannot be empty",
        exceptions::FileSystemHelperException::PATH_EMPTY);
  }

  if (!exists(path)) {
    std::ostringstream oss;
    oss << "Cannot change owner: path not found: '" << path << "'";
    throw exceptions::FileSystemHelperException(
        oss.str(), exceptions::FileSystemHelperException::PATH_NOT_FOUND);
  }

  if (chown(path.c_str(), owner, group) != 0) {
    std::ostringstream oss;
    oss << "Failed to change owner for path: '" << path << "' - "
        << strerror(errno);
    throw exceptions::FileSystemHelperException(
        oss.str(), exceptions::FileSystemHelperException::CHANGE_OWNER_FAILED);
  }

  return true;
}

time_t FileSystemHelper::getLastModifiedTime(const std::string& path) {
  if (isPathEmpty(path)) {
    throw exceptions::FileSystemHelperException(
        "Path cannot be empty",
        exceptions::FileSystemHelperException::PATH_EMPTY);
  }

  if (!exists(path)) {
    std::ostringstream oss;
    oss << "Cannot get last modified time: path not found: '" << path << "'";
    throw exceptions::FileSystemHelperException(
        oss.str(), exceptions::FileSystemHelperException::PATH_NOT_FOUND);
  }

  struct stat pathStat;
  if (stat(path.c_str(), &pathStat) != 0) {
    std::ostringstream oss;
    oss << "Failed to get last modified time for path: '" << path << "' - "
        << strerror(errno);
    throw exceptions::FileSystemHelperException(
        oss.str(), exceptions::FileSystemHelperException::IO_ERROR);
  }

  return pathStat.st_mtime;
}

bool FileSystemHelper::setLastModifiedTime(const std::string& path,
                                           time_t time) {
  if (isPathEmpty(path)) {
    throw exceptions::FileSystemHelperException(
        "Path cannot be empty",
        exceptions::FileSystemHelperException::PATH_EMPTY);
  }

  if (!exists(path)) {
    std::ostringstream oss;
    oss << "Cannot set last modified time: path not found: '" << path << "'";
    throw exceptions::FileSystemHelperException(
        oss.str(), exceptions::FileSystemHelperException::PATH_NOT_FOUND);
  }

  struct utimbuf times;
  times.actime = time;
  times.modtime = time;

  if (utime(path.c_str(), &times) != 0) {
    std::ostringstream oss;
    oss << "Failed to set last modified time for path: '" << path << "' - "
        << strerror(errno);
    throw exceptions::FileSystemHelperException(
        oss.str(), exceptions::FileSystemHelperException::IO_ERROR);
  }

  return true;
}

std::string FileSystemHelper::getCurrentWorkingDirectory() {
  char* cwd = getcwd(NULL, 0);
  if (cwd == 0) {
    std::ostringstream oss;
    oss << "Failed to get current working directory: " << strerror(errno);
    throw exceptions::FileSystemHelperException(
        oss.str(),
        exceptions::FileSystemHelperException::GET_CURRENT_DIR_FAILED);
  }

  std::string result(cwd);
  free(cwd);
  return result;
}

bool FileSystemHelper::changeWorkingDirectory(const std::string& path) {
  if (isPathEmpty(path)) {
    throw exceptions::FileSystemHelperException(
        "Path cannot be empty",
        exceptions::FileSystemHelperException::PATH_EMPTY);
  }

  if (!isDirectory(path)) {
    std::ostringstream oss;
    oss << "Cannot change working directory: path is not a directory: '" << path
        << "'";
    throw exceptions::FileSystemHelperException(
        oss.str(), exceptions::FileSystemHelperException::PATH_NOT_DIRECTORY);
  }

  if (chdir(path.c_str()) != 0) {
    std::ostringstream oss;
    oss << "Failed to change working directory to: '" << path << "' - "
        << strerror(errno);
    throw exceptions::FileSystemHelperException(
        oss.str(),
        exceptions::FileSystemHelperException::CHANGE_DIRECTORY_FAILED);
  }

  return true;
}

std::string FileSystemHelper::getAbsolutePath(const std::string& path) {
  if (isPathEmpty(path)) {
    throw exceptions::FileSystemHelperException(
        "Path cannot be empty",
        exceptions::FileSystemHelperException::PATH_EMPTY);
  }

  if (path[0] == '/') {
    return path;
  }

  try {
    std::string cwd = getCurrentWorkingDirectory();
    if (cwd.empty()) {
      throw exceptions::FileSystemHelperException(
          "Failed to get absolute path: current working directory is empty",
          exceptions::FileSystemHelperException::GET_ABSOLUTE_PATH_FAILED);
    }

    if (cwd[cwd.length() - 1] == '/') {
      return cwd + path;
    }
    return cwd + "/" + path;
  } catch (const exceptions::FileSystemHelperException& e) {
    std::ostringstream oss;
    oss << "Failed to get absolute path for '" << path << "': " << e.what();
    throw exceptions::FileSystemHelperException(
        oss.str(),
        exceptions::FileSystemHelperException::GET_ABSOLUTE_PATH_FAILED);
  }
}

std::string FileSystemHelper::resolveSymbolicLink(const std::string& path) {
  if (isPathEmpty(path)) {
    throw exceptions::FileSystemHelperException(
        "Path cannot be empty",
        exceptions::FileSystemHelperException::PATH_EMPTY);
  }

  if (!isSymbolicLink(path)) {
    std::ostringstream oss;
    oss << "Cannot resolve symbolic link: path is not a symbolic link: '"
        << path << "'";
    throw exceptions::FileSystemHelperException(
        oss.str(),
        exceptions::FileSystemHelperException::PATH_NOT_SYMBOLIC_LINK);
  }

  char buffer[domain::filesystem::value_objects::Path::MAX_PATH_LENGTH];
  ssize_t len = readlink(path.c_str(), buffer, sizeof(buffer) - 1);
  if (len == -1) {
    std::ostringstream oss;
    oss << "Failed to resolve symbolic link: '" << path << "' - "
        << strerror(errno);
    throw exceptions::FileSystemHelperException(
        oss.str(),
        exceptions::FileSystemHelperException::RESOLVE_SYMLINK_FAILED);
  }

  buffer[len] = '\0';
  return std::string(buffer);
}

}  // namespace adapters
}  // namespace filesystem
}  // namespace infrastructure
