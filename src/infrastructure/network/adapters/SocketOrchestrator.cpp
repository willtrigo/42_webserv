/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SocketOrchestrator.cpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/03 06:49:26 by dande-je          #+#    #+#             */
/*   Updated: 2026/01/03 10:04:04 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "infrastructure/network/SocketOrchestrator.hpp"

#include <sstream>
#include <stdexcept>

namespace infrastructure {
namespace network {

SocketOrchestrator::SocketOrchestrator(
    application::ports::ILogger& logger,
    application::ports::IConfigProvider& configProvider)
    : m_logger(logger),
      m_configProvider(configProvider),
      m_multiplexer(NULL),
      m_isRunning(false),
      m_shutdownRequested(false) {
  if (!configProvider.isValid()) {
    throw std::invalid_argument(
        "SocketOrchestrator: ConfigProvider must be valid before construction");
  }
}

SocketOrchestrator::~SocketOrchestrator() {
  shutdown();

  // Cleanup connection handlers
  for (std::map<int, ConnectionHandler*>::iterator it =
           m_connectionHandlers.begin();
       it != m_connectionHandlers.end(); ++it) {
    delete it->second;
  }
  m_connectionHandlers.clear();

  // Cleanup server sockets
  for (std::vector<TcpSocket*>::iterator it = m_serverSockets.begin();
       it != m_serverSockets.end(); ++it) {
    delete *it;
  }
  m_serverSockets.clear();

  // Cleanup multiplexer
  delete m_multiplexer;
  m_multiplexer = NULL;
}

void SocketOrchestrator::initialize() {
  if (m_isRunning) {
    m_logger.warn(
        "SocketOrchestrator::initialize() invoked while already running; "
        "ignoring");
    return;
  }

  try {
    m_logger.info("Initializing SocketOrchestrator");

    initializeServerSockets();
    registerServerSocketsWithMultiplexer();

    m_isRunning = true;
    m_shutdownRequested = false;

    std::ostringstream oss;
    oss << "SocketOrchestrator initialized successfully with "
        << m_serverSockets.size() << " server socket(s)";
    m_logger.info(oss.str());

  } catch (const std::exception& ex) {
    m_logger.error(std::string("Initialization failed: ") + ex.what());
    m_isRunning = false;
    throw;
  }
}

void SocketOrchestrator::run() {
  if (!m_isRunning) {
    throw std::logic_error(
        "SocketOrchestrator::run() invoked before initialize()");
  }

  m_logger.info("Starting event loop");

  while (m_isRunning && !m_shutdownRequested) {
    try {
      processEventLoopIteration();
    } catch (const std::exception& ex) {
      m_logger.error(std::string("Event loop iteration failed: ") + ex.what());
      // Continue processing; don't terminate on single iteration failure
    }
  }

  m_logger.info("Event loop terminated");
}

void SocketOrchestrator::shutdown() {
  if (!m_isRunning) {
    return;
  }

  m_logger.info("Initiating graceful shutdown");
  m_shutdownRequested = true;

  // Close all client connections
  std::vector<int> clientFds;
  clientFds.reserve(m_connectionHandlers.size());

  for (std::map<int, ConnectionHandler*>::const_iterator it =
           m_connectionHandlers.begin();
       it != m_connectionHandlers.end(); ++it) {
    clientFds.push_back(it->first);
  }

  for (std::vector<int>::const_iterator it = clientFds.begin();
       it != clientFds.end(); ++it) {
    closeConnection(*it);
  }

  m_isRunning = false;
  m_logger.info("Shutdown complete");
}

bool SocketOrchestrator::isRunning() const { return m_isRunning; }

size_t SocketOrchestrator::getActiveConnectionCount() const {
  return m_connectionHandlers.size();
}

void SocketOrchestrator::initializeServerSockets() {
  const std::vector<const domain::configuration::entities::ServerConfig*>&
      serverConfigs = m_configProvider.getAllServers();

  if (serverConfigs.empty()) {
    throw std::runtime_error(
        "No server configurations available for socket initialization");
  }

  m_logger.info("Creating server sockets from configuration");

  // TODO: Implement TcpSocket creation logic
  // For each unique listen directive across all ServerConfigs:
  //   1. Create TcpSocket instance
  //   2. Bind to address:port
  //   3. Set non-blocking mode
  //   4. Begin listening (backlog from config or default 128)
  //   5. Store in m_serverSockets

  // Placeholder implementation structure:
  /*
  std::set<std::string> uniqueBindings;  // "ip:port" format

  for (size_t i = 0; i < serverConfigs.size(); ++i) {
    const domain::configuration::entities::ServerConfig* config =
        serverConfigs[i];
    const std::vector<domain::configuration::value_objects::ListenDirective>&
        listenDirectives = config->getListenDirectives();

    for (size_t j = 0; j < listenDirectives.size(); ++j) {
      std::ostringstream bindKey;
      bindKey << listenDirectives[j].getHost() << ":"
              << listenDirectives[j].getPort();

      if (uniqueBindings.insert(bindKey.str()).second) {
        TcpSocket* socket = new TcpSocket(
            listenDirectives[j].getHost(),
            listenDirectives[j].getPort(),
            m_logger);

        socket->bind();
        socket->listen(128);  // Standard backlog
        socket->setNonBlocking(true);

        m_serverSockets.push_back(socket);

        m_logger.info("Server socket bound to " + bindKey.str());
      }
    }
  }
  */

  if (m_serverSockets.empty()) {
    throw std::runtime_error(
        "Failed to create any server sockets from configuration");
  }
}

