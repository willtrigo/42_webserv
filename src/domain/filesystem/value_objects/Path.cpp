/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Path.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: umeneses <umeneses@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/20 01:10:52 by dande-je          #+#    #+#             */
/*   Updated: 2026/01/07 03:42:57 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "domain/filesystem/exceptions/PathException.hpp"
#include "domain/filesystem/value_objects/Path.hpp"
#include "domain/shared/utils/StringUtils.hpp"

#include <cctype>
#include <cstdlib>
#include <cstring>
#include <sstream>

namespace domain {
namespace filesystem {
namespace value_objects {

namespace {
const char ASCII_SPACE = ' ';
const char ASCII_NULL = '\0';
const char ASCII_NEWLINE = '\n';
const char ASCII_CARRIAGE_RETURN = '\r';
const char ASCII_TAB = '\t';
const char ASCII_BACKSLASH = '\\';
const char ASCII_COLON = ':';
const char ASCII_ASTERISK = '*';
const char ASCII_QUESTION = '?';
const char ASCII_QUOTE = '"';
const char ASCII_LESS_THAN = '<';
const char ASCII_GREATER_THAN = '>';
const char ASCII_PIPE = '|';
const char ASCII_DOT = '.';
const char ASCII_HYPHEN = '-';
const char ASCII_UNDERSCORE = '_';

const unsigned char ASCII_PRINTABLE_MIN = 0x20;
const unsigned char ASCII_PRINTABLE_MAX = 0x7E;

const char NUMERIC_MIN = '0';
const char NUMERIC_MAX = '9';
const char LOWERCASE_MIN = 'a';
const char LOWERCASE_MAX = 'z';
const char UPPERCASE_MIN = 'A';
const char UPPERCASE_MAX = 'Z';

const char* RESERVED_FILENAMES[] = {
    ".",    "..",   "CON",  "PRN",  "AUX",  "NUL",  "COM1", "COM2", "COM3",
    "COM4", "COM5", "COM6", "COM7", "COM8", "COM9", "LPT1", "LPT2", "LPT3",
    "LPT4", "LPT5", "LPT6", "LPT7", "LPT8", "LPT9", NULL};
}  // namespace

const char Path::PATH_SEPARATOR = '/';
const std::string Path::CURRENT_DIR = ".";
const std::string Path::PARENT_DIR = "..";
const std::string Path::INVALID_FILENAME_CHARS = "/\\:*?\"<>|";

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

  // Remove trailing slash if present (directory paths)
  std::string pathWithoutTrailing = m_path;
  if (pathWithoutTrailing.length() > 1 && 
      pathWithoutTrailing[pathWithoutTrailing.length() - 1] == PATH_SEPARATOR) {
    pathWithoutTrailing = pathWithoutTrailing.substr(0, pathWithoutTrailing.length() - 1);
  }

  std::size_t lastSlash = pathWithoutTrailing.find_last_of(PATH_SEPARATOR);
  if (lastSlash == std::string::npos) {
    return "";
  }
  if (lastSlash == 0) {
    return std::string(1, PATH_SEPARATOR);
  }

  return pathWithoutTrailing.substr(0, lastSlash);
}

std::string Path::getFilename() const {
  if (m_path.empty()) return "";

  // Remove trailing slash if present (directory paths)
  std::string pathWithoutTrailing = m_path;
  if (pathWithoutTrailing.length() > 1 && 
      pathWithoutTrailing[pathWithoutTrailing.length() - 1] == PATH_SEPARATOR) {
    pathWithoutTrailing = pathWithoutTrailing.substr(0, pathWithoutTrailing.length() - 1);
  }

  std::size_t lastSlash = pathWithoutTrailing.find_last_of(PATH_SEPARATOR);
  if (lastSlash == std::string::npos) {
    return pathWithoutTrailing;
  }

  return pathWithoutTrailing.substr(lastSlash + 1);
}

std::string Path::getExtension() const {
  std::string filename = getFilename();
  if (filename.empty()) return "";

  std::size_t lastDot = filename.find_last_of(ASCII_DOT);
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

  // If subpath is absolute (starts with /), treat it as an absolute path replacement
  if (!subpath.empty() && subpath[0] == PATH_SEPARATOR) {
    // If subpath starts with /, it should be treated as absolute
    return Path(subpath, true);
  }

  // Handle empty base path
  if (m_path.empty()) {
    // Join with empty base: return subpath with same absolute/relative status
    return Path(subpath, true);
  }

  std::string newPath = m_path;
  
  // Add separator if base path doesn't end with one
  if (newPath[newPath.size() - 1] != PATH_SEPARATOR) {
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
  return norm1.m_path == norm2.m_path &&
         norm1.m_isAbsolute == norm2.m_isAbsolute;
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
    throw exceptions::PathException("Path cannot be empty",
                                    exceptions::PathException::EMPTY_PATH);
  }

  if (path.length() > MAX_PATH_LENGTH) {
    std::ostringstream oss;
    oss << "Path too long: " << path.length()
        << " characters (max: " << MAX_PATH_LENGTH << ")";
    throw exceptions::PathException(oss.str(),
                                    exceptions::PathException::TOO_LONG);
  }

  if (mustBeAbsolute && (path.empty() || path[0] != PATH_SEPARATOR)) {
    throw exceptions::PathException("Path must be absolute: '" + path + "'",
                                    exceptions::PathException::NOT_ABSOLUTE);
  }
}

void Path::validatePathSecurity(const std::string& path) {
  if (containsInvalidChars(path)) {
    throw exceptions::PathException(
        "Path contains invalid characters: '" + path + "'",
        exceptions::PathException::INVALID_CHARACTER);
  }
  // Note: Directory traversal (../) is allowed in paths
  // Use isSafePath() to check for security concerns
}

void Path::validateFilenameProperties(const std::string& path) {
  std::size_t lastSlash = path.find_last_of(PATH_SEPARATOR);
  std::string filename =
      (lastSlash == std::string::npos) ? path : path.substr(lastSlash + 1);

  if (filename.length() > MAX_FILENAME_LENGTH) {
    std::ostringstream oss;
    oss << "Filename too long: '" << filename << "' (" << filename.length()
        << " characters, max: " << MAX_FILENAME_LENGTH << ")";
    throw exceptions::PathException(oss.str(),
                                    exceptions::PathException::TOO_LONG);
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
    const char chr = path[i];

    if (!isValidPathCharacter(chr)) {
      return true;
    }
  }
  return false;
}

bool Path::isValidPathCharacter(char chr) {
  const bool isNumeric = (chr >= NUMERIC_MIN && chr <= NUMERIC_MAX);
  const bool isLowercase = (chr >= LOWERCASE_MIN && chr <= LOWERCASE_MAX);
  const bool isUppercase = (chr >= UPPERCASE_MIN && chr <= UPPERCASE_MAX);
  const bool isAlphaNum = isNumeric || isLowercase || isUppercase;
  const bool isPathSeparator = (chr == PATH_SEPARATOR);
  const bool isSafeSpecial = (chr == ASCII_DOT || chr == ASCII_HYPHEN ||
                              chr == ASCII_UNDERSCORE || chr == ASCII_SPACE);

  if (isAlphaNum || isPathSeparator || isSafeSpecial) {
    return true;
  }

  if (chr < ASCII_PRINTABLE_MIN || chr > ASCII_PRINTABLE_MAX) {
    return false;
  }

  return !isDangerousCharacter(chr);
}

bool Path::isDangerousCharacter(char chr) {
  return chr == ASCII_NULL || chr == ASCII_NEWLINE ||
         chr == ASCII_CARRIAGE_RETURN || chr == ASCII_BACKSLASH ||
         chr == ASCII_COLON || chr == ASCII_ASTERISK || chr == ASCII_QUESTION ||
         chr == ASCII_QUOTE || chr == ASCII_LESS_THAN ||
         chr == ASCII_GREATER_THAN || chr == ASCII_PIPE || chr == ASCII_TAB;
}

bool Path::isReservedFilename(const std::string& filename) {
  const std::string upperFilename =
      shared::utils::StringUtils::toUpperCase(filename);

  for (int i = 0; RESERVED_FILENAMES[i] != NULL; ++i) {
    if (upperFilename == RESERVED_FILENAMES[i]) {
      return true;
    }
  }
  return false;
}

std::string Path::urlDecode(const std::string& str) {
  std::string result;
  result.reserve(str.length());
  
  for (std::size_t i = 0; i < str.length(); ++i) {
    if (str[i] == '%' && i + 2 < str.length()) {
      // Try to decode hex sequence
      char hex[3] = {str[i + 1], str[i + 2], '\0'};
      char* endptr;
      long value = std::strtol(hex, &endptr, 16);
      
      if (endptr == hex + 2) {  // Successfully decoded 2 hex digits
        result += static_cast<char>(value);
        i += 2;  // Skip the two hex digits
        continue;
      }
    }
    result += str[i];
  }
  
  return result;
}

bool Path::hasDirectoryTraversal(const std::string& path) {
  // Decode URL-encoded characters first
  std::string decodedPath = urlDecode(path);
  
  std::vector<std::string> components = splitComponents(decodedPath);

  for (std::size_t i = 0; i < components.size(); ++i) {
    if (components[i] == CURRENT_DIR) {
      continue;
    }
    if (components[i] == PARENT_DIR) {
      // Any .. in path is considered traversal (even if safe mathematically)
      // This is for security - we want to reject any path with ..
      return true;
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
