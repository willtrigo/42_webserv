/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Path.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: umeneses <umeneses@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/20 01:07:22 by dande-je          #+#    #+#             */
/*   Updated: 2026/01/03 15:41:21 by umeneses         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PATH_HPP
#define PATH_HPP

#include <string>
#include <vector>

namespace domain {
namespace filesystem {
namespace value_objects {

class Path {
 public:
  static const std::size_t MAX_PATH_LENGTH = 4096;
  static const std::size_t MAX_FILENAME_LENGTH = 255;
  static const char PATH_SEPARATOR;
  static const std::string CURRENT_DIR;
  static const std::string PARENT_DIR;

  static const std::string VALID_PATH_CHARS;
  static const std::string INVALID_FILENAME_CHARS;
  
  Path();
  explicit Path(const std::string& path, bool mustBeAbsolute = true);
  Path(const Path& other);
  ~Path();

  Path& operator=(const Path& other);

  std::string toString() const;
  std::string getDirectory() const;
  std::string getFilename() const;
  std::string getExtension() const;
  bool isAbsolute() const;
  bool isRelative() const;
  bool isEmpty() const;
  bool isDirectory() const;

  static bool isValidPath(const std::string& path, bool mustBeAbsolute = true);
  static bool isSafePath(const std::string& path);

  Path join(const std::string& subpath) const;
  Path join(const Path& subpath) const;
  Path normalize() const;
  Path makeAbsolute(const Path& base) const;

  static Path fromString(const std::string& path, bool mustBeAbsolute = true);
  static Path currentDirectory();
  static Path rootDirectory();

  bool operator==(const Path& other) const;
  bool operator!=(const Path& other) const;
  bool operator<(const Path& other) const;

  Path operator+(const std::string& suffix) const;
  Path operator+(const Path& suffix) const;

 private:
  std::string m_path;
  bool m_isAbsolute;

  void validate() const;
  static void validatePathString(const std::string& path, bool mustBeAbsolute);
  static void validateBasicProperties(const std::string& path,
                                      bool mustBeAbsolute);
  static void validatePathSecurity(const std::string& path);
  static void validateFilenameProperties(const std::string& path);

  static bool containsInvalidChars(const std::string& path);
  static bool isValidPathCharacter(char chr);
  static bool isDangerousCharacter(char chr);
  static bool isReservedFilename(const std::string& filename);
  static bool hasDirectoryTraversal(const std::string& path);
  static std::string urlDecode(const std::string& str);

  static std::vector<std::string> splitComponents(const std::string& path);
  static std::string joinComponents(const std::vector<std::string>& components);
  static std::string normalizeComponents(
      const std::vector<std::string>& components);
};

}  // namespace value_objects
}  // namespace filesystem
}  // namespace domain

#endif  // PATH_HPP
