/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpConfig.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/26 17:11:38 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/27 15:46:42 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTP_CONFIG_HPP
#define HTTP_CONFIG_HPP

#include "domain/configuration/entities/ServerConfig.hpp"
#include "domain/filesystem/value_objects/Path.hpp"
#include "domain/filesystem/value_objects/Size.hpp"
#include "domain/http/value_objects/Host.hpp"
#include "domain/http/value_objects/Port.hpp"

#include <map>
#include <string>
#include <vector>

namespace domain {
namespace configuration {
namespace entities {

class HttpConfig {
 public:
  static const unsigned int DEFAULT_WORKER_PROCESSES = 1;
  static const unsigned int DEFAULT_WORKER_CONNECTIONS = 1024;
  static const unsigned int DEFAULT_KEEPALIVE_TIMEOUT = 75;
  static const unsigned int DEFAULT_SEND_TIMEOUT = 60;
  static const std::string DEFAULT_MIME_TYPES_PATH;
  static const std::string DEFAULT_ERROR_LOG_PATH;
  static const std::string DEFAULT_ACCESS_LOG_PATH;

  static const unsigned int MIN_WORKER_PROCESSES = 1;
  static const unsigned int MAX_WORKER_PROCESSES = 64;

  static const unsigned int MIN_WORKER_CONNECTIONS = 1;
  static const unsigned int MAX_WORKER_CONNECTIONS = 65535;

  static const unsigned int MIN_TIMEOUT = 0;
  static const unsigned int MAX_KEEPALIVE_TIMEOUT = 300;
  static const unsigned int MAX_SEND_TIMEOUT = 300;

  static const unsigned int MAX_CLIENT_BODY_SIZE_GB = 1;

  typedef std::vector<entities::ServerConfig*> ServerConfigs;
  typedef std::map<std::string, std::string> MimeTypesMap;

  HttpConfig();
  explicit HttpConfig(const std::string& configFilePath);
  HttpConfig(const HttpConfig& other);
  ~HttpConfig();

  HttpConfig& operator=(const HttpConfig& other);

  unsigned int getWorkerProcesses() const;
  unsigned int getWorkerConnections() const;
  unsigned int getKeepaliveTimeout() const;
  unsigned int getSendTimeout() const;
  const filesystem::value_objects::Path& getErrorLogPath() const;
  const filesystem::value_objects::Path& getAccessLogPath() const;
  const filesystem::value_objects::Path& getMimeTypesPath() const;
  const filesystem::value_objects::Size& getClientMaxBodySize() const;
  const ServerConfigs& getServerConfigs() const;

  const entities::ServerConfig* selectServer(const std::string& host,
                                             unsigned int port) const;
  const entities::ServerConfig* selectServer(
      const http::value_objects::Host& host,
      const http::value_objects::Port& port) const;

  std::string getMimeType(const std::string& extension) const;
  void loadMimeTypes();
  bool hasMimeType(const std::string& extension) const;

  void addServerConfig(entities::ServerConfig* serverConfig);
  void setWorkerProcesses(unsigned int processes);
  void setWorkerConnections(unsigned int connections);
  void setKeepaliveTimeout(unsigned int timeout);
  void setSendTimeout(unsigned int timeout);
  void setErrorLogPath(const filesystem::value_objects::Path& path);
  void setErrorLogPath(const std::string& path);
  void setAccessLogPath(const filesystem::value_objects::Path& path);
  void setAccessLogPath(const std::string& path);
  void setMimeTypesPath(const filesystem::value_objects::Path& path);
  void setMimeTypesPath(const std::string& path);
  void setClientMaxBodySize(const filesystem::value_objects::Size& size);
  void setClientMaxBodySize(const std::string& sizeString);

  bool isValid() const;
  void validate() const;

  void clear();
  std::string toString() const;
  static HttpConfig fromFile(const std::string& configFilePath);

 private:
  unsigned int m_workerProcesses;
  unsigned int m_workerConnections;
  unsigned int m_keepaliveTimeout;
  unsigned int m_sendTimeout;
  filesystem::value_objects::Path m_errorLogPath;
  filesystem::value_objects::Path m_accessLogPath;
  filesystem::value_objects::Path m_mimeTypesPath;
  filesystem::value_objects::Size m_clientMaxBodySize;
  ServerConfigs m_serverConfigs;
  MimeTypesMap m_mimeTypes;
  bool m_mimeTypesLoaded;

  void copyFrom(const HttpConfig& other);
  void clearServerConfigs();
  void initializeDefaults();

  void validateGlobalConfig() const;
  void validateServerConfigs() const;
  void validateNoPortConflicts() const;
  void validateNoAddressConflicts() const;
  void validateDefaultServers() const;

  void loadMimeTypesFromFile();
  static bool isValidTimeout(unsigned int timeout);
  static bool isValidWorkerCount(unsigned int count);
  static bool isValidConnectionCount(unsigned int count);

  static bool hasPortConflict(const entities::ServerConfig* config1,
                              const entities::ServerConfig* config2);
  static bool hasAddressConflict(const entities::ServerConfig* config1,
                                 const entities::ServerConfig* config2);
};

}  // namespace entities
}  // namespace configuration
}  // namespace domain

#endif  // HTTP_CONFIG_HPP
