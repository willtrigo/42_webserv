/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SocketOrchestrator.hpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/03 06:43:24 by dande-je          #+#    #+#             */
/*   Updated: 2026/01/03 08:54:05 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef INFRASTRUCTURE_NETWORK_SOCKETORCHESTRATOR_HPP
#define INFRASTRUCTURE_NETWORK_SOCKETORCHESTRATOR_HPP

#include "application/ports/IConfigProvider.hpp"
#include "application/ports/ILogger.hpp"
#include "application/ports/ISocketOrchestrator.hpp"

#include <map>
#include <vector>

namespace infrastructure {
namespace network {

class TcpSocket;
class EventMultiplexer;
class ConnectionHandler;

class SocketOrchestrator : public application::ports::ISocketOrchestrator {
 public:
  SocketOrchestrator(application::ports::ILogger& logger,
                     application::ports::IConfigProvider& configProvider);
  ~SocketOrchestrator();

  void initialize();
  void run();
  void shutdown();
  bool isRunning() const;
  size_t getActiveConnectionCount() const;

 private:
  SocketOrchestrator(const SocketOrchestrator&);
  SocketOrchestrator& operator=(const SocketOrchestrator&);

  void initializeServerSockets();

  void registerServerSocketsWithMultiplexer();

  void processEventLoopIteration();

  void handleNewConnection(int serverSocketFd);

  void handleClientEvent(int clientSocketFd);

  void closeConnection(int clientSocketFd);

  const domain::configuration::entities::ServerConfig* resolveServerConfig(
      int clientSocketFd) const;

  application::ports::ILogger& m_logger;
  application::ports::IConfigProvider& m_configProvider;

  std::vector<TcpSocket*> m_serverSockets;
  EventMultiplexer* m_multiplexer;
  std::map<int, ConnectionHandler*> m_connectionHandlers;

  volatile bool m_isRunning;
  volatile bool m_shutdownRequested;
};

}  // namespace network
}  // namespace infrastructure

#endif  // INFRASTRUCTURE_NETWORK_SOCKETORCHESTRATOR_HPP
