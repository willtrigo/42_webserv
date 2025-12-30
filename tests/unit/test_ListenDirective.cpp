/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_ListenDirective.cpp                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: umeneses <umeneses@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/30 11:09:48 by umeneses          #+#    #+#             */
/*   Updated: 2025/12/30 12:40:20 by umeneses         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <gtest/gtest.h>
#include "domain/configuration/value_objects/ListenDirective.hpp"
#include "domain/configuration/exceptions/ListenDirectiveException.hpp"
#include "domain/http/exceptions/PortException.hpp"

using namespace domain::configuration::entities;
using namespace domain::configuration::exceptions;
using namespace domain::http::exceptions;

class ListenDirectiveTest : public ::testing::Test {
 protected:
  void SetUp() {}
  void TearDown() {}
};

// ============================================================================
// Construction Tests
// ============================================================================

TEST_F(ListenDirectiveTest, DefaultConstructor) {
  ListenDirective directive;
  EXPECT_EQ(ListenDirective::DEFAULT_HOST, directive.getHost().getValue());
  EXPECT_EQ(ListenDirective::DEFAULT_PORT.getValue(), directive.getPort().getValue());
}

TEST_F(ListenDirectiveTest, ConstructFromValidString) {
    EXPECT_NO_THROW(ListenDirective(":80"));
    EXPECT_NO_THROW(ListenDirective("[::1]:80"));
    EXPECT_NO_THROW(ListenDirective("localhost:80"));
    EXPECT_NO_THROW(ListenDirective("127.0.0.1:80"));
    EXPECT_NO_THROW(ListenDirective("0.0.0.0:80"));
    EXPECT_NO_THROW(ListenDirective("[::]:80"));
    EXPECT_NO_THROW(ListenDirective("[::]:8080"));
    EXPECT_NO_THROW(ListenDirective("0.0.0.0:8080"));
}

TEST_F(ListenDirectiveTest, ConstructFromInvalidString) {
    EXPECT_THROW(ListenDirective("invalid"), ListenDirectiveException);
    EXPECT_THROW(ListenDirective(""), ListenDirectiveException);
    EXPECT_THROW(ListenDirective("80"), ListenDirectiveException);
    EXPECT_THROW(ListenDirective("localhost"), ListenDirectiveException);
    EXPECT_THROW(ListenDirective("127.0.0.1"), ListenDirectiveException);
    EXPECT_THROW(ListenDirective("::1"), ListenDirectiveException);
    EXPECT_THROW(ListenDirective("::"), ListenDirectiveException);
    EXPECT_THROW(ListenDirective("[::"), ListenDirectiveException);
    EXPECT_THROW(ListenDirective("::]"), ListenDirectiveException);
    EXPECT_THROW(ListenDirective("::80"), ListenDirectiveException);
    EXPECT_THROW(ListenDirective("localhost:"), ListenDirectiveException);
    EXPECT_THROW(ListenDirective("127.0.0.1:"), ListenDirectiveException);
    EXPECT_THROW(ListenDirective("::1:"), ListenDirectiveException);
    EXPECT_THROW(ListenDirective(":::"), ListenDirectiveException);
    EXPECT_THROW(ListenDirective("[::]:"), ListenDirectiveException);
    EXPECT_THROW(ListenDirective("::]:"), ListenDirectiveException);
    EXPECT_THROW(ListenDirective("::80:"), ListenDirectiveException);
    EXPECT_THROW(ListenDirective("localhost:80:"), ListenDirectiveException);
    EXPECT_THROW(ListenDirective("127.0.0.1:80:"), ListenDirectiveException);
    EXPECT_THROW(ListenDirective("::1:80:"), ListenDirectiveException);
    EXPECT_THROW(ListenDirective(":::80:"), ListenDirectiveException);
}

TEST_F(ListenDirectiveTest, ConstructFromHostAndPort) {
  domain::http::value_objects::Host host = domain::http::value_objects::Host::localhost();
  domain::http::value_objects::Port port = domain::http::value_objects::Port(8080);
  
  ListenDirective directive(host, port);
  EXPECT_EQ(host.getValue(), directive.getHost().getValue());
  EXPECT_EQ(8080, directive.getPort().getValue());
}

TEST_F(ListenDirectiveTest, CopyConstructor) {
  ListenDirective original("localhost:8080");
  ListenDirective copy(original);
  
  EXPECT_EQ(original.getHost().getValue(), copy.getHost().getValue());
  EXPECT_EQ(original.getPort().getValue(), copy.getPort().getValue());
}

// ============================================================================
// Assignment Tests
// ============================================================================

TEST_F(ListenDirectiveTest, AssignmentOperator) {
  ListenDirective directive1("localhost:8080");
  ListenDirective directive2(":80");
  
  directive2 = directive1;
  EXPECT_EQ(directive1.getHost().getValue(), directive2.getHost().getValue());
  EXPECT_EQ(directive1.getPort().getValue(), directive2.getPort().getValue());
}

TEST_F(ListenDirectiveTest, AssignmentOperatorSelfAssignment) {
  ListenDirective directive("localhost:8080");
  directive = directive;
  
  EXPECT_EQ("localhost", directive.getHost().getValue());
  EXPECT_EQ(8080, directive.getPort().getValue());
}

// ============================================================================
// Getter Tests
// ============================================================================

TEST_F(ListenDirectiveTest, GetHost) {
  ListenDirective directive("localhost:8080");
  EXPECT_EQ("localhost", directive.getHost().getValue());
}

TEST_F(ListenDirectiveTest, GetPort) {
  ListenDirective directive("localhost:8080");
  EXPECT_EQ(8080, directive.getPort().getValue());
}

TEST_F(ListenDirectiveTest, GetHostWildcard) {
  ListenDirective directive(":80");
  EXPECT_TRUE(directive.getHost().isWildcard());
}

TEST_F(ListenDirectiveTest, GetPortDefault) {
  ListenDirective directive;
  EXPECT_EQ(ListenDirective::DEFAULT_PORT.getValue(), directive.getPort().getValue());
}

// ============================================================================
// Parsing Tests
// ============================================================================

TEST_F(ListenDirectiveTest, ParseIPv4WithPort) {
  ListenDirective directive("192.168.1.1:8080");
  EXPECT_EQ("192.168.1.1", directive.getHost().getValue());
  EXPECT_EQ(8080, directive.getPort().getValue());
}

TEST_F(ListenDirectiveTest, ParseIPv6WithPort) {
  ListenDirective directive("[::1]:8080");
  EXPECT_EQ("::1", directive.getHost().getValue());
  EXPECT_EQ(8080, directive.getPort().getValue());
}

TEST_F(ListenDirectiveTest, ParseHostnameWithPort) {
  ListenDirective directive("example.com:3000");
  EXPECT_EQ("example.com", directive.getHost().getValue());
  EXPECT_EQ(3000, directive.getPort().getValue());
}

TEST_F(ListenDirectiveTest, ParseWildcardWithPort) {
  ListenDirective directive(":8080");
  EXPECT_TRUE(directive.getHost().isWildcard());
  EXPECT_EQ(8080, directive.getPort().getValue());
}

TEST_F(ListenDirectiveTest, ParseWildcardIPv6WithPort) {
  ListenDirective directive("[::]:8080");
  EXPECT_TRUE(directive.getHost().getValue().find("::") != std::string::npos);
  EXPECT_EQ(8080, directive.getPort().getValue());
}

TEST_F(ListenDirectiveTest, SplitDirectiveIPv4) {
  std::pair<std::string, std::string> parts = ListenDirective::splitDirective("127.0.0.1:8080");
  EXPECT_EQ("127.0.0.1", parts.first);
  EXPECT_EQ("8080", parts.second);
}

TEST_F(ListenDirectiveTest, SplitDirectiveIPv6) {
  std::pair<std::string, std::string> parts = ListenDirective::splitDirective("[::1]:8080");
  EXPECT_EQ("::1", parts.first);
  EXPECT_EQ("8080", parts.second);
}

TEST_F(ListenDirectiveTest, SplitDirectiveWildcard) {
  std::pair<std::string, std::string> parts = ListenDirective::splitDirective(":8080");
  EXPECT_TRUE(parts.first.empty());
  EXPECT_EQ("8080", parts.second);
}

// ============================================================================
// Validation Tests
// ============================================================================

TEST_F(ListenDirectiveTest, IsValidDirectiveValid) {
  EXPECT_TRUE(ListenDirective::isValidDirective("localhost:8080"));
  EXPECT_TRUE(ListenDirective::isValidDirective("127.0.0.1:80"));
  EXPECT_TRUE(ListenDirective::isValidDirective("[::1]:8080"));
  EXPECT_TRUE(ListenDirective::isValidDirective(":80"));
}

TEST_F(ListenDirectiveTest, IsValidDirectiveInvalid) {
  EXPECT_FALSE(ListenDirective::isValidDirective(""));
  EXPECT_FALSE(ListenDirective::isValidDirective("invalid"));
  EXPECT_FALSE(ListenDirective::isValidDirective("localhost"));
  EXPECT_FALSE(ListenDirective::isValidDirective("80"));
}

TEST_F(ListenDirectiveTest, IsDefaultListen) {
  ListenDirective defaultDir;
  EXPECT_TRUE(ListenDirective::isDefaultListen(defaultDir));
  
  ListenDirective customDir("localhost:8080");
  EXPECT_FALSE(ListenDirective::isDefaultListen(customDir));
}

// ============================================================================
// Comparison Tests
// ============================================================================

TEST_F(ListenDirectiveTest, EqualityOperator) {
  ListenDirective dir1("localhost:8080");
  ListenDirective dir2("localhost:8080");
  ListenDirective dir3("localhost:3000");
  
  EXPECT_TRUE(dir1 == dir2);
  EXPECT_FALSE(dir1 == dir3);
}

TEST_F(ListenDirectiveTest, InequalityOperator) {
  ListenDirective dir1("localhost:8080");
  ListenDirective dir2("localhost:3000");
  
  EXPECT_TRUE(dir1 != dir2);
  EXPECT_FALSE(dir1 != ListenDirective("localhost:8080"));
}

TEST_F(ListenDirectiveTest, LessThanOperator) {
  ListenDirective dir1("localhost:8080");
  ListenDirective dir2("localhost:9000");
  
  EXPECT_TRUE(dir1 < dir2);
  EXPECT_FALSE(dir2 < dir1);
}

TEST_F(ListenDirectiveTest, LessThanOperatorSamePort) {
  ListenDirective dir1("aaa.com:8080");
  ListenDirective dir2("zzz.com:8080");
  
  EXPECT_TRUE(dir1 < dir2);
}

// ============================================================================
// String Representation Tests
// ============================================================================

TEST_F(ListenDirectiveTest, ToStringDefault) {
  ListenDirective directive;
  std::string result = directive.toString();
  EXPECT_FALSE(result.empty());
}

TEST_F(ListenDirectiveTest, ToStringIPv4) {
  ListenDirective directive("127.0.0.1:8080");
  std::string result = directive.toString();
  EXPECT_NE(result.find("127.0.0.1"), std::string::npos);
  EXPECT_NE(result.find("8080"), std::string::npos);
}

TEST_F(ListenDirectiveTest, ToStringIPv6) {
  ListenDirective directive("[::1]:8080");
  std::string result = directive.toString();
  EXPECT_NE(result.find("["), std::string::npos);
  EXPECT_NE(result.find("]"), std::string::npos);
  EXPECT_NE(result.find("::1"), std::string::npos);
}

TEST_F(ListenDirectiveTest, ToCanonicalString) {
  ListenDirective directive("localhost:8080");
  std::string canonical = directive.toCanonicalString();
  EXPECT_NE(canonical.find("localhost"), std::string::npos);
  EXPECT_NE(canonical.find("8080"), std::string::npos);
  EXPECT_NE(canonical.find(":"), std::string::npos);
}

TEST_F(ListenDirectiveTest, ToCanonicalStringIPv6) {
  ListenDirective directive("[::1]:8080");
  std::string canonical = directive.toCanonicalString();
  EXPECT_NE(canonical.find("[::1]:8080"), std::string::npos);
}

// ============================================================================
// State Query Tests
// ============================================================================

TEST_F(ListenDirectiveTest, IsEmpty) {
  ListenDirective directive;
  EXPECT_FALSE(directive.isEmpty());
}

TEST_F(ListenDirectiveTest, IsWildcard) {
  ListenDirective wildcard(":8080");
  EXPECT_TRUE(wildcard.isWildcard());
  
  ListenDirective specific("localhost:8080");
  EXPECT_FALSE(specific.isWildcard());
}

TEST_F(ListenDirectiveTest, IsLocalhost) {
  ListenDirective localhost("localhost:8080");
  EXPECT_TRUE(localhost.isLocalhost());
  
  ListenDirective other("example.com:8080");
  EXPECT_FALSE(other.isLocalhost());
}

TEST_F(ListenDirectiveTest, IsDefaultPort) {
  ListenDirective defaultPort;
  EXPECT_TRUE(defaultPort.isDefaultPort());
  
  ListenDirective customPort("localhost:8080");
  EXPECT_FALSE(customPort.isDefaultPort());
}

// ============================================================================
// Factory Method Tests
// ============================================================================

TEST_F(ListenDirectiveTest, FromString) {
  ListenDirective directive = ListenDirective::fromString("localhost:8080");
  EXPECT_EQ("localhost", directive.getHost().getValue());
  EXPECT_EQ(8080, directive.getPort().getValue());
}

TEST_F(ListenDirectiveTest, CreateDefault) {
  ListenDirective directive = ListenDirective::createDefault();
  EXPECT_EQ(ListenDirective::DEFAULT_HOST, directive.getHost().getValue());
  EXPECT_EQ(ListenDirective::DEFAULT_PORT.getValue(), directive.getPort().getValue());
}

TEST_F(ListenDirectiveTest, CreateWildcard) {
  domain::http::value_objects::Port port(8080);
  ListenDirective directive = ListenDirective::createWildcard(port);
  
  EXPECT_TRUE(directive.isWildcard());
  EXPECT_EQ(8080, directive.getPort().getValue());
}

TEST_F(ListenDirectiveTest, CreateLocalhost) {
  domain::http::value_objects::Port port(3000);
  ListenDirective directive = ListenDirective::createLocalhost(port);
  
  EXPECT_TRUE(directive.isLocalhost());
  EXPECT_EQ(3000, directive.getPort().getValue());
}

// ============================================================================
// Edge Cases
// ============================================================================

TEST_F(ListenDirectiveTest, IPv6Wildcard) {
  ListenDirective directive("[::]:8080");
  EXPECT_EQ(8080, directive.getPort().getValue());
}

TEST_F(ListenDirectiveTest, MinimumPort) {
  ListenDirective directive("localhost:1");
  EXPECT_EQ(1, directive.getPort().getValue());
}

TEST_F(ListenDirectiveTest, MaximumPort) {
  ListenDirective directive("localhost:65535");
  EXPECT_EQ(65535, directive.getPort().getValue());
}

TEST_F(ListenDirectiveTest, IPv4Wildcard) {
  ListenDirective directive("0.0.0.0:8080");
  EXPECT_EQ(8080, directive.getPort().getValue());
}

TEST_F(ListenDirectiveTest, LongHostname) {
  const std::string longHost = "very-long-hostname-that-should-still-work.example.com:8080";
  ListenDirective* directive = NULL;
  EXPECT_NO_THROW(directive = new ListenDirective(longHost));
  if (directive) delete directive;
}

// ============================================================================
// Boundary Tests
// ============================================================================

TEST_F(ListenDirectiveTest, PortBoundaryZero) {
  EXPECT_THROW(ListenDirective("localhost:0"), PortException);
}

TEST_F(ListenDirectiveTest, PortBoundaryMaxPlus) {
  EXPECT_THROW(ListenDirective("localhost:65536"), PortException);
}

TEST_F(ListenDirectiveTest, WellKnownPorts) {
  EXPECT_NO_THROW(ListenDirective("localhost:80"));
  EXPECT_NO_THROW(ListenDirective("localhost:443"));
  EXPECT_NO_THROW(ListenDirective("localhost:21"));
  EXPECT_NO_THROW(ListenDirective("localhost:22"));
}

TEST_F(ListenDirectiveTest, HighPorts) {
  EXPECT_NO_THROW(ListenDirective("localhost:49152"));
  EXPECT_NO_THROW(ListenDirective("localhost:60000"));
  EXPECT_NO_THROW(ListenDirective("localhost:65535"));
}

// ============================================================================
// Exception Handling Tests
// ============================================================================

TEST_F(ListenDirectiveTest, EmptyStringException) {
  EXPECT_THROW(ListenDirective(""), ListenDirectiveException);
}

TEST_F(ListenDirectiveTest, InvalidFormatException) {
  EXPECT_THROW(ListenDirective(":::"), ListenDirectiveException);
  EXPECT_THROW(ListenDirective("[::"), ListenDirectiveException);
}

TEST_F(ListenDirectiveTest, MissingPortColon) {
  EXPECT_THROW(ListenDirective("localhost:"), ListenDirectiveException);
}

TEST_F(ListenDirectiveTest, InvalidCharacters) {
  EXPECT_THROW(ListenDirective("local@host:8080"), ListenDirectiveException);
  EXPECT_THROW(ListenDirective("localhost:80$0"), ListenDirectiveException);
}

// ============================================================================
// Internal Validation Tests (through public API)
// ============================================================================

TEST_F(ListenDirectiveTest, ValidateDirectiveStringEmpty) {
  EXPECT_FALSE(ListenDirective::isValidDirective(""));
}

TEST_F(ListenDirectiveTest, ValidateDirectiveStringWhitespace) {
  EXPECT_FALSE(ListenDirective::isValidDirective("   "));
}

TEST_F(ListenDirectiveTest, ValidateHostParsing) {
  ListenDirective dir1(":8080");
  EXPECT_TRUE(dir1.getHost().isWildcard());
  
  ListenDirective dir2("localhost:8080");
  EXPECT_FALSE(dir2.getHost().isWildcard());
}

TEST_F(ListenDirectiveTest, ValidatePortParsing) {
  ListenDirective directive("localhost:8080");
  EXPECT_EQ(8080, directive.getPort().getValue());
  EXPECT_FALSE(directive.isDefaultPort());
}

TEST_F(ListenDirectiveTest, ValidateIPv6Brackets) {
  EXPECT_NO_THROW(ListenDirective("[::1]:8080"));
  EXPECT_THROW(ListenDirective("::1:8080"), ListenDirectiveException);
}

TEST_F(ListenDirectiveTest, ValidateMultipleColons) {
  EXPECT_THROW(ListenDirective("localhost:8080:extra"), ListenDirectiveException);
}

TEST_F(ListenDirectiveTest, ValidateConsistency) {
  ListenDirective dir1("localhost:8080");
  ListenDirective dir2("localhost:8080");
  
  EXPECT_EQ(dir1.getHost().getValue(), dir2.getHost().getValue());
  EXPECT_EQ(dir1.getPort().getValue(), dir2.getPort().getValue());
  EXPECT_TRUE(dir1 == dir2);
}

