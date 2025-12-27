/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MockServer.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: umeneses <umeneses@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/27 11:42:16 by umeneses          #+#    #+#             */
/*   Updated: 2025/12/27 14:05:55 by umeneses         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MOCK_SERVER_HPP
#define MOCK_SERVER_HPP

#include <string>
#include <vector>

namespace mocks {

class MockServer {
 public:
  MockServer();
  ~MockServer();

  // Server Control - Simulate server lifecycle
  void start(int port);
  void stop();
  bool isRunning() const;

  // Port Management - Track which ports the server listens on
  void addPort(int port);
  std::vector<int> getListeningPorts() const;
  bool isListeningOn(int port) const;

  // Request Handling - Simulate receiving/processing requests
  void handleRequest(const std::string& request);
  std::vector<std::string> getReceivedRequests() const;
  std::size_t getRequestCount() const;
  void clearRequests();

  // Error Simulation - Test error handling
  void setError(const std::string& error);
  std::string getLastError() const;
  bool hasError() const;
  void clearError();

  // Connection Tracking - Simulate client connections
  void addClient(int clientId);
  void removeClient(int clientId);
  std::size_t getClientCount() const;

  // Test Helpers - Reset state between tests
  void reset();

 private:
  bool m_isRunning;
  std::vector<int> m_ports;
  std::vector<std::string> m_receivedRequests;
  std::vector<int> m_connectedClients;
  std::string m_lastError;
};

}  // namespace mocks

#endif  // MOCK_SERVER_HPP

