/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   UploadConfig.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/22 13:30:43 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/23 21:14:17 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "domain/entities/UploadConfig.hpp"
#include "shared/exceptions/UploadConfigException.hpp"

#include <algorithm>
#include <cstring>
#include <ctime>
#include <sstream>

namespace domain {
namespace entities {

const domain::value_objects::Size UploadConfig::DEFAULT_MAX_FILE_SIZE =
    domain::value_objects::Size(10485760);  // 10 MB
const domain::value_objects::Size UploadConfig::DEFAULT_MAX_TOTAL_SIZE =
    domain::value_objects::Size(104857600);  // 100 MB

const std::string UploadConfig::DEFAULT_ALLOWED_EXTENSIONS[] = {
    "jpg",  "jpeg", "png",  "gif", "pdf",  "txt", "doc",
    "docx", "xls",  "xlsx", "ppt", "pptx", "zip", "rar"};
const std::size_t UploadConfig::DEFAULT_ALLOWED_EXTENSIONS_COUNT = 14;

const std::string UploadConfig::DEFAULT_BLOCKED_EXTENSIONS[] = {
    "exe", "dll",  "bat", "cmd", "sh",  "php",
    "js",  "html", "htm", "jar", "war", "ear"};
const std::size_t UploadConfig::DEFAULT_BLOCKED_EXTENSIONS_COUNT = 12;

const std::string UploadConfig::THUMBNAIL_SUFFIX = "_thumb";
const std::string UploadConfig::WATERMARK_SUFFIX = "_watermarked";
const std::string UploadConfig::ENCRYPTED_SUFFIX = "_encrypted";
const std::string UploadConfig::COMPRESSED_SUFFIX = "_compressed";

UploadConfig::UploadConfig(const domain::value_objects::Path& uploadDirectory)
    : m_uploadDirectory(uploadDirectory),
      m_maxFileSize(DEFAULT_MAX_FILE_SIZE),
      m_maxTotalSize(DEFAULT_MAX_TOTAL_SIZE),
      m_maxFilesPerUpload(DEFAULT_MAX_FILES_PER_UPLOAD),
      m_maxFilenameLength(DEFAULT_MAX_FILENAME_LENGTH),
      m_maxUploadsPerHour(DEFAULT_MAX_UPLOADS_PER_HOUR),
      m_overwriteExisting(false),
      m_generateThumbnails(false),
      m_applyWatermark(false),
      m_encryptFiles(false),
      m_compressFiles(false),
      m_fileHandler(NULL),
      m_directoryLister(NULL),
      m_pathResolver(NULL) {
  for (std::size_t i = 0; i < DEFAULT_ALLOWED_EXTENSIONS_COUNT; ++i) {
    m_allowedExtensions.push_back(DEFAULT_ALLOWED_EXTENSIONS[i]);
  }

  for (std::size_t i = 0; i < DEFAULT_BLOCKED_EXTENSIONS_COUNT; ++i) {
    m_blockedExtensions.push_back(DEFAULT_BLOCKED_EXTENSIONS[i]);
  }
}

UploadConfig::UploadConfig(const domain::value_objects::Path& uploadDirectory,
                           const domain::value_objects::Size& maxFileSize,
                           const domain::value_objects::Size& maxTotalSize)
    : m_uploadDirectory(uploadDirectory),
      m_maxFileSize(maxFileSize),
      m_maxTotalSize(maxTotalSize),
      m_maxFilesPerUpload(DEFAULT_MAX_FILES_PER_UPLOAD),
      m_maxFilenameLength(DEFAULT_MAX_FILENAME_LENGTH),
      m_maxUploadsPerHour(DEFAULT_MAX_UPLOADS_PER_HOUR),
      m_overwriteExisting(false),
      m_generateThumbnails(false),
      m_applyWatermark(false),
      m_encryptFiles(false),
      m_compressFiles(false),
      m_fileHandler(NULL),
      m_directoryLister(NULL),
      m_pathResolver(NULL) {
  for (std::size_t i = 0; i < DEFAULT_ALLOWED_EXTENSIONS_COUNT; ++i) {
    m_allowedExtensions.push_back(DEFAULT_ALLOWED_EXTENSIONS[i]);
  }

  for (std::size_t i = 0; i < DEFAULT_BLOCKED_EXTENSIONS_COUNT; ++i) {
    m_blockedExtensions.push_back(DEFAULT_BLOCKED_EXTENSIONS[i]);
  }
}

UploadConfig::~UploadConfig() { cleanupDependencies(); }

UploadConfig::UploadConfig(const UploadConfig& other)
    : m_uploadDirectory(other.m_uploadDirectory),
      m_maxFileSize(other.m_maxFileSize),
      m_maxTotalSize(other.m_maxTotalSize),
      m_maxFilesPerUpload(other.m_maxFilesPerUpload),
      m_maxFilenameLength(other.m_maxFilenameLength),
      m_maxUploadsPerHour(other.m_maxUploadsPerHour),
      m_overwriteExisting(other.m_overwriteExisting),
      m_generateThumbnails(other.m_generateThumbnails),
      m_applyWatermark(other.m_applyWatermark),
      m_encryptFiles(other.m_encryptFiles),
      m_compressFiles(other.m_compressFiles),
      m_allowedExtensions(other.m_allowedExtensions),
      m_blockedExtensions(other.m_blockedExtensions),
      m_allowedMimeTypes(other.m_allowedMimeTypes),
      m_blockedMimeTypes(other.m_blockedMimeTypes),
      m_fileHandler(NULL),
      m_directoryLister(NULL),
      m_pathResolver(NULL),
      m_uploadHistory(other.m_uploadHistory) {}

UploadConfig& UploadConfig::operator=(const UploadConfig& other) {
  if (this != &other) {
    cleanupDependencies();

    m_uploadDirectory = other.m_uploadDirectory;
    m_maxFileSize = other.m_maxFileSize;
    m_maxTotalSize = other.m_maxTotalSize;
    m_maxFilesPerUpload = other.m_maxFilesPerUpload;
    m_maxFilenameLength = other.m_maxFilenameLength;
    m_maxUploadsPerHour = other.m_maxUploadsPerHour;
    m_overwriteExisting = other.m_overwriteExisting;
    m_generateThumbnails = other.m_generateThumbnails;
    m_applyWatermark = other.m_applyWatermark;
    m_encryptFiles = other.m_encryptFiles;
    m_compressFiles = other.m_compressFiles;
    m_allowedExtensions = other.m_allowedExtensions;
    m_blockedExtensions = other.m_blockedExtensions;
    m_allowedMimeTypes = other.m_allowedMimeTypes;
    m_blockedMimeTypes = other.m_blockedMimeTypes;
    m_uploadHistory = other.m_uploadHistory;

    m_fileHandler = NULL;
    m_directoryLister = NULL;
    m_pathResolver = NULL;
  }
  return *this;
}

void UploadConfig::setMaxFileSize(const domain::value_objects::Size& maxSize) {
  m_maxFileSize = maxSize;
}

void UploadConfig::setMaxTotalSize(const domain::value_objects::Size& maxSize) {
  m_maxTotalSize = maxSize;
}

void UploadConfig::setMaxFilesPerUpload(std::size_t maxFiles) {
  m_maxFilesPerUpload = maxFiles;
}

void UploadConfig::setMaxFilenameLength(std::size_t maxLength) {
  m_maxFilenameLength = maxLength;
}

void UploadConfig::setMaxUploadsPerHour(std::size_t maxUploads) {
  m_maxUploadsPerHour = maxUploads;
}

void UploadConfig::addAllowedExtension(const std::string& extension) {
  std::string ext = extension;
  if (!ext.empty() && ext[0] == '.') {
    ext = ext.substr(1);
  }
  std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

  if (std::find(m_blockedExtensions.begin(), m_blockedExtensions.end(), ext) ==
      m_blockedExtensions.end()) {
    if (std::find(m_allowedExtensions.begin(), m_allowedExtensions.end(),
                  ext) == m_allowedExtensions.end()) {
      m_allowedExtensions.push_back(ext);
    }
  }
}

void UploadConfig::addBlockedExtension(const std::string& extension) {
  std::string ext = extension;
  if (!ext.empty() && ext[0] == '.') {
    ext = ext.substr(1);
  }
  std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

  std::vector<std::string>::iterator iter =
      std::find(m_allowedExtensions.begin(), m_allowedExtensions.end(), ext);
  if (iter != m_allowedExtensions.end()) {
    m_allowedExtensions.erase(iter);
  }

  if (std::find(m_blockedExtensions.begin(), m_blockedExtensions.end(), ext) ==
      m_blockedExtensions.end()) {
    m_blockedExtensions.push_back(ext);
  }
}

void UploadConfig::addAllowedMimeType(const std::string& mimeType) {
  if (std::find(m_allowedMimeTypes.begin(), m_allowedMimeTypes.end(),
                mimeType) == m_allowedMimeTypes.end()) {
    m_allowedMimeTypes.push_back(mimeType);
  }
}

void UploadConfig::addBlockedMimeType(const std::string& mimeType) {
  std::vector<std::string>::iterator iter =
      std::find(m_allowedMimeTypes.begin(), m_allowedMimeTypes.end(), mimeType);
  if (iter != m_allowedMimeTypes.end()) {
    m_allowedMimeTypes.erase(iter);
  }

  if (std::find(m_blockedMimeTypes.begin(), m_blockedMimeTypes.end(),
                mimeType) == m_blockedMimeTypes.end()) {
    m_blockedMimeTypes.push_back(mimeType);
  }
}

void UploadConfig::setOverwriteExisting(bool allowOverwrite) {
  m_overwriteExisting = allowOverwrite;
}

void UploadConfig::setGenerateThumbnails(bool generate) {
  m_generateThumbnails = generate;
}

void UploadConfig::setApplyWatermark(bool apply) { m_applyWatermark = apply; }

void UploadConfig::setEncryptFiles(bool encrypt) { m_encryptFiles = encrypt; }

void UploadConfig::setCompressFiles(bool compress) {
  m_compressFiles = compress;
}

bool UploadConfig::validateUploadDirectory() const {
  try {
    initializeDependencies();

    infrastructure::filesystem::FileMetadata metadata =
        m_fileHandler->getMetadata(m_uploadDirectory);
    if (!metadata.isDirectory) {
      return false;
    }

    domain::value_objects::Path testPath =
        domain::value_objects::Path(m_uploadDirectory.toString() +
                                    "/.upload_test_" + getCurrentTimestamp());

    std::vector<char> testData;
    testData.push_back('t');
    testData.push_back('e');
    testData.push_back('s');
    testData.push_back('t');

    bool writeSuccess = m_fileHandler->writeFile(testPath, testData, true);
    if (writeSuccess) {
      m_fileHandler->deleteFile(testPath);
      return true;
    }

    return false;
  } catch (...) {
    return false;
  }
}

bool UploadConfig::validateFileSize(
    const domain::value_objects::Size& fileSize) const {
  return fileSize <= m_maxFileSize;
}

bool UploadConfig::validateTotalSize(
    const domain::value_objects::Size& totalSize) const {
  return totalSize <= m_maxTotalSize;
}

bool UploadConfig::validateFilename(const std::string& filename) const {
  if (filename.empty()) {
    return false;
  }

  if (filename.length() > m_maxFilenameLength) {
    return false;
  }

  const char* invalidChars = "/\\?%*:|\"<>";
  for (std::size_t i = 0; i < filename.length(); ++i) {
    for (std::size_t j = 0; j < std::strlen(invalidChars); ++j) {
      if (filename[i] == invalidChars[j]) {
        return false;
      }
    }
    if (filename[i] < ' ') {
      return false;
    }
  }

  const char* reservedNames[] = {"CON",  "PRN",  "AUX",  "NUL",  "COM1", "COM2",
                                 "COM3", "COM4", "COM5", "COM6", "COM7", "COM8",
                                 "COM9", "LPT1", "LPT2", "LPT3", "LPT4", "LPT5",
                                 "LPT6", "LPT7", "LPT8", "LPT9"};

  std::string upperName = filename;
  std::transform(upperName.begin(), upperName.end(), upperName.begin(),
                 ::toupper);

  for (std::size_t i = 0; i < sizeof(reservedNames) / sizeof(reservedNames[0]);
       ++i) {
    if (upperName == reservedNames[i] ||
        (upperName.length() > strlen(reservedNames[i]) &&
         upperName.substr(0, strlen(reservedNames[i])) == reservedNames[i] &&
         upperName[strlen(reservedNames[i])] == '.')) {
      return false;
    }
  }

  return true;
}

bool UploadConfig::validateExtension(const std::string& filename) const {
  std::string ext = extractExtension(filename);
  std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

  for (std::size_t i = 0; i < m_blockedExtensions.size(); ++i) {
    if (m_blockedExtensions[i] == ext) {
      return false;
    }
  }

  if (m_allowedExtensions.empty()) {
    return true;
  }

  for (std::size_t i = 0; i < m_allowedExtensions.size(); ++i) {
    if (m_allowedExtensions[i] == ext) {
      return true;
    }
  }

  return false;
}

bool UploadConfig::validateMimeType(const std::string& mimeType) const {
  for (std::size_t i = 0; i < m_blockedMimeTypes.size(); ++i) {
    if (m_blockedMimeTypes[i] == mimeType) {
      return false;
    }
  }

  if (m_allowedMimeTypes.empty()) {
    return true;
  }

  for (std::size_t i = 0; i < m_allowedMimeTypes.size(); ++i) {
    if (m_allowedMimeTypes[i] == mimeType) {
      return true;
    }
  }

  return false;
}

UploadFileInfo UploadConfig::processUpload(const std::string& originalFilename,
                                           const std::vector<char>& fileData,
                                           const std::string& uploader) const {
  try {
    initializeDependencies();

    if (!validateUploadConstraints(uploader)) {
      throw shared::exceptions::UploadConfigException(
          "Upload limit reached for uploader: " + uploader,
          shared::exceptions::UploadConfigException::UPLOAD_LIMIT_REACHED);
    }

    if (!validateFilename(originalFilename)) {
      throw shared::exceptions::UploadConfigException(
          "Invalid filename: " + originalFilename,
          shared::exceptions::UploadConfigException::
              INVALID_FILENAME_CHARACTERS);
    }

    domain::value_objects::Size fileSize(fileData.size());
    if (!validateFileSize(fileSize)) {
      throw shared::exceptions::UploadConfigException(
          "File size exceeds maximum allowed",
          shared::exceptions::UploadConfigException::MAX_FILE_SIZE_EXCEEDED);
    }

    std::string storedFilename = generateUniqueFilename(originalFilename);
    domain::value_objects::Path filePath = getFullPath(storedFilename);

    try {
      infrastructure::filesystem::FileMetadata existingMetadata =
          m_fileHandler->getMetadata(filePath);
      if (!m_overwriteExisting) {
        throw shared::exceptions::UploadConfigException(
            "File already exists: " + storedFilename,
            shared::exceptions::UploadConfigException::DUPLICATE_FILENAME);
      }
    } catch (...) {
      // File doesn't exist, which is fine
    }

    std::string mimeType = m_fileHandler->detectMimeType(filePath);
    if (mimeType.empty()) {
      mimeType = "application/octet-stream";
    }

    if (!validateMimeType(mimeType)) {
      throw shared::exceptions::UploadConfigException(
          "MIME type not allowed: " + mimeType,
          shared::exceptions::UploadConfigException::INVALID_MIME_TYPE);
    }

    if (!m_fileHandler->writeFile(filePath, fileData, m_overwriteExisting)) {
      throw shared::exceptions::UploadConfigException(
          "Failed to write file: " + storedFilename,
          shared::exceptions::UploadConfigException::PERMANENT_STORAGE_ERROR);
    }

    std::string checksum = m_fileHandler->calculateChecksum(filePath, "md5");

    if (m_generateThumbnails && isImageFile(storedFilename)) {
      domain::value_objects::Path thumbPath =
          domain::value_objects::Path(filePath.toString() + THUMBNAIL_SUFFIX);
      if (!generateThumbnail(thumbPath)) {
        // if (!generateThumbnail(filePath, thumbPath)) { remove filePath to
        // bypass the error Log warning but don't fail the upload
      }
    }

    if (m_applyWatermark && isImageFile(storedFilename)) {
      domain::value_objects::Path watermarkedPath =
          domain::value_objects::Path(filePath.toString() + WATERMARK_SUFFIX);
      if (!applyWatermark(filePath, watermarkedPath)) {
        // Log warning but don't fail the upload
      }
    }

    if (m_encryptFiles) {
      domain::value_objects::Path encryptedPath =
          domain::value_objects::Path(filePath.toString() + ENCRYPTED_SUFFIX);
      if (!encryptFile(filePath, encryptedPath)) {
        // Log warning but don't fail the upload
      }
    }

    if (m_compressFiles) {
      domain::value_objects::Path compressedPath =
          domain::value_objects::Path(filePath.toString() + COMPRESSED_SUFFIX);
      if (!compressFile(filePath, compressedPath)) {
        // Log warning but don't fail the upload
      }
    }

    updateUploadStatistics(uploader);
    // updateUploadStatistics(uploader, fileSize); remove filesize to bypass the
    // error

    return createFileInfo(originalFilename, storedFilename, fileSize, mimeType,
                          uploader);

  } catch (const shared::exceptions::UploadConfigException&) {
    throw;
  } catch (const std::exception& e) {
    throw shared::exceptions::UploadConfigException(
        std::string("Upload processing failed: ") + e.what(),
        shared::exceptions::UploadConfigException::FILE_VALIDATION_FAILED);
  } catch (...) {
    throw shared::exceptions::UploadConfigException(
        "Unknown error during upload processing",
        shared::exceptions::UploadConfigException::FILE_VALIDATION_FAILED);
  }
}

std::vector<UploadFileInfo> UploadConfig::processMultiUpload(
    const std::map<std::string, std::vector<char> >& files,
    const std::string& uploader) const {
  std::vector<UploadFileInfo> results;

  if (files.size() > m_maxFilesPerUpload) {
    throw shared::exceptions::UploadConfigException(
        "Too many files in upload",
        shared::exceptions::UploadConfigException::UPLOAD_LIMIT_REACHED);
  }

  domain::value_objects::Size totalSize(0);
  for (std::map<std::string, std::vector<char> >::const_iterator it =
           files.begin();
       it != files.end(); ++it) {
    totalSize = totalSize + domain::value_objects::Size(it->second.size());
  }

  if (!validateTotalSize(totalSize)) {
    throw shared::exceptions::UploadConfigException(
        "Total upload size exceeds maximum allowed",
        shared::exceptions::UploadConfigException::MAX_TOTAL_SIZE_EXCEEDED);
  }

  for (std::map<std::string, std::vector<char> >::const_iterator it =
           files.begin();
       it != files.end(); ++it) {
    try {
      UploadFileInfo info = processUpload(it->first, it->second, uploader);
      results.push_back(info);
    } catch (const shared::exceptions::UploadConfigException& e) {
      // Log the error but continue with other files
      // In production, you might want to collect all errors
      throw;  // Re-throw for now
    }
  }

  return results;
}

bool UploadConfig::deleteUploadedFile(const std::string& storedFilename) const {
  try {
    initializeDependencies();

    domain::value_objects::Path filePath = getFullPath(storedFilename);

    if (!m_fileHandler->deleteFile(filePath)) {
      return false;
    }

    if (m_generateThumbnails && isImageFile(storedFilename)) {
      domain::value_objects::Path thumbPath =
          domain::value_objects::Path(filePath.toString() + THUMBNAIL_SUFFIX);
      m_fileHandler->deleteFile(thumbPath);
    }

    if (m_applyWatermark && isImageFile(storedFilename)) {
      domain::value_objects::Path watermarkedPath =
          domain::value_objects::Path(filePath.toString() + WATERMARK_SUFFIX);
      m_fileHandler->deleteFile(watermarkedPath);
    }

    if (m_encryptFiles) {
      domain::value_objects::Path encryptedPath =
          domain::value_objects::Path(filePath.toString() + ENCRYPTED_SUFFIX);
      m_fileHandler->deleteFile(encryptedPath);
    }

    if (m_compressFiles) {
      domain::value_objects::Path compressedPath =
          domain::value_objects::Path(filePath.toString() + COMPRESSED_SUFFIX);
      m_fileHandler->deleteFile(compressedPath);
    }

    return true;

  } catch (...) {
    return false;
  }
}

bool UploadConfig::renameUploadedFile(const std::string& oldFilename,
                                      const std::string& newFilename) const {
  try {
    initializeDependencies();

    if (!validateFilename(newFilename)) {
      return false;
    }

    domain::value_objects::Path oldPath = getFullPath(oldFilename);
    domain::value_objects::Path newPath = getFullPath(newFilename);

    try {
      infrastructure::filesystem::FileMetadata existingMetadata =
          m_fileHandler->getMetadata(newPath);
      if (!m_overwriteExisting) {
        return false;
      }
    } catch (...) {
      // New file doesn't exist, which is fine
    }

    if (!m_fileHandler->renameFile(oldPath, newPath)) {
      return false;
    }

    if (m_generateThumbnails && isImageFile(oldFilename)) {
      domain::value_objects::Path oldThumbPath =
          domain::value_objects::Path(oldPath.toString() + THUMBNAIL_SUFFIX);
      domain::value_objects::Path newThumbPath =
          domain::value_objects::Path(newPath.toString() + THUMBNAIL_SUFFIX);
      m_fileHandler->renameFile(oldThumbPath, newThumbPath);
    }

    if (m_applyWatermark && isImageFile(oldFilename)) {
      domain::value_objects::Path oldWatermarkedPath =
          domain::value_objects::Path(oldPath.toString() + WATERMARK_SUFFIX);
      domain::value_objects::Path newWatermarkedPath =
          domain::value_objects::Path(newPath.toString() + WATERMARK_SUFFIX);
      m_fileHandler->renameFile(oldWatermarkedPath, newWatermarkedPath);
    }

    if (m_encryptFiles) {
      domain::value_objects::Path oldEncryptedPath =
          domain::value_objects::Path(oldPath.toString() + ENCRYPTED_SUFFIX);
      domain::value_objects::Path newEncryptedPath =
          domain::value_objects::Path(newPath.toString() + ENCRYPTED_SUFFIX);
      m_fileHandler->renameFile(oldEncryptedPath, newEncryptedPath);
    }

    if (m_compressFiles) {
      domain::value_objects::Path oldCompressedPath =
          domain::value_objects::Path(oldPath.toString() + COMPRESSED_SUFFIX);
      domain::value_objects::Path newCompressedPath =
          domain::value_objects::Path(newPath.toString() + COMPRESSED_SUFFIX);
      m_fileHandler->renameFile(oldCompressedPath, newCompressedPath);
    }

    return true;

  } catch (...) {
    return false;
  }
}

std::vector<UploadFileInfo> UploadConfig::listUploadedFiles(
    bool showHidden) const {
  try {
    initializeDependencies();

    std::vector<UploadFileInfo> fileList;

    std::vector<infrastructure::filesystem::DirectoryEntry> entries =
        infrastructure::filesystem::DirectoryLister::listDirectory(
            m_uploadDirectory, showHidden, "name", true);

    for (std::size_t i = 0; i < entries.size(); ++i) {
      const infrastructure::filesystem::DirectoryEntry& entry = entries[i];

      if (entry.m_isDirectory || entry.m_name == "." || entry.m_name == ".." ||
          (!showHidden && entry.m_name[0] == '.')) {
        continue;
      }

      std::string name = entry.m_name;
      if (name.find(THUMBNAIL_SUFFIX) != std::string::npos ||
          name.find(WATERMARK_SUFFIX) != std::string::npos ||
          name.find(ENCRYPTED_SUFFIX) != std::string::npos ||
          name.find(COMPRESSED_SUFFIX) != std::string::npos) {
        continue;
      }

      UploadFileInfo info;
      info.originalFilename = name;  // In a real system, you'd have a mapping
      info.storedFilename = name;
      info.filePath = entry.m_fullPath;
      info.fileSize = entry.m_size;
      info.mimeType = m_fileHandler->detectMimeType(entry.m_fullPath);
      info.checksum = m_fileHandler->calculateChecksum(entry.m_fullPath, "md5");
      info.uploadTime = entry.m_lastModified;
      info.uploader = "unknown";  // In a real system, you'd store this metadata

      fileList.push_back(info);
    }

    return fileList;

  } catch (...) {
    return std::vector<UploadFileInfo>();
  }
}

UploadFileInfo UploadConfig::getFileInfo(
    const std::string& storedFilename) const {
  try {
    initializeDependencies();

    domain::value_objects::Path filePath = getFullPath(storedFilename);

    infrastructure::filesystem::FileMetadata metadata =
        m_fileHandler->getMetadata(filePath);

    UploadFileInfo info;
    info.originalFilename =
        storedFilename;  // In a real system, you'd have a mapping
    info.storedFilename = storedFilename;
    info.filePath = filePath;
    info.fileSize = metadata.size;
    info.mimeType = metadata.mimeType;
    info.checksum = metadata.checksum;
    info.uploadTime = metadata.lastModified;
    info.uploader = "unknown";  // In a real system, you'd store this metadata

    if (info.mimeType.find("image/") == 0) {
      info.metadata["type"] = "image";
    } else if (info.mimeType.find("text/") == 0) {
      info.metadata["type"] = "text";
    } else if (info.mimeType.find("application/") == 0) {
      info.metadata["type"] = "application";
    }

    return info;

  } catch (...) {
    throw shared::exceptions::UploadConfigException(
        "File not found: " + storedFilename,
        shared::exceptions::UploadConfigException::FILE_VALIDATION_FAILED);
  }
}

std::vector<char> UploadConfig::getFileContent(
    const std::string& storedFilename) const {
  try {
    initializeDependencies();

    domain::value_objects::Path filePath = getFullPath(storedFilename);

    return m_fileHandler->readFile(filePath);

  } catch (...) {
    throw shared::exceptions::UploadConfigException(
        "Failed to read file: " + storedFilename,
        shared::exceptions::UploadConfigException::FILE_VALIDATION_FAILED);
  }
}

UploadStatistics UploadConfig::getStatistics() const {
  try {
    initializeDependencies();

    UploadStatistics stats;

    std::vector<UploadFileInfo> files = listUploadedFiles(true);

    stats.totalFiles = files.size();
    stats.successfulUploads =
        files.size();         // In a real system, you'd track this separately
    stats.failedUploads = 0;  // In a real system, you'd track this

    for (std::size_t i = 0; i < files.size(); ++i) {
      stats.totalSize = stats.totalSize + files[i].fileSize;
    }

    stats.diskSpaceUsed =
        m_fileHandler->getAvailableDiskSpace(m_uploadDirectory);
    // Note: This returns available space, not used space
    // In a real system, you'd calculate used space differently

    stats.diskSpaceAvailable = stats.diskSpaceUsed;

    return stats;

  } catch (...) {
    return UploadStatistics();
  }
}

std::map<std::string, UploadStatistics> UploadConfig::getStatisticsByUploader()
    const {
  // In a real implementation, you would track uploads by uploader
  // For now, return empty map
  return std::map<std::string, UploadStatistics>();
}

std::map<std::string, std::size_t> UploadConfig::getFileTypeDistribution()
    const {
  try {
    initializeDependencies();

    std::map<std::string, std::size_t> distribution;

    std::vector<UploadFileInfo> files = listUploadedFiles(true);

    for (std::size_t i = 0; i < files.size(); ++i) {
      std::string type = "unknown";

      if (files[i].mimeType.find("image/") == 0) {
        type = "image";
      } else if (files[i].mimeType.find("text/") == 0) {
        type = "text";
      } else if (files[i].mimeType.find("application/pdf") == 0) {
        type = "pdf";
      } else if (files[i].mimeType.find("application/zip") == 0 ||
                 files[i].mimeType.find("application/x-rar-compressed") == 0) {
        type = "archive";
      } else if (files[i].mimeType.find("application/") == 0) {
        type = "application";
      } else if (files[i].mimeType.find("video/") == 0) {
        type = "video";
      } else if (files[i].mimeType.find("audio/") == 0) {
        type = "audio";
      }

      if (distribution.find(type) == distribution.end()) {
        distribution[type] = 1;
      } else {
        distribution[type]++;
      }
    }

    return distribution;

  } catch (...) {
    return std::map<std::string, std::size_t>();
  }
}

bool UploadConfig::cleanupOldFiles(int daysToKeep) const {
  try {
    initializeDependencies();

    bool success = true;
    std::vector<UploadFileInfo> files = listUploadedFiles(true);

    time_t now = time(NULL);
    time_t cutoffTime = now - (daysToKeep * 24 * 60 * 60);

    for (std::size_t i = 0; i < files.size(); ++i) {
      // Parse last modified time (simplified)
      // In a real system, you'd parse the actual timestamp
      struct tm tmTime;
      memset(&tmTime, 0, sizeof(tmTime));

      if (strptime(files[i].uploadTime.c_str(), "%Y-%m-%d %H:%M:%S", &tmTime) !=
          NULL) {
        time_t fileTime = mktime(&tmTime);

        if (fileTime < cutoffTime) {
          if (!deleteUploadedFile(files[i].storedFilename)) {
            success = false;
          }
        }
      }
    }

    return success;

  } catch (...) {
    return false;
  }
}

bool UploadConfig::cleanupOrphanedFiles() const {
  try {
    initializeDependencies();

    bool success = true;

    std::vector<infrastructure::filesystem::DirectoryEntry> entries =
        infrastructure::filesystem::DirectoryLister::listDirectory(
            m_uploadDirectory, true, "name", true);

    // In a real system, you would check which files are orphaned
    // (e.g., no corresponding database entry)
    // For now, just clean up temporary files

    for (std::size_t i = 0; i < entries.size(); ++i) {
      const infrastructure::filesystem::DirectoryEntry& entry = entries[i];

      if (entry.m_isDirectory) {
        continue;
      }

      std::string name = entry.m_name;

      if (name.find("temp_") == 0 || name.find("tmp_") == 0 ||
          name.find(".temp") != std::string::npos ||
          name.find(".tmp") != std::string::npos) {
        if (!m_fileHandler->deleteFile(entry.m_fullPath)) {
          success = false;
        }
      }
    }

    return success;

  } catch (...) {
    return false;
  }
}

bool UploadConfig::cleanupTemporaryFiles() const {
  // This is similar to cleanupOrphanedFiles but focused on temp files
  return cleanupOrphanedFiles();
}

bool UploadConfig::scanForViruses(
    const domain::value_objects::Path& filePath) const {
  // In a real implementation, you would integrate with an antivirus library
  // For now, just check file extensions and basic heuristics

  try {
    initializeDependencies();

    std::string filename = filePath.toString();

    std::string ext = extractExtension(filename);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

    const char* dangerousExts[] = {"exe", "dll", "bat",  "cmd", "sh",
                                   "php", "js",  "vbs",  "scr", "pif",
                                   "com", "jar", "class"};

    for (std::size_t i = 0;
         i < sizeof(dangerousExts) / sizeof(dangerousExts[0]); ++i) {
      if (ext == dangerousExts[i]) {
        return false;
      }
    }

    infrastructure::filesystem::FileMetadata metadata =
        m_fileHandler->getMetadata(filePath);

    if (metadata.size < domain::value_objects::Size(100) &&
        (ext == "" || ext == "bin")) {
      return false;
    }

    std::vector<char> chunk = m_fileHandler->readFileChunk(filePath, 0, 256);

    if (chunk.size() >= 2) {
      if (chunk[0] == 'M' && chunk[1] == 'Z') {
        return false;
      }

      if (chunk[0] == 0x7F && chunk[1] == 'E' && chunk[2] == 'L' &&
          chunk[3] == 'F') {
        return false;
      }
    }

    return true;

  } catch (...) {
    return false;
  }
}

bool UploadConfig::verifyFileIntegrity(
    const std::string& storedFilename,
    const std::string& expectedChecksum) const {
  try {
    initializeDependencies();

    domain::value_objects::Path filePath = getFullPath(storedFilename);

    std::string actualChecksum =
        m_fileHandler->calculateChecksum(filePath, "md5");

    return actualChecksum == expectedChecksum;

  } catch (...) {
    return false;
  }
}

std::string UploadConfig::generateUniqueFilename(
    const std::string& originalFilename) const {
  std::string sanitized = sanitizeFilename(originalFilename);

  std::string ext = extractExtension(sanitized);
  std::string nameWithoutExt = sanitized;

  if (!ext.empty()) {
    nameWithoutExt = sanitized.substr(0, sanitized.length() - ext.length() - 1);
  }

  std::string timestamp = getCurrentTimestamp();

  // Generate unique identifier (using timestamp and random number in real
  // system)
  std::ostringstream oss;
  oss << nameWithoutExt << "_" << timestamp;

  if (!ext.empty()) {
    oss << "." << ext;
  }

  return oss.str();
}

std::string UploadConfig::sanitizeFilename(const std::string& filename) const {
  std::string result = filename;

  std::size_t lastSlash = result.find_last_of("/\\");
  if (lastSlash != std::string::npos) {
    result = result.substr(lastSlash + 1);
  }

  const char* invalidChars = "/\\?%*:|\"<>";
  for (std::size_t i = 0; i < result.length(); ++i) {
    for (std::size_t j = 0; j < std::strlen(invalidChars); ++j) {
      if (result[i] == invalidChars[j]) {
        result[i] = '_';
        break;
      }
    }

    if (result[i] < 32) {
      result[i] = '_';
    }
  }

  std::size_t start = result.find_first_not_of(" \t\n\r");
  std::size_t end = result.find_last_not_of(" \t\n\r");

  if (start == std::string::npos || end == std::string::npos) {
    result = "file";
  } else {
    result = result.substr(start, end - start + 1);
  }

  if (result.length() > m_maxFilenameLength) {
    std::string ext = extractExtension(result);
    std::string nameWithoutExt = result.substr(
        0, result.length() - ext.length() - (ext.empty() ? 0 : 1));

    if (nameWithoutExt.length() >
        m_maxFilenameLength - (ext.empty() ? 0 : ext.length() + 1)) {
      nameWithoutExt = nameWithoutExt.substr(
          0, m_maxFilenameLength - (ext.empty() ? 0 : ext.length() + 1));
    }

    if (ext.empty()) {
      result = nameWithoutExt;
    } else {
      result = nameWithoutExt + "." + ext;
    }
  }

  return result;
}

domain::value_objects::Path UploadConfig::getFullPath(
    const std::string& filename) const {
  std::string fullPath = m_uploadDirectory.toString();
  if (!fullPath.empty() && fullPath[fullPath.length() - 1] != '/') {
    fullPath += "/";
  }
  fullPath += filename;

  return domain::value_objects::Path(fullPath, true);
}

domain::value_objects::Path UploadConfig::getUploadDirectory() const {
  return m_uploadDirectory;
}

domain::value_objects::Size UploadConfig::getMaxFileSize() const {
  return m_maxFileSize;
}

domain::value_objects::Size UploadConfig::getMaxTotalSize() const {
  return m_maxTotalSize;
}

std::size_t UploadConfig::getMaxFilesPerUpload() const {
  return m_maxFilesPerUpload;
}

std::size_t UploadConfig::getMaxFilenameLength() const {
  return m_maxFilenameLength;
}

std::size_t UploadConfig::getMaxUploadsPerHour() const {
  return m_maxUploadsPerHour;
}

bool UploadConfig::getOverwriteExisting() const { return m_overwriteExisting; }

bool UploadConfig::getGenerateThumbnails() const {
  return m_generateThumbnails;
}

bool UploadConfig::getApplyWatermark() const { return m_applyWatermark; }

bool UploadConfig::getEncryptFiles() const { return m_encryptFiles; }

bool UploadConfig::getCompressFiles() const { return m_compressFiles; }

void UploadConfig::initializeDependencies() const {
  if (m_fileHandler == NULL) {
    // Note: In a real DDD implementation, these would be injected
    // For now, create them with NULL helpers (which would need to be
    // implemented)
    m_fileHandler = new infrastructure::filesystem::FileHandler(NULL, NULL);
    m_directoryLister =
        new infrastructure::filesystem::DirectoryLister(NULL, NULL);
    m_pathResolver = new infrastructure::filesystem::PathResolver(NULL);
  }
}

void UploadConfig::cleanupDependencies() const {
  if (m_fileHandler != NULL) {
    delete m_fileHandler;
    m_fileHandler = NULL;
  }

  if (m_directoryLister != NULL) {
    delete m_directoryLister;
    m_directoryLister = NULL;
  }

  if (m_pathResolver != NULL) {
    delete m_pathResolver;
    m_pathResolver = NULL;
  }
}

bool UploadConfig::validateUploadConstraints(
    const std::string& uploader) const {
  if (uploader.empty() || m_maxUploadsPerHour == 0) {
    return true;
  }

  time_t now = time(NULL);
  time_t oneHourAgo = now - 3600;

  if (m_uploadHistory.find(uploader) != m_uploadHistory.end()) {
    std::vector<time_t>& history = m_uploadHistory[uploader];
    std::vector<time_t> recentHistory;

    for (std::size_t i = 0; i < history.size(); ++i) {
      if (history[i] >= oneHourAgo) {
        recentHistory.push_back(history[i]);
      }
    }

    m_uploadHistory[uploader] = recentHistory;

    if (recentHistory.size() >= m_maxUploadsPerHour) {
      return false;
    }
  }

  return true;
}

void UploadConfig::updateUploadStatistics(const std::string& uploader) const {
  // add fileSize remove to bypass the errors
  if (!uploader.empty()) {
    time_t now = time(NULL);
    m_uploadHistory[uploader].push_back(now);
  }
}

UploadFileInfo UploadConfig::createFileInfo(
    const std::string& originalFilename, const std::string& storedFilename,
    const domain::value_objects::Size& fileSize, const std::string& mimeType,
    const std::string& uploader) const {
  UploadFileInfo info;
  info.originalFilename = originalFilename;
  info.storedFilename = storedFilename;
  info.filePath = getFullPath(storedFilename);
  info.fileSize = fileSize;
  info.mimeType = mimeType;
  info.checksum = m_fileHandler->calculateChecksum(info.filePath, "md5");
  info.uploadTime = getCurrentTimestamp();
  info.uploader = uploader;

  if (mimeType.find("image/") == 0) {
    info.metadata["type"] = "image";
  } else if (mimeType.find("text/") == 0) {
    info.metadata["type"] = "text";
  } else if (mimeType.find("application/pdf") == 0) {
    info.metadata["type"] = "pdf";
  } else if (mimeType.find("application/") == 0) {
    info.metadata["type"] = "application";
  }

  return info;
}

std::string UploadConfig::extractExtension(const std::string& filename) const {
  std::size_t dotPos = filename.find_last_of('.');
  if (dotPos != std::string::npos && dotPos < filename.length() - 1) {
    return filename.substr(dotPos + 1);
  }
  return "";
}

std::string UploadConfig::getCurrentTimestamp() const {
  time_t now = time(NULL);
  struct tm* timeinfo = localtime(&now);

  char buffer[80];
  strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);

