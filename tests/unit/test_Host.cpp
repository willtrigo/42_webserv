/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_Host.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: umeneses <umeneses@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/29 18:38:19 by umeneses          #+#    #+#             */
/*   Updated: 2025/12/29 20:43:29 by umeneses         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <gtest/gtest.h>
#include "domain/http/value_objects/Host.hpp"
#include "domain/http/exceptions/HostException.hpp"

using namespace domain::http::value_objects;

class HostTest : public ::testing::Test {
 protected:
  void SetUp() {}
  void TearDown() {}
};

// ============================================================================
// Construction Tests
// ============================================================================

TEST_F(HostTest, DefaultConstructor) {
  Host host;
  EXPECT_EQ("", host.getValue());
  EXPECT_EQ(Host::TYPE_UNKNOWN, host.getType());
}

TEST_F(HostTest, ConstructWithValidHost) {
    EXPECT_NO_THROW(Host("example.com"));
    EXPECT_NO_THROW(Host("192.168.0.1"));
    EXPECT_NO_THROW(Host("localhost"));
    EXPECT_NO_THROW(Host("::1"));
}

TEST_F(HostTest, ConstructWithInvalidHost) {
    EXPECT_THROW(Host(""), domain::http::exceptions::HostException);
    EXPECT_THROW(Host("invalid-host"), domain::http::exceptions::HostException);
}

TEST_F(HostTest, CopyConstructor) {
  Host original("example.com");
  Host copy(original);
  EXPECT_EQ(original.getValue(), copy.getValue());
  EXPECT_EQ(original.getType(), copy.getType());
}

TEST_F(HostTest, ConstructWithTooLongHost) {
  std::string longHost(Host::MAX_HOST_LENGTH + 1, 'a');
  EXPECT_THROW({ Host host(longHost); }, domain::http::exceptions::HostException);
}

TEST_F(HostTest, ConstructWithMalformedIpv4) {
  EXPECT_THROW(Host("256.100.50.25"), domain::http::exceptions::HostException);
  EXPECT_THROW(Host("192.168.0"), domain::http::exceptions::HostException);
}

TEST_F(HostTest, ConstructWithMalformedIpv6) {
  EXPECT_THROW(Host("2001:0db8:85a3::8a2e:0370:7334:1234"), domain::http::exceptions::HostException);
  EXPECT_THROW(Host("::2001:0db8:85a3::8a2e:0370:7334:1234"), domain::http::exceptions::HostException);
}

TEST_F(HostTest, ConstructWithInvalidHostname) {
  EXPECT_THROW(Host("-invalidhostname.com"), domain::http::exceptions::HostException);
  EXPECT_THROW(Host("invalid_hostname.com"), domain::http::exceptions::HostException);
}

TEST_F(HostTest, ConstructWithInvalidIpv4) {
  EXPECT_THROW(Host("192.168.0.256"), domain::http::exceptions::HostException);
  EXPECT_THROW(Host("192.168.0.-1"), domain::http::exceptions::HostException);
}

TEST_F(HostTest, ConstructWithInvalidIpv6) {
  EXPECT_THROW(Host("2001:0db8:85a3::8a2e:0370:7334:12345"), domain::http::exceptions::HostException);
  EXPECT_THROW(Host("::2001:0db8:85a3::8a2e:0370:7334:12345"), domain::http::exceptions::HostException);
}

TEST_F(HostTest, ConstructWithIpv4) {
  Host host("192.168.0.1");
  EXPECT_EQ("192.168.0.1", host.getValue());
  EXPECT_EQ(Host::TYPE_IPV4, host.getType());
}

// ============================================================================
// Assignment Tests
// ============================================================================

TEST_F(HostTest, AssignmentOperator) {
  Host host1("example.com");
  Host host2("localhost");
  host2 = host1;
  EXPECT_EQ(host1.getValue(), host2.getValue());
  EXPECT_EQ(host1.getType(), host2.getType());
}

TEST_F(HostTest, AssignmentOperatorSelfAssignment) {
  Host host("example.com");
  host = host;
  EXPECT_EQ("example.com", host.getValue());
  EXPECT_EQ(Host::TYPE_HOSTNAME, host.getType());
}

TEST_F(HostTest, InvalidHostAssignment) {
  Host host("example.com");
  EXPECT_THROW(host = Host("invalid-host"), domain::http::exceptions::HostException);
}

TEST_F(HostTest, AssignmentWithTooLongHost) {
  Host host("example.com");
  std::string longHost(Host::MAX_HOST_LENGTH + 1, 'a');
  EXPECT_THROW({ host = Host(longHost); }, domain::http::exceptions::HostException);
}

TEST_F(HostTest, AssignmentWithMalformedIpv4) {
  Host host("example.com");
  EXPECT_THROW(host = Host("256.100.50.25"), domain::http::exceptions::HostException);
}

TEST_F(HostTest, AssignmentWithMalformedIpv6) {
  Host host("example.com");
  EXPECT_THROW(host = Host("2001:0db8:85a3::8a2e:0370:7334:1234"), domain::http::exceptions::HostException);
}

TEST_F(HostTest, AssignmentWithInvalidHostname) {
  Host host("example.com");
  EXPECT_THROW(host = Host("-invalidhostname.com"), domain::http::exceptions::HostException);
}

TEST_F(HostTest, AssignmentWithInvalidIpv4) {
  Host host("example.com");
  EXPECT_THROW(host = Host("192.168.0.256"), domain::http::exceptions::HostException);
}

TEST_F(HostTest, AssignmentWithInvalidIpv6) {
  Host host("example.com");
  EXPECT_THROW(host = Host("2001:0db8:85a3::8a2e:0370:7334:12345"), domain::http::exceptions::HostException);
}



// ============================================================================
// Validation Tests
// ============================================================================

TEST_F(HostTest, IsValidHost) {
  EXPECT_TRUE(Host::isValidHost("example.com"));
  EXPECT_TRUE(Host::isValidHost("192.168.0.1"));
  EXPECT_TRUE(Host::isValidHost("localhost"));
  EXPECT_TRUE(Host::isValidHost("::1"));
}

TEST_F(HostTest, IsValidInvalidHost) {
  EXPECT_FALSE(Host::isValidHost(""));
  EXPECT_FALSE(Host::isValidHost("invalid-host"));
  EXPECT_FALSE(Host::isValidHost("256.100.50.25"));
  EXPECT_FALSE(Host::isValidHost("192.168.0"));
  EXPECT_FALSE(Host::isValidHost("2001:0db8:85a3::8a2e:0370:7334:1234"));
  EXPECT_FALSE(Host::isValidHost("-invalidhostname.com"));
  EXPECT_FALSE(Host::isValidHost("192.168.0.256"));
  EXPECT_FALSE(Host::isValidHost("2001:0db8:85a3::8a2e:0370:7334:12345"));
}

TEST_F(HostTest, IsValidIpv4) {
  EXPECT_TRUE(Host::isValidIpv4("192.168.0.1"));
  EXPECT_TRUE(Host::isValidIpv4("127.0.0.1"));
  EXPECT_TRUE(Host::isValidIpv4("0.0.0.0"));
  EXPECT_TRUE(Host::isValidIpv4("255.255.255.255"));
}

TEST_F(HostTest, IsValidInvalidIpv4) {
  EXPECT_FALSE(Host::isValidIpv4("256.100.50.25"));
  EXPECT_FALSE(Host::isValidIpv4("192.168.0"));
  EXPECT_FALSE(Host::isValidIpv4("192.168.0.-1"));
  EXPECT_FALSE(Host::isValidIpv4("192.168.0.256"));
}

TEST_F(HostTest, IsValidIpv6) {
  EXPECT_TRUE(Host::isValidIpv6("2001:0db8:85a3:0000:0000:8a2e:0370:7334"));
  EXPECT_TRUE(Host::isValidIpv6("::1"));
  EXPECT_TRUE(Host::isValidIpv6("fe80::1ff:fe23:4567:890a"));
}

TEST_F(HostTest, IsValidInvalidIpv6) {
  EXPECT_FALSE(Host::isValidIpv6("2001:0db8:85a3::8a2e:0370:7334:1234"));
  EXPECT_FALSE(Host::isValidIpv6("::2001:0db8:85a3::8a2e:0370:7334:1234"));
  EXPECT_FALSE(Host::isValidIpv6("2001:0db8:85a3::8a2e:0370:7334:12345"));
  EXPECT_FALSE(Host::isValidIpv6("::2001:0db8:85a3::8a2e:0370:7334:12345"));
}

TEST_F(HostTest, IsValidHostname) {
  EXPECT_TRUE(Host::isValidHostname("example.com"));
  EXPECT_TRUE(Host::isValidHostname("sub.domain.co.uk"));
  EXPECT_TRUE(Host::isValidHostname("localhost"));
}

TEST_F(HostTest, IsValidInvalidHostname) {
  EXPECT_FALSE(Host::isValidHostname(""));
  EXPECT_FALSE(Host::isValidHostname("-invalidhostname.com"));
  EXPECT_FALSE(Host::isValidHostname("invalid_hostname.com"));
}

TEST_F(HostTest, IsLocalhost) {
  Host host("example.com");
  EXPECT_FALSE(host.isLocalhost());
  Host localhost("localhost");
  EXPECT_TRUE(localhost.isLocalhost());
}

TEST_F(HostTest, IsWildcard) {
  Host wildcard = Host::wildcard();
  Host ipv4Host("192.168.0.1");
  Host ipv6Host("2001:0db8:85a3:0000:0000:8a2e:0370:7334");
  Host domainHost("example.com");
  EXPECT_TRUE(wildcard.isWildcard());
  EXPECT_FALSE(ipv4Host.isWildcard());
  EXPECT_FALSE(ipv6Host.isWildcard());
  EXPECT_FALSE(domainHost.isWildcard());
}

TEST_F(HostTest, IsHostname) {
  Host host("example.com");
  Host ipv4Host("192.168.0.1");
  Host ipv6Host("2001:0db8:85a3:0000:0000:8a2e:0370:7334");
  EXPECT_TRUE(host.isHostname());
  EXPECT_FALSE(ipv4Host.isHostname());
  EXPECT_FALSE(ipv6Host.isHostname());
}


// ============================================================================
// Operations Tests
// ============================================================================

TEST_F(HostTest, ToString) {
  Host host("example.com");
  EXPECT_EQ("example.com", host.getValue());
}

TEST_F(HostTest, FromString) {
  Host host = Host::fromString("example.com");
  EXPECT_EQ("example.com", host.getValue());
  EXPECT_EQ(Host::TYPE_HOSTNAME, host.getType());
}

TEST_F(HostTest, LocalhostStaticMethod) {
  Host localhost = Host::localhost();
  EXPECT_TRUE(localhost.isLocalhost());
}

TEST_F(HostTest, WildcardStaticMethod) {
  Host wildcard = Host::wildcard();
  EXPECT_TRUE(wildcard.isWildcard());
}


// ============================================================================
// Getters Tests
// ============================================================================

TEST_F(HostTest, GetTypeString) {
  Host ipv4Host("192.168.0.1");
  Host ipv6Host("2001:0db8:85a3:0000:0000:8a2e:0370:7334");
  Host domainHost("example.com");
  Host unknownHost;

  EXPECT_EQ("IPv4", ipv4Host.getTypeString());
  EXPECT_EQ("IPv6", ipv6Host.getTypeString());
  EXPECT_EQ("Hostname", domainHost.getTypeString());
  EXPECT_EQ("Unknown", unknownHost.getTypeString());
}

TEST_F(HostTest, GetType) {
  Host ipv4Host("192.168.0.1");
  Host ipv6Host("2001:0db8:85a3:0000:0000:8a2e:0370:7334");
  Host domainHost("example.com");
  Host unknownHost;

  EXPECT_EQ(Host::TYPE_IPV4, ipv4Host.getType());
  EXPECT_EQ(Host::TYPE_IPV6, ipv6Host.getType());
  EXPECT_EQ(Host::TYPE_HOSTNAME, domainHost.getType());
  EXPECT_EQ(Host::TYPE_UNKNOWN, unknownHost.getType());
}

TEST_F(HostTest, GetValue) {
  Host host("example.com");
  EXPECT_EQ("example.com", host.getValue());
}

TEST_F(HostTest, GetHostname) {
  Host host("example.com");
  EXPECT_EQ("example.com", host.getValue());
}

TEST_F(HostTest, GetIpv4) {
  Host host("192.168.0.1");
  EXPECT_EQ("192.168.0.1", host.getValue());
}



// ===========================================================================
// Setters Tests
// ============================================================================

TEST_F(HostTest, SetValue) {
  Host host("example.com");
  EXPECT_EQ("example.com", host.getValue());
  host = Host("localhost");
  EXPECT_EQ("localhost", host.getValue());
}

TEST_F(HostTest, SetType) {
  Host host("example.com");
  EXPECT_EQ(Host::TYPE_HOSTNAME, host.getType());
  host = Host("192.168.0.1");
  EXPECT_EQ(Host::TYPE_IPV4, host.getType());
}

TEST_F(HostTest, SetInvalidValue) {
  Host host("example.com");
  EXPECT_THROW(host = Host("invalid-host"), domain::http::exceptions::HostException);
}

TEST_F(HostTest, SetTooLongValue) {
  Host host("example.com");
  std::string longHost(Host::MAX_HOST_LENGTH + 1, 'a');
  EXPECT_THROW(host = Host(longHost), domain::http::exceptions::HostException);
}

TEST_F(HostTest, SetMalformedIpv4) {
  Host host("example.com");
  EXPECT_THROW(host = Host("256.100.50.25"), domain::http::exceptions::HostException);
}

TEST_F(HostTest, SetMalformedIpv6) {
  Host host("example.com");
  EXPECT_THROW(host = Host("2001:0db8:85a3::8a2e:0370:7334:1234"), domain::http::exceptions::HostException);
}


// ==========================================================================
// Comparison Tests
// ==========================================================================

TEST_F(HostTest, EqualityOperator) {
  Host host1("example.com");
  Host host2("example.com");
  Host host3("localhost");
  EXPECT_TRUE(host1 == host2);
  EXPECT_FALSE(host1 == host3);
}

TEST_F(HostTest, InequalityOperator) {
  Host host1("example.com");
  Host host2("localhost");
  EXPECT_TRUE(host1 != host2);
}

TEST_F(HostTest, LessThanOperator) {
  Host host1("a.example.com");
  Host host2("b.example.com");
  EXPECT_TRUE(host1 < host2);
  EXPECT_FALSE(host2 < host1);
}

TEST_F(HostTest, ComparisonWithDifferentTypes) {
  Host ipv4Host("192.168.0.1");
  Host ipv6Host("2001:0db8:85a3:0000:0000:8a2e:0370:7334");
  Host domainHost("example.com");
  Host unknownHost;

  EXPECT_TRUE(ipv4Host == ipv6Host);
  EXPECT_FALSE(ipv4Host == domainHost);
  EXPECT_TRUE(unknownHost != domainHost);
}

TEST_F(HostTest, ComparisonWithInvalidHost) {
  Host host1("example.com");
  EXPECT_THROW(host1 == Host("invalid-host"), domain::http::exceptions::HostException);
  EXPECT_THROW(host1 != Host("invalid-host"), domain::http::exceptions::HostException);
  EXPECT_THROW(host1 < Host("invalid-host"), domain::http::exceptions::HostException);
}


// ============================================================================
// Internal Validation Tests (through public API)
// ============================================================================

// Tests for parseIpv4 (through isValidIpv4)
TEST_F(HostTest, ParseIpv4ValidOctets) {
  EXPECT_TRUE(Host::isValidIpv4("0.0.0.0"));
  EXPECT_TRUE(Host::isValidIpv4("255.255.255.255"));
  EXPECT_TRUE(Host::isValidIpv4("192.168.1.1"));
  EXPECT_TRUE(Host::isValidIpv4("10.0.0.1"));
}

TEST_F(HostTest, ParseIpv4InvalidOctets) {
  EXPECT_FALSE(Host::isValidIpv4("256.0.0.0"));
  EXPECT_FALSE(Host::isValidIpv4("0.256.0.0"));
  EXPECT_FALSE(Host::isValidIpv4("0.0.256.0"));
  EXPECT_FALSE(Host::isValidIpv4("0.0.0.256"));
  EXPECT_FALSE(Host::isValidIpv4("300.300.300.300"));
  EXPECT_FALSE(Host::isValidIpv4("-1.0.0.0"));
  EXPECT_FALSE(Host::isValidIpv4("1.-1.0.0"));
}

TEST_F(HostTest, ParseIpv4LeadingZeros) {
  EXPECT_FALSE(Host::isValidIpv4("01.0.0.0"));
  EXPECT_FALSE(Host::isValidIpv4("192.01.0.0"));
  EXPECT_FALSE(Host::isValidIpv4("192.168.01.0"));
  EXPECT_FALSE(Host::isValidIpv4("192.168.0.01"));
}

TEST_F(HostTest, ParseIpv4MissingOctets) {
  EXPECT_FALSE(Host::isValidIpv4("192.168.0"));
  EXPECT_FALSE(Host::isValidIpv4("192.168"));
  EXPECT_FALSE(Host::isValidIpv4("192"));
  EXPECT_FALSE(Host::isValidIpv4(""));
}

TEST_F(HostTest, ParseIpv4ExtraOctets) {
  EXPECT_FALSE(Host::isValidIpv4("192.168.0.1.1"));
  EXPECT_FALSE(Host::isValidIpv4("192.168.0.1.2.3"));
}

TEST_F(HostTest, ParseIpv4EmptyOctets) {
  EXPECT_FALSE(Host::isValidIpv4(".."));
  EXPECT_FALSE(Host::isValidIpv4("192..0.1"));
  EXPECT_FALSE(Host::isValidIpv4(".192.168.0.1"));
  EXPECT_FALSE(Host::isValidIpv4("192.168.0.1."));
}

TEST_F(HostTest, ParseIpv4NonNumericOctets) {
  EXPECT_FALSE(Host::isValidIpv4("abc.def.ghi.jkl"));
  EXPECT_FALSE(Host::isValidIpv4("192.168.0.a"));
  EXPECT_FALSE(Host::isValidIpv4("192.168.x.1"));
}

// Tests for parseIpv6 (through isValidIpv6)
TEST_F(HostTest, ParseIpv6FullForm) {
  EXPECT_TRUE(Host::isValidIpv6("2001:0db8:85a3:0000:0000:8a2e:0370:7334"));
  EXPECT_TRUE(Host::isValidIpv6("fe80:0000:0000:0000:0204:61ff:fe9d:f156"));
}

TEST_F(HostTest, ParseIpv6CompressedForm) {
  EXPECT_TRUE(Host::isValidIpv6("::1"));
  EXPECT_TRUE(Host::isValidIpv6("::"));
  EXPECT_TRUE(Host::isValidIpv6("fe80::1"));
  EXPECT_TRUE(Host::isValidIpv6("2001:db8::1"));
}

TEST_F(HostTest, ParseIpv6InvalidFormat) {
  EXPECT_FALSE(Host::isValidIpv6(""));
  EXPECT_FALSE(Host::isValidIpv6("gggg::1"));
  EXPECT_FALSE(Host::isValidIpv6("12345::1"));
  EXPECT_FALSE(Host::isValidIpv6("::xyz"));
}

TEST_F(HostTest, ParseIpv6TooManyGroups) {
  EXPECT_FALSE(Host::isValidIpv6("1:2:3:4:5:6:7:8:9"));
}

TEST_F(HostTest, ParseIpv6TooManyDoubleColons) {
  EXPECT_FALSE(Host::isValidIpv6("::1::2"));
  EXPECT_FALSE(Host::isValidIpv6("2001::db8::1"));
}

TEST_F(HostTest, ParseIpv6TooLong) {
  std::string tooLong(Host::MAX_IPV6_LENGTH + 1, '0');
  EXPECT_FALSE(Host::isValidIpv6(tooLong));
}

// Tests for parseHostname (through isValidHostname)
TEST_F(HostTest, ParseHostnameValidLabels) {
  EXPECT_TRUE(Host::isValidHostname("example"));
  EXPECT_TRUE(Host::isValidHostname("example.com"));
  EXPECT_TRUE(Host::isValidHostname("sub.domain.example.com"));
  EXPECT_TRUE(Host::isValidHostname("my-server.example.com"));
  EXPECT_TRUE(Host::isValidHostname("server123.example.com"));
}

TEST_F(HostTest, ParseHostnameInvalidLabels) {
  EXPECT_FALSE(Host::isValidHostname("-example.com"));
  EXPECT_FALSE(Host::isValidHostname("example-.com"));
  EXPECT_FALSE(Host::isValidHostname("example.-com"));
  EXPECT_FALSE(Host::isValidHostname("example.com-"));
}

TEST_F(HostTest, ParseHostnameTooLongLabel) {
  std::string longLabel(Host::MAX_LABEL_LENGTH + 1, 'a');
  EXPECT_FALSE(Host::isValidHostname(longLabel + ".com"));
}

TEST_F(HostTest, ParseHostnameTooLongTotal) {
  std::string longHost(Host::MAX_HOST_LENGTH + 1, 'a');
  EXPECT_FALSE(Host::isValidHostname(longHost));
}

TEST_F(HostTest, ParseHostnameEmptyLabels) {
  EXPECT_FALSE(Host::isValidHostname(".example.com"));
  EXPECT_FALSE(Host::isValidHostname("example..com"));
  EXPECT_FALSE(Host::isValidHostname("example.com."));
}

TEST_F(HostTest, ParseHostnameInvalidCharacters) {
  EXPECT_FALSE(Host::isValidHostname("example_com"));
  EXPECT_FALSE(Host::isValidHostname("example@com"));
  EXPECT_FALSE(Host::isValidHostname("example.com!"));
  EXPECT_FALSE(Host::isValidHostname("example com"));
}

TEST_F(HostTest, ParseHostnameAllNumericTld) {
  EXPECT_FALSE(Host::isValidHostname("example.123"));
  EXPECT_FALSE(Host::isValidHostname("server.999"));
}

TEST_F(HostTest, ParseHostnameMixedTld) {
  EXPECT_TRUE(Host::isValidHostname("example.com1"));
  EXPECT_TRUE(Host::isValidHostname("example.1com"));
}

// Tests for determineType (through getType)
TEST_F(HostTest, DetermineTypeIpv4) {
  Host host("192.168.0.1");
  EXPECT_EQ(Host::TYPE_IPV4, host.getType());
}

TEST_F(HostTest, DetermineTypeIpv6) {
  Host host("2001:0db8:85a3:0000:0000:8a2e:0370:7334");
  EXPECT_EQ(Host::TYPE_IPV6, host.getType());
}

TEST_F(HostTest, DetermineTypeHostname) {
  Host host("example.com");
  EXPECT_EQ(Host::TYPE_HOSTNAME, host.getType());
}

TEST_F(HostTest, DetermineTypeWildcard) {
  Host wildcard1("*");
  Host wildcard2("0.0.0.0");
  EXPECT_TRUE(wildcard1.isWildcard());
  EXPECT_TRUE(wildcard2.isWildcard());
}

TEST_F(HostTest, DetermineTypeUnknown) {
  Host host;
  EXPECT_EQ(Host::TYPE_UNKNOWN, host.getType());
}

// Tests for validateHostString (through constructor)
TEST_F(HostTest, ValidateHostStringEmpty) {
  EXPECT_THROW(Host(""), domain::http::exceptions::HostException);
}

TEST_F(HostTest, ValidateHostStringTooLong) {
  std::string tooLong(Host::MAX_HOST_LENGTH + 1, 'a');
  EXPECT_THROW({ Host host(tooLong); }, domain::http::exceptions::HostException);
}

TEST_F(HostTest, ValidateHostStringInvalidFormat) {
  EXPECT_THROW(Host("invalid_host_name"), domain::http::exceptions::HostException);
  EXPECT_THROW(Host("!!!invalid!!!"), domain::http::exceptions::HostException);
  EXPECT_THROW(Host("256.256.256.256"), domain::http::exceptions::HostException);
}

TEST_F(HostTest, ValidateHostStringValidFormats) {
  EXPECT_NO_THROW(Host("example.com"));
  EXPECT_NO_THROW(Host("192.168.0.1"));
  EXPECT_NO_THROW(Host("::1"));
  EXPECT_NO_THROW(Host("localhost"));
}

// Tests for isValidIpv4Octet (indirectly through IPv4 validation)
TEST_F(HostTest, ValidIpv4OctetBoundaries) {
  EXPECT_TRUE(Host::isValidIpv4("0.0.0.0"));
  EXPECT_TRUE(Host::isValidIpv4("255.255.255.255"));
  EXPECT_TRUE(Host::isValidIpv4("127.0.0.1"));
}

TEST_F(HostTest, InvalidIpv4OctetOutOfRange) {
  EXPECT_FALSE(Host::isValidIpv4("256.0.0.0"));
  EXPECT_FALSE(Host::isValidIpv4("999.0.0.0"));
  EXPECT_FALSE(Host::isValidIpv4("1000.0.0.0"));
}

TEST_F(HostTest, InvalidIpv4OctetNonNumeric) {
  EXPECT_FALSE(Host::isValidIpv4("abc.0.0.0"));
  EXPECT_FALSE(Host::isValidIpv4("1a.0.0.0"));
  EXPECT_FALSE(Host::isValidIpv4("1.2.3.x"));
}

TEST_F(HostTest, InvalidIpv4OctetLeadingZero) {
  EXPECT_FALSE(Host::isValidIpv4("01.0.0.0"));
  EXPECT_FALSE(Host::isValidIpv4("001.0.0.0"));
  EXPECT_FALSE(Host::isValidIpv4("0001.0.0.0"));
}

TEST_F(HostTest, InvalidIpv4OctetEmpty) {
  EXPECT_FALSE(Host::isValidIpv4(".0.0.0"));
  EXPECT_FALSE(Host::isValidIpv4("0..0.0"));
  EXPECT_FALSE(Host::isValidIpv4("0.0.0."));
}

TEST_F(HostTest, InvalidIpv4OctetTooLong) {
  EXPECT_FALSE(Host::isValidIpv4("1234.0.0.0"));
  EXPECT_FALSE(Host::isValidIpv4("12345.0.0.0"));
}

// Tests for isValidHostnameLabel (indirectly through hostname validation)
TEST_F(HostTest, ValidHostnameLabel) {
  EXPECT_TRUE(Host::isValidHostname("a.com"));
  EXPECT_TRUE(Host::isValidHostname("abc.com"));
  EXPECT_TRUE(Host::isValidHostname("a-b-c.com"));
  EXPECT_TRUE(Host::isValidHostname("abc123.com"));
}

TEST_F(HostTest, InvalidHostnameLabelStartsWithHyphen) {
  EXPECT_FALSE(Host::isValidHostname("-abc.com"));
  EXPECT_FALSE(Host::isValidHostname("sub.-abc.com"));
}

TEST_F(HostTest, InvalidHostnameLabelEndsWithHyphen) {
  EXPECT_FALSE(Host::isValidHostname("abc-.com"));
  EXPECT_FALSE(Host::isValidHostname("sub.abc-.com"));
}

TEST_F(HostTest, InvalidHostnameLabelTooLong) {
  std::string longLabel(Host::MAX_LABEL_LENGTH + 1, 'a');
  EXPECT_FALSE(Host::isValidHostname(longLabel));
  EXPECT_FALSE(Host::isValidHostname("sub." + longLabel + ".com"));
}

TEST_F(HostTest, InvalidHostnameLabelEmpty) {
  EXPECT_FALSE(Host::isValidHostname(""));
  EXPECT_FALSE(Host::isValidHostname("."));
  EXPECT_FALSE(Host::isValidHostname(".."));
}

TEST_F(HostTest, InvalidHostnameLabelInvalidChars) {
  EXPECT_FALSE(Host::isValidHostname("abc_def.com"));
  EXPECT_FALSE(Host::isValidHostname("abc@def.com"));
  EXPECT_FALSE(Host::isValidHostname("abc def.com"));
  EXPECT_FALSE(Host::isValidHostname("abc#def.com"));
}

// Tests for isAllDigits (indirectly through hostname TLD validation)
TEST_F(HostTest, HostnameWithAllDigitsTld) {
  EXPECT_FALSE(Host::isValidHostname("server.123"));
  EXPECT_FALSE(Host::isValidHostname("example.999"));
  EXPECT_FALSE(Host::isValidHostname("sub.domain.000"));
}

TEST_F(HostTest, HostnameWithMixedTld) {
  EXPECT_TRUE(Host::isValidHostname("server.com1"));
  EXPECT_TRUE(Host::isValidHostname("example.1com"));
  EXPECT_TRUE(Host::isValidHostname("sub.domain.co1"));
}

TEST_F(HostTest, HostnameWithAllDigitsNonTldLabel) {
  EXPECT_TRUE(Host::isValidHostname("123.example.com"));
  EXPECT_TRUE(Host::isValidHostname("server.999.example.com"));
}

// Tests for parseHost (indirectly through constructor)
TEST_F(HostTest, ParseHostSetsCorrectType) {
  Host ipv4("192.168.0.1");
  Host ipv6("2001:0db8:85a3:0000:0000:8a2e:0370:7334");
  Host hostname("example.com");
  
  EXPECT_EQ(Host::TYPE_IPV4, ipv4.getType());
  EXPECT_EQ(Host::TYPE_IPV6, ipv6.getType());
  EXPECT_EQ(Host::TYPE_HOSTNAME, hostname.getType());
}

TEST_F(HostTest, ParseHostSetsCorrectValue) {
  Host ipv4("192.168.0.1");
  Host ipv6("::1");
  Host hostname("example.com");
  
  EXPECT_EQ("192.168.0.1", ipv4.getValue());
  EXPECT_EQ("::1", ipv6.getValue());
  EXPECT_EQ("example.com", hostname.getValue());
}

