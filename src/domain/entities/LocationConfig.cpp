/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationConfig.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/22 12:23:41 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/25 21:29:28 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "domain/entities/LocationConfig.hpp"
#include "shared/exceptions/LocationConfigException.hpp"

#include <cctype>
#include <cstring>
#include <sstream>

namespace domain {
namespace entities {

LocationConfig::LocationConfig()
    : m_path("/"),
      m_matchType(MATCH_PREFIX),
      m_root(value_objects::Path::rootDirectory()),
      m_autoIndex(false),
      m_returnCode(value_objects::ErrorCode::movedPermanently()),
      m_uploadConfig(value_objects::Path("/tmp/uploads")),
      m_hasUploadConfig(false),
      m_clientMaxBodySize(
          value_objects::Size::fromMegabytes(DEFAULT_CLIENT_MAX_BODY_SIZE)),
      m_clientBodyBufferSize(
          value_objects::Size::fromKilobytes(DEFAULT_CLIENT_BODY_BUFFER_SIZE)),
      m_clientBodyBufferSizeSet(false),
      m_regexPatternValid(false) {
  m_allowedMethods.insert(value_objects::HttpMethod::get());
  m_allowedMethods.insert(value_objects::HttpMethod::post());
  m_allowedMethods.insert(value_objects::HttpMethod::deleteMethod());
  m_allowedMethods.insert(value_objects::HttpMethod::head());

  m_indexFiles.push_back("index.html");
  m_indexFiles.push_back("index.htm");
}

LocationConfig::LocationConfig(const std::string& path,
                               LocationMatchType matchType)
    : m_path(path),
      m_matchType(matchType),
      m_root(value_objects::Path::rootDirectory()),
      m_autoIndex(false),
      m_returnCode(value_objects::ErrorCode::movedPermanently()),
      m_uploadConfig(value_objects::Path("/tmp/uploads")),
      m_hasUploadConfig(false),
      m_clientMaxBodySize(
          value_objects::Size::fromMegabytes(DEFAULT_CLIENT_MAX_BODY_SIZE)),
      m_clientBodyBufferSize(
          value_objects::Size::fromKilobytes(DEFAULT_CLIENT_BODY_BUFFER_SIZE)),
      m_clientBodyBufferSizeSet(false),
      m_regexPatternValid(false) {
  validatePath();

  m_allowedMethods.insert(value_objects::HttpMethod::get());
  m_allowedMethods.insert(value_objects::HttpMethod::post());
  m_allowedMethods.insert(value_objects::HttpMethod::deleteMethod());
  m_allowedMethods.insert(value_objects::HttpMethod::head());

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
      m_uploadConfig(other.m_uploadConfig),
      m_hasUploadConfig(other.m_hasUploadConfig),
      m_cgiConfig(other.m_cgiConfig),
      m_errorPages(other.m_errorPages),
      m_clientMaxBodySize(other.m_clientMaxBodySize),
      m_proxyPass(other.m_proxyPass),
      m_alias(other.m_alias),
      m_clientBodyBufferSize(other.m_clientBodyBufferSize),
      m_clientBodyBufferSizeSet(other.m_clientBodyBufferSizeSet) {
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
    m_uploadConfig = other.m_uploadConfig;
    m_hasUploadConfig = other.m_hasUploadConfig;
    m_cgiConfig = other.m_cgiConfig;
    m_errorPages = other.m_errorPages;
    m_clientMaxBodySize = other.m_clientMaxBodySize;
    m_proxyPass = other.m_proxyPass;
    m_alias = other.m_alias;
    m_clientBodyBufferSize = other.m_clientBodyBufferSize;
    m_clientBodyBufferSizeSet = other.m_clientBodyBufferSizeSet;
    m_regexPatternValid = false;
  }
  return *this;
}

const std::string& LocationConfig::getPath() const { return m_path; }

LocationConfig::LocationMatchType LocationConfig::getMatchType() const {
  return m_matchType;
}

const value_objects::Path& LocationConfig::getRoot() const { return m_root; }

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

const value_objects::Uri& LocationConfig::getReturnRedirect() const {
  return m_returnRedirect;
}

const value_objects::ErrorCode& LocationConfig::getReturnCode() const {
  return m_returnCode;
}

bool LocationConfig::hasUploadConfig() const { return m_hasUploadConfig; }

const UploadConfig& LocationConfig::getUploadConfig() const {
  return m_uploadConfig;
}

UploadConfig& LocationConfig::getUploadConfigMutable() {
  return m_uploadConfig;
}

const CgiConfig& LocationConfig::getCgiConfig() const { return m_cgiConfig; }

const LocationConfig::ErrorPageMap& LocationConfig::getErrorPages() const {
  return m_errorPages;
}

const value_objects::Size& LocationConfig::getClientMaxBodySize() const {
  return m_clientMaxBodySize;
}

bool LocationConfig::hasProxyPass() const { return !m_proxyPass.isEmpty(); }

const value_objects::Uri& LocationConfig::getProxyPass() const {
  return m_proxyPass;
}

const value_objects::Path& LocationConfig::getAlias() const { return m_alias; }

bool LocationConfig::getClientBodyBufferSizeSet() const {
  return m_clientBodyBufferSizeSet;
}

const value_objects::Size& LocationConfig::getClientBodyBufferSize() const {
  return m_clientBodyBufferSize;
}

void LocationConfig::setPath(const std::string& path,
                             LocationMatchType matchType) {
  m_path = path;
  m_matchType = matchType;
  m_regexPatternValid = false;
  validatePath();
}

void LocationConfig::setRoot(const value_objects::Path& root) { m_root = root; }

void LocationConfig::setRoot(const std::string& root) {
  try {
    m_root = value_objects::Path::fromString(root, true);
  } catch (const std::exception& e) {
    std::ostringstream oss;
    oss << "Invalid root path: " << e.what();
    throw shared::exceptions::LocationConfigException(
        oss.str(),
        shared::exceptions::LocationConfigException::INVALID_ROOT_PATH);
  }
}

void LocationConfig::addIndexFile(const std::string& index) {
  if (index.empty()) {
    throw shared::exceptions::LocationConfigException(
        "Index file cannot be empty",
        shared::exceptions::LocationConfigException::EMPTY_INDEX_FILE);
  }

  for (std::size_t i = 0; i < m_indexFiles.size(); ++i) {
    if (m_indexFiles[i] == index) {
      std::ostringstream oss;
      oss << "Duplicate index file: '" << index << "'";
      throw shared::exceptions::LocationConfigException(
          oss.str(),
          shared::exceptions::LocationConfigException::DUPLICATE_INDEX_FILE);
    }
  }

  m_indexFiles.push_back(index);
}

void LocationConfig::clearIndexFiles() { m_indexFiles.clear(); }

void LocationConfig::addAllowedMethod(const value_objects::HttpMethod& method) {
  m_allowedMethods.insert(method);
}

void LocationConfig::removeAllowedMethod(
    const value_objects::HttpMethod& method) {
  m_allowedMethods.erase(method);
}

void LocationConfig::setAutoIndex(bool autoIndex) { m_autoIndex = autoIndex; }

void LocationConfig::setTryFiles(const TryFiles& tryFiles) {
  m_tryFiles = tryFiles;
  validateTryFiles();
}

void LocationConfig::addTryFile(const std::string& tryFile) {
  if (tryFile.empty()) {
    throw shared::exceptions::LocationConfigException(
        "Try files entry cannot be empty",
        shared::exceptions::LocationConfigException::EMPTY_TRY_FILE);
  }
  m_tryFiles.push_back(tryFile);
}

void LocationConfig::setReturnRedirect(const value_objects::Uri& redirect,
                                       const value_objects::ErrorCode& code) {
  if (redirect.isEmpty()) {
    throw shared::exceptions::LocationConfigException(
        "Return redirect URL cannot be empty",
        shared::exceptions::LocationConfigException::EMPTY_REDIRECT_URL);
  }

  if (!code.isRedirection() && !code.isError()) {
    std::ostringstream oss;
    oss << "Invalid redirect/error code: " << code.getValue()
        << " (must be a redirection or error code)";
    throw shared::exceptions::LocationConfigException(
        oss.str(),
        shared::exceptions::LocationConfigException::INVALID_REDIRECT_CODE);
  }

  m_returnRedirect = redirect;
  m_returnCode = code;
}

void LocationConfig::setReturnRedirect(const std::string& redirect,
                                       unsigned int code) {
  try {
    value_objects::ErrorCode errorCode(code);
    value_objects::Uri redirectUri(redirect);
    setReturnRedirect(redirectUri, errorCode);
  } catch (const std::exception& e) {
    std::ostringstream oss;
    oss << "Invalid redirect: " << e.what();
    throw shared::exceptions::LocationConfigException(
        oss.str(),
        shared::exceptions::LocationConfigException::INVALID_REDIRECT_CODE);
  }
}

void LocationConfig::setUploadConfig(const UploadConfig& config) {
  m_uploadConfig = config;
  m_hasUploadConfig = true;
  validateUploadConfig();
}

void LocationConfig::enableUpload(const value_objects::Path& uploadDirectory) {
  m_uploadConfig = UploadConfig(uploadDirectory);
  m_hasUploadConfig = true;
  validateUploadConfig();
}

void LocationConfig::enableUpload(const value_objects::Path& uploadDirectory,
                                  const value_objects::Size& maxFileSize,
                                  const value_objects::Size& maxTotalSize) {
  m_uploadConfig = UploadConfig(uploadDirectory, maxFileSize, maxTotalSize);
  m_hasUploadConfig = true;
  validateUploadConfig();
}

void LocationConfig::disableUpload() {
  m_hasUploadConfig = false;
  m_uploadConfig = UploadConfig(value_objects::Path("/tmp/uploads"));
}

void LocationConfig::setCgiConfig(const CgiConfig& config) {
  if (!config.isValid()) {
    throw shared::exceptions::LocationConfigException(
        "Invalid CGI configuration",
        shared::exceptions::LocationConfigException::INVALID_CGI_CONFIG);
  }

  m_cgiConfig = config;
}

void LocationConfig::clearCgiConfig() { m_cgiConfig = CgiConfig(); }

void LocationConfig::addErrorPage(const value_objects::ErrorCode& code,
                                  const std::string& uri) {
  if (!code.isError()) {
    std::ostringstream oss;
    oss << "Invalid error code for error page: " << code.getValue()
        << " (must be an error code between 400 and 599)";
    throw shared::exceptions::LocationConfigException(
        oss.str(),
        shared::exceptions::LocationConfigException::INVALID_ERROR_CODE);
  }

  if (uri.empty()) {
    std::ostringstream oss;
    oss << "Error page URI cannot be empty for error code " << code.getValue();
    throw shared::exceptions::LocationConfigException(
        oss.str(),
        shared::exceptions::LocationConfigException::EMPTY_ERROR_PAGE_URI);
  }

  if (uri[0] != '/') {
    std::ostringstream oss;
    oss << "Error page URI must start with '/': " << uri << " for error code "
        << code.getValue();
    throw shared::exceptions::LocationConfigException(
        oss.str(),
        shared::exceptions::LocationConfigException::INVALID_ERROR_PAGE_URI);
  }

  m_errorPages[code] = uri;
}

void LocationConfig::removeErrorPage(const value_objects::ErrorCode& code) {
  m_errorPages.erase(code);
}

void LocationConfig::setClientMaxBodySize(const value_objects::Size& size) {
  const value_objects::Size MAX_BODY_SIZE =
      value_objects::Size::fromMegabytes(MAX_ALLOWED_CLIENT_BODY_SIZE);

  if (size.getBytes() > MAX_BODY_SIZE.getBytes()) {
    std::ostringstream oss;
    oss << "Client max body size too large: " << size.toString()
        << " (maximum is " << MAX_BODY_SIZE.toString() << ")";
    throw shared::exceptions::LocationConfigException(
        oss.str(),
        shared::exceptions::LocationConfigException::BODY_SIZE_TOO_LARGE);
  }

  m_clientMaxBodySize = size;
}

void LocationConfig::setClientMaxBodySize(const std::string& sizeString) {
  try {
    value_objects::Size size = value_objects::Size::fromString(sizeString);
    setClientMaxBodySize(size);
  } catch (const std::exception& e) {
    std::ostringstream oss;
    oss << "Invalid client max body size: " << e.what();
    throw shared::exceptions::LocationConfigException(
        oss.str(),
        shared::exceptions::LocationConfigException::BODY_SIZE_TOO_LARGE);
  }
}

void LocationConfig::setProxyPass(const value_objects::Uri& proxyPass) {
  if (proxyPass.isEmpty()) {
    throw shared::exceptions::LocationConfigException(
        "Proxy pass URL cannot be empty",
        shared::exceptions::LocationConfigException::EMPTY_PROXY_PASS);
  }

  if (!proxyPass.isHttp() && !proxyPass.isHttps()) {
    throw shared::exceptions::LocationConfigException(
        "Proxy pass must be an HTTP or HTTPS URL: " + proxyPass.toString(),
        shared::exceptions::LocationConfigException::INVALID_PROXY_PASS);
  }

  m_proxyPass = proxyPass;
}

void LocationConfig::setProxyPass(const std::string& proxyPass) {
  try {
    value_objects::Uri uri(proxyPass);
    setProxyPass(uri);
  } catch (const std::exception& e) {
    std::ostringstream oss;
    oss << "Invalid proxy pass URL: " << e.what();
    throw shared::exceptions::LocationConfigException(
        oss.str(),
        shared::exceptions::LocationConfigException::INVALID_PROXY_PASS);
  }
}

void LocationConfig::clearProxyPass() { m_proxyPass = value_objects::Uri(); }

void LocationConfig::setAlias(const value_objects::Path& alias) {
  if (!alias.isEmpty() && !alias.isAbsolute()) {
    throw shared::exceptions::LocationConfigException(
        "Alias path must be absolute",
        shared::exceptions::LocationConfigException::INVALID_ROOT_PATH);
  }
  m_alias = alias;
}

void LocationConfig::setAlias(const std::string& alias) {
  try {
    value_objects::Path path = value_objects::Path::fromString(alias, true);
    setAlias(path);
  } catch (const std::exception& e) {
    std::ostringstream oss;
    oss << "Invalid alias path: " << e.what();
    throw shared::exceptions::LocationConfigException(
        oss.str(),
        shared::exceptions::LocationConfigException::INVALID_ROOT_PATH);
  }
}

void LocationConfig::setClientBodyBufferSize(const value_objects::Size& size) {
  m_clientBodyBufferSize = size;
  m_clientBodyBufferSizeSet = true;
}

void LocationConfig::setClientBodyBufferSize(const std::string& sizeString) {
  try {
    value_objects::Size size = value_objects::Size::fromString(sizeString);
    setClientBodyBufferSize(size);
  } catch (const std::exception& e) {
    std::ostringstream oss;
    oss << "Invalid client body buffer size: " << e.what();
    throw shared::exceptions::LocationConfigException(
        oss.str(),
        shared::exceptions::LocationConfigException::BODY_SIZE_TOO_LARGE);
  }
}

bool LocationConfig::isValid() const {
  try {
    validate();
    return true;
  } catch (const shared::exceptions::LocationConfigException&) {
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
    throw shared::exceptions::LocationConfigException(
        "Proxy pass cannot be combined with upload, CGI, or return directives",
        shared::exceptions::LocationConfigException::CONFLICTING_DIRECTIVES);
  }

  if (hasReturnRedirect() &&
      (isUploadEnabled() || hasCgiConfig() || hasProxyPass())) {
    throw shared::exceptions::LocationConfigException(
        "Return redirect cannot be combined with upload, CGI, or proxy pass "
        "directives",
        shared::exceptions::LocationConfigException::CONFLICTING_DIRECTIVES);
  }

  if (hasAlias() && !m_root.isEmpty()) {
    throw shared::exceptions::LocationConfigException(
        "Alias and root directives cannot be used together",
        shared::exceptions::LocationConfigException::CONFLICTING_DIRECTIVES);
  }

  if (isUploadEnabled() && !allowsPost()) {
    throw shared::exceptions::LocationConfigException(
        "Upload enabled but POST method not allowed",
        shared::exceptions::LocationConfigException::CONFLICTING_DIRECTIVES);
  }
}

void LocationConfig::validatePath() const {
  if (m_path.empty()) {
    throw shared::exceptions::LocationConfigException(
        "Location path cannot be empty",
        shared::exceptions::LocationConfigException::EMPTY_PATH);
  }

  if (m_matchType == MATCH_REGEX_CASE_SENSITIVE ||
      m_matchType == MATCH_REGEX_CASE_INSENSITIVE) {
    if (!value_objects::RegexPattern::isValidPattern(m_path)) {
      std::ostringstream oss;
      oss << "Invalid regex pattern for location: '" << m_path << "'";
      throw shared::exceptions::LocationConfigException(
          oss.str(),
          shared::exceptions::LocationConfigException::INVALID_REGEX_PATTERN);
    }
  } else if (m_path[0] != '/' && m_matchType != MATCH_EXACT) {
    std::ostringstream oss;
    oss << "Location path must start with '/' for prefix matching: '" << m_path
        << "'";
    throw shared::exceptions::LocationConfigException(
        oss.str(),
        shared::exceptions::LocationConfigException::INVALID_PATH_FORMAT);
  }
}

void LocationConfig::validateRoot() const {
  if (m_root.isEmpty()) {
    throw shared::exceptions::LocationConfigException(
        "Root path cannot be empty",
        shared::exceptions::LocationConfigException::EMPTY_ROOT);
  }
}

void LocationConfig::validateAllowedMethods() const {
  if (m_allowedMethods.empty()) {
    throw shared::exceptions::LocationConfigException(
        "Location must have at least one allowed HTTP method",
        shared::exceptions::LocationConfigException::NO_ALLOWED_METHODS);
  }
}

void LocationConfig::validateTryFiles() const {
  if (!m_tryFiles.empty()) {
    for (std::size_t i = 0; i < m_tryFiles.size(); ++i) {
      if (m_tryFiles[i].empty()) {
        throw shared::exceptions::LocationConfigException(
            "Try files entry cannot be empty",
            shared::exceptions::LocationConfigException::EMPTY_TRY_FILE);
      }
    }
  }
}

void LocationConfig::validateReturnRedirect() const {
  if (!m_returnRedirect.isEmpty() && !m_returnCode.isRedirection()) {
    std::ostringstream oss;
    oss << "Return code must be a redirection code (3xx) for return directive: "
        << m_returnCode.getValue();
    throw shared::exceptions::LocationConfigException(
        oss.str(),
        shared::exceptions::LocationConfigException::INVALID_REDIRECT_CODE);
  }
}

void LocationConfig::validateUploadConfig() const {
  if (m_hasUploadConfig && !m_uploadConfig.validateUploadDirectory()) {
    throw shared::exceptions::LocationConfigException(
        "Invalid upload directory",
        shared::exceptions::LocationConfigException::INVALID_UPLOAD_CONFIG);
  }
}

void LocationConfig::validateCgiConfig() const {
  if (!m_cgiConfig.isValid()) {
    throw shared::exceptions::LocationConfigException(
        "Invalid CGI configuration",
        shared::exceptions::LocationConfigException::INVALID_CGI_CONFIG);
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
      throw shared::exceptions::LocationConfigException(
          oss.str(),
          shared::exceptions::LocationConfigException::INVALID_ERROR_PAGE_URI);
    }
  }
}

void LocationConfig::validateClientMaxBodySize() const {
  const value_objects::Size MAX_BODY_SIZE =
      value_objects::Size::fromMegabytes(MAX_ALLOWED_CLIENT_BODY_SIZE);

  if (m_clientMaxBodySize.getBytes() > MAX_BODY_SIZE.getBytes()) {
    std::ostringstream oss;
    oss << "Client max body size exceeds limit: "
        << m_clientMaxBodySize.toString() << " (maximum is "
        << MAX_BODY_SIZE.toString() << ")";
    throw shared::exceptions::LocationConfigException(
        oss.str(),
        shared::exceptions::LocationConfigException::BODY_SIZE_TOO_LARGE);
  }
}

void LocationConfig::validateProxyPass() const {
  if (!m_proxyPass.isEmpty() && !m_proxyPass.isHttp() &&
      !m_proxyPass.isHttps()) {
    throw shared::exceptions::LocationConfigException(
        "Proxy pass must be an HTTP or HTTPS URL",
        shared::exceptions::LocationConfigException::INVALID_PROXY_PASS);
  }
}

void LocationConfig::validateAlias() const {
  if (!m_alias.isEmpty() && !m_alias.isAbsolute()) {
    throw shared::exceptions::LocationConfigException(
        "Alias path must be absolute",
        shared::exceptions::LocationConfigException::INVALID_ROOT_PATH);
  }
}

void LocationConfig::validateClientBodyBufferSize() const {
  const value_objects::Size MAX_BUFFER_SIZE =
      value_objects::Size::fromMegabytes(MAX_ALLOWED_BUFFER_SIZE);

  if (m_clientBodyBufferSize.getBytes() > MAX_BUFFER_SIZE.getBytes()) {
    std::ostringstream oss;
    oss << "Client body buffer size too large: "
        << m_clientBodyBufferSize.toString() << " (maximum is "
        << MAX_BUFFER_SIZE.toString() << ")";
    throw shared::exceptions::LocationConfigException(
        oss.str(),
        shared::exceptions::LocationConfigException::BODY_SIZE_TOO_LARGE);
  }
}

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
    const value_objects::HttpMethod& method) const {
  return m_allowedMethods.find(method) != m_allowedMethods.end();
}

