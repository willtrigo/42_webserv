/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SocketOrchestrator.hpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/03 06:43:24 by dande-je          #+#    #+#             */
/*   Updated: 2026/01/06 05:27:14 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SOCKET_ORCHESTRATOR_HPP
#define SOCKET_ORCHESTRATOR_HPP

#include "application/ports/IConfigProvider.hpp"
#include "application/ports/ILogger.hpp"
#include "application/ports/ISocketOrchestrator.hpp"
#include "domain/configuration/entities/ServerConfig.hpp"
#include "infrastructure/network/primitives/SocketEvent.hpp"

#include <ctime>
#include <map>
#include <set>
#include <string>
#include <vector>

namespace infrastructure {
namespace network {
namespace adapters {

class TcpSocket;
class EventMultiplexer;
class ConnectionHandler;

class SocketOrchestrator : public application::ports::ISocketOrchestrator {
 public:
  static const int K_EVENT_LOOP_TIMEOUT_MS = 1000;
  static const time_t K_CONNECTION_SWEEP_INTERVAL = 30;
  static const int K_DEFAULT_LISTEN_BACKLOG = 128;
  static const size_t K_MAX_CONNECTIONS = 10000;

  SocketOrchestrator(application::ports::ILogger& logger,
                     application::ports::IConfigProvider& configProvider);
  virtual ~SocketOrchestrator();

  virtual void initialize();
  virtual void run();
  virtual void shutdown();
  virtual bool isRunning() const;

  virtual size_t getActiveConnectionCount() const;
  virtual size_t getServerSocketCount() const;

 private:
  struct ListenSocket {
    TcpSocket* socket;
    std::string bindAddress;
    unsigned int bindPort;
    std::vector<const domain::configuration::entities::ServerConfig*>
        serverConfigs;

    ListenSocket();
    ListenSocket(TcpSocket* sock, const std::string& address,
                 unsigned int port);
    ~ListenSocket();

    bool matchesBinding(const std::string& address, unsigned int port) const;
    void addServerConfig(
        const domain::configuration::entities::ServerConfig* config);
    const domain::configuration::entities::ServerConfig* resolveDefaultServer()
        const;

   private:
    ListenSocket(const ListenSocket&);
    ListenSocket& operator=(const ListenSocket&);
  };

  typedef std::map<int, ListenSocket*> ListenSocketMap;
  typedef std::map<int, ConnectionHandler*> ConnectionHandlerMap;
  typedef std::set<std::string> UniqueBindingSet;

  SocketOrchestrator(const SocketOrchestrator&);
  SocketOrchestrator& operator=(const SocketOrchestrator&);

  void initializeServerSockets();
  void registerServerSocketsWithMultiplexer();
  void collectUniqueBindings(UniqueBindingSet& uniqueBindings) const;
  void createListenSocketsFromBindings(const UniqueBindingSet& bindings);
  void associateServerConfigsWithListenSockets();

  void processEventLoopIteration();
  void processReadyEvents(
      const std::vector<primitives::SocketEvent>& readyEvents);
  void performConnectionSweep(time_t currentTime);

  void handleNewConnection(int serverSocketFd);
  void handleClientEvent(int clientSocketFd);
  void closeConnection(int clientSocketFd);

  bool canAcceptNewConnection() const;
  void registerClientSocket(int clientFd, ConnectionHandler* handler);
  void deregisterClientSocket(int clientFd);

  const domain::configuration::entities::ServerConfig* resolveServerConfig(
      const ListenSocket* listenSocket) const;

  ListenSocket* findListenSocket(int serverSocketFd) const;
  bool isServerSocket(int fileDescriptor) const;

  void validateInitializationState() const;
  void cleanupResources();
  void cleanupServerSockets();
  void cleanupConnectionHandlers();
  void cleanupMultiplexer();

  std::string formatOrchestrationState() const;

  application::ports::ILogger& m_logger;
  application::ports::IConfigProvider& m_configProvider;

  ListenSocketMap m_listenSockets;
  EventMultiplexer* m_multiplexer;
  ConnectionHandlerMap m_connectionHandlers;

  volatile bool m_isRunning;
  volatile bool m_shutdownRequested;
  time_t m_lastConnectionSweep;
};

}  // namespace adapters
}  // namespace network
}  // namespace infrastructure

#endif  // SOCKET_ORCHESTRATOR_HPP
