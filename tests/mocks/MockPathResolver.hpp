/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MockPathResolver.hpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: umeneses <umeneses@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/01 00:00:00 by copilot           #+#    #+#             */
/*   Updated: 2026/01/01 19:18:55 by umeneses         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MOCKPATHRESOLVER_HPP
#define MOCKPATHRESOLVER_HPP

#include "domain/filesystem/value_objects/Path.hpp"

#include <map>
#include <string>
#include <vector>

class MockPathResolver {
 public:
  MockPathResolver();
  ~MockPathResolver();

  // Core path resolution operations
  domain::filesystem::value_objects::Path resolve(
      const domain::filesystem::value_objects::Path& requestedPath,
      const domain::filesystem::value_objects::Path& rootDirectory,
      const domain::filesystem::value_objects::Path& indexFile =
          domain::filesystem::value_objects::Path("index.html"));

  bool isPathWithinRoot(
      const domain::filesystem::value_objects::Path& resolvedPath,
      const domain::filesystem::value_objects::Path& rootDirectory);

  domain::filesystem::value_objects::Path normalizeRequestedPath(
      const domain::filesystem::value_objects::Path& requestedPath);

  bool isDirectoryPath(
      const domain::filesystem::value_objects::Path& path);

  // Mock configuration
  void setResolveSuccess(bool success);
  void setPathWithinRoot(bool withinRoot);
  void setMockResolution(const std::string& requestPath, 
                         const std::string& resolvedPath);
  void setIsDirectory(const std::string& path, bool isDir);

  // Verification methods
  int getCallCount(const std::string& methodName) const;
  std::vector<std::string> getCalledMethods() const;
  void clearCallHistory();
  const std::map<std::string, std::string>& getMockResolutions() const;

 private:
  // Mock storage
  std::map<std::string, std::string> m_mockResolutions;
  std::map<std::string, bool> m_isDirectory;
  
  // Configurable behavior
  bool m_resolveSuccess;
  bool m_pathWithinRoot;
  
  // Call tracking
  std::map<std::string, int> m_callCounts;
  std::vector<std::string> m_callHistory;

  void trackCall(const std::string& methodName);
  std::string normalizePath(const std::string& path) const;
  std::string joinPaths(const std::string& base, const std::string& relative) const;
};

#endif  // MOCKPATHRESOLVER_HPP
