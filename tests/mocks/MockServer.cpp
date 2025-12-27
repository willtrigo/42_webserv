/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MockServer.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: umeneses <umeneses@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/27 11:40:20 by umeneses          #+#    #+#             */
/*   Updated: 2025/12/27 14:05:45 by umeneses         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "MockServer.hpp"

namespace mocks {

MockServer::MockServer() : m_isRunning(false), m_lastError("") {}

MockServer::~MockServer() {}

// ============================================================================
// Server Control
// ============================================================================

void MockServer::start(int port) {
  m_isRunning = true;
  m_ports.push_back(port);
}

void MockServer::stop() {
  m_isRunning = false;
}

bool MockServer::isRunning() const {
  return m_isRunning;
}

// ============================================================================
// Port Management
// ============================================================================

void MockServer::addPort(int port) {
  m_ports.push_back(port);
}

std::vector<int> MockServer::getListeningPorts() const {
  return m_ports;
}

bool MockServer::isListeningOn(int port) const {
  for (std::size_t i = 0; i < m_ports.size(); ++i) {
    if (m_ports[i] == port) {
      return true;
    }
  }
  return false;
}

// ============================================================================
// Request Handling
// ============================================================================

void MockServer::handleRequest(const std::string& request) {
  m_receivedRequests.push_back(request);
}

std::vector<std::string> MockServer::getReceivedRequests() const {
  return m_receivedRequests;
}

std::size_t MockServer::getRequestCount() const {
  return m_receivedRequests.size();
}

void MockServer::clearRequests() {
  m_receivedRequests.clear();
}

// ============================================================================
// Error Simulation
// ============================================================================

void MockServer::setError(const std::string& error) {
  m_lastError = error;
}

std::string MockServer::getLastError() const {
  return m_lastError;
}

bool MockServer::hasError() const {
  return !m_lastError.empty();
}

void MockServer::clearError() {
  m_lastError = "";
}

// ============================================================================
// Connection Tracking
// ============================================================================

void MockServer::addClient(int clientId) {
  m_connectedClients.push_back(clientId);
}

void MockServer::removeClient(int clientId) {
  for (std::size_t i = 0; i < m_connectedClients.size(); ++i) {
    if (m_connectedClients[i] == clientId) {
      m_connectedClients.erase(m_connectedClients.begin() + i);
      break;
    }
  }
}

std::size_t MockServer::getClientCount() const {
  return m_connectedClients.size();
}

// ============================================================================
// Test Helpers
// ============================================================================

void MockServer::reset() {
  m_isRunning = false;
  m_ports.clear();
  m_receivedRequests.clear();
  m_connectedClients.clear();
  m_lastError = "";
}

}  // namespace mocks
