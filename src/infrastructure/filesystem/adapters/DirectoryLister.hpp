/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   DirectoryLister.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: umeneses <umeneses@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/22 21:42:37 by dande-je          #+#    #+#             */
/*   Updated: 2026/01/08 11:50:27 by umeneses         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DIRECTORYLISTER_HPP
#define DIRECTORYLISTER_HPP

#include "domain/filesystem/value_objects/Path.hpp"
#include "domain/filesystem/value_objects/Permission.hpp"
#include "domain/filesystem/value_objects/Size.hpp"
#include "domain/shared/value_objects/RegexPattern.hpp"
#include "infrastructure/filesystem/adapters/FileSystemHelper.hpp"
#include "infrastructure/filesystem/adapters/PathResolver.hpp"

#include <string>
#include <vector>

namespace infrastructure {
namespace filesystem {
namespace adapters {

struct DirectoryEntry {
  std::string m_name;
  bool m_isDirectory;
  domain::filesystem::value_objects::Size m_size;
  std::string m_lastModified;
  domain::filesystem::value_objects::Permission m_permissions;
  domain::filesystem::value_objects::Path m_fullPath;

  DirectoryEntry() : m_isDirectory(false) {}
  DirectoryEntry(const std::string& name, bool isDir,
                 const domain::filesystem::value_objects::Size& size,
                 const std::string& mod,
                 const domain::filesystem::value_objects::Permission& perm,
                 const domain::filesystem::value_objects::Path& path)
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
      const domain::filesystem::value_objects::Path& directoryPath,
      bool showHidden = false, const std::string& sortBy = "name",
      bool ascending = true);

  static std::string generateHtmlListing(
      const domain::filesystem::value_objects::Path& directoryPath,
      const domain::filesystem::value_objects::Path& requestPath,
      bool showHidden = false, const std::string& sortBy = "name",
      bool ascending = true);

  std::string generateJsonListing(
      const domain::filesystem::value_objects::Path& directoryPath,
      bool showHidden = false) const;

  std::string generatePlainTextListing(
      const domain::filesystem::value_objects::Path& directoryPath,
      bool showHidden = false) const;

  static bool isDirectoryListingEnabled(
      const domain::filesystem::value_objects::Path& directoryPath);

  static bool validateDirectoryForListing(
      const domain::filesystem::value_objects::Path& directoryPath);

  static domain::filesystem::value_objects::Path getParentDirectoryPath(
      const domain::filesystem::value_objects::Path& currentPath);

  static std::string getParentDirectoryUri(
      const domain::filesystem::value_objects::Path& currentPath,
      const domain::filesystem::value_objects::Path& requestPath);

  static bool isHiddenFile(const std::string& filename);
  static bool isExecutableFile(
      const domain::filesystem::value_objects::Permission& perm);
  static bool isImageFile(const std::string& filename);
  static bool isScriptFile(const std::string& filename);

  static bool hasAccess(
      const DirectoryEntry& entry,
      domain::filesystem::value_objects::Permission::Class userClass);

 private:
  FileSystemHelper* m_fileSystemHelper;
  PathResolver* m_pathResolver;

  static const domain::shared::value_objects::RegexPattern&
  getImageExtensionPattern();
  static const domain::shared::value_objects::RegexPattern&
  getScriptExtensionPattern();
  static const std::string* getDefaultIndexFiles();
  static const std::size_t DEFAULT_INDEX_FILES_COUNT = 5;

  static const std::size_t PLAIN_TEXT_LINE_WIDTH = 60;
  static const std::size_t PLAIN_TEXT_COLUMN_WIDTH_NAME = 12;
  static const std::size_t PLAIN_TEXT_COLUMN_WIDTH_SIZE = 12;
  static const std::size_t PLAIN_TEXT_COLUMN_WIDTH_DATE = 20;
  static const std::size_t LAST_MODIFIED_BUFFER_SIZE = 80;

  static std::vector<DirectoryEntry> readDirectoryEntries(
      const domain::filesystem::value_objects::Path& directoryPath,
      bool showHidden);

  static void sortEntries(std::vector<DirectoryEntry>& entries,
                          const std::string& sortBy, bool ascending);

  static bool shouldSkipEntry(const std::string& entryName, bool showHidden);

  static DirectoryEntry createEntryFromStat(
      const std::string& entryName,
      const domain::filesystem::value_objects::Path& fullPath);

  static std::string getLastModifiedTime(
      const domain::filesystem::value_objects::Path& path);
  static domain::filesystem::value_objects::Permission getFilePermissions(
      const domain::filesystem::value_objects::Path& path);
  static domain::filesystem::value_objects::Size getFileSize(
      const domain::filesystem::value_objects::Path& path);

  static std::string generateDirectoryEntryHtml(
      const DirectoryEntry& entry,
      const domain::filesystem::value_objects::Path& requestPath,
      bool isParentLink = false);

  static std::string generateTableHeaderHtml(
      const std::string& sortBy, bool ascending,
      const domain::filesystem::value_objects::Path& requestPath);
  static std::string generateSortUrl(
      const domain::filesystem::value_objects::Path& requestPath,
      const std::string& currentSortBy, const std::string& columnToSort,
      bool isAscending);

  static std::string generateIconHtml(const DirectoryEntry& entry);
  static std::string generateFileTypeClass(const DirectoryEntry& entry);
};

}  // namespace adapters
}  // namespace filesystem
}  // namespace infrastructure

#endif  // DIRECTORYLISTER_HPP
