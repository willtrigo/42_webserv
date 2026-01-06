/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SocketOrchestrator.cpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/03 06:49:26 by dande-je          #+#    #+#             */
/*   Updated: 2026/01/06 05:29:39 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "infrastructure/network/adapters/SocketOrchestrator.hpp"
#include "infrastructure/network/adapters/ConnectionHandler.hpp"
#include "infrastructure/network/adapters/EventMultiplexer.hpp"
#include "infrastructure/network/adapters/TcpSocket.hpp"
#include "infrastructure/network/primitives/SocketEvent.hpp"
#include "domain/configuration/value_objects/ListenDirective.hpp"

#include <cerrno>
#include <cstring>
#include <sstream>
#include <stdexcept>

namespace infrastructure {
namespace network {
namespace adapters {

// ============================================================================
// ListenSocket Implementation
// ============================================================================

SocketOrchestrator::ListenSocket::ListenSocket()
    : socket(NULL), bindPort(0) {}

SocketOrchestrator::ListenSocket::ListenSocket(TcpSocket* sock,
                                                const std::string& address,
                                                unsigned int port)
    : socket(sock), bindAddress(address), bindPort(port) {}

SocketOrchestrator::ListenSocket::~ListenSocket() {
  delete socket;
  socket = NULL;
  serverConfigs.clear();
}

bool SocketOrchestrator::ListenSocket::matchesBinding(
    const std::string& address, unsigned int port) const {
  return bindAddress == address && bindPort == port;
}

void SocketOrchestrator::ListenSocket::addServerConfig(
    const domain::configuration::entities::ServerConfig* config) {
  if (config == NULL) {
    return;
  }

  for (size_t i = 0; i < serverConfigs.size(); ++i) {
    if (serverConfigs[i] == config) {
      return;
    }
  }

  serverConfigs.push_back(config);
}

const domain::configuration::entities::ServerConfig*
SocketOrchestrator::ListenSocket::resolveDefaultServer() const {
  for (size_t i = 0; i < serverConfigs.size(); ++i) {
    if (serverConfigs[i]->isDefaultServer()) {
      return serverConfigs[i];
    }
  }

  return serverConfigs.empty() ? NULL : serverConfigs[0];
}

// ============================================================================
// SocketOrchestrator Implementation
// ============================================================================

SocketOrchestrator::SocketOrchestrator(
    application::ports::ILogger& logger,
    application::ports::IConfigProvider& configProvider)
    : m_logger(logger),
      m_configProvider(configProvider),
      m_multiplexer(NULL),
      m_isRunning(false),
      m_shutdownRequested(false),
      m_lastConnectionSweep(0) {
  if (!configProvider.isValid()) {
    throw std::invalid_argument(
        "SocketOrchestrator requires valid ConfigProvider");
  }
}

SocketOrchestrator::~SocketOrchestrator() {
  if (m_isRunning) {
    shutdown();
  }
  cleanupResources();
}

// ============================================================================
// Public Interface Implementation
// ============================================================================

void SocketOrchestrator::initialize() {
  if (m_isRunning) {
    m_logger.warn(
        "SocketOrchestrator::initialize() invoked while already initialized; "
        "ignoring");
    return;
  }

  try {
    m_logger.info("Initializing SocketOrchestrator");

    initializeServerSockets();
    registerServerSocketsWithMultiplexer();

    m_isRunning = true;
    m_shutdownRequested = false;
    m_lastConnectionSweep = std::time(NULL);

    std::ostringstream oss;
    oss << "SocketOrchestrator initialized with " << m_listenSockets.size()
        << " listen socket(s)";
    m_logger.info(oss.str());

  } catch (const std::exception& ex) {
    m_logger.error(std::string("Initialization failed: ") + ex.what());
    cleanupResources();
    m_isRunning = false;
    throw;
  }
}

void SocketOrchestrator::run() {
  validateInitializationState();

  m_logger.info("Starting event loop");

  while (m_isRunning && !m_shutdownRequested) {
    try {
      processEventLoopIteration();
    } catch (const std::exception& ex) {
      std::ostringstream oss;
      oss << "Event loop iteration failed: " << ex.what();
      m_logger.error(oss.str());
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

  cleanupConnectionHandlers();

  m_isRunning = false;
  m_logger.info("Shutdown complete");
}

bool SocketOrchestrator::isRunning() const { return m_isRunning; }

size_t SocketOrchestrator::getActiveConnectionCount() const {
  return m_connectionHandlers.size();
}

size_t SocketOrchestrator::getServerSocketCount() const {
  return m_listenSockets.size();
}

// ============================================================================
// Initialization Methods
// ============================================================================

void SocketOrchestrator::initializeServerSockets() {
  const std::vector<const domain::configuration::entities::ServerConfig*>&
      serverConfigs = m_configProvider.getAllServers();

  if (serverConfigs.empty()) {
    throw std::runtime_error("No server configurations available");
  }

  m_logger.info("Creating server sockets from configuration");

  UniqueBindingSet uniqueBindings;
  collectUniqueBindings(uniqueBindings);
  createListenSocketsFromBindings(uniqueBindings);
  associateServerConfigsWithListenSockets();

  if (m_listenSockets.empty()) {
    throw std::runtime_error("Failed to create any server sockets");
  }

  std::ostringstream oss;
  oss << "Created " << m_listenSockets.size() << " listen socket(s)";
  m_logger.info(oss.str());
}

void SocketOrchestrator::registerServerSocketsWithMultiplexer() {
  m_multiplexer = new EventMultiplexer(m_logger);

  for (ListenSocketMap::const_iterator it = m_listenSockets.begin();
       it != m_listenSockets.end(); ++it) {
    const int fd = it->first;
    m_multiplexer->registerSocket(fd, primitives::SocketEvent::EVENT_READ);

    std::ostringstream oss;
    oss << "Registered listen socket fd=" << fd << " ("
        << it->second->bindAddress << ":" << it->second->bindPort << ")";
    m_logger.debug(oss.str());
  }

  m_logger.info("Server sockets registered with event multiplexer");
}

void SocketOrchestrator::collectUniqueBindings(
    UniqueBindingSet& uniqueBindings) const {
  const std::vector<const domain::configuration::entities::ServerConfig*>&
      serverConfigs = m_configProvider.getAllServers();

  for (size_t i = 0; i < serverConfigs.size(); ++i) {
    const domain::configuration::entities::ServerConfig::ListenDirectives&
        listenDirectives = serverConfigs[i]->getListenDirectives();

    for (size_t j = 0; j < listenDirectives.size(); ++j) {
      const domain::configuration::entities::ListenDirective& directive =
          listenDirectives[j];

      std::ostringstream bindKey;
      bindKey << directive.getHost().getValue() << ":"
              << directive.getPort().getValue();

      uniqueBindings.insert(bindKey.str());
    }
  }
}

void SocketOrchestrator::createListenSocketsFromBindings(
    const UniqueBindingSet& bindings) {
  for (UniqueBindingSet::const_iterator it = bindings.begin();
       it != bindings.end(); ++it) {
    const std::string& binding = *it;
    const size_t colonPos = binding.find(':');

    if (colonPos == std::string::npos) {
      std::ostringstream oss;
      oss << "Invalid binding format: " << binding;
      throw std::runtime_error(oss.str());
    }

    const std::string hostStr = binding.substr(0, colonPos);
    const std::string portStr = binding.substr(colonPos + 1);

    const domain::http::value_objects::Host host(hostStr);
    const domain::http::value_objects::Port port(portStr);

    TcpSocket* socket = new TcpSocket(host, port, m_logger);

    try {
      socket->bind();
      socket->listen(K_DEFAULT_LISTEN_BACKLOG);
      socket->setNonBlocking(true);

      ListenSocket* listenSocket =
          new ListenSocket(socket, hostStr, port.getValue());
      m_listenSockets[socket->getFd()] = listenSocket;

      std::ostringstream oss;
      oss << "Bound listen socket to " << binding
          << " (fd=" << socket->getFd() << ")";
      m_logger.info(oss.str());

    } catch (const std::exception& ex) {
      delete socket;
      throw;
    }
  }
}

void SocketOrchestrator::associateServerConfigsWithListenSockets() {
  const std::vector<const domain::configuration::entities::ServerConfig*>&
      serverConfigs = m_configProvider.getAllServers();

  for (size_t i = 0; i < serverConfigs.size(); ++i) {
    const domain::configuration::entities::ServerConfig* serverConfig =
        serverConfigs[i];
    const domain::configuration::entities::ServerConfig::ListenDirectives&
        listenDirectives = serverConfig->getListenDirectives();

    for (size_t j = 0; j < listenDirectives.size(); ++j) {
      const domain::configuration::entities::ListenDirective& directive =
          listenDirectives[j];

      const std::string hostStr = directive.getHost().getValue();
      const unsigned int portVal = directive.getPort().getValue();

      for (ListenSocketMap::iterator lsIt = m_listenSockets.begin();
           lsIt != m_listenSockets.end(); ++lsIt) {
        if (lsIt->second->matchesBinding(hostStr, portVal)) {
          lsIt->second->addServerConfig(serverConfig);
        }
      }
    }
  }
}

// ============================================================================
// Event Loop Methods
// ============================================================================

void SocketOrchestrator::processEventLoopIteration() {
  const std::vector<primitives::SocketEvent> readyEvents =
      m_multiplexer->wait(K_EVENT_LOOP_TIMEOUT_MS);

  processReadyEvents(readyEvents);

  const time_t currentTime = std::time(NULL);
  if (currentTime - m_lastConnectionSweep >= K_CONNECTION_SWEEP_INTERVAL) {
    performConnectionSweep(currentTime);
    m_lastConnectionSweep = currentTime;
  }
}

void SocketOrchestrator::processReadyEvents(
    const std::vector<primitives::SocketEvent>& readyEvents) {
  for (size_t i = 0; i < readyEvents.size(); ++i) {
    const primitives::SocketEvent& event = readyEvents[i];
    const int fd = event.getFd();

    if (!event.isValid()) {
      continue;
    }

    if (event.hasError() || event.hasHangup()) {
      if (isServerSocket(fd)) {
        std::ostringstream oss;
        oss << "Error/Hangup on server socket fd=" << fd;
        m_logger.error(oss.str());
      } else {
        closeConnection(fd);
      }
      continue;
    }

    if (isServerSocket(fd)) {
      if (event.isReadable()) {
        handleNewConnection(fd);
      }
    } else {
      handleClientEvent(fd);
    }
  }
}

void SocketOrchestrator::performConnectionSweep(time_t currentTime) {
  std::vector<int> timedOutConnections;
  timedOutConnections.reserve(m_connectionHandlers.size() / 10);

  for (ConnectionHandlerMap::iterator it = m_connectionHandlers.begin();
       it != m_connectionHandlers.end(); ++it) {
    if (it->second->isTimedOut(currentTime)) {
      timedOutConnections.push_back(it->first);
    }
  }

  for (size_t i = 0; i < timedOutConnections.size(); ++i) {
    std::ostringstream oss;
    oss << "Closing timed-out connection fd=" << timedOutConnections[i];
    m_logger.debug(oss.str());
    closeConnection(timedOutConnections[i]);
  }

  if (!timedOutConnections.empty()) {
    std::ostringstream oss;
    oss << "Closed " << timedOutConnections.size() << " timed-out connection(s)";
    m_logger.info(oss.str());
  }
}

// ============================================================================
// Connection Management Methods
// ============================================================================

void SocketOrchestrator::handleNewConnection(int serverSocketFd) {
  ListenSocket* listenSocket = findListenSocket(serverSocketFd);
  if (listenSocket == NULL || listenSocket->socket == NULL) {
    std::ostringstream oss;
    oss << "Invalid listen socket for fd=" << serverSocketFd;
    m_logger.error(oss.str());
    return;
  }

  if (!canAcceptNewConnection()) {
    m_logger.warn(
        "Connection limit reached; rejecting new connection attempt");
    return;
  }

  try {
    TcpSocket* clientSocket = listenSocket->socket->accept();

    if (clientSocket == NULL) {
      return;
    }

    clientSocket->setNonBlocking(true);
    const int clientFd = clientSocket->getFd();

    const domain::configuration::entities::ServerConfig* serverConfig =
        resolveServerConfig(listenSocket);

    if (serverConfig == NULL) {
      std::ostringstream oss;
      oss << "No server configuration resolved for connection fd=" << clientFd;
      m_logger.error(oss.str());
      delete clientSocket;
      return;
    }

    ConnectionHandler* handler = new ConnectionHandler(
        clientSocket, serverConfig, m_logger, m_configProvider);

    registerClientSocket(clientFd, handler);

    std::ostringstream oss;
    oss << "Accepted connection from " << clientSocket->getRemoteAddress()
        << " (fd=" << clientFd << ")";
    m_logger.info(oss.str());

  } catch (const std::exception& ex) {
    std::ostringstream oss;
    oss << "Failed to accept connection: " << ex.what();
    m_logger.error(oss.str());
  }
}

void SocketOrchestrator::handleClientEvent(int clientSocketFd) {
  ConnectionHandlerMap::iterator it =
      m_connectionHandlers.find(clientSocketFd);

  if (it == m_connectionHandlers.end()) {
    std::ostringstream oss;
    oss << "Received event for unknown client socket fd=" << clientSocketFd;
    m_logger.warn(oss.str());
    closeConnection(clientSocketFd);
    return;
  }

  ConnectionHandler* handler = it->second;

  try {
    handler->processEvent();

    if (handler->shouldClose()) {
      closeConnection(clientSocketFd);
    }

  } catch (const std::exception& ex) {
    std::ostringstream oss;
    oss << "Client event processing failed for fd=" << clientSocketFd << ": "
        << ex.what();
    m_logger.error(oss.str());
    closeConnection(clientSocketFd);
  }
}

void SocketOrchestrator::closeConnection(int clientSocketFd) {
  ConnectionHandlerMap::iterator it =
      m_connectionHandlers.find(clientSocketFd);

  if (it == m_connectionHandlers.end()) {
    return;
  }

  deregisterClientSocket(clientSocketFd);

  delete it->second;
  m_connectionHandlers.erase(it);

  std::ostringstream oss;
  oss << "Closed connection fd=" << clientSocketFd << " (active="
      << m_connectionHandlers.size() << ")";
  m_logger.debug(oss.str());
}

bool SocketOrchestrator::canAcceptNewConnection() const {
  return m_connectionHandlers.size() < K_MAX_CONNECTIONS;
}

void SocketOrchestrator::registerClientSocket(int clientFd,
                                               ConnectionHandler* handler) {
  m_connectionHandlers[clientFd] = handler;
  m_multiplexer->registerSocket(clientFd, primitives::SocketEvent::EVENT_READ);
}

void SocketOrchestrator::deregisterClientSocket(int clientFd) {
  m_multiplexer->deregisterSocket(clientFd);
}

// ============================================================================
// Server Resolution Methods
// ============================================================================

const domain::configuration::entities::ServerConfig*
SocketOrchestrator::resolveServerConfig(const ListenSocket* listenSocket) const {
  if (listenSocket == NULL) {
    return NULL;
  }

  return listenSocket->resolveDefaultServer();
}

SocketOrchestrator::ListenSocket* SocketOrchestrator::findListenSocket(
    int serverSocketFd) const {
  ListenSocketMap::const_iterator it = m_listenSockets.find(serverSocketFd);
  return (it != m_listenSockets.end()) ? it->second : NULL;
}

bool SocketOrchestrator::isServerSocket(int fileDescriptor) const {
  return m_listenSockets.find(fileDescriptor) != m_listenSockets.end();
}

// ============================================================================
// State Management Methods
// ============================================================================

void SocketOrchestrator::validateInitializationState() const {
  if (!m_isRunning) {
    throw std::logic_error(
        "SocketOrchestrator::run() invoked before initialize()");
  }

  if (m_multiplexer == NULL) {
    throw std::logic_error("EventMultiplexer not initialized");
  }

  if (m_listenSockets.empty()) {
    throw std::logic_error("No listen sockets available");
  }
}

void SocketOrchestrator::cleanupResources() {
  cleanupConnectionHandlers();
  cleanupServerSockets();
  cleanupMultiplexer();
}

void SocketOrchestrator::cleanupServerSockets() {
  for (ListenSocketMap::iterator it = m_listenSockets.begin();
       it != m_listenSockets.end(); ++it) {
    delete it->second;
  }
  m_listenSockets.clear();
}

void SocketOrchestrator::cleanupConnectionHandlers() {
  std::vector<int> clientFds;
  clientFds.reserve(m_connectionHandlers.size());

  for (ConnectionHandlerMap::const_iterator it = m_connectionHandlers.begin();
       it != m_connectionHandlers.end(); ++it) {
    clientFds.push_back(it->first);
  }

  for (size_t i = 0; i < clientFds.size(); ++i) {
    closeConnection(clientFds[i]);
  }

  m_connectionHandlers.clear();
}

void SocketOrchestrator::cleanupMultiplexer() {
  delete m_multiplexer;
  m_multiplexer = NULL;
}

std::string SocketOrchestrator::formatOrchestrationState() const {
  std::ostringstream oss;
  oss << "SocketOrchestrator[running=" << (m_isRunning ? "yes" : "no")
      << ", servers=" << m_listenSockets.size()
      << ", connections=" << m_connectionHandlers.size() << "]";
  return oss.str();
}

}  // namespace adapters
}  // namespace network
}  // namespace infrastructure
