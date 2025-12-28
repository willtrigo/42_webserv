/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   UploadConfigException.cpp                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/22 13:24:13 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/28 00:23:01 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "domain/configuration/exceptions/UploadConfigException.hpp"

#include <sstream>

namespace domain {
namespace configuration {
namespace exceptions {

const std::pair<UploadConfigException::ErrorCode, std::string>
    UploadConfigException::K_CODE_MSGS[] = {
        std::make_pair(UploadConfigException::INVALID_UPLOAD_DIRECTORY,
                       "Invalid upload directory"),
        std::make_pair(UploadConfigException::UPLOAD_DIRECTORY_NOT_WRITABLE,
                       "Upload directory is not writable"),
        std::make_pair(UploadConfigException::MAX_FILE_SIZE_EXCEEDED,
                       "Maximum file size exceeded"),
        std::make_pair(UploadConfigException::MAX_TOTAL_SIZE_EXCEEDED,
                       "Maximum total upload size exceeded"),
        std::make_pair(UploadConfigException::INVALID_FILE_TYPE,
                       "Invalid file type"),
        std::make_pair(UploadConfigException::DUPLICATE_FILENAME,
                       "Duplicate filename"),
        std::make_pair(UploadConfigException::FILENAME_TOO_LONG,
                       "Filename is too long"),
        std::make_pair(UploadConfigException::INVALID_FILENAME_CHARACTERS,
                       "Filename contains invalid characters"),
        std::make_pair(UploadConfigException::UPLOAD_LIMIT_REACHED,
                       "Upload limit reached"),
        std::make_pair(UploadConfigException::TEMPORARY_STORAGE_ERROR,
                       "Temporary storage error"),
        std::make_pair(UploadConfigException::PERMANENT_STORAGE_ERROR,
                       "Permanent storage error"),
        std::make_pair(UploadConfigException::FILE_VALIDATION_FAILED,
                       "File validation failed"),
        std::make_pair(UploadConfigException::CONFIGURATION_ERROR,
                       "Configuration error"),
        std::make_pair(UploadConfigException::INVALID_MIME_TYPE,
                       "Invalid MIME type"),
        std::make_pair(UploadConfigException::HASH_VERIFICATION_FAILED,
                       "Hash verification failed"),
        std::make_pair(UploadConfigException::ANTIVIRUS_SCAN_FAILED,
                       "Antivirus scan failed"),
        std::make_pair(UploadConfigException::METADATA_EXTRACTION_FAILED,
                       "Metadata extraction failed"),
        std::make_pair(UploadConfigException::THUMBNAIL_GENERATION_FAILED,
                       "Thumbnail generation failed"),
        std::make_pair(UploadConfigException::WATERMARK_APPLICATION_FAILED,
                       "Watermark application failed"),
        std::make_pair(UploadConfigException::ENCRYPTION_FAILED,
                       "Encryption failed"),
        std::make_pair(UploadConfigException::COMPRESSION_FAILED,
                       "Compression failed")};

UploadConfigException::UploadConfigException(const std::string& msg,
                                             ErrorCode code)
    : BaseException("", static_cast<int>(code)) {
  std::ostringstream oss;
  oss << getErrorMsg(code) << ": " << msg;
  this->m_whatMsg = oss.str();
}

UploadConfigException::UploadConfigException(const UploadConfigException& other)
    : BaseException(other) {}

UploadConfigException::~UploadConfigException() throw() {}

UploadConfigException& UploadConfigException::operator=(
    const UploadConfigException& other) {
  if (this != &other) {
    BaseException::operator=(other);
  }
  return *this;
}

std::string UploadConfigException::getErrorMsg(
    UploadConfigException::ErrorCode code) {
  for (int index = 0; index < CODE_COUNT; ++index) {
    if (K_CODE_MSGS[index].first == code) {
      return K_CODE_MSGS[index].second;
    }
  }
  return "unknown upload configuration error";
}

}  // namespace exceptions
}  // namespace configuration
}  // namespace domain
