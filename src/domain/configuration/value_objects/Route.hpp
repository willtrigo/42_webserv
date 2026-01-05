/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Route.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/24 21:46:04 by dande-je          #+#    #+#             */
/*   Updated: 2026/01/05 15:19:57 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ROUTE_HPP
#define ROUTE_HPP

#include "domain/filesystem/value_objects/Path.hpp"
#include "domain/filesystem/value_objects/Permission.hpp"
#include "domain/filesystem/value_objects/Size.hpp"
#include "domain/http/value_objects/HttpMethod.hpp"
#include "domain/http/value_objects/RouteMatchInfo.hpp"
#include "domain/shared/value_objects/ErrorCode.hpp"

#include <map>
#include <set>
#include <string>

namespace domain {
namespace configuration {
namespace value_objects {

class Route {
 public:
  enum HandlerType {
    STATIC_FILE,
    DIRECTORY_LISTING,
    CGI_EXECUTION,
    POST,
    DELETE,
    REDIRECT,
    UPLOAD
  };

  Route();
  Route(const filesystem::value_objects::Path& path,
        const std::set<http::value_objects::HttpMethod>& allowedMethods,
        HandlerType handlerType);

  const filesystem::value_objects::Path& getPath() const;
  const std::set<http::value_objects::HttpMethod>& getAllowedMethods() const;
  HandlerType getHandlerType() const;
  std::string getHandlerTarget() const;

  bool allowsMethod(const http::value_objects::HttpMethod& method) const;
  bool matches(const std::string& requestPath) const;
  bool isCgiRoute() const;
  bool isRedirect() const;
  bool isUploadRoute() const;
  bool allowsDirectoryListing() const;

  void setRootDirectory(const std::string& root);
  void setIndexFile(const std::string& index);
  void setUploadDirectory(const std::string& uploadDir);
  void setMaxBodySize(const filesystem::value_objects::Size& maxSize);
  void setDirectoryListing(bool enable);
  void setCgiConfig(const std::string& interpreter,
                    const std::string& extension);
  void setRedirect(const std::string& target,
                   const shared::value_objects::ErrorCode& code);
  void setFilePermissions(
      const filesystem::value_objects::Permission& permissions);

  std::string getRootDirectory() const;
  std::string getIndexFile() const;
  std::string getUploadDirectory() const;
  filesystem::value_objects::Size getMaxBodySize() const;
  filesystem::value_objects::Permission getFilePermissions() const;
  std::string getCgiInterpreter() const;
  std::string getCgiExtension() const;
  std::string getRedirectTarget() const;
  shared::value_objects::ErrorCode getRedirectCode() const;

  bool isStaticFileRoute() const;
  bool isPostRoute() const;
  bool isDeleteRoute() const;
  bool hasRootDirectory() const;
  bool hasIndexFile() const;
  bool hasCgiConfig() const;
  bool hasRedirectConfig() const;
  bool hasUploadConfig() const;

  http::value_objects::RouteMatchInfo resolveRequest(
      const std::string& requestPath,
      const std::map<std::string, std::string>& queryParams =
          std::map<std::string, std::string>()) const;

 private:
  filesystem::value_objects::Path m_pathPattern;
  std::set<http::value_objects::HttpMethod> m_allowedMethods;
  HandlerType m_handlerType;

  std::string m_rootDirectory;
  std::string m_indexFile;
  std::string m_uploadDirectory;
  filesystem::value_objects::Size m_maxBodySize;
  filesystem::value_objects::Permission m_filePermissions;
  bool m_directoryListing;

  std::string m_cgiInterpreter;
  std::string m_cgiExtension;

  std::string m_redirectTarget;
  shared::value_objects::ErrorCode m_redirectCode;

  filesystem::value_objects::Path buildFullPath(
      const std::string& requestPath) const;
  bool isPathMatch(const std::string& requestPath) const;
  std::string findFileToServe(
      const filesystem::value_objects::Path& fullPath) const;
  void validate() const;
};

}  // namespace value_objects
}  // namespace configuration
}  // namespace domain

#endif  // ROUTE_HPP
