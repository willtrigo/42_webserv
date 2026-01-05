/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RouteMatchInfo.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/05 15:13:50 by dande-je          #+#    #+#             */
/*   Updated: 2026/01/05 15:15:45 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ROUTE_MATCH_INFO_HPP
#define ROUTE_MATCH_INFO_HPP

#include "domain/filesystem/value_objects/Path.hpp"

#include <map>
#include <string>

namespace domain {
namespace http {
namespace value_objects {

class RouteMatchInfo {
 public:
  RouteMatchInfo();
  RouteMatchInfo(const filesystem::value_objects::Path& resolvedPath,
                 bool isDirectory, const std::string& fileToServe,
                 const std::map<std::string, std::string>& queryParams);
  RouteMatchInfo(const RouteMatchInfo& other);
  ~RouteMatchInfo();

  RouteMatchInfo& operator=(const RouteMatchInfo& other);
  bool operator==(const RouteMatchInfo& other) const;
  bool operator!=(const RouteMatchInfo& other) const;

  const filesystem::value_objects::Path& getResolvedPath() const;
  bool isDirectory() const;
  const std::string& getFileToServe() const;
  const std::map<std::string, std::string>& getQueryParams() const;

  bool hasQueryParam(const std::string& key) const;
  std::string getQueryParam(const std::string& key) const;
  bool isEmpty() const;

  static RouteMatchInfo createEmpty();
  static RouteMatchInfo createForFile(
      const filesystem::value_objects::Path& resolvedPath,
      const std::string& fileToServe);
  static RouteMatchInfo createForDirectory(
      const filesystem::value_objects::Path& resolvedPath);
  static RouteMatchInfo createForFileWithQuery(
      const filesystem::value_objects::Path& resolvedPath,
      const std::string& fileToServe,
      const std::map<std::string, std::string>& queryParams);

 private:
  filesystem::value_objects::Path m_resolvedPath;
  bool m_isDirectory;
  std::string m_fileToServe;
  std::map<std::string, std::string> m_queryParams;

  void validate() const;
};

}  // namespace value_objects
}  // namespace http
}  // namespace domain

#endif  // ROUTE_MATCH_INFO_HPP
