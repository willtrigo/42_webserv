/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PathResolver.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/22 19:33:32 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/22 20:04:56 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PATHRESOLVER_HPP
#define PATHRESOLVER_HPP

#include "domain/value_objects/Path.hpp"
#include "infrastructure/filesystem/FileSystemHelper.hpp"

#include <string>
#include <vector>

namespace infrastructure {
namespace filesystem {

class PathResolver {
 public:
  explicit PathResolver(FileSystemHelper* fileSystemHelper);
  virtual ~PathResolver();

  PathResolver(const PathResolver& other);
  PathResolver& operator=(const PathResolver& other);

  domain::value_objects::Path resolve(
      const domain::value_objects::Path& requestedPath,
      const domain::value_objects::Path& rootDirectory,
      const domain::value_objects::Path& indexFile =
          domain::value_objects::Path("index.html")) const;

  domain::value_objects::Path resolveWithLocations(
      const domain::value_objects::Path& requestedUri,
      const domain::value_objects::Path& serverRoot,
      const std::vector<
          std::pair<domain::value_objects::Path, domain::value_objects::Path> >&
          locationMappings) const;

  static domain::value_objects::Path findBestLocationMatch(
      const domain::value_objects::Path& requestedUri,
      const std::vector<
          std::pair<domain::value_objects::Path, domain::value_objects::Path> >&
          locationMappings);

  domain::value_objects::Path applyLocationMapping(
      const domain::value_objects::Path& requestedUri,
      const domain::value_objects::Path& locationPath,
      const domain::value_objects::Path& locationRoot) const;

  static bool isPathWithinRoot(
      const domain::value_objects::Path& resolvedPath,
      const domain::value_objects::Path& rootDirectory);

  static domain::value_objects::Path normalizeRequestedPath(
      const domain::value_objects::Path& requestedPath);

  static domain::value_objects::Path findIndexFile(
      const domain::value_objects::Path& directoryPath,
      const std::vector<domain::value_objects::Path>& indexFiles);

  static bool isDirectoryPath(const domain::value_objects::Path& path);

 private:
  FileSystemHelper* m_fileSystemHelper;

  static bool validateRootDirectory(
      const domain::value_objects::Path& rootDirectory);
  static bool validateRequestedPath(
      const domain::value_objects::Path& requestedPath);
  static domain::value_objects::Path constructAbsolutePath(
      const domain::value_objects::Path& requestedPath,
      const domain::value_objects::Path& rootDirectory);
  static domain::value_objects::Path handleDirectoryRequest(
      const domain::value_objects::Path& directoryPath,
      const domain::value_objects::Path& indexFile);
  static domain::value_objects::Path removeTrailingSlash(
      const domain::value_objects::Path& path);
  static domain::value_objects::Path addTrailingSlash(
      const domain::value_objects::Path& path);
};

}  // namespace filesystem
}  // namespace infrastructure

#endif  // PATHRESOLVER_HPP
