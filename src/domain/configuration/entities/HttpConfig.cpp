/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpConfig.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/26 17:13:33 by dande-je          #+#    #+#             */
/*   Updated: 2026/01/02 14:32:31 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "domain/configuration/entities/HttpConfig.hpp"
#include "domain/configuration/exceptions/HttpConfigException.hpp"
#include "domain/shared/utils/StringUtils.hpp"

#include <fstream>
#include <iostream>
#include <sstream>

namespace domain {
namespace configuration {
namespace entities {

const std::string HttpConfig::DEFAULT_MIME_TYPES_PATH = "/etc/mime.types";
const std::string HttpConfig::DEFAULT_ERROR_LOG_PATH =
    "/var/log/webserv_error.log";
const std::string HttpConfig::DEFAULT_ACCESS_LOG_PATH =
    "/var/log/webserv_access.log";

HttpConfig::HttpConfig()
    : m_workerProcesses(DEFAULT_WORKER_PROCESSES),
      m_workerConnections(DEFAULT_WORKER_CONNECTIONS),
      m_keepaliveTimeout(DEFAULT_KEEPALIVE_TIMEOUT),
      m_sendTimeout(DEFAULT_SEND_TIMEOUT),
      m_errorLogPath(filesystem::value_objects::Path::fromString(
          DEFAULT_ERROR_LOG_PATH, true)),
      m_accessLogPath(filesystem::value_objects::Path::fromString(
          DEFAULT_ACCESS_LOG_PATH, true)),
      m_mimeTypesPath(filesystem::value_objects::Path::fromString(
          DEFAULT_MIME_TYPES_PATH, true)),
      m_clientMaxBodySize(filesystem::value_objects::Size::fromMegabytes(
          MAX_CLIENT_BODY_SIZE_GB)),
      m_mimeTypesLoaded(false) {}

HttpConfig::HttpConfig(const std::string& configFilePath) {
  initializeDefaults();
  // This constructor would parse a configuration file
  // For now, it just initializes with defaults
  // TODO: implement necessary things to parse information
  (void)configFilePath;
}

HttpConfig::HttpConfig(const HttpConfig& other) { copyFrom(other); }

HttpConfig::~HttpConfig() { clearServerConfigs(); }

HttpConfig& HttpConfig::operator=(const HttpConfig& other) {
  if (this != &other) {
    clear();
    copyFrom(other);
  }
  return *this;
}

void HttpConfig::copyFrom(const HttpConfig& other) {
  m_workerProcesses = other.m_workerProcesses;
  m_workerConnections = other.m_workerConnections;
  m_keepaliveTimeout = other.m_keepaliveTimeout;
  m_sendTimeout = other.m_sendTimeout;
  m_errorLogPath = other.m_errorLogPath;
  m_accessLogPath = other.m_accessLogPath;
  m_mimeTypesPath = other.m_mimeTypesPath;
  m_clientMaxBodySize = other.m_clientMaxBodySize;
  m_mimeTypes = other.m_mimeTypes;
  m_mimeTypesLoaded = other.m_mimeTypesLoaded;
  m_errorPages = other.m_errorPages;

  for (ServerConfigs::const_iterator it = other.m_serverConfigs.begin();
       it != other.m_serverConfigs.end(); ++it) {
    if (*it != NULL) {
      m_serverConfigs.push_back(new entities::ServerConfig(**it));
    }
  }
}

void HttpConfig::clearServerConfigs() {
  for (ServerConfigs::iterator it = m_serverConfigs.begin();
       it != m_serverConfigs.end(); ++it) {
    delete *it;
  }
  m_serverConfigs.clear();
}

void HttpConfig::initializeDefaults() {
  m_workerProcesses = DEFAULT_WORKER_PROCESSES;
  m_workerConnections = DEFAULT_WORKER_CONNECTIONS;
  m_keepaliveTimeout = DEFAULT_KEEPALIVE_TIMEOUT;
  m_sendTimeout = DEFAULT_SEND_TIMEOUT;
  m_errorLogPath =
      filesystem::value_objects::Path::fromString(DEFAULT_ERROR_LOG_PATH, true);
  m_accessLogPath = filesystem::value_objects::Path::fromString(
      DEFAULT_ACCESS_LOG_PATH, true);
  m_mimeTypesPath = filesystem::value_objects::Path::fromString(
      DEFAULT_MIME_TYPES_PATH, true);
  m_clientMaxBodySize =
      filesystem::value_objects::Size::fromMegabytes(MAX_CLIENT_BODY_SIZE_GB);
  m_mimeTypesLoaded = false;
}

unsigned int HttpConfig::getWorkerProcesses() const {
  return m_workerProcesses;
}

unsigned int HttpConfig::getWorkerConnections() const {
  return m_workerConnections;
}

unsigned int HttpConfig::getKeepaliveTimeout() const {
  return m_keepaliveTimeout;
}

unsigned int HttpConfig::getSendTimeout() const { return m_sendTimeout; }

const filesystem::value_objects::Path& HttpConfig::getErrorLogPath() const {
  return m_errorLogPath;
}

const filesystem::value_objects::Path& HttpConfig::getAccessLogPath() const {
  return m_accessLogPath;
}

const filesystem::value_objects::Path& HttpConfig::getMimeTypesPath() const {
  return m_mimeTypesPath;
}

const filesystem::value_objects::Size& HttpConfig::getClientMaxBodySize()
    const {
  return m_clientMaxBodySize;
}

const HttpConfig::ServerConfigs& HttpConfig::getServerConfigs() const {
  return m_serverConfigs;
}

const entities::ServerConfig* HttpConfig::selectServer(
    const std::string& host, unsigned int port) const {
  http::value_objects::Host hostObj =
      http::value_objects::Host::fromString(host);
  http::value_objects::Port portObj(port);
  return selectServer(hostObj, portObj);
}

const entities::ServerConfig* HttpConfig::selectServer(
    const http::value_objects::Host& host,
    const http::value_objects::Port& port) const {
  const entities::ServerConfig* selectedServer = NULL;
  const entities::ServerConfig* defaultServer = NULL;

  for (ServerConfigs::const_iterator it = m_serverConfigs.begin();
       it != m_serverConfigs.end(); ++it) {
    if (*it == NULL) {
      continue;
    }

    if ((*it)->matchesRequest(host, port)) {
      if ((*it)->isDefaultServer()) {
        defaultServer = *it;
      } else {
        selectedServer = *it;
        break;
      }
    }
  }

  if (selectedServer != NULL) {
    return selectedServer;
  }

  if (defaultServer != NULL) {
    return defaultServer;
  }

  std::ostringstream oss;
  oss << "No server found for host: " << host.getValue()
      << " and port: " << port.getValue();
  throw exceptions::HttpConfigException(
      oss.str(), exceptions::HttpConfigException::SERVER_SELECTION_FAILED);
}

std::string HttpConfig::getMimeType(const std::string& extension) const {
  if (!m_mimeTypesLoaded) {
    const_cast<HttpConfig*>(this)->loadMimeTypes();
  }

  std::string ext = shared::utils::StringUtils::toLowerCase(
      shared::utils::StringUtils::trim(extension));

  if (!ext.empty() && ext[0] != '.') {
    ext = "." + ext;
  }

  MimeTypesMap::const_iterator iter = m_mimeTypes.find(ext);
  if (iter != m_mimeTypes.end()) {
    return iter->second;
  }

  return "application/octet-stream";
}

void HttpConfig::loadMimeTypes() {
  if (m_mimeTypesLoaded) {
    return;
  }

  try {
    loadMimeTypesFromFile();
    m_mimeTypesLoaded = true;
  } catch (const std::exception& e) {
    m_mimeTypes[".html"] = "text/html";
    m_mimeTypes[".htm"] = "text/html";
    m_mimeTypes[".css"] = "text/css";
    m_mimeTypes[".js"] = "application/javascript";
    m_mimeTypes[".json"] = "application/json";
    m_mimeTypes[".xml"] = "application/xml";
    m_mimeTypes[".txt"] = "text/plain";
    m_mimeTypes[".jpg"] = "image/jpeg";
    m_mimeTypes[".jpeg"] = "image/jpeg";
    m_mimeTypes[".png"] = "image/png";
    m_mimeTypes[".gif"] = "image/gif";
    m_mimeTypes[".pdf"] = "application/pdf";
    m_mimeTypes[".zip"] = "application/zip";
    m_mimeTypes[".tar"] = "application/x-tar";
    m_mimeTypes[".gz"] = "application/gzip";
    m_mimeTypesLoaded = true;
  }
}

bool HttpConfig::hasMimeType(const std::string& extension) const {
  if (!m_mimeTypesLoaded) {
    const_cast<HttpConfig*>(this)->loadMimeTypes();
  }

  std::string ext = shared::utils::StringUtils::toLowerCase(
      shared::utils::StringUtils::trim(extension));

  if (!ext.empty() && ext[0] != '.') {
    ext = "." + ext;
  }

  return m_mimeTypes.find(ext) != m_mimeTypes.end();
}

const HttpConfig::ErrorPagesMap& HttpConfig::getErrorPages() const {
  return m_errorPages;
}

void HttpConfig::addServerConfig(entities::ServerConfig* serverConfig) {
  if (serverConfig == NULL) {
    throw exceptions::HttpConfigException(
        "Cannot add null server config",
        exceptions::HttpConfigException::INVALID_SERVER_CONFIG);
  }

  try {
    serverConfig->validate();
  } catch (const std::exception& e) {
    throw exceptions::HttpConfigException(
        "Invalid server config: " + std::string(e.what()),
        exceptions::HttpConfigException::INVALID_SERVER_CONFIG);
  }

  for (ServerConfigs::const_iterator it = m_serverConfigs.begin();
       it != m_serverConfigs.end(); ++it) {
    if (*it != NULL) {
      if (hasPortConflict(*it, serverConfig)) {
        throw exceptions::HttpConfigException(
            "Port conflict with existing server configuration",
            exceptions::HttpConfigException::DUPLICATE_SERVER);
      }

      if (hasAddressConflict(*it, serverConfig)) {
        throw exceptions::HttpConfigException(
            "Server name conflict with existing server configuration",
            exceptions::HttpConfigException::DUPLICATE_SERVER);
      }
    }
  }

  m_serverConfigs.push_back(serverConfig);
}

void HttpConfig::setWorkerProcesses(unsigned int processes) {
  if (!isValidWorkerCount(processes)) {
    std::ostringstream oss;
    oss << "Invalid worker processes: " << processes << " (must be between "
        << MIN_WORKER_PROCESSES << " and " << MAX_WORKER_PROCESSES << ")";
    throw exceptions::HttpConfigException(
        oss.str(), exceptions::HttpConfigException::INVALID_WORKER_PROCESSES);
  }
  m_workerProcesses = processes;
}

void HttpConfig::setWorkerConnections(unsigned int connections) {
  if (!isValidConnectionCount(connections)) {
    std::ostringstream oss;
    oss << "Invalid worker connections: " << connections << " (must be between "
        << MIN_WORKER_CONNECTIONS << " and " << MAX_WORKER_CONNECTIONS << ")";
    throw exceptions::HttpConfigException(
        oss.str(), exceptions::HttpConfigException::INVALID_WORKER_CONNECTIONS);
  }
  m_workerConnections = connections;
}

void HttpConfig::setKeepaliveTimeout(unsigned int timeout) {
  if (!isValidTimeout(timeout)) {
    std::ostringstream oss;
    oss << "Invalid keepalive timeout: " << timeout << " (must be between "
        << MIN_TIMEOUT << " and " << MAX_KEEPALIVE_TIMEOUT << " seconds)";
    throw exceptions::HttpConfigException(
        oss.str(), exceptions::HttpConfigException::INVALID_KEEPALIVE_TIMEOUT);
  }
  m_keepaliveTimeout = timeout;
}

void HttpConfig::setSendTimeout(unsigned int timeout) {
  if (!isValidTimeout(timeout)) {
    std::ostringstream oss;
    oss << "Invalid send timeout: " << timeout << " (must be between "
        << MIN_TIMEOUT << " and " << MAX_SEND_TIMEOUT << " seconds)";
    throw exceptions::HttpConfigException(
        oss.str(), exceptions::HttpConfigException::INVALID_SEND_TIMEOUT);
  }
  m_sendTimeout = timeout;
}

void HttpConfig::setErrorLogPath(const filesystem::value_objects::Path& path) {
  m_errorLogPath = path;
}

void HttpConfig::setErrorLogPath(const std::string& path) {
  try {
    std::string trimmedPath = shared::utils::StringUtils::trim(path);
    m_errorLogPath =
        filesystem::value_objects::Path::fromString(trimmedPath, false);
  } catch (const std::exception& e) {
    std::ostringstream oss;
    oss << "Invalid error log path: " << e.what();
    throw exceptions::HttpConfigException(
        oss.str(), exceptions::HttpConfigException::INVALID_ERROR_LOG_PATH);
  }
}

void HttpConfig::setAccessLogPath(const filesystem::value_objects::Path& path) {
  m_accessLogPath = path;
}

void HttpConfig::setAccessLogPath(const std::string& path) {
  try {
    std::string trimmedPath = shared::utils::StringUtils::trim(path);
    m_accessLogPath =
        filesystem::value_objects::Path::fromString(trimmedPath, false);
  } catch (const std::exception& e) {
    std::ostringstream oss;
    oss << "Invalid access log path: " << e.what();
    throw exceptions::HttpConfigException(
        oss.str(), exceptions::HttpConfigException::INVALID_ACCESS_LOG_PATH);
  }
}

void HttpConfig::setErrorPage(const shared::value_objects::ErrorCode& code,
                               const std::string& uri) {
  if (uri.empty()) {
    throw exceptions::HttpConfigException(
        "Error page URI cannot be empty",
        exceptions::HttpConfigException::INVALID_ERROR_PAGE);
  }

  std::string trimmedUri = shared::utils::StringUtils::trim(uri);
  
  if (trimmedUri.empty()) {
    throw exceptions::HttpConfigException(
        "Error page URI cannot be empty or whitespace",
        exceptions::HttpConfigException::INVALID_ERROR_PAGE);
  }

  if (trimmedUri[0] != '/') {
    throw exceptions::HttpConfigException(
        "Error page URI must start with '/'",
        exceptions::HttpConfigException::INVALID_ERROR_PAGE);
  }

  m_errorPages[code.getValue()] = trimmedUri;
}

void HttpConfig::setMimeTypesPath(const filesystem::value_objects::Path& path) {
  m_mimeTypesPath = path;
}

void HttpConfig::setMimeTypesPath(const std::string& path) {
  try {
    std::string trimmedPath = shared::utils::StringUtils::trim(path);
    m_mimeTypesPath =
        filesystem::value_objects::Path::fromString(trimmedPath, true);
  } catch (const std::exception& e) {
    std::ostringstream oss;
    oss << "Invalid MIME types path: " << e.what();
    throw exceptions::HttpConfigException(
        oss.str(), exceptions::HttpConfigException::INVALID_MIME_TYPES_PATH);
  }
}

void HttpConfig::setClientMaxBodySize(
    const filesystem::value_objects::Size& size) {
  const filesystem::value_objects::Size MAX_SIZE =
      filesystem::value_objects::Size::fromGigabytes(MAX_CLIENT_BODY_SIZE_GB);
  if (size.getBytes() > MAX_SIZE.getBytes()) {
    std::ostringstream oss;
    oss << "Client max body size too large: " << size.toString()
        << " (maximum is " << MAX_SIZE.toString() << ")";
    throw exceptions::HttpConfigException(
        oss.str(),
        exceptions::HttpConfigException::INVALID_CLIENT_MAX_BODY_SIZE);
  }
  m_clientMaxBodySize = size;
}

void HttpConfig::setClientMaxBodySize(const std::string& sizeString) {
  try {
    std::string trimmedSize = shared::utils::StringUtils::trim(sizeString);
    filesystem::value_objects::Size size =
        filesystem::value_objects::Size::fromString(trimmedSize);
    setClientMaxBodySize(size);
  } catch (const std::exception& e) {
    std::ostringstream oss;
    oss << "Invalid client max body size: " << e.what();
    throw exceptions::HttpConfigException(
        oss.str(),
        exceptions::HttpConfigException::INVALID_CLIENT_MAX_BODY_SIZE);
  }
}

bool HttpConfig::isValid() const {
  try {
    validate();
    return true;
  } catch (const exceptions::HttpConfigException&) {
    return false;
  }
}

void HttpConfig::validate() const {
  validateGlobalConfig();
  validateServerConfigs();
  validateNoPortConflicts();
  validateNoAddressConflicts();
  validateDefaultServers();
}

void HttpConfig::validateGlobalConfig() const {
  if (!isValidWorkerCount(m_workerProcesses)) {
    throw exceptions::HttpConfigException(
        "Invalid worker processes",
        exceptions::HttpConfigException::INVALID_WORKER_PROCESSES);
  }

  if (!isValidConnectionCount(m_workerConnections)) {
    throw exceptions::HttpConfigException(
        "Invalid worker connections",
        exceptions::HttpConfigException::INVALID_WORKER_CONNECTIONS);
  }

  if (!isValidTimeout(m_keepaliveTimeout)) {
    throw exceptions::HttpConfigException(
        "Invalid keepalive timeout",
        exceptions::HttpConfigException::INVALID_KEEPALIVE_TIMEOUT);
  }

  if (!isValidTimeout(m_sendTimeout)) {
    throw exceptions::HttpConfigException(
        "Invalid send timeout",
        exceptions::HttpConfigException::INVALID_SEND_TIMEOUT);
  }

  if (m_errorLogPath.isEmpty()) {
    throw exceptions::HttpConfigException(
        "Error log path cannot be empty",
        exceptions::HttpConfigException::INVALID_ERROR_LOG_PATH);
  }

  if (m_accessLogPath.isEmpty()) {
    throw exceptions::HttpConfigException(
        "Access log path cannot be empty",
        exceptions::HttpConfigException::INVALID_ACCESS_LOG_PATH);
  }

  if (m_mimeTypesPath.isEmpty()) {
    throw exceptions::HttpConfigException(
        "MIME types path cannot be empty",
        exceptions::HttpConfigException::INVALID_MIME_TYPES_PATH);
  }
}

void HttpConfig::validateServerConfigs() const {
  if (m_serverConfigs.empty()) {
    throw exceptions::HttpConfigException(
        "HTTP configuration must contain at least one server",
        exceptions::HttpConfigException::EMPTY_CONFIGURATION);
  }

  for (ServerConfigs::const_iterator it = m_serverConfigs.begin();
       it != m_serverConfigs.end(); ++it) {
    if (*it == NULL) {
      throw exceptions::HttpConfigException(
          "Null server configuration found",
          exceptions::HttpConfigException::INVALID_SERVER_CONFIG);
    }

    try {
      (*it)->validate();
    } catch (const std::exception& e) {
      throw exceptions::HttpConfigException(
          "Server configuration validation failed: " + std::string(e.what()),
          exceptions::HttpConfigException::CONFIG_VALIDATION_FAILED);
    }
  }
}

void HttpConfig::validateNoPortConflicts() const {
  for (size_t i = 0; i < m_serverConfigs.size(); ++i) {
    for (size_t j = i + 1; j < m_serverConfigs.size(); ++j) {
      if (hasPortConflict(m_serverConfigs[i], m_serverConfigs[j])) {
        throw exceptions::HttpConfigException(
            "Port conflict detected between servers",
            exceptions::HttpConfigException::PORT_CONFLICT);
      }
    }
  }
}

void HttpConfig::validateNoAddressConflicts() const {
  for (size_t i = 0; i < m_serverConfigs.size(); ++i) {
    for (size_t j = i + 1; j < m_serverConfigs.size(); ++j) {
      if (hasAddressConflict(m_serverConfigs[i], m_serverConfigs[j])) {
        throw exceptions::HttpConfigException(
            "Address conflict detected between servers",
            exceptions::HttpConfigException::ADDRESS_CONFLICT);
      }
    }
  }
}

void HttpConfig::validateDefaultServers() const {
  std::map<unsigned int, int> defaultServerCounts;

  for (ServerConfigs::const_iterator it = m_serverConfigs.begin();
       it != m_serverConfigs.end(); ++it) {
    if (*it != NULL && (*it)->isDefaultServer()) {
      const entities::ServerConfig::ListenDirectives& directives =
          (*it)->getListenDirectives();
      for (size_t i = 0; i < directives.size(); ++i) {
        if (directives[i].isWildcard()) {
          unsigned int port = directives[i].getPort().getValue();
          defaultServerCounts[port]++;

          // TODO: possible error the defaultServerCounts was > 1
          if (defaultServerCounts[port] > 2) {
            std::ostringstream oss;
            oss << "Multiple default servers found for port " << port;
            throw exceptions::HttpConfigException(
                oss.str(),
                exceptions::HttpConfigException::MULTIPLE_DEFAULT_SERVERS);
          }
        }
      }
    }
  }
}

void HttpConfig::clear() {
  m_workerProcesses = DEFAULT_WORKER_PROCESSES;
  m_workerConnections = DEFAULT_WORKER_CONNECTIONS;
  m_keepaliveTimeout = DEFAULT_KEEPALIVE_TIMEOUT;
  m_sendTimeout = DEFAULT_SEND_TIMEOUT;
  m_errorLogPath =
      filesystem::value_objects::Path::fromString(DEFAULT_ERROR_LOG_PATH, true);
  m_accessLogPath = filesystem::value_objects::Path::fromString(
      DEFAULT_ACCESS_LOG_PATH, true);
  m_mimeTypesPath = filesystem::value_objects::Path::fromString(
      DEFAULT_MIME_TYPES_PATH, true);
  m_clientMaxBodySize =
      filesystem::value_objects::Size::fromMegabytes(MAX_CLIENT_BODY_SIZE_GB);
  m_mimeTypes.clear();
  m_mimeTypesLoaded = false;
  m_errorPages.clear();
  clearServerConfigs();
}

std::string HttpConfig::toString() const {
  std::ostringstream oss;
  oss << "HttpConfig {\n";
  oss << "  WorkerProcesses: " << m_workerProcesses << "\n";
  oss << "  WorkerConnections: " << m_workerConnections << "\n";
  oss << "  KeepaliveTimeout: " << m_keepaliveTimeout << "s\n";
  oss << "  SendTimeout: " << m_sendTimeout << "s\n";
  oss << "  ErrorLogPath: " << m_errorLogPath.toString() << "\n";
  oss << "  AccessLogPath: " << m_accessLogPath.toString() << "\n";
  oss << "  MimeTypesPath: " << m_mimeTypesPath.toString() << "\n";
  oss << "  ClientMaxBodySize: " << m_clientMaxBodySize.toString() << "\n";
  oss << "  ServerConfigs: " << m_serverConfigs.size() << "\n";
  for (size_t i = 0; i < m_serverConfigs.size(); ++i) {
    oss << "    Server[" << i << "]: " << m_serverConfigs[i]->toString()
        << "\n";
  }
  oss << "}";
  return oss.str();
}

HttpConfig HttpConfig::fromFile(const std::string& configFilePath) {
  HttpConfig config;
  config.setMimeTypesPath(configFilePath);
  return config;
}

void HttpConfig::loadMimeTypesFromFile() {
  std::ifstream file(m_mimeTypesPath.toString().c_str());
  if (!file.is_open()) {
    throw std::runtime_error("Cannot open MIME types file: " +
                             m_mimeTypesPath.toString());
  }

  std::string line;
  while (std::getline(file, line) != 0) {
    line = shared::utils::StringUtils::trim(line);

    if (line.empty() || line[0] == '#') {
      continue;
    }

    std::istringstream iss(line);
    std::string mimeType;
    iss >> mimeType;

    if (mimeType.empty()) {
      continue;
    }

    std::string extension;
    while ((iss >> extension) != 0) {
      m_mimeTypes[extension] = mimeType;
    }
  }

  file.close();
}

bool HttpConfig::isValidTimeout(unsigned int timeout) {
  return timeout <= MAX_KEEPALIVE_TIMEOUT;
}

bool HttpConfig::isValidWorkerCount(unsigned int count) {
  return count >= MIN_WORKER_PROCESSES && count <= MAX_WORKER_PROCESSES;
}

bool HttpConfig::isValidConnectionCount(unsigned int count) {
  return count >= MIN_WORKER_CONNECTIONS && count <= MAX_WORKER_CONNECTIONS;
}

bool HttpConfig::hasPortConflict(const entities::ServerConfig* config1,
                                 const entities::ServerConfig* config2) {
  const entities::ServerConfig::ListenDirectives& dir1 =
      config1->getListenDirectives();
  const entities::ServerConfig::ListenDirectives& dir2 =
      config2->getListenDirectives();

  for (size_t dir1Index = 0; dir1Index < dir1.size(); ++dir1Index) {
    for (size_t dir2Index = 0; dir2Index < dir2.size(); ++dir2Index) {
      if (dir1[dir1Index].getPort() == dir2[dir2Index].getPort()) {
        if (dir1[dir1Index].isWildcard() || dir2[dir2Index].isWildcard()) {
          return true;
        }

        if (dir1[dir1Index].getHost() == dir2[dir2Index].getHost()) {
          return true;
        }

        if (dir1[dir1Index].isLocalhost() && dir2[dir2Index].isLocalhost()) {
          return true;
        }
      }
    }
  }

  return false;
}

bool HttpConfig::hasAddressConflict(const entities::ServerConfig* config1,
                                    const entities::ServerConfig* config2) {
  const entities::ServerConfig::ServerNames& names1 = config1->getServerNames();
  const entities::ServerConfig::ServerNames& names2 = config2->getServerNames();

  for (size_t i = 0; i < names1.size(); ++i) {
    for (size_t j = 0; j < names2.size(); ++j) {
      std::string name1Lower =
          shared::utils::StringUtils::toLowerCase(names1[i]);
      std::string name2Lower =
          shared::utils::StringUtils::toLowerCase(names2[j]);

      if (name1Lower == name2Lower) {
        return true;
      }

      if (shared::utils::StringUtils::startsWith(name1Lower, "*.")) {
        std::string suffix = name1Lower.substr(1);
        if (shared::utils::StringUtils::endsWith(name2Lower, suffix)) {
          return true;
        }
      }

      if (shared::utils::StringUtils::startsWith(name2Lower, "*.")) {
        std::string suffix = name2Lower.substr(1);
        if (shared::utils::StringUtils::endsWith(name1Lower, suffix)) {
          return true;
        }
      }
    }
  }

  return false;
}

}  // namespace entities
}  // namespace configuration
}  // namespace domain
