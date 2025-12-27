/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Path.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: umeneses <umeneses@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/20 01:10:52 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/27 19:38:22 by umeneses         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "domain/filesystem/exceptions/PathException.hpp"
#include "domain/filesystem/value_objects/Path.hpp"

#include <algorithm>
#include <cctype>
#include <sstream>

namespace domain {
namespace filesystem {
namespace value_objects {

const std::string Path::CURRENT_DIR = ".";
const std::string Path::PARENT_DIR = "..";

const std::string Path::VALID_PATH_CHARS =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789"
    ".-_/";

const std::string Path::INVALID_FILENAME_CHARS = "/\\:*?\"<>|";

const std::string Path::RESERVED_FILENAMES[] = {
    ".",    "..",   "CON",  "PRN",  "AUX",  "NUL",  "COM1", "COM2", "COM3",
    "COM4", "COM5", "COM6", "COM7", "COM8", "COM9", "LPT1", "LPT2", "LPT3",
    "LPT4", "LPT5", "LPT6", "LPT7", "LPT8", "LPT9", ""};

Path::Path() : m_isAbsolute(false) {}

Path::Path(const std::string& path, bool mustBeAbsolute)
    : m_path(path), m_isAbsolute(false) {
  validatePathString(path, mustBeAbsolute);
  m_isAbsolute = (!path.empty() && path[0] == PATH_SEPARATOR);
}

Path::Path(const Path& other)
    : m_path(other.m_path), m_isAbsolute(other.m_isAbsolute) {}

Path::~Path() {}

Path& Path::operator=(const Path& other) {
  if (this != &other) {
    m_path = other.m_path;
    m_isAbsolute = other.m_isAbsolute;
  }
  return *this;
}

std::string Path::toString() const { return m_path; }

std::string Path::getDirectory() const {
  if (m_path.empty()) return "";

  std::size_t lastSlash = m_path.find_last_of(PATH_SEPARATOR);
  if (lastSlash == std::string::npos) {
    return "";
  }
  if (lastSlash == 0) {
    return std::string(1, PATH_SEPARATOR);
  }

  return m_path.substr(0, lastSlash);
}

std::string Path::getFilename() const {
  if (m_path.empty()) return "";

  std::size_t lastSlash = m_path.find_last_of(PATH_SEPARATOR);
  if (lastSlash == std::string::npos) {
    return m_path;
  }

  return m_path.substr(lastSlash + 1);
}

std::string Path::getExtension() const {
  std::string filename = getFilename();
  if (filename.empty()) return "";

  std::size_t lastDot = filename.find_last_of('.');
  if (lastDot == std::string::npos || lastDot == 0) {
    return "";
  }

  return filename.substr(lastDot);
}

bool Path::isAbsolute() const { return m_isAbsolute; }

bool Path::isRelative() const { return !m_isAbsolute; }

bool Path::isEmpty() const { return m_path.empty(); }

bool Path::isDirectory() const {
  return !m_path.empty() && m_path[m_path.size() - 1] == PATH_SEPARATOR;
}

bool Path::isValidPath(const std::string& path, bool mustBeAbsolute) {
  try {
    validatePathString(path, mustBeAbsolute);
    return true;
  } catch (const exceptions::PathException&) {
    return false;
  }
}

bool Path::isSafePath(const std::string& path) {
  return !hasDirectoryTraversal(path);
}

Path Path::join(const std::string& subpath) const {
  if (subpath.empty()) {
    return *this;
  }

  if (!subpath.empty() && subpath[0] == PATH_SEPARATOR) {
    return Path(subpath, m_isAbsolute);
  }

  std::string newPath = m_path;
  if (!newPath.empty() && newPath[newPath.size() - 1] != PATH_SEPARATOR) {
    newPath += PATH_SEPARATOR;
  }
  newPath += subpath;

  return Path(newPath, m_isAbsolute);
}

Path Path::join(const Path& subpath) const { return join(subpath.toString()); }

Path Path::normalize() const {
  if (m_path.empty()) {
    return *this;
  }

  std::vector<std::string> components = splitComponents(m_path);
  std::string normalized = normalizeComponents(components);

  if (isDirectory() && !normalized.empty() &&
      normalized[normalized.size() - 1] != PATH_SEPARATOR) {
    normalized += PATH_SEPARATOR;
  }

  if (m_isAbsolute) {
    if (normalized.empty()) {
      normalized = std::string(1, PATH_SEPARATOR);
    } else if (normalized[0] != PATH_SEPARATOR) {
      normalized = PATH_SEPARATOR + normalized;
    }
  }

  return Path(normalized, m_isAbsolute);
}

Path Path::makeAbsolute(const Path& base) const {
  if (m_isAbsolute || base.isEmpty()) {
    return *this;
  }

  return base.join(*this).normalize();
}

Path Path::fromString(const std::string& path, bool mustBeAbsolute) {
  return Path(path, mustBeAbsolute);
}

Path Path::currentDirectory() { return Path(CURRENT_DIR, false); }

Path Path::rootDirectory() { return Path("/", true); }

bool Path::operator==(const Path& other) const {
  Path norm1 = this->normalize();
  Path norm2 = other.normalize();
  return norm1.m_path == norm2.m_path && norm1.m_isAbsolute == norm2.m_isAbsolute;
}

bool Path::operator!=(const Path& other) const { return !(*this == other); }

bool Path::operator<(const Path& other) const {
  Path norm1 = this->normalize();
  Path norm2 = other.normalize();
  return norm1.m_path < norm2.m_path;
}

Path Path::operator+(const std::string& suffix) const {
  return Path(m_path + suffix, m_isAbsolute);
}

Path Path::operator+(const Path& suffix) const {
  return Path(m_path + suffix.toString(), m_isAbsolute);
}

void Path::validate() const { validatePathString(m_path, m_isAbsolute); }

void Path::validatePathString(const std::string& path, bool mustBeAbsolute) {
  validateBasicProperties(path, mustBeAbsolute);

  validatePathSecurity(path);

  if (!path.empty() && path[path.length() - 1] != PATH_SEPARATOR) {
    validateFilenameProperties(path);
  }
}

void Path::validateBasicProperties(const std::string& path,
                                   bool mustBeAbsolute) {
  if (path.empty()) {
    throw exceptions::PathException(
        "Path cannot be empty", exceptions::PathException::EMPTY_PATH);
  }

  if (path.length() > MAX_PATH_LENGTH) {
    std::ostringstream oss;
    oss << "Path too long: " << path.length()
        << " characters (max: " << MAX_PATH_LENGTH << ")";
    throw exceptions::PathException(
        oss.str(), exceptions::PathException::TOO_LONG);
  }

  if (mustBeAbsolute && (path.empty() || path[0] != PATH_SEPARATOR)) {
    throw exceptions::PathException(
        "Path must be absolute: '" + path + "'",
        exceptions::PathException::NOT_ABSOLUTE);
  }

  if (!mustBeAbsolute && !path.empty() && path[0] == PATH_SEPARATOR) {
    throw exceptions::PathException(
        "Path must be relative: '" + path + "'",
        exceptions::PathException::NOT_RELATIVE);
  }
}

void Path::validatePathSecurity(const std::string& path) {
  if (containsInvalidChars(path)) {
    throw exceptions::PathException(
        "Path contains invalid characters: '" + path + "'",
        exceptions::PathException::INVALID_CHARACTER);
  }

  if (hasDirectoryTraversal(path)) {
    throw exceptions::PathException(
        "Path contains directory traversal: '" + path + "'",
        exceptions::PathException::TRAVERSAL_ATTEMPT);
  }
}

void Path::validateFilenameProperties(const std::string& path) {
  std::size_t lastSlash = path.find_last_of(PATH_SEPARATOR);
  std::string filename =
      (lastSlash == std::string::npos) ? path : path.substr(lastSlash + 1);

  if (filename.length() > MAX_FILENAME_LENGTH) {
    std::ostringstream oss;
    oss << "Filename too long: '" << filename << "' (" << filename.length()
        << " characters, max: " << MAX_FILENAME_LENGTH << ")";
    throw exceptions::PathException(
        oss.str(), exceptions::PathException::TOO_LONG);
  }

  if (filename.find_first_of(INVALID_FILENAME_CHARS) != std::string::npos) {
    throw exceptions::PathException(
        "Filename contains invalid characters: '" + filename + "'",
        exceptions::PathException::INVALID_FILENAME);
  }

  if (isReservedFilename(filename)) {
    throw exceptions::PathException(
        "Filename is reserved: '" + filename + "'",
        exceptions::PathException::INVALID_FILENAME);
  }
}

bool Path::containsInvalidChars(const std::string& path) {
  for (std::size_t i = 0; i < path.length(); ++i) {
    char chr = path[i];

    if (VALID_PATH_CHARS.find(chr) == std::string::npos) {
      if (std::isspace(static_cast<unsigned char>(chr)) == 0) {
        return true;
      }
    }
  }
  return false;
}

bool Path::isReservedFilename(const std::string& filename) {
  std::string upperFilename = filename;
  std::transform(upperFilename.begin(), upperFilename.end(),
                 upperFilename.begin(), ::toupper);

  for (int i = 0; !RESERVED_FILENAMES[i].empty(); ++i) {
    if (upperFilename == RESERVED_FILENAMES[i]) {
      return true;
    }
  }
  return false;
}

bool Path::hasDirectoryTraversal(const std::string& path) {
  std::vector<std::string> components = splitComponents(path);

  bool isAbsolute = (!path.empty() && path[0] == PATH_SEPARATOR);
  int depth = 0;
  
  for (std::size_t i = 0; i < components.size(); ++i) {
    if (components[i] == CURRENT_DIR) {
      continue;
    }
    if (components[i] == PARENT_DIR) {
      if (depth == 0 && isAbsolute) {
        return true;
      }
      if (depth > 0) {
        --depth;
      }
    } else if (!components[i].empty()) {
      ++depth;
    }
  }

  return false;
}

std::vector<std::string> Path::splitComponents(const std::string& path) {
  std::vector<std::string> components;
  if (path.empty()) return components;

  std::size_t start = 0;
  std::size_t end = path.find(PATH_SEPARATOR);

  while (end != std::string::npos) {
    if (end != start) {
      components.push_back(path.substr(start, end - start));
    }
    start = end + 1;
    end = path.find(PATH_SEPARATOR, start);
  }

  if (start < path.length()) {
    components.push_back(path.substr(start));
  }

  return components;
}

std::string Path::joinComponents(const std::vector<std::string>& components) {
  if (components.empty()) return "";

  std::ostringstream oss;
  for (std::size_t i = 0; i < components.size(); ++i) {
    if (i > 0) oss << PATH_SEPARATOR;
    oss << components[i];
  }

  return oss.str();
}

std::string Path::normalizeComponents(
    const std::vector<std::string>& components) {
  std::vector<std::string> normalized;

  for (std::size_t i = 0; i < components.size(); ++i) {
    if (components[i].empty() || components[i] == CURRENT_DIR) {
      continue;
    }
    if (components[i] == PARENT_DIR) {
      if (!normalized.empty() && normalized.back() != PARENT_DIR) {
        normalized.pop_back();
      } else {
        normalized.push_back(components[i]);
      }
    } else {
      normalized.push_back(components[i]);
    }
  }

  return joinComponents(normalized);
}

}  // namespace value_objects
}  // namespace filesystem
}  // namespace domain
