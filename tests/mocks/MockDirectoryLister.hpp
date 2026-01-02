/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MockDirectoryLister.hpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: umeneses <umeneses@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/01 00:00:00 by copilot           #+#    #+#             */
/*   Updated: 2026/01/01 19:18:55 by umeneses         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MOCKDIRECTORYLISTER_HPP
#define MOCKDIRECTORYLISTER_HPP

#include "domain/filesystem/value_objects/Path.hpp"
#include "domain/filesystem/value_objects/Size.hpp"
#include "domain/filesystem/value_objects/Permission.hpp"

#include <map>
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

}  // namespace adapters
}  // namespace filesystem
}  // namespace infrastructure

class MockDirectoryLister {
 public:
  MockDirectoryLister();
  ~MockDirectoryLister();

  // Core directory listing operations
  std::vector<infrastructure::filesystem::adapters::DirectoryEntry> listDirectory(
      const domain::filesystem::value_objects::Path& directoryPath,
      bool showHidden = false,
      const std::string& sortBy = "name",
      bool ascending = true);

  bool validateDirectoryForListing(
      const domain::filesystem::value_objects::Path& directoryPath);

  std::string generateHtmlListing(
      const domain::filesystem::value_objects::Path& directoryPath,
      const domain::filesystem::value_objects::Path& requestPath,
      bool showHidden = false);

  // Mock configuration
  void setListSuccess(bool success);
  void setValidationSuccess(bool success);
  void setDirectoryExists(const std::string& path, bool exists);
  void addMockEntry(const std::string& directoryPath,
                   const infrastructure::filesystem::adapters::DirectoryEntry& entry);
  void clearMockEntries(const std::string& directoryPath);

  // Verification methods
  int getCallCount(const std::string& methodName) const;
  std::vector<std::string> getCalledMethods() const;
  void clearCallHistory();
  const std::map<std::string, std::vector<infrastructure::filesystem::adapters::DirectoryEntry> >&
      getMockDirectories() const;

 private:
  // Mock storage
  std::map<std::string, std::vector<infrastructure::filesystem::adapters::DirectoryEntry> > m_mockDirectories;
  std::map<std::string, bool> m_directoryExists;
  
  // Configurable behavior
  bool m_listSuccess;
  bool m_validationSuccess;
  
  // Call tracking
  std::map<std::string, int> m_callCounts;
  std::vector<std::string> m_callHistory;

  void trackCall(const std::string& methodName);
  std::string normalizePath(const std::string& path) const;
};

#endif  // MOCKDIRECTORYLISTER_HPP
