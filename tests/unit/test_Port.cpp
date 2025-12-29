/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_Port.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: umeneses <umeneses@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/24 11:19:59 by umeneses          #+#    #+#             */
/*   Updated: 2025/12/29 12:50:12 by umeneses         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "domain/http/value_objects/Port.hpp"
#include "domain/http/exceptions/PortException.hpp"
#include <gtest/gtest.h>

using namespace domain::http::value_objects;

class PortTest : public ::testing::Test {
 protected:
  virtual void SetUp() {}
  virtual void TearDown() {}
};

TEST_F(PortTest, DefaultConstructor) {
  Port port;
  EXPECT_EQ(80, port.getValue());  // Assuming default is 80
}

TEST_F(PortTest, ConstructWithValidPort) {
  Port port(8080);
  EXPECT_EQ(8080, port.getValue());
}

TEST_F(PortTest, WellKnownPorts) {
  EXPECT_NO_THROW(Port(80));
  EXPECT_NO_THROW(Port(443));
  EXPECT_NO_THROW(Port(8080));
  EXPECT_NO_THROW(Port(3000));
}

TEST_F(PortTest, InvalidPortZero) {
  EXPECT_THROW(Port(0), domain::http::exceptions::PortException);
}

TEST_F(PortTest, InvalidPortTooHigh) {
  EXPECT_THROW(Port(65536), domain::http::exceptions::PortException);
}

TEST_F(PortTest, ValidPortRangeLimits) {
  EXPECT_NO_THROW(Port(1));      // Minimum valid port
  EXPECT_NO_THROW(Port(65535));  // Maximum valid port
}

TEST_F(PortTest, CopyConstructor) {
  Port original(8080);
  Port copy(original);
  EXPECT_EQ(original.getValue(), copy.getValue());
}

TEST_F(PortTest, AssignmentOperator) {
  Port port1(8080);
  Port port2(3000);
  port2 = port1;
  EXPECT_EQ(port1.getValue(), port2.getValue());
}

TEST_F(PortTest, EqualityOperator) {
  Port port1(8080);
  Port port2(8080);
  Port port3(3000);
  
  EXPECT_TRUE(port1 == port2);
  EXPECT_FALSE(port1 == port3);
}

TEST_F(PortTest, InequalityOperator) {
  Port port1(8080);
  Port port2(3000);
  
  EXPECT_TRUE(port1 != port2);
}
