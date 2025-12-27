/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_ErrorCode.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: umeneses <umeneses@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/27 15:28:27 by umeneses          #+#    #+#             */
/*   Updated: 2025/12/27 15:28:31 by umeneses         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "domain/value_objects/ErrorCode.hpp"
#include "shared/exceptions/ErrorCodeException.hpp"
#include <gtest/gtest.h>

using namespace domain::value_objects;

class ErrorCodeTest : public ::testing::Test {
 protected:
  virtual void SetUp() {}
  virtual void TearDown() {}
};

// ============================================================================
// Construction Tests
// ============================================================================

TEST_F(ErrorCodeTest, DefaultConstructorShouldReturn200) {
  ErrorCode code;
  
  EXPECT_EQ(200, code.getValue());
  EXPECT_TRUE(code.isOk());
}

TEST_F(ErrorCodeTest, ConstructWithValidCodeShouldStore) {
  ErrorCode code(404);
  
  EXPECT_EQ(404, code.getValue());
  EXPECT_TRUE(code.isNotFound());
}

TEST_F(ErrorCodeTest, ConstructWithStringCodeShouldParse) {
  ErrorCode code("500");
  
  EXPECT_EQ(500, code.getValue());
  EXPECT_TRUE(code.isInternalServerError());
}

TEST_F(ErrorCodeTest, ConstructWithInvalidCodeShouldThrow) {
  EXPECT_THROW(ErrorCode(999), shared::exceptions::ErrorCodeException);
  EXPECT_THROW(ErrorCode(99), shared::exceptions::ErrorCodeException);
}

TEST_F(ErrorCodeTest, ConstructWithInvalidStringShouldThrow) {
  EXPECT_THROW(ErrorCode("abc"), shared::exceptions::ErrorCodeException);
  EXPECT_THROW(ErrorCode(""), shared::exceptions::ErrorCodeException);
}

TEST_F(ErrorCodeTest, CopyConstructorShouldCopyValue) {
  ErrorCode original(404);
  ErrorCode copy(original);
  
  EXPECT_EQ(original.getValue(), copy.getValue());
}

TEST_F(ErrorCodeTest, AssignmentOperatorShouldCopyValue) {
  ErrorCode original(500);
  ErrorCode assigned(200);
  
  assigned = original;
  
  EXPECT_EQ(500, assigned.getValue());
}

// ============================================================================
// Status Code Category Tests (Static Methods)
// ============================================================================

TEST_F(ErrorCodeTest, IsValidErrorCodeShouldDetectValidCodes) {
  EXPECT_TRUE(ErrorCode::isValidErrorCode(200));
  EXPECT_TRUE(ErrorCode::isValidErrorCode(404));
  EXPECT_TRUE(ErrorCode::isValidErrorCode(500));
  EXPECT_TRUE(ErrorCode::isValidErrorCode(100));
  EXPECT_TRUE(ErrorCode::isValidErrorCode(599));
}

TEST_F(ErrorCodeTest, IsValidErrorCodeShouldRejectInvalidCodes) {
  EXPECT_FALSE(ErrorCode::isValidErrorCode(99));
  EXPECT_FALSE(ErrorCode::isValidErrorCode(600));
  EXPECT_FALSE(ErrorCode::isValidErrorCode(0));
}

TEST_F(ErrorCodeTest, IsClientErrorShouldDetect4xxCodes) {
  EXPECT_TRUE(ErrorCode::isClientError(400));
  EXPECT_TRUE(ErrorCode::isClientError(404));
  EXPECT_TRUE(ErrorCode::isClientError(499));
  EXPECT_FALSE(ErrorCode::isClientError(200));
  EXPECT_FALSE(ErrorCode::isClientError(500));
}

TEST_F(ErrorCodeTest, IsServerErrorShouldDetect5xxCodes) {
  EXPECT_TRUE(ErrorCode::isServerError(500));
  EXPECT_TRUE(ErrorCode::isServerError(503));
  EXPECT_TRUE(ErrorCode::isServerError(599));
  EXPECT_FALSE(ErrorCode::isServerError(200));
  EXPECT_FALSE(ErrorCode::isServerError(400));
}

TEST_F(ErrorCodeTest, IsErrorCodeShouldDetect4xxAnd5xxCodes) {
  EXPECT_TRUE(ErrorCode::isErrorCode(400));
  EXPECT_TRUE(ErrorCode::isErrorCode(404));
  EXPECT_TRUE(ErrorCode::isErrorCode(500));
  EXPECT_TRUE(ErrorCode::isErrorCode(503));
  EXPECT_FALSE(ErrorCode::isErrorCode(200));
  EXPECT_FALSE(ErrorCode::isErrorCode(300));
}

TEST_F(ErrorCodeTest, IsSuccessCodeShouldDetect2xxCodes) {
  EXPECT_TRUE(ErrorCode::isSuccessCode(200));
  EXPECT_TRUE(ErrorCode::isSuccessCode(201));
  EXPECT_TRUE(ErrorCode::isSuccessCode(204));
  EXPECT_TRUE(ErrorCode::isSuccessCode(299));
  EXPECT_FALSE(ErrorCode::isSuccessCode(300));
  EXPECT_FALSE(ErrorCode::isSuccessCode(400));
}

TEST_F(ErrorCodeTest, IsRedirectionCodeShouldDetect3xxCodes) {
  EXPECT_TRUE(ErrorCode::isRedirectionCode(300));
  EXPECT_TRUE(ErrorCode::isRedirectionCode(301));
  EXPECT_TRUE(ErrorCode::isRedirectionCode(302));
  EXPECT_TRUE(ErrorCode::isRedirectionCode(399));
  EXPECT_FALSE(ErrorCode::isRedirectionCode(200));
  EXPECT_FALSE(ErrorCode::isRedirectionCode(400));
}

TEST_F(ErrorCodeTest, IsInformationalCodeShouldDetect1xxCodes) {
  EXPECT_TRUE(ErrorCode::isInformationalCode(100));
  EXPECT_TRUE(ErrorCode::isInformationalCode(101));
  EXPECT_TRUE(ErrorCode::isInformationalCode(199));
  EXPECT_FALSE(ErrorCode::isInformationalCode(200));
}

// ============================================================================
// Instance Category Tests
// ============================================================================

TEST_F(ErrorCodeTest, InstanceIsClientErrorShouldWork) {
  ErrorCode clientError(404);
  ErrorCode serverError(500);
  
  EXPECT_TRUE(clientError.isClientError());
  EXPECT_FALSE(serverError.isClientError());
}

TEST_F(ErrorCodeTest, InstanceIsServerErrorShouldWork) {
  ErrorCode serverError(500);
  ErrorCode clientError(404);
  
  EXPECT_TRUE(serverError.isServerError());
  EXPECT_FALSE(clientError.isServerError());
}

TEST_F(ErrorCodeTest, InstanceIsErrorShouldWork) {
  ErrorCode clientError(404);
  ErrorCode serverError(500);
  ErrorCode success(200);
  
  EXPECT_TRUE(clientError.isError());
  EXPECT_TRUE(serverError.isError());
  EXPECT_FALSE(success.isError());
}

TEST_F(ErrorCodeTest, InstanceIsSuccessShouldWork) {
  ErrorCode success(200);
  ErrorCode error(404);
  
  EXPECT_TRUE(success.isSuccess());
  EXPECT_FALSE(error.isSuccess());
}

TEST_F(ErrorCodeTest, InstanceIsRedirectionShouldWork) {
  ErrorCode redirect(301);
  ErrorCode success(200);
  
  EXPECT_TRUE(redirect.isRedirection());
  EXPECT_FALSE(success.isRedirection());
}

TEST_F(ErrorCodeTest, InstanceIsInformationalShouldWork) {
  ErrorCode info(100);
  ErrorCode success(200);
  
  EXPECT_TRUE(info.isInformational());
  EXPECT_FALSE(success.isInformational());
}

// ============================================================================
// Static Factory Methods Tests
// ============================================================================

TEST_F(ErrorCodeTest, BadRequestFactoryShouldReturn400) {
  ErrorCode code = ErrorCode::badRequest();
  
  EXPECT_EQ(400, code.getValue());
  EXPECT_TRUE(code.isBadRequest());
}

TEST_F(ErrorCodeTest, NotFoundFactoryShouldReturn404) {
  ErrorCode code = ErrorCode::notFound();
  
  EXPECT_EQ(404, code.getValue());
  EXPECT_TRUE(code.isNotFound());
}

TEST_F(ErrorCodeTest, InternalServerErrorFactoryShouldReturn500) {
  ErrorCode code = ErrorCode::internalServerError();
  
  EXPECT_EQ(500, code.getValue());
  EXPECT_TRUE(code.isInternalServerError());
}

TEST_F(ErrorCodeTest, OkFactoryShouldReturn200) {
  ErrorCode code = ErrorCode::ok();
  
  EXPECT_EQ(200, code.getValue());
  EXPECT_TRUE(code.isOk());
}

TEST_F(ErrorCodeTest, CreatedFactoryShouldReturn201) {
  ErrorCode code = ErrorCode::created();
  
  EXPECT_EQ(201, code.getValue());
  EXPECT_TRUE(code.isCreated());
}

TEST_F(ErrorCodeTest, MovedPermanentlyFactoryShouldReturn301) {
  ErrorCode code = ErrorCode::movedPermanently();
  
  EXPECT_EQ(301, code.getValue());
  EXPECT_TRUE(code.isMovedPermanently());
}

// ============================================================================
// String Parsing Tests (fromString)
// ============================================================================

TEST_F(ErrorCodeTest, FromStringShouldParseValidCode) {
  ErrorCode code = ErrorCode::fromString("404");
  
  EXPECT_EQ(404, code.getValue());
}

TEST_F(ErrorCodeTest, FromStringShouldThrowOnInvalidString) {
  EXPECT_THROW(ErrorCode::fromString("abc"), shared::exceptions::ErrorCodeException);
  EXPECT_THROW(ErrorCode::fromString(""), shared::exceptions::ErrorCodeException);
  EXPECT_THROW(ErrorCode::fromString("12.3"), shared::exceptions::ErrorCodeException);
}

TEST_F(ErrorCodeTest, FromStringShouldThrowOnOutOfRangeCode) {
  EXPECT_THROW(ErrorCode::fromString("999"), shared::exceptions::ErrorCodeException);
  EXPECT_THROW(ErrorCode::fromString("99"), shared::exceptions::ErrorCodeException);
}

// ============================================================================
// String Conversion Tests
// ============================================================================

TEST_F(ErrorCodeTest, ToStringShouldReturnCodeAsString) {
  ErrorCode code(404);
  
  EXPECT_EQ("404", code.toString());
}

TEST_F(ErrorCodeTest, GetDescriptionShouldReturnMessage) {
  ErrorCode notFound(404);
  ErrorCode ok(200);
  
  EXPECT_EQ("Not Found", notFound.getDescription());
  EXPECT_EQ("OK", ok.getDescription());
}

// ============================================================================
// Comparison Operators Tests
// ============================================================================

TEST_F(ErrorCodeTest, EqualityOperatorShouldCompareValues) {
  ErrorCode code1(404);
  ErrorCode code2(404);
  ErrorCode code3(500);
  
  EXPECT_TRUE(code1 == code2);
  EXPECT_FALSE(code1 == code3);
}

TEST_F(ErrorCodeTest, InequalityOperatorShouldCompareValues) {
  ErrorCode code1(404);
  ErrorCode code2(500);
  
  EXPECT_TRUE(code1 != code2);
  EXPECT_FALSE(code1 != ErrorCode(404));
}

TEST_F(ErrorCodeTest, LessThanOperatorShouldCompareValues) {
  ErrorCode code1(200);
  ErrorCode code2(404);
  
  EXPECT_TRUE(code1 < code2);
  EXPECT_FALSE(code2 < code1);
}

TEST_F(ErrorCodeTest, GreaterThanOperatorShouldCompareValues) {
  ErrorCode code1(500);
  ErrorCode code2(404);
  
  EXPECT_TRUE(code1 > code2);
  EXPECT_FALSE(code2 > code1);
}

// ============================================================================
// Specific Status Code Tests
// ============================================================================

TEST_F(ErrorCodeTest, IsBadRequestShouldDetect400) {
  ErrorCode code(400);
  
  EXPECT_TRUE(code.isBadRequest());
  EXPECT_FALSE(ErrorCode(404).isBadRequest());
}

TEST_F(ErrorCodeTest, IsNotFoundShouldDetect404) {
  ErrorCode code(404);
  
  EXPECT_TRUE(code.isNotFound());
  EXPECT_FALSE(ErrorCode(400).isNotFound());
}

TEST_F(ErrorCodeTest, IsMethodNotAllowedShouldDetect405) {
  ErrorCode code(405);
  
  EXPECT_TRUE(code.isMethodNotAllowed());
  EXPECT_FALSE(ErrorCode(404).isMethodNotAllowed());
}

TEST_F(ErrorCodeTest, IsInternalServerErrorShouldDetect500) {
  ErrorCode code(500);
  
  EXPECT_TRUE(code.isInternalServerError());
  EXPECT_FALSE(ErrorCode(404).isInternalServerError());
}

TEST_F(ErrorCodeTest, IsServiceUnavailableShouldDetect503) {
  ErrorCode code(503);
  
  EXPECT_TRUE(code.isServiceUnavailable());
  EXPECT_FALSE(ErrorCode(500).isServiceUnavailable());
}

TEST_F(ErrorCodeTest, AllCommonStatusCodesShouldHaveDescriptions) {
  // 2xx Success
  EXPECT_FALSE(ErrorCode::ok().getDescription().empty());
  EXPECT_FALSE(ErrorCode::created().getDescription().empty());
  EXPECT_FALSE(ErrorCode::noContent().getDescription().empty());
  
  // 3xx Redirection
  EXPECT_FALSE(ErrorCode::movedPermanently().getDescription().empty());
  EXPECT_FALSE(ErrorCode::found().getDescription().empty());
  EXPECT_FALSE(ErrorCode::notModified().getDescription().empty());
  
  // 4xx Client Errors
  EXPECT_FALSE(ErrorCode::badRequest().getDescription().empty());
  EXPECT_FALSE(ErrorCode::unauthorized().getDescription().empty());
  EXPECT_FALSE(ErrorCode::forbidden().getDescription().empty());
  EXPECT_FALSE(ErrorCode::notFound().getDescription().empty());
  EXPECT_FALSE(ErrorCode::methodNotAllowed().getDescription().empty());
  
  // 5xx Server Errors
  EXPECT_FALSE(ErrorCode::internalServerError().getDescription().empty());
  EXPECT_FALSE(ErrorCode::serviceUnavailable().getDescription().empty());
}
