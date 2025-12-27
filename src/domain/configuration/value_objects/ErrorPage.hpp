/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ErrorPage.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/25 21:59:32 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/27 16:18:49 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ERROR_PAGE_HPP
#define ERROR_PAGE_HPP

#include "domain/filesystem/value_objects/Path.hpp"
#include "domain/filesystem/value_objects/Size.hpp"
#include "domain/shared/value_objects/ErrorCode.hpp"

#include <map>
#include <string>

namespace domain {
namespace configuration {
namespace entities {

class ErrorPage {
 public:
  static const std::size_t MAX_CONTENT_LENGTH = 65536;
  static const std::string DEFAULT_CONTENT_TYPE;
  static const std::string HTML_CONTENT_TYPE;
  static const std::string PLAIN_TEXT_CONTENT_TYPE;

  static const std::string CODE_PLACEHOLDER;
  static const std::string MESSAGE_PLACEHOLDER;
  static const std::string DEFAULT_ERROR_PAGE_TEMPLATE;
  static const filesystem::value_objects::Path DEFAULT_ERROR_PAGES_DIR;

  ErrorPage();
  explicit ErrorPage(const shared::value_objects::ErrorCode& errorCode);
  ErrorPage(const shared::value_objects::ErrorCode& errorCode,
            const std::string& content);
  ErrorPage(const shared::value_objects::ErrorCode& errorCode,
            const filesystem::value_objects::Path& filePath);
  ErrorPage(const shared::value_objects::ErrorCode& errorCode,
            const std::string& content, const std::string& contentType);
  ErrorPage(const shared::value_objects::ErrorCode& errorCode,
            const filesystem::value_objects::Path& filePath,
            const std::string& contentType);
  ErrorPage(const ErrorPage& other);
  ~ErrorPage();

  ErrorPage& operator=(const ErrorPage& other);

  shared::value_objects::ErrorCode getErrorCode() const;
  std::string getContent() const;
  filesystem::value_objects::Path getFilePath() const;
  std::string getContentType() const;
  filesystem::value_objects::Size getContentSize() const;
  std::string toString() const;

  static bool isValidErrorPage(const std::string& content);
  static bool isValidErrorPageFile(const filesystem::value_objects::Path& filePath);
  static bool isValidContentType(const std::string& contentType);

  void setContent(const std::string& content);
  void setFilePath(const filesystem::value_objects::Path& filePath);
  void setContentType(const std::string& contentType);
  void setErrorCode(const shared::value_objects::ErrorCode& errorCode);

  bool hasFile() const;
  bool hasContent() const;
  bool isEmpty() const;
  bool isDefault() const;

  std::string buildResponse() const;
  std::string buildHtmlResponse() const;
  std::string buildPlainTextResponse() const;

  bool operator==(const ErrorPage& other) const;
  bool operator!=(const ErrorPage& other) const;
  bool operator<(const ErrorPage& other) const;

  static ErrorPage fromString(const std::string& errorPageString);
  static ErrorPage fromFile(const filesystem::value_objects::Path& filePath,
                            const shared::value_objects::ErrorCode& errorCode);
  static ErrorPage fromContent(const std::string& content,
                               const shared::value_objects::ErrorCode& errorCode);

  static ErrorPage createDefault(const shared::value_objects::ErrorCode& errorCode);
  static ErrorPage createHtmlDefault(const shared::value_objects::ErrorCode& errorCode);
  static ErrorPage createPlainTextDefault(
      const shared::value_objects::ErrorCode& errorCode);

  static std::map<shared::value_objects::ErrorCode, ErrorPage>
  createDefaultErrorPages();
  static std::map<unsigned int, ErrorPage> createDefaultErrorPagesByCode();

 private:
  shared::value_objects::ErrorCode m_errorCode;
  std::string m_content;
  filesystem::value_objects::Path m_filePath;
  std::string m_contentType;
  bool m_hasFile;
  bool m_hasContent;

  void validate() const;
  void validateContent() const;
  void validateFilePath() const;
  void validateContentType() const;

  static std::string loadFileContent(const filesystem::value_objects::Path& filePath);
  static std::string generateDefaultContent(
      const shared::value_objects::ErrorCode& errorCode);
  static std::string generateHtmlDefaultContent(
      const shared::value_objects::ErrorCode& errorCode);
  static std::string generatePlainTextDefaultContent(
      const shared::value_objects::ErrorCode& errorCode);

  static std::string buildStatusLine(const shared::value_objects::ErrorCode& errorCode);
  static std::string buildHeaders(const std::string& contentType,
                                  const filesystem::value_objects::Size& contentSize);
  static std::string normalizeContentType(const std::string& contentType);

  void initializeFromFile();
};

}  // namespace entities
}  // namespace configuration
}  // namespace domain

#endif  // ERROR_PAGE_HPP
