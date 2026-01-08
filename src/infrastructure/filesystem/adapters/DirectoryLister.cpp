/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   DirectoryLister.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: umeneses <umeneses@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/22 21:48:36 by dande-je          #+#    #+#             */
/*   Updated: 2026/01/08 11:50:27 by umeneses         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "domain/filesystem/value_objects/Path.hpp"
#include "domain/filesystem/value_objects/Permission.hpp"
#include "domain/filesystem/value_objects/Size.hpp"
#include "domain/http/value_objects/QueryStringBuilder.hpp"
#include "domain/shared/value_objects/RegexPattern.hpp"
#include "infrastructure/filesystem/adapters/DirectoryEntryComparators.hpp"
#include "infrastructure/filesystem/adapters/DirectoryLister.hpp"
#include "infrastructure/filesystem/exceptions/DirectoryListerException.hpp"

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <dirent.h>
#include <iomanip>
#include <sstream>
#include <sys/stat.h>

namespace infrastructure {
namespace filesystem {
namespace adapters {

// Meyer's Singleton pattern - guarantees initialization on first use
// This fixes the static initialization order fiasco
const domain::shared::value_objects::RegexPattern&
DirectoryLister::getImageExtensionPattern() {
  static const domain::shared::value_objects::RegexPattern pattern =
      domain::shared::value_objects::RegexPattern::imageExtensions();
  return pattern;
}

const domain::shared::value_objects::RegexPattern&
DirectoryLister::getScriptExtensionPattern() {
  static const domain::shared::value_objects::RegexPattern pattern(
      "\\.(php|py|pl|sh|cgi)$",
      domain::shared::value_objects::RegexPattern::FLAG_CASE_INSENSITIVE);
  return pattern;
}

const std::string* DirectoryLister::getDefaultIndexFiles() {
  static const std::string files[] = {"index.html", "index.htm", "index.php",
                                      "default.html", "default.htm"};
  return files;
}

DirectoryLister::DirectoryLister(FileSystemHelper* fileSystemHelper,
                                 PathResolver* pathResolver)
    : m_fileSystemHelper(fileSystemHelper), m_pathResolver(pathResolver) {
  if (fileSystemHelper == NULL) {
    throw exceptions::DirectoryListerException(
        "FileSystemHelper cannot be NULL",
        exceptions::DirectoryListerException::INVALID_DEPENDENCY);
  }
  if (pathResolver == NULL) {
    throw exceptions::DirectoryListerException(
        "PathResolver cannot be NULL",
        exceptions::DirectoryListerException::INVALID_DEPENDENCY);
  }
}

DirectoryLister::~DirectoryLister() {}

DirectoryLister::DirectoryLister(const DirectoryLister& other)
    : m_fileSystemHelper(other.m_fileSystemHelper),
      m_pathResolver(other.m_pathResolver) {}

DirectoryLister& DirectoryLister::operator=(const DirectoryLister& other) {
  if (this != &other) {
    m_fileSystemHelper = other.m_fileSystemHelper;
    m_pathResolver = other.m_pathResolver;
  }
  return *this;
}

std::vector<DirectoryEntry> DirectoryLister::listDirectory(
    const domain::filesystem::value_objects::Path& directoryPath,
    bool showHidden, const std::string& sortBy, bool ascending) {
  if (!validateDirectoryForListing(directoryPath)) {
    throw exceptions::DirectoryListerException(
        "Directory not valid for listing: " + directoryPath.toString(),
        exceptions::DirectoryListerException::INVALID_DIRECTORY);
  }

  std::vector<DirectoryEntry> entries =
      readDirectoryEntries(directoryPath, showHidden);

  if (!entries.empty()) {
    sortEntries(entries, sortBy, ascending);
  }

  return entries;
}

std::string DirectoryLister::generateHtmlListing(
    const domain::filesystem::value_objects::Path& directoryPath,
    const domain::filesystem::value_objects::Path& requestPath, bool showHidden,
    const std::string& sortBy, bool ascending) {
  std::vector<DirectoryEntry> entries =
      listDirectory(directoryPath, showHidden, sortBy, ascending);

  std::ostringstream html;
  html << "<!DOCTYPE html>\n";
  html << "<html>\n";
  html << "<head>\n";
  html << "  <title>Index of " << requestPath.toString() << "</title>\n";
  html << "  <style>\n";
  html << "    body { font-family: Arial, sans-serif; margin: 40px; }\n";
  html << "    h1 { color: #333; }\n";
  html << "    table { border-collapse: collapse; width: 100%; }\n";
  html << "    th, td { border: 1px solid #ddd; padding: 8px; text-align: "
          "left; }\n";
  html << "    th { background-color: #f2f2f2; cursor: pointer; }\n";
  html << "    th:hover { background-color: #e8e8e8; }\n";
  html << "    tr:nth-child(even) { background-color: #f9f9f9; }\n";
  html << "    tr:hover { background-color: #f5f5f5; }\n";
  html << "    a { text-decoration: none; color: #0066cc; }\n";
  html << "    a:hover { text-decoration: underline; }\n";
  html << "    .icon { margin-right: 5px; width: 16px; text-align: center; "
          "display: inline-block; }\n";
  html << "    .size { text-align: right; }\n";
  html << "    .date { white-space: nowrap; }\n";
  html << "    .permissions { font-family: monospace; }\n";
  html << "    .directory { color: #0066cc; }\n";
  html << "    .executable { color: #00cc00; }\n";
  html << "    .image { color: #cc00cc; }\n";
  html << "    .script { color: #cc6600; }\n";
  html << "    .hidden { color: #999; font-style: italic; }\n";
  html << "    .sort-indicator { margin-left: 5px; font-weight: bold; }\n";
  html << "    .header-link { display: block; width: 100%; height: 100%; "
          "color: inherit; text-decoration: none; }\n";
  html << "  </style>\n";
  html << "  <script>\n";
  html << "    function toggleHidden() {\n";
  html << "      const url = new URL(window.location.href);\n";
  html << "      const showHidden = url.searchParams.get('hidden') === "
          "'true';\n";
  html << "      url.searchParams.set('hidden', !showHidden);\n";
  html << "      window.location.href = url.toString();\n";
  html << "    }\n";
  html << "  </script>\n";
  html << "</head>\n";
  html << "<body>\n";
  html << "  <h1>Index of " << requestPath.toString() << "</h1>\n";

  html << "  <div style=\"margin-bottom: 10px;\">\n";
  html << "    <button onclick=\"toggleHidden()\" style=\"padding: 5px 10px; "
          "margin-right: 10px;\">\n";
  html << "      " << (showHidden ? "Hide" : "Show") << " Hidden Files\n";
  html << "    </button>\n";
  html << "  </div>\n";

  html << "  <hr>\n";
  html << "  <table>\n";

  html << generateTableHeaderHtml(sortBy, ascending, requestPath);

  html << "    <tbody>\n";

  domain::filesystem::value_objects::Path parentPath =
      getParentDirectoryPath(directoryPath);
  if (!parentPath.isEmpty() &&
      parentPath.toString() != directoryPath.toString()) {
    DirectoryEntry parentEntry(
        "..", true, domain::filesystem::value_objects::Size::zero(), "",
        domain::filesystem::value_objects::Permission::readOnly(), parentPath);
    html << generateDirectoryEntryHtml(parentEntry, requestPath, true);
  }

  for (std::size_t i = 0; i < entries.size(); ++i) {
    html << generateDirectoryEntryHtml(entries[i], requestPath);
  }

  html << "    </tbody>\n";
  html << "  </table>\n";
  html << "  <hr>\n";

  domain::filesystem::value_objects::Size totalSize(0);
  int fileCount = 0;
  int dirCount = 0;
  int hiddenCount = 0;

  for (std::size_t i = 0; i < entries.size(); ++i) {
    if (entries[i].m_isDirectory) {
      dirCount++;
    } else {
      fileCount++;
      totalSize = totalSize + entries[i].m_size;
    }

    if (isHiddenFile(entries[i].m_name)) {
      hiddenCount++;
    }
  }

  html << "  <div style=\"color: #666; font-size: 0.9em;\">\n";
  html << "    " << entries.size() << " entries";
  if (fileCount > 0) {
    html << " (" << fileCount << " files";
    if (!totalSize.isZero()) {
      html << ", " << totalSize.toString() << " total";
    }
    html << ")";
  }
  if (dirCount > 0) {
    html << " (" << dirCount << " directories)";
  }
  if (hiddenCount > 0 && showHidden) {
    html << " (" << hiddenCount << " hidden)";
  }
  html << "\n";
  html << "  </div>\n";

  html << "  <div style=\"color: #888; font-size: 0.8em; margin-top: 5px;\">\n";
  html << "    Sorted by: " << sortBy << " ("
       << (ascending ? "ascending" : "descending") << ")\n";
  html << "  </div>\n";

  html << "  <address>";
  // html << m_fileSystemHelper->getServerSignature();
  html << " at " << requestPath.toString() << "</address>\n";
  html << "</body>\n";
  html << "</html>\n";

  return html.str();
}

std::string DirectoryLister::generateJsonListing(
    const domain::filesystem::value_objects::Path& directoryPath,
    bool showHidden) const {
  std::vector<DirectoryEntry> entries =
      listDirectory(directoryPath, showHidden);

  std::ostringstream json;
  json << "{\n";
  json << "  \"path\": \"" << directoryPath.toString() << "\",\n";
  json << "  \"entries\": [\n";

  for (std::size_t i = 0; i < entries.size(); ++i) {
    const DirectoryEntry& entry = entries[i];
    json << "    {\n";
    json << "      \"name\": \"" << entry.m_name << "\",\n";
    json << "      \"type\": \"" << (entry.m_isDirectory ? "directory" : "file")
         << "\",\n";
    json << "      \"size\": " << entry.m_size.getBytes() << ",\n";
    json << "      \"sizeFormatted\": \"" << entry.m_size.toString() << "\",\n";
    json << "      \"lastModified\": \"" << entry.m_lastModified << "\",\n";
    json << "      \"permissions\": \"" << entry.m_permissions.toString()
         << "\",\n";
    json << "      \"permissionsSymbolic\": \""
         << entry.m_permissions.toSymbolicString() << "\",\n";
    json << "      \"fullPath\": \"" << entry.m_fullPath.toString() << "\"\n";
    json << "    }";

    if (i < entries.size() - 1) {
      json << ",";
    }
    json << "\n";
  }

  json << "  ]\n";
  json << "}\n";

  return json.str();
}

std::string DirectoryLister::generatePlainTextListing(
    const domain::filesystem::value_objects::Path& directoryPath,
    bool showHidden) const {
  std::vector<DirectoryEntry> entries =
      listDirectory(directoryPath, showHidden);

  std::ostringstream text;
  text << "Index of " << directoryPath.toString() << "\n";
  for (std::size_t i = 0; i < PLAIN_TEXT_LINE_WIDTH; ++i) {
    text << "-";
  }
  text << "\n";

  for (std::size_t i = 0; i < entries.size(); ++i) {
    const DirectoryEntry& entry = entries[i];

    text << std::setw(PLAIN_TEXT_COLUMN_WIDTH_NAME) << std::left
         << entry.m_permissions.toSymbolicString() << " ";
    text << std::setw(PLAIN_TEXT_COLUMN_WIDTH_SIZE) << std::right
         << entry.m_size.toString() << " ";
    text << std::setw(PLAIN_TEXT_COLUMN_WIDTH_DATE) << std::left
         << entry.m_lastModified << " ";

    if (entry.m_isDirectory) {
      text << entry.m_name << "/";
    } else {
      text << entry.m_name;
    }

    text << "\n";
  }

  for (std::size_t i = 0; i < PLAIN_TEXT_LINE_WIDTH; ++i) {
    text << "-";
  }
  text << "\n";

  int fileCount = 0;
  int dirCount = 0;
  domain::filesystem::value_objects::Size totalSize(0);

  for (std::size_t i = 0; i < entries.size(); ++i) {
    if (entries[i].m_isDirectory) {
      dirCount++;
    } else {
      fileCount++;
      totalSize = totalSize + entries[i].m_size;
    }
  }

  text << "Total: " << entries.size() << " entries";
  if (fileCount > 0) {
    text << " (" << fileCount << " files";
    if (!totalSize.isZero()) {
      text << ", " << totalSize.toString() << ")";
    } else {
      text << ")";
    }
  }
  if (dirCount > 0) {
    text << " (" << dirCount << " directories)";
  }
  text << "\n";

  return text.str();
}

bool DirectoryLister::isDirectoryListingEnabled(
    const domain::filesystem::value_objects::Path& directoryPath) {
  // In a real implementation, this would check configuration
  // For now, we'll assume it's enabled if directory exists and is readable
  if (!FileSystemHelper::exists(directoryPath.toString())) {
    return false;
  }

  if (!FileSystemHelper::isDirectory(directoryPath.toString())) {
    return false;
  }

  return FileSystemHelper::isReadable(directoryPath.toString());
}

bool DirectoryLister::validateDirectoryForListing(
    const domain::filesystem::value_objects::Path& directoryPath) {
  if (!FileSystemHelper::exists(directoryPath.toString())) {
    throw exceptions::DirectoryListerException(
        "Directory does not exist: " + directoryPath.toString(),
        exceptions::DirectoryListerException::DIRECTORY_NOT_FOUND);
  }

  if (!FileSystemHelper::isDirectory(directoryPath.toString())) {
    throw exceptions::DirectoryListerException(
        "Path is not a directory: " + directoryPath.toString(),
        exceptions::DirectoryListerException::NOT_A_DIRECTORY);
  }

  if (!FileSystemHelper::isReadable(directoryPath.toString())) {
    throw exceptions::DirectoryListerException(
        "Directory not readable: " + directoryPath.toString(),
        exceptions::DirectoryListerException::PERMISSION_DENIED);
  }

  return true;
}

domain::filesystem::value_objects::Path DirectoryLister::getParentDirectoryPath(
    const domain::filesystem::value_objects::Path& currentPath) {
  if (currentPath.isEmpty() || currentPath.toString() == "/") {
    return domain::filesystem::value_objects::Path();
  }

  std::string currentStr = currentPath.normalize().toString();

  if (!currentStr.empty() && currentStr[currentStr.length() - 1] == '/') {
    currentStr = currentStr.substr(0, currentStr.length() - 1);
  }

  std::size_t lastSlash = currentStr.find_last_of('/');
  if (lastSlash == std::string::npos) {
    return domain::filesystem::value_objects::Path();
  }

  if (lastSlash == 0) {
    return domain::filesystem::value_objects::Path::rootDirectory();
  }

  return domain::filesystem::value_objects::Path(
      currentStr.substr(0, lastSlash), true);
}

std::string DirectoryLister::getParentDirectoryUri(
    const domain::filesystem::value_objects::Path& currentPath,
    const domain::filesystem::value_objects::Path& requestPath) {
  domain::filesystem::value_objects::Path parentPath =
      getParentDirectoryPath(currentPath);
  if (parentPath.isEmpty()) {
    return "";
  }

  // Convert filesystem path back to URI
  // This is a simplified version - in reality you'd need to map back to URI
  // space
  std::string parentStr = parentPath.toString();

  if (parentStr == "/") {
    return "/";
  }

  std::string requestStr = requestPath.toString();
  if (!requestStr.empty() && requestStr[requestStr.length() - 1] == '/') {
    requestStr = requestStr.substr(0, requestStr.length() - 1);
  }

  std::size_t lastSlash = requestStr.find_last_of('/');
  if (lastSlash == std::string::npos) {
    return "/";
  }

  return requestStr.substr(0, lastSlash + 1);
}

bool DirectoryLister::isHiddenFile(const std::string& filename) {
  return !filename.empty() && filename[0] == '.';
}

bool DirectoryLister::isExecutableFile(
    const domain::filesystem::value_objects::Permission& perm) {
  return perm.canExecute(
             domain::filesystem::value_objects::Permission::CLASS_OWNER) ||
         perm.canExecute(
             domain::filesystem::value_objects::Permission::CLASS_GROUP) ||
         perm.canExecute(
             domain::filesystem::value_objects::Permission::CLASS_OTHER);
}

bool DirectoryLister::isImageFile(const std::string& filename) {
  return getImageExtensionPattern().matches(filename);
}

bool DirectoryLister::isScriptFile(const std::string& filename) {
  return getScriptExtensionPattern().matches(filename);
}

bool DirectoryLister::hasAccess(
    const DirectoryEntry& entry,
    domain::filesystem::value_objects::Permission::Class userClass) {
  return entry.m_permissions.canRead(userClass);
}

std::vector<DirectoryEntry> DirectoryLister::readDirectoryEntries(
    const domain::filesystem::value_objects::Path& directoryPath,
    bool showHidden) {
  std::vector<DirectoryEntry> entries;
  std::string dirPath = directoryPath.toString();

  DIR* dir = opendir(dirPath.c_str());
  if (dir == NULL) {
    throw exceptions::DirectoryListerException(
        "Cannot open directory: " + dirPath,
        exceptions::DirectoryListerException::CANNOT_OPEN_DIRECTORY);
  }

  struct dirent* entry;
  while ((entry = readdir(dir)) != NULL) {
    std::string entryName = entry->d_name;

    if (shouldSkipEntry(entryName, showHidden)) {
      continue;
    }

    domain::filesystem::value_objects::Path fullPath =
        directoryPath.join(entryName);
    DirectoryEntry dirEntry = createEntryFromStat(entryName, fullPath);
    entries.push_back(dirEntry);
  }

  closedir(dir);
  return entries;
}

void DirectoryLister::sortEntries(std::vector<DirectoryEntry>& entries,
                                  const std::string& sortBy, bool ascending) {
  if (sortBy == "name") {
    CompareByName comparator(ascending);
    std::sort(entries.begin(), entries.end(), comparator);
  } else if (sortBy == "size") {
    CompareBySize comparator(ascending);
    std::sort(entries.begin(), entries.end(), comparator);
  } else if (sortBy == "date") {
    CompareByDate comparator(ascending);
    std::sort(entries.begin(), entries.end(), comparator);
  } else if (sortBy == "type") {
    CompareByType comparator(ascending);
    std::sort(entries.begin(), entries.end(), comparator);
  }
}

bool DirectoryLister::shouldSkipEntry(const std::string& entryName,
                                      bool showHidden) {
  if (entryName == "." || entryName == "..") {
    return true;
  }

  if (!showHidden && isHiddenFile(entryName)) {
    return true;
  }

  return false;
}

DirectoryEntry DirectoryLister::createEntryFromStat(
    const std::string& entryName,
    const domain::filesystem::value_objects::Path& fullPath) {
  struct stat fileStat;
  std::string fullPathStr = fullPath.toString();

  if (stat(fullPathStr.c_str(), &fileStat) != 0) {
    return DirectoryEntry(
        entryName, false, domain::filesystem::value_objects::Size::zero(),
        "Unknown", domain::filesystem::value_objects::Permission::readOnly(),
        fullPath);
  }

  bool isDir = S_ISDIR(fileStat.st_mode);
  domain::filesystem::value_objects::Size size =
      isDir ? domain::filesystem::value_objects::Size::zero()
            : domain::filesystem::value_objects::Size(
                  static_cast<std::size_t>(fileStat.st_size));
  std::string lastModified = getLastModifiedTime(fullPath);
  domain::filesystem::value_objects::Permission permissions =
      getFilePermissions(fullPath);

  return DirectoryEntry(entryName, isDir, size, lastModified, permissions,
                        fullPath);
}

std::string DirectoryLister::getLastModifiedTime(
    const domain::filesystem::value_objects::Path& path) {
  struct stat fileStat;
  std::string pathStr = path.toString();

  if (stat(pathStr.c_str(), &fileStat) != 0) {
    return "Unknown";
  }

  struct tm* timeinfo = localtime(&fileStat.st_mtime);
  if (timeinfo == NULL) {
    return "Unknown";
  }

  char buffer[LAST_MODIFIED_BUFFER_SIZE];
  strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);
  return std::string(buffer);
}

domain::filesystem::value_objects::Permission
DirectoryLister::getFilePermissions(
    const domain::filesystem::value_objects::Path& path) {
  struct stat fileStat;
  std::string pathStr = path.toString();

  if (stat(pathStr.c_str(), &fileStat) != 0) {
    return domain::filesystem::value_objects::Permission::readOnly();
  }

  return domain::filesystem::value_objects::Permission(
      fileStat.st_mode &
      domain::filesystem::value_objects::Permission::MAX_PERMISSION);
}

domain::filesystem::value_objects::Size DirectoryLister::getFileSize(
    const domain::filesystem::value_objects::Path& path) {
  struct stat fileStat;
  std::string pathStr = path.toString();

  if (stat(pathStr.c_str(), &fileStat) != 0) {
    return domain::filesystem::value_objects::Size::zero();
  }

  return domain::filesystem::value_objects::Size(
      static_cast<std::size_t>(fileStat.st_size));
}

std::string DirectoryLister::generateDirectoryEntryHtml(
    const DirectoryEntry& entry,
    const domain::filesystem::value_objects::Path& requestPath,
    bool isParentLink) {
  std::ostringstream html;
  html << "      <tr>\n";

  std::string icon = generateIconHtml(entry);
  std::string name = entry.m_name;
  std::string href;
  std::string fileTypeClass = generateFileTypeClass(entry);
  std::string typeDisplay = entry.m_isDirectory ? "Directory" : "File";

  if (!entry.m_isDirectory) {
    if (isImageFile(entry.m_name)) {
      typeDisplay = "Image";
    } else if (isScriptFile(entry.m_name)) {
      typeDisplay = "Script";
    } else if (isExecutableFile(entry.m_permissions)) {
      typeDisplay = "Executable";
    }
  }

  if (isHiddenFile(entry.m_name)) {
    fileTypeClass += " hidden";
  }

  if (isParentLink) {
    name = "Parent Directory";
    href = getParentDirectoryUri(entry.m_fullPath, requestPath);
    fileTypeClass = "directory";
    typeDisplay = "Directory";
  } else {
    href = requestPath.toString();
    if (!href.empty() && href[href.length() - 1] != '/') {
      href += "/";
    }
    href += name;

    if (entry.m_isDirectory) {
      href += "/";
      name += "/";
    }
  }

  html << "        <td class=\"" << fileTypeClass << "\">";
  html << "<span class=\"icon\">" << icon << "</span> ";
  html << "<a href=\"" << href << "\" title=\"" << entry.m_fullPath.toString()
       << "\">" << name << "</a>";
  html << "</td>\n";

  html << "        <td class=\"size\">" << entry.m_size.toString() << "</td>\n";

  html << "        <td class=\"date\">" << entry.m_lastModified << "</td>\n";

  html << "        <td class=\"permissions\">"
       << entry.m_permissions.toSymbolicString() << "</td>\n";

  html << "        <td>" << typeDisplay << "</td>\n";

  html << "      </tr>\n";

  return html.str();
}

std::string DirectoryLister::generateTableHeaderHtml(
    const std::string& sortBy, bool ascending,
    const domain::filesystem::value_objects::Path& requestPath) {
  std::ostringstream html;

  html << "    <thead>\n";
  html << "      <tr>\n";

  html << "        <th>\n";
  html << "          <a href=\""
       << generateSortUrl(requestPath, sortBy, "name", ascending)
       << "\" class=\"header-link\">\n";
  html << "            Name";
  if (sortBy == "name") {
    html << " <span class=\"sort-indicator\">" << (ascending ? "▲" : "▼")
         << "</span>";
  }
  html << "\n";
  html << "          </a>\n";
  html << "        </th>\n";

  html << "        <th class=\"size\">\n";
  html << "          <a href=\""
       << generateSortUrl(requestPath, sortBy, "size", ascending)
       << "\" class=\"header-link\">\n";
  html << "            Size";
  if (sortBy == "size") {
    html << " <span class=\"sort-indicator\">" << (ascending ? "▲" : "▼")
         << "</span>";
  }
  html << "\n";
  html << "          </a>\n";
  html << "        </th>\n";

  html << "        <th class=\"date\">\n";
  html << "          <a href=\""
       << generateSortUrl(requestPath, sortBy, "date", ascending)
       << "\" class=\"header-link\">\n";
  html << "            Last Modified";
  if (sortBy == "date") {
    html << " <span class=\"sort-indicator\">" << (ascending ? "▲" : "▼")
         << "</span>";
  }
  html << "\n";
  html << "          </a>\n";
  html << "        </th>\n";

  html << "        <th>\n";
  html << "          <a href=\""
       << generateSortUrl(requestPath, sortBy, "permissions", ascending)
       << "\" class=\"header-link\">\n";
  html << "            Permissions";
  if (sortBy == "permissions") {
    html << " <span class=\"sort-indicator\">" << (ascending ? "▲" : "▼")
         << "</span>";
  }
  html << "\n";
  html << "          </a>\n";
  html << "        </th>\n";

  html << "        <th>\n";
  html << "          <a href=\""
       << generateSortUrl(requestPath, sortBy, "type", ascending)
       << "\" class=\"header-link\">\n";
  html << "            Type";
  if (sortBy == "type") {
    html << " <span class=\"sort-indicator\">" << (ascending ? "▲" : "▼")
         << "</span>";
  }
  html << "\n";
  html << "          </a>\n";
  html << "        </th>\n";

  html << "      </tr>\n";
  html << "    </thead>\n";

  return html.str();
}

std::string DirectoryLister::generateSortUrl(
    const domain::filesystem::value_objects::Path& requestPath,
    const std::string& currentSortBy, const std::string& columnToSort,
    bool isAscending) {
  domain::http::value_objects::QueryStringBuilder builder(
      requestPath.toString());

  builder.setParameter("sort", columnToSort);

  if (currentSortBy == columnToSort) {
    builder.setParameter("order", (isAscending ? "desc" : "asc") != 0);
  } else {
    builder.setParameter("order", true);
  }

  return builder.build();
}

std::string DirectoryLister::generateIconHtml(const DirectoryEntry& entry) {
  if (entry.m_isDirectory) {
    return "📁";
  }
  if (isImageFile(entry.m_name)) {
    return "🖼️";
  }
  if (isScriptFile(entry.m_name)) {
    return "📜";
  }
  if (isExecutableFile(entry.m_permissions)) {
    return "⚙️";
  }
  return "📄";
}

std::string DirectoryLister::generateFileTypeClass(
    const DirectoryEntry& entry) {
  if (entry.m_isDirectory) {
    return "directory";
  }
  if (isImageFile(entry.m_name)) {
    return "image";
  }
  if (isScriptFile(entry.m_name)) {
    return "script";
  }
  if (isExecutableFile(entry.m_permissions)) {
    return "executable";
  }
  return "";
}

}  // namespace adapters
}  // namespace filesystem
}  // namespace infrastructure
