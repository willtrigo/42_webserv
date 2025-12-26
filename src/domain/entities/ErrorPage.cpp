/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ErrorPage.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/25 21:59:54 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/26 02:14:06 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "domain/entities/ErrorPage.hpp"
#include "domain/value_objects/ErrorCode.hpp"
#include "domain/value_objects/Path.hpp"
#include "domain/value_objects/Size.hpp"
#include "shared/exceptions/ErrorPageException.hpp"

#include <fstream>
#include <sstream>

namespace domain {
namespace entities {

const std::string ErrorPage::DEFAULT_CONTENT_TYPE = "text/html";
const std::string ErrorPage::HTML_CONTENT_TYPE = "text/html";
const std::string ErrorPage::PLAIN_TEXT_CONTENT_TYPE = "text/plain";
const std::string ErrorPage::CODE_PLACEHOLDER = "{CODE}";
const std::string ErrorPage::MESSAGE_PLACEHOLDER = "{MESSAGE}";

const std::string ErrorPage::DEFAULT_ERROR_PAGE_TEMPLATE =
    "<!DOCTYPE html>\n"
    "<html>\n"
    "<head>\n"
    "    <title>Error " +
    CODE_PLACEHOLDER +
    "</title>\n"
    "    <style>\n"
    "        body { font-family: Arial, sans-serif; text-align: center; "
    "padding: 50px; }\n"
    "        h1 { color: #d9534f; }\n"
    "        .code { font-size: 3em; font-weight: bold; }\n"
    "        .message { font-size: 1.5em; margin: 20px 0; }\n"
    "    </style>\n"
    "</head>\n"
    "<body>\n"
    "    <div class=\"code\">" +
    CODE_PLACEHOLDER +
    "</div>\n"
    "    <div class=\"message\">" +
    MESSAGE_PLACEHOLDER +
    "</div>\n"
    "    <hr>\n"
    "    <p>Webserv/1.0</p>\n"
    "</body>\n"
    "</html>";

const value_objects::Path ErrorPage::DEFAULT_ERROR_PAGES_DIR =
    value_objects::Path("error_pages");

ErrorPage::ErrorPage()
    : m_errorCode(value_objects::ErrorCode::STATUS_INTERNAL_SERVER_ERROR),
      m_contentType(DEFAULT_CONTENT_TYPE),
      m_hasFile(false),
      m_hasContent(false) {}

ErrorPage::ErrorPage(const value_objects::ErrorCode& errorCode)
    : m_errorCode(errorCode),
      m_contentType(DEFAULT_CONTENT_TYPE),
      m_hasFile(false),
      m_hasContent(false) {
  m_content = generateDefaultContent(errorCode);
  m_hasContent = true;
  validate();
}

ErrorPage::ErrorPage(const value_objects::ErrorCode& errorCode,
                     const std::string& content)
    : m_errorCode(errorCode),
      m_content(content),
      m_contentType(DEFAULT_CONTENT_TYPE),
      m_hasFile(false),
      m_hasContent(true) {
  validate();
}

ErrorPage::ErrorPage(const value_objects::ErrorCode& errorCode,
                     const value_objects::Path& filePath)
    : m_errorCode(errorCode),
      m_filePath(filePath),
      m_contentType(DEFAULT_CONTENT_TYPE),
      m_hasFile(true),
      m_hasContent(false) {
  initializeFromFile();
  validate();
}

ErrorPage::ErrorPage(const value_objects::ErrorCode& errorCode,
                     const std::string& content, const std::string& contentType)
    : m_errorCode(errorCode),
      m_content(content),
      m_contentType(normalizeContentType(contentType)),
      m_hasFile(false),
      m_hasContent(true) {
  validate();
}

ErrorPage::ErrorPage(const value_objects::ErrorCode& errorCode,
                     const value_objects::Path& filePath,
                     const std::string& contentType)
    : m_errorCode(errorCode),
      m_filePath(filePath),
      m_contentType(normalizeContentType(contentType)),
      m_hasFile(true),
      m_hasContent(false) {
  initializeFromFile();
  validate();
}

ErrorPage::ErrorPage(const ErrorPage& other)
    : m_errorCode(other.m_errorCode),
      m_content(other.m_content),
      m_filePath(other.m_filePath),
      m_contentType(other.m_contentType),
      m_hasFile(other.m_hasFile),
      m_hasContent(other.m_hasContent) {}

ErrorPage::~ErrorPage() {}

ErrorPage& ErrorPage::operator=(const ErrorPage& other) {
  if (this != &other) {
    m_errorCode = other.m_errorCode;
    m_content = other.m_content;
    m_filePath = other.m_filePath;
    m_contentType = other.m_contentType;
    m_hasFile = other.m_hasFile;
    m_hasContent = other.m_hasContent;
  }
  return *this;
}

value_objects::ErrorCode ErrorPage::getErrorCode() const { return m_errorCode; }

std::string ErrorPage::getContent() const { return m_content; }

value_objects::Path ErrorPage::getFilePath() const { return m_filePath; }

std::string ErrorPage::getContentType() const { return m_contentType; }

value_objects::Size ErrorPage::getContentSize() const {
  return value_objects::Size(m_content.size());
}

std::string ErrorPage::toString() const {
  std::ostringstream oss;
  oss << "ErrorPage{code=" << m_errorCode.toString()
      << ", contentType=" << m_contentType << ", size=" << m_content.size()
      << ", hasFile=" << (m_hasFile ? "true" : "false")
      << ", hasContent=" << (m_hasContent ? "true" : "false") << "}";
  return oss.str();
}

bool ErrorPage::isValidErrorPage(const std::string& content) {
  return content.size() <= MAX_CONTENT_LENGTH;
}

bool ErrorPage::isValidErrorPageFile(const value_objects::Path& filePath) {
  if (!filePath.isAbsolute()) {
    return false;
  }

  std::ifstream file(filePath.toString().c_str());
  if (!file.is_open()) {
    return false;
  }

  file.seekg(0, std::ios::end);
  std::size_t fileSize = static_cast<std::size_t>(file.tellg());
  file.close();

  return fileSize <= MAX_CONTENT_LENGTH;
}

bool ErrorPage::isValidContentType(const std::string& contentType) {
  if (contentType.empty()) {
    return false;
  }

  std::string normalized = normalizeContentType(contentType);

  return normalized == HTML_CONTENT_TYPE ||
         normalized == PLAIN_TEXT_CONTENT_TYPE || normalized.find("text/") == 0;
}

void ErrorPage::setContent(const std::string& content) {
  m_content = content;
  m_hasContent = true;
  m_hasFile = false;
  validateContent();
}

void ErrorPage::setFilePath(const value_objects::Path& filePath) {
  m_filePath = filePath;
  m_hasFile = true;
  m_hasContent = false;
  initializeFromFile();
  validateFilePath();
}

void ErrorPage::setContentType(const std::string& contentType) {
  m_contentType = normalizeContentType(contentType);
  validateContentType();
}

void ErrorPage::setErrorCode(const value_objects::ErrorCode& errorCode) {
  m_errorCode = errorCode;
}

bool ErrorPage::hasFile() const { return m_hasFile; }

bool ErrorPage::hasContent() const { return m_hasContent; }

bool ErrorPage::isEmpty() const { return m_content.empty() && !m_hasFile; }

bool ErrorPage::isDefault() const {
  std::string defaultContent = generateDefaultContent(m_errorCode);
  return m_content == defaultContent && !m_hasFile;
}

std::string ErrorPage::buildResponse() const {
  std::ostringstream response;

  response << buildStatusLine(m_errorCode);
  response << buildHeaders(m_contentType, getContentSize());
  response << "\r\n";
  response << m_content;

  return response.str();
}

std::string ErrorPage::buildHtmlResponse() const {
  ErrorPage htmlPage = *this;
  htmlPage.setContentType(HTML_CONTENT_TYPE);

  if (!m_content.empty()) {
    std::string htmlContent =
        "<!DOCTYPE html>\n<html>\n<head>\n"
        "<title>Error " +
        m_errorCode.toString() +
        "</title>\n"
        "</head>\n<body>\n" +
        m_content + "\n</body>\n</html>";
    htmlPage.setContent(htmlContent);
  }

  return htmlPage.buildResponse();
}

std::string ErrorPage::buildPlainTextResponse() const {
  ErrorPage textPage = *this;
  textPage.setContentType(PLAIN_TEXT_CONTENT_TYPE);
  return textPage.buildResponse();
}

bool ErrorPage::operator==(const ErrorPage& other) const {
  return m_errorCode == other.m_errorCode && m_content == other.m_content &&
         m_filePath == other.m_filePath && m_contentType == other.m_contentType;
}

bool ErrorPage::operator!=(const ErrorPage& other) const {
  return !(*this == other);
}

bool ErrorPage::operator<(const ErrorPage& other) const {
  if (m_errorCode < other.m_errorCode) return true;
  if (other.m_errorCode < m_errorCode) return false;
  if (m_contentType < other.m_contentType) return true;
  if (other.m_contentType < m_contentType) return false;
  return m_content < other.m_content;
}

ErrorPage ErrorPage::fromString(const std::string& errorPageString) {
  std::istringstream iss(errorPageString);
  std::string codeStr;
  std::string content;

  if (!std::getline(iss, codeStr, '|') || !std::getline(iss, content)) {
    throw shared::exceptions::ErrorPageException(
        "Invalid error page string format: '" + errorPageString + "'",
        shared::exceptions::ErrorPageException::INVALID_FORMAT);
  }

  value_objects::ErrorCode code = value_objects::ErrorCode::fromString(codeStr);
  return ErrorPage(code, content);
}

ErrorPage ErrorPage::fromFile(const value_objects::Path& filePath,
                              const value_objects::ErrorCode& errorCode) {
  return ErrorPage(errorCode, filePath);
}

ErrorPage ErrorPage::fromContent(const std::string& content,
                                 const value_objects::ErrorCode& errorCode) {
  return ErrorPage(errorCode, content);
}

ErrorPage ErrorPage::createDefault(const value_objects::ErrorCode& errorCode) {
  return ErrorPage(errorCode);
}

ErrorPage ErrorPage::createHtmlDefault(
    const value_objects::ErrorCode& errorCode) {
  std::string content = generateHtmlDefaultContent(errorCode);
  return ErrorPage(errorCode, content, HTML_CONTENT_TYPE);
}

ErrorPage ErrorPage::createPlainTextDefault(
    const value_objects::ErrorCode& errorCode) {
  std::string content = generatePlainTextDefaultContent(errorCode);
  return ErrorPage(errorCode, content, PLAIN_TEXT_CONTENT_TYPE);
}

std::map<value_objects::ErrorCode, ErrorPage>
ErrorPage::createDefaultErrorPages() {
  std::map<value_objects::ErrorCode, ErrorPage> errorPages;

  value_objects::ErrorCode commonCodes[] = {
      value_objects::ErrorCode::badRequest(),
      value_objects::ErrorCode::notFound(),
      value_objects::ErrorCode::methodNotAllowed(),
      value_objects::ErrorCode::payloadTooLarge(),
      value_objects::ErrorCode::internalServerError(),
      value_objects::ErrorCode::serviceUnavailable(),
      value_objects::ErrorCode::forbidden(),
      value_objects::ErrorCode::unauthorized(),
      value_objects::ErrorCode::requestTimeout(),
      value_objects::ErrorCode::conflict()};

  for (size_t i = 0; i < sizeof(commonCodes) / sizeof(commonCodes[0]); ++i) {
    errorPages[commonCodes[i]] = createDefault(commonCodes[i]);
  }

  return errorPages;
}

std::map<unsigned int, ErrorPage> ErrorPage::createDefaultErrorPagesByCode() {
  std::map<unsigned int, ErrorPage> errorPages;
  std::map<value_objects::ErrorCode, ErrorPage> byErrorCode =
      createDefaultErrorPages();

  for (std::map<value_objects::ErrorCode, ErrorPage>::const_iterator it =
           byErrorCode.begin();
       it != byErrorCode.end(); ++it) {
    errorPages[it->first.getValue()] = it->second;
  }

  return errorPages;
}

void ErrorPage::validate() const {
  if (!m_hasFile && !m_hasContent) {
    throw shared::exceptions::ErrorPageException(
        "ErrorPage must have either file path or content",
        shared::exceptions::ErrorPageException::NO_CONTENT);
  }

  if (m_hasFile && m_hasContent) {
    throw shared::exceptions::ErrorPageException(
        "ErrorPage cannot have both file path and content",
        shared::exceptions::ErrorPageException::CONFLICTING_SOURCES);
  }

  if (m_hasFile) {
    validateFilePath();
  }

  if (m_hasContent) {
    validateContent();
  }

  validateContentType();
}

void ErrorPage::validateContent() const {
  if (!isValidErrorPage(m_content)) {
    throw shared::exceptions::ErrorPageException(
        "Error page content exceeds maximum length",
        shared::exceptions::ErrorPageException::CONTENT_TOO_LARGE);
  }
}

void ErrorPage::validateFilePath() const {
  if (!m_filePath.isAbsolute()) {
    throw shared::exceptions::ErrorPageException(
        "Error page file path must be absolute: '" + m_filePath.toString() +
            "'",
        shared::exceptions::ErrorPageException::INVALID_PATH);
  }

  if (!isValidErrorPageFile(m_filePath)) {
    throw shared::exceptions::ErrorPageException(
        "Invalid error page file: '" + m_filePath.toString() + "'",
        shared::exceptions::ErrorPageException::FILE_ERROR);
  }
}

void ErrorPage::validateContentType() const {
  if (!isValidContentType(m_contentType)) {
    throw shared::exceptions::ErrorPageException(
        "Invalid content type: '" + m_contentType + "'",
        shared::exceptions::ErrorPageException::INVALID_CONTENT_TYPE);
  }
}

void ErrorPage::initializeFromFile() {
  m_content = loadFileContent(m_filePath);
  m_hasContent = true;
}

std::string ErrorPage::loadFileContent(const value_objects::Path& filePath) {
  std::ifstream file(filePath.toString().c_str());
  if (!file.is_open()) {
    throw shared::exceptions::ErrorPageException(
        "Cannot open error page file: '" + filePath.toString() + "'",
        shared::exceptions::ErrorPageException::FILE_ERROR);
  }

  std::ostringstream contentStream;
  contentStream << file.rdbuf();
  file.close();

  std::string content = contentStream.str();

  if (content.size() > MAX_CONTENT_LENGTH) {
    throw shared::exceptions::ErrorPageException(
        "Error page file too large: '" + filePath.toString() + "'",
        shared::exceptions::ErrorPageException::CONTENT_TOO_LARGE);
  }

  return content;
}

std::string ErrorPage::generateDefaultContent(
    const value_objects::ErrorCode& errorCode) {
  std::string htmlTemplate = DEFAULT_ERROR_PAGE_TEMPLATE;

  std::string codeStr = errorCode.toString();
  std::string message = errorCode.getDescription();

  size_t pos;
  while ((pos = htmlTemplate.find(CODE_PLACEHOLDER)) != std::string::npos) {
    htmlTemplate.replace(pos, CODE_PLACEHOLDER.length(), codeStr);
  }

  while ((pos = htmlTemplate.find(MESSAGE_PLACEHOLDER)) != std::string::npos) {
    htmlTemplate.replace(pos, MESSAGE_PLACEHOLDER.length(), message);
  }

  return htmlTemplate;
}

std::string ErrorPage::generateHtmlDefaultContent(
    const value_objects::ErrorCode& errorCode) {
  return generateDefaultContent(errorCode);
}

std::string ErrorPage::generatePlainTextDefaultContent(
    const value_objects::ErrorCode& errorCode) {
  std::ostringstream oss;
  oss << "Error " << errorCode.toString() << ": " << errorCode.getDescription();
  return oss.str();
}

std::string ErrorPage::buildStatusLine(
    const value_objects::ErrorCode& errorCode) {
  std::ostringstream oss;
  oss << "HTTP/1.1 " << errorCode.toString() << " "
      << errorCode.getDescription() << "\r\n";
  return oss.str();
}

std::string ErrorPage::buildHeaders(const std::string& contentType,
                                    const value_objects::Size& contentSize) {
  std::ostringstream oss;
  oss << "Content-Type: " << contentType << "\r\n";
  oss << "Content-Length: " << contentSize.getBytes() << "\r\n";
  oss << "Connection: close\r\n";
  oss << "Server: Webserv/1.0\r\n";
  return oss.str();
}

std::string ErrorPage::normalizeContentType(const std::string& contentType) {
  std::string normalized = contentType;

  for (size_t i = 0; i < normalized.size(); ++i) {
    normalized[i] = static_cast<char>(
        std::tolower(static_cast<unsigned char>(normalized[i])));
  }

  if (normalized.find("html") != std::string::npos &&
      normalized.find("text/") != 0) {
    normalized = HTML_CONTENT_TYPE;
  } else if (normalized.find("plain") != std::string::npos &&
             normalized.find("text/") != 0) {
    normalized = PLAIN_TEXT_CONTENT_TYPE;
  }

  return normalized;
}

}  // namespace entities
}  // namespace domain
