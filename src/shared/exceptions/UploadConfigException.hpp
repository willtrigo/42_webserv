/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   UploadConfigException.hpp                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/22 13:23:51 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/23 20:29:39 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UPLOADCONFIGEXCEPTION_HPP
#define UPLOADCONFIGEXCEPTION_HPP

#include "shared/exceptions/BaseException.hpp"

namespace shared {
namespace exceptions {

class UploadConfigException : public BaseException {
 public:
  enum ErrorCode {
    INVALID_UPLOAD_DIRECTORY,
    UPLOAD_DIRECTORY_NOT_WRITABLE,
    MAX_FILE_SIZE_EXCEEDED,
    MAX_TOTAL_SIZE_EXCEEDED,
    INVALID_FILE_TYPE,
    DUPLICATE_FILENAME,
    FILENAME_TOO_LONG,
    INVALID_FILENAME_CHARACTERS,
    UPLOAD_LIMIT_REACHED,
    TEMPORARY_STORAGE_ERROR,
    PERMANENT_STORAGE_ERROR,
    FILE_VALIDATION_FAILED,
    CONFIGURATION_ERROR,
    INVALID_MIME_TYPE,
    HASH_VERIFICATION_FAILED,
    ANTIVIRUS_SCAN_FAILED,
    METADATA_EXTRACTION_FAILED,
    THUMBNAIL_GENERATION_FAILED,
    WATERMARK_APPLICATION_FAILED,
    ENCRYPTION_FAILED,
    COMPRESSION_FAILED,
    CODE_COUNT
  };

  explicit UploadConfigException(const std::string& msg, ErrorCode code);
  UploadConfigException(const UploadConfigException& other);
  virtual ~UploadConfigException() throw();

  UploadConfigException& operator=(const UploadConfigException& other);

 private:
  static const std::pair<ErrorCode, std::string> K_CODE_MSGS[];

  static std::string getErrorMsg(ErrorCode code);
};

}  // namespace exceptions
}  // namespace shared

#endif  // UPLOADCONFIGEXCEPTION_HPP