void SocketOrchestrator::registerServerSocketsWithMultiplexer() {
  // TODO: Implement EventMultiplexer creation and registration
  // m_multiplexer = new EventMultiplexer(m_logger);
  //
  // for (std::vector<TcpSocket*>::iterator it = m_serverSockets.begin();
  //      it != m_serverSockets.end(); ++it) {
  //   m_multiplexer->registerSocket((*it)->getFd(), EventMultiplexer::READ);
  // }

  m_logger.info("Server sockets registered with event multiplexer");
}

void SocketOrchestrator::processEventLoopIteration() {
  // TODO: Implement event processing
  // 1. Call m_multiplexer->wait(timeout) to get ready file descriptors
  // 2. For each ready FD:
  //    - If server socket: handleNewConnection(fd)
  //    - If client socket: handleClientEvent(fd)
  // 3. Handle timeouts for idle connections

  // Placeholder structure:
  /*
  std::vector<int> readyFds = m_multiplexer->wait(1000);  // 1s timeout

  for (std::vector<int>::const_iterator it = readyFds.begin();
       it != readyFds.end(); ++it) {
    int fd = *it;

    if (isServerSocket(fd)) {
      handleNewConnection(fd);
    } else {
      handleClientEvent(fd);
    }
  }
  */
}

void SocketOrchestrator::handleNewConnection(int serverSocketFd) {
  // TODO: Implement connection acceptance
  // 1. Accept new connection on server socket
  // 2. Set client socket to non-blocking
  // 3. Resolve ServerConfig for this connection
  // 4. Create ConnectionHandler with resolved config
  // 5. Register client socket with multiplexer
  // 6. Store handler in m_connectionHandlers

  // Placeholder structure:
  /*
  TcpSocket* serverSocket = findServerSocket(serverSocketFd);
  TcpSocket* clientSocket = serverSocket->accept();

  if (clientSocket == NULL) {
    if (errno != EAGAIN && errno != EWOULDBLOCK) {
      m_logger.error("Accept failed: " + std::string(std::strerror(errno)));
    }
    return;
  }

  clientSocket->setNonBlocking(true);
  int clientFd = clientSocket->getFd();

  const domain::configuration::entities::ServerConfig* config =
      resolveServerConfig(clientFd);

  ConnectionHandler* handler = new ConnectionHandler(
      clientSocket, config, m_logger, m_configProvider);

  m_connectionHandlers[clientFd] = handler;
  m_multiplexer->registerSocket(clientFd, EventMultiplexer::READ);

  std::ostringstream oss;
  oss << "Accepted new connection: FD=" << clientFd;
  m_logger.debug(oss.str());
  */
}

void SocketOrchestrator::handleClientEvent(int clientSocketFd) {
  std::map<int, ConnectionHandler*>::iterator it =
      m_connectionHandlers.find(clientSocketFd);

  if (it == m_connectionHandlers.end()) {
    m_logger.warning("Received event for unknown client socket; closing");
    closeConnection(clientSocketFd);
    return;
  }

  ConnectionHandler* handler = it->second;

  // TODO: Delegate to ConnectionHandler
  // try {
  //   handler->processEvent();
  //
  //   if (handler->shouldClose()) {
  //     closeConnection(clientSocketFd);
  //   }
  // } catch (const std::exception& ex) {
  //   m_logger.error("Client event processing failed: " +
  //   std::string(ex.what())); closeConnection(clientSocketFd);
  // }
}

void SocketOrchestrator::closeConnection(int clientSocketFd) {
  std::map<int, ConnectionHandler*>::iterator it =
      m_connectionHandlers.find(clientSocketFd);

  if (it == m_connectionHandlers.end()) {
    return;
  }

  // TODO: Cleanup sequence
  // 1. Deregister from multiplexer
  // 2. Delete handler (which should close socket)
  // 3. Remove from map

  // m_multiplexer->deregisterSocket(clientSocketFd);
  delete it->second;
  m_connectionHandlers.erase(it);

  std::ostringstream oss;
  oss << "Closed connection: FD=" << clientSocketFd;
  m_logger.debug(oss.str());
}

const domain::configuration::entities::ServerConfig*
SocketOrchestrator::resolveServerConfig(int clientSocketFd) const {
  // TODO: Implement virtual host resolution
  // 1. Get local address/port from socket
  // 2. Match against ServerConfig listen directives
  // 3. If multiple matches, defer to Host header (handled by
  // ConnectionHandler)
  // 4. Return first matching config as default

  // Placeholder: return first server config
  const std::vector<const domain::configuration::entities::ServerConfig*>&
      configs = m_configProvider.getAllServers();

  if (configs.empty()) {
    throw std::runtime_error(
        "No server configurations available for connection resolution");
  }

  return configs[0];
}

}  // namespace network
}  // namespace infrastructure
