/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_Port.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: umeneses <umeneses@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/24 11:19:59 by umeneses          #+#    #+#             */
/*   Updated: 2025/12/29 21:51:30 by umeneses         ###   ########.fr       */
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

// ============================================================================
// Internal Validation Tests (through public API)
// ============================================================================

TEST_F(PortTest, ValidateShouldAcceptPort1) {
  EXPECT_NO_THROW(Port(1));
  Port port(1);
  EXPECT_EQ(1, port.getValue());
}

TEST_F(PortTest, ValidateShouldAcceptPort1023) {
  EXPECT_NO_THROW(Port(1023));
  Port port(1023);
  EXPECT_EQ(1023, port.getValue());
  EXPECT_TRUE(port.isWellKnown());
}

TEST_F(PortTest, ValidateShouldAcceptPort1024) {
  EXPECT_NO_THROW(Port(1024));
  Port port(1024);
  EXPECT_EQ(1024, port.getValue());
  EXPECT_TRUE(port.isRegistered());
}

TEST_F(PortTest, ValidateShouldAcceptPort49151) {
  EXPECT_NO_THROW(Port(49151));
  Port port(49151);
  EXPECT_EQ(49151, port.getValue());
  EXPECT_TRUE(port.isRegistered());
}

TEST_F(PortTest, ValidateShouldAcceptPort49152) {
  EXPECT_NO_THROW(Port(49152));
  Port port(49152);
  EXPECT_EQ(49152, port.getValue());
  EXPECT_TRUE(port.isDynamic());
}

TEST_F(PortTest, ValidateShouldRejectNegativePort) {
  // unsigned int won't allow negative, but testing edge case
  EXPECT_THROW(Port(-1), domain::http::exceptions::PortException);
}

TEST_F(PortTest, ValidateShouldRejectPort65536) {
  EXPECT_THROW(Port(65536), domain::http::exceptions::PortException);
}

TEST_F(PortTest, ValidateShouldRejectPort70000) {
  EXPECT_THROW(Port(70000), domain::http::exceptions::PortException);
}

TEST_F(PortTest, ValidateShouldRejectPort100000) {
  EXPECT_THROW(Port(100000), domain::http::exceptions::PortException);
}

// parsePortString() and isAllDigits() through string constructor and fromString()
TEST_F(PortTest, ParsePortStringShouldHandleValidPort) {
  Port port("8080");
  EXPECT_EQ(8080, port.getValue());
}

TEST_F(PortTest, ParsePortStringShouldHandleMinimumPort) {
  Port port("1");
  EXPECT_EQ(1, port.getValue());
}

TEST_F(PortTest, ParsePortStringShouldHandleMaximumPort) {
  Port port("65535");
  EXPECT_EQ(65535, port.getValue());
}

TEST_F(PortTest, ParsePortStringShouldRejectEmptyString) {
  EXPECT_THROW(Port(""), domain::http::exceptions::PortException);
}

TEST_F(PortTest, ParsePortStringShouldRejectNonNumeric) {
  EXPECT_THROW(Port("abc"), domain::http::exceptions::PortException);
  EXPECT_THROW(Port("12abc"), domain::http::exceptions::PortException);
  EXPECT_THROW(Port("ab12"), domain::http::exceptions::PortException);
}

TEST_F(PortTest, ParsePortStringShouldRejectLeadingWhitespace) {
  EXPECT_THROW(Port(" 8080"), domain::http::exceptions::PortException);
  EXPECT_THROW(Port("\t8080"), domain::http::exceptions::PortException);
  EXPECT_THROW(Port("\n8080"), domain::http::exceptions::PortException);
}

TEST_F(PortTest, ParsePortStringShouldRejectTrailingWhitespace) {
  EXPECT_THROW(Port("8080 "), domain::http::exceptions::PortException);
  EXPECT_THROW(Port("8080\t"), domain::http::exceptions::PortException);
  EXPECT_THROW(Port("8080\n"), domain::http::exceptions::PortException);
}

TEST_F(PortTest, ParsePortStringShouldRejectSurroundedByWhitespace) {
  EXPECT_THROW(Port("  8080  "), domain::http::exceptions::PortException);
}

TEST_F(PortTest, ParsePortStringShouldHandleLeadingZeros) {
  // Leading zeros should be accepted and parsed correctly
  Port port("08080");
  EXPECT_EQ(8080, port.getValue());
}

TEST_F(PortTest, ParsePortStringShouldHandleMultipleLeadingZeros) {
  Port port("00080");
  EXPECT_EQ(80, port.getValue());
}

TEST_F(PortTest, ParsePortStringShouldRejectNegativeSign) {
  EXPECT_THROW(Port("-8080"), domain::http::exceptions::PortException);
  EXPECT_THROW(Port("-1"), domain::http::exceptions::PortException);
}

