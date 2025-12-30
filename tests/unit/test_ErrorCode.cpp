/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_ErrorCode.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: umeneses <umeneses@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/27 15:28:27 by umeneses          #+#    #+#             */
/*   Updated: 2025/12/29 21:41:37 by umeneses         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "domain/shared/value_objects/ErrorCode.hpp"
#include "domain/shared/exceptions/ErrorCodeException.hpp"
#include <gtest/gtest.h>

using namespace domain::shared::value_objects;

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
  EXPECT_THROW(ErrorCode(999), domain::shared::exceptions::ErrorCodeException);
  EXPECT_THROW(ErrorCode(99), domain::shared::exceptions::ErrorCodeException);
}

TEST_F(ErrorCodeTest, ConstructWithInvalidStringShouldThrow) {
  EXPECT_THROW(ErrorCode("abc"), domain::shared::exceptions::ErrorCodeException);
  EXPECT_THROW(ErrorCode(""), domain::shared::exceptions::ErrorCodeException);
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
  EXPECT_THROW(ErrorCode::fromString("abc"), domain::shared::exceptions::ErrorCodeException);
  EXPECT_THROW(ErrorCode::fromString(""), domain::shared::exceptions::ErrorCodeException);
  EXPECT_THROW(ErrorCode::fromString("12.3"), domain::shared::exceptions::ErrorCodeException);
}

TEST_F(ErrorCodeTest, FromStringShouldThrowOnOutOfRangeCode) {
  EXPECT_THROW(ErrorCode::fromString("999"), domain::shared::exceptions::ErrorCodeException);
  EXPECT_THROW(ErrorCode::fromString("99"), domain::shared::exceptions::ErrorCodeException);
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

TEST_F(ErrorCodeTest, LessThanOrEqualOperatorShouldCompareValues) {
  ErrorCode code1(200);
  ErrorCode code2(404);
  ErrorCode code3(404);
  
  EXPECT_TRUE(code1 <= code2);
  EXPECT_TRUE(code2 <= code3);
  EXPECT_FALSE(code2 <= code1);
}

TEST_F(ErrorCodeTest, GreaterThanOrEqualOperatorShouldCompareValues) {
  ErrorCode code1(500);
  ErrorCode code2(404);
  ErrorCode code3(404);
  
  EXPECT_TRUE(code1 >= code2);
  EXPECT_TRUE(code2 >= code3);
  EXPECT_FALSE(code2 >= code1);
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

TEST_F(ErrorCodeTest, IsUnauthorizedShouldDetect401) {
  ErrorCode code(401);
  
  EXPECT_TRUE(code.isUnauthorized());
  EXPECT_FALSE(ErrorCode(400).isUnauthorized());
}

TEST_F(ErrorCodeTest, IsForbiddenShouldDetect403) {
  ErrorCode code(403);
  
  EXPECT_TRUE(code.isForbidden());
  EXPECT_FALSE(ErrorCode(404).isForbidden());
}

TEST_F(ErrorCodeTest, IsRequestTimeoutShouldDetect408) {
  ErrorCode code(408);
  
  EXPECT_TRUE(code.isRequestTimeout());
  EXPECT_FALSE(ErrorCode(404).isRequestTimeout());
}

TEST_F(ErrorCodeTest, IsConflictShouldDetect409) {
  ErrorCode code(409);
  
  EXPECT_TRUE(code.isConflict());
  EXPECT_FALSE(ErrorCode(404).isConflict());
}

TEST_F(ErrorCodeTest, IsPayloadTooLargeShouldDetect413) {
  ErrorCode code(413);
  
  EXPECT_TRUE(code.isPayloadTooLarge());
  EXPECT_FALSE(ErrorCode(404).isPayloadTooLarge());
}

TEST_F(ErrorCodeTest, IsFoundShouldDetect302) {
  ErrorCode code(302);
  
  EXPECT_TRUE(code.isFound());
  EXPECT_FALSE(ErrorCode(301).isFound());
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

// ============================================================================
// Internal Validation Tests (through public API)
// ============================================================================

TEST_F(ErrorCodeTest, ValidateShouldRejectCodeBelow100) {
  EXPECT_THROW(ErrorCode(99), domain::shared::exceptions::ErrorCodeException);
  EXPECT_THROW(ErrorCode(0), domain::shared::exceptions::ErrorCodeException);
  EXPECT_THROW(ErrorCode(-1), domain::shared::exceptions::ErrorCodeException);
}

TEST_F(ErrorCodeTest, ValidateShouldAcceptCode100) {
  EXPECT_NO_THROW(ErrorCode(100));
  ErrorCode code(100);
  EXPECT_EQ(100, code.getValue());
}

TEST_F(ErrorCodeTest, ValidateShouldAcceptCode599) {
  EXPECT_NO_THROW(ErrorCode(599));
  ErrorCode code(599);
  EXPECT_EQ(599, code.getValue());
}

TEST_F(ErrorCodeTest, ValidateShouldRejectCode600AndAbove) {
  EXPECT_THROW(ErrorCode(600), domain::shared::exceptions::ErrorCodeException);
  EXPECT_THROW(ErrorCode(1000), domain::shared::exceptions::ErrorCodeException);
  EXPECT_THROW(ErrorCode(9999), domain::shared::exceptions::ErrorCodeException);
}

// parseCodeString() and isAllDigits() testing through string constructor
TEST_F(ErrorCodeTest, ParseCodeStringShouldRejectNonNumeric) {
  EXPECT_THROW(ErrorCode("abc"), domain::shared::exceptions::ErrorCodeException);
  EXPECT_THROW(ErrorCode("12abc"), domain::shared::exceptions::ErrorCodeException);
  EXPECT_THROW(ErrorCode("ab12"), domain::shared::exceptions::ErrorCodeException);
}

TEST_F(ErrorCodeTest, ParseCodeStringShouldRejectFloatNotation) {
  EXPECT_THROW(ErrorCode("12.34"), domain::shared::exceptions::ErrorCodeException);
  EXPECT_THROW(ErrorCode("200.0"), domain::shared::exceptions::ErrorCodeException);
  EXPECT_THROW(ErrorCode("4.04"), domain::shared::exceptions::ErrorCodeException);
}

TEST_F(ErrorCodeTest, ParseCodeStringShouldRejectEmptyString) {
  EXPECT_THROW(ErrorCode(""), domain::shared::exceptions::ErrorCodeException);
}

TEST_F(ErrorCodeTest, ParseCodeStringShouldRejectWhitespaceOnly) {
  EXPECT_THROW(ErrorCode("   "), domain::shared::exceptions::ErrorCodeException);
  EXPECT_THROW(ErrorCode("\t"), domain::shared::exceptions::ErrorCodeException);
  EXPECT_THROW(ErrorCode("\n"), domain::shared::exceptions::ErrorCodeException);
}

TEST_F(ErrorCodeTest, ParseCodeStringShouldRejectLeadingWhitespace) {
  EXPECT_THROW(ErrorCode("  200"), domain::shared::exceptions::ErrorCodeException);
  EXPECT_THROW(ErrorCode("\t404"), domain::shared::exceptions::ErrorCodeException);
}

TEST_F(ErrorCodeTest, ParseCodeStringShouldRejectTrailingWhitespace) {
  EXPECT_THROW(ErrorCode("200  "), domain::shared::exceptions::ErrorCodeException);
  EXPECT_THROW(ErrorCode("404\n"), domain::shared::exceptions::ErrorCodeException);
}

TEST_F(ErrorCodeTest, ParseCodeStringShouldRejectSurroundedByWhitespace) {
  EXPECT_THROW(ErrorCode("  200  "), domain::shared::exceptions::ErrorCodeException);
}

TEST_F(ErrorCodeTest, ParseCodeStringShouldRejectAlphanumericMix) {
  EXPECT_THROW(ErrorCode("200x"), domain::shared::exceptions::ErrorCodeException);
  EXPECT_THROW(ErrorCode("x200"), domain::shared::exceptions::ErrorCodeException);
  EXPECT_THROW(ErrorCode("2x00"), domain::shared::exceptions::ErrorCodeException);
}

TEST_F(ErrorCodeTest, ParseCodeStringShouldRejectSpecialCharacters) {
  EXPECT_THROW(ErrorCode("200!"), domain::shared::exceptions::ErrorCodeException);
  EXPECT_THROW(ErrorCode("200@"), domain::shared::exceptions::ErrorCodeException);
  EXPECT_THROW(ErrorCode("200#"), domain::shared::exceptions::ErrorCodeException);
  EXPECT_THROW(ErrorCode("200$"), domain::shared::exceptions::ErrorCodeException);
}

TEST_F(ErrorCodeTest, ParseCodeStringShouldHandleLeadingZeros) {
  // Leading zeros should either be accepted (and parsed correctly) or rejected
  // Testing current behavior - adjust expectation based on implementation
  ErrorCode code("0200");
  EXPECT_EQ(200, code.getValue());
}

TEST_F(ErrorCodeTest, ParseCodeStringShouldRejectNegativeSign) {
  EXPECT_THROW(ErrorCode("-200"), domain::shared::exceptions::ErrorCodeException);
  EXPECT_THROW(ErrorCode("-404"), domain::shared::exceptions::ErrorCodeException);
}

TEST_F(ErrorCodeTest, ParseCodeStringShouldRejectPlusSign) {
  EXPECT_THROW(ErrorCode("+200"), domain::shared::exceptions::ErrorCodeException);
}

TEST_F(ErrorCodeTest, IsAllDigitsShouldAcceptValidNumericStrings) {
  // Testing through successful string construction
  EXPECT_NO_THROW(ErrorCode("200"));
  EXPECT_NO_THROW(ErrorCode("404"));
  EXPECT_NO_THROW(ErrorCode("500"));
}

// getDescription() comprehensive testing for all status codes
TEST_F(ErrorCodeTest, GetDescriptionShouldReturnNonEmptyFor1xxCodes) {
  ErrorCode code100(100);
  ErrorCode code101(101);
  
  EXPECT_FALSE(code100.getDescription().empty());
  EXPECT_FALSE(code101.getDescription().empty());
}

TEST_F(ErrorCodeTest, GetDescriptionShouldReturnNonEmptyFor2xxCodes) {
  EXPECT_FALSE(ErrorCode(200).getDescription().empty());
  EXPECT_FALSE(ErrorCode(201).getDescription().empty());
  EXPECT_FALSE(ErrorCode(204).getDescription().empty());
}

TEST_F(ErrorCodeTest, GetDescriptionShouldReturnNonEmptyFor3xxCodes) {
  EXPECT_FALSE(ErrorCode(301).getDescription().empty());
  EXPECT_FALSE(ErrorCode(302).getDescription().empty());
  EXPECT_FALSE(ErrorCode(304).getDescription().empty());
}

TEST_F(ErrorCodeTest, GetDescriptionShouldReturnNonEmptyFor4xxCodes) {
  EXPECT_FALSE(ErrorCode(400).getDescription().empty());
  EXPECT_FALSE(ErrorCode(401).getDescription().empty());
  EXPECT_FALSE(ErrorCode(403).getDescription().empty());
  EXPECT_FALSE(ErrorCode(404).getDescription().empty());
  EXPECT_FALSE(ErrorCode(405).getDescription().empty());
  EXPECT_FALSE(ErrorCode(409).getDescription().empty());
  EXPECT_FALSE(ErrorCode(413).getDescription().empty());
}

TEST_F(ErrorCodeTest, GetDescriptionShouldReturnNonEmptyFor5xxCodes) {
  EXPECT_FALSE(ErrorCode(500).getDescription().empty());
  EXPECT_FALSE(ErrorCode(503).getDescription().empty());
}

// Status code categorization boundary testing
TEST_F(ErrorCodeTest, CategoryBoundaryShouldDistinguish199And200) {
  ErrorCode code199(199);
  ErrorCode code200(200);
  
  EXPECT_TRUE(code199.isInformational());
  EXPECT_FALSE(code199.isSuccess());
  
  EXPECT_FALSE(code200.isInformational());
  EXPECT_TRUE(code200.isSuccess());
}

TEST_F(ErrorCodeTest, CategoryBoundaryShouldDistinguish299And300) {
  ErrorCode code299(299);
  ErrorCode code300(300);
  
  EXPECT_TRUE(code299.isSuccess());
  EXPECT_FALSE(code299.isRedirection());
  
  EXPECT_FALSE(code300.isSuccess());
  EXPECT_TRUE(code300.isRedirection());
}

TEST_F(ErrorCodeTest, CategoryBoundaryShouldDistinguish399And400) {
  ErrorCode code399(399);
  ErrorCode code400(400);
  
  EXPECT_TRUE(code399.isRedirection());
  EXPECT_FALSE(code399.isClientError());
  
  EXPECT_FALSE(code400.isRedirection());
  EXPECT_TRUE(code400.isClientError());
}

TEST_F(ErrorCodeTest, CategoryBoundaryShouldDistinguish499And500) {
  ErrorCode code499(499);
  ErrorCode code500(500);
  
  EXPECT_TRUE(code499.isClientError());
  EXPECT_FALSE(code499.isServerError());
  
  EXPECT_FALSE(code500.isClientError());
  EXPECT_TRUE(code500.isServerError());
}

TEST_F(ErrorCodeTest, CategoryBoundaryShouldDistinguish599And600) {
  ErrorCode code599(599);
  
  EXPECT_TRUE(code599.isServerError());
  
  // 600 should be invalid
  EXPECT_THROW(ErrorCode(600), domain::shared::exceptions::ErrorCodeException);
}
