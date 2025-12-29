/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_HttpMethod.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: umeneses <umeneses@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/24 14:11:11 by umeneses          #+#    #+#             */
/*   Updated: 2025/12/29 12:51:21 by umeneses         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// Include the class we want to test
#include "domain/http/value_objects/HttpMethod.hpp"
#include "domain/http/exceptions/HttpMethodException.hpp"

// Include Google Test
#include <gtest/gtest.h>

// Short form - easier to type
using namespace domain::http::value_objects;

// Simple test class for HTTP methods
// (This one is simple, so SetUp/TearDown don't do anything)
class HttpMethodTest : public ::testing::Test {
 protected:
  virtual void SetUp() {
    // Nothing to set up for these simple tests
  }
  
  virtual void TearDown() {
    // Nothing to clean up
  }
};

// TEST 1: Check that valid HTTP methods work correctly
// HTTP has standard methods like GET, POST, DELETE, etc.
TEST_F(HttpMethodTest, ValidMethods) {
  // Each of these should work without throwing an exception
  // EXPECT_NO_THROW means "this should NOT cause an error"
  
  EXPECT_NO_THROW(HttpMethod("GET"));      // Used to retrieve data
  EXPECT_NO_THROW(HttpMethod("POST"));     // Used to send data
  EXPECT_NO_THROW(HttpMethod("PUT"));      // Used to update data
  EXPECT_NO_THROW(HttpMethod("DELETE"));   // Used to delete data
  EXPECT_NO_THROW(HttpMethod("HEAD"));     // Like GET but only gets headers
  EXPECT_NO_THROW(HttpMethod("OPTIONS"));  // Used to check what methods are allowed
}

// TEST 2: Check that invalid methods are rejected
TEST_F(HttpMethodTest, InvalidMethod) {
  // "INVALID" is not a real HTTP method - this should throw an exception
  EXPECT_THROW(
    HttpMethod("INVALID"),                       // This is wrong
    domain::http::exceptions::HttpMethodException      // So it should throw this
  );
  
  // Empty string is also invalid
  EXPECT_THROW(
    HttpMethod(""),                              // Empty is wrong too
    domain::http::exceptions::HttpMethodException
  );
}

// TEST 3: HTTP methods should work regardless of uppercase/lowercase
// "GET", "get", and "Get" should all be treated the same
TEST_F(HttpMethodTest, CaseInsensitive) {
  // Create three HttpMethod objects with different cases
  HttpMethod upperCase("GET");
  HttpMethod lowerCase("get");
  HttpMethod mixedCase("Get");
  
  // Convert each to string
  std::string upper = upperCase.toString();
  std::string lower = lowerCase.toString();
  std::string mixed = mixedCase.toString();
  
  // They should all be the same
  EXPECT_EQ(upper, lower);
  EXPECT_EQ(upper, mixed);
}

// TEST 4: Check if two HttpMethod objects are equal
TEST_F(HttpMethodTest, Equality) {
  // Create two GET methods
  HttpMethod get1("GET");
  HttpMethod get2("GET");
  
  // Create a POST method
  HttpMethod post("POST");
  
  // The two GET methods should be equal
  bool getsAreEqual = (get1 == get2);
  EXPECT_TRUE(getsAreEqual);
  
  // GET should NOT equal POST
  bool getEqualsPost = (get1 == post);
  EXPECT_FALSE(getEqualsPost);
}

// TEST 5: Check that copying an HttpMethod works correctly
// This tests the "copy constructor"
TEST_F(HttpMethodTest, CopyConstructor) {
  // Create an original HttpMethod
  HttpMethod original("GET");
  
  // Create a copy of it
  HttpMethod copy(original);
  
  // They should have the same value
  std::string originalStr = original.toString();
  std::string copyStr = copy.toString();
  
  EXPECT_EQ(originalStr, copyStr);
}
