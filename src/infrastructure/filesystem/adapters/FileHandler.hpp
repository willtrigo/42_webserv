/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FileHandler.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/23 13:00:04 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/27 23:43:32 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FILEHANDLER_HPP
#define FILEHANDLER_HPP

#include "domain/filesystem/value_objects/Path.hpp"
#include "domain/filesystem/value_objects/Permission.hpp"
#include "domain/filesystem/value_objects/Size.hpp"
#include "infrastructure/filesystem/adapters/FileSystemHelper.hpp"
#include "infrastructure/filesystem/adapters/PathResolver.hpp"

#include <map>
#include <string>
#include <vector>

namespace infrastructure {
namespace filesystem {
namespace adapters {

struct FileMetadata {
  domain::filesystem::value_objects::Path path;
  domain::filesystem::value_objects::Size size;
  domain::filesystem::value_objects::Permission permissions;
  std::string lastModified;
  std::string mimeType;
  std::string checksum;
  bool isDirectory;
  bool isSymbolicLink;
  bool isHidden;

  FileMetadata() : isDirectory(false), isSymbolicLink(false), isHidden(false) {}
};

class FileHandler {
 public:
  static const std::size_t DEFAULT_CHUNK_SIZE = 8192;
  static const std::size_t MAX_CHUNK_SIZE = 1048576;
  static const std::size_t MIN_CHUNK_SIZE = 1024;

  explicit FileHandler(FileSystemHelper* fileSystemHelper,
                       PathResolver* pathResolver);
  virtual ~FileHandler();

  FileHandler(const FileHandler& other);
  FileHandler& operator=(const FileHandler& other);

  FileMetadata getMetadata(
      const domain::filesystem::value_objects::Path& filePath) const;

  std::vector<char> readFile(
      const domain::filesystem::value_objects::Path& filePath) const;
  std::vector<char> readFileChunk(
      const domain::filesystem::value_objects::Path& filePath,
      std::size_t offset = 0, std::size_t chunkSize = DEFAULT_CHUNK_SIZE) const;

  bool writeFile(const domain::filesystem::value_objects::Path& filePath,
                 const std::vector<char>& data, bool overwrite = true) const;
  bool writeFile(const domain::filesystem::value_objects::Path& filePath,
                 const std::string& data, bool overwrite = true) const;

  bool appendToFile(const domain::filesystem::value_objects::Path& filePath,
                    const std::vector<char>& data) const;
  bool appendToFile(const domain::filesystem::value_objects::Path& filePath,
                    const std::string& data) const;

  bool createFile(
      const domain::filesystem::value_objects::Path& filePath,
      const domain::filesystem::value_objects::Permission& permissions =
          domain::filesystem::value_objects::Permission::readWrite()) const;

  bool deleteFile(
      const domain::filesystem::value_objects::Path& filePath) const;
  bool renameFile(const domain::filesystem::value_objects::Path& oldPath,
                  const domain::filesystem::value_objects::Path& newPath) const;
  bool copyFile(const domain::filesystem::value_objects::Path& sourcePath,
                const domain::filesystem::value_objects::Path& destinationPath,
                bool overwrite = true) const;

  bool createDirectory(
      const domain::filesystem::value_objects::Path& dirPath,
      const domain::filesystem::value_objects::Permission& permissions =
          domain::filesystem::value_objects::Permission::
              ownerAllGroupReadExecuteOtherReadExecute()) const;
  bool deleteDirectory(const domain::filesystem::value_objects::Path& dirPath,
                       bool recursive = false) const;

  domain::filesystem::value_objects::Path createTemporaryFile(
      const std::string& prefix = "tmp_", const std::string& suffix = "",
      const domain::filesystem::value_objects::Path& directory =
          domain::filesystem::value_objects::Path("/tmp", true)) const;

  domain::filesystem::value_objects::Path createTemporaryDirectory(
      const std::string& prefix = "tmp_",
      const domain::filesystem::value_objects::Path& directory =
          domain::filesystem::value_objects::Path("/tmp", true)) const;

  bool lockFile(const domain::filesystem::value_objects::Path& filePath) const;
  bool unlockFile(
      const domain::filesystem::value_objects::Path& filePath) const;
  bool isFileLocked(
      const domain::filesystem::value_objects::Path& filePath) const;

  std::string calculateChecksum(
      const domain::filesystem::value_objects::Path& filePath,
      const std::string& algorithm = "md5") const;

  bool verifyChecksum(const domain::filesystem::value_objects::Path& filePath,
                      const std::string& expectedChecksum,
                      const std::string& algorithm = "md5") const;

  std::vector<char> compressData(const std::vector<char>& data,
                                 const std::string& algorithm = "gzip") const;
  std::vector<char> decompressData(const std::vector<char>& compressedData,
                                   const std::string& algorithm = "gzip") const;

  bool compressFile(const domain::filesystem::value_objects::Path& sourcePath,
                    const domain::filesystem::value_objects::Path& destPath,
                    const std::string& algorithm = "gzip") const;
  bool decompressFile(const domain::filesystem::value_objects::Path& sourcePath,
                      const domain::filesystem::value_objects::Path& destPath,
                      const std::string& algorithm = "gzip") const;