bool LocationConfig::isUploadEnabled() const { return m_hasUploadConfig; }

bool LocationConfig::hasCgiConfig() const { return m_cgiConfig.isValid(); }

bool LocationConfig::hasAlias() const { return !m_alias.isEmpty(); }

bool LocationConfig::hasClientBodyBufferSize() const {
  return m_clientBodyBufferSizeSet;
}

bool LocationConfig::allowsGet() const {
  return isMethodAllowed(value_objects::HttpMethod::get());
}

bool LocationConfig::allowsPost() const {
  return isMethodAllowed(value_objects::HttpMethod::post());
}

bool LocationConfig::allowsDelete() const {
  return isMethodAllowed(value_objects::HttpMethod::deleteMethod());
}

bool LocationConfig::allowsPut() const {
  return isMethodAllowed(value_objects::HttpMethod::put());
}

bool LocationConfig::allowsHead() const {
  return isMethodAllowed(value_objects::HttpMethod::head());
}

bool LocationConfig::isUploadRoute() const {
  return isUploadEnabled() && allowsPost();
}

value_objects::Path LocationConfig::resolvePath(
    const std::string& requestPath) const {
  if (hasAlias()) {
    std::string relativePath = requestPath.substr(m_path.length());
    return m_alias.join(relativePath);
  }
  return m_root.join(requestPath);
}

