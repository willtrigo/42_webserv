/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FileHandler.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/23 13:02:00 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/27 23:50:13 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "domain/filesystem/value_objects/Path.hpp"
#include "domain/filesystem/value_objects/Permission.hpp"
#include "domain/filesystem/value_objects/Size.hpp"
#include "infrastructure/filesystem/adapters/FileHandler.hpp"
#include "infrastructure/filesystem/exceptions/FileHandlerException.hpp"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <dirent.h>
#include <fstream>
#include <iomanip>
#include <openssl/md5.h>
#include <openssl/sha.h>
#include <sstream>
#include <sys/file.h>
#include <sys/stat.h>
#include <unistd.h>
#include <zlib.h>

namespace infrastructure {
namespace filesystem {
namespace adapters {

FileHandler::FileHandler(FileSystemHelper* fileSystemHelper,
                         PathResolver* pathResolver)
    : m_fileSystemHelper(fileSystemHelper), m_pathResolver(pathResolver) {
  if (fileSystemHelper == NULL) {
    throw exceptions::FileHandlerException(
        "FileSystemHelper cannot be NULL",
        exceptions::FileHandlerException::IO_ERROR);
  }
  if (pathResolver == NULL) {
    throw exceptions::FileHandlerException(
        "PathResolver cannot be NULL",
        exceptions::FileHandlerException::IO_ERROR);
  }
}

FileHandler::~FileHandler() {
  for (std::map<std::string, LockInfo>::iterator it = m_fileLocks.begin();
       it != m_fileLocks.end(); ++it) {
    if (it->second.fd >= 0) {
      ::close(it->second.fd);
    }
  }
  m_fileLocks.clear();
}

FileHandler::FileHandler(const FileHandler& other)
    : m_fileSystemHelper(other.m_fileSystemHelper),
      m_pathResolver(other.m_pathResolver),
      m_fileLocks(other.m_fileLocks) {}

FileHandler& FileHandler::operator=(const FileHandler& other) {
  if (this != &other) {
    m_fileSystemHelper = other.m_fileSystemHelper;
    m_pathResolver = other.m_pathResolver;
    m_fileLocks = other.m_fileLocks;
  }
  return *this;
}

FileMetadata FileHandler::getMetadata(
    const domain::filesystem::value_objects::Path& filePath) const {
  FileMetadata metadata;
  metadata.path = filePath;

  std::string pathStr = filePath.toString();

  if (!FileSystemHelper::exists(pathStr)) {
    throw exceptions::FileHandlerException(
        "File not found: " + pathStr,
        exceptions::FileHandlerException::FILE_NOT_FOUND);
  }

  struct stat fileStat;
  if (stat(pathStr.c_str(), &fileStat) != 0) {
    throw exceptions::FileHandlerException(
        "Cannot get file stats: " + pathStr,
        exceptions::FileHandlerException::IO_ERROR);
  }

  metadata.size =
      domain::filesystem::value_objects::Size(static_cast<std::size_t>(fileStat.st_size));
  metadata.permissions =
      domain::filesystem::value_objects::Permission(fileStat.st_mode & 07777);
  metadata.isDirectory = S_ISDIR(fileStat.st_mode);
  metadata.isSymbolicLink = S_ISLNK(fileStat.st_mode);

  struct tm* timeinfo = localtime(&fileStat.st_mtime);
  if (timeinfo != NULL) {
    char buffer[80];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);
    metadata.lastModified = std::string(buffer);
  }

  metadata.mimeType = detectMimeType(filePath);

  std::string filename = filePath.getFilename();
  metadata.isHidden = !filename.empty() && filename[0] == '.';

