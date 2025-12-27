/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_MockServer.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: umeneses <umeneses@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/27 12:42:28 by umeneses          #+#    #+#             */
/*   Updated: 2025/12/27 14:56:48 by umeneses         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */



#include "mocks/MockServer.hpp"
#include <gtest/gtest.h>

using namespace mocks;

class MockServerTest : public ::testing::Test {
 protected:
  virtual void SetUp() {}
  virtual void TearDown() {}
};

// ============================================================================
// Server Control Tests
// ============================================================================

TEST_F(MockServerTest, DefaultConstructorShouldCreateStoppedServer) {
  MockServer server;
  
  EXPECT_FALSE(server.isRunning());
}

TEST_F(MockServerTest, StartShouldSetServerToRunning) {
  MockServer server;
  
  server.start(8080);
  
  EXPECT_TRUE(server.isRunning());
}

TEST_F(MockServerTest, StopShouldSetServerToStopped) {
  MockServer server;
  server.start(8080);
  
  server.stop();
  
  EXPECT_FALSE(server.isRunning());
}

// ============================================================================
// Port Management Tests
// ============================================================================

TEST_F(MockServerTest, StartShouldAddPortToListeningPorts) {
  MockServer server;
  
  server.start(8080);
  
  EXPECT_TRUE(server.isListeningOn(8080));
}

TEST_F(MockServerTest, AddPortShouldAddToListeningPorts) {
  MockServer server;
  
  server.addPort(8080);
  server.addPort(8443);
  
  EXPECT_TRUE(server.isListeningOn(8080));
  EXPECT_TRUE(server.isListeningOn(8443));
}

TEST_F(MockServerTest, GetListeningPortsShouldReturnAllPorts) {
  MockServer server;
  server.addPort(8080);
  server.addPort(8443);
  
  std::vector<int> ports = server.getListeningPorts();
  
  EXPECT_EQ(2, ports.size());
  EXPECT_EQ(8080, ports[0]);
  EXPECT_EQ(8443, ports[1]);
}

TEST_F(MockServerTest, IsListeningOnShouldReturnFalseForNonExistentPort) {
  MockServer server;
  server.addPort(8080);
  
  EXPECT_FALSE(server.isListeningOn(9999));
}

// ============================================================================
// Request Handling Tests
// ============================================================================

TEST_F(MockServerTest, HandleRequestShouldStoreRequest) {
  MockServer server;
  
  server.handleRequest("GET / HTTP/1.1");
  
  EXPECT_EQ(1, server.getRequestCount());
}

TEST_F(MockServerTest, GetReceivedRequestsShouldReturnAllRequests) {
  MockServer server;
  server.handleRequest("GET / HTTP/1.1");
  server.handleRequest("POST /data HTTP/1.1");
  
  std::vector<std::string> requests = server.getReceivedRequests();
  
  EXPECT_EQ(2, requests.size());
  EXPECT_EQ("GET / HTTP/1.1", requests[0]);
  EXPECT_EQ("POST /data HTTP/1.1", requests[1]);
}

TEST_F(MockServerTest, ClearRequestsShouldRemoveAllRequests) {
  MockServer server;
  server.handleRequest("GET / HTTP/1.1");
  server.handleRequest("POST /data HTTP/1.1");
  
  server.clearRequests();
  
  EXPECT_EQ(0, server.getRequestCount());
}

// ============================================================================
// Error Simulation Tests
// ============================================================================

TEST_F(MockServerTest, DefaultConstructorShouldHaveNoError) {
  MockServer server;
  
  EXPECT_FALSE(server.hasError());
  EXPECT_EQ("", server.getLastError());
}

TEST_F(MockServerTest, SetErrorShouldStoreErrorMessage) {
  MockServer server;
  
  server.setError("Connection timeout");
  
  EXPECT_TRUE(server.hasError());
  EXPECT_EQ("Connection timeout", server.getLastError());
}

TEST_F(MockServerTest, ClearErrorShouldRemoveErrorMessage) {
  MockServer server;
  server.setError("Connection timeout");
  
  server.clearError();
  
  EXPECT_FALSE(server.hasError());
  EXPECT_EQ("", server.getLastError());
}

// ============================================================================
// Connection Tracking Tests
// ============================================================================

TEST_F(MockServerTest, AddClientShouldIncrementClientCount) {
  MockServer server;
  
  server.addClient(1);
  server.addClient(2);
  
  EXPECT_EQ(2, server.getClientCount());
}

TEST_F(MockServerTest, RemoveClientShouldDecrementClientCount) {
  MockServer server;
  server.addClient(1);
  server.addClient(2);
  
  server.removeClient(1);
  
  EXPECT_EQ(1, server.getClientCount());
}

TEST_F(MockServerTest, GetClientCountShouldReturnCurrentClientCount) {
  MockServer server;
  server.addClient(1);
  server.addClient(2);
  server.removeClient(1);
  
  EXPECT_EQ(1, server.getClientCount());
}
