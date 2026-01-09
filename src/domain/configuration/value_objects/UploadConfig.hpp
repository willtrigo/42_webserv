/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   UploadConfig.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: umeneses <umeneses@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/22 13:27:36 by dande-je          #+#    #+#             */
/*   Updated: 2026/01/08 22:13:30 by umeneses         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UPLOADCONFIG_HPP
#define UPLOADCONFIG_HPP

#include "domain/filesystem/value_objects/Path.hpp"
#include "domain/filesystem/value_objects/Size.hpp"
#include "infrastructure/filesystem/adapters/DirectoryLister.hpp"
#include "infrastructure/filesystem/adapters/FileHandler.hpp"
#include "infrastructure/filesystem/adapters/PathResolver.hpp"

#include <map>
#include <string>
#include <vector>

namespace domain {
namespace configuration {
namespace value_objects {

struct UploadFileInfo {
  std::string originalFilename;
  std::string storedFilename;
  domain::filesystem::value_objects::Path filePath;
  domain::filesystem::value_objects::Size fileSize;
  std::string mimeType;
  std::string checksum;
  std::string uploadTime;
  std::string uploader;
  std::map<std::string, std::string> metadata;

  UploadFileInfo() : fileSize(0) {}
};

struct UploadStatistics {
  std::size_t totalFiles;
  std::size_t successfulUploads;
  std::size_t failedUploads;
  domain::filesystem::value_objects::Size totalSize;
  domain::filesystem::value_objects::Size diskSpaceUsed;
  domain::filesystem::value_objects::Size diskSpaceAvailable;

  UploadStatistics()
      : totalFiles(0),
        successfulUploads(0),
        failedUploads(0),
        totalSize(0),
        diskSpaceUsed(0),
        diskSpaceAvailable(0) {}
};

class UploadConfig {
 public:
  static const std::size_t DEFAULT_MAX_FILES_PER_UPLOAD = 10;
  static const std::size_t DEFAULT_MAX_FILENAME_LENGTH = 255;
  static const domain::filesystem::value_objects::Size DEFAULT_MAX_FILE_SIZE;
  static const domain::filesystem::value_objects::Size DEFAULT_MAX_TOTAL_SIZE;
  static const std::size_t DEFAULT_MAX_UPLOADS_PER_HOUR = 100;

  static const std::string DEFAULT_ALLOWED_EXTENSIONS[];
  static const std::size_t DEFAULT_ALLOWED_EXTENSIONS_COUNT;
  static const std::string DEFAULT_BLOCKED_EXTENSIONS[];
  static const std::size_t DEFAULT_BLOCKED_EXTENSIONS_COUNT;

  explicit UploadConfig(
      const domain::filesystem::value_objects::Path& uploadDirectory);
  UploadConfig(const domain::filesystem::value_objects::Path& uploadDirectory,
               const domain::filesystem::value_objects::Size& maxFileSize,
               const domain::filesystem::value_objects::Size& maxTotalSize);

  // Constructor for dependency injection (primarily for testing)
  UploadConfig(
      const domain::filesystem::value_objects::Path& uploadDirectory,
      infrastructure::filesystem::adapters::FileHandler* fileHandler,
      infrastructure::filesystem::adapters::DirectoryLister* directoryLister,
      infrastructure::filesystem::adapters::PathResolver* pathResolver);

  ~UploadConfig();

  UploadConfig(const UploadConfig& other);
  UploadConfig& operator=(const UploadConfig& other);

  // Configuration methods
  void setMaxFileSize(const domain::filesystem::value_objects::Size& maxSize);
  void setMaxTotalSize(const domain::filesystem::value_objects::Size& maxSize);
  void setMaxFilesPerUpload(std::size_t maxFiles);
  void setMaxFilenameLength(std::size_t maxLength);
  void setMaxUploadsPerHour(std::size_t maxUploads);

  void addAllowedExtension(const std::string& extension);
  void addBlockedExtension(const std::string& extension);
  void addAllowedMimeType(const std::string& mimeType);
  void addBlockedMimeType(const std::string& mimeType);

  void setOverwriteExisting(bool allowOverwrite);
  void setGenerateThumbnails(bool generate);
  void setApplyWatermark(bool apply);
  void setEncryptFiles(bool encrypt);
  void setCompressFiles(bool compress);

  void setUploadDirectory(
      const domain::filesystem::value_objects::Path& directory);
  void setPermissions(
      const domain::filesystem::value_objects::Permission& permissions);
  void setUploadAccess(const std::string& accessString);
  const domain::filesystem::value_objects::Permission& getPermissions() const;
  const std::string& getUploadAccess() const;

  // Validation methods
  bool validateUploadDirectory() const;
  bool validateFileSize(
      const domain::filesystem::value_objects::Size& fileSize) const;
  bool validateTotalSize(
      const domain::filesystem::value_objects::Size& totalSize) const;
  bool validateFilename(const std::string& filename) const;
  bool validateExtension(const std::string& filename) const;
  bool validateMimeType(const std::string& mimeType) const;

  // Upload operations
  UploadFileInfo processUpload(const std::string& originalFilename,
                               const std::vector<char>& fileData,
                               const std::string& uploader = "") const;

  std::vector<UploadFileInfo> processMultiUpload(
      const std::map<std::string, std::vector<char> >& files,
      const std::string& uploader = "") const;

  bool deleteUploadedFile(const std::string& storedFilename) const;
  bool renameUploadedFile(const std::string& oldFilename,
                          const std::string& newFilename) const;

  // File management
  std::vector<UploadFileInfo> listUploadedFiles(bool showHidden = false) const;
  UploadFileInfo getFileInfo(const std::string& storedFilename) const;
  std::vector<char> getFileContent(const std::string& storedFilename) const;

  // Statistics
  UploadStatistics getStatistics() const;
  std::map<std::string, UploadStatistics> getStatisticsByUploader() const;
  std::map<std::string, std::size_t> getFileTypeDistribution() const;

  // Cleanup
  bool cleanupOldFiles(int daysToKeep = 30) const;  // TODO:remove magic number
  bool cleanupOrphanedFiles() const;
  bool cleanupTemporaryFiles() const;

  // Security
  bool scanForViruses(
      const domain::filesystem::value_objects::Path& filePath) const;
  bool verifyFileIntegrity(const std::string& storedFilename,
                           const std::string& expectedChecksum) const;

  // Utility methods
  std::string generateUniqueFilename(const std::string& originalFilename) const;
  std::string sanitizeFilename(const std::string& filename) const;
  domain::filesystem::value_objects::Path getFullPath(
      const std::string& filename) const;

  // Getters
  domain::filesystem::value_objects::Path getUploadDirectory() const;
  domain::filesystem::value_objects::Size getMaxFileSize() const;
  domain::filesystem::value_objects::Size getMaxTotalSize() const;
  std::size_t getMaxFilesPerUpload() const;
  std::size_t getMaxFilenameLength() const;
  std::size_t getMaxUploadsPerHour() const;
  bool getOverwriteExisting() const;
  bool getGenerateThumbnails() const;
  bool getApplyWatermark() const;
  bool getEncryptFiles() const;
  bool getCompressFiles() const;

 private:
  domain::filesystem::value_objects::Path m_uploadDirectory;
  domain::filesystem::value_objects::Size m_maxFileSize;
  domain::filesystem::value_objects::Size m_maxTotalSize;
  std::size_t m_maxFilesPerUpload;
  std::size_t m_maxFilenameLength;
  std::size_t m_maxUploadsPerHour;

  bool m_overwriteExisting;
  bool m_generateThumbnails;
  bool m_applyWatermark;
  bool m_encryptFiles;
  bool m_compressFiles;

  std::vector<std::string> m_allowedExtensions;
  std::vector<std::string> m_blockedExtensions;
  std::vector<std::string> m_allowedMimeTypes;
  std::vector<std::string> m_blockedMimeTypes;

  // Dependencies (would be injected in a real implementation)
  mutable infrastructure::filesystem::adapters::FileHandler* m_fileHandler;
  mutable infrastructure::filesystem::adapters::DirectoryLister*
      m_directoryLister;
  mutable infrastructure::filesystem::adapters::PathResolver* m_pathResolver;

  domain::filesystem::value_objects::Permission m_permissions;
  std::string m_uploadAccess;

  // Helper methods
  void initializeDependencies() const;
  void cleanupDependencies() const;

  bool validateUploadConstraints(const std::string& uploader) const;
  void updateUploadStatistics(const std::string& uploader) const;

  UploadFileInfo createFileInfo(
      const std::string& originalFilename, const std::string& storedFilename,
      const domain::filesystem::value_objects::Size& fileSize,
      const std::string& mimeType, const std::string& uploader) const;

  std::string extractExtension(const std::string& filename) const;
  std::string getCurrentTimestamp() const;

  bool generateThumbnail(
      const domain::filesystem::value_objects::Path& thumbPath) const;
  bool applyWatermark(
      const domain::filesystem::value_objects::Path& sourcePath,
      const domain::filesystem::value_objects::Path& watermarkedPath) const;
  bool encryptFile(
      const domain::filesystem::value_objects::Path& sourcePath,
      const domain::filesystem::value_objects::Path& encryptedPath) const;
  bool compressFile(
      const domain::filesystem::value_objects::Path& sourcePath,
      const domain::filesystem::value_objects::Path& compressedPath) const;

  static bool isImageFile(const std::string& filename);
  static bool isTextFile(const std::string& filename);
  static bool isArchiveFile(const std::string& filename);

  // Rate limiting
  mutable std::map<std::string, std::vector<time_t> > m_uploadHistory;

  static const std::size_t MAX_THUMBNAIL_WIDTH = 200;
  static const std::size_t MAX_THUMBNAIL_HEIGHT = 200;
  static const std::string THUMBNAIL_SUFFIX;
  static const std::string WATERMARK_SUFFIX;
  static const std::string ENCRYPTED_SUFFIX;
  static const std::string COMPRESSED_SUFFIX;
};

}  // namespace value_objects
}  // namespace configuration
}  // namespace domain

#endif  // UPLOADCONFIG_HPP