TEST_F(PortTest, ParsePortStringShouldRejectPlusSign) {
  EXPECT_THROW(Port("+8080"), domain::http::exceptions::PortException);
}

TEST_F(PortTest, ParsePortStringShouldRejectFloatNotation) {
  EXPECT_THROW(Port("80.80"), domain::http::exceptions::PortException);
  EXPECT_THROW(Port("8080.0"), domain::http::exceptions::PortException);
}

TEST_F(PortTest, ParsePortStringShouldRejectSpecialCharacters) {
  EXPECT_THROW(Port("8080!"), domain::http::exceptions::PortException);
  EXPECT_THROW(Port("8080@"), domain::http::exceptions::PortException);
  EXPECT_THROW(Port("8080#"), domain::http::exceptions::PortException);
  EXPECT_THROW(Port("8080$"), domain::http::exceptions::PortException);
}

TEST_F(PortTest, ParsePortStringShouldRejectOverflow) {
  EXPECT_THROW(Port("65536"), domain::http::exceptions::PortException);
  EXPECT_THROW(Port("100000"), domain::http::exceptions::PortException);
  EXPECT_THROW(Port("999999"), domain::http::exceptions::PortException);
}

TEST_F(PortTest, ParsePortStringShouldRejectZero) {
  EXPECT_THROW(Port("0"), domain::http::exceptions::PortException);
  EXPECT_THROW(Port("00"), domain::http::exceptions::PortException);
}

TEST_F(PortTest, ParsePortStringShouldRejectAlphanumericMix) {
  EXPECT_THROW(Port("80x80"), domain::http::exceptions::PortException);
  EXPECT_THROW(Port("x8080"), domain::http::exceptions::PortException);
  EXPECT_THROW(Port("8080x"), domain::http::exceptions::PortException);
}

// fromString() static method testing
TEST_F(PortTest, FromStringShouldParseValidPort) {
  Port port = Port::fromString("8080");
  EXPECT_EQ(8080, port.getValue());
}

TEST_F(PortTest, FromStringShouldParseBoundaryPorts) {
  EXPECT_EQ(1, Port::fromString("1").getValue());
  EXPECT_EQ(65535, Port::fromString("65535").getValue());
}

TEST_F(PortTest, FromStringShouldRejectInvalidFormats) {
  EXPECT_THROW(Port::fromString(""), domain::http::exceptions::PortException);
  EXPECT_THROW(Port::fromString("abc"), domain::http::exceptions::PortException);
  EXPECT_THROW(Port::fromString("  8080  "), domain::http::exceptions::PortException);
}

// isAllDigits() testing through various string inputs
TEST_F(PortTest, IsAllDigitsShouldAcceptNumericStrings) {
  EXPECT_NO_THROW(Port("123"));
  EXPECT_NO_THROW(Port("8080"));
  EXPECT_NO_THROW(Port("65535"));
}

TEST_F(PortTest, IsAllDigitsShouldRejectMixedContent) {
  EXPECT_THROW(Port("123abc"), domain::http::exceptions::PortException);
  EXPECT_THROW(Port("abc123"), domain::http::exceptions::PortException);
  EXPECT_THROW(Port("12 34"), domain::http::exceptions::PortException);
}

// Port categorization boundary testing
TEST_F(PortTest, WellKnownPortBoundary) {
  Port port1(1023);
  Port port1024(1024);
  
  EXPECT_TRUE(port1.isWellKnown());
  EXPECT_FALSE(port1.isRegistered());
  
  EXPECT_FALSE(port1024.isWellKnown());
  EXPECT_TRUE(port1024.isRegistered());
}

TEST_F(PortTest, RegisteredPortBoundary) {
  Port port49151(49151);
  Port port49152(49152);
  
  EXPECT_TRUE(port49151.isRegistered());
  EXPECT_FALSE(port49151.isDynamic());
  
  EXPECT_FALSE(port49152.isRegistered());
  EXPECT_TRUE(port49152.isDynamic());
}

TEST_F(PortTest, DynamicPortBoundary) {
  Port port49152(49152);
  Port port65535(65535);
  
  EXPECT_TRUE(port49152.isDynamic());
  EXPECT_TRUE(port65535.isDynamic());
}

// Factory methods testing
TEST_F(PortTest, HttpPortFactory) {
  Port port = Port::httpPort();
  EXPECT_EQ(80, port.getValue());
}

TEST_F(PortTest, HttpsPortFactory) {
  Port port = Port::httpsPort();
  EXPECT_EQ(443, port.getValue());
}

TEST_F(PortTest, FtpPortFactory) {
  Port port = Port::ftpPort();
  EXPECT_EQ(21, port.getValue());
}