  return metadata;
}

std::vector<char> FileHandler::readFile(
    const domain::filesystem::value_objects::Path& filePath) const {
  validateFileForReading(filePath);

  std::string pathStr = filePath.toString();
  std::ifstream file(pathStr.c_str(), std::ios::binary | std::ios::ate);

  if (!file.is_open()) {
    throw exceptions::FileHandlerException(
        "Cannot open file for reading: " + pathStr,
        exceptions::FileHandlerException::FILE_NOT_READABLE);
  }

  std::streamsize size = file.tellg();
  file.seekg(0, std::ios::beg);

  std::vector<char> buffer(static_cast<std::size_t>(size));

  if (!file.read(buffer.data(), size)) {
    throw exceptions::FileHandlerException(
        "Failed to read file: " + pathStr,
        exceptions::FileHandlerException::IO_ERROR);
  }

  return buffer;
}

std::vector<char> FileHandler::readFileChunk(
    const domain::filesystem::value_objects::Path& filePath, std::size_t offset,
    std::size_t chunkSize) const {
  validateFileForReading(filePath);

  if (chunkSize < MIN_CHUNK_SIZE || chunkSize > MAX_CHUNK_SIZE) {
    throw exceptions::FileHandlerException(
        "Invalid chunk size",
        exceptions::FileHandlerException::INVALID_CHUNK_SIZE);
  }

  std::string pathStr = filePath.toString();
  struct stat fileStat;
  if (stat(pathStr.c_str(), &fileStat) != 0) {
    throw exceptions::FileHandlerException(
        "Cannot get file stats: " + pathStr,
        exceptions::FileHandlerException::IO_ERROR);
  }

  domain::filesystem::value_objects::Size fileSize(
      static_cast<std::size_t>(fileStat.st_size));

  if (!validateChunkParameters(offset, chunkSize, fileSize)) {
    throw exceptions::FileHandlerException(
        "Invalid chunk parameters",
        exceptions::FileHandlerException::INVALID_OFFSET);
  }

  std::ifstream file(pathStr.c_str(), std::ios::binary);
  if (!file.is_open()) {
    throw exceptions::FileHandlerException(
        "Cannot open file for reading: " + pathStr,
        exceptions::FileHandlerException::FILE_NOT_READABLE);
  }

  file.seekg(static_cast<std::streamoff>(offset), std::ios::beg);

  std::size_t remaining = static_cast<std::size_t>(fileStat.st_size) - offset;
  std::size_t actualChunkSize = (chunkSize > remaining) ? remaining : chunkSize;

  std::vector<char> buffer(actualChunkSize);

  if (!file.read(buffer.data(),
                 static_cast<std::streamsize>(actualChunkSize))) {
    throw exceptions::FileHandlerException(
        "Failed to read file chunk: " + pathStr,
        exceptions::FileHandlerException::IO_ERROR);
  }

  return buffer;
}

bool FileHandler::writeFile(const domain::filesystem::value_objects::Path& filePath,
                            const std::vector<char>& data,
                            bool overwrite) const {
  validateFileForWriting(filePath, overwrite);

  std::string pathStr = filePath.toString();

  domain::filesystem::value_objects::Size requiredSpace(data.size());
  domain::filesystem::value_objects::Size availableSpace = getAvailableDiskSpace(filePath);

  if (availableSpace < requiredSpace) {
    throw exceptions::FileHandlerException(
        "Insufficient disk space",
        exceptions::FileHandlerException::DISK_SPACE_INSUFFICIENT);
  }

  std::ofstream file(pathStr.c_str(), std::ios::binary);
  if (!file.is_open()) {
    throw exceptions::FileHandlerException(
        "Cannot open file for writing: " + pathStr,
        exceptions::FileHandlerException::FILE_NOT_WRITABLE);
  }

  if (!file.write(data.data(), static_cast<std::streamsize>(data.size()))) {
    throw exceptions::FileHandlerException(
        "Failed to write file: " + pathStr,
        exceptions::FileHandlerException::IO_ERROR);
  }

  return true;
}

bool FileHandler::writeFile(const domain::filesystem::value_objects::Path& filePath,
                            const std::string& data, bool overwrite) const {
  std::vector<char> charData(data.begin(), data.end());
  return writeFile(filePath, charData, overwrite);
}

bool FileHandler::appendToFile(const domain::filesystem::value_objects::Path& filePath,
                               const std::vector<char>& data) const {
  std::string pathStr = filePath.toString();

  if (!FileSystemHelper::exists(pathStr)) {
    return writeFile(filePath, data, true);
  }

  if (!FileSystemHelper::isWritable(pathStr)) {
    throw exceptions::FileHandlerException(
        "File is not writable: " + pathStr,
        exceptions::FileHandlerException::FILE_NOT_WRITABLE);
  }

  domain::filesystem::value_objects::Size requiredSpace(data.size());
  domain::filesystem::value_objects::Size availableSpace = getAvailableDiskSpace(filePath);

  if (availableSpace < requiredSpace) {
    throw exceptions::FileHandlerException(
        "Insufficient disk space",
        exceptions::FileHandlerException::DISK_SPACE_INSUFFICIENT);
  }

  std::ofstream file(pathStr.c_str(), std::ios::binary | std::ios::app);
  if (!file.is_open()) {
    throw exceptions::FileHandlerException(
        "Cannot open file for appending: " + pathStr,
        exceptions::FileHandlerException::FILE_NOT_WRITABLE);
  }

  if (!file.write(data.data(), static_cast<std::streamsize>(data.size()))) {
    throw exceptions::FileHandlerException(
        "Failed to append to file: " + pathStr,
        exceptions::FileHandlerException::IO_ERROR);
  }

  return true;
}

bool FileHandler::appendToFile(const domain::filesystem::value_objects::Path& filePath,
                               const std::string& data) const {
  std::vector<char> charData(data.begin(), data.end());
  return appendToFile(filePath, charData);
}

bool FileHandler::createFile(
    const domain::filesystem::value_objects::Path& filePath,
    const domain::filesystem::value_objects::Permission& permissions) const {
  std::string pathStr = filePath.toString();

  if (FileSystemHelper::exists(pathStr)) {
    throw exceptions::FileHandlerException(
        "File already exists: " + pathStr,
        exceptions::FileHandlerException::FILE_ALREADY_EXISTS);
  }

  domain::filesystem::value_objects::Path parentDir = filePath;
  std::string parentStr = parentDir.getDirectory();
  if (!parentStr.empty() &&
      !FileSystemHelper::exists(parentStr)) {
    domain::filesystem::value_objects::Path parentPath(parentStr, true);
    createDirectory(parentPath);
  }

  std::ofstream file(pathStr.c_str(), std::ios::binary);
  if (!file.is_open()) {
    throw exceptions::FileHandlerException(
        "Cannot create file: " + pathStr,
        exceptions::FileHandlerException::IO_ERROR);
  }
  file.close();

  return setPermissions(filePath, permissions);
}

bool FileHandler::deleteFile(
    const domain::filesystem::value_objects::Path& filePath) const {
  std::string pathStr = filePath.toString();

  if (!FileSystemHelper::exists(pathStr)) {
    throw exceptions::FileHandlerException(
        "File not found: " + pathStr,
        exceptions::FileHandlerException::FILE_NOT_FOUND);
  }

  if (FileSystemHelper::isDirectory(pathStr)) {
    throw exceptions::FileHandlerException(
        "Path is a directory, use deleteDirectory instead: " + pathStr,
        exceptions::FileHandlerException::INVALID_FILE_TYPE);
  }

  if (std::remove(pathStr.c_str()) != 0) {
    throw exceptions::FileHandlerException(
        "Failed to delete file: " + pathStr,
        exceptions::FileHandlerException::IO_ERROR);
  }

  return true;
}

bool FileHandler::renameFile(const domain::filesystem::value_objects::Path& oldPath,
                             const domain::filesystem::value_objects::Path& newPath) const {
  std::string oldStr = oldPath.toString();
  std::string newStr = newPath.toString();

  if (!FileSystemHelper::exists(oldStr)) {
    throw exceptions::FileHandlerException(
        "Source file not found: " + oldStr,
        exceptions::FileHandlerException::FILE_NOT_FOUND);
  }

  if (FileSystemHelper::exists(newStr)) {
    throw exceptions::FileHandlerException(
        "Destination file already exists: " + newStr,
        exceptions::FileHandlerException::FILE_ALREADY_EXISTS);
  }

  domain::filesystem::value_objects::Path newParent = newPath;
  std::string newParentStr = newParent.getDirectory();
  if (!newParentStr.empty() &&
      !FileSystemHelper::exists(newParentStr)) {
    domain::filesystem::value_objects::Path parentPath(newParentStr, true);
    createDirectory(parentPath);
  }

  if (std::rename(oldStr.c_str(), newStr.c_str()) != 0) {
    throw exceptions::FileHandlerException(
        "Failed to rename file: " + oldStr + " to " + newStr,
        exceptions::FileHandlerException::IO_ERROR);
  }

  return true;
}

bool FileHandler::copyFile(const domain::filesystem::value_objects::Path& sourcePath,
                           const domain::filesystem::value_objects::Path& destinationPath,
                           bool overwrite) const {
  return copyFileInternal(sourcePath, destinationPath, overwrite);
}

bool FileHandler::createDirectory(
    const domain::filesystem::value_objects::Path& dirPath,
    const domain::filesystem::value_objects::Permission& permissions) const {
  validateDirectoryForCreation(dirPath);

  std::string pathStr = dirPath.toString();

  if (mkdir(pathStr.c_str(), 0777) != 0) {
    throw exceptions::FileHandlerException(
        "Failed to create directory: " + pathStr,
        exceptions::FileHandlerException::IO_ERROR);
  }

  return setPermissions(dirPath, permissions);
}

bool FileHandler::deleteDirectory(const domain::filesystem::value_objects::Path& dirPath,
                                  bool recursive) const {
  validateDirectoryForDeletion(dirPath, recursive);

  std::string pathStr = dirPath.toString();

  if (recursive) {
    return deleteDirectoryRecursive(dirPath);
  }

  if (rmdir(pathStr.c_str()) != 0) {
    throw exceptions::FileHandlerException(
        "Failed to delete directory: " + pathStr,
        exceptions::FileHandlerException::IO_ERROR);
  }

  return true;
}

domain::filesystem::value_objects::Path FileHandler::createTemporaryFile(
    const std::string& prefix, const std::string& suffix,
    const domain::filesystem::value_objects::Path& directory) const {
  std::string tempFilename =
      generateTemporaryFilename(prefix, suffix, directory);
  domain::filesystem::value_objects::Path tempPath(tempFilename, true);

  if (!createFile(tempPath)) {
    throw exceptions::FileHandlerException(
        "Failed to create temporary file: " + tempFilename,
        exceptions::FileHandlerException::
            TEMPORARY_FILE_CREATION_FAILED);
  }

  return tempPath;
}

domain::filesystem::value_objects::Path FileHandler::createTemporaryDirectory(
    const std::string& prefix,
    const domain::filesystem::value_objects::Path& directory) const {
  std::string tempDirname = generateTemporaryFilename(prefix, "", directory);
  domain::filesystem::value_objects::Path tempPath(tempDirname, true);

  if (!createDirectory(tempPath)) {
    throw exceptions::FileHandlerException(
        "Failed to create temporary directory: " + tempDirname,
        exceptions::FileHandlerException::IO_ERROR);
  }

  return tempPath;
}

bool FileHandler::lockFile(const domain::filesystem::value_objects::Path& filePath) const {
  std::string pathStr = filePath.toString();

  if (!FileSystemHelper::exists(pathStr)) {
    throw exceptions::FileHandlerException(
        "File not found: " + pathStr,
        exceptions::FileHandlerException::FILE_NOT_FOUND);
  }

  // Check if already locked
  if (m_fileLocks.find(pathStr) != m_fileLocks.end()) {
    return true;  // Already locked
  }

  int fd = open(pathStr.c_str(), O_RDWR);
  if (fd < 0) {
    throw exceptions::FileHandlerException(
        "Cannot open file for locking: " + pathStr,
        exceptions::FileHandlerException::IO_ERROR);
  }

  if (flock(fd, LOCK_EX | LOCK_NB) != 0) {
    close(fd);
    throw exceptions::FileHandlerException(
        "File is already locked: " + pathStr,
        exceptions::FileHandlerException::LOCK_FAILED);
  }

  LockInfo lockInfo;
  lockInfo.fd = fd;
  lockInfo.lockPath = pathStr;
  m_fileLocks[pathStr] = lockInfo;

  return true;
}

bool FileHandler::unlockFile(
    const domain::filesystem::value_objects::Path& filePath) const {
  std::string pathStr = filePath.toString();

  std::map<std::string, LockInfo>::iterator it = m_fileLocks.find(pathStr);
  if (it == m_fileLocks.end()) {
    throw exceptions::FileHandlerException(
        "File is not locked: " + pathStr,
        exceptions::FileHandlerException::UNLOCK_FAILED);
  }

  if (flock(it->second.fd, LOCK_UN) != 0) {
    throw exceptions::FileHandlerException(
        "Failed to unlock file: " + pathStr,
        exceptions::FileHandlerException::UNLOCK_FAILED);
  }

  close(it->second.fd);
  m_fileLocks.erase(it);

  return true;
}

bool FileHandler::isFileLocked(
    const domain::filesystem::value_objects::Path& filePath) const {
  std::string pathStr = filePath.toString();

  if (m_fileLocks.find(pathStr) != m_fileLocks.end()) {
    return true;
  }

  // Try to lock the file temporarily
  int fd = open(pathStr.c_str(), O_RDWR);
  if (fd < 0) {
    return false;
  }

  bool isLocked = (flock(fd, LOCK_EX | LOCK_NB) != 0);

  if (!isLocked) {
    flock(fd, LOCK_UN);
  }

  close(fd);
  return isLocked;
}

std::string FileHandler::calculateChecksum(
    const domain::filesystem::value_objects::Path& filePath,
    const std::string& algorithm) const {
  if (!isSupportedChecksumAlgorithm(algorithm)) {
    throw exceptions::FileHandlerException(
        "Unsupported checksum algorithm: " + algorithm,
        exceptions::FileHandlerException::IO_ERROR);
  }

  std::vector<char> data = readFile(filePath);

  if (algorithm == "md5") {
    return calculateMD5(data);
  }
  if (algorithm == "sha1") {
    return calculateSHA1(data);
  }
  if (algorithm == "sha256") {
    return calculateSHA256(data);
  }

  return "";
}

bool FileHandler::verifyChecksum(const domain::filesystem::value_objects::Path& filePath,
                                 const std::string& expectedChecksum,
                                 const std::string& algorithm) const {
  std::string actualChecksum = calculateChecksum(filePath, algorithm);
  return actualChecksum == expectedChecksum;
}

domain::filesystem::value_objects::Size FileHandler::getAvailableDiskSpace(
    const domain::filesystem::value_objects::Path& path) const {
  std::string pathStr = path.toString();

  // This is a simplified implementation
  // In a real implementation, you would use statvfs or similar
  return domain::filesystem::value_objects::Size(1024 * 1024 * 1024);  // 1GB default
}

bool FileHandler::validateFileSize(
    const domain::filesystem::value_objects::Path& filePath,
    const domain::filesystem::value_objects::Size& maxSize) const {
  FileMetadata metadata = getMetadata(filePath);
  return metadata.size <= maxSize;
}

std::string FileHandler::detectMimeType(
    const domain::filesystem::value_objects::Path& filePath) const {
  std::string filename = filePath.getFilename();
  std::string extension = getFileExtension(filename);
  return extensionToMimeType(extension);
}

bool FileHandler::setPermissions(
    const domain::filesystem::value_objects::Path& filePath,
    const domain::filesystem::value_objects::Permission& permissions) const {
  std::string pathStr = filePath.toString();

  if (!FileSystemHelper::exists(pathStr)) {
    throw exceptions::FileHandlerException(
        "File not found: " + pathStr,
        exceptions::FileHandlerException::FILE_NOT_FOUND);
  }

  if (chmod(pathStr.c_str(), permissions.getOctalValue()) != 0) {
    throw exceptions::FileHandlerException(
        "Failed to set permissions: " + pathStr,
        exceptions::FileHandlerException::PERMISSION_DENIED);
  }

  return true;
}

bool FileHandler::isSafeFilename(const std::string& filename) const {
  if (filename.empty() || filename.length() > MAX_FILENAME_LENGTH) {
    return false;
  }

  if (filename.find("..") != std::string::npos ||
      filename.find("/") != std::string::npos ||
      filename.find("\\") != std::string::npos) {
    return false;
  }

  const std::string dangerousChars = "\"<>|:*?";
  for (std::size_t i = 0; i < filename.length(); ++i) {
    if (dangerousChars.find(filename[i]) != std::string::npos) {
      return false;
    }
  }

  return true;
}

std::string FileHandler::sanitizeFilename(const std::string& filename) const {
  if (filename.empty()) {
    return "file";
  }

  std::string sanitized;
  const std::string dangerousChars = "\"<>|:*?\\/";

  for (std::size_t i = 0; i < filename.length(); ++i) {
    char c = filename[i];
    if (dangerousChars.find(c) == std::string::npos) {
      sanitized += c;
    } else {
      sanitized += '_';
    }
  }

  std::size_t pos;
  while ((pos = sanitized.find("..")) != std::string::npos) {
    sanitized.replace(pos, 2, "__");
  }

  if (sanitized.length() > MAX_FILENAME_LENGTH) {
    sanitized = sanitized.substr(0, MAX_FILENAME_LENGTH);
  }

  return sanitized;
}

void FileHandler::validateFileForReading(
    const domain::filesystem::value_objects::Path& filePath) const {
  std::string pathStr = filePath.toString();

  if (!FileSystemHelper::exists(pathStr)) {
    throw exceptions::FileHandlerException(
        "File not found: " + pathStr,
        exceptions::FileHandlerException::FILE_NOT_FOUND);
  }

  if (!FileSystemHelper::isReadable(pathStr)) {
    throw exceptions::FileHandlerException(
        "File is not readable: " + pathStr,
        exceptions::FileHandlerException::FILE_NOT_READABLE);
  }

  if (FileSystemHelper::isDirectory(pathStr)) {
    throw exceptions::FileHandlerException(
        "Path is a directory: " + pathStr,
        exceptions::FileHandlerException::INVALID_FILE_TYPE);
  }
}

void FileHandler::validateFileForWriting(
    const domain::filesystem::value_objects::Path& filePath, bool overwrite) const {
  std::string pathStr = filePath.toString();

  if (FileSystemHelper::exists(pathStr)) {
    if (!overwrite) {
      throw exceptions::FileHandlerException(
          "File already exists: " + pathStr,
          exceptions::FileHandlerException::FILE_ALREADY_EXISTS);
    }

    if (!FileSystemHelper::isWritable(pathStr)) {
      throw exceptions::FileHandlerException(
          "File is not writable: " + pathStr,
          exceptions::FileHandlerException::FILE_NOT_WRITABLE);
    }
  } else {
    domain::filesystem::value_objects::Path parent = filePath;
    std::string parentStr = parent.getDirectory();
    if (!parentStr.empty() &&
        FileSystemHelper::exists(parentStr)) {
      if (!FileSystemHelper::isWritable(
              parentStr)) {
        throw exceptions::FileHandlerException(
            "Parent directory is not writable: " + parentStr,
            exceptions::FileHandlerException::PERMISSION_DENIED);
      }
    }
  }
}

void FileHandler::validateDirectoryForCreation(
    const domain::filesystem::value_objects::Path& dirPath) const {
  std::string pathStr = dirPath.toString();

  if (FileSystemHelper::exists(pathStr)) {
    throw exceptions::FileHandlerException(
        "Directory already exists: " + pathStr,
        exceptions::FileHandlerException::FILE_ALREADY_EXISTS);
  }
}

void FileHandler::validateDirectoryForDeletion(
    const domain::filesystem::value_objects::Path& dirPath, bool recursive) const {
  std::string pathStr = dirPath.toString();

  if (!FileSystemHelper::exists(pathStr)) {
    throw exceptions::FileHandlerException(
        "Directory not found: " + pathStr,
        exceptions::FileHandlerException::FILE_NOT_FOUND);
  }

  if (!FileSystemHelper::isDirectory(pathStr)) {
    throw exceptions::FileHandlerException(
        "Path is not a directory: " + pathStr,
        exceptions::FileHandlerException::INVALID_FILE_TYPE);
  }

  if (!recursive) {
    DIR* dir = opendir(pathStr.c_str());
    if (dir == NULL) {
      throw exceptions::FileHandlerException(
          "Cannot open directory: " + pathStr,
          exceptions::FileHandlerException::IO_ERROR);
    }

    struct dirent* entry;
    int count = 0;
    while ((entry = readdir(dir)) != NULL) {
      if (++count > 2) {  // . and ..
        closedir(dir);
        throw exceptions::FileHandlerException(
            "Directory is not empty: " + pathStr,
            exceptions::FileHandlerException::DIRECTORY_NOT_EMPTY);
      }
    }

    closedir(dir);
  }
}

bool FileHandler::copyFileInternal(
    const domain::filesystem::value_objects::Path& sourcePath,
    const domain::filesystem::value_objects::Path& destinationPath, bool overwrite) const {
  std::string sourceStr = sourcePath.toString();
  std::string destStr = destinationPath.toString();

  if (!FileSystemHelper::exists(sourceStr)) {
    throw exceptions::FileHandlerException(
        "Source file not found: " + sourceStr,
        exceptions::FileHandlerException::FILE_NOT_FOUND);
  }

  if (FileSystemHelper::exists(destStr) &&
      !overwrite) {
    throw exceptions::FileHandlerException(
        "Destination file already exists: " + destStr,
        exceptions::FileHandlerException::FILE_ALREADY_EXISTS);
  }

  std::ifstream source(sourceStr.c_str(), std::ios::binary);
  if (!source.is_open()) {
    throw exceptions::FileHandlerException(
        "Cannot open source file: " + sourceStr,
        exceptions::FileHandlerException::FILE_NOT_READABLE);
  }

  std::ofstream dest(destStr.c_str(), std::ios::binary);
  if (!dest.is_open()) {
    source.close();
    throw exceptions::FileHandlerException(
        "Cannot open destination file: " + destStr,
        exceptions::FileHandlerException::FILE_NOT_WRITABLE);
  }

  char buffer[BUFFER_SIZE];
  while ((source.read(buffer, BUFFER_SIZE) != 0) || source.gcount() > 0) {
    dest.write(buffer, source.gcount());
    if (!dest) {
      source.close();
      dest.close();
      throw exceptions::FileHandlerException(
          "Failed to write to destination file: " + destStr,
          exceptions::FileHandlerException::IO_ERROR);
    }
  }

  source.close();
  dest.close();

  return true;
}

bool FileHandler::deleteDirectoryRecursive(
    const domain::filesystem::value_objects::Path& dirPath) const {
  std::string pathStr = dirPath.toString();

  DIR* dir = opendir(pathStr.c_str());
  if (dir == NULL) {
    throw exceptions::FileHandlerException(
        "Cannot open directory: " + pathStr,
        exceptions::FileHandlerException::IO_ERROR);
  }

  struct dirent* entry;
  while ((entry = readdir(dir)) != NULL) {
    std::string entryName = entry->d_name;

    if (entryName == "." || entryName == "..") {
      continue;
    }

    std::string fullPath = pathStr + "/" + entryName;
    domain::filesystem::value_objects::Path entryPath(fullPath, true);

    struct stat statBuf;
    if (stat(fullPath.c_str(), &statBuf) != 0) {
      closedir(dir);
      throw exceptions::FileHandlerException(
          "Cannot stat file: " + fullPath,
          exceptions::FileHandlerException::IO_ERROR);
    }

    if (S_ISDIR(statBuf.st_mode)) {
      if (!deleteDirectoryRecursive(entryPath)) {
        closedir(dir);
        return false;
      }
    } else {
      if (!deleteFile(entryPath)) {
        closedir(dir);
        return false;
      }
    }
  }

  closedir(dir);

  if (rmdir(pathStr.c_str()) != 0) {
    throw exceptions::FileHandlerException(
        "Failed to delete directory: " + pathStr,
        exceptions::FileHandlerException::IO_ERROR);
  }

  return true;
}

std::string FileHandler::toHexDigest(const unsigned char* digest,
                                     unsigned int length) const {
  std::ostringstream oss;
  oss << std::hex << std::setfill('0');
  for (unsigned int i = 0; i < length; ++i) {
    oss << std::setw(2) << static_cast<unsigned int>(digest[i]);
  }
  return oss.str();
}

unsigned int FileHandler::leftRotate(unsigned int value,
                                     unsigned int bits) const {
  return (value << bits) | (value >> (32 - bits));
}

unsigned int FileHandler::rightRotate(unsigned int value,
                                      unsigned int bits) const {
  return (value >> bits) | (value << (32 - bits));
}

void FileHandler::sha1Transform(unsigned int* state,
                                const unsigned char* block) const {
  unsigned int w[80];
  for (int i = 0; i < 16; ++i) {
    w[i] = (static_cast<unsigned int>(block[i * 4]) << 24) |
           (static_cast<unsigned int>(block[i * 4 + 1]) << 16) |
           (static_cast<unsigned int>(block[i * 4 + 2]) << 8) |
           static_cast<unsigned int>(block[i * 4 + 3]);
  }
  for (int i = 16; i < 80; ++i) {
    w[i] = leftRotate(w[i - 3] ^ w[i - 8] ^ w[i - 14] ^ w[i - 16], 1);
  }

  unsigned int a = state[0], b = state[1], c = state[2], d = state[3],
               e = state[4];
  for (int i = 0; i < 80; ++i) {
    unsigned int f, k;
    if (i < 20) {
      f = (b & c) | (~b & d);
      k = 0x5A827999;
    } else if (i < 40) {
      f = b ^ c ^ d;
      k = 0x6ED9EBA1;
    } else if (i < 60) {
      f = (b & c) | (b & d) | (c & d);
      k = 0x8F1BBCDC;
    } else {
      f = b ^ c ^ d;
      k = 0xCA62C1D6;
    }
    unsigned int temp = leftRotate(a, 5) + f + e + k + w[i];
    e = d;
    d = c;
    c = leftRotate(b, 30);
    b = a;
    a = temp;
  }
  state[0] += a;
  state[1] += b;
  state[2] += c;
  state[3] += d;
  state[4] += e;
}

// TODO: remove magic number
void FileHandler::sha1PadAndProcess(const std::vector<char>& data,
                                    unsigned int* state, U64& bitLen) const {
  unsigned char padded[64];
  std::vector<char>::size_type idx =
      (bitLen.m_lo / 8) % 64;  // Low word for modulo
  bitLen.add(static_cast<unsigned long>(data.size()));

  unsigned char finalOctet = static_cast<unsigned char>(0x80);
  padded[idx++] = finalOctet;
  if (idx > 56) {
    std::memset(padded + idx, 0, 64 - idx);
    sha1Transform(state, padded);
    idx = 0;
  }
  std::memset(padded + idx, 0, 56 - idx);
  bitLen.toBytes(padded + 56);
  sha1Transform(state, padded);
}

std::string FileHandler::calculateSHA1(const std::vector<char>& data) const {
  unsigned int state[5] = {0x67452301, 0xEFCDAB89, 0x98BADCFE, 0x10325476,
                           0xC3D2E1F0};
  U64 bitLen;
  const unsigned char* ptr =
      reinterpret_cast<const unsigned char*>(data.data());
  std::vector<char>::size_type blocks = data.size() / 64;
  for (std::vector<char>::size_type i = 0; i < blocks; ++i) {
    sha1Transform(state, ptr + (i * 64));
    bitLen.add(64UL);
  }
  // Process remainder
  std::vector<char>::size_type rem = data.size() % 64;
  if (rem > 0) {
    sha1PadAndProcess(std::vector<char>(ptr + (blocks * 64), ptr + data.size()),
                      state, bitLen);
  } else {
    sha1PadAndProcess(std::vector<char>(), state, bitLen);
  }

  unsigned char digest[20];
  for (int i = 0; i < 5; ++i) {
    digest[i * 4] = static_cast<unsigned char>((state[i] >> 24) & 0xFF);
    digest[i * 4 + 1] = static_cast<unsigned char>((state[i] >> 16) & 0xFF);
    digest[i * 4 + 2] = static_cast<unsigned char>((state[i] >> 8) & 0xFF);
    digest[i * 4 + 3] = static_cast<unsigned char>(state[i] & 0xFF);
  }
  return toHexDigest(digest, 20);
}

void FileHandler::sha256Transform(unsigned int* state,
                                  const unsigned char* block) const {
  unsigned int w[64];
  for (int i = 0; i < 16; ++i) {
    w[i] = (static_cast<unsigned int>(block[i * 4]) << 24) |
           (static_cast<unsigned int>(block[i * 4 + 1]) << 16) |
           (static_cast<unsigned int>(block[i * 4 + 2]) << 8) |
           static_cast<unsigned int>(block[i * 4 + 3]);
  }
  for (int i = 16; i < 64; ++i) {
    unsigned int s0 = rightRotate(w[i - 15], 7) ^ rightRotate(w[i - 15], 18) ^
                      (w[i - 15] >> 3);
    unsigned int s1 = rightRotate(w[i - 2], 17) ^ rightRotate(w[i - 2], 19) ^
                      (w[i - 2] >> 10);
    w[i] = w[i - 16] + s0 + w[i - 7] + s1;
  }

  unsigned int a = state[0], b = state[1], c = state[2], d = state[3],
               e = state[4], f = state[5], g = state[6], h = state[7];
  const unsigned int k[64] = {
      0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1,
      0x923f82a4, 0xab1c5ed5, 0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
      0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174, 0xe49b69c1, 0xefbe4786,
      0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
      0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147,
      0x06ca6351, 0x14292967, 0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
      0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85, 0xa2bfe8a1, 0xa81a664b,
      0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
      0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a,
      0x5b9cca4f, 0x682e6ff3, 0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
      0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2};
  for (int i = 0; i < 64; ++i) {
    unsigned int s1 =
        rightRotate(e, 6) ^ rightRotate(e, 11) ^ rightRotate(e, 25);
    unsigned int ch = (e & f) ^ (~e & g);
    unsigned int temp1 = h + s1 + ch + k[i] + w[i];
    unsigned int s0 =
        rightRotate(a, 2) ^ rightRotate(a, 13) ^ rightRotate(a, 22);
    unsigned int maj = (a & b) | (a & c) | (b & c);
    unsigned int temp2 = s0 + maj;
    h = g;
    g = f;
    f = e;
    e = d + temp1;
    d = c;
    c = b;
    b = a;
    a = temp1 + temp2;
  }
  state[0] += a;
  state[1] += b;
  state[2] += c;
  state[3] += d;
  state[4] += e;
  state[5] += f;
  state[6] += g;
  state[7] += h;
}

void FileHandler::sha256PadAndProcess(const std::vector<char>& data,
                                      unsigned int* state, U64& bitLen) const {
  unsigned char padded[64];
  std::vector<char>::size_type idx = (bitLen.m_lo / 8) % 64;
  bitLen.add(static_cast<unsigned long>(data.size()));

  unsigned char finalOctet = static_cast<unsigned char>(0x80);
  padded[idx++] = finalOctet;
  if (idx > 56) {
    std::memset(padded + idx, 0, 64 - idx);
    sha256Transform(state, padded);
    idx = 0;
  }
  std::memset(padded + idx, 0, 56 - idx);
  bitLen.toBytes(padded + 56);
  sha256Transform(state, padded);
}

std::string FileHandler::calculateSHA256(const std::vector<char>& data) const {
  unsigned int state[8] = {0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
                           0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19};
  U64 bitLen;
  const unsigned char* ptr =
      reinterpret_cast<const unsigned char*>(data.data());
  std::vector<char>::size_type blocks = data.size() / 64;
  for (std::vector<char>::size_type i = 0; i < blocks; ++i) {
    sha256Transform(state, ptr + (i * 64));
    bitLen.add(64UL);
  }
  // Process remainder
  std::vector<char>::size_type rem = data.size() % 64;
  if (rem > 0) {
    sha256PadAndProcess(
        std::vector<char>(ptr + (blocks * 64), ptr + data.size()), state,
        bitLen);
  } else {
    sha256PadAndProcess(std::vector<char>(), state, bitLen);
  }

  unsigned char digest[32];
  for (int i = 0; i < 8; ++i) {
    digest[i * 4] = static_cast<unsigned char>((state[i] >> 24) & 0xFF);
    digest[i * 4 + 1] = static_cast<unsigned char>((state[i] >> 16) & 0xFF);
    digest[i * 4 + 2] = static_cast<unsigned char>((state[i] >> 8) & 0xFF);
    digest[i * 4 + 3] = static_cast<unsigned char>(state[i] & 0xFF);
  }
  return toHexDigest(digest, 32);
}

void FileHandler::md5Transform(unsigned int* state,
                               const unsigned char* block) const {
  unsigned int a = state[0], b = state[1], c = state[2], d = state[3];
  unsigned int x[16];
  for (int i = 0; i < 16; ++i) {
    x[i] = (static_cast<unsigned int>(block[i * 4]) << 24) |
           (static_cast<unsigned int>(block[i * 4 + 1]) << 16) |
           (static_cast<unsigned int>(block[i * 4 + 2]) << 8) |
           static_cast<unsigned int>(block[i * 4 + 3]);
  }

  const unsigned int s[64] = {
      7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22,
      5, 9,  14, 20, 5, 9,  14, 20, 5, 9,  14, 20, 5, 9,  14, 20,
      4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23,
      6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21};
  const unsigned int k[64] = {
      0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee, 0xf57c0faf, 0x4787c62a,
      0xa8304613, 0xfd469501, 0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be,
      0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821, 0xf61e2562, 0xc040b340,
      0x265e5a51, 0xe9b6c7aa, 0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
      0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed, 0xa9e3e905, 0xfcefa3f8,
      0x676f02d9, 0x8d2a4c8a, 0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c,
      0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70, 0x289b7ec6, 0xeaa127fa,
      0xd4ef3085, 0x04881d05, 0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
      0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039, 0x655b59c3, 0x8f0ccc92,
      0xffeff47d, 0x85845dd1, 0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1,
      0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391};
  const int r[64] = {0,  1,  2,  3, 4, 5,  6,  7,  8,  9,  10, 11, 12,
                     13, 14, 15, 1, 6, 11, 0,  5,  10, 15, 4,  9,  14,
                     3,  8,  13, 2, 7, 12, 5,  8,  11, 14, 1,  4,  7,
                     10, 13, 0,  3, 6, 9,  12, 15, 2,  0,  1,  2,  3,
                     4,  5,  6,  7, 8, 9,  10, 11, 12, 13, 14, 15};

  for (int i = 0; i < 64; ++i) {
    unsigned int f, g;
    if (i < 16) {
      f = (b & c) | (~b & d);
      g = r[i];
    } else if (i < 32) {
      f = (b & d) | (c & ~d);
      g = (3 * i + 5) % 16;
    } else if (i < 48) {
      f = b ^ c ^ d;
      g = (7 * i) % 16;
    } else {
      f = c ^ (b | ~d);
      g = (i * 7) % 16;
    }
    unsigned int temp = d;
    d = c;
    c = b;
    b = b + leftRotate(a + f + k[i] + x[g], s[i]);
    a = temp;
  }
  state[0] += a;
  state[1] += b;
  state[2] += c;
  state[3] += d;
}

void FileHandler::md5PadAndProcess(const std::vector<char>& data,
                                   unsigned int* state, U64& bitLen) const {
  unsigned char padded[64];
  std::vector<char>::size_type idx = (bitLen.m_lo / 8) % 64;
  bitLen.add(static_cast<unsigned long>(data.size()));

  unsigned char finalOctet = static_cast<unsigned char>(0x80);
  padded[idx++] = finalOctet;
  if (idx > 56) {
    std::memset(padded + idx, 0, 64 - idx);
    md5Transform(state, padded);
    idx = 0;
  }
  std::memset(padded + idx, 0, 56 - idx);
  bitLen.toBytes(padded + 56);
  md5Transform(state, padded);
}

std::string FileHandler::calculateMD5(const std::vector<char>& data) const {
  unsigned int state[4] = {0x67452301, 0xefcdab89, 0x98badcfe, 0x10325476};
  U64 bitLen;
  const unsigned char* ptr =
      reinterpret_cast<const unsigned char*>(data.data());
  std::vector<char>::size_type blocks = data.size() / 64;
  for (std::vector<char>::size_type i = 0; i < blocks; ++i) {
    md5Transform(state, ptr + (i * 64));
    bitLen.add(64UL);
  }

  std::vector<char>::size_type rem = data.size() % 64;
  if (rem > 0) {
    md5PadAndProcess(std::vector<char>(ptr + (blocks * 64), ptr + data.size()),
                     state, bitLen);
  } else {
    md5PadAndProcess(std::vector<char>(), state, bitLen);
  }

  unsigned char digest[16];
  for (int i = 0; i < 4; ++i) {
    digest[i * 4] = static_cast<unsigned char>((state[i] >> 24) & 0xFF);
    digest[i * 4 + 1] = static_cast<unsigned char>((state[i] >> 16) & 0xFF);
    digest[i * 4 + 2] = static_cast<unsigned char>((state[i] >> 8) & 0xFF);
    digest[i * 4 + 3] = static_cast<unsigned char>(state[i] & 0xFF);
  }
  return toHexDigest(digest, 16);
}

std::string FileHandler::generateTemporaryFilename(
    const std::string& prefix, const std::string& suffix,
    const domain::filesystem::value_objects::Path& directory) const {
  std::string dirStr = directory.toString();
  if (!FileSystemHelper::exists(dirStr) ||
      !FileSystemHelper::isDirectory(dirStr) ||
      !FileSystemHelper::isWritable(dirStr)) {
    throw exceptions::FileHandlerException(
        "Invalid temporary directory: " + dirStr,
        exceptions::FileHandlerException::
            TEMPORARY_FILE_CREATION_FAILED);
  }

  std::ostringstream oss;
  oss << dirStr;
  if (dirStr[dirStr.length() - 1] != '/') {
    oss << '/';
  }
  oss << prefix << "XXXXXX" << suffix;

  std::string templateStr = oss.str();
  char* temp = new char[templateStr.length() + 1];
  std::strcpy(temp, templateStr.c_str());

  int fd = mkstemps(temp, static_cast<int>(suffix.length()));
  if (fd < 0) {
    delete[] temp;
    throw exceptions::FileHandlerException(
        "Failed to create temporary file",
        exceptions::FileHandlerException::
            TEMPORARY_FILE_CREATION_FAILED);
  }

  close(fd);
  std::string result(temp);
  delete[] temp;

  return result;
}

bool FileHandler::validateChunkParameters(
    std::size_t offset, std::size_t chunkSize,
    const domain::filesystem::value_objects::Size& fileSize) const {
  if (chunkSize == 0) {
    return false;
  }

  if (offset >= fileSize.getBytes()) {
    return false;
  }

  return true;
}

bool FileHandler::isSupportedCompressionAlgorithm(
    const std::string& algorithm) {
  return algorithm == "gzip" || algorithm == "deflate";
}

bool FileHandler::isSupportedChecksumAlgorithm(const std::string& algorithm) {
  return algorithm == "md5" || algorithm == "sha1" || algorithm == "sha256";
}

std::string FileHandler::getFileExtension(const std::string& filename) const {
  std::size_t dotPos = filename.find_last_of('.');
  if (dotPos == std::string::npos || dotPos == 0) {
    return "";
  }

  std::string extension = filename.substr(dotPos + 1);
  for (std::size_t i = 0; i < extension.length(); ++i) {
    extension[i] =
        static_cast<char>(::tolower(static_cast<unsigned char>(extension[i])));
  }

  return extension;
}

std::string FileHandler::extensionToMimeType(
    const std::string& extension) const {
  if (extension == "html" || extension == "htm") return "text/html";
  if (extension == "css") return "text/css";
  if (extension == "js") return "application/javascript";
  if (extension == "json") return "application/json";
  if (extension == "xml") return "application/xml";
  if (extension == "txt") return "text/plain";
  if (extension == "pdf") return "application/pdf";
  if (extension == "jpg" || extension == "jpeg") return "image/jpeg";
  if (extension == "png") return "image/png";
  if (extension == "gif") return "image/gif";
  if (extension == "bmp") return "image/bmp";
  if (extension == "ico") return "image/x-icon";
  if (extension == "svg") return "image/svg+xml";
  if (extension == "zip") return "application/zip";
  if (extension == "tar") return "application/x-tar";
  if (extension == "gz") return "application/gzip";
  if (extension == "mp3") return "audio/mpeg";
  if (extension == "mp4") return "video/mp4";
  if (extension == "avi") return "video/x-msvideo";
  if (extension == "mov") return "video/quicktime";
  if (extension == "php") return "application/x-httpd-php";
  if (extension == "py") return "text/x-python";
  if (extension == "c") return "text/x-c";
  if (extension == "cpp" || extension == "cc" || extension == "cxx") {
    return "text/x-c++";
  }
  if (extension == "h" || extension == "hpp") return "text/x-c++hdr";

  return "application/octet-stream";
}

}  // namespace filesystem
}  // namespace infrastructure
}  // namespace infrastructure
