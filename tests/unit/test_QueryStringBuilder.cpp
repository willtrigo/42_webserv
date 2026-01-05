/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_QueryStringBuilder.cpp                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: umeneses <umeneses@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/29 14:21:59 by umeneses          #+#    #+#             */
/*   Updated: 2026/01/04 15:38:25 by umeneses         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <gtest/gtest.h>
#include "domain/http/value_objects/QueryStringBuilder.hpp"
#include "domain/http/exceptions/QueryStringBuilderException.hpp"

using namespace domain::http::value_objects;

class QueryStringBuilderTest : public ::testing::Test {
 protected:
  void SetUp() {}
  void TearDown() {}
};

// ============================================================================
// Construction Tests
// ============================================================================

TEST_F(QueryStringBuilderTest, DefaultConstructor) {
  QueryStringBuilder builder;
  EXPECT_TRUE(builder.isEmpty());
  EXPECT_EQ("", builder.getBaseUrl());
  EXPECT_EQ(0, builder.getParameterCount());
}

TEST_F(QueryStringBuilderTest, ConstructWithValidBaseUrl) {
  EXPECT_NO_THROW(QueryStringBuilder("http://example.com"));
  EXPECT_NO_THROW(QueryStringBuilder("https://api.example.com/v1/users"));
  EXPECT_NO_THROW(QueryStringBuilder("/api/resource"));
  EXPECT_NO_THROW(QueryStringBuilder("https://example.com:8080/path"));
}

TEST_F(QueryStringBuilderTest, ConstructWithInvalidBaseUrl) {
  EXPECT_THROW(QueryStringBuilder(""), domain::http::exceptions::QueryStringBuilderException);
  EXPECT_THROW(QueryStringBuilder("http://"), domain::http::exceptions::QueryStringBuilderException);
  EXPECT_THROW(QueryStringBuilder("http://example.com:99999"), domain::http::exceptions::QueryStringBuilderException);
}

TEST_F(QueryStringBuilderTest, CopyConstructor) {
  QueryStringBuilder original("http://example.com");
  original.addParameter("key1", "value1");
  original.addParameter("key2", "value2");
  
  QueryStringBuilder copy(original);
  
  EXPECT_EQ(original.getBaseUrl(), copy.getBaseUrl());
  EXPECT_EQ(original.getParameterCount(), copy.getParameterCount());
  EXPECT_EQ(original.getParameter("key1"), copy.getParameter("key1"));
  EXPECT_EQ(original.getParameter("key2"), copy.getParameter("key2"));
  EXPECT_EQ(original.build(), copy.build());
}

TEST_F(QueryStringBuilderTest, AssignmentOperator) {
  QueryStringBuilder builder1("http://example.com");
  builder1.addParameter("key", "value");
  
  QueryStringBuilder builder2;
  builder2 = builder1;
  
  EXPECT_EQ(builder1.getBaseUrl(), builder2.getBaseUrl());
  EXPECT_EQ(builder1.getParameterCount(), builder2.getParameterCount());
  EXPECT_EQ(builder1.getParameter("key"), builder2.getParameter("key"));
  EXPECT_EQ(builder1.build(), builder2.build());
}

TEST_F(QueryStringBuilderTest, AssignmentOperatorSelfAssignment) {
  QueryStringBuilder builder("http://example.com");
  builder.addParameter("key", "value");
  builder = builder;
  
  EXPECT_EQ("http://example.com", builder.getBaseUrl());
  EXPECT_EQ(1, builder.getParameterCount());
  EXPECT_EQ("value", builder.getParameter("key"));
}

TEST_F(QueryStringBuilderTest, ConstructorSetsBaseUrl) {
  QueryStringBuilder builder("http://api.example.com/search");
  EXPECT_EQ("http://api.example.com/search", builder.getBaseUrl());
  EXPECT_FALSE(builder.isEmpty());
}

TEST_F(QueryStringBuilderTest, EmptyBuilder) {
  QueryStringBuilder builder;
  EXPECT_TRUE(builder.isEmpty());
  EXPECT_FALSE(builder.hasQueryString());
}

// ===========================================================================
// Building Queries Tests
// ============================================================================

TEST_F(QueryStringBuilderTest, AddParametersValidParameters) {
  QueryStringBuilder builder("http://example.com");
  EXPECT_NO_THROW(builder.addParameter("key1", "value1"));
  EXPECT_NO_THROW(builder.addParameter("key2", "value2"));
  EXPECT_EQ(2, builder.getParameterCount());
  EXPECT_EQ("value1", builder.getParameter("key1"));
  EXPECT_EQ("value2", builder.getParameter("key2"));
}

TEST_F(QueryStringBuilderTest, AddParametersInvalidParameters) {
    QueryStringBuilder builder("http://example.com");
    EXPECT_THROW(builder.addParameter("", "value"), domain::http::exceptions::QueryStringBuilderException);
    EXPECT_THROW(builder.addParameter(std::string(300, 'a'), "value"), domain::http::exceptions::QueryStringBuilderException);
    EXPECT_THROW(builder.addParameter("key", std::string(2000, 'a')), domain::http::exceptions::QueryStringBuilderException);
}

TEST_F(QueryStringBuilderTest, AddDuplicateParameter) {
  QueryStringBuilder builder("http://example.com");
  builder.addParameter("key", "value1");
  EXPECT_THROW(builder.addParameter("key", "value2"), domain::http::exceptions::QueryStringBuilderException);
}

TEST_F(QueryStringBuilderTest, BuildQuery) {
  QueryStringBuilder builder("http://example.com");
  builder.addParameter("key1", "value1");
  builder.addParameter("key2", "value2");
  std::string url = builder.build();
  EXPECT_EQ("http://example.com?key1=value1&key2=value2", url);
}

TEST_F(QueryStringBuilderTest, AddMultipleParams) {
  QueryStringBuilder builder("http://api.example.com/search");
  builder.addParameter("q", "webserver");
  builder.addParameter("page", "1");
  builder.addParameter("limit", "10");
  builder.addParameter("sort", "desc");
  
  EXPECT_EQ(4, builder.getParameterCount());
  EXPECT_EQ("webserver", builder.getParameter("q"));
  EXPECT_EQ("1", builder.getParameter("page"));
  EXPECT_EQ("10", builder.getParameter("limit"));
  EXPECT_EQ("desc", builder.getParameter("sort"));
  
  std::string url = builder.build();
  EXPECT_TRUE(url.find("q=webserver") != std::string::npos);
  EXPECT_TRUE(url.find("page=1") != std::string::npos);
  EXPECT_TRUE(url.find("limit=10") != std::string::npos);
  EXPECT_TRUE(url.find("sort=desc") != std::string::npos);
}

TEST_F(QueryStringBuilderTest, UrlEncoding) {
  QueryStringBuilder builder("http://example.com");
  builder.addParameter("message", "Hello World");
  builder.addParameter("email", "user@example.com");
  
  std::string url = builder.build();
  EXPECT_TRUE(url.find("message=Hello+World") != std::string::npos || 
              url.find("message=Hello%20World") != std::string::npos);
  EXPECT_TRUE(url.find("email=user%40example.com") != std::string::npos);
}

TEST_F(QueryStringBuilderTest, SpecialCharacters) {
  QueryStringBuilder builder("http://example.com");
  builder.addParameter("query", "C++");
  builder.addParameter("path", "/home/user");
  builder.addParameter("symbol", "&=?");
  
  std::string url = builder.build();
  EXPECT_TRUE(url.find("query=C%2B%2B") != std::string::npos);
  EXPECT_TRUE(url.find("path=%2Fhome%2Fuser") != std::string::npos);
  EXPECT_TRUE(url.find("symbol=%26%3D%3F") != std::string::npos);
}

// ===========================================================================
// Get Parameter As Type Tests
// ============================================================================

TEST_F(QueryStringBuilderTest, GetParameterAsInt) {
  QueryStringBuilder builder("http://example.com");
  builder.addParameter("age", "25");
  builder.addParameter("count", "100");
  builder.addParameter("negative", "-42");
  
  EXPECT_EQ(25, builder.getParameterAsInt("age"));
  EXPECT_EQ(100, builder.getParameterAsInt("count"));
  EXPECT_EQ(-42, builder.getParameterAsInt("negative"));
  EXPECT_EQ(0, builder.getParameterAsInt("missing"));
  EXPECT_EQ(99, builder.getParameterAsInt("missing", 99));
}

TEST_F(QueryStringBuilderTest, GetParameterAsBool) {
  QueryStringBuilder builder("http://example.com");
  builder.addParameter("active", "true");
  builder.addParameter("enabled", "1");
  builder.addParameter("disabled", "false");
  builder.addParameter("zero", "0");
  
  EXPECT_TRUE(builder.getParameterAsBool("active"));
  EXPECT_TRUE(builder.getParameterAsBool("enabled"));
  EXPECT_FALSE(builder.getParameterAsBool("disabled"));
  EXPECT_FALSE(builder.getParameterAsBool("zero"));
  EXPECT_FALSE(builder.getParameterAsBool("missing"));
  EXPECT_TRUE(builder.getParameterAsBool("missing", true));
}

TEST_F(QueryStringBuilderTest, GetParameterAsLong) {
  QueryStringBuilder builder("http://example.com");
  builder.addParameter("timestamp", "1609459200");
  builder.addParameter("bignum", "9223372036854775807");
  builder.addParameter("negative", "-1234567890");
  
  EXPECT_EQ(1609459200L, builder.getParameterAsLong("timestamp"));
  EXPECT_EQ(9223372036854775807L, builder.getParameterAsLong("bignum"));
  EXPECT_EQ(-1234567890L, builder.getParameterAsLong("negative"));
  EXPECT_EQ(0L, builder.getParameterAsLong("missing"));
  EXPECT_EQ(12345L, builder.getParameterAsLong("missing", 12345L));
}

TEST_F(QueryStringBuilderTest, GetParameterAsDouble) {
  QueryStringBuilder builder("http://example.com");
  builder.addParameter("price", "19.99");
  builder.addParameter("rate", "0.125");
  builder.addParameter("negative", "-3.14");
  
  EXPECT_DOUBLE_EQ(19.99, builder.getParameterAsDouble("price"));
  EXPECT_DOUBLE_EQ(0.125, builder.getParameterAsDouble("rate"));
  EXPECT_DOUBLE_EQ(-3.14, builder.getParameterAsDouble("negative"));
  EXPECT_DOUBLE_EQ(0.0, builder.getParameterAsDouble("missing"));
  EXPECT_DOUBLE_EQ(9.99, builder.getParameterAsDouble("missing", 9.99));
}

TEST_F(QueryStringBuilderTest, GetParameterAsIntInvalidValue) {
  QueryStringBuilder builder("http://example.com");
  builder.addParameter("invalid", "not_a_number");
  builder.addParameter("overflow", "99999999999999999999");
  
  EXPECT_EQ(0, builder.getParameterAsInt("invalid"));
  EXPECT_EQ(42, builder.getParameterAsInt("invalid", 42));
}

TEST_F(QueryStringBuilderTest, GetParameterAsDoubleInvalidValue) {
  QueryStringBuilder builder("http://example.com");
  builder.addParameter("invalid", "not_a_number");
  builder.addParameter("text", "abc123");
  
  EXPECT_DOUBLE_EQ(0.0, builder.getParameterAsDouble("invalid"));
  EXPECT_DOUBLE_EQ(1.5, builder.getParameterAsDouble("invalid", 1.5));
}

// ===========================================================================
// Parsing Queries Tests
// ============================================================================

TEST_F(QueryStringBuilderTest, ParseSimple) {
  QueryStringBuilder builder = QueryStringBuilder::parseQueryString("key=value");
  
  EXPECT_EQ("", builder.getBaseUrl());
  EXPECT_EQ(1, builder.getParameterCount());
  EXPECT_EQ("value", builder.getParameter("key"));
}

TEST_F(QueryStringBuilderTest, ParseMultiple) {
  QueryStringBuilder builder = QueryStringBuilder::parseQueryString("name=John&age=30&city=NYC");
  
  EXPECT_EQ("", builder.getBaseUrl());
  EXPECT_EQ(3, builder.getParameterCount());
  EXPECT_EQ("John", builder.getParameter("name"));
  EXPECT_EQ("30", builder.getParameter("age"));
  EXPECT_EQ("NYC", builder.getParameter("city"));
}

TEST_F(QueryStringBuilderTest, ParseEncoded) {
  QueryStringBuilder builder = QueryStringBuilder::parseQueryString("search=hello+world&email=test%40example.com&path=%2Fhome%2Fuser");
  
  EXPECT_EQ("", builder.getBaseUrl());
  EXPECT_EQ(3, builder.getParameterCount());
  EXPECT_EQ("hello world", builder.getParameterDecoded("search"));
  EXPECT_EQ("test@example.com", builder.getParameterDecoded("email"));
  EXPECT_EQ("/home/user", builder.getParameterDecoded("path"));
}

TEST_F(QueryStringBuilderTest, ParseQueryWithParameters) {
  QueryStringBuilder builder = QueryStringBuilder::fromString("http://example.com/search?q=webserver&page=2&limit=10");
  
  EXPECT_EQ("http://example.com/search", builder.getBaseUrl());
  EXPECT_EQ(3, builder.getParameterCount());
  EXPECT_EQ("webserver", builder.getParameter("q"));
  EXPECT_EQ("2", builder.getParameter("page"));
  EXPECT_EQ("10", builder.getParameter("limit"));
}

TEST_F(QueryStringBuilderTest, ParseQueryWithNoParameters) {
  QueryStringBuilder builder = QueryStringBuilder::fromString("http://example.com/path");
  
  EXPECT_EQ("http://example.com/path", builder.getBaseUrl());
  EXPECT_EQ(0, builder.getParameterCount());
}

TEST_F(QueryStringBuilderTest, ParseQueryWithDuplicateParameters) {
  QueryStringBuilder builder = QueryStringBuilder::fromString("http://example.com/search?q=first&q=second");
  
  EXPECT_EQ("http://example.com/search", builder.getBaseUrl());
  EXPECT_EQ(1, builder.getParameterCount());
  EXPECT_EQ("first", builder.getParameter("q"));
}

TEST_F(QueryStringBuilderTest, ParseQueryWithInvalidParameters) {
  QueryStringBuilder builder = QueryStringBuilder::fromString("http://example.com/search?=novalue&keyonly&valid=ok");
  
  EXPECT_EQ("http://example.com/search", builder.getBaseUrl());
  EXPECT_EQ(1, builder.getParameterCount());
  EXPECT_EQ("ok", builder.getParameter("valid"));
}

TEST_F(QueryStringBuilderTest, ParseEmptyQueryString) {
  QueryStringBuilder builder = QueryStringBuilder::fromString("http://example.com?");
  
  EXPECT_EQ("http://example.com", builder.getBaseUrl());
  EXPECT_EQ(0, builder.getParameterCount());
}

TEST_F(QueryStringBuilderTest, ParseQueryStringOnly) {
  QueryStringBuilder builder = QueryStringBuilder::parseQueryString("key1=value1&key2=value2");
  
  EXPECT_EQ("", builder.getBaseUrl());
  EXPECT_EQ(2, builder.getParameterCount());
  EXPECT_EQ("value1", builder.getParameter("key1"));
  EXPECT_EQ("value2", builder.getParameter("key2"));
}

TEST_F(QueryStringBuilderTest, ParseEmptyQueryStringOnly) {
  QueryStringBuilder builder = QueryStringBuilder::parseQueryString("");
  
  EXPECT_EQ("", builder.getBaseUrl());
  EXPECT_EQ(0, builder.getParameterCount());
}

TEST_F(QueryStringBuilderTest, ParseQueryStringWithInvalidParametersOnly) {
  QueryStringBuilder builder = QueryStringBuilder::parseQueryString("=novalue&keyonly&valid=ok");
  
  EXPECT_EQ("", builder.getBaseUrl());
  EXPECT_EQ(1, builder.getParameterCount());
  EXPECT_EQ("ok", builder.getParameter("valid"));
}

TEST_F(QueryStringBuilderTest, ParseQueryStringWithDuplicateParametersOnly) {
  QueryStringBuilder builder = QueryStringBuilder::parseQueryString("key=first&key=second");
  
  EXPECT_EQ("", builder.getBaseUrl());
  EXPECT_EQ(1, builder.getParameterCount());
  EXPECT_EQ("first", builder.getParameter("key"));
}

TEST_F(QueryStringBuilderTest, ParseQueryStringWithSpecialCharactersOnly) {
  QueryStringBuilder builder = QueryStringBuilder::parseQueryString("message=Hello+World&email=user%40example.com");
  
  EXPECT_EQ("", builder.getBaseUrl());
  EXPECT_EQ(2, builder.getParameterCount());
  EXPECT_EQ("Hello World", builder.getParameterDecoded("message"));
  EXPECT_EQ("user@example.com", builder.getParameterDecoded("email"));
}

TEST_F(QueryStringBuilderTest, ParseAndBuildRoundtrip) {
  std::string originalUrl = "http://example.com/search?q=webserver&page=2&limit=10";
  QueryStringBuilder builder = QueryStringBuilder::fromString(originalUrl);
  std::string rebuiltUrl = builder.build();
  
  // Note: Parameter order may differ due to std::map sorting (alphabetical)
  // Check that all parameters are preserved correctly
  EXPECT_EQ("http://example.com/search", builder.getBaseUrl());
  EXPECT_EQ(3, builder.getParameterCount());
  EXPECT_EQ("webserver", builder.getParameter("q"));
  EXPECT_EQ("2", builder.getParameter("page"));
  EXPECT_EQ("10", builder.getParameter("limit"));
  
  // Verify the rebuilt URL contains all parameters with correct encoding
  EXPECT_TRUE(rebuiltUrl.find("q=webserver") != std::string::npos);
  EXPECT_TRUE(rebuiltUrl.find("page=2") != std::string::npos);
  EXPECT_TRUE(rebuiltUrl.find("limit=10") != std::string::npos);
}

TEST_F(QueryStringBuilderTest, ParseEmpty) {
  QueryStringBuilder builder = QueryStringBuilder::parseQueryString("");
  
  EXPECT_EQ("", builder.getBaseUrl());
  EXPECT_EQ(0, builder.getParameterCount());
  EXPECT_TRUE(builder.isEmpty());
}

// ============================================================================
// TODO: Internal Validation Tests (through public API)
// ============================================================================
// NOTE: QueryStringBuilder has a segfault bug - fix before adding these tests
//
// Private methods to test through public API:
// - validate() → Test through constructor with invalid URLs
// - parseUrl() → Test through various URL formats
// - parseQueryString() → Test through query parsing edge cases
// - encodeParameter() → Test through special character handling
// - decodeParameter() → Test through URL decoding
// - isValidUrl() → Test through URL validation edge cases
// - validateParameterKey() → Test through invalid keys
// - validateParameterValue() → Test through invalid values

TEST_F(QueryStringBuilderTest, ValidateUrlFormat) {
  EXPECT_THROW(QueryStringBuilder("not-a-url"), domain::http::exceptions::QueryStringBuilderException);
  EXPECT_THROW(QueryStringBuilder("http://"), domain::http::exceptions::QueryStringBuilderException);
  EXPECT_NO_THROW(QueryStringBuilder("http://example.com"));
}

TEST_F(QueryStringBuilderTest, ParseUrlWithComplexQuery) {
  QueryStringBuilder builder("http://example.com?key1=value1&key2=value%202&key3=");
  EXPECT_EQ(3, builder.getParameterCount());
  EXPECT_EQ("value1", builder.getParameter("key1"));
  EXPECT_EQ("value 2", builder.getParameter("key2"));
  EXPECT_EQ("", builder.getParameter("key3"));
}

TEST_F(QueryStringBuilderTest, EncodeSpecialCharacters) {
  QueryStringBuilder builder("http://example.com");
  builder.addParameter("special", "a b+c&d=e");
  std::string url = builder.build();
  EXPECT_TRUE(url.find("a+b") != std::string::npos || url.find("a%20b") != std::string::npos);
  EXPECT_TRUE(url.find("%2B") != std::string::npos);
  EXPECT_TRUE(url.find("%26") != std::string::npos);
  EXPECT_TRUE(url.find("%3D") != std::string::npos);
}

