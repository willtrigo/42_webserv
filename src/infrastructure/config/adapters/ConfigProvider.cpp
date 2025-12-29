/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigProvider.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/17 13:53:39 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/28 16:56:44 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "application/ports/ILogger.hpp"
#include "infrastructure/config/adapters/ConfigProvider.hpp"
#include "infrastructure/config/exceptions/ConfigException.hpp"
#include "infrastructure/config/parsers/ConfigParser.hpp"

#include <sstream>
#include <string>

namespace infrastructure {
namespace config {
namespace adapters {

ConfigProvider::ConfigProvider(application::ports::ILogger& logger)
    : m_logger(logger), m_valid(false) {
  this->m_parser.reset(new parsers::ConfigParser(m_logger));
  this->m_logger.info("ConfigProvider initialized with parser.");
}

ConfigProvider::~ConfigProvider() {}

void ConfigProvider::load(const std::string& configPath,
                          const std::string& includePath) {
  this->m_configPath = configPath;
  this->m_includePath = includePath;
  this->m_valid = false;
  this->m_serverPtrs.clear();

  try {
    // Parse main configuration file
    domain::configuration::entities::HttpConfig* httpConfig =
        this->m_parser->parseFile(configPath);

    // Merge includes if any
    this->m_parser->mergeIncludes(*httpConfig, includePath);

    // Validate the complete configuration
    this->m_parser->validateConfiguration(*httpConfig);

    // Take ownership
    this->m_httpConfig.reset(httpConfig);

    // Update server pointers
    updateServerPointers();

    this->m_valid = true;

    std::ostringstream oss;
    oss << "Configuration loaded successfully from " << configPath << " with "
        << this->m_serverPtrs.size() << " servers";
    this->m_logger.info(oss.str());

  } catch (const exceptions::ConfigException& exception) {
    this->m_logger.error("ConfigException: " + std::string(exception.what()));
    throw;
  } catch (const std::exception& exception) {
    this->m_logger.error("Exception: " + std::string(exception.what()));
    throw exceptions::ConfigException(
        "Unexpected error during configuration load: " +
            std::string(exception.what()),
        exceptions::ConfigException::LOAD_UNEXPECTED);
  }
}

const domain::configuration::entities::HttpConfig&
ConfigProvider::getConfiguration() const {
  if (this->m_httpConfig.get() == 0) {
    throw exceptions::ConfigException(
        "Configuration not loaded", exceptions::ConfigException::INVALID_STATE);
  }
  return *this->m_httpConfig;
}

const domain::configuration::entities::ServerConfig* ConfigProvider::findServer(
    const std::string& host, unsigned int port) const {
  if (this->m_httpConfig.get() == 0) {
    return NULL;
  }
  return this->m_httpConfig->selectServer(host, port);
}

const domain::configuration::entities::LocationConfig*
ConfigProvider::findLocation(
    const domain::configuration::entities::ServerConfig& server,
    const std::string& path) const {
  return server.findLocation(path);
}

const std::vector<const domain::configuration::entities::ServerConfig*>&
ConfigProvider::getAllServers() const {
  return this->m_serverPtrs;
}

void ConfigProvider::reload() {
  if (this->m_configPath.empty()) {
    throw exceptions::ConfigException(
        "Cannot reload: no configuration path stored",
        exceptions::ConfigException::INVALID_STATE);
  }
  load(this->m_configPath, this->m_includePath);
}

bool ConfigProvider::isValid() const {
  return this->m_valid && this->m_httpConfig.get() != NULL;
}

void ConfigProvider::updateServerPointers() {
  this->m_serverPtrs.clear();
  if (this->m_httpConfig.get() != 0) {
    const domain::configuration::entities::HttpConfig::ServerConfigs& servers =
        this->m_httpConfig->getServerConfigs();
    for (domain::configuration::entities::HttpConfig::ServerConfigs::
             const_iterator it = servers.begin();
         it != servers.end(); ++it) {
      this->m_serverPtrs.push_back(*it);
    }
  }
}

}  // namespace adapters
}  // namespace config
}  // namespace infrastructure
