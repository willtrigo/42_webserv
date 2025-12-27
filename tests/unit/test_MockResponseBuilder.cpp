/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_MockResponseBuilder.cpp                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: umeneses <umeneses@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/27 12:06:18 by umeneses          #+#    #+#             */
/*   Updated: 2025/12/27 14:56:53 by umeneses         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "mocks/MockResponseBuilder.hpp"
#include <gtest/gtest.h>

using namespace mocks;

class MockResponseBuilderTest : public ::testing::Test {
 protected:
  virtual void SetUp() {}
  virtual void TearDown() {}
};

// ============================================================================
// Status Line Tests
// ============================================================================

TEST_F(MockResponseBuilderTest, DefaultConstructorShouldSetStatus200) {
  MockResponseBuilder builder;
  
  EXPECT_EQ(200, builder.getStatusCode());
  EXPECT_EQ("OK", builder.getStatusMessage());
  EXPECT_EQ("HTTP/1.1", builder.getHttpVersion());
}

TEST_F(MockResponseBuilderTest, SetStatusWithMessageShouldUpdateBoth) {
  MockResponseBuilder builder;
  
  builder.setStatus(404, "Not Found");
  
  EXPECT_EQ(404, builder.getStatusCode());
  EXPECT_EQ("Not Found", builder.getStatusMessage());
}

TEST_F(MockResponseBuilderTest, SetStatusWithCodeOnlyShouldUseDefaultMessage) {
  MockResponseBuilder builder;
  
  builder.setStatus(500);
  
  EXPECT_EQ(500, builder.getStatusCode());
  EXPECT_EQ("Internal Server Error", builder.getStatusMessage());
}

TEST_F(MockResponseBuilderTest, SetHttpVersionShouldUpdateVersion) {
  MockResponseBuilder builder;
  
  builder.setHttpVersion("HTTP/1.0");
  
  EXPECT_EQ("HTTP/1.0", builder.getHttpVersion());
}

// ============================================================================
// Header Tests
// ============================================================================

TEST_F(MockResponseBuilderTest, AddHeaderShouldStoreHeader) {
  MockResponseBuilder builder;
  
  builder.addHeader("Content-Type", "text/html");
  
  EXPECT_TRUE(builder.hasHeader("Content-Type"));
  EXPECT_EQ("text/html", builder.getHeader("Content-Type"));
}

TEST_F(MockResponseBuilderTest, GetHeaderShouldReturnEmptyStringIfNotFound) {
  MockResponseBuilder builder;
  
  std::string value = builder.getHeader("NonExistent");
  
  EXPECT_EQ("", value);
}

TEST_F(MockResponseBuilderTest, RemoveHeaderShouldDeleteHeader) {
  MockResponseBuilder builder;
  builder.addHeader("X-Custom", "value");
  
  builder.removeHeader("X-Custom");
  
  EXPECT_FALSE(builder.hasHeader("X-Custom"));
}

TEST_F(MockResponseBuilderTest, ClearHeadersShouldRemoveAllHeaders) {
  MockResponseBuilder builder;
  builder.addHeader("Header1", "value1");
  builder.addHeader("Header2", "value2");
  
  builder.clearHeaders();
  
  EXPECT_FALSE(builder.hasHeader("Header1"));
  EXPECT_FALSE(builder.hasHeader("Header2"));
}

// ============================================================================
// Body Tests
// ============================================================================

TEST_F(MockResponseBuilderTest, SetBodyShouldUpdateBodyAndContentLength) {
  MockResponseBuilder builder;
  
  builder.setBody("Hello, World!");
  
  EXPECT_EQ("Hello, World!", builder.getBody());
  EXPECT_EQ(13, builder.getBodySize());
  EXPECT_TRUE(builder.hasBody());
  EXPECT_EQ("13", builder.getHeader("Content-Length"));
}

TEST_F(MockResponseBuilderTest, AppendBodyShouldAddToExistingBody) {
  MockResponseBuilder builder;
  builder.setBody("Hello");
  
  builder.appendBody(", World!");
  
  EXPECT_EQ("Hello, World!", builder.getBody());
  EXPECT_EQ(13, builder.getBodySize());
  EXPECT_EQ("13", builder.getHeader("Content-Length"));
}

TEST_F(MockResponseBuilderTest, ClearBodyShouldResetBodyAndContentLength) {
  MockResponseBuilder builder;
  builder.setBody("content");
  
  builder.clearBody();
  
  EXPECT_EQ("", builder.getBody());
  EXPECT_EQ(0, builder.getBodySize());
  EXPECT_FALSE(builder.hasBody());
  EXPECT_EQ("0", builder.getHeader("Content-Length"));
}

// ============================================================================
// Response Building Tests
// ============================================================================

TEST_F(MockResponseBuilderTest, BuildShouldGenerateCompleteHttpResponse) {
  MockResponseBuilder builder;
  builder.setStatus(200, "OK");
  builder.addHeader("Content-Type", "text/html");
  builder.setBody("<html><body>Hello</body></html>");
  
  std::string response = builder.build();
  
  EXPECT_NE(std::string::npos, response.find("HTTP/1.1 200 OK"));
  EXPECT_NE(std::string::npos, response.find("Content-Type: text/html"));
  EXPECT_NE(std::string::npos, response.find("Content-Length: 31"));
  EXPECT_NE(std::string::npos, response.find("<html><body>Hello</body></html>"));
}

TEST_F(MockResponseBuilderTest, BuildStatusLineShouldReturnOnlyStatusLine) {
  MockResponseBuilder builder;
  builder.setStatus(404, "Not Found");
  
  std::string statusLine = builder.buildStatusLine();
  
  EXPECT_EQ("HTTP/1.1 404 Not Found", statusLine);
}

// ============================================================================
// Reset Tests
// ============================================================================

TEST_F(MockResponseBuilderTest, ResetShouldRestoreDefaults) {
  MockResponseBuilder builder;
  builder.setStatus(500);
  builder.addHeader("X-Custom", "value");
  builder.setBody("error");
  
  builder.reset();
  
  EXPECT_EQ(200, builder.getStatusCode());
  EXPECT_EQ("OK", builder.getStatusMessage());
  EXPECT_EQ("HTTP/1.1", builder.getHttpVersion());
  EXPECT_FALSE(builder.hasHeader("X-Custom"));
  EXPECT_EQ("", builder.getBody());
}

// ============================================================================
// Convenience Method Tests
// ============================================================================

TEST_F(MockResponseBuilderTest, BuildOkResponseShouldSet200WithBody) {
  MockResponseBuilder builder;
  
  builder.buildOkResponse("Success!", "text/plain");
  
  EXPECT_EQ(200, builder.getStatusCode());
  EXPECT_EQ("text/plain", builder.getHeader("Content-Type"));
  EXPECT_EQ("Success!", builder.getBody());
}

TEST_F(MockResponseBuilderTest, BuildNotFoundResponseShouldSet404WithHtml) {
  MockResponseBuilder builder;
  
  builder.buildNotFoundResponse("Page not found");
  
  EXPECT_EQ(404, builder.getStatusCode());
  EXPECT_EQ("text/html", builder.getHeader("Content-Type"));
  EXPECT_NE(std::string::npos, builder.getBody().find("404 Not Found"));
  EXPECT_NE(std::string::npos, builder.getBody().find("Page not found"));
}

TEST_F(MockResponseBuilderTest, BuildServerErrorResponseShouldSet500WithHtml) {
  MockResponseBuilder builder;
  
  builder.buildServerErrorResponse("Database error");
  
  EXPECT_EQ(500, builder.getStatusCode());
  EXPECT_EQ("text/html", builder.getHeader("Content-Type"));
  EXPECT_NE(std::string::npos, builder.getBody().find("500 Server Error"));
  EXPECT_NE(std::string::npos, builder.getBody().find("Database error"));
}

TEST_F(MockResponseBuilderTest, BuildBadRequestResponseShouldSet400WithHtml) {
  MockResponseBuilder builder;
  
  builder.buildBadRequestResponse("Invalid input");
  
  EXPECT_EQ(400, builder.getStatusCode());
  EXPECT_EQ("text/html", builder.getHeader("Content-Type"));
  EXPECT_NE(std::string::npos, builder.getBody().find("400 Bad Request"));
  EXPECT_NE(std::string::npos, builder.getBody().find("Invalid input"));
}

// ============================================================================
// Test Helper Tests
// ============================================================================

TEST_F(MockResponseBuilderTest, IsSuccessResponseShouldDetect2xxCodes) {
  MockResponseBuilder builder;
  
  builder.setStatus(200);
  EXPECT_TRUE(builder.isSuccessResponse());
  
  builder.setStatus(201);
  EXPECT_TRUE(builder.isSuccessResponse());
  
  builder.setStatus(299);
  EXPECT_TRUE(builder.isSuccessResponse());
  
  builder.setStatus(300);
  EXPECT_FALSE(builder.isSuccessResponse());
}

TEST_F(MockResponseBuilderTest, IsErrorResponseShouldDetect4xxAnd5xxCodes) {
  MockResponseBuilder builder;
  
  builder.setStatus(400);
  EXPECT_TRUE(builder.isErrorResponse());
  
  builder.setStatus(500);
  EXPECT_TRUE(builder.isErrorResponse());
  
  builder.setStatus(200);
  EXPECT_FALSE(builder.isErrorResponse());
}

TEST_F(MockResponseBuilderTest, IsRedirectResponseShouldDetect3xxCodes) {
  MockResponseBuilder builder;
  
  builder.setStatus(301);
  EXPECT_TRUE(builder.isRedirectResponse());
  
  builder.setStatus(302);
  EXPECT_TRUE(builder.isRedirectResponse());
  
  builder.setStatus(200);
  EXPECT_FALSE(builder.isRedirectResponse());
  
  builder.setStatus(400);
  EXPECT_FALSE(builder.isRedirectResponse());
}
