/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MockDirectoryLister.cpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: umeneses <umeneses@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/01 00:00:00 by copilot           #+#    #+#             */
/*   Updated: 2026/01/01 19:18:55 by umeneses         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "mocks/MockDirectoryLister.hpp"
#include <algorithm>
#include <sstream>

MockDirectoryLister::MockDirectoryLister()
    : m_listSuccess(true), m_validationSuccess(true) {}

MockDirectoryLister::~MockDirectoryLister() {}

std::vector<infrastructure::filesystem::adapters::DirectoryEntry>
MockDirectoryLister::listDirectory(
    const domain::filesystem::value_objects::Path& directoryPath,
    bool showHidden,
    const std::string& sortBy,
    bool ascending) {
  trackCall("listDirectory");

  if (!m_listSuccess) {
    return std::vector<infrastructure::filesystem::adapters::DirectoryEntry>();
  }

  std::string pathStr = normalizePath(directoryPath.toString());
  
  if (m_mockDirectories.find(pathStr) == m_mockDirectories.end()) {
    return std::vector<infrastructure::filesystem::adapters::DirectoryEntry>();
  }

  std::vector<infrastructure::filesystem::adapters::DirectoryEntry> entries = 
      m_mockDirectories[pathStr];

  // Filter hidden files
  if (!showHidden) {
    std::vector<infrastructure::filesystem::adapters::DirectoryEntry> filtered;
    for (size_t i = 0; i < entries.size(); ++i) {
      if (entries[i].m_name.empty() || entries[i].m_name[0] != '.') {
        filtered.push_back(entries[i]);
      }
    }
    entries = filtered;
  }

  // Simple sorting by name (ascending/descending)
  if (sortBy == "name") {
    for (size_t i = 0; i < entries.size(); ++i) {
      for (size_t j = i + 1; j < entries.size(); ++j) {
        bool shouldSwap = ascending ? 
            (entries[i].m_name > entries[j].m_name) :
            (entries[i].m_name < entries[j].m_name);
        if (shouldSwap) {
          infrastructure::filesystem::adapters::DirectoryEntry temp = entries[i];
          entries[i] = entries[j];
          entries[j] = temp;
        }
      }
    }
  }

  return entries;
}

bool MockDirectoryLister::validateDirectoryForListing(
    const domain::filesystem::value_objects::Path& directoryPath) {
  trackCall("validateDirectoryForListing");

  if (!m_validationSuccess) {
    return false;
  }

  std::string pathStr = normalizePath(directoryPath.toString());
  
  // Check if directory is marked as existing
  if (m_directoryExists.find(pathStr) != m_directoryExists.end()) {
    return m_directoryExists[pathStr];
  }

  // Default: directory exists if we have mock entries for it
  return m_mockDirectories.find(pathStr) != m_mockDirectories.end();
}

std::string MockDirectoryLister::generateHtmlListing(
    const domain::filesystem::value_objects::Path& directoryPath,
    const domain::filesystem::value_objects::Path& requestPath,
    bool showHidden) {
  trackCall("generateHtmlListing");

  std::vector<infrastructure::filesystem::adapters::DirectoryEntry> entries = 
      listDirectory(directoryPath, showHidden, "name", true);

  std::ostringstream html;
  html << "<html><body><h1>Directory Listing: " 
       << requestPath.toString() << "</h1><ul>";

  for (size_t i = 0; i < entries.size(); ++i) {
    html << "<li><a href=\"" << entries[i].m_name << "\">" 
         << entries[i].m_name;
    if (entries[i].m_isDirectory) {
      html << "/";
    }
    html << "</a> (" << entries[i].m_size.getBytes() << " bytes)</li>";
  }

  html << "</ul></body></html>";
  return html.str();
}

void MockDirectoryLister::setListSuccess(bool success) {
  m_listSuccess = success;
}

void MockDirectoryLister::setValidationSuccess(bool success) {
  m_validationSuccess = success;
}

void MockDirectoryLister::setDirectoryExists(const std::string& path, bool exists) {
  m_directoryExists[normalizePath(path)] = exists;
}

void MockDirectoryLister::addMockEntry(
    const std::string& directoryPath,
    const infrastructure::filesystem::adapters::DirectoryEntry& entry) {
  std::string pathStr = normalizePath(directoryPath);
  m_mockDirectories[pathStr].push_back(entry);
  m_directoryExists[pathStr] = true;
}

void MockDirectoryLister::clearMockEntries(const std::string& directoryPath) {
  std::string pathStr = normalizePath(directoryPath);
  m_mockDirectories.erase(pathStr);
}

int MockDirectoryLister::getCallCount(const std::string& methodName) const {
  std::map<std::string, int>::const_iterator it = m_callCounts.find(methodName);
  return (it != m_callCounts.end()) ? it->second : 0;
}

std::vector<std::string> MockDirectoryLister::getCalledMethods() const {
  return m_callHistory;
}

void MockDirectoryLister::clearCallHistory() {
  m_callCounts.clear();
  m_callHistory.clear();
}

const std::map<std::string, std::vector<infrastructure::filesystem::adapters::DirectoryEntry> >&
MockDirectoryLister::getMockDirectories() const {
  return m_mockDirectories;
}

void MockDirectoryLister::trackCall(const std::string& methodName) {
  m_callCounts[methodName]++;
  m_callHistory.push_back(methodName);
}

std::string MockDirectoryLister::normalizePath(const std::string& path) const {
  std::string normalized = path;
  // Remove trailing slash for consistency
  if (!normalized.empty() && normalized[normalized.length() - 1] == '/') {
    normalized = normalized.substr(0, normalized.length() - 1);
  }
  return normalized;
}
