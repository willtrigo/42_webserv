/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_HttpMethod.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: umeneses <umeneses@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/24 14:11:11 by umeneses          #+#    #+#             */
/*   Updated: 2025/12/30 11:23:49 by umeneses         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <gtest/gtest.h>
#include "domain/http/value_objects/HttpMethod.hpp"
#include "domain/http/exceptions/HttpMethodException.hpp"

using namespace domain::http::value_objects;
using namespace domain::http::exceptions;

class HttpMethodTest : public ::testing::Test {
 protected:
  void SetUp() {}
  void TearDown() {}
};

// ============================================================================
// Construction Tests
// ============================================================================

TEST_F(HttpMethodTest, DefaultConstructor) {
  HttpMethod method;
  EXPECT_EQ(HttpMethod::METHOD_GET, method.getMethod());
}

TEST_F(HttpMethodTest, ConstructFromValidString) {
  EXPECT_NO_THROW(HttpMethod("GET"));
  EXPECT_NO_THROW(HttpMethod("POST"));
  EXPECT_NO_THROW(HttpMethod("PUT"));
  EXPECT_NO_THROW(HttpMethod("DELETE"));
  EXPECT_NO_THROW(HttpMethod("HEAD"));
  EXPECT_NO_THROW(HttpMethod("OPTIONS"));
  EXPECT_NO_THROW(HttpMethod("TRACE"));
  EXPECT_NO_THROW(HttpMethod("CONNECT"));
  EXPECT_NO_THROW(HttpMethod("PATCH"));
}

TEST_F(HttpMethodTest, ConstructFromInvalidString) {
  EXPECT_THROW(HttpMethod("INVALID"), HttpMethodException);
  EXPECT_THROW(HttpMethod(""), HttpMethodException);
  EXPECT_THROW(HttpMethod("get post"), HttpMethodException);
}

TEST_F(HttpMethodTest, ConstructFromEnum) {
  HttpMethod get(HttpMethod::METHOD_GET);
  HttpMethod post(HttpMethod::METHOD_POST);
  HttpMethod del(HttpMethod::METHOD_DELETE);
  
  EXPECT_TRUE(get.isGet());
  EXPECT_TRUE(post.isPost());
  EXPECT_TRUE(del.isDelete());
}

TEST_F(HttpMethodTest, ConstructCaseInsensitive) {
  HttpMethod upperCase("GET");
  HttpMethod lowerCase("get");
  HttpMethod mixedCase("Get");
  
  EXPECT_EQ(upperCase.getMethod(), lowerCase.getMethod());
  EXPECT_EQ(upperCase.getMethod(), mixedCase.getMethod());
}

TEST_F(HttpMethodTest, CopyConstructor) {
  HttpMethod original("GET");
  HttpMethod copy(original);
  
  EXPECT_EQ(original.getMethod(), copy.getMethod());
  EXPECT_EQ(original.toString(), copy.toString());
}

// ============================================================================
// Assignment Tests
// ============================================================================

TEST_F(HttpMethodTest, AssignmentOperator) {
  HttpMethod method1("GET");
  HttpMethod method2("POST");
  
  method2 = method1;
  EXPECT_EQ(method1.getMethod(), method2.getMethod());
}

TEST_F(HttpMethodTest, AssignmentOperatorSelfAssignment) {
  HttpMethod method("GET");
  method = method;
  EXPECT_TRUE(method.isGet());
}

// ============================================================================
// Comparison Tests
// ============================================================================

TEST_F(HttpMethodTest, EqualityOperator) {
  HttpMethod get1("GET");
  HttpMethod get2("GET");
  HttpMethod post("POST");
  
  EXPECT_TRUE(get1 == get2);
  EXPECT_FALSE(get1 == post);
}

TEST_F(HttpMethodTest, InequalityOperator) {
  HttpMethod get("GET");
  HttpMethod post("POST");
  
  EXPECT_TRUE(get != post);
  EXPECT_FALSE(get != HttpMethod("GET"));
}

TEST_F(HttpMethodTest, LessThanOperator) {
  HttpMethod get("GET");
  HttpMethod post("POST");
  
  EXPECT_TRUE(get < post);
  EXPECT_FALSE(post < get);
  EXPECT_FALSE(get < HttpMethod("GET"));
}

// ============================================================================
// Getter Tests
// ============================================================================

TEST_F(HttpMethodTest, GetMethodEnum) {
  HttpMethod get("GET");
  HttpMethod post("POST");
  HttpMethod del("DELETE");
  
  EXPECT_EQ(HttpMethod::METHOD_GET, get.getMethod());
  EXPECT_EQ(HttpMethod::METHOD_POST, post.getMethod());
  EXPECT_EQ(HttpMethod::METHOD_DELETE, del.getMethod());
}

TEST_F(HttpMethodTest, ToStringMethod) {
  HttpMethod get("GET");
  HttpMethod post("POST");
  
  EXPECT_EQ("GET", get.toString());
  EXPECT_EQ("POST", post.toString());
}

TEST_F(HttpMethodTest, ToUpperCaseString) {
  HttpMethod get("get");
  HttpMethod post("post");
  
  EXPECT_EQ("GET", get.toUpperCaseString());
  EXPECT_EQ("POST", post.toUpperCaseString());
}

// ============================================================================
// Method Type Check Tests
// ============================================================================

TEST_F(HttpMethodTest, IsGetMethod) {
  HttpMethod get("GET");
  HttpMethod post("POST");
  
  EXPECT_TRUE(get.isGet());
  EXPECT_FALSE(post.isGet());
}

TEST_F(HttpMethodTest, IsPostMethod) {
  HttpMethod post("POST");
  HttpMethod get("GET");
  
  EXPECT_TRUE(post.isPost());
  EXPECT_FALSE(get.isPost());
}

TEST_F(HttpMethodTest, IsPutMethod) {
  HttpMethod put("PUT");
  HttpMethod get("GET");
  
  EXPECT_TRUE(put.isPut());
  EXPECT_FALSE(get.isPut());
}

TEST_F(HttpMethodTest, IsDeleteMethod) {
  HttpMethod del("DELETE");
  HttpMethod get("GET");
  
  EXPECT_TRUE(del.isDelete());
  EXPECT_FALSE(get.isDelete());
}

TEST_F(HttpMethodTest, IsHeadMethod) {
  HttpMethod head("HEAD");
  HttpMethod get("GET");
  
  EXPECT_TRUE(head.isHead());
  EXPECT_FALSE(get.isHead());
}

TEST_F(HttpMethodTest, IsOptionsMethod) {
  HttpMethod options("OPTIONS");
  HttpMethod get("GET");
  
  EXPECT_TRUE(options.isOptions());
  EXPECT_FALSE(get.isOptions());
}

TEST_F(HttpMethodTest, IsTraceMethod) {
  HttpMethod trace("TRACE");
  HttpMethod get("GET");
  
  EXPECT_TRUE(trace.isTrace());
  EXPECT_FALSE(get.isTrace());
}

TEST_F(HttpMethodTest, IsConnectMethod) {
  HttpMethod connect("CONNECT");
  HttpMethod get("GET");
  
  EXPECT_TRUE(connect.isConnect());
  EXPECT_FALSE(get.isConnect());
}

TEST_F(HttpMethodTest, IsPatchMethod) {
  HttpMethod patch("PATCH");
  HttpMethod get("GET");
  
  EXPECT_TRUE(patch.isPatch());
  EXPECT_FALSE(get.isPatch());
}

// ============================================================================
// Validation Tests
// ============================================================================

TEST_F(HttpMethodTest, IsValidMethodString) {
  EXPECT_TRUE(HttpMethod::isValidMethodString("GET"));
  EXPECT_TRUE(HttpMethod::isValidMethodString("POST"));
  EXPECT_TRUE(HttpMethod::isValidMethodString("DELETE"));
  EXPECT_FALSE(HttpMethod::isValidMethodString("INVALID"));
  EXPECT_FALSE(HttpMethod::isValidMethodString(""));
}

// ============================================================================
// HTTP Semantics Tests - Safe Methods
// ============================================================================

TEST_F(HttpMethodTest, IsSafeMethod) {
  HttpMethod get("GET");
  HttpMethod head("HEAD");
  HttpMethod options("OPTIONS");
  HttpMethod post("POST");
  HttpMethod put("PUT");
  HttpMethod del("DELETE");
  
  EXPECT_TRUE(get.isSafe());
  EXPECT_TRUE(head.isSafe());
  EXPECT_TRUE(options.isSafe());
  EXPECT_FALSE(post.isSafe());
  EXPECT_FALSE(put.isSafe());
  EXPECT_FALSE(del.isSafe());
}

TEST_F(HttpMethodTest, StaticIsSafeMethod) {
  EXPECT_TRUE(HttpMethod::isSafeMethod(HttpMethod::METHOD_GET));
  EXPECT_TRUE(HttpMethod::isSafeMethod(HttpMethod::METHOD_HEAD));
  EXPECT_TRUE(HttpMethod::isSafeMethod(HttpMethod::METHOD_OPTIONS));
  EXPECT_FALSE(HttpMethod::isSafeMethod(HttpMethod::METHOD_POST));
  EXPECT_FALSE(HttpMethod::isSafeMethod(HttpMethod::METHOD_PUT));
  EXPECT_FALSE(HttpMethod::isSafeMethod(HttpMethod::METHOD_DELETE));
}

// ============================================================================
// HTTP Semantics Tests - Idempotent Methods
// ============================================================================

TEST_F(HttpMethodTest, IsIdempotentMethod) {
  HttpMethod get("GET");
  HttpMethod put("PUT");
  HttpMethod del("DELETE");
  HttpMethod post("POST");
  
  EXPECT_TRUE(get.isIdempotent());
  EXPECT_TRUE(put.isIdempotent());
  EXPECT_TRUE(del.isIdempotent());
  EXPECT_FALSE(post.isIdempotent());
}

TEST_F(HttpMethodTest, StaticIsIdempotentMethod) {
  EXPECT_TRUE(HttpMethod::isIdempotentMethod(HttpMethod::METHOD_GET));
  EXPECT_TRUE(HttpMethod::isIdempotentMethod(HttpMethod::METHOD_PUT));
  EXPECT_TRUE(HttpMethod::isIdempotentMethod(HttpMethod::METHOD_DELETE));
  EXPECT_FALSE(HttpMethod::isIdempotentMethod(HttpMethod::METHOD_POST));
}

// ============================================================================
// HTTP Semantics Tests - Cacheable Methods
// ============================================================================

TEST_F(HttpMethodTest, IsCacheableMethod) {
  HttpMethod get("GET");
  HttpMethod head("HEAD");
  HttpMethod post("POST");
  HttpMethod del("DELETE");
  
  EXPECT_TRUE(get.isCacheable());
  EXPECT_TRUE(head.isCacheable());
  EXPECT_TRUE(post.isCacheable());
  EXPECT_FALSE(del.isCacheable());
}

TEST_F(HttpMethodTest, StaticIsCacheableMethod) {
  EXPECT_TRUE(HttpMethod::isCacheableMethod(HttpMethod::METHOD_GET));
  EXPECT_TRUE(HttpMethod::isCacheableMethod(HttpMethod::METHOD_HEAD));
  EXPECT_TRUE(HttpMethod::isCacheableMethod(HttpMethod::METHOD_POST));
  EXPECT_FALSE(HttpMethod::isCacheableMethod(HttpMethod::METHOD_DELETE));
}

// ============================================================================
// HTTP Semantics Tests - Request/Response Body
// ============================================================================

TEST_F(HttpMethodTest, HasRequestBody) {
  HttpMethod post("POST");
  HttpMethod put("PUT");
  HttpMethod patch("PATCH");
  HttpMethod get("GET");
  HttpMethod del("DELETE");
  
  EXPECT_TRUE(post.hasRequestBody());
  EXPECT_TRUE(put.hasRequestBody());
  EXPECT_TRUE(patch.hasRequestBody());
  EXPECT_FALSE(get.hasRequestBody());
  EXPECT_FALSE(del.hasRequestBody());
}

TEST_F(HttpMethodTest, HasResponseBody) {
  HttpMethod get("GET");
  HttpMethod post("POST");
  HttpMethod head("HEAD");
  
  EXPECT_TRUE(get.hasResponseBody());
  EXPECT_TRUE(post.hasResponseBody());
  EXPECT_FALSE(head.hasResponseBody());
}

// ============================================================================
// Factory Method Tests
// ============================================================================

TEST_F(HttpMethodTest, FactoryGetMethod) {
  HttpMethod method = HttpMethod::get();
  EXPECT_TRUE(method.isGet());
  EXPECT_EQ("GET", method.toString());
}

TEST_F(HttpMethodTest, FactoryPostMethod) {
  HttpMethod method = HttpMethod::post();
  EXPECT_TRUE(method.isPost());
  EXPECT_EQ("POST", method.toString());
}

TEST_F(HttpMethodTest, FactoryPutMethod) {
  HttpMethod method = HttpMethod::put();
  EXPECT_TRUE(method.isPut());
  EXPECT_EQ("PUT", method.toString());
}

TEST_F(HttpMethodTest, FactoryDeleteMethod) {
  HttpMethod method = HttpMethod::deleteMethod();
  EXPECT_TRUE(method.isDelete());
  EXPECT_EQ("DELETE", method.toString());
}

TEST_F(HttpMethodTest, FactoryHeadMethod) {
  HttpMethod method = HttpMethod::head();
  EXPECT_TRUE(method.isHead());
  EXPECT_EQ("HEAD", method.toString());
}

// ============================================================================
// Edge Cases
// ============================================================================

TEST_F(HttpMethodTest, MethodStringsArray) {
  EXPECT_STREQ("GET", HttpMethod::METHOD_STRINGS[HttpMethod::METHOD_GET]);
  EXPECT_STREQ("POST", HttpMethod::METHOD_STRINGS[HttpMethod::METHOD_POST]);
  EXPECT_STREQ("DELETE", HttpMethod::METHOD_STRINGS[HttpMethod::METHOD_DELETE]);
}

TEST_F(HttpMethodTest, AllMethodsCovered) {
  HttpMethod get("GET");
  HttpMethod post("POST");
  HttpMethod put("PUT");
  HttpMethod del("DELETE");
  HttpMethod head("HEAD");
  HttpMethod options("OPTIONS");
  HttpMethod trace("TRACE");
  HttpMethod connect("CONNECT");
  HttpMethod patch("PATCH");
  
  EXPECT_NO_THROW(get.toString());
  EXPECT_NO_THROW(post.toString());
  EXPECT_NO_THROW(put.toString());
  EXPECT_NO_THROW(del.toString());
  EXPECT_NO_THROW(head.toString());
  EXPECT_NO_THROW(options.toString());
  EXPECT_NO_THROW(trace.toString());
  EXPECT_NO_THROW(connect.toString());
  EXPECT_NO_THROW(patch.toString());
}