  domain::filesystem::value_objects::Size getAvailableDiskSpace(
      const domain::filesystem::value_objects::Path& path) const;

  bool validateFileSize(
      const domain::filesystem::value_objects::Path& filePath,
      const domain::filesystem::value_objects::Size& maxSize) const;

  std::string detectMimeType(
      const domain::filesystem::value_objects::Path& filePath) const;

  bool setPermissions(
      const domain::filesystem::value_objects::Path& filePath,
      const domain::filesystem::value_objects::Permission& permissions) const;

  bool setOwner(const domain::filesystem::value_objects::Path& filePath,
                const std::string& owner) const;
  bool setGroup(const domain::filesystem::value_objects::Path& filePath,
                const std::string& group) const;

  bool isSafeFilename(const std::string& filename) const;
  std::string sanitizeFilename(const std::string& filename) const;

  bool backupFile(const domain::filesystem::value_objects::Path& filePath,
                  const domain::filesystem::value_objects::Path& backupDir,
                  int maxBackups = 5) const;

 private:
  struct U64 {
    unsigned int m_hi;
    unsigned int m_lo;

    U64() : m_hi(0), m_lo(0) {}

    void add(unsigned long bytes) {
      unsigned long bits = bytes * 8;
      unsigned long add_lo = static_cast<unsigned long>(m_lo) + bits;
      m_lo = static_cast<unsigned int>(add_lo & 0xFFFFFFFFUL);
      unsigned long carry = add_lo >> 32;
      m_hi += static_cast<unsigned int>(carry);
    }

    void toBytes(unsigned char* buf) const {
      buf[0] = static_cast<unsigned char>((m_hi >> 24) & 0xFF);
      buf[1] = static_cast<unsigned char>((m_hi >> 16) & 0xFF);
      buf[2] = static_cast<unsigned char>((m_hi >> 8) & 0xFF);
      buf[3] = static_cast<unsigned char>(m_hi & 0xFF);
      buf[4] = static_cast<unsigned char>((m_lo >> 24) & 0xFF);
      buf[5] = static_cast<unsigned char>((m_lo >> 16) & 0xFF);
      buf[6] = static_cast<unsigned char>((m_lo >> 8) & 0xFF);
      buf[7] = static_cast<unsigned char>(m_lo & 0xFF);
    }
  }; // TODO: remove magic numbers

  FileSystemHelper* m_fileSystemHelper;
  PathResolver* m_pathResolver;

  static const std::size_t BUFFER_SIZE = 4096;
  static const std::size_t MAX_TEMPORARY_FILES = 1000;
  static const std::size_t MAX_FILENAME_LENGTH = 255;

  void validateFileForReading(
      const domain::filesystem::value_objects::Path& filePath) const;
  void validateFileForWriting(
      const domain::filesystem::value_objects::Path& filePath,
      bool overwrite) const;
  void validateDirectoryForCreation(
      const domain::filesystem::value_objects::Path& dirPath) const;
  void validateDirectoryForDeletion(
      const domain::filesystem::value_objects::Path& dirPath,
      bool recursive) const;

  bool copyFileInternal(
      const domain::filesystem::value_objects::Path& sourcePath,
      const domain::filesystem::value_objects::Path& destinationPath,
      bool overwrite) const;
  bool deleteDirectoryRecursive(
      const domain::filesystem::value_objects::Path& dirPath) const;

  std::string calculateMD5(const std::vector<char>& data) const;
  std::string calculateSHA1(const std::vector<char>& data) const;
  std::string calculateSHA256(const std::vector<char>& data) const;
  void sha1Transform(unsigned int* state, const unsigned char* block) const;
  void sha1PadAndProcess(const std::vector<char>& data, unsigned int* state,
                         U64& bitLen) const;
  void sha256Transform(unsigned int* state, const unsigned char* block) const;
  void sha256PadAndProcess(const std::vector<char>& data, unsigned int* state,
                           U64& bitLen) const;
  void md5Transform(unsigned int* state, const unsigned char* block) const;
  void md5PadAndProcess(const std::vector<char>& data, unsigned int* state,
                        U64& bitLen) const;
  unsigned int leftRotate(unsigned int value, unsigned int bits) const;
  unsigned int rightRotate(unsigned int value, unsigned int bits) const;
  std::string toHexDigest(const unsigned char* digest,
                          unsigned int length) const;

  std::string generateTemporaryFilename(
      const std::string& prefix, const std::string& suffix,
      const domain::filesystem::value_objects::Path& directory) const;

  bool validateChunkParameters(
      std::size_t offset, std::size_t chunkSize,
      const domain::filesystem::value_objects::Size& fileSize) const;

  static bool isSupportedCompressionAlgorithm(const std::string& algorithm);
  static bool isSupportedChecksumAlgorithm(const std::string& algorithm);

  std::string getFileExtension(const std::string& filename) const;
  std::string extensionToMimeType(const std::string& extension) const;

  struct LockInfo {
    int fd;
    std::string lockPath;

    LockInfo() : fd(-1) {}
  };

  mutable std::map<std::string, LockInfo> m_fileLocks;
};

}  // namespace adapters
}  // namespace filesystem
}  // namespace infrastructure

#endif  // FILEHANDLER_HPP
