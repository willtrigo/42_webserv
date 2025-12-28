/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Route.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/24 21:56:57 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/27 22:53:47 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "domain/configuration/exceptions/RouteException.hpp"
#include "domain/configuration/value_objects/Route.hpp"

#include <sstream>

namespace domain {
namespace configuration {
namespace value_objects {

Route::MatchInfo::MatchInfo() : isDirectory(false) {}

Route::Route()
    : m_handlerType(STATIC_FILE),
      m_maxBodySize(filesystem::value_objects::Size::megabyte()),
      m_filePermissions(filesystem::value_objects::Permission::
                            ownerAllGroupReadExecuteOtherReadExecute()),
      m_directoryListing(false),
      m_redirectCode(shared::value_objects::ErrorCode::movedPermanently()) {
  try {
    validate();
  } catch (const std::exception& e) {
    std::ostringstream oss;
    oss << "Failed to create route: " << e.what();
    throw exceptions::RouteException(
        oss.str(), exceptions::RouteException::CONFIGURATION_ERROR);
  }
}

Route::Route(const filesystem::value_objects::Path& path,
             const std::set<http::value_objects::HttpMethod>& allowedMethods,
             HandlerType handlerType)
    : m_pathPattern(path),
      m_allowedMethods(allowedMethods),
      m_handlerType(handlerType),
      m_maxBodySize(filesystem::value_objects::Size::megabyte()),
      m_filePermissions(filesystem::value_objects::Permission::
                            ownerAllGroupReadExecuteOtherReadExecute()),
      m_directoryListing(false),
      m_redirectCode(shared::value_objects::ErrorCode::movedPermanently()) {
  try {
    validate();
  } catch (const std::exception& e) {
    std::ostringstream oss;
    oss << "Failed to create route: " << e.what();
    throw exceptions::RouteException(
        oss.str(), exceptions::RouteException::CONFIGURATION_ERROR);
  }
}

const filesystem::value_objects::Path& Route::getPath() const {
  return m_pathPattern;
}

const std::set<http::value_objects::HttpMethod>& Route::getAllowedMethods()
    const {
  return m_allowedMethods;
}

Route::HandlerType Route::getHandlerType() const { return m_handlerType; }

std::string Route::getHandlerTarget() const {
  switch (m_handlerType) {
    case STATIC_FILE:
      return m_rootDirectory.empty() ? "." : m_rootDirectory;
    case DIRECTORY_LISTING:
      return m_rootDirectory;
    case CGI_EXECUTION:
      return m_cgiInterpreter;
    case POST:
      return m_rootDirectory;
    case REDIRECT:
      return m_redirectTarget;
    case DELETE:
      return m_rootDirectory;
    case UPLOAD:
      return m_uploadDirectory;
    default:
      std::ostringstream oss;
      oss << "Unknown handler type: " << m_handlerType;
      throw exceptions::RouteException(
          oss.str(), exceptions::RouteException::INVALID_HANDLER);
  }
}

bool Route::allowsMethod(const http::value_objects::HttpMethod& method) const {
  return m_allowedMethods.find(method) != m_allowedMethods.end();
}

bool Route::matches(const std::string& requestPath) const {
  if (requestPath.empty()) {
    std::ostringstream oss;
    oss << "Empty request path";
    throw exceptions::RouteException(oss.str(),
                                     exceptions::RouteException::INVALID_PATH);
  }

  try {
    return isPathMatch(requestPath);
  } catch (const std::exception& e) {
    std::ostringstream oss;
    oss << "Failed to match path '" << requestPath << "': " << e.what();
    throw exceptions::RouteException(oss.str(),
                                     exceptions::RouteException::INVALID_PATH);
  }
}

bool Route::isCgiRoute() const { return m_handlerType == CGI_EXECUTION; }

bool Route::isRedirect() const { return m_handlerType == REDIRECT; }

bool Route::isUploadRoute() const { return m_handlerType == UPLOAD; }

bool Route::allowsDirectoryListing() const {
  return m_directoryListing && m_handlerType == DIRECTORY_LISTING;
}

void Route::setRootDirectory(const std::string& root) {
  if (root.empty()) {
    std::ostringstream oss;
    oss << "Root directory cannot be empty";
    throw exceptions::RouteException(
        oss.str(), exceptions::RouteException::CONFIGURATION_ERROR);
  }

  try {
    filesystem::value_objects::Path::isValidPath(root, true);
    m_rootDirectory = root;
  } catch (const std::exception& e) {
    std::ostringstream oss;
    oss << "Invalid root directory '" << root << "': " << e.what();
    throw exceptions::RouteException(
        oss.str(), exceptions::RouteException::CONFIGURATION_ERROR);
  }
}

void Route::setIndexFile(const std::string& index) {
  if (index.empty()) {
    m_indexFile = "";
    return;
  }

  for (std::size_t i = 0; i < index.length(); ++i) {
    if (index[i] == '/') {
      std::ostringstream oss;
      oss << "Index file cannot contain path separator: " << index;
      throw exceptions::RouteException(
          oss.str(), exceptions::RouteException::CONFIGURATION_ERROR);
    }
  }

  m_indexFile = index;
}

void Route::setUploadDirectory(const std::string& uploadDir) {
  if (uploadDir.empty()) {
    std::ostringstream oss;
    oss << "Upload directory cannot be empty";
    throw exceptions::RouteException(
        oss.str(), exceptions::RouteException::CONFIGURATION_ERROR);
  }

  try {
    filesystem::value_objects::Path::isValidPath(uploadDir, true);
    m_uploadDirectory = uploadDir;
  } catch (const std::exception& e) {
    std::ostringstream oss;
    oss << "Invalid upload directory '" << uploadDir << "': " << e.what();
    throw exceptions::RouteException(
        oss.str(), exceptions::RouteException::CONFIGURATION_ERROR);
  }
}

void Route::setMaxBodySize(const filesystem::value_objects::Size& maxSize) {
  if (maxSize.getBytes() == 0) {
    std::ostringstream oss;
    oss << "Max body size cannot be zero";
    throw exceptions::RouteException(
        oss.str(), exceptions::RouteException::CONFIGURATION_ERROR);
  }

  m_maxBodySize = maxSize;
}

void Route::setDirectoryListing(bool enable) { m_directoryListing = enable; }

void Route::setCgiConfig(const std::string& interpreter,
                         const std::string& extension) {
  if (interpreter.empty()) {
    std::ostringstream oss;
    oss << "CGI interpreter cannot be empty";
    throw exceptions::RouteException(
        oss.str(), exceptions::RouteException::CONFIGURATION_ERROR);
  }

  if (extension.empty() || extension[0] != '.') {
    std::ostringstream oss;
    oss << "CGI extension must start with '.': " << extension;
    throw exceptions::RouteException(
        oss.str(), exceptions::RouteException::CONFIGURATION_ERROR);
  }

  m_cgiInterpreter = interpreter;
  m_cgiExtension = extension;
}

void Route::setRedirect(const std::string& target,
                        const shared::value_objects::ErrorCode& code) {
  if (target.empty()) {
    std::ostringstream oss;
    oss << "Redirect target cannot be empty";
    throw exceptions::RouteException(
        oss.str(), exceptions::RouteException::CONFIGURATION_ERROR);
  }

  if (!code.isRedirection()) {
    std::ostringstream oss;
    oss << "Redirect code must be a redirection code (3xx), got: "
        << code.getValue();
    throw exceptions::RouteException(
        oss.str(), exceptions::RouteException::CONFIGURATION_ERROR);
  }

  m_redirectTarget = target;
  m_redirectCode = code;
}

void Route::setFilePermissions(
    const filesystem::value_objects::Permission& permissions) {
  if (!domain::filesystem::value_objects::Permission::isValidPermission(
          permissions.getOctalValue())) {
    std::ostringstream oss;
    oss << "Invalid file permissions: " << permissions.getOctalValue();
    throw exceptions::RouteException(
        oss.str(), exceptions::RouteException::CONFIGURATION_ERROR);
  }

  m_filePermissions = permissions;
}

std::string Route::getRootDirectory() const { return m_rootDirectory; }

std::string Route::getIndexFile() const { return m_indexFile; }

std::string Route::getUploadDirectory() const { return m_uploadDirectory; }

filesystem::value_objects::Size Route::getMaxBodySize() const {
  return m_maxBodySize;
}

filesystem::value_objects::Permission Route::getFilePermissions() const {
  return m_filePermissions;
}

std::string Route::getCgiInterpreter() const { return m_cgiInterpreter; }

std::string Route::getCgiExtension() const { return m_cgiExtension; }

std::string Route::getRedirectTarget() const { return m_redirectTarget; }

shared::value_objects::ErrorCode Route::getRedirectCode() const {
  return m_redirectCode;
}

Route::MatchInfo Route::resolveRequest(
    const std::string& requestPath,
    const std::map<std::string, std::string>& queryParams) const {
  if (requestPath.empty()) {
    std::ostringstream oss;
    oss << "Cannot resolve empty request path";
    throw exceptions::RouteException(oss.str(),
                                     exceptions::RouteException::INVALID_PATH);
  }

  MatchInfo info;

  try {
    filesystem::value_objects::Path fullPath = buildFullPath(requestPath);
    info.resolvedPath = fullPath;
    info.queryParams = queryParams;

    std::string pathStr = fullPath.toString();
    info.isDirectory =
        (!pathStr.empty() && pathStr[pathStr.length() - 1] == '/');

    info.fileToServe = findFileToServe(fullPath);

  } catch (const std::exception& e) {
    std::ostringstream oss;
    oss << "Failed to resolve request path '" << requestPath
        << "': " << e.what();
    throw exceptions::RouteException(
        oss.str(), exceptions::RouteException::FILE_NOT_FOUND);
  }

  return info;
}

filesystem::value_objects::Path Route::buildFullPath(
    const std::string& requestPath) const {
  if (m_rootDirectory.empty()) {
    try {
      return filesystem::value_objects::Path::fromString(requestPath, true);
    } catch (const std::exception& e) {
      std::ostringstream oss;
      oss << "Invalid request path '" << requestPath << "': " << e.what();
      throw exceptions::RouteException(
          oss.str(), exceptions::RouteException::INVALID_PATH);
    }
  }

  std::string fullPath = m_rootDirectory;

  if (!fullPath.empty() && fullPath[fullPath.length() - 1] != '/' &&
      !requestPath.empty() && requestPath[0] != '/') {
    fullPath += '/';
  }

  if (!fullPath.empty() && fullPath[fullPath.length() - 1] == '/' &&
      !requestPath.empty() && requestPath[0] == '/') {
    fullPath += requestPath.substr(1);
  } else {
    fullPath += requestPath;
  }

  try {
    return filesystem::value_objects::Path::fromString(fullPath, true);
  } catch (const std::exception& e) {
    std::ostringstream oss;
    oss << "Invalid full path '" << fullPath << "': " << e.what();
    throw exceptions::RouteException(oss.str(),
                                     exceptions::RouteException::INVALID_PATH);
  }
}

bool Route::isPathMatch(const std::string& requestPath) const {
  std::string pattern = m_pathPattern.toString();

  if (pattern.empty()) {
    std::ostringstream oss;
    oss << "Route pattern is empty";
    throw exceptions::RouteException(oss.str(),
                                     exceptions::RouteException::INVALID_PATH);
  }

  if (requestPath.empty()) {
    std::ostringstream oss;
    oss << "Request path is empty";
    throw exceptions::RouteException(oss.str(),
                                     exceptions::RouteException::INVALID_PATH);
  }

  if (pattern == requestPath) {
    return true;
  }

  bool patternIsDirectory =
      (!pattern.empty() && pattern[pattern.length() - 1] == '/');

  if (patternIsDirectory) {
    if (requestPath.find(pattern) == 0) {
      return true;
    }

    std::string requestWithoutSlash = requestPath;
    if (!requestWithoutSlash.empty() &&
        requestWithoutSlash[requestWithoutSlash.length() - 1] == '/') {
      requestWithoutSlash.erase(requestWithoutSlash.length() - 1);
    }

    if (pattern == requestWithoutSlash + "/") {
      return true;
    }
  } else {
    if (pattern + "/" == requestPath) {
      return true;
    }
  }

  return false;
}

// TODO:Check if index file exists (in a real implementation, you'd check
// filesystem) For now, we'll just return it
std::string Route::findFileToServe(
    const filesystem::value_objects::Path& fullPath) const {
  std::string pathStr = fullPath.toString();

  if (pathStr.empty()) {
    std::ostringstream oss;
    oss << "Empty path for file serving";
    throw exceptions::RouteException(
        oss.str(), exceptions::RouteException::FILE_NOT_FOUND);
  }

  // If it's not a directory (doesn't end with '/'), return the path itself
  if (pathStr.empty() || pathStr[pathStr.length() - 1] != '/') {
    return pathStr;
  }

  // It's a directory, check for index file
  if (!m_indexFile.empty()) {
    // Build path to index file
    std::string indexPath = pathStr;
    if (!indexPath.empty() && indexPath[indexPath.length() - 1] != '/') {
      indexPath += '/';
    }
    indexPath += m_indexFile;

    return indexPath;
  }

  // No index file specified, check if directory listing is enabled
  if (m_directoryListing && m_handlerType == DIRECTORY_LISTING) {
    // Return the directory path itself for listing
    return pathStr;
  }

  // Default: return the path as is
  return pathStr;
}

void Route::validate() const {
  if (m_pathPattern.isEmpty()) {
    throw exceptions::RouteException("Route path pattern cannot be empty",
                                     exceptions::RouteException::INVALID_PATH);
  }

  if (m_allowedMethods.empty()) {
    throw exceptions::RouteException(
        "Route must have at least one allowed method",
        exceptions::RouteException::INVALID_METHOD);
  }

  if (m_handlerType < STATIC_FILE || m_handlerType > UPLOAD) {
    std::ostringstream oss;
    oss << "Invalid handler type: " << m_handlerType;
    throw exceptions::RouteException(
        oss.str(), exceptions::RouteException::INVALID_HANDLER);
  }

  if (m_handlerType == CGI_EXECUTION) {
    if (m_cgiInterpreter.empty() || m_cgiExtension.empty()) {
      throw exceptions::RouteException(
          "CGI route requires both interpreter and extension",
          exceptions::RouteException::CONFIGURATION_ERROR);
    }
  }

  if (m_handlerType == REDIRECT) {
    if (m_redirectTarget.empty()) {
      throw exceptions::RouteException(
          "Redirect route requires a target",
          exceptions::RouteException::CONFIGURATION_ERROR);
    }
    if (!m_redirectCode.isRedirection()) {
      std::ostringstream oss;
      oss << "Redirect code must be 3xx, got: " << m_redirectCode.getValue();
      throw exceptions::RouteException(
          oss.str(), exceptions::RouteException::CONFIGURATION_ERROR);
    }
  }

  if (m_handlerType == UPLOAD) {
    if (m_uploadDirectory.empty()) {
      throw exceptions::RouteException(
          "Upload route requires an upload directory",
          exceptions::RouteException::CONFIGURATION_ERROR);
    }
  }
}

bool Route::isStaticFileRoute() const { return m_handlerType == STATIC_FILE; }

bool Route::isPostRoute() const { return m_handlerType == POST; }

bool Route::isDeleteRoute() const { return m_handlerType == DELETE; }

bool Route::hasRootDirectory() const { return !m_rootDirectory.empty(); }

bool Route::hasIndexFile() const { return !m_indexFile.empty(); }

bool Route::hasCgiConfig() const {
  return !m_cgiInterpreter.empty() && !m_cgiExtension.empty();
}

bool Route::hasRedirectConfig() const { return !m_redirectTarget.empty(); }

bool Route::hasUploadConfig() const { return !m_uploadDirectory.empty(); }

}  // namespace value_objects
}  // namespace configuration
}  // namespace domain
