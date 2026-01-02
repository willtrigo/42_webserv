/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationConfig.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/22 12:23:41 by dande-je          #+#    #+#             */
/*   Updated: 2026/01/02 02:36:51 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "domain/configuration/entities/LocationConfig.hpp"
#include "domain/configuration/exceptions/LocationConfigException.hpp"
#include "domain/shared/utils/StringUtils.hpp"

#include <cctype>
#include <cstring>
#include <iostream>
#include <sstream>

namespace domain {
namespace configuration {
namespace entities {

LocationConfig::LocationConfig()
    : m_path("/"),
      m_matchType(MATCH_PREFIX),
      m_root(filesystem::value_objects::Path::rootDirectory()),
      m_autoIndex(false),
      m_returnCode(shared::value_objects::ErrorCode::movedPermanently()),
      m_hasReturnContent(false),
      m_uploadConfig(filesystem::value_objects::Path("/tmp/uploads")),
      m_hasUploadConfig(false),
      m_clientMaxBodySize(filesystem::value_objects::Size::fromMegabytes(
          DEFAULT_CLIENT_MAX_BODY_SIZE)),
      m_clientBodyBufferSize(filesystem::value_objects::Size::fromKilobytes(
          DEFAULT_CLIENT_BODY_BUFFER_SIZE)),
      m_clientBodyBufferSizeSet(false),
      m_regexPatternValid(false) {
  m_allowedMethods.insert(http::value_objects::HttpMethod::get());
  m_allowedMethods.insert(http::value_objects::HttpMethod::post());
  m_allowedMethods.insert(http::value_objects::HttpMethod::deleteMethod());
  m_allowedMethods.insert(http::value_objects::HttpMethod::head());

  m_indexFiles.push_back("index.html");
  m_indexFiles.push_back("index.htm");
}

LocationConfig::LocationConfig(const std::string& path,
                               LocationMatchType matchType)
    : m_path(path),
      m_matchType(matchType),
      m_root(filesystem::value_objects::Path::rootDirectory()),
      m_autoIndex(false),
      m_returnCode(shared::value_objects::ErrorCode::movedPermanently()),
      m_hasReturnContent(false),
      m_uploadConfig(filesystem::value_objects::Path("/tmp/uploads")),
      m_hasUploadConfig(false),
      m_clientMaxBodySize(filesystem::value_objects::Size::fromMegabytes(
          DEFAULT_CLIENT_MAX_BODY_SIZE)),
      m_clientBodyBufferSize(filesystem::value_objects::Size::fromKilobytes(
          DEFAULT_CLIENT_BODY_BUFFER_SIZE)),
      m_clientBodyBufferSizeSet(false),
      m_regexPatternValid(false) {
  validatePath();

  m_allowedMethods.insert(http::value_objects::HttpMethod::get());
  m_allowedMethods.insert(http::value_objects::HttpMethod::post());
  m_allowedMethods.insert(http::value_objects::HttpMethod::deleteMethod());
  m_allowedMethods.insert(http::value_objects::HttpMethod::head());

  m_indexFiles.push_back("index.html");
  m_indexFiles.push_back("index.htm");
}

LocationConfig::LocationConfig(const LocationConfig& other)
    : m_path(other.m_path),
      m_matchType(other.m_matchType),
      m_root(other.m_root),
      m_indexFiles(other.m_indexFiles),
      m_allowedMethods(other.m_allowedMethods),
      m_autoIndex(other.m_autoIndex),
      m_tryFiles(other.m_tryFiles),
      m_returnRedirect(other.m_returnRedirect),
      m_returnCode(other.m_returnCode),
      m_returnContent(other.m_returnContent),
      m_hasReturnContent(other.m_hasReturnContent),
      m_uploadConfig(other.m_uploadConfig),
      m_hasUploadConfig(other.m_hasUploadConfig),
      m_cgiConfig(other.m_cgiConfig),
      m_errorPages(other.m_errorPages),
      m_clientMaxBodySize(other.m_clientMaxBodySize),
      m_proxyPass(other.m_proxyPass),
      m_alias(other.m_alias),
      m_clientBodyBufferSize(other.m_clientBodyBufferSize),
      m_clientBodyBufferSizeSet(other.m_clientBodyBufferSizeSet),
      m_customHeaders(other.m_customHeaders) {
  m_regexPatternValid = false;
}

LocationConfig::~LocationConfig() {}

LocationConfig& LocationConfig::operator=(const LocationConfig& other) {
  if (this != &other) {
    clear();
    m_path = other.m_path;
    m_matchType = other.m_matchType;
    m_root = other.m_root;
    m_indexFiles = other.m_indexFiles;
    m_allowedMethods = other.m_allowedMethods;
    m_autoIndex = other.m_autoIndex;
    m_tryFiles = other.m_tryFiles;
    m_returnRedirect = other.m_returnRedirect;
    m_returnCode = other.m_returnCode;
    m_returnContent = other.m_returnContent;
    m_hasReturnContent = other.m_hasReturnContent;
    m_uploadConfig = other.m_uploadConfig;
    m_hasUploadConfig = other.m_hasUploadConfig;
    m_cgiConfig = other.m_cgiConfig;
    m_errorPages = other.m_errorPages;
    m_clientMaxBodySize = other.m_clientMaxBodySize;
    m_proxyPass = other.m_proxyPass;
    m_alias = other.m_alias;
    m_clientBodyBufferSize = other.m_clientBodyBufferSize;
    m_clientBodyBufferSizeSet = other.m_clientBodyBufferSizeSet;
    m_customHeaders = other.m_customHeaders;
    m_regexPatternValid = false;
  }
  return *this;
}

const std::string& LocationConfig::getPath() const { return m_path; }

LocationConfig::LocationMatchType LocationConfig::getMatchType() const {
  return m_matchType;
}

const filesystem::value_objects::Path& LocationConfig::getRoot() const {
  return m_root;
}

const std::vector<std::string>& LocationConfig::getIndexFiles() const {
  return m_indexFiles;
}

const LocationConfig::AllowedMethods& LocationConfig::getAllowedMethods()
    const {
  return m_allowedMethods;
}

bool LocationConfig::getAutoIndex() const { return m_autoIndex; }

const LocationConfig::TryFiles& LocationConfig::getTryFiles() const {
  return m_tryFiles;
}

bool LocationConfig::hasReturnRedirect() const {
  return !m_returnRedirect.isEmpty();
}

const http::value_objects::Uri& LocationConfig::getReturnRedirect() const {
  return m_returnRedirect;
}

const shared::value_objects::ErrorCode& LocationConfig::getReturnCode() const {
  return m_returnCode;
}

bool LocationConfig::hasUploadConfig() const { return m_hasUploadConfig; }

const value_objects::UploadConfig& LocationConfig::getUploadConfig() const {
  return m_uploadConfig;
}

value_objects::UploadConfig& LocationConfig::getUploadConfigMutable() {
  return m_uploadConfig;
}

const value_objects::CgiConfig& LocationConfig::getCgiConfig() const {
  return m_cgiConfig;
}

const LocationConfig::ErrorPageMap& LocationConfig::getErrorPages() const {
  return m_errorPages;
}

const filesystem::value_objects::Size& LocationConfig::getClientMaxBodySize()
    const {
  return m_clientMaxBodySize;
}

bool LocationConfig::hasProxyPass() const { return !m_proxyPass.isEmpty(); }

const http::value_objects::Uri& LocationConfig::getProxyPass() const {
  return m_proxyPass;
}

const filesystem::value_objects::Path& LocationConfig::getAlias() const {
  return m_alias;
}

bool LocationConfig::getClientBodyBufferSizeSet() const {
  return m_clientBodyBufferSizeSet;
}

const filesystem::value_objects::Size& LocationConfig::getClientBodyBufferSize()
    const {
  return m_clientBodyBufferSize;
}

void LocationConfig::setPath(const std::string& path,
                             LocationMatchType matchType) {
  m_path = path;
  m_matchType = matchType;
  m_regexPatternValid = false;
  validatePath();
}

void LocationConfig::setRoot(const filesystem::value_objects::Path& root) {
  m_root = root;
}

void LocationConfig::setRoot(const std::string& root) {
  try {
    m_root = filesystem::value_objects::Path::fromString(root, false);
  } catch (const std::exception& e) {
    std::ostringstream oss;
    oss << "Invalid root path: " << e.what();
    throw exceptions::LocationConfigException(
        oss.str(), exceptions::LocationConfigException::INVALID_ROOT_PATH);
  }
}

void LocationConfig::addIndexFile(const std::string& index) {
  if (index.empty()) {
    throw exceptions::LocationConfigException(
        "Index file cannot be empty",
        exceptions::LocationConfigException::EMPTY_INDEX_FILE);
  }

  for (std::size_t i = 0; i < m_indexFiles.size(); ++i) {
    std::ostringstream oss;
    if (m_indexFiles[i] == index) {
      oss << "Duplicate index file: '" << index << "'";
      throw exceptions::LocationConfigException(
          oss.str(), exceptions::LocationConfigException::DUPLICATE_INDEX_FILE);
    }
  }

  m_indexFiles.push_back(index);
}

void LocationConfig::clearIndexFiles() { m_indexFiles.clear(); }

void LocationConfig::addAllowedMethod(
    const http::value_objects::HttpMethod& method) {
  m_allowedMethods.insert(method);
}

void LocationConfig::removeAllowedMethod(
    const http::value_objects::HttpMethod& method) {
  m_allowedMethods.erase(method);
}

void LocationConfig::setAutoIndex(bool autoIndex) { m_autoIndex = autoIndex; }

void LocationConfig::setTryFiles(const TryFiles& tryFiles) {
  if (!tryFiles.empty()) {
    for (std::size_t i = 0; i < tryFiles.size(); ++i) {
      if (tryFiles[i].empty()) {
        throw exceptions::LocationConfigException(
            "Try files entry cannot be empty",
            exceptions::LocationConfigException::EMPTY_TRY_FILE);
      }
    }
  }
  m_tryFiles = tryFiles;
}

void LocationConfig::addTryFile(const std::string& tryFile) {
  if (tryFile.empty()) {
    throw exceptions::LocationConfigException(
        "Try files entry cannot be empty",
        exceptions::LocationConfigException::EMPTY_TRY_FILE);
  }
  m_tryFiles.push_back(tryFile);
}

void LocationConfig::setReturnRedirect(
    const http::value_objects::Uri& redirect,
    const shared::value_objects::ErrorCode& code) {
  if (redirect.isEmpty()) {
    throw exceptions::LocationConfigException(
        "Return redirect URL cannot be empty",
        exceptions::LocationConfigException::EMPTY_REDIRECT_URL);
  }

  if (!code.isRedirection() && !code.isError()) {
    std::ostringstream oss;
    oss << "Invalid redirect/error code: " << code.getValue()
        << " (must be a redirection or error code)";
    throw exceptions::LocationConfigException(
        oss.str(), exceptions::LocationConfigException::INVALID_REDIRECT_CODE);
  }

  m_returnRedirect = redirect;
  m_returnCode = code;
}

void LocationConfig::setReturnRedirect(const std::string& redirect,
                                       unsigned int code) {
  try {
    shared::value_objects::ErrorCode errorCode(code);
    http::value_objects::Uri redirectUri(redirect);
    setReturnRedirect(redirectUri, errorCode);
  } catch (const std::exception& e) {
    std::ostringstream oss;
    oss << "Invalid redirect: " << e.what();
    throw exceptions::LocationConfigException(
        oss.str(), exceptions::LocationConfigException::INVALID_REDIRECT_CODE);
  }
}

void LocationConfig::setReturnContent(
    const std::string& content, const shared::value_objects::ErrorCode& code) {
  std::string trimmedContent = shared::utils::StringUtils::trim(content);

  // Validate code is NOT a redirect code (should be 2xx, 4xx, or 5xx)
  if (code.isRedirection()) {
    std::ostringstream oss;
    oss << "Return content code cannot be a redirect code: " << code.getValue()
        << " (must be 2xx, 4xx, or 5xx)";
    throw exceptions::LocationConfigException(
        oss.str(), exceptions::LocationConfigException::INVALID_REDIRECT_CODE);
  }

  // Validate it's a valid return code (2xx, 4xx, or 5xx)
  if (!code.isSuccess() && !code.isClientError() && !code.isServerError()) {
    std::ostringstream oss;
    oss << "Invalid return content code: " << code.getValue()
        << " (must be 2xx, 4xx, or 5xx)";
    throw exceptions::LocationConfigException(
        oss.str(), exceptions::LocationConfigException::INVALID_REDIRECT_CODE);
  }

  m_returnContent = trimmedContent;
  m_returnCode = code;
  m_hasReturnContent = true;
  m_returnRedirect = http::value_objects::Uri();  // Clear any redirect
}

void LocationConfig::setReturnContent(const std::string& content,
                                      unsigned int code) {
  try {
    std::string trimmedContent = shared::utils::StringUtils::trim(content);
    shared::value_objects::ErrorCode errorCode(code);
    setReturnContent(trimmedContent, errorCode);
  } catch (const std::exception& e) {
    std::ostringstream oss;
    oss << "Invalid return content: " << e.what();
    throw exceptions::LocationConfigException(
        oss.str(), exceptions::LocationConfigException::INVALID_REDIRECT_CODE);
  }
}

const std::string& LocationConfig::getReturnContent() const {
  return m_returnContent;
}

bool LocationConfig::hasReturnContent() const {
  return m_hasReturnContent &&
         (m_returnCode.isSuccess() || m_returnCode.isClientError() ||
          m_returnCode.isServerError());
}

void LocationConfig::setUploadConfig(
    const value_objects::UploadConfig& config) {
  m_uploadConfig = config;
  m_hasUploadConfig = true;
  validateUploadConfig();
}

void LocationConfig::enableUpload(
    const filesystem::value_objects::Path& uploadDirectory) {
  m_uploadConfig = value_objects::UploadConfig(uploadDirectory);
  m_hasUploadConfig = true;
  validateUploadConfig();
}

void LocationConfig::enableUpload(
    const filesystem::value_objects::Path& uploadDirectory,
    const filesystem::value_objects::Size& maxFileSize,
    const filesystem::value_objects::Size& maxTotalSize) {
  m_uploadConfig =
      value_objects::UploadConfig(uploadDirectory, maxFileSize, maxTotalSize);
  m_hasUploadConfig = true;
  validateUploadConfig();
}

void LocationConfig::disableUpload() {
  m_hasUploadConfig = false;
  m_uploadConfig = value_objects::UploadConfig(
      filesystem::value_objects::Path("/tmp/uploads"));
}

void LocationConfig::setCgiConfig(const value_objects::CgiConfig& config) {
  if (!config.isValid()) {
    throw exceptions::LocationConfigException(
        "Invalid CGI configuration",
        exceptions::LocationConfigException::INVALID_CGI_CONFIG);
  }

  m_cgiConfig = config;
}

void LocationConfig::clearCgiConfig() {
  m_cgiConfig = value_objects::CgiConfig();
}

void LocationConfig::addErrorPage(const shared::value_objects::ErrorCode& code,
                                  const std::string& uri) {
  if (!code.isError()) {
    std::ostringstream oss;
    oss << "Invalid error code for error page: " << code.getValue()
        << " (must be an error code between 400 and 599)";
    throw exceptions::LocationConfigException(
        oss.str(), exceptions::LocationConfigException::INVALID_ERROR_CODE);
  }

  if (uri.empty()) {
    std::ostringstream oss;
    oss << "Error page URI cannot be empty for error code " << code.getValue();
    throw exceptions::LocationConfigException(
        oss.str(), exceptions::LocationConfigException::EMPTY_ERROR_PAGE_URI);
  }

  if (uri[0] != '/') {
    std::ostringstream oss;
    oss << "Error page URI must start with '/': " << uri << " for error code "
        << code.getValue();
    throw exceptions::LocationConfigException(
        oss.str(), exceptions::LocationConfigException::INVALID_ERROR_PAGE_URI);
  }

  m_errorPages[code] = uri;
}

void LocationConfig::removeErrorPage(
    const shared::value_objects::ErrorCode& code) {
  m_errorPages.erase(code);
}

void LocationConfig::setClientMaxBodySize(
    const filesystem::value_objects::Size& size) {
  const filesystem::value_objects::Size MAX_BODY_SIZE =
      filesystem::value_objects::Size::fromMegabytes(
          MAX_ALLOWED_CLIENT_BODY_SIZE);

  if (size.getBytes() > MAX_BODY_SIZE.getBytes()) {
    std::ostringstream oss;
    oss << "Client max body size too large: " << size.toString()
        << " (maximum is " << MAX_BODY_SIZE.toString() << ")";
    throw exceptions::LocationConfigException(
        oss.str(), exceptions::LocationConfigException::BODY_SIZE_TOO_LARGE);
  }

  m_clientMaxBodySize = size;
}

void LocationConfig::setClientMaxBodySize(const std::string& sizeString) {
  try {
    filesystem::value_objects::Size size =
        filesystem::value_objects::Size::fromString(sizeString);
    setClientMaxBodySize(size);
  } catch (const std::exception& e) {
    std::ostringstream oss;
    oss << "Invalid client max body size: " << e.what();
    throw exceptions::LocationConfigException(
        oss.str(), exceptions::LocationConfigException::BODY_SIZE_TOO_LARGE);
  }
}

void LocationConfig::setProxyPass(const http::value_objects::Uri& proxyPass) {
  if (proxyPass.isEmpty()) {
    throw exceptions::LocationConfigException(
        "Proxy pass URL cannot be empty",
        exceptions::LocationConfigException::EMPTY_PROXY_PASS);
  }

  if (!proxyPass.isHttp() && !proxyPass.isHttps()) {
    throw exceptions::LocationConfigException(
        "Proxy pass must be an HTTP or HTTPS URL: " + proxyPass.toString(),
        exceptions::LocationConfigException::INVALID_PROXY_PASS);
  }

  m_proxyPass = proxyPass;
}

void LocationConfig::setProxyPass(const std::string& proxyPass) {
  try {
    http::value_objects::Uri uri(proxyPass);
    setProxyPass(uri);
  } catch (const std::exception& e) {
    std::ostringstream oss;
    oss << "Invalid proxy pass URL: " << e.what();
    throw exceptions::LocationConfigException(
        oss.str(), exceptions::LocationConfigException::INVALID_PROXY_PASS);
  }
}

void LocationConfig::clearProxyPass() {
  m_proxyPass = http::value_objects::Uri();
}

void LocationConfig::setAlias(const filesystem::value_objects::Path& alias) {
  if (!alias.isEmpty() && !alias.isAbsolute()) {
    throw exceptions::LocationConfigException(
        "Alias path must be absolute",
        exceptions::LocationConfigException::INVALID_ROOT_PATH);
  }
  m_alias = alias;
}

void LocationConfig::setAlias(const std::string& alias) {
  try {
    filesystem::value_objects::Path path =
        filesystem::value_objects::Path::fromString(alias, false);
    setAlias(path);
  } catch (const std::exception& e) {
    std::ostringstream oss;
    oss << "Invalid alias path: " << e.what();
    throw exceptions::LocationConfigException(
        oss.str(), exceptions::LocationConfigException::INVALID_ROOT_PATH);
  }
}

void LocationConfig::setClientBodyBufferSize(
    const filesystem::value_objects::Size& size) {
  m_clientBodyBufferSize = size;
  m_clientBodyBufferSizeSet = true;
}

void LocationConfig::setClientBodyBufferSize(const std::string& sizeString) {
  try {
    filesystem::value_objects::Size size =
        filesystem::value_objects::Size::fromString(sizeString);
    setClientBodyBufferSize(size);
  } catch (const std::exception& e) {
    std::ostringstream oss;
    oss << "Invalid client body buffer size: " << e.what();
    throw exceptions::LocationConfigException(
        oss.str(), exceptions::LocationConfigException::BODY_SIZE_TOO_LARGE);
  }
}

void LocationConfig::setUploadDirectory(const std::string& directory) {
  domain::filesystem::value_objects::Path uploadDir(directory, false);

  if (!m_hasUploadConfig) {
    // Create new UploadConfig with directory
    m_uploadConfig =
        domain::configuration::value_objects::UploadConfig(uploadDir);
    m_hasUploadConfig = true;
  } else {
    // Update existing UploadConfig directory
    m_uploadConfig.setUploadDirectory(uploadDir);
  }
}

void LocationConfig::setUploadPermissions(unsigned int permissions) {
  if (!m_hasUploadConfig) {
    // Create default UploadConfig first
    domain::filesystem::value_objects::Path defaultDir(
        "/tmp");  // Default directory
    m_uploadConfig =
        domain::configuration::value_objects::UploadConfig(defaultDir);
    m_hasUploadConfig = true;
  }

  // Create Permission object and set it
  domain::filesystem::value_objects::Permission perm(permissions);
  m_uploadConfig.setPermissions(perm);
}

void LocationConfig::setUploadAccess(const std::string& accessString) {
  if (!m_hasUploadConfig) {
    // Create default UploadConfig first
    domain::filesystem::value_objects::Path defaultDir("/tmp");
    m_uploadConfig =
        domain::configuration::value_objects::UploadConfig(defaultDir);
    m_hasUploadConfig = true;
  }

  m_uploadConfig.setUploadAccess(accessString);
}

void LocationConfig::setUploadMaxFileSize(const std::string& sizeString) {
  if (!m_hasUploadConfig) {
    // Create default UploadConfig first
    domain::filesystem::value_objects::Path defaultDir("/tmp");
    m_uploadConfig =
        domain::configuration::value_objects::UploadConfig(defaultDir);
    m_hasUploadConfig = true;
  }

  domain::filesystem::value_objects::Size maxSize(sizeString);
  m_uploadConfig.setMaxFileSize(maxSize);
}

void LocationConfig::setUploadMaxTotalSize(const std::string& sizeString) {
  if (!m_hasUploadConfig) {
    // Create default UploadConfig first
    domain::filesystem::value_objects::Path defaultDir("/tmp");
    m_uploadConfig =
        domain::configuration::value_objects::UploadConfig(defaultDir);
    m_hasUploadConfig = true;
  }

  domain::filesystem::value_objects::Size maxTotalSize(sizeString);
  m_uploadConfig.setMaxTotalSize(maxTotalSize);
}

bool LocationConfig::isValid() const {
  try {
    validate();
    return true;
  } catch (const exceptions::LocationConfigException&) {
    return false;
  }
}

void LocationConfig::validate() const {
  validatePath();
  validateRoot();
  validateAllowedMethods();
  validateTryFiles();
  validateReturnRedirect();
  if (m_hasUploadConfig) validateUploadConfig();
  if (hasCgiConfig()) validateCgiConfig();
  validateErrorPages();
  validateClientMaxBodySize();
  if (hasProxyPass()) validateProxyPass();
  if (hasAlias()) validateAlias();
  if (m_clientBodyBufferSizeSet) validateClientBodyBufferSize();

  if (hasProxyPass() &&
      (isUploadEnabled() || hasCgiConfig() || hasReturnRedirect())) {
    throw exceptions::LocationConfigException(
        "Proxy pass cannot be combined with upload, CGI, or return directives",
        exceptions::LocationConfigException::CONFLICTING_DIRECTIVES);
  }

  // TODO: may need this validation
  // if (hasReturnRedirect() &&
  //     (isUploadEnabled() || hasCgiConfig() || hasProxyPass())) {
  //   throw exceptions::LocationConfigException(
  //       "Return redirect cannot be combined with upload, CGI, or proxy pass "
  //       "directives",
  //       exceptions::LocationConfigException::CONFLICTING_DIRECTIVES);
  // }

  if (hasAlias() && !m_root.isEmpty()) {
    throw exceptions::LocationConfigException(
        "Alias and root directives cannot be used together",
        exceptions::LocationConfigException::CONFLICTING_DIRECTIVES);
  }

  if (isUploadEnabled() && !allowsPost()) {
    throw exceptions::LocationConfigException(
        "Upload enabled but POST method not allowed",
        exceptions::LocationConfigException::CONFLICTING_DIRECTIVES);
  }
}

void LocationConfig::validatePath() const {
  if (m_path.empty()) {
    throw exceptions::LocationConfigException(
        "Location path cannot be empty",
        exceptions::LocationConfigException::EMPTY_PATH);
  }

  if (m_path[0] == '@') {
    // Validate named location format
    if (m_path.length() == 1) {
      throw exceptions::LocationConfigException(
          "Named location cannot be just '@'",
          exceptions::LocationConfigException::INVALID_PATH_FORMAT);
    }

    // Named locations can only have alphanumeric characters and underscores
    for (std::size_t i = 1; i < m_path.length(); ++i) {
      char c = m_path[i];
      if ((std::isalnum(static_cast<unsigned char>(c)) == 0) && c != '_') {
        std::ostringstream oss;
        oss << "Invalid character in named location '" << m_path
            << "': only alphanumeric and underscore allowed";
        throw exceptions::LocationConfigException(
            oss.str(),
            exceptions::LocationConfigException::INVALID_PATH_FORMAT);
      }
    }

    // Named locations must use MATCH_EXACT match type
    if (m_matchType != MATCH_EXACT) {
      std::ostringstream oss;
      oss << "Named location '" << m_path << "' must use exact match type";
      throw exceptions::LocationConfigException(
          oss.str(), exceptions::LocationConfigException::INVALID_PATH_FORMAT);
    }

    return;  // Skip further validation for named locations
  }

  if (m_matchType == MATCH_REGEX_CASE_SENSITIVE ||
      m_matchType == MATCH_REGEX_CASE_INSENSITIVE) {
    if (!shared::value_objects::RegexPattern::isValidPattern(m_path)) {
      std::ostringstream oss;
      oss << "Invalid regex pattern for location: '" << m_path << "'";
      throw exceptions::LocationConfigException(
          oss.str(),
          exceptions::LocationConfigException::INVALID_REGEX_PATTERN);
    }
  } else if (m_path[0] != '/' && m_matchType != MATCH_EXACT) {
    std::ostringstream oss;
    oss << "Location path must start with '/' for prefix matching: '" << m_path
        << "'";
    throw exceptions::LocationConfigException(
        oss.str(), exceptions::LocationConfigException::INVALID_PATH_FORMAT);
  }
}

void LocationConfig::validateRoot() const {
  if (m_root.isEmpty()) {
    throw exceptions::LocationConfigException(
        "Root path cannot be empty",
        exceptions::LocationConfigException::EMPTY_ROOT);
  }
}

void LocationConfig::validateAllowedMethods() const {
  if (m_allowedMethods.empty()) {
    throw exceptions::LocationConfigException(
        "Location must have at least one allowed HTTP method",
        exceptions::LocationConfigException::NO_ALLOWED_METHODS);
  }
}

void LocationConfig::validateTryFiles() const {
  if (!m_tryFiles.empty()) {
    for (std::size_t i = 0; i < m_tryFiles.size(); ++i) {
      if (m_tryFiles[i].empty()) {
        throw exceptions::LocationConfigException(
            "Try files entry cannot be empty",
            exceptions::LocationConfigException::EMPTY_TRY_FILE);
      }
    }
  }
}

void LocationConfig::validateReturnRedirect() const {
  if (!m_returnRedirect.isEmpty() && !m_returnCode.isRedirection()) {
    std::ostringstream oss;
    oss << "Return code must be a redirection code (3xx) for return directive: "
        << m_returnCode.getValue();
    throw exceptions::LocationConfigException(
        oss.str(), exceptions::LocationConfigException::INVALID_REDIRECT_CODE);
  }
}

void LocationConfig::validateUploadConfig() const {
  if (m_hasUploadConfig) {
    const filesystem::value_objects::Path& uploadDir =
        m_uploadConfig.getUploadDirectory();

    if (uploadDir.isEmpty()) {
      throw exceptions::LocationConfigException(
          "Upload directory path cannot be empty",
          exceptions::LocationConfigException::INVALID_UPLOAD_CONFIG);
    }
  }
}

void LocationConfig::validateCgiConfig() const {
  if (!m_cgiConfig.isValid()) {
    throw exceptions::LocationConfigException(
        "Invalid CGI configuration",
        exceptions::LocationConfigException::INVALID_CGI_CONFIG);
  }
}

void LocationConfig::validateErrorPages() const {
  for (ErrorPageMap::const_iterator it = m_errorPages.begin();
       it != m_errorPages.end(); ++it) {
    const std::string& uri = it->second;

    if (uri.empty() || uri[0] != '/') {
      std::ostringstream oss;
      oss << "Invalid error page URI for error " << it->first.getValue()
          << ": '" << uri << "' (must start with '/')";
      throw exceptions::LocationConfigException(
          oss.str(),
          exceptions::LocationConfigException::INVALID_ERROR_PAGE_URI);
    }
  }
}

void LocationConfig::validateClientMaxBodySize() const {
  const filesystem::value_objects::Size MAX_BODY_SIZE =
      filesystem::value_objects::Size::fromMegabytes(
          MAX_ALLOWED_CLIENT_BODY_SIZE);

  if (m_clientMaxBodySize.getBytes() > MAX_BODY_SIZE.getBytes()) {
    std::ostringstream oss;
    oss << "Client max body size exceeds limit: "
        << m_clientMaxBodySize.toString() << " (maximum is "
        << MAX_BODY_SIZE.toString() << ")";
    throw exceptions::LocationConfigException(
        oss.str(), exceptions::LocationConfigException::BODY_SIZE_TOO_LARGE);
  }
}

void LocationConfig::validateProxyPass() const {
  if (!m_proxyPass.isEmpty() && !m_proxyPass.isHttp() &&
      !m_proxyPass.isHttps()) {
    throw exceptions::LocationConfigException(
        "Proxy pass must be an HTTP or HTTPS URL",
        exceptions::LocationConfigException::INVALID_PROXY_PASS);
  }
}

void LocationConfig::validateAlias() const {
  if (!m_alias.isEmpty() && !m_alias.isAbsolute()) {
    throw exceptions::LocationConfigException(
        "Alias path must be absolute",
        exceptions::LocationConfigException::INVALID_ROOT_PATH);
  }
}

void LocationConfig::validateClientBodyBufferSize() const {
  const filesystem::value_objects::Size MAX_BUFFER_SIZE =
      filesystem::value_objects::Size::fromMegabytes(MAX_ALLOWED_BUFFER_SIZE);

  if (m_clientBodyBufferSize.getBytes() > MAX_BUFFER_SIZE.getBytes()) {
    std::ostringstream oss;
    oss << "Client body buffer size too large: "
        << m_clientBodyBufferSize.toString() << " (maximum is "
        << MAX_BUFFER_SIZE.toString() << ")";
    throw exceptions::LocationConfigException(
        oss.str(), exceptions::LocationConfigException::BODY_SIZE_TOO_LARGE);
  }
}

const LocationConfig::CustomHeaderMap& LocationConfig::getCustomHeaders()
    const {
  return m_customHeaders;
}

bool LocationConfig::hasCustomHeaders() const {
  return !m_customHeaders.empty();
}

void LocationConfig::addCustomHeader(const std::string& name,
                                     const std::string& value) {
  if (name.empty()) {
    throw exceptions::LocationConfigException(
        "Custom header name cannot be empty",
        exceptions::LocationConfigException::INVALID_CUSTOM_HEADER);
  }

  if (value.empty()) {
    throw exceptions::LocationConfigException(
        "Custom header value cannot be empty for header '" + name + "'",
        exceptions::LocationConfigException::INVALID_CUSTOM_HEADER);
  }

  // Validate header name format (RFC 7230)
  for (std::size_t i = 0; i < name.length(); ++i) {
    char c = name[i];
    bool isValidChar = (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
                       (c >= '0' && c <= '9') || c == '-' || c == '_';

    if (!isValidChar) {
      std::ostringstream oss;
      oss << "Invalid character in header name '" << name
          << "': only alphanumeric, hyphen, and underscore allowed";
      throw exceptions::LocationConfigException(
          oss.str(),
          exceptions::LocationConfigException::INVALID_CUSTOM_HEADER);
    }
  }

  // Check for reserved/dangerous headers
  std::string lowerName = name;
  for (std::size_t i = 0; i < lowerName.length(); ++i) {
    if (lowerName[i] >= 'A' && lowerName[i] <= 'Z') {
      lowerName[i] = lowerName[i] + ('a' - 'A');
    }
  }

  if (lowerName == "content-length" || lowerName == "transfer-encoding" ||
      lowerName == "connection" || lowerName == "host") {
    std::ostringstream oss;
    oss << "Cannot set reserved header: '" << name << "'";
    throw exceptions::LocationConfigException(
        oss.str(), exceptions::LocationConfigException::RESERVED_HEADER);
  }

  m_customHeaders[name] = value;

  std::ostringstream oss;
  oss << "Added custom header '" << name << "' with value '" << value << "'";
  // If you have a logger, log here: m_logger.debug(oss.str());
}

void LocationConfig::removeCustomHeader(const std::string& name) {
  CustomHeaderMap::iterator it = m_customHeaders.find(name);
  if (it != m_customHeaders.end()) {
    m_customHeaders.erase(it);
  }
}

void LocationConfig::clearCustomHeaders() { m_customHeaders.clear(); }

bool LocationConfig::matchesPath(const std::string& requestPath) const {
  switch (m_matchType) {
    case MATCH_EXACT:
      return requestPath == m_path;

    case MATCH_PREFIX:
      return requestPath.find(m_path) == 0;

    case MATCH_REGEX_CASE_SENSITIVE:
    case MATCH_REGEX_CASE_INSENSITIVE:
      if (!m_regexPatternValid) {
        compileRegexPattern();
      }
      return m_regexPattern.matches(requestPath);

    default:
      return false;
  }
}

bool LocationConfig::isMethodAllowed(
    const http::value_objects::HttpMethod& method) const {
  return m_allowedMethods.find(method) != m_allowedMethods.end();
}

bool LocationConfig::isUploadEnabled() const { return m_hasUploadConfig; }

bool LocationConfig::hasCgiConfig() const { return m_cgiConfig.isValid(); }

bool LocationConfig::hasAlias() const { return !m_alias.isEmpty(); }

bool LocationConfig::hasClientBodyBufferSize() const {
  return m_clientBodyBufferSizeSet;
}

bool LocationConfig::allowsGet() const {
  return isMethodAllowed(http::value_objects::HttpMethod::get());
}

bool LocationConfig::allowsPost() const {
  return isMethodAllowed(http::value_objects::HttpMethod::post());
}

bool LocationConfig::allowsDelete() const {
  return isMethodAllowed(http::value_objects::HttpMethod::deleteMethod());
}

bool LocationConfig::allowsPut() const {
  return isMethodAllowed(http::value_objects::HttpMethod::put());
}

bool LocationConfig::allowsHead() const {
  return isMethodAllowed(http::value_objects::HttpMethod::head());
}

bool LocationConfig::isUploadRoute() const {
  return isUploadEnabled() && allowsPost();
}

filesystem::value_objects::Path LocationConfig::resolvePath(
    const std::string& requestPath) const {
  if (hasAlias()) {
    std::string relativePath = requestPath.substr(m_path.length());
    return m_alias.join(relativePath);
  }
  return m_root.join(requestPath);
}

bool LocationConfig::validateUploadFile(
    const std::string& filename,
    const filesystem::value_objects::Size& fileSize,
    const std::string& mimeType) const {
  if (!isUploadEnabled()) {
    return false;
  }

  if (!m_uploadConfig.validateFileSize(fileSize)) {
    return false;
  }

  if (!m_uploadConfig.validateFilename(filename)) {
    return false;
  }

  if (!m_uploadConfig.validateExtension(filename)) {
    return false;
  }

  if (!mimeType.empty() && !m_uploadConfig.validateMimeType(mimeType)) {
    return false;
  }

  return true;
}

bool LocationConfig::canHandleUpload() const {
  return isUploadEnabled() && allowsPost();
}

value_objects::Route LocationConfig::toRoute() const {
  value_objects::Route::HandlerType handlerType =
      value_objects::Route::STATIC_FILE;

  if (hasCgiConfig()) {
    handlerType = value_objects::Route::CGI_EXECUTION;
  } else if (isUploadEnabled()) {
    handlerType = value_objects::Route::UPLOAD;
  } else if (hasReturnRedirect()) {
    handlerType = value_objects::Route::REDIRECT;
  }

  value_objects::Route route(filesystem::value_objects::Path(m_path),
                             m_allowedMethods, handlerType);

  if (!m_root.isEmpty()) {
    route.setRootDirectory(m_root.toString());
  }

  if (!m_indexFiles.empty()) {
    route.setIndexFile(m_indexFiles.front());
  }

  if (isUploadEnabled()) {
    route.setUploadDirectory(m_uploadConfig.getUploadDirectory().toString());
  }

  if (hasReturnRedirect()) {
    route.setRedirect(m_returnRedirect.toString(), m_returnCode);
  }

  route.setDirectoryListing(m_autoIndex);
  route.setMaxBodySize(m_clientMaxBodySize);

  return route;
}

void LocationConfig::clear() {
  m_path = "/";
  m_matchType = MATCH_PREFIX;
  m_root = filesystem::value_objects::Path::rootDirectory();
  m_indexFiles.clear();
  m_allowedMethods.clear();
  m_autoIndex = false;
  m_tryFiles.clear();
  m_returnRedirect = http::value_objects::Uri();
  m_returnCode = shared::value_objects::ErrorCode::movedPermanently();
  m_uploadConfig = value_objects::UploadConfig(
      filesystem::value_objects::Path("/tmp/uploads"));
  m_hasUploadConfig = false;
  m_cgiConfig = value_objects::CgiConfig();
  m_errorPages.clear();
  m_clientMaxBodySize = filesystem::value_objects::Size::fromMegabytes(
      DEFAULT_CLIENT_MAX_BODY_SIZE);
  m_proxyPass = http::value_objects::Uri();
  m_alias = filesystem::value_objects::Path();
  m_clientBodyBufferSize = filesystem::value_objects::Size::fromKilobytes(
      DEFAULT_CLIENT_BODY_BUFFER_SIZE);
  m_clientBodyBufferSizeSet = false;
  m_customHeaders.clear();
  m_regexPatternValid = false;

  m_indexFiles.push_back("index.html");
  m_indexFiles.push_back("index.htm");

  m_allowedMethods.insert(http::value_objects::HttpMethod::get());
  m_allowedMethods.insert(http::value_objects::HttpMethod::post());
  m_allowedMethods.insert(http::value_objects::HttpMethod::deleteMethod());
  m_allowedMethods.insert(http::value_objects::HttpMethod::head());
}

void LocationConfig::compileRegexPattern() const {
  if (m_matchType == MATCH_REGEX_CASE_SENSITIVE ||
      m_matchType == MATCH_REGEX_CASE_INSENSITIVE) {
    try {
      int flags = shared::value_objects::RegexPattern::FLAG_NONE;
      if (m_matchType == MATCH_REGEX_CASE_INSENSITIVE) {
        flags |= shared::value_objects::RegexPattern::FLAG_CASE_INSENSITIVE;
      }

      m_regexPattern = shared::value_objects::RegexPattern(m_path, flags);
      m_regexPatternValid = true;
    } catch (const std::exception& e) {
      m_regexPatternValid = false;
      throw exceptions::LocationConfigException(
          std::string("Failed to compile regex pattern: ") + e.what(),
          exceptions::LocationConfigException::REGEX_COMPILATION_FAILED);
    }
  }
}

bool LocationConfig::shouldUseAlias() const { return !m_alias.isEmpty(); }

LocationConfig::LocationMatchType LocationConfig::parseMatchType(
    const std::string& path) {
  if (path.empty()) {
    return MATCH_PREFIX;
  }

  if (path[0] == '@') {
    return MATCH_EXACT;
  }

  if (path[0] == '=') {
    return MATCH_EXACT;
  }
  if (path[0] == '~') {
    if (path.length() > 1 && path[1] == '*') {
      return MATCH_REGEX_CASE_INSENSITIVE;
    }
    return MATCH_REGEX_CASE_SENSITIVE;
  }
  if (path[0] == '^' && path.length() > 1 && path[1] == '~') {
    return MATCH_REGEX_CASE_SENSITIVE;
  }

  return MATCH_PREFIX;
}

std::string LocationConfig::stripMatchPrefix(const std::string& path) {
  LocationMatchType type = parseMatchType(path);

  switch (type) {
    case MATCH_EXACT:
      if (path[0] == '@') {
        return path;  // Keep the @
      }
      return path.substr(1);

    case MATCH_REGEX_CASE_SENSITIVE:
      if (path[0] == '~') {
        return path.substr(1);
      } else if (path[0] == '^' && path.length() > 1 && path[1] == '~') {
        return path.substr(2);
      }
      return path;

    case MATCH_REGEX_CASE_INSENSITIVE:
      if (path.length() > 1 && path[1] == '*') {
        return path.substr(2);
      }
      return path.substr(1);

    case MATCH_PREFIX:
    default:
      return path;
  }
}

}  // namespace entities
}  // namespace configuration
}  // namespace domain
