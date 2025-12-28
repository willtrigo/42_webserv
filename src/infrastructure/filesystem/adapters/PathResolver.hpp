/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PathResolver.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/22 19:33:32 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/27 23:26:59 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PATHRESOLVER_HPP
#define PATHRESOLVER_HPP

#include "domain/filesystem/value_objects/Path.hpp"
#include "infrastructure/filesystem/adapters/FileSystemHelper.hpp"

#include <string>
#include <vector>

namespace infrastructure {
namespace filesystem {
namespace adapters {

class PathResolver {
 public:
  explicit PathResolver(adapters::FileSystemHelper* fileSystemHelper);
  virtual ~PathResolver();

  PathResolver(const PathResolver& other);
  PathResolver& operator=(const PathResolver& other);

  static domain::filesystem::value_objects::Path resolve(
      const domain::filesystem::value_objects::Path& requestedPath,
      const domain::filesystem::value_objects::Path& rootDirectory,
      const domain::filesystem::value_objects::Path& indexFile =
          domain::filesystem::value_objects::Path("index.html"));

  static domain::filesystem::value_objects::Path resolveWithLocations(
      const domain::filesystem::value_objects::Path& requestedUri,
      const domain::filesystem::value_objects::Path& serverRoot,
      const std::vector<std::pair<domain::filesystem::value_objects::Path,
                                  domain::filesystem::value_objects::Path> >&
          locationMappings);

  static domain::filesystem::value_objects::Path findBestLocationMatch(
      const domain::filesystem::value_objects::Path& requestedUri,
      const std::vector<std::pair<domain::filesystem::value_objects::Path,
                                  domain::filesystem::value_objects::Path> >&
          locationMappings);

  static domain::filesystem::value_objects::Path applyLocationMapping(
      const domain::filesystem::value_objects::Path& requestedUri,
      const domain::filesystem::value_objects::Path& locationPath,
      const domain::filesystem::value_objects::Path& locationRoot);

  static bool isPathWithinRoot(
      const domain::filesystem::value_objects::Path& resolvedPath,
      const domain::filesystem::value_objects::Path& rootDirectory);

  static domain::filesystem::value_objects::Path normalizeRequestedPath(
      const domain::filesystem::value_objects::Path& requestedPath);

  static domain::filesystem::value_objects::Path findIndexFile(
      const domain::filesystem::value_objects::Path& directoryPath,
      const std::vector<domain::filesystem::value_objects::Path>& indexFiles);

  static bool isDirectoryPath(
      const domain::filesystem::value_objects::Path& path);

 private:
  adapters::FileSystemHelper* m_fileSystemHelper;

  static bool validateRootDirectory(
      const domain::filesystem::value_objects::Path& rootDirectory);
  static bool validateRequestedPath(
      const domain::filesystem::value_objects::Path& requestedPath);
  static domain::filesystem::value_objects::Path constructAbsolutePath(
      const domain::filesystem::value_objects::Path& requestedPath,
      const domain::filesystem::value_objects::Path& rootDirectory);
  static domain::filesystem::value_objects::Path handleDirectoryRequest(
      const domain::filesystem::value_objects::Path& directoryPath,
      const domain::filesystem::value_objects::Path& indexFile);
  static domain::filesystem::value_objects::Path removeTrailingSlash(
      const domain::filesystem::value_objects::Path& path);
  static domain::filesystem::value_objects::Path addTrailingSlash(
      const domain::filesystem::value_objects::Path& path);
};

}  // namespace adapters
}  // namespace filesystem
}  // namespace infrastructure

#endif  // PATHRESOLVER_HPP