  return std::string(buffer);
}

bool UploadConfig::generateThumbnail(
    const domain::value_objects::Path& thumbPath) const {
  // In a real implementation, you would use an image processing library
  // For now, just create a placeholder
  // put sourcePath, remove to bypass the error
  try {
    std::vector<char> placeholder;
    const char* text = "Thumbnail placeholder";
    placeholder.assign(text, text + strlen(text));

    return m_fileHandler->writeFile(thumbPath, placeholder, true);
  } catch (...) {
    return false;
  }
}

bool UploadConfig::applyWatermark(
    const domain::value_objects::Path& sourcePath,
    const domain::value_objects::Path& watermarkedPath) const {
  // In a real implementation, you would apply a watermark to the image
  // For now, just copy the file
  try {
    std::vector<char> data = m_fileHandler->readFile(sourcePath);
    return m_fileHandler->writeFile(watermarkedPath, data, true);
  } catch (...) {
    return false;
  }
}

bool UploadConfig::encryptFile(
    const domain::value_objects::Path& sourcePath,
    const domain::value_objects::Path& encryptedPath) const {
  // In a real implementation, you would encrypt the file
  // For now, just compress it as a simple example
  try {
    std::vector<char> data = m_fileHandler->readFile(sourcePath);
    std::vector<char> compressed = m_fileHandler->compressData(data, "gzip");
    return m_fileHandler->writeFile(encryptedPath, compressed, true);
  } catch (...) {
    return false;
  }
}

bool UploadConfig::compressFile(
    const domain::value_objects::Path& sourcePath,
    const domain::value_objects::Path& compressedPath) const {
  try {
    return m_fileHandler->compressFile(sourcePath, compressedPath, "gzip");
  } catch (...) {
    return false;
  }
}

bool UploadConfig::isImageFile(const std::string& filename) {
  std::string ext = filename.substr(filename.find_last_of(".") + 1);
  std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

  const char* imageExts[] = {"jpg", "jpeg", "png", "gif",
                             "bmp", "tiff", "tif", "webp"};

  for (std::size_t i = 0; i < sizeof(imageExts) / sizeof(imageExts[0]); ++i) {
    if (ext == imageExts[i]) {
      return true;
    }
  }

  return false;
}

bool UploadConfig::isTextFile(const std::string& filename) {
  std::string ext = filename.substr(filename.find_last_of(".") + 1);
  std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

  const char* textExts[] = {"txt", "text", "csv", "xml",  "html",
                            "htm", "js",   "css", "json", "md"};

  for (std::size_t i = 0; i < sizeof(textExts) / sizeof(textExts[0]); ++i) {
    if (ext == textExts[i]) {
      return true;
    }
  }

  return false;
}

bool UploadConfig::isArchiveFile(const std::string& filename) {
  std::string ext = filename.substr(filename.find_last_of(".") + 1);
  std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

  const char* archiveExts[] = {"zip", "rar", "7z", "tar", "gz", "bz2", "xz"};

  for (std::size_t i = 0; i < sizeof(archiveExts) / sizeof(archiveExts[0]);
       ++i) {
    if (ext == archiveExts[i]) {
      return true;
    }
  }

  return false;
}

}  // namespace entities
}  // namespace domain
