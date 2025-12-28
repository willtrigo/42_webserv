/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PathResolver.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/22 19:34:04 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/27 23:27:04 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "domain/filesystem/value_objects/Path.hpp"
#include "infrastructure/filesystem/adapters/PathResolver.hpp"
#include "infrastructure/filesystem/exceptions/PathResolverException.hpp"

namespace infrastructure {
namespace filesystem {
namespace adapters {

PathResolver::PathResolver(adapters::FileSystemHelper* fileSystemHelper)
    : m_fileSystemHelper(fileSystemHelper) {
  if (fileSystemHelper == NULL) {
    throw exceptions::PathResolverException(
        "FileSystemHelper cannot be NULL",
        exceptions::PathResolverException::INVALID_DEPENDENCY);
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

domain::filesystem::value_objects::Path PathResolver::resolve(
    const domain::filesystem::value_objects::Path& requestedPath,
    const domain::filesystem::value_objects::Path& rootDirectory,
    const domain::filesystem::value_objects::Path& indexFile) {
  validateRootDirectory(rootDirectory);
  validateRequestedPath(requestedPath);

  domain::filesystem::value_objects::Path normalizedRequest =
      normalizeRequestedPath(requestedPath);
  domain::filesystem::value_objects::Path absolutePath =
      constructAbsolutePath(normalizedRequest, rootDirectory);

  if (!isPathWithinRoot(absolutePath, rootDirectory)) {
    throw exceptions::PathResolverException(
        "Path traversal outside root directory",
        exceptions::PathResolverException::PATH_TRAVERSAL);
  }

  if (!FileSystemHelper::exists(absolutePath.toString())) {
    throw exceptions::PathResolverException(
        "Path does not exist: " + absolutePath.toString(),
        exceptions::PathResolverException::PATH_NOT_FOUND);
  }

  if (FileSystemHelper::isDirectory(absolutePath.toString())) {
    return handleDirectoryRequest(absolutePath, indexFile);
  }

  return absolutePath;
}

domain::filesystem::value_objects::Path PathResolver::resolveWithLocations(
    const domain::filesystem::value_objects::Path& requestedUri,
    const domain::filesystem::value_objects::Path& serverRoot,
    const std::vector<std::pair<domain::filesystem::value_objects::Path,
                                domain::filesystem::value_objects::Path> >&
        locationMappings) {
  if (locationMappings.empty()) {
    return resolve(requestedUri, serverRoot);
  }

  domain::filesystem::value_objects::Path bestLocation =
      findBestLocationMatch(requestedUri, locationMappings);

  for (std::size_t i = 0; i < locationMappings.size(); ++i) {
    if (locationMappings[i].first == bestLocation) {
      return applyLocationMapping(requestedUri, locationMappings[i].first,
                                  locationMappings[i].second);
    }
  }

  return resolve(requestedUri, serverRoot);
}

domain::filesystem::value_objects::Path PathResolver::findBestLocationMatch(
    const domain::filesystem::value_objects::Path& requestedUri,
    const std::vector<std::pair<domain::filesystem::value_objects::Path,
                                domain::filesystem::value_objects::Path> >&
        locationMappings) {
  domain::filesystem::value_objects::Path bestMatch;
  std::size_t bestLength = 0;

  for (std::size_t i = 0; i < locationMappings.size(); ++i) {
    const domain::filesystem::value_objects::Path& locationPath =
        locationMappings[i].first;
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

domain::filesystem::value_objects::Path PathResolver::applyLocationMapping(
    const domain::filesystem::value_objects::Path& requestedUri,
    const domain::filesystem::value_objects::Path& locationPath,
    const domain::filesystem::value_objects::Path& locationRoot) {
  std::string requestStr = requestedUri.toString();
  std::string locationStr = locationPath.toString();

  if (requestStr.find(locationStr) != 0) {
    throw exceptions::PathResolverException(
        "Requested URI does not match location prefix",
        exceptions::PathResolverException::INVALID_MAPPING);
  }

  std::string remainingPath = requestStr.substr(locationStr.length());
  if (remainingPath.empty()) {
    remainingPath = "/";
  }

  domain::filesystem::value_objects::Path newRequest(remainingPath, true);
  return resolve(newRequest, locationRoot);
}

bool PathResolver::isPathWithinRoot(
    const domain::filesystem::value_objects::Path& resolvedPath,
    const domain::filesystem::value_objects::Path& rootDirectory) {
  std::string resolvedStr = resolvedPath.normalize().toString();
  std::string rootStr = rootDirectory.normalize().toString();

  if (rootStr.empty()) {
    return true;
  }

  if (resolvedStr.find(rootStr) != 0) {
    return false;
  }

  std::string relativePart = resolvedStr.substr(rootStr.length());
  return !domain::filesystem::value_objects::Path::isValidPath(relativePart);
}

domain::filesystem::value_objects::Path PathResolver::normalizeRequestedPath(
    const domain::filesystem::value_objects::Path& requestedPath) {
  if (requestedPath.isEmpty()) {
    return domain::filesystem::value_objects::Path("/", true);
  }

  domain::filesystem::value_objects::Path normalized =
      requestedPath.normalize();

  if (normalized.isRelative()) {
    return domain::filesystem::value_objects::Path("/" + normalized.toString(),
                                                   true);
  }

  return normalized;
}

domain::filesystem::value_objects::Path PathResolver::findIndexFile(
    const domain::filesystem::value_objects::Path& directoryPath,
    const std::vector<domain::filesystem::value_objects::Path>& indexFiles) {
  for (std::size_t i = 0; i < indexFiles.size(); ++i) {
    domain::filesystem::value_objects::Path indexPath =
        directoryPath.join(indexFiles[i]);

    if (FileSystemHelper::exists(indexPath.toString()) &&
        !FileSystemHelper::isDirectory(indexPath.toString())) {
      return indexPath;
    }
  }

  return domain::filesystem::value_objects::Path();
}

bool PathResolver::isDirectoryPath(
    const domain::filesystem::value_objects::Path& path) {
  return path.toString().empty() ||
         path.toString()[path.toString().length() - 1] == '/';
}

bool PathResolver::validateRootDirectory(
    const domain::filesystem::value_objects::Path& rootDirectory) {
  if (!rootDirectory.isAbsolute()) {
    throw exceptions::PathResolverException(
        "Root directory must be absolute path",
        exceptions::PathResolverException::INVALID_ROOT);
  }

  if (!FileSystemHelper::exists(rootDirectory.toString())) {
    throw exceptions::PathResolverException(
        "Root directory does not exist: " + rootDirectory.toString(),
        exceptions::PathResolverException::ROOT_NOT_FOUND);
  }

  if (!FileSystemHelper::isDirectory(rootDirectory.toString())) {
    throw exceptions::PathResolverException(
        "Root must be a directory: " + rootDirectory.toString(),
        exceptions::PathResolverException::ROOT_NOT_DIRECTORY);
  }

  return true;
}

bool PathResolver::validateRequestedPath(
    const domain::filesystem::value_objects::Path& requestedPath) {
  if (requestedPath.isEmpty()) {
    return true;
  }

  if (!requestedPath.isAbsolute()) {
    throw exceptions::PathResolverException(
        "Requested path must be absolute: " + requestedPath.toString(),
        exceptions::PathResolverException::INVALID_REQUEST);
  }

  return true;
}

domain::filesystem::value_objects::Path PathResolver::constructAbsolutePath(
    const domain::filesystem::value_objects::Path& requestedPath,
    const domain::filesystem::value_objects::Path& rootDirectory) {
  std::string requestStr = requestedPath.toString();

  if (requestStr == "/") {
    return rootDirectory;
  }

  domain::filesystem::value_objects::Path relativePath(requestStr.substr(1),
                                                       false);
  return rootDirectory.join(relativePath).normalize();
}

domain::filesystem::value_objects::Path PathResolver::handleDirectoryRequest(
    const domain::filesystem::value_objects::Path& directoryPath,
    const domain::filesystem::value_objects::Path& indexFile) {
  std::vector<domain::filesystem::value_objects::Path> indexFiles;
  indexFiles.push_back(indexFile);
  indexFiles.push_back(
      domain::filesystem::value_objects::Path("index.htm", false));
  indexFiles.push_back(
      domain::filesystem::value_objects::Path("index.php", false));
  indexFiles.push_back(
      domain::filesystem::value_objects::Path("default.html", false));

  domain::filesystem::value_objects::Path indexPath =
      findIndexFile(directoryPath, indexFiles);

  if (!indexPath.isEmpty()) {
    return indexPath;
  }

  return directoryPath;
}

domain::filesystem::value_objects::Path PathResolver::removeTrailingSlash(
    const domain::filesystem::value_objects::Path& path) {
  std::string pathStr = path.toString();
  if (!pathStr.empty() && pathStr[pathStr.length() - 1] == '/') {
    return domain::filesystem::value_objects::Path(
        pathStr.substr(0, pathStr.length() - 1), path.isAbsolute());
  }
  return path;
}

domain::filesystem::value_objects::Path PathResolver::addTrailingSlash(
    const domain::filesystem::value_objects::Path& path) {
  std::string pathStr = path.toString();
  if (!pathStr.empty() && pathStr[pathStr.length() - 1] != '/') {
    return domain::filesystem::value_objects::Path(pathStr + "/",
                                                   path.isAbsolute());
  }
  return path;
}

}  // namespace adapters
}  // namespace filesystem
}  // namespace infrastructure
