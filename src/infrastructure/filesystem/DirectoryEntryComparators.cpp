/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   DirectoryEntryComparators.cpp                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/22 23:01:07 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/22 23:12:08 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "infrastructure/filesystem/DirectoryEntryComparators.hpp"

namespace infrastructure {
namespace filesystem {

CompareByName::CompareByName(bool ascending) : m_ascending(ascending) {}

bool CompareByName::operator()(const DirectoryEntry& compareA,
                               const DirectoryEntry& compareB) const {
  if (compareA.m_isDirectory != compareB.m_isDirectory) {
    return compareA.m_isDirectory;
  }

  std::string aName = compareA.m_name;
  std::string bName = compareB.m_name;
  for (std::size_t i = 0; i < aName.size(); ++i) {
    aName[i] =
        static_cast<char>(::tolower(static_cast<unsigned char>(aName[i])));
  }
  for (std::size_t i = 0; i < bName.size(); ++i) {
    bName[i] =
        static_cast<char>(::tolower(static_cast<unsigned char>(bName[i])));
  }

  if (m_ascending) {
    return aName < bName;
  }
  return aName > bName;
}

CompareBySize::CompareBySize(bool ascending) : m_ascending(ascending) {}

bool CompareBySize::operator()(const DirectoryEntry& compareA,
                               const DirectoryEntry& compareB) const {
  if (compareA.m_isDirectory != compareB.m_isDirectory) {
    return compareA.m_isDirectory;
  }

  if (m_ascending) {
    return compareA.m_size < compareB.m_size;
  }
  return compareA.m_size > compareB.m_size;
}

CompareByDate::CompareByDate(bool ascending) : m_ascending(ascending) {}

bool CompareByDate::operator()(const DirectoryEntry& compareA,
                               const DirectoryEntry& compareB) const {
  if (compareA.m_isDirectory != compareB.m_isDirectory) {
    return compareA.m_isDirectory;
  }

  if (m_ascending) {
    return compareA.m_lastModified < compareB.m_lastModified;
  }
  return compareA.m_lastModified > compareB.m_lastModified;
}

CompareByType::CompareByType(bool ascending) : m_ascending(ascending) {}

bool CompareByType::operator()(const DirectoryEntry& compareA,
                               const DirectoryEntry& compareB) const {
  if (compareA.m_isDirectory != compareB.m_isDirectory) {
    if (m_ascending) {
      return compareA.m_isDirectory;
    }
    return !compareA.m_isDirectory;
  }

  if (compareA.m_isDirectory && compareB.m_isDirectory) {
    CompareByName nameComparator(m_ascending);
    return nameComparator(compareA, compareB);
  }

  std::string aExt = compareA.m_name;
  std::string bExt = compareB.m_name;

  std::size_t aDotPos = aExt.find_last_of(".");
  std::size_t bDotPos = bExt.find_last_of(".");

  if (aDotPos != std::string::npos) {
    aExt = aExt.substr(aDotPos + 1);
  }
  if (bDotPos != std::string::npos) {
    bExt = bExt.substr(bDotPos + 1);
  }

  for (std::size_t i = 0; i < aExt.size(); ++i) {
    aExt[i] = static_cast<char>(::tolower(static_cast<unsigned char>(aExt[i])));
  }
  for (std::size_t i = 0; i < bExt.size(); ++i) {
    bExt[i] = static_cast<char>(::tolower(static_cast<unsigned char>(bExt[i])));
  }

  if (m_ascending) {
    return aExt < bExt;
  }
  return aExt > bExt;
}

}  // namespace filesystem
}  // namespace infrastructure
