/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   DirectoryLister.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/22 21:42:37 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/23 01:13:12 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DIRECTORYLISTER_HPP
#define DIRECTORYLISTER_HPP

#include "domain/value_objects/Path.hpp"
#include "domain/value_objects/Permission.hpp"
#include "domain/value_objects/RegexPattern.hpp"
#include "domain/value_objects/Size.hpp"
#include "infrastructure/filesystem/FileSystemHelper.hpp"
#include "infrastructure/filesystem/PathResolver.hpp"

#include <string>
#include <vector>

namespace infrastructure {
namespace filesystem {

struct DirectoryEntry {
  std::string m_name;
  bool m_isDirectory;
  domain::value_objects::Size m_size;
  std::string m_lastModified;
  domain::value_objects::Permission m_permissions;
  domain::value_objects::Path m_fullPath;

  DirectoryEntry() : m_isDirectory(false) {}
  DirectoryEntry(const std::string& name, bool isDir,
                 const domain::value_objects::Size& size,
                 const std::string& mod,
                 const domain::value_objects::Permission& perm,
                 const domain::value_objects::Path& path)
      : m_name(name),
        m_isDirectory(isDir),
        m_size(size),
        m_lastModified(mod),
        m_permissions(perm),
        m_fullPath(path) {}
};

class DirectoryLister {
 public:
  explicit DirectoryLister(FileSystemHelper* fileSystemHelper,
                           PathResolver* pathResolver);
  virtual ~DirectoryLister();

  DirectoryLister(const DirectoryLister& other);
  DirectoryLister& operator=(const DirectoryLister& other);

  static std::vector<DirectoryEntry> listDirectory(
      const domain::value_objects::Path& directoryPath, bool showHidden = false,
      const std::string& sortBy = "name", bool ascending = true);

  static std::string generateHtmlListing(
      const domain::value_objects::Path& directoryPath,
      const domain::value_objects::Path& requestPath, bool showHidden = false,
      const std::string& sortBy = "name", bool ascending = true);

  std::string generateJsonListing(
      const domain::value_objects::Path& directoryPath,
      bool showHidden = false) const;

  std::string generatePlainTextListing(
      const domain::value_objects::Path& directoryPath,
      bool showHidden = false) const;

  static bool isDirectoryListingEnabled(
      const domain::value_objects::Path& directoryPath);

  static bool validateDirectoryForListing(
      const domain::value_objects::Path& directoryPath);

  static domain::value_objects::Path getParentDirectoryPath(
      const domain::value_objects::Path& currentPath);

  static std::string getParentDirectoryUri(
      const domain::value_objects::Path& currentPath,
      const domain::value_objects::Path& requestPath);

  static bool isHiddenFile(const std::string& filename);
  static bool isExecutableFile(const domain::value_objects::Permission& perm);
  static bool isImageFile(const std::string& filename);
  static bool isScriptFile(const std::string& filename);

  static bool hasAccess(const DirectoryEntry& entry,
                        domain::value_objects::Permission::Class userClass);

 private:
  FileSystemHelper* m_fileSystemHelper;
  PathResolver* m_pathResolver;

  static const domain::value_objects::RegexPattern IMAGE_EXTENSION_PATTERN;
  static const domain::value_objects::RegexPattern SCRIPT_EXTENSION_PATTERN;
  static const std::string DEFAULT_INDEX_FILES[];
  static const std::size_t DEFAULT_INDEX_FILES_COUNT = 5;

  static const std::size_t PLAIN_TEXT_LINE_WIDTH = 60;
  static const std::size_t PLAIN_TEXT_COLUMN_WIDTH_NAME = 12;
  static const std::size_t PLAIN_TEXT_COLUMN_WIDTH_SIZE = 12;
  static const std::size_t PLAIN_TEXT_COLUMN_WIDTH_DATE = 20;
  static const std::size_t LAST_MODIFIED_BUFFER_SIZE = 80;

  static std::vector<DirectoryEntry> readDirectoryEntries(
      const domain::value_objects::Path& directoryPath, bool showHidden);

  static void sortEntries(std::vector<DirectoryEntry>& entries,
                          const std::string& sortBy, bool ascending);

  static bool shouldSkipEntry(const std::string& entryName, bool showHidden);

  static DirectoryEntry createEntryFromStat(
      const std::string& entryName,
      const domain::value_objects::Path& fullPath);

  static std::string getLastModifiedTime(
      const domain::value_objects::Path& path);
  static domain::value_objects::Permission getFilePermissions(
      const domain::value_objects::Path& path);
  static domain::value_objects::Size getFileSize(
      const domain::value_objects::Path& path);

  static std::string generateDirectoryEntryHtml(
      const DirectoryEntry& entry,
      const domain::value_objects::Path& requestPath,
      bool isParentLink = false);

  static std::string generateTableHeaderHtml(
      const std::string& sortBy, bool ascending,
      const domain::value_objects::Path& requestPath);
  static std::string generateSortUrl(
      const domain::value_objects::Path& requestPath,
      const std::string& currentSortBy,
      const std::string& columnToSort,
      bool isAscending);

  static std::string generateIconHtml(const DirectoryEntry& entry);
  static std::string generateFileTypeClass(const DirectoryEntry& entry);
};

}  // namespace filesystem
}  // namespace infrastructure

#endif  // DIRECTORYLISTER_HPP
