/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FileSystemHelper.hpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: umeneses <umeneses@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/22 14:09:30 by dande-je          #+#    #+#             */
/*   Updated: 2026/01/08 23:04:56 by umeneses         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FILE_SYSTEM_HELPER_HPP
#define FILE_SYSTEM_HELPER_HPP

#include <string>
#include <sys/stat.h>

namespace infrastructure {
namespace filesystem {
namespace adapters {

class FileSystemHelper {
 public:
  // Singleton instance for dependency injection
  // Returns a valid pointer that can be passed to adapters
  // The pointer itself is never dereferenced - only static methods are called
  static FileSystemHelper* getInstance();

  static bool exists(const std::string& path);
  static bool isDirectory(const std::string& path);
  static bool isFile(const std::string& path);
  static bool isSymbolicLink(const std::string& path);
  static bool isReadable(const std::string& path);
  static bool isWritable(const std::string& path);
  static bool isExecutable(const std::string& path);

  static std::size_t getFileSize(const std::string& path);

  static bool createDirectory(const std::string& path);
  static bool createDirectoryRecursive(const std::string& path);
  static bool removeDirectory(const std::string& path);
  static bool removeDirectoryRecursive(const std::string& path);

  static bool removeFile(const std::string& path);
  static bool renameFile(const std::string& oldPath,
                         const std::string& newPath);
  static bool copyFile(const std::string& source,
                       const std::string& destination);
  static void validateCopyFilePaths(const std::string& source,
                                    const std::string& destination);
  static void validateSourceIsFile(const std::string& source);
  static FILE* openSourceFileForCopying(const std::string& source);
  static FILE* openDestinationFileForCopying(const std::string& destination,
                                             FILE* sourceHandle);

  static bool setPermissions(const std::string& path, mode_t mode);
  static mode_t getPermissions(const std::string& path);

  static bool changeOwner(const std::string& path, uid_t owner, gid_t group);

  static time_t getLastModifiedTime(const std::string& path);
  static bool setLastModifiedTime(const std::string& path, time_t time);

  static std::string getCurrentWorkingDirectory();
  static bool changeWorkingDirectory(const std::string& path);
  static std::string getAbsolutePath(const std::string& path);
  static std::string resolveSymbolicLink(const std::string& path);

 private:
  FileSystemHelper();
  FileSystemHelper(const FileSystemHelper&);
  ~FileSystemHelper();

  FileSystemHelper& operator=(const FileSystemHelper&);

  static bool isPathEmpty(const std::string& path);
  static std::string getParentDirectory(const std::string& path);
  static bool isRootDirectory(const std::string& path);
};

}  // namespace adapters
}  // namespace filesystem
}  // namespace infrastructure

#endif  // FILE_SYSTEM_HELPER_HPP
