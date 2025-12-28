/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   DirectoryEntryComparators.hpp                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/22 22:53:16 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/27 23:54:57 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DIRECTORY_ENTRY_COMPARATORS_HPP
#define DIRECTORY_ENTRY_COMPARATORS_HPP

#include "infrastructure/filesystem/adapters/DirectoryLister.hpp"

#include <cctype>

namespace infrastructure {
namespace filesystem {
namespace adapters {

class CompareByName {
 public:
  CompareByName(bool ascending);

  bool operator()(const DirectoryEntry& compareA,
                  const DirectoryEntry& compareB) const;

 private:
  bool m_ascending;
};

class CompareBySize {
 public:
  CompareBySize(bool ascending);

  bool operator()(const DirectoryEntry& compareA,
                  const DirectoryEntry& compareB) const;

 private:
  bool m_ascending;
};

class CompareByDate {
 public:
  CompareByDate(bool ascending);

  bool operator()(const DirectoryEntry& compareA,
                  const DirectoryEntry& compareB) const;

 private:
  bool m_ascending;
};

class CompareByType {
 public:
  CompareByType(bool ascending);

  bool operator()(const DirectoryEntry& compareA,
                  const DirectoryEntry& compareB) const;

 private:
  bool m_ascending;
};

}  // namespace adapters
}  // namespace filesystem
}  // namespace infrastructure

#endif  // DIRECTORY_ENTRY_COMPARATORS_HPP
