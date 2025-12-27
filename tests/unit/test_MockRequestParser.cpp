/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_MockRequestParser.cpp                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: umeneses <umeneses@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/27 13:30:18 by umeneses          #+#    #+#             */
/*   Updated: 2025/12/27 14:57:00 by umeneses         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "mocks/MockRequestParser.hpp"
#include <gtest/gtest.h>
#include <sstream>

using namespace mocks;

class MockRequestParserTest : public ::testing::Test {
 protected:
  virtual void SetUp() {}
  virtual void TearDown() {}
};

// Test that setBody correctly sets Content-Length header to the actual body length
TEST_F(MockRequestParserTest, SetBodyShouldSetCorrectContentLength) {
  MockRequestParser parser;
  
  std::string body = "Hello, World!";
  parser.setBody(body);
  
  // Body size should be correct
  EXPECT_EQ(13, parser.getBodySize());
  EXPECT_EQ(body, parser.getBody());
  
  // Content-Length header should match body length
  std::string contentLength = parser.getHeader("Content-Length");
  EXPECT_EQ("13", contentLength);  // Should be "13", not ""
}

TEST_F(MockRequestParserTest, SetBodyWithEmptyStringShouldSetZeroContentLength) {
  MockRequestParser parser;
  
  parser.setBody("");
  
  EXPECT_EQ(0, parser.getBodySize());
  EXPECT_EQ("0", parser.getHeader("Content-Length"));  // Should be "0", not ""
}

TEST_F(MockRequestParserTest, SetBodyMultipleTimesShouldUpdateContentLength) {
  MockRequestParser parser;
  
  parser.setBody("short");
  EXPECT_EQ("5", parser.getHeader("Content-Length"));
  
  parser.setBody("much longer body content");
  EXPECT_EQ("24", parser.getHeader("Content-Length"));
}