bool LocationConfig::validateUploadFile(const std::string& filename,
                                        const value_objects::Size& fileSize,
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

Route LocationConfig::toRoute() const {
  Route::HandlerType handlerType = Route::STATIC_FILE;

  if (hasCgiConfig()) {
    handlerType = Route::CGI_EXECUTION;
  } else if (isUploadEnabled()) {
    handlerType = Route::UPLOAD;
  } else if (hasReturnRedirect()) {
    handlerType = Route::REDIRECT;
  }

  Route route(value_objects::Path(m_path), m_allowedMethods, handlerType);

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
  m_root = value_objects::Path::rootDirectory();
  m_indexFiles.clear();
  m_allowedMethods.clear();
  m_autoIndex = false;
  m_tryFiles.clear();
  m_returnRedirect = value_objects::Uri();
  m_returnCode = value_objects::ErrorCode::movedPermanently();
  m_uploadConfig = UploadConfig(value_objects::Path("/tmp/uploads"));
  m_hasUploadConfig = false;
  m_cgiConfig = CgiConfig();
  m_errorPages.clear();
  m_clientMaxBodySize =
      value_objects::Size::fromMegabytes(DEFAULT_CLIENT_MAX_BODY_SIZE);
  m_proxyPass = value_objects::Uri();
  m_alias = value_objects::Path();
  m_clientBodyBufferSize =
      value_objects::Size::fromKilobytes(DEFAULT_CLIENT_BODY_BUFFER_SIZE);
  m_clientBodyBufferSizeSet = false;
  m_regexPatternValid = false;

  m_indexFiles.push_back("index.html");
  m_indexFiles.push_back("index.htm");

  m_allowedMethods.insert(value_objects::HttpMethod::get());
  m_allowedMethods.insert(value_objects::HttpMethod::post());
  m_allowedMethods.insert(value_objects::HttpMethod::deleteMethod());
  m_allowedMethods.insert(value_objects::HttpMethod::head());
}

void LocationConfig::compileRegexPattern() const {
  if (m_matchType == MATCH_REGEX_CASE_SENSITIVE ||
      m_matchType == MATCH_REGEX_CASE_INSENSITIVE) {
    try {
      int flags = value_objects::RegexPattern::FLAG_NONE;
      if (m_matchType == MATCH_REGEX_CASE_INSENSITIVE) {
        flags |= value_objects::RegexPattern::FLAG_CASE_INSENSITIVE;
      }

      m_regexPattern = value_objects::RegexPattern(m_path, flags);
      m_regexPatternValid = true;
    } catch (const std::exception& e) {
      m_regexPatternValid = false;
      throw shared::exceptions::LocationConfigException(
          std::string("Failed to compile regex pattern: ") + e.what(),
          shared::exceptions::LocationConfigException::
              REGEX_COMPILATION_FAILED);
    }
  }
}

bool LocationConfig::shouldUseAlias() const { return !m_alias.isEmpty(); }

LocationConfig::LocationMatchType LocationConfig::parseMatchType(
    const std::string& path) {
  if (path.empty()) {
    return MATCH_PREFIX;
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
}  // namespace domain
