/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigProvider.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/17 13:53:39 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/19 13:03:55 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "application/ports/ILogger.hpp"
#include "infrastructure/adapters/ConfigProvider.hpp"
#include "infrastructure/config/ConfigParser.hpp"
#include "shared/exceptions/ConfigException.hpp"

#include <stdexcept>
#include <string>

namespace infrastructure {
namespace adapters {

ConfigProvider::ConfigProvider(application::ports::ILogger& logger)
    : m_logger(logger), m_valid(false) {
  this->m_parser.reset(new config::ConfigParser(m_logger));
  this->m_logger.info("ConfigProvider initialized with parser and validator.");
}

ConfigProvider::ConfigProvider(const ConfigProvider& other)
    : m_logger(other.m_logger),
      m_globalDirectives(other.m_globalDirectives),
      m_valid(other.m_valid),
      m_configPath(other.m_configPath),
      m_includePath(other.m_includePath) {
  throw std::runtime_error("Copy construction ConfigProvider is disabled");
}

ConfigProvider::~ConfigProvider() {
  // this->m_servers.clear();
  this->m_globalDirectives.clear();
}

ConfigProvider& ConfigProvider::operator=(const ConfigProvider& other) {
  if (this != &other) {
    throw std::runtime_error("Assignment ConfigProvider is disabled");
  }
  return *this;
}

void ConfigProvider::load(const std::string& configPath,
                          const std::string& includePath) {
  this->m_configPath = configPath;
  this->m_includePath = includePath;

  try {
    // this->m_servers.clear();
    this->m_globalDirectives.clear();
    this->m_valid = false;

    this->m_parser->parseFile(configPath);
    this->m_parser->mergeIncludes(includePath);

    if (validateConfig()) {
      this->m_valid = true;
      // std::ostringstream oss;
      // oss << "Configuration loaded successfully: " << this->m_servers.size()
      //     << " servers configured.";
      // this->m_logger.info(oss.str());
    } else {
      throw shared::exceptions::ConfigException(
          "Post-parsing validation failed: invalid server configurations",
          shared::exceptions::ConfigException::VALIDATION_MISSING_DIRECTIVE);
    }
  } catch (const shared::exceptions::ConfigException& exception) {
    throw;
  } catch (const std::exception& exception) {
    throw shared::exceptions::ConfigException(
        "Unexpected error during configuration load",
        shared::exceptions::ConfigException::LOAD_UNEXPECTED);
  }
}

// const domain::entities::ServerConfig& ConfigProvider::getServerConfig(const
// std::string& uri) const {
//   for (std::size_t i = 0; i < this->m_servers.size(); ++i) {
//     if (this->m_servers[i]->matches_uri(uri)) {
//       return *this->m_server[i];
//     }
//   }
//   throw "No server config for URI: " + uri;
// }

// const domain::entities::LocationConfig&
// ConfigProvider::getLocationConfig(const domain::entities::ServerConfig&
// server, const std::string& path) const {
//   const domain::entities::LocationConfig* best = nullptr;
//   std::size_t bestLen = 0;
//
//   * make logic of get the location
//   if (best) {
//     return *best;
//   }
//
//   throw "No location for path: " + path;
// }

// std::vector<const domain::entities::ServerConfig*>
// ConfigProvider::getAllServers() const {
//   std::vector<const domain::entities::ServerConfig*> allServers;
//   for (std::size_t i = 0; i < this->m_servers.size(); ++i) {
//     allServers.push_back(this->me_servers[i].get());
//   }
//   return allServers;
// }

void ConfigProvider::reload() {
  // if (m_configPath.empty()) {
  //   throw shared::exceptions::ConfigException(
  //       "Cannot reload: no configuration path stored",
  //       shared::exceptions::ConfigException::INVALID_STATE);
  // }
  load(this->m_configPath, this->m_includePath);
}

bool ConfigProvider::validateConfig() const {
  // std::set<int> ports;
  // for (std::size_t i = 0; this->m_servers.size(); ++i) {
  //   int port = this->m_servers[i]->listen_port();
  //   if (ports.count(port)) {
  //     return false;
  //   }
  //   ports.insert(port);
  //   // root.path.validation.via.filesystem::PathResolver
  // }
  return true;
}

// void ConfigProvider::parseServerBlock() {}

// void ConfigProvider::parseLocationBlock() {}

// std::size_t ConfigProvider::serverCount() const {
//   return this->m_server.size();
// }

// bool ConfigProvider::isValid() const {
//   return this->m_valid && !this->m_servers.empty() && validateConfig();
// }

}  // namespace adapters
}  // namespace infrastructure
