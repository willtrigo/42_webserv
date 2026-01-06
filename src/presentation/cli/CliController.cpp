/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CliController.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/12 16:08:17 by dande-je          #+#    #+#             */
/*   Updated: 2026/01/06 19:12:03 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "application/ports/IConfigProvider.hpp"
#include "infrastructure/config/adapters/ConfigProvider.hpp"
#include "infrastructure/network/adapters/SocketOrchestrator.hpp"
#include "presentation/cli/CliController.hpp"
#include "presentation/cli/CliView.hpp"
#include "shared/utils/SignalHandler.hpp"

#include <csignal>
#include <cstdlib>
#include <sstream>
#include <stdexcept>
#include <string>

namespace presentation {
namespace cli {

CliController::CliController(CliView& view)
    : m_view(view), m_configProvider(NULL), m_orchestrator(NULL) {}

CliController::CliController(const CliController& other)
    : m_view(other.m_view), m_configProvider(NULL), m_orchestrator(NULL) {}

CliController::~CliController() {
  delete m_orchestrator;
  m_orchestrator = NULL;

  delete m_configProvider;
  m_configProvider = NULL;
}

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

  const std::string configPath =
      ((argv[K_LITERAL_ARGUMENT_INDEX] != 0)
           ? std::string(argv[K_LITERAL_ARGUMENT_INDEX])
           : "conf/default.conf");

  if (!loadConfiguration(configPath)) {
    return false;
  }

  displayConfigurationSummary(*m_configProvider);

  if (!startServer()) {
    return false;
  }

  runServerLoop();

  stopServer();

  m_view.getLogger().info("Webserv shutdown complete");
  return true;
}
// try {
//   std::auto_ptr<application::ports::IConfigProvider> configProvider(
//       new infrastructure::config::adapters::ConfigProvider(
//           this->m_view.getLogger()));
//   configProvider->load(configPath);
//   if (configProvider->isValid()) {
//     const domain::configuration::entities::HttpConfig& config =
//         configProvider->getConfiguration();
//
//     std::ostringstream oss;
//     oss << "Configuration loaded successfully:\n";
//     oss << "  Worker processes: " << config.getWorkerProcesses() << "\n";
//     oss << "  Worker connections: " << config.getWorkerConnections() <<
//     "\n"; oss << "  Servers: " << config.getServerConfigs().size() << "\n";
//
//     const std::vector<const
//     domain::configuration::entities::ServerConfig*>&
//         servers = configProvider->getAllServers();
//     for (size_t i = 0; i < servers.size(); ++i) {
//       const domain::configuration::entities::ServerConfig* server =
//           servers[i];
//       oss << "\n  Server " << (i + 1) << ":\n";
//       if (!server->getServerNames().empty()) {
//         oss << "    Server names: ";
//         const domain::configuration::entities::ServerConfig::ServerNames&
//             names = server->getServerNames();
//         for (size_t j = 0; j < names.size(); ++j) {
//           if (j > 0) oss << ", ";
//           oss << names[j];
//         }
//         oss << "\n";
//       }
//       oss << "    Locations: " << server->getLocations().size() << "\n";
//     }
//
//     this->m_view.getLogger().info(oss.str());
//   }
// } catch (const std::exception& exception) {
//   this->m_view.displayError(exception.what());
//   return false;
// }
// return true;

bool CliController::parseArguments(int argc, char** argv) {
  if (argc > K_MAX_SIZE_ARGS) {
    this->m_view.displayUsage(std::string(argv[K_NAME_PROGRAM]));
    return false;
  }

  return true;
}

bool CliController::loadConfiguration(const std::string& configPath) {
  try {
    m_view.getLogger().info("Loading configuration from: " + configPath);

    delete m_configProvider;
    m_configProvider = new infrastructure::config::adapters::ConfigProvider(
        m_view.getLogger());

    m_configProvider->load(configPath);

    if (!m_configProvider->isValid()) {
      m_view.displayError("Configuration validation failed");
      return false;
    }

    m_view.getLogger().info("Configuration loaded and validated successfully");
    return true;

  } catch (const std::exception& exception) {
    m_view.displayError(std::string("Configuration error: ") +
                        exception.what());
    return false;
  }
}

void CliController::displayConfigurationSummary(
    const application::ports::IConfigProvider& configProvider) const {
  try {
    const domain::configuration::entities::HttpConfig& config =
        configProvider.getConfiguration();

    std::ostringstream oss;
    oss << "\n=== Configuration Summary ===\n";
    oss << "Worker processes: " << config.getWorkerProcesses() << "\n";
    oss << "Worker connections: " << config.getWorkerConnections() << "\n";
    oss << "Keepalive timeout: " << config.getKeepaliveTimeout() << "s\n";
    oss << "Send timeout: " << config.getSendTimeout() << "s\n";
    oss << "Total servers: " << config.getServerConfigs().size() << "\n";

    const std::vector<const domain::configuration::entities::ServerConfig*>&
        servers = configProvider.getAllServers();

    for (size_t i = 0; i < servers.size(); ++i) {
      const domain::configuration::entities::ServerConfig* server = servers[i];

      oss << "\n--- Server " << (i + 1);
      if (server->isDefaultServer()) {
        oss << " (default)";
      }
      oss << " ---\n";

      const domain::configuration::entities::ServerConfig::ListenDirectives&
          directives = server->getListenDirectives();

      oss << "  Listen: ";
      for (size_t j = 0; j < directives.size(); ++j) {
        if (j > 0) oss << ", ";
        oss << directives[j].getHost().getValue() << ":"
            << directives[j].getPort().getValue();
      }
      oss << "\n";

      if (!server->getServerNames().empty()) {
        oss << "  Server names: ";
        const domain::configuration::entities::ServerConfig::ServerNames&
            names = server->getServerNames();
        for (size_t j = 0; j < names.size(); ++j) {
          if (j > 0) oss << ", ";
          oss << names[j];
        }
        oss << "\n";
      }

      oss << "  Root: " << server->getRoot().getFilename() << "\n";
      oss << "  Locations: " << server->getLocations().size() << "\n";
      oss << "  Max body size: " << server->getClientMaxBodySize().getBytes()
          << " bytes\n";
    }

    oss << "\n=============================\n";
    m_view.getLogger().info(oss.str());

  } catch (const std::exception& ex) {
    std::ostringstream oss;
    oss << "Failed to display configuration summary: " << ex.what();
    m_view.getLogger().warn(oss.str());
  }
}

bool CliController::startServer() {
  if (m_configProvider == NULL) {
    m_view.displayError("Configuration not loaded");
    return false;
  }

  try {
    shared::utils::SignalHandler::initialize();

    m_view.getLogger().info("Initializing socket orchestrator");

    delete m_orchestrator;
    m_orchestrator = new infrastructure::network::adapters::SocketOrchestrator(
        m_view.getLogger(), *m_configProvider);

    m_orchestrator->initialize();

    std::ostringstream oss;
    oss << "Server initialized with " << m_orchestrator->getServerSocketCount()
        << " listen socket(s)";
    m_view.getLogger().info(oss.str());

    m_view.getLogger().info("Webserv is ready to accept connections");
    return true;

  } catch (const std::exception& ex) {
    std::ostringstream oss;
    oss << "Server initialization failed: " << ex.what();
    m_view.displayError(oss.str());
    return false;
  }
}

void CliController::runServerLoop() {
  if (m_orchestrator == NULL) {
    m_view.displayError("Server not initialized");
    return;
  }

  try {
    m_view.getLogger().info("Starting event loop");

    while (m_orchestrator->isRunning() &&
           !shared::utils::SignalHandler::isShutdownRequested()) {
      m_orchestrator->run();
    }

    if (shared::utils::SignalHandler::isShutdownRequested()) {
      m_view.getLogger().info("Shutdown signal received");
    }

  } catch (const std::exception& ex) {
    std::ostringstream oss;
    oss << "Server loop failed: " << ex.what();
    m_view.displayError(oss.str());
  }
}

void CliController::stopServer() {
  if (m_orchestrator == NULL) {
    shared::utils::SignalHandler::cleanup();
    return;
  }

  try {
    m_view.getLogger().info("Shutting down server");

    const size_t activeConnections = m_orchestrator->getActiveConnectionCount();
    m_orchestrator->shutdown();

    std::ostringstream oss;
    oss << "Closed " << activeConnections << " active connection(s)";
    m_view.getLogger().info(oss.str());

  } catch (const std::exception& ex) {
    std::ostringstream oss;
    oss << "Error during shutdown: " << ex.what();
    m_view.getLogger().warn(oss.str());
  }
}

}  // namespace cli
}  // namespace presentation
