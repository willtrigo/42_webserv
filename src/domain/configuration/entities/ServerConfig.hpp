/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/22 11:50:30 by dande-je          #+#    #+#             */
/*   Updated: 2026/01/01 19:00:39 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_CONFIG_HPP
#define SERVER_CONFIG_HPP

#include "domain/configuration/entities/LocationConfig.hpp"
#include "domain/configuration/value_objects/ListenDirective.hpp"
#include "domain/filesystem/value_objects/Path.hpp"
#include "domain/filesystem/value_objects/Size.hpp"
#include "domain/http/value_objects/Host.hpp"
#include "domain/http/value_objects/Port.hpp"
#include "domain/shared/value_objects/ErrorCode.hpp"

#include <map>
#include <string>
#include <vector>

namespace domain {
namespace configuration {
namespace entities {

class ServerConfig {
 public:
  typedef std::vector<std::string> ServerNames;
  typedef std::vector<ListenDirective> ListenDirectives;
  typedef std::vector<LocationConfig*> Locations;
  typedef std::map<shared::value_objects::ErrorCode, std::string> ErrorPageMap;

  static const std::size_t DEFAULT_CLIENT_MAX_BODY_SIZE_MB = 1;
  static const std::size_t MAX_CLIENT_MAX_BODY_SIZE_MB = 100;
  static const std::string DEFAULT_ROOT;
  static const std::string DEFAULT_INDEX;

  ServerConfig();
  explicit ServerConfig(const ListenDirectives& listenDirectives);
  ServerConfig(const ServerConfig& other);
  ~ServerConfig();

  ServerConfig& operator=(const ServerConfig& other);

  const ListenDirectives& getListenDirectives() const;
  const ServerNames& getServerNames() const;
  const filesystem::value_objects::Path& getRoot() const;
  const std::vector<std::string>& getIndexFiles() const;
  const ErrorPageMap& getErrorPages() const;
  const Locations& getLocations() const;
  const filesystem::value_objects::Size& getClientMaxBodySize() const;
  const std::string& getReturnRedirect() const;
  const shared::value_objects::ErrorCode& getReturnCode() const;
  bool isDefaultServer() const;

  void addListenDirective(const ListenDirective& directive);
  void addListenDirective(const std::string& directiveString);
  void addServerName(const std::string& name);
  void setRoot(const filesystem::value_objects::Path& root);
  void setRoot(const std::string& root);
  void addIndexFile(const std::string& index);
  void addErrorPage(const shared::value_objects::ErrorCode& code,
                    const std::string& uri);
  void addLocation(LocationConfig* location);
  void setClientMaxBodySize(const filesystem::value_objects::Size& size);
  void setClientMaxBodySize(const std::string& sizeString);
  void setReturnRedirect(const std::string& redirect,
                         const shared::value_objects::ErrorCode& code);
  void setReturnRedirect(const std::string& redirect, unsigned int code);
  void setReturnContent(const std::string& content,
                        const shared::value_objects::ErrorCode& code);
  void setReturnContent(const std::string& content, unsigned int code);
  const std::string& getReturnContent() const;
  bool hasReturnRedirect() const;
  bool hasReturnContent() const;

  bool isValid() const;
  void validate() const;

  bool matchesRequest(const std::string& host, unsigned int port) const;
  bool matchesRequest(const http::value_objects::Host& host,
                      const http::value_objects::Port& port) const;
  const LocationConfig* findLocation(const std::string& uriPath) const;
  bool hasListenDirective(const std::string& address, unsigned int port) const;
  bool hasListenDirective(const http::value_objects::Host& host,
                          const http::value_objects::Port& port) const;

  void setListenDirectives(const std::vector<std::string>& directives);
  void setListenDirectives(const ListenDirectives& directives);

  void clear();
  std::string toString() const;

 private:
  ListenDirectives m_listenDirectives;
  ServerNames m_serverNames;
  filesystem::value_objects::Path m_root;
  std::vector<std::string> m_indexFiles;
  ErrorPageMap m_errorPages;
  Locations m_locations;
  filesystem::value_objects::Size m_clientMaxBodySize;
  std::string m_returnRedirect;
  shared::value_objects::ErrorCode m_returnCode;
  std::string m_returnContent;

  void copyFrom(const ServerConfig& other);
  void clearLocations();

  void validateListenDirectives() const;
  void validateServerNames() const;
  void validateRoot() const;
  void validateErrorPages() const;
  void validateLocations() const;
  void validateClientMaxBodySize() const;

  static bool isValidServerName(const std::string& name);
  static bool isWildcardServerName(const std::string& name);
  static bool matchesServerName(const std::string& configName,
                                const std::string& requestName);
};

}  // namespace entities
}  // namespace configuration
}  // namespace domain

#endif  // SERVER_CONFIG_HPP
