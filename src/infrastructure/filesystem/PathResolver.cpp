/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PathResolver.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/22 19:34:04 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/22 20:04:51 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "domain/value_objects/Path.hpp"
#include "infrastructure/filesystem/PathResolver.hpp"
#include "shared/exceptions/PathResolverException.hpp"

namespace infrastructure {
namespace filesystem {

PathResolver::PathResolver(FileSystemHelper* fileSystemHelper)
    : m_fileSystemHelper(fileSystemHelper) {
  if (fileSystemHelper == NULL) {
    throw shared::exceptions::PathResolverException(
        "FileSystemHelper cannot be NULL",
        shared::exceptions::PathResolverException::INVALID_DEPENDENCY);
  }
}

PathResolver::~PathResolver() {}

PathResolver::PathResolver(const PathResolver& other)
    : m_fileSystemHelper(other.m_fileSystemHelper) {}

PathResolver& PathResolver::operator=(const PathResolver& other) {
  if (this != &other) {
    m_fileSystemHelper = other.m_fileSystemHelper;
  }
  return *this;
}

domain::value_objects::Path PathResolver::resolve(
    const domain::value_objects::Path& requestedPath,
    const domain::value_objects::Path& rootDirectory,
    const domain::value_objects::Path& indexFile) const {
  validateRootDirectory(rootDirectory);
  validateRequestedPath(requestedPath);

  domain::value_objects::Path normalizedRequest =
      normalizeRequestedPath(requestedPath);
  domain::value_objects::Path absolutePath =
      constructAbsolutePath(normalizedRequest, rootDirectory);

  if (!isPathWithinRoot(absolutePath, rootDirectory)) {
    throw shared::exceptions::PathResolverException(
        "Path traversal outside root directory",
        shared::exceptions::PathResolverException::PATH_TRAVERSAL);
  }

  if (!infrastructure::filesystem::FileSystemHelper::exists(
          absolutePath.toString())) {
    throw shared::exceptions::PathResolverException(
        "Path does not exist: " + absolutePath.toString(),
        shared::exceptions::PathResolverException::PATH_NOT_FOUND);
  }

  if (infrastructure::filesystem::FileSystemHelper::isDirectory(
          absolutePath.toString())) {
    return handleDirectoryRequest(absolutePath, indexFile);
  }

  return absolutePath;
}

domain::value_objects::Path PathResolver::resolveWithLocations(
    const domain::value_objects::Path& requestedUri,
    const domain::value_objects::Path& serverRoot,
    const std::vector<
        std::pair<domain::value_objects::Path, domain::value_objects::Path> >&
        locationMappings) const {
  if (locationMappings.empty()) {
    return resolve(requestedUri, serverRoot);
  }

  domain::value_objects::Path bestLocation =
      findBestLocationMatch(requestedUri, locationMappings);

  for (std::size_t i = 0; i < locationMappings.size(); ++i) {
    if (locationMappings[i].first == bestLocation) {
      return applyLocationMapping(requestedUri, locationMappings[i].first,
                                  locationMappings[i].second);
    }
  }

  return resolve(requestedUri, serverRoot);
}

domain::value_objects::Path PathResolver::findBestLocationMatch(
    const domain::value_objects::Path& requestedUri,
    const std::vector<
        std::pair<domain::value_objects::Path, domain::value_objects::Path> >&
        locationMappings) {
  domain::value_objects::Path bestMatch;
  std::size_t bestLength = 0;

  for (std::size_t i = 0; i < locationMappings.size(); ++i) {
    const domain::value_objects::Path& locationPath = locationMappings[i].first;
    std::string locationStr = locationPath.toString();
    std::string requestStr = requestedUri.toString();

    if (locationStr.length() > bestLength &&
        requestStr.find(locationStr) == 0) {
      bestMatch = locationPath;
      bestLength = locationStr.length();
    }
  }

  return bestMatch;
}

domain::value_objects::Path PathResolver::applyLocationMapping(
    const domain::value_objects::Path& requestedUri,
    const domain::value_objects::Path& locationPath,
    const domain::value_objects::Path& locationRoot) const {
  std::string requestStr = requestedUri.toString();
  std::string locationStr = locationPath.toString();

  if (requestStr.find(locationStr) != 0) {
    throw shared::exceptions::PathResolverException(
        "Requested URI does not match location prefix",
        shared::exceptions::PathResolverException::INVALID_MAPPING);
  }

  std::string remainingPath = requestStr.substr(locationStr.length());
  if (remainingPath.empty()) {
    remainingPath = "/";
  }

  domain::value_objects::Path newRequest(remainingPath, true);
  return resolve(newRequest, locationRoot);
}

bool PathResolver::isPathWithinRoot(
    const domain::value_objects::Path& resolvedPath,
    const domain::value_objects::Path& rootDirectory) {
  std::string resolvedStr = resolvedPath.normalize().toString();
  std::string rootStr = rootDirectory.normalize().toString();

  if (rootStr.empty()) {
    return true;
  }

  if (resolvedStr.find(rootStr) != 0) {
    return false;
  }

  std::string relativePart = resolvedStr.substr(rootStr.length());
  return !domain::value_objects::Path::isValidPath(relativePart);
}

domain::value_objects::Path PathResolver::normalizeRequestedPath(
    const domain::value_objects::Path& requestedPath) {
  if (requestedPath.isEmpty()) {
    return domain::value_objects::Path("/", true);
  }

  domain::value_objects::Path normalized = requestedPath.normalize();

  if (normalized.isRelative()) {
    return domain::value_objects::Path("/" + normalized.toString(), true);
  }

  return normalized;
}

domain::value_objects::Path PathResolver::findIndexFile(
    const domain::value_objects::Path& directoryPath,
    const std::vector<domain::value_objects::Path>& indexFiles) {
  for (std::size_t i = 0; i < indexFiles.size(); ++i) {
    domain::value_objects::Path indexPath = directoryPath.join(indexFiles[i]);

    if (infrastructure::filesystem::FileSystemHelper::exists(
            indexPath.toString()) &&
        !infrastructure::filesystem::FileSystemHelper::isDirectory(
            indexPath.toString())) {
      return indexPath;
    }
  }

  return domain::value_objects::Path();
}

bool PathResolver::isDirectoryPath(const domain::value_objects::Path& path) {
  return path.toString().empty() ||
         path.toString()[path.toString().length() - 1] == '/';
}

bool PathResolver::validateRootDirectory(
    const domain::value_objects::Path& rootDirectory) {
  if (!rootDirectory.isAbsolute()) {
    throw shared::exceptions::PathResolverException(
        "Root directory must be absolute path",
        shared::exceptions::PathResolverException::INVALID_ROOT);
  }

  if (!infrastructure::filesystem::FileSystemHelper::exists(
          rootDirectory.toString())) {
    throw shared::exceptions::PathResolverException(
        "Root directory does not exist: " + rootDirectory.toString(),
        shared::exceptions::PathResolverException::ROOT_NOT_FOUND);
  }

  if (!infrastructure::filesystem::FileSystemHelper::isDirectory(
          rootDirectory.toString())) {
    throw shared::exceptions::PathResolverException(
        "Root must be a directory: " + rootDirectory.toString(),
        shared::exceptions::PathResolverException::ROOT_NOT_DIRECTORY);
  }

  return true;
}

bool PathResolver::validateRequestedPath(
    const domain::value_objects::Path& requestedPath) {
  if (requestedPath.isEmpty()) {
    return true;
  }

  if (!requestedPath.isAbsolute()) {
    throw shared::exceptions::PathResolverException(
        "Requested path must be absolute: " + requestedPath.toString(),
        shared::exceptions::PathResolverException::INVALID_REQUEST);
  }

  return true;
}

domain::value_objects::Path PathResolver::constructAbsolutePath(
    const domain::value_objects::Path& requestedPath,
    const domain::value_objects::Path& rootDirectory) {
  std::string requestStr = requestedPath.toString();

  if (requestStr == "/") {
    return rootDirectory;
  }

  domain::value_objects::Path relativePath(requestStr.substr(1), false);
  return rootDirectory.join(relativePath).normalize();
}

domain::value_objects::Path PathResolver::handleDirectoryRequest(
    const domain::value_objects::Path& directoryPath,
    const domain::value_objects::Path& indexFile) {
  std::vector<domain::value_objects::Path> indexFiles;
  indexFiles.push_back(indexFile);
  indexFiles.push_back(domain::value_objects::Path("index.htm", false));
  indexFiles.push_back(domain::value_objects::Path("index.php", false));
  indexFiles.push_back(domain::value_objects::Path("default.html", false));

  domain::value_objects::Path indexPath =
      findIndexFile(directoryPath, indexFiles);

  if (!indexPath.isEmpty()) {
    return indexPath;
  }

  return directoryPath;
}

domain::value_objects::Path PathResolver::removeTrailingSlash(
    const domain::value_objects::Path& path) {
  std::string pathStr = path.toString();
  if (!pathStr.empty() && pathStr[pathStr.length() - 1] == '/') {
    return domain::value_objects::Path(pathStr.substr(0, pathStr.length() - 1),
                                       path.isAbsolute());
  }
  return path;
}

domain::value_objects::Path PathResolver::addTrailingSlash(
    const domain::value_objects::Path& path) {
  std::string pathStr = path.toString();
  if (!pathStr.empty() && pathStr[pathStr.length() - 1] != '/') {
    return domain::value_objects::Path(pathStr + "/", path.isAbsolute());
  }
  return path;
}

}  // namespace filesystem
}  // namespace infrastructure
