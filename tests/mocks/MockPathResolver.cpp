/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MockPathResolver.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: umeneses <umeneses@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/01 00:00:00 by copilot           #+#    #+#             */
/*   Updated: 2026/01/01 19:22:31 by umeneses         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "mocks/MockPathResolver.hpp"
#include <algorithm>

MockPathResolver::MockPathResolver()
    : m_resolveSuccess(true), m_pathWithinRoot(true) {}

MockPathResolver::~MockPathResolver() {}

domain::filesystem::value_objects::Path MockPathResolver::resolve(
    const domain::filesystem::value_objects::Path& requestedPath,
    const domain::filesystem::value_objects::Path& rootDirectory,
    const domain::filesystem::value_objects::Path& indexFile) {
  trackCall("resolve");

  if (!m_resolveSuccess) {
    return domain::filesystem::value_objects::Path("/");
  }

  std::string requestStr = normalizePath(requestedPath.toString());
  
  // Check if we have a mock resolution for this path
  if (m_mockResolutions.find(requestStr) != m_mockResolutions.end()) {
    return domain::filesystem::value_objects::Path(m_mockResolutions[requestStr]);
  }

  // Default behavior: join root and requested path
  std::string resolved = joinPaths(rootDirectory.toString(), requestStr);
  
  // If path is a directory and has index file, append it
  if (m_isDirectory.find(resolved) != m_isDirectory.end() && 
      m_isDirectory[resolved]) {
    resolved = joinPaths(resolved, indexFile.toString());
  }

  return domain::filesystem::value_objects::Path(resolved);
}

bool MockPathResolver::isPathWithinRoot(
    const domain::filesystem::value_objects::Path& resolvedPath,
    const domain::filesystem::value_objects::Path& rootDirectory) {
  trackCall("isPathWithinRoot");

  if (!m_pathWithinRoot) {
    return false;
  }

  std::string resolved = normalizePath(resolvedPath.toString());
  std::string root = normalizePath(rootDirectory.toString());

  // Check if resolved path starts with root path
  if (resolved.length() >= root.length()) {
    if (resolved.substr(0, root.length()) != root) {
      return false;
    }
    // If lengths are equal, paths match exactly
    if (resolved.length() == root.length()) {
      return true;
    }
    // Check next character is a slash to avoid /var/www matching /var/www2
    return resolved[root.length()] == '/';
  }

  return false;
}

domain::filesystem::value_objects::Path MockPathResolver::normalizeRequestedPath(
    const domain::filesystem::value_objects::Path& requestedPath) {
  trackCall("normalizeRequestedPath");

  std::string path = requestedPath.toString();
  
  // Remove duplicate slashes
  std::string normalized;
  bool lastWasSlash = false;
  for (size_t i = 0; i < path.length(); ++i) {
    if (path[i] == '/') {
      if (!lastWasSlash) {
        normalized += path[i];
      }
      lastWasSlash = true;
    } else {
      normalized += path[i];
      lastWasSlash = false;
    }
  }

  // Remove trailing slash unless it's the root
  if (normalized.length() > 1 && 
      normalized[normalized.length() - 1] == '/') {
    normalized = normalized.substr(0, normalized.length() - 1);
  }

  return domain::filesystem::value_objects::Path(normalized);
}

bool MockPathResolver::isDirectoryPath(
    const domain::filesystem::value_objects::Path& path) {
  trackCall("isDirectoryPath");

  std::string pathStr = path.toString();  // Don't normalize yet
  std::string normalized = normalizePath(pathStr);
  
  if (m_isDirectory.find(normalized) != m_isDirectory.end()) {
    return m_isDirectory[normalized];
  }

  // Default: paths ending with / are directories (check original, not normalized)
  return !pathStr.empty() && pathStr[pathStr.length() - 1] == '/';
}

void MockPathResolver::setResolveSuccess(bool success) {
  m_resolveSuccess = success;
}

void MockPathResolver::setPathWithinRoot(bool withinRoot) {
  m_pathWithinRoot = withinRoot;
}

void MockPathResolver::setMockResolution(const std::string& requestPath,
                                         const std::string& resolvedPath) {
  m_mockResolutions[normalizePath(requestPath)] = resolvedPath;
}

void MockPathResolver::setIsDirectory(const std::string& path, bool isDir) {
  m_isDirectory[normalizePath(path)] = isDir;
}

int MockPathResolver::getCallCount(const std::string& methodName) const {
  std::map<std::string, int>::const_iterator it = m_callCounts.find(methodName);
  return (it != m_callCounts.end()) ? it->second : 0;
}

std::vector<std::string> MockPathResolver::getCalledMethods() const {
  return m_callHistory;
}

void MockPathResolver::clearCallHistory() {
  m_callCounts.clear();
  m_callHistory.clear();
}

const std::map<std::string, std::string>& 
MockPathResolver::getMockResolutions() const {
  return m_mockResolutions;
}

void MockPathResolver::trackCall(const std::string& methodName) {
  m_callCounts[methodName]++;
  m_callHistory.push_back(methodName);
}

std::string MockPathResolver::normalizePath(const std::string& path) const {
  std::string normalized = path;
  // Remove trailing slash for consistency (except root)
  if (normalized.length() > 1 && 
      normalized[normalized.length() - 1] == '/') {
    normalized = normalized.substr(0, normalized.length() - 1);
  }
  return normalized;
}

std::string MockPathResolver::joinPaths(const std::string& base,
                                        const std::string& relative) const {
  if (base.empty()) return relative;
  if (relative.empty()) return base;

  std::string result = base;
  if (result[result.length() - 1] != '/') {
    result += "/";
  }

  std::string rel = relative;
  if (!rel.empty() && rel[0] == '/') {
    rel = rel.substr(1);
  }

  result += rel;
  return result;
}
