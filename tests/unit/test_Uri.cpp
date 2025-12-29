/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_Uri.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: umeneses <umeneses@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/29 12:52:32 by umeneses          #+#    #+#             */
/*   Updated: 2025/12/29 13:43:24 by umeneses         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <gtest/gtest.h>
#include "domain/http/value_objects/Uri.hpp"
#include "domain/http/exceptions/UriException.hpp"

using namespace domain::http::value_objects;

class UriTest : public ::testing::Test {
 protected:
  void SetUp() {}
  void TearDown() {}
};

// ============================================================================
// Construction Tests
// ============================================================================

TEST_F(UriTest, DefaultConstructor) {
  Uri uri;
  EXPECT_TRUE(uri.isEmpty());
}

TEST_F(UriTest, ConstructWithValidUri) {
  EXPECT_NO_THROW(Uri("http://www.example.com:80/path?query#fragment"));
  EXPECT_NO_THROW(Uri("https://www.example.com"));
  EXPECT_NO_THROW(Uri("ftp://ftp.example.com/resource"));
  EXPECT_NO_THROW(Uri("file:///C:/path/to/file.txt"));
}

TEST_F(UriTest, ConstructWithInvalidUri) {
    EXPECT_THROW(Uri(""), domain::http::exceptions::UriException);
    EXPECT_THROW(Uri("http://"), domain::http::exceptions::UriException);
    EXPECT_THROW(Uri("http://www.example.com:99999"), domain::http::exceptions::UriException);
}

TEST_F(UriTest, ParseMalformedUri_invalid_port_format) {
    EXPECT_THROW(Uri("http://example.com:abc"), domain::http::exceptions::UriException);
    EXPECT_THROW(Uri("http://example.com:-80"), domain::http::exceptions::UriException);
    EXPECT_THROW(Uri("http://example.com:0"), domain::http::exceptions::UriException);
    EXPECT_THROW(Uri("http://example.com:65536"), domain::http::exceptions::UriException);
}

TEST_F(UriTest, ParseMalformedUri_missing_host) {
    EXPECT_THROW(Uri("http:///path"), domain::http::exceptions::UriException);
    EXPECT_THROW(Uri("https://"), domain::http::exceptions::UriException);
}

TEST_F(UriTest, ParseMalformedUri_invalid_scheme) {
    EXPECT_THROW(Uri("123://example.com"), domain::http::exceptions::UriException);
    EXPECT_THROW(Uri("-http://example.com"), domain::http::exceptions::UriException);
}

TEST_F(UriTest, ParseMalformedUri_too_long_uri) {
    std::string tooLongUri = "http://example.com/" + std::string(4100, 'a');
    EXPECT_THROW({Uri uri(tooLongUri);}, domain::http::exceptions::UriException);
}

TEST_F(UriTest, CopyConstructor) {
    Uri original("http://www.example.com:80/path?query#fragment");
    Uri copy(original);
    EXPECT_EQ(original.toString(), copy.toString());
}

TEST_F(UriTest, AssignmentOperator) {
    Uri uri1("http://www.example.com:80/path?query#fragment");
    Uri uri2;
    uri2 = uri1;
    EXPECT_EQ(uri1.toString(), uri2.toString());
}

TEST_F(UriTest, EqualityOperator) {
    Uri uri1("http://www.example.com:80/path?query#fragment");
    Uri uri2("http://www.example.com:80/path?query#fragment");
    Uri uri3("http://www.example.com:80/otherpath?query#fragment");
    EXPECT_TRUE(uri1 == uri2);
    EXPECT_FALSE(uri1 == uri3);
}

TEST_F(UriTest, InequalityOperator) {
    Uri uri1("http://www.example.com:80/path?query#fragment");
    Uri uri2("http://www.example.com:80/otherpath?query#fragment");
    EXPECT_TRUE(uri1 != uri2);
}

TEST_F(UriTest, LessThanOperator) {
    Uri uri1("http://a.example.com");
    Uri uri2("http://b.example.com");
    EXPECT_TRUE(uri1 < uri2);
    EXPECT_FALSE(uri2 < uri1);
}

TEST_F(UriTest, GetComponents) {
    Uri uri("http://www.example.com:80/path?query#fragment");
    EXPECT_EQ("http", uri.getScheme());
    EXPECT_EQ("www.example.com", uri.getHost());
    EXPECT_EQ(80, uri.getPort().getValue());
    EXPECT_EQ("/path", uri.getPath());
    EXPECT_EQ("query", uri.getQuery());
    EXPECT_EQ("fragment", uri.getFragment());
}

TEST_F(UriTest, ToString) {
    // Default port 80 is omitted from output (correct HTTP behavior)
    Uri uri("http://www.example.com:80/path?query#fragment");
    EXPECT_EQ("http://www.example.com/path?query#fragment", uri.toString());
}

TEST_F(UriTest, ConstructFromString) {
    Uri uri = Uri::fromString("http://www.example.com:80/path?query#fragment");
    EXPECT_EQ("http", uri.getScheme());
    EXPECT_EQ("www.example.com", uri.getHost());
    EXPECT_EQ(80, uri.getPort().getValue());
    EXPECT_EQ("/path", uri.getPath());
    EXPECT_EQ("query", uri.getQuery());
    EXPECT_EQ("fragment", uri.getFragment());
}

TEST_F(UriTest, ConstructFromComponents_V2) {
    Uri::UriParameters params;
    params.m_scheme = "http";
    params.m_host = "www.example.com";
    params.m_port = Port(80);
    params.m_path = "/path";
    params.m_query = "query";
    params.m_fragment = "fragment";
    Uri uri (params);
    // Default port 80 is omitted from toString() output
    EXPECT_EQ("http://www.example.com/path?query#fragment", uri.toString());
    EXPECT_EQ("http", uri.getScheme());
    EXPECT_EQ("www.example.com", uri.getHost());
    EXPECT_EQ(80, uri.getPort().getValue());
    EXPECT_EQ("/path", uri.getPath());
    EXPECT_EQ("query", uri.getQuery());
    EXPECT_EQ("fragment", uri.getFragment());
}

// ============================================================================
// Parsing Tests
// ============================================================================

TEST_F(UriTest, ParseAuthority) {
    Uri uri = Uri::parseAuthority("www.example.com:8080");
    EXPECT_EQ("", uri.getScheme());
    EXPECT_EQ("www.example.com", uri.getHost());
    EXPECT_EQ(8080, uri.getPort().getValue());
    EXPECT_EQ("", uri.getPath());
    EXPECT_EQ("", uri.getQuery());
    EXPECT_EQ("", uri.getFragment());
}

TEST_F(UriTest, ParseAuthorityWithoutPort) {
    Uri uri = Uri::parseAuthority("www.example.com");
    EXPECT_EQ("", uri.getScheme());
    EXPECT_EQ("www.example.com", uri.getHost());
    EXPECT_EQ(0, uri.getPort().getValue());
    EXPECT_EQ("", uri.getPath());
    EXPECT_EQ("", uri.getQuery());
    EXPECT_EQ("", uri.getFragment());
}

TEST_F(UriTest, CreateHttpUri) {
    Uri uri = Uri::createHttp("www.example.com", Port(80), "/index.html");
    EXPECT_EQ("http", uri.getScheme());
    EXPECT_EQ("www.example.com", uri.getHost());
    EXPECT_EQ(80, uri.getPort().getValue());
    EXPECT_EQ("/index.html", uri.getPath());
}

TEST_F(UriTest, CreateHttpsUri) {
    Uri uri = Uri::createHttps("secure.example.com", Port(443), "/secure");
    EXPECT_EQ("https", uri.getScheme());
    EXPECT_EQ("secure.example.com", uri.getHost());
    EXPECT_EQ(443, uri.getPort().getValue());
    EXPECT_EQ("/secure", uri.getPath());
}

TEST_F(UriTest, CreateFileUri) {
    Uri uri = Uri::createFile("/C:/path/to/file.txt");
    EXPECT_EQ("file", uri.getScheme());
    EXPECT_EQ("", uri.getHost());
    EXPECT_EQ(0, uri.getPort().getValue());
    EXPECT_EQ("/C:/path/to/file.txt", uri.getPath());
}

TEST_F(UriTest, InvalidAuthorityParsing) {
    // Empty authority returns empty Uri (not an error)
    // Test truly invalid authorities:
    EXPECT_THROW(Uri::parseAuthority(":"), domain::http::exceptions::UriException);
    EXPECT_THROW(Uri::parseAuthority("www.example.com:99999"), domain::http::exceptions::UriException);
    EXPECT_THROW(Uri::parseAuthority("host:port:extra"), domain::http::exceptions::UriException);
}

TEST_F(UriTest, ParseAbsoluteUri) {
    Uri uri("http://www.example.com:80/path?query#fragment");
    EXPECT_TRUE(uri.isAbsolute());
    EXPECT_FALSE(uri.isRelative());
    EXPECT_EQ("http", uri.getScheme());
    EXPECT_EQ("www.example.com", uri.getHost());
    EXPECT_EQ(80, uri.getPort().getValue());
    EXPECT_EQ("/path", uri.getPath());
    EXPECT_EQ("query", uri.getQuery());
    EXPECT_EQ("fragment", uri.getFragment());
}

TEST_F(UriTest, ParseRelativeUri) {
    Uri uri("/path/to/resource?key=value");
    EXPECT_TRUE(uri.isRelative());
    EXPECT_FALSE(uri.isAbsolute());
    EXPECT_EQ("", uri.getScheme());
    EXPECT_EQ("", uri.getHost());
    EXPECT_EQ("/path/to/resource", uri.getPath());
    EXPECT_EQ("key=value", uri.getQuery());
}

TEST_F(UriTest, ParseUriWithQueryOnly) {
    Uri uri("http://example.com?search=test&page=1");
    EXPECT_EQ("http", uri.getScheme());
    EXPECT_EQ("example.com", uri.getHost());
    EXPECT_EQ("/", uri.getPath());
    EXPECT_EQ("search=test&page=1", uri.getQuery());
    EXPECT_EQ("", uri.getFragment());
}

TEST_F(UriTest, ParseUriWithFragmentOnly) {
    Uri uri("http://example.com/page#section");
    EXPECT_EQ("http", uri.getScheme());
    EXPECT_EQ("example.com", uri.getHost());
    EXPECT_EQ("/page", uri.getPath());
    EXPECT_EQ("", uri.getQuery());
    EXPECT_EQ("section", uri.getFragment());
}

TEST_F(UriTest, ParseUriWithExplicitPort) {
    Uri uri("https://example.com:8443/secure");
    EXPECT_EQ("https", uri.getScheme());
    EXPECT_EQ("example.com", uri.getHost());
    EXPECT_EQ(8443, uri.getPort().getValue());
    EXPECT_EQ("/secure", uri.getPath());
}

TEST_F(UriTest, ParseUriWithDefaultPort) {
    Uri uri("http://example.com/page");
    EXPECT_EQ("http", uri.getScheme());
    EXPECT_EQ("example.com", uri.getHost());
    EXPECT_EQ(0, uri.getPort().getValue());
    EXPECT_EQ("/page", uri.getPath());
}

TEST_F(UriTest, StaticIsValidUri) {
    EXPECT_TRUE(Uri::isValidUri("http://example.com"));
    EXPECT_TRUE(Uri::isValidUri("/relative/path"));
    EXPECT_FALSE(Uri::isValidUri(""));
    EXPECT_FALSE(Uri::isValidUri("http://"));
}

TEST_F(UriTest, StaticIsAbsoluteUri) {
    EXPECT_TRUE(Uri::isAbsoluteUri("http://example.com"));
    EXPECT_TRUE(Uri::isAbsoluteUri("https://example.com/path"));
    EXPECT_FALSE(Uri::isAbsoluteUri("/path"));
    EXPECT_FALSE(Uri::isAbsoluteUri(""));
}

TEST_F(UriTest, StaticIsRelativeUri) {
    EXPECT_TRUE(Uri::isRelativeUri("/path"));
    EXPECT_TRUE(Uri::isRelativeUri("/path?query"));
    EXPECT_FALSE(Uri::isRelativeUri("http://example.com"));
    EXPECT_FALSE(Uri::isRelativeUri(""));
}

// ============================================================================
// Component Extraction Tests
// ============================================================================

TEST_F(UriTest, GetAuthority) {
    Uri uri1("http://example.com:8080/path");
    EXPECT_EQ("example.com:8080", uri1.getAuthority());
    
    Uri uri2("http://example.com/path");
    EXPECT_EQ("example.com", uri2.getAuthority());
    
    Uri uri3("/relative/path");
    EXPECT_EQ("", uri3.getAuthority());
}

TEST_F(UriTest, GetBasePath) {
    Uri uri1("http://example.com/path/to/file.html");
    EXPECT_EQ("/path/to/", uri1.getBasePath());
    
    Uri uri2("http://example.com/file.html");
    EXPECT_EQ("/", uri2.getBasePath());
    
    Uri uri3("http://example.com/");
    EXPECT_EQ("/", uri3.getBasePath());
}

TEST_F(UriTest, GetFilename) {
    Uri uri1("http://example.com/path/to/file.html");
    EXPECT_EQ("file.html", uri1.getFilename());
    
    Uri uri2("http://example.com/path/to/");
    EXPECT_EQ("", uri2.getFilename());
    
    Uri uri3("http://example.com/document.pdf");
    EXPECT_EQ("document.pdf", uri3.getFilename());
}

TEST_F(UriTest, GetExtension) {
    Uri uri1("http://example.com/file.html");
    EXPECT_EQ(".html", uri1.getExtension());
    
    Uri uri2("http://example.com/archive.tar.gz");
    EXPECT_EQ(".gz", uri2.getExtension());
    
    Uri uri3("http://example.com/noextension");
    EXPECT_EQ("", uri3.getExtension());
}

TEST_F(UriTest, GetParentUri) {
    Uri uri1("http://example.com/path/to/file.html");
    Uri parent1 = uri1.getParentUri();
    EXPECT_EQ("/path/to", parent1.getPath());
    
    Uri uri2("http://example.com/path/");
    Uri parent2 = uri2.getParentUri();
    EXPECT_EQ("/path", parent2.getPath());
}

TEST_F(UriTest, GetQueryParameter) {
    std::string query = "key1=value1&key2=value2&key3=value3";
    EXPECT_EQ("value1", Uri::getQueryParameter(query, "key1"));
    EXPECT_EQ("value2", Uri::getQueryParameter(query, "key2"));
    EXPECT_EQ("value3", Uri::getQueryParameter(query, "key3"));
    EXPECT_EQ("", Uri::getQueryParameter(query, "nonexistent"));
}

TEST_F(UriTest, GetScheme) {
    Uri uri1("http://example.com/path");
    EXPECT_EQ("http", uri1.getScheme());
    
    Uri uri2("https://secure.example.com");
    EXPECT_EQ("https", uri2.getScheme());
    
    Uri uri3("ftp://ftp.example.com/file");
    EXPECT_EQ("ftp", uri3.getScheme());
    
    Uri uri4("file:///C:/path/to/file.txt");
    EXPECT_EQ("file", uri4.getScheme());
    
    Uri uri5("/relative/path");
    EXPECT_EQ("", uri5.getScheme());
}

TEST_F(UriTest, GetHost) {
    Uri uri1("http://example.com/path");
    EXPECT_EQ("example.com", uri1.getHost());
    
    Uri uri2("http://subdomain.example.com:8080");
    EXPECT_EQ("subdomain.example.com", uri2.getHost());
    
    Uri uri3("http://192.168.1.1/path");
    EXPECT_EQ("192.168.1.1", uri3.getHost());
    
    Uri uri4("/relative/path");
    EXPECT_EQ("", uri4.getHost());
}

TEST_F(UriTest, GetPort) {
    Uri uri1("http://example.com:8080/path");
    EXPECT_EQ(8080, uri1.getPort().getValue());
    
    Uri uri2("https://example.com:443");
    EXPECT_EQ(443, uri2.getPort().getValue());
    
    Uri uri3("http://example.com/path");
    EXPECT_EQ(0, uri3.getPort().getValue());
    
    Uri uri4("/relative/path");
    EXPECT_EQ(0, uri4.getPort().getValue());
}

TEST_F(UriTest, GetPath) {
    Uri uri1("http://example.com/path/to/resource");
    EXPECT_EQ("/path/to/resource", uri1.getPath());
    
    Uri uri2("http://example.com/");
    EXPECT_EQ("/", uri2.getPath());
    
    Uri uri3("http://example.com");
    EXPECT_EQ("/", uri3.getPath());
    
    Uri uri4("/relative/path");
    EXPECT_EQ("/relative/path", uri4.getPath());
}

TEST_F(UriTest, GetQuery) {
    Uri uri1("http://example.com/path?key=value&foo=bar");
    EXPECT_EQ("key=value&foo=bar", uri1.getQuery());
    
    Uri uri2("http://example.com/path?single");
    EXPECT_EQ("single", uri2.getQuery());
    
    Uri uri3("http://example.com/path");
    EXPECT_EQ("", uri3.getQuery());
    
    Uri uri4("/path?query=test");
    EXPECT_EQ("query=test", uri4.getQuery());
}

TEST_F(UriTest, GetFragment) {
    Uri uri1("http://example.com/path#section");
    EXPECT_EQ("section", uri1.getFragment());
    
    Uri uri2("http://example.com/page#top");
    EXPECT_EQ("top", uri2.getFragment());
    
    Uri uri3("http://example.com/path");
    EXPECT_EQ("", uri3.getFragment());
    
    Uri uri4("/path#anchor");
    EXPECT_EQ("anchor", uri4.getFragment());
}

TEST_F(UriTest, ToStringMethod) {
    Uri uri1("http://example.com:8080/path?query#fragment");
    EXPECT_EQ("http://example.com:8080/path?query#fragment", uri1.toString());
    
    Uri uri2("https://example.com/path");
    EXPECT_EQ("https://example.com/path", uri2.toString());
    
    Uri uri3("/relative/path?key=value");
    EXPECT_EQ("/relative/path?key=value", uri3.toString());
    
    // Default port should be omitted
    Uri uri4("http://example.com:80/path");
    EXPECT_EQ("http://example.com/path", uri4.toString());
}

// ============================================================================
// URI Manipulation Tests
// ============================================================================

TEST_F(UriTest, NormalizePath) {
    Uri uri1("http://example.com/a/b/../c/./d");
    Uri normalized1 = uri1.normalize();
    EXPECT_EQ("/a/c/d", normalized1.getPath());
    
    Uri uri2("http://example.com/a/b/../../c");
    Uri normalized2 = uri2.normalize();
    EXPECT_EQ("/c", normalized2.getPath());
}

TEST_F(UriTest, ResolveRelativeUri) {
    Uri base("http://example.com/path/to/page.html");
    
    Uri relative1("../other.html");
    Uri resolved1 = relative1.resolve(base);
    EXPECT_EQ("http", resolved1.getScheme());
    EXPECT_EQ("example.com", resolved1.getHost());
    
    Uri relative2("/absolute/path");
    Uri resolved2 = relative2.resolve(base);
    EXPECT_EQ("/absolute/path", resolved2.getPath());
}

TEST_F(UriTest, WithScheme) {
    Uri uri("http://example.com/path");
    Uri changed = uri.withScheme("https");
    EXPECT_EQ("https", changed.getScheme());
    EXPECT_EQ("example.com", changed.getHost());
    EXPECT_EQ("/path", changed.getPath());
}

TEST_F(UriTest, WithHost) {
    Uri uri("http://example.com/path");
    Uri changed = uri.withHost("newhost.com");
    EXPECT_EQ("http", changed.getScheme());
    EXPECT_EQ("newhost.com", changed.getHost());
    EXPECT_EQ("/path", changed.getPath());
}

TEST_F(UriTest, WithPort) {
    Uri uri("http://example.com/path");
    Uri changed = uri.withPort(Port(8080));
    EXPECT_EQ(8080, changed.getPort().getValue());
}

TEST_F(UriTest, WithPath) {
    Uri uri("http://example.com/oldpath");
    Uri changed = uri.withPath("/newpath");
    EXPECT_EQ("/newpath", changed.getPath());
}

TEST_F(UriTest, WithQuery) {
    Uri uri("http://example.com/path");
    Uri changed = uri.withQuery("key=value");
    EXPECT_EQ("key=value", changed.getQuery());
}

TEST_F(UriTest, WithFragment) {
    Uri uri("http://example.com/path");
    Uri changed = uri.withFragment("section");
    EXPECT_EQ("section", changed.getFragment());
}

// ============================================================================
// Encoding/Decoding Tests
// ============================================================================

TEST_F(UriTest, EncodeComponent) {
    EXPECT_EQ("hello%20world", Uri::encodeComponent("hello world"));
    EXPECT_EQ("test%2Bvalue", Uri::encodeComponent("test+value"));
    EXPECT_EQ("user%40example.com", Uri::encodeComponent("user@example.com"));
}

TEST_F(UriTest, DecodeComponent) {
    EXPECT_EQ("hello world", Uri::decodeComponent("hello%20world"));
    EXPECT_EQ("test+value", Uri::decodeComponent("test%2Bvalue"));
    EXPECT_EQ("user@example.com", Uri::decodeComponent("user%40example.com"));
}

TEST_F(UriTest, EncodeDecodeRoundtrip) {
    std::string original = "Hello World! @#$%^&*()";
    std::string encoded = Uri::encodeComponent(original);
    std::string decoded = Uri::decodeComponent(encoded);
    EXPECT_EQ(original, decoded);
}

// ============================================================================
// Type Checking Tests
// ============================================================================

TEST_F(UriTest, IsHttp) {
    Uri uri1("http://example.com");
    EXPECT_TRUE(uri1.isHttp());
    EXPECT_FALSE(uri1.isHttps());
    
    Uri uri2("https://example.com");
    EXPECT_FALSE(uri2.isHttp());
    EXPECT_TRUE(uri2.isHttps());
}

TEST_F(UriTest, IsDefaultPort) {
    Uri uri1("http://example.com:80/path");
    EXPECT_TRUE(uri1.isDefaultPort());
    
    Uri uri2("http://example.com:8080/path");
    EXPECT_FALSE(uri2.isDefaultPort());
    
    Uri uri3("https://example.com:443/path");
    EXPECT_TRUE(uri3.isDefaultPort());
}

TEST_F(UriTest, HasComponents) {
    Uri uri("http://example.com:8080/path?query#fragment");
    EXPECT_TRUE(uri.hasScheme());
    EXPECT_TRUE(uri.hasHost());
    EXPECT_TRUE(uri.hasPort());
    EXPECT_TRUE(uri.hasPath());
    EXPECT_TRUE(uri.hasQuery());
    EXPECT_TRUE(uri.hasFragment());
    
    Uri minimal("/path");
    EXPECT_FALSE(minimal.hasScheme());
    EXPECT_FALSE(minimal.hasHost());
    EXPECT_FALSE(minimal.hasPort());
    EXPECT_TRUE(minimal.hasPath());
    EXPECT_FALSE(minimal.hasQuery());
    EXPECT_FALSE(minimal.hasFragment());
}