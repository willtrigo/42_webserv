/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CliController.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/12 16:08:17 by dande-je          #+#    #+#             */
/*   Updated: 2026/01/02 13:40:19 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "application/ports/IConfigProvider.hpp"
#include "infrastructure/config/adapters/ConfigProvider.hpp"
#include "presentation/cli/CliController.hpp"
#include "presentation/cli/CliView.hpp"

#include <cstdlib>
#include <memory>
#include <stdexcept>
#include <string>
#include <sstream>

CliController::CliController(CliView& view) : m_view(view) {}

CliController::CliController(const CliController& other)
    : m_view(other.m_view) {}

CliController::~CliController() {}

CliController& CliController::operator=(const CliController& other) {
  if (this == &other) {
    return *this;
  }
  throw std::runtime_error("CliController assignment is not allowed");
}

bool CliController::run(int argc, char** argv) {
  if (!parseArguments(argc, argv)) {
    return false;
  }

  try {
    const std::string configPath =
        ((argv[K_LITERAL_ARGUMENT_INDEX] != 0)
             ? std::string(argv[K_LITERAL_ARGUMENT_INDEX])
             : "conf/default.conf");

    std::auto_ptr<application::ports::IConfigProvider> configProvider(
        new infrastructure::config::adapters::ConfigProvider(
            this->m_view.getLogger()));
    configProvider->load(configPath);
    // configProvider->load(configPath, "conf/default.conf");
    if (configProvider->isValid()) {
      const domain::configuration::entities::HttpConfig& config =
          configProvider->getConfiguration();

      std::ostringstream oss;
      oss << "Configuration loaded successfully:\n";
      oss << "  Worker processes: " << config.getWorkerProcesses() << "\n";
      oss << "  Worker connections: " << config.getWorkerConnections() << "\n";
      oss << "  Servers: " << config.getServerConfigs().size() << "\n";

      // Display server information
      const std::vector<const domain::configuration::entities::ServerConfig*>&
          servers = configProvider->getAllServers();
      for (size_t i = 0; i < servers.size(); ++i) {
        const domain::configuration::entities::ServerConfig* server =
            servers[i];
        oss << "\n  Server " << (i + 1) << ":\n";
        if (!server->getServerNames().empty()) {
          oss << "    Server names: ";
          const domain::configuration::entities::ServerConfig::ServerNames&
              names = server->getServerNames();
          for (size_t j = 0; j < names.size(); ++j) {
            if (j > 0) oss << ", ";
            oss << names[j];
          }
          oss << "\n";
        }
        oss << "    Locations: " << server->getLocations().size() << "\n";
      }

      this->m_view.getLogger().info(oss.str());
    }
  } catch (const std::exception& exception) {
    this->m_view.displayError(exception.what());
    return false;
  }
  return true;
}

bool CliController::parseArguments(int argc, char** argv) {
  if (argc > K_MAX_SIZE_ARGS) {
    this->m_view.displayUsage(std::string(argv[K_NAME_PROGRAM]));
    return false;
  }

  return true;
}
