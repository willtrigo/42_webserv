/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: umeneses <umeneses@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/22 11:52:37 by dande-je          #+#    #+#             */
/*   Updated: 2026/01/05 20:36:27 by umeneses         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "domain/configuration/entities/ServerConfig.hpp"
#include "domain/configuration/exceptions/ServerConfigException.hpp"
#include "domain/shared/utils/StringUtils.hpp"

#include <sstream>

namespace domain {
namespace configuration {
namespace entities {

const std::string ServerConfig::DEFAULT_ROOT = "/var/www/html";
const std::string ServerConfig::DEFAULT_INDEX = "index.html";

ServerConfig::ServerConfig()
    : m_root(filesystem::value_objects::Path::fromString(DEFAULT_ROOT, true)),
      m_clientMaxBodySize(filesystem::value_objects::Size::fromMegabytes(
          DEFAULT_CLIENT_MAX_BODY_SIZE_MB)),
      m_returnCode(shared::value_objects::ErrorCode::ok()) {}

ServerConfig::ServerConfig(const ListenDirectives& listenDirectives)
    : m_listenDirectives(listenDirectives),
      m_root(filesystem::value_objects::Path::fromString(DEFAULT_ROOT, true)),
      m_clientMaxBodySize(filesystem::value_objects::Size::fromMegabytes(
          DEFAULT_CLIENT_MAX_BODY_SIZE_MB)),
      m_returnCode(shared::value_objects::ErrorCode::ok()) {}

ServerConfig::ServerConfig(const ServerConfig& other) { copyFrom(other); }

ServerConfig::~ServerConfig() { clearLocations(); }

ServerConfig& ServerConfig::operator=(const ServerConfig& other) {
  if (this != &other) {
    clear();
    copyFrom(other);
  }
  return *this;
}

void ServerConfig::copyFrom(const ServerConfig& other) {
  m_listenDirectives = other.m_listenDirectives;
  m_serverNames = other.m_serverNames;
  m_root = other.m_root;
  m_indexFiles = other.m_indexFiles;
  m_errorPages = other.m_errorPages;
  m_clientMaxBodySize = other.m_clientMaxBodySize;
  m_returnRedirect = other.m_returnRedirect;
  m_returnCode = other.m_returnCode;

  for (std::size_t i = 0; i < other.m_locations.size(); ++i) {
    if (other.m_locations[i] != NULL) {
      m_locations.push_back(new LocationConfig(*other.m_locations[i]));
    }
  }
}

void ServerConfig::clearLocations() {
  for (std::size_t i = 0; i < m_locations.size(); ++i) {
    delete m_locations[i];
  }
  m_locations.clear();
}

const ServerConfig::ListenDirectives& ServerConfig::getListenDirectives()
    const {
  return m_listenDirectives;
}

const ServerConfig::ServerNames& ServerConfig::getServerNames() const {
  return m_serverNames;
}

const filesystem::value_objects::Path& ServerConfig::getRoot() const {
  return m_root;
}

const std::vector<std::string>& ServerConfig::getIndexFiles() const {
  return m_indexFiles;
}

const ServerConfig::ErrorPageMap& ServerConfig::getErrorPages() const {
  return m_errorPages;
}

const ServerConfig::Locations& ServerConfig::getLocations() const {
  return m_locations;
}

const filesystem::value_objects::Size& ServerConfig::getClientMaxBodySize()
    const {
  return m_clientMaxBodySize;
}

const std::string& ServerConfig::getReturnRedirect() const {
  return m_returnRedirect;
}

const shared::value_objects::ErrorCode& ServerConfig::getReturnCode() const {
  return m_returnCode;
}

bool ServerConfig::isDefaultServer() const {
  for (std::size_t i = 0; i < m_listenDirectives.size(); ++i) {
    if (m_listenDirectives[i].isWildcard() || m_serverNames.empty()) {
      return true;
    }
  }
  return false;
}

std::string ServerConfig::normalizeListenDirective(
    const std::string& directive) {
  std::string trimmed = shared::utils::StringUtils::trim(directive);

  // Already has colon - return as-is
  if (trimmed.find(':') != std::string::npos) {
    return trimmed;
  }

  // Check if it's a port-only format (all digits)
  if (shared::utils::StringUtils::isAllDigits(trimmed)) {
    return "0.0.0.0:" + trimmed;
  }

  // Check if it's an IPv6 without port
  if (trimmed.find("::") != std::string::npos ||
      (trimmed.length() > 0 && trimmed[0] == '[')) {
    return trimmed + ":80";
  }

  // Otherwise it's a hostname/IP without port
  return trimmed + ":80";
}

void ServerConfig::addListenDirective(const ListenDirective& directive) {
  for (std::size_t i = 0; i < m_listenDirectives.size(); ++i) {
    if (m_listenDirectives[i] == directive) {
      std::ostringstream oss;
      oss << directive.toString();
      throw exceptions::ServerConfigException(
          oss.str(), exceptions::ServerConfigException::DUPLICATE_LISTEN);
    }
  }
  m_listenDirectives.push_back(directive);
}

void ServerConfig::addListenDirective(const std::string& directiveString) {
  try {
    std::string normalized = normalizeListenDirective(directiveString);
    ListenDirective directive = ListenDirective::fromString(normalized);
    addListenDirective(directive);
  } catch (const std::exception& e) {
    std::ostringstream oss;
    oss << "Invalid listen directive format: " << directiveString << " - "
        << e.what();
    throw exceptions::ServerConfigException(
        oss.str(),
        exceptions::ServerConfigException::INVALID_LISTEN_DIRECTIVE_FORMAT);
  }
}

void ServerConfig::addServerName(const std::string& name) {
  std::string trimmedName = shared::utils::StringUtils::trim(name);

  if (trimmedName.empty()) {
    throw exceptions::ServerConfigException(
        "Server name cannot be empty",
        exceptions::ServerConfigException::EMPTY_SERVER_NAME);
  }

  if (!isValidServerName(trimmedName)) {
    std::ostringstream oss;
    oss << "Invalid server name: '" << trimmedName << "'";
    throw exceptions::ServerConfigException(
        oss.str(), exceptions::ServerConfigException::INVALID_SERVER_NAME);
  }

  for (std::size_t i = 0; i < m_serverNames.size(); ++i) {
    if (m_serverNames[i] == trimmedName) {
      std::ostringstream oss;
      oss << "Duplicate server name: '" << trimmedName << "'";
      throw exceptions::ServerConfigException(
          oss.str(), exceptions::ServerConfigException::DUPLICATE_SERVER_NAME);
    }
  }

  m_serverNames.push_back(trimmedName);
}

void ServerConfig::setRoot(const filesystem::value_objects::Path& root) {
  m_root = root;
}

void ServerConfig::setRoot(const std::string& root) {
  try {
    std::string trimmedRoot = shared::utils::StringUtils::trim(root);
    m_root = filesystem::value_objects::Path::fromString(trimmedRoot, false);
  } catch (const std::exception& e) {
    std::ostringstream oss;
    oss << "Invalid root path: " << e.what();
    throw exceptions::ServerConfigException(
        oss.str(), exceptions::ServerConfigException::INVALID_ROOT_PATH);
  }
}

void ServerConfig::addIndexFile(const std::string& index) {
  std::string trimmedIndex = shared::utils::StringUtils::trim(index);

  if (trimmedIndex.empty()) {
    throw exceptions::ServerConfigException(
        "Index file cannot be empty",
        exceptions::ServerConfigException::EMPTY_INDEX_FILE);
  }

  for (std::size_t i = 0; i < m_indexFiles.size(); ++i) {
    if (m_indexFiles[i] == trimmedIndex) {
      std::ostringstream oss;
      oss << "Duplicate index file: '" << trimmedIndex << "'";
      throw exceptions::ServerConfigException(
          oss.str(), exceptions::ServerConfigException::DUPLICATE_INDEX_FILE);
    }
  }

  m_indexFiles.push_back(trimmedIndex);
}

void ServerConfig::addErrorPage(const shared::value_objects::ErrorCode& code,
                                const std::string& uri) {
  if (!code.isError()) {
    std::ostringstream oss;
    oss << "Invalid error code for error page: " << code.getValue()
        << " (must be an error code between 400 and 599)";
    throw exceptions::ServerConfigException(
        oss.str(), exceptions::ServerConfigException::INVALID_ERROR_CODE);
  }

  std::string trimmedUri = shared::utils::StringUtils::trim(uri);

  if (trimmedUri.empty()) {
    std::ostringstream oss;
    oss << "Error page URI cannot be empty for error code " << code.getValue();
    throw exceptions::ServerConfigException(
        oss.str(), exceptions::ServerConfigException::EMPTY_ERROR_PAGE_URI);
  }

  if (trimmedUri[0] != '/') {
    std::ostringstream oss;
    oss << "Error page URI must start with '/': " << trimmedUri
        << " for error code " << code.getValue();
    throw exceptions::ServerConfigException(
        oss.str(), exceptions::ServerConfigException::INVALID_ERROR_PAGE_URI);
  }

  m_errorPages[code] = trimmedUri;
}

void ServerConfig::addLocation(LocationConfig* location) {
  if (location == NULL) {
    throw exceptions::ServerConfigException(
        "Cannot add null location",
        exceptions::ServerConfigException::NULL_LOCATION);
  }

  try {
    location->validate();
  } catch (const std::exception& e) {
    std::ostringstream oss;
    oss << "Location validation failed: " << e.what();
    throw exceptions::ServerConfigException(
        oss.str(),
        exceptions::ServerConfigException::LOCATION_VALIDATION_FAILED);
  }

  for (std::size_t i = 0; i < m_locations.size(); ++i) {
    if (m_locations[i] != NULL &&
        m_locations[i]->getPath() == location->getPath()) {
      std::ostringstream oss;
      oss << "Duplicate location path: '" << location->getPath() << "'";
      throw exceptions::ServerConfigException(
          oss.str(),
          exceptions::ServerConfigException::DUPLICATE_LOCATION_PATH);
    }
  }

  m_locations.push_back(location);
}

void ServerConfig::setClientMaxBodySize(
    const filesystem::value_objects::Size& size) {
  const filesystem::value_objects::Size MAX_BODY_SIZE =
      filesystem::value_objects::Size::fromMegabytes(
          MAX_CLIENT_MAX_BODY_SIZE_MB);

  if (size.getBytes() > MAX_BODY_SIZE.getBytes()) {
    std::ostringstream oss;
    oss << "Client max body size too large: " << size.toString()
        << " (maximum is " << MAX_BODY_SIZE.toString() << ")";
    throw exceptions::ServerConfigException(
        oss.str(), exceptions::ServerConfigException::BODY_SIZE_TOO_LARGE);
  }

  m_clientMaxBodySize = size;
}

void ServerConfig::setClientMaxBodySize(const std::string& sizeString) {
  try {
    std::string trimmedSize = shared::utils::StringUtils::trim(sizeString);
    filesystem::value_objects::Size size =
        filesystem::value_objects::Size::fromString(trimmedSize);
    setClientMaxBodySize(size);
  } catch (const std::exception& e) {
    std::ostringstream oss;
    oss << "Invalid client max body size: " << e.what();
    throw exceptions::ServerConfigException(
        oss.str(), exceptions::ServerConfigException::BODY_SIZE_TOO_LARGE);
  }
}

void ServerConfig::setReturnRedirect(
    const std::string& redirect, const shared::value_objects::ErrorCode& code) {
  std::string trimmedRedirect = shared::utils::StringUtils::trim(redirect);

  if (trimmedRedirect.empty()) {
    throw exceptions::ServerConfigException(
        "Return redirect URL cannot be empty",
        exceptions::ServerConfigException::EMPTY_REDIRECT_URL);
  }

  if (!code.isRedirection()) {
    std::ostringstream oss;
    oss << "Invalid redirect code: " << code.getValue()
        << " (must be a redirection code between 300 and 399)";
    throw exceptions::ServerConfigException(
        oss.str(), exceptions::ServerConfigException::INVALID_REDIRECT_CODE);
  }

  m_returnRedirect = trimmedRedirect;
  m_returnCode = code;
}

void ServerConfig::setReturnRedirect(const std::string& redirect,
                                     unsigned int code) {
  try {
    std::string trimmedRedirect = shared::utils::StringUtils::trim(redirect);
    shared::value_objects::ErrorCode errorCode(code);
    setReturnRedirect(trimmedRedirect, errorCode);
  } catch (const std::exception& e) {
    std::ostringstream oss;
    oss << "Invalid redirect code: " << e.what();
    throw exceptions::ServerConfigException(
        oss.str(), exceptions::ServerConfigException::INVALID_REDIRECT_CODE);
  }
}

void ServerConfig::setReturnContent(
    const std::string& content, const shared::value_objects::ErrorCode& code) {
  std::string trimmedContent = shared::utils::StringUtils::trim(content);

  // Validate code is NOT a redirect code
  if (code.isRedirection()) {
    std::ostringstream oss;
    oss << "Return content code cannot be a redirect code: " << code.getValue()
        << " (must be 2xx, 4xx, or 5xx)";
    throw exceptions::ServerConfigException(
        oss.str(), exceptions::ServerConfigException::INVALID_RETURN_CODE);
  }

  // Validate it's a valid return code (2xx, 4xx, or 5xx)
  if (!code.isSuccess() && !code.isClientError() && !code.isServerError()) {
    std::ostringstream oss;
    oss << "Invalid return content code: " << code.getValue()
        << " (must be 2xx, 4xx, or 5xx)";
    throw exceptions::ServerConfigException(
        oss.str(), exceptions::ServerConfigException::INVALID_RETURN_CODE);
  }

  m_returnContent = trimmedContent;
  m_returnCode = code;
}

void ServerConfig::setReturnContent(const std::string& content,
                                    unsigned int code) {
  try {
    std::string trimmedContent = shared::utils::StringUtils::trim(content);
    shared::value_objects::ErrorCode errorCode(code);
    setReturnContent(trimmedContent, errorCode);
  } catch (const std::exception& e) {
    std::ostringstream oss;
    oss << "Invalid return content: " << e.what();
    throw exceptions::ServerConfigException(
        oss.str(), exceptions::ServerConfigException::INVALID_RETURN_CODE);
  }
}

const std::string& ServerConfig::getReturnContent() const {
  return m_returnContent;
}

bool ServerConfig::hasReturnRedirect() const {
  return !m_returnRedirect.empty() && m_returnCode.isRedirection();
}

bool ServerConfig::hasReturnContent() const {
  return !m_returnContent.empty() &&
         (m_returnCode.isSuccess() || m_returnCode.isClientError() ||
          m_returnCode.isServerError());
}

bool ServerConfig::isValid() const {
  try {
    validate();
    return true;
  } catch (const exceptions::ServerConfigException&) {
    return false;
  }
}

void ServerConfig::validate() const {
  validateListenDirectives();
  validateServerNames();
  validateRoot();
  validateErrorPages();
  validateLocations();
  validateClientMaxBodySize();

  if (m_locations.empty() && m_root.isEmpty() && m_returnRedirect.empty()) {
    throw exceptions::ServerConfigException(
        "Server must have at least one location, a root, or a return directive",
        exceptions::ServerConfigException::MISSING_CONFIGURATION);
  }
}

void ServerConfig::validateListenDirectives() const {
  if (m_listenDirectives.empty()) {
    throw exceptions::ServerConfigException(
        "Server must have at least one listen directive",
        exceptions::ServerConfigException::MISSING_LISTEN_DIRECTIVE);
  }
}

void ServerConfig::validateServerNames() const {
  for (std::size_t i = 0; i < m_serverNames.size(); ++i) {
    const std::string& name = m_serverNames[i];
    if (!isValidServerName(name)) {
      std::ostringstream oss;
      oss << "Invalid server name format: '" << name << "'";
      throw exceptions::ServerConfigException(
          oss.str(), exceptions::ServerConfigException::INVALID_SERVER_NAME);
    }
  }
}

void ServerConfig::validateRoot() const {
  if (m_root.isEmpty()) {
    bool allLocationsHaveRoot = true;
    for (std::size_t i = 0; i < m_locations.size(); ++i) {
      if (m_locations[i] != NULL && m_locations[i]->getRoot().isEmpty()) {
        allLocationsHaveRoot = false;
        break;
      }
    }

    if (!allLocationsHaveRoot) {
      throw exceptions::ServerConfigException(
          "Server root must be set if any location doesn't have its own root",
          exceptions::ServerConfigException::MISSING_ROOT);
    }
  }
}

void ServerConfig::validateErrorPages() const {
  for (ErrorPageMap::const_iterator it = m_errorPages.begin();
       it != m_errorPages.end(); ++it) {
    const std::string& uri = it->second;

    if (uri.empty() || uri[0] != '/') {
      std::ostringstream oss;
      oss << "Invalid error page URI for error " << it->first.getValue()
          << ": '" << uri << "' (must start with '/')";
      throw exceptions::ServerConfigException(
          oss.str(), exceptions::ServerConfigException::INVALID_ERROR_PAGE_URI);
    }
  }
}

void ServerConfig::validateLocations() const {
  for (std::size_t i = 0; i < m_locations.size(); ++i) {
    if (m_locations[i] == NULL) {
      throw exceptions::ServerConfigException(
          "Server contains null location",
          exceptions::ServerConfigException::NULL_LOCATION);
    }
  }
}

void ServerConfig::validateClientMaxBodySize() const {
  const filesystem::value_objects::Size MAX_BODY_SIZE =
      filesystem::value_objects::Size::fromMegabytes(
          MAX_CLIENT_MAX_BODY_SIZE_MB);

  if (m_clientMaxBodySize.getBytes() > MAX_BODY_SIZE.getBytes()) {
    std::ostringstream oss;
    oss << "Client max body size exceeds limit: "
        << m_clientMaxBodySize.toString() << " (maximum is "
        << MAX_BODY_SIZE.toString() << ")";
    throw exceptions::ServerConfigException(
        oss.str(), exceptions::ServerConfigException::BODY_SIZE_TOO_LARGE);
  }
}

bool ServerConfig::matchesRequest(const std::string& host,
                                  unsigned int port) const {
  http::value_objects::Port portObj(port);
  http::value_objects::Host hostObj =
      http::value_objects::Host::fromString(host);
  return matchesRequest(hostObj, portObj);
}

bool ServerConfig::matchesRequest(const http::value_objects::Host& host,
                                  const http::value_objects::Port& port) const {
  bool portMatches = false;
  for (std::size_t i = 0; i < m_listenDirectives.size(); ++i) {
    if (m_listenDirectives[i].getPort() == port) {
      portMatches = true;
      break;
    }
  }

  if (!portMatches) {
    return false;
  }

  if (m_serverNames.empty()) {
    return true;
  }

  std::string hostStr = host.getValue();
  for (std::size_t i = 0; i < m_serverNames.size(); ++i) {
    if (matchesServerName(m_serverNames[i], hostStr)) {
      return true;
    }
  }

  return false;
}

const LocationConfig* ServerConfig::findLocation(
    const std::string& uriPath) const {
  const LocationConfig* bestMatch = NULL;
  std::size_t bestLength = 0;

  for (std::size_t i = 0; i < m_locations.size(); ++i) {
    if (m_locations[i] == NULL) {
      continue;
    }

    if (m_locations[i]->matchesPath(uriPath)) {
      if (m_locations[i]->getPath() == uriPath) {
        return m_locations[i];
      }

      std::size_t pathLength = m_locations[i]->getPath().length();
      if (pathLength > bestLength) {
        bestLength = pathLength;
        bestMatch = m_locations[i];
      }
    }
  }

  return bestMatch;
}

bool ServerConfig::hasListenDirective(const std::string& address,
                                      unsigned int port) const {
  http::value_objects::Port portObj(port);
  http::value_objects::Host hostObj =
      http::value_objects::Host::fromString(address);
  return hasListenDirective(hostObj, portObj);
}

bool ServerConfig::hasListenDirective(
    const http::value_objects::Host& host,
    const http::value_objects::Port& port) const {
  ListenDirective directive(host, port);
  for (std::size_t i = 0; i < m_listenDirectives.size(); ++i) {
    if (m_listenDirectives[i] == directive) {
      return true;
    }
  }
  return false;
}

void ServerConfig::setListenDirectives(
    const std::vector<std::string>& directives) {
  m_listenDirectives.clear();

  for (std::size_t i = 0; i < directives.size(); ++i) {
    addListenDirective(directives[i]);
  }
}

void ServerConfig::setListenDirectives(const ListenDirectives& directives) {
  m_listenDirectives.clear();

  for (std::size_t i = 0; i < directives.size(); ++i) {
    addListenDirective(directives[i]);
  }
}

void ServerConfig::clear() {
  m_listenDirectives.clear();
  m_serverNames.clear();
  m_root = filesystem::value_objects::Path::fromString(DEFAULT_ROOT, true);
  m_indexFiles.clear();
  m_errorPages.clear();
  clearLocations();
  m_clientMaxBodySize = filesystem::value_objects::Size::fromMegabytes(
      DEFAULT_CLIENT_MAX_BODY_SIZE_MB);
  m_returnRedirect.clear();
  m_returnCode = shared::value_objects::ErrorCode::ok();
}

std::string ServerConfig::toString() const {
  std::ostringstream oss;
  oss << "ServerConfig {\n";
  oss << "  ListenDirectives: ";
  for (std::size_t i = 0; i < m_listenDirectives.size(); ++i) {
    if (i > 0) oss << ", ";
    oss << m_listenDirectives[i].toString();
  }
  oss << "\n";
  oss << "  ServerNames: ";
  for (std::size_t i = 0; i < m_serverNames.size(); ++i) {
    if (i > 0) oss << ", ";
    oss << "'" << m_serverNames[i] << "'";
  }
  oss << "\n";
  oss << "  Root: " << m_root.toString() << "\n";
  oss << "  IndexFiles: ";
  for (std::size_t i = 0; i < m_indexFiles.size(); ++i) {
    if (i > 0) oss << ", ";
    oss << "'" << m_indexFiles[i] << "'";
  }
  oss << "\n";
  oss << "  ClientMaxBodySize: " << m_clientMaxBodySize.toString() << "\n";
  if (hasReturnRedirect()) {
    oss << "  ReturnRedirect: " << m_returnCode.getValue() << " -> '"
        << m_returnRedirect << "'\n";
  } else if (hasReturnContent()) {
    oss << "  ReturnContent: " << m_returnCode.getValue() << " -> '"
        << m_returnContent << "'\n";
  }
  oss << "  Locations: " << m_locations.size() << "\n";
  oss << "}";
  return oss.str();
}

bool ServerConfig::isValidServerName(const std::string& name) {
  if (name.empty()) {
    return false;
  }

  if (name == "_") {
    return true;
  }

  if (shared::utils::StringUtils::startsWith(name, "*.")) {
    std::string rest = name.substr(2);
    if (rest.empty() || rest.find('*') != std::string::npos) {
      return false;
    }
    return isValidServerName(rest);
  }

  for (std::size_t i = 0; i < name.length(); ++i) {
    unsigned char chr = name[i];
    if ((std::isalnum(chr) == 0) && chr != '-' && chr != '.' && chr != '_') {
      return false;
    }
  }

  if (name[0] == '.' || name[name.length() - 1] == '.') {
    return false;
  }

  if (name.find("..") != std::string::npos) {
    return false;
  }

  return true;
}

bool ServerConfig::isWildcardServerName(const std::string& name) {
  return name == "_" || shared::utils::StringUtils::startsWith(name, "*.");
}

bool ServerConfig::matchesServerName(const std::string& configName,
                                     const std::string& requestName) {
  if (configName == requestName) {
    return true;
  }

  if (configName == "_") {
    return true;
  }

  if (shared::utils::StringUtils::startsWith(configName, "*.")) {
    std::string suffix = configName.substr(1);  // Keep the dot
    if (shared::utils::StringUtils::endsWith(requestName, suffix)) {
      if (requestName.length() > suffix.length()) {
        return true;
      }
    }
  }

  return false;
}

}  // namespace entities
}  // namespace configuration
}  // namespace domain
