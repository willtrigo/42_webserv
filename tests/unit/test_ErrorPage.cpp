/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_ErrorPage.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: umeneses <umeneses@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/30 08:59:36 by umeneses          #+#    #+#             */
/*   Updated: 2025/12/30 09:30:46 by umeneses         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <gtest/gtest.h>
#include "domain/configuration/value_objects/ErrorPage.hpp"
#include "domain/configuration/exceptions/ErrorPageException.hpp"

using namespace domain::configuration::entities;
using namespace domain::configuration::exceptions;

class ErrorPageTest : public ::testing::Test {
 protected:
  void SetUp() {}
  void TearDown() {}
};

// ============================================================================
// Construction Tests
// ============================================================================

TEST_F(ErrorPageTest, DefaultConstructor) {
  ErrorPage errorPage;
  EXPECT_EQ(500, errorPage.getErrorCode().getValue());
  EXPECT_FALSE(errorPage.isEmpty());  // Should generate default content
  EXPECT_TRUE(errorPage.hasContent());  // Should have content
  EXPECT_TRUE(errorPage.isDefault());  // Default constructor creates default page
}

TEST_F(ErrorPageTest, ConstructWithErrorCode) {
  ErrorPage errorPage(domain::shared::value_objects::ErrorCode::notFound());
  EXPECT_EQ(404, errorPage.getErrorCode().getValue());
  EXPECT_FALSE(errorPage.isEmpty());
  EXPECT_TRUE(errorPage.hasContent());
  EXPECT_FALSE(errorPage.hasFile());
}

TEST_F(ErrorPageTest, ConstructWithErrorCodeAndContent) {
  ErrorPage errorPage(domain::shared::value_objects::ErrorCode::notFound(), "<h1>Not Found</h1>");
  EXPECT_EQ(404, errorPage.getErrorCode().getValue());
  EXPECT_EQ("<h1>Not Found</h1>", errorPage.getContent());
  EXPECT_TRUE(errorPage.hasContent());
  EXPECT_FALSE(errorPage.hasFile());
}

TEST_F(ErrorPageTest, ConstructWithErrorCodeAndFilePath) {
  domain::filesystem::value_objects::Path path("/var/www/errors/404.html");
  ErrorPage* ep = NULL;
  EXPECT_THROW(ep = new ErrorPage(domain::shared::value_objects::ErrorCode::notFound(), path), ErrorPageException);
  if (ep) delete ep;
}

TEST_F(ErrorPageTest, ConstructWithErrorCodeContentAndType) {
  ErrorPage errorPage(domain::shared::value_objects::ErrorCode::notFound(), 
                      "<h1>Not Found</h1>", "text/html");
  EXPECT_EQ(404, errorPage.getErrorCode().getValue());
  EXPECT_EQ("<h1>Not Found</h1>", errorPage.getContent());
  EXPECT_EQ("text/html", errorPage.getContentType());
}

TEST_F(ErrorPageTest, ConstructWithEmptyContent) {
  // Empty content should throw - ErrorPage must have content or file
  ErrorPage* ep = NULL;
  EXPECT_THROW(ep = new ErrorPage(domain::shared::value_objects::ErrorCode::notFound(), ""), ErrorPageException);
  if (ep) delete ep;
}

TEST_F(ErrorPageTest, CopyConstructor) {
  ErrorPage original(domain::shared::value_objects::ErrorCode::notFound(), "<h1>Not Found</h1>");
  ErrorPage copy(original);
  EXPECT_EQ(original.getErrorCode().getValue(), copy.getErrorCode().getValue());
  EXPECT_EQ(original.getContent(), copy.getContent());
}

// ============================================================================
// Getter Tests
// ============================================================================

TEST_F(ErrorPageTest, GetErrorCode) {
  ErrorPage errorPage(domain::shared::value_objects::ErrorCode::badRequest());
  EXPECT_EQ(400, errorPage.getErrorCode().getValue());
}

TEST_F(ErrorPageTest, GetContent) {
  std::string content = "<h1>Server Error</h1>";
  ErrorPage errorPage(domain::shared::value_objects::ErrorCode::internalServerError(), content);
  EXPECT_EQ(content, errorPage.getContent());
}

TEST_F(ErrorPageTest, GetContentType) {
  ErrorPage errorPage(domain::shared::value_objects::ErrorCode::notFound(), 
                      "Not found", "text/plain");
  EXPECT_EQ("text/plain", errorPage.getContentType());
}

TEST_F(ErrorPageTest, GetContentSize) {
  std::string content = "<h1>Test</h1>";
  ErrorPage errorPage(domain::shared::value_objects::ErrorCode::notFound(), content);
  EXPECT_EQ(content.size(), errorPage.getContentSize().getBytes());
}

TEST_F(ErrorPageTest, GetContentSizeEmpty) {
  ErrorPage errorPage(domain::shared::value_objects::ErrorCode::notFound());
  EXPECT_GT(errorPage.getContentSize().getBytes(), 0u);  // Default content exists
}

// ============================================================================
// Setters Tests
// ============================================================================

TEST_F(ErrorPageTest, SetContent) {
  ErrorPage errorPage(domain::shared::value_objects::ErrorCode::notFound());
  std::string newContent = "<h1>New Content</h1>";
  errorPage.setContent(newContent);
  EXPECT_EQ(newContent, errorPage.getContent());
  EXPECT_TRUE(errorPage.hasContent());
  EXPECT_FALSE(errorPage.hasFile());
}

TEST_F(ErrorPageTest, SetContentType) {
  ErrorPage errorPage(domain::shared::value_objects::ErrorCode::notFound(), "Error");
  errorPage.setContentType("text/plain");
  EXPECT_EQ("text/plain", errorPage.getContentType());
}

TEST_F(ErrorPageTest, SetContentTypeInvalid) {
  ErrorPage errorPage(domain::shared::value_objects::ErrorCode::notFound(), "Error");
  EXPECT_THROW(errorPage.setContentType("invalid/type"), ErrorPageException);
}

TEST_F(ErrorPageTest, SetErrorCode) {
  ErrorPage errorPage(domain::shared::value_objects::ErrorCode::notFound());
  errorPage.setErrorCode(domain::shared::value_objects::ErrorCode::badRequest());
  EXPECT_EQ(400, errorPage.getErrorCode().getValue());
}

// ============================================================================
// Assignment Operator Tests
// ============================================================================

TEST_F(ErrorPageTest, AssignmentOperator) {
  ErrorPage original(domain::shared::value_objects::ErrorCode::notFound(), "<h1>Original</h1>");
  ErrorPage assigned(domain::shared::value_objects::ErrorCode::badRequest(), "<h1>Assigned</h1>");
  
  assigned = original;
  
  EXPECT_EQ(original.getErrorCode().getValue(), assigned.getErrorCode().getValue());
  EXPECT_EQ(original.getContent(), assigned.getContent());
}

TEST_F(ErrorPageTest, SelfAssignment) {
  ErrorPage errorPage(domain::shared::value_objects::ErrorCode::notFound(), "<h1>Test</h1>");
  errorPage = errorPage;
  EXPECT_EQ(404, errorPage.getErrorCode().getValue());
  EXPECT_EQ("<h1>Test</h1>", errorPage.getContent());
}

// ============================================================================
// toString Tests
// ============================================================================

TEST_F(ErrorPageTest, ToStringContainsErrorCode) {
  ErrorPage errorPage(domain::shared::value_objects::ErrorCode::notFound());
  std::string str = errorPage.toString();
  EXPECT_NE(std::string::npos, str.find("404"));
}

TEST_F(ErrorPageTest, ToStringContainsContentType) {
  ErrorPage errorPage(domain::shared::value_objects::ErrorCode::notFound(), "Error", "text/plain");
  std::string str = errorPage.toString();
  EXPECT_NE(std::string::npos, str.find("text/plain"));
}

TEST_F(ErrorPageTest, ToStringContainsSize) {
  ErrorPage errorPage(domain::shared::value_objects::ErrorCode::notFound(), "Error");
  std::string str = errorPage.toString();
  EXPECT_NE(std::string::npos, str.find("size="));
}

// ============================================================================
// State Query Tests
// ============================================================================

TEST_F(ErrorPageTest, HasContent) {
  ErrorPage errorPage(domain::shared::value_objects::ErrorCode::notFound(), "<h1>Content</h1>");
  EXPECT_TRUE(errorPage.hasContent());
  EXPECT_FALSE(errorPage.hasFile());
}

TEST_F(ErrorPageTest, IsEmpty) {
  ErrorPage errorPage(domain::shared::value_objects::ErrorCode::notFound());
  EXPECT_FALSE(errorPage.isEmpty());  // Default content exists
}

TEST_F(ErrorPageTest, IsDefault) {
  ErrorPage defaultPage(domain::shared::value_objects::ErrorCode::notFound());
  EXPECT_TRUE(defaultPage.isDefault());
  
  ErrorPage customPage(domain::shared::value_objects::ErrorCode::notFound(), "Custom");
  EXPECT_FALSE(customPage.isDefault());
}

// ============================================================================
// Response Builder Tests
// ============================================================================

TEST_F(ErrorPageTest, BuildResponseContainsStatusLine) {
  ErrorPage errorPage(domain::shared::value_objects::ErrorCode::notFound(), "Not Found");
  std::string response = errorPage.buildResponse();
  EXPECT_NE(std::string::npos, response.find("HTTP/1.1 404"));
}

TEST_F(ErrorPageTest, BuildResponseContainsContentType) {
  ErrorPage errorPage(domain::shared::value_objects::ErrorCode::notFound(), "Error", "text/plain");
  std::string response = errorPage.buildResponse();
  EXPECT_NE(std::string::npos, response.find("Content-Type: text/plain"));
}

TEST_F(ErrorPageTest, BuildResponseContainsContentLength) {
  std::string content = "Not Found";
  ErrorPage errorPage(domain::shared::value_objects::ErrorCode::notFound(), content);
  std::string response = errorPage.buildResponse();
  EXPECT_NE(std::string::npos, response.find("Content-Length:"));
}

TEST_F(ErrorPageTest, BuildResponseContainsBody) {
  std::string content = "<h1>Not Found</h1>";
  ErrorPage errorPage(domain::shared::value_objects::ErrorCode::notFound(), content);
  std::string response = errorPage.buildResponse();
  EXPECT_NE(std::string::npos, response.find(content));
}

TEST_F(ErrorPageTest, BuildHtmlResponse) {
  ErrorPage errorPage(domain::shared::value_objects::ErrorCode::notFound(), "Error");
  std::string response = errorPage.buildHtmlResponse();
  EXPECT_NE(std::string::npos, response.find("text/html"));
  EXPECT_NE(std::string::npos, response.find("<!DOCTYPE html>"));
}

TEST_F(ErrorPageTest, BuildPlainTextResponse) {
  ErrorPage errorPage(domain::shared::value_objects::ErrorCode::notFound(), "Error");
  std::string response = errorPage.buildPlainTextResponse();
  EXPECT_NE(std::string::npos, response.find("text/plain"));
}

// ============================================================================
// Default Error Page Generation Tests
// ============================================================================

TEST_F(ErrorPageTest, CreateDefault) {
  ErrorPage errorPage = ErrorPage::createDefault(domain::shared::value_objects::ErrorCode::notFound());
  EXPECT_EQ(404, errorPage.getErrorCode().getValue());
  EXPECT_FALSE(errorPage.getContent().empty());
  EXPECT_TRUE(errorPage.isDefault());
}

TEST_F(ErrorPageTest, CreateHtmlDefault) {
  ErrorPage errorPage = ErrorPage::createHtmlDefault(domain::shared::value_objects::ErrorCode::internalServerError());
  EXPECT_EQ(500, errorPage.getErrorCode().getValue());
  EXPECT_EQ("text/html", errorPage.getContentType());
  EXPECT_NE(std::string::npos, errorPage.getContent().find("<!DOCTYPE html>"));
}

TEST_F(ErrorPageTest, CreatePlainTextDefault) {
  ErrorPage errorPage = ErrorPage::createPlainTextDefault(domain::shared::value_objects::ErrorCode::badRequest());
  EXPECT_EQ(400, errorPage.getErrorCode().getValue());
  EXPECT_EQ("text/plain", errorPage.getContentType());
}

TEST_F(ErrorPageTest, CreateDefaultErrorPages) {
  std::map<domain::shared::value_objects::ErrorCode, ErrorPage> errorPages = 
      ErrorPage::createDefaultErrorPages();
  EXPECT_FALSE(errorPages.empty());
  EXPECT_GE(errorPages.size(), 5u);  // At least 5 common error codes
}

TEST_F(ErrorPageTest, CreateDefaultErrorPagesByCode) {
  std::map<unsigned int, ErrorPage> errorPages = ErrorPage::createDefaultErrorPagesByCode();
  EXPECT_FALSE(errorPages.empty());
  
  // Check for common error codes
  EXPECT_NE(errorPages.find(404), errorPages.end());
  EXPECT_NE(errorPages.find(500), errorPages.end());
}

// ============================================================================
// Factory Method Tests
// ============================================================================

TEST_F(ErrorPageTest, FromContent) {
  std::string content = "<h1>Custom Error</h1>";
  ErrorPage errorPage = ErrorPage::fromContent(content, domain::shared::value_objects::ErrorCode::notFound());
  EXPECT_EQ(404, errorPage.getErrorCode().getValue());
  EXPECT_EQ(content, errorPage.getContent());
}

TEST_F(ErrorPageTest, FromString) {
  std::string errorPageStr = "404|<h1>Not Found</h1>";
  ErrorPage errorPage = ErrorPage::fromString(errorPageStr);
  EXPECT_EQ(404, errorPage.getErrorCode().getValue());
  EXPECT_EQ("<h1>Not Found</h1>", errorPage.getContent());
}

TEST_F(ErrorPageTest, FromStringInvalidFormat) {
  EXPECT_THROW(ErrorPage::fromString("invalid"), ErrorPageException);
  EXPECT_THROW(ErrorPage::fromString("404"), ErrorPageException);
  EXPECT_THROW(ErrorPage::fromString(""), ErrorPageException);
}

// ============================================================================
// Comparison Operator Tests
// ============================================================================

TEST_F(ErrorPageTest, EqualityOperator) {
  ErrorPage page1(domain::shared::value_objects::ErrorCode::notFound(), "Error");
  ErrorPage page2(domain::shared::value_objects::ErrorCode::notFound(), "Error");
  ErrorPage page3(domain::shared::value_objects::ErrorCode::badRequest(), "Error");
  
  EXPECT_TRUE(page1 == page2);
  EXPECT_FALSE(page1 == page3);
}

TEST_F(ErrorPageTest, InequalityOperator) {
  ErrorPage page1(domain::shared::value_objects::ErrorCode::notFound(), "Error");
  ErrorPage page2(domain::shared::value_objects::ErrorCode::badRequest(), "Error");
  
  EXPECT_TRUE(page1 != page2);
}

TEST_F(ErrorPageTest, LessThanOperator) {
  ErrorPage page1(domain::shared::value_objects::ErrorCode::badRequest(), "Error");
  ErrorPage page2(domain::shared::value_objects::ErrorCode::notFound(), "Error");
  
  EXPECT_TRUE(page1 < page2);
  EXPECT_FALSE(page2 < page1);
}

// ============================================================================
// Edge Case Tests
// ============================================================================

TEST_F(ErrorPageTest, ContentTooLarge) {
  std::string largeContent(ErrorPage::MAX_CONTENT_LENGTH + 1, 'x');
  ErrorPage* ep = NULL;
  EXPECT_THROW(ep = new ErrorPage(domain::shared::value_objects::ErrorCode::notFound(), largeContent), 
               ErrorPageException);
  if (ep) delete ep;
}

TEST_F(ErrorPageTest, ContentAtMaxLength) {
  std::string maxContent(ErrorPage::MAX_CONTENT_LENGTH, 'x');
  EXPECT_NO_THROW(ErrorPage(domain::shared::value_objects::ErrorCode::notFound(), maxContent));
}

TEST_F(ErrorPageTest, SpecialCharactersInContent) {
  std::string specialContent = "<h1>Error</h1>\n\r\t\"'&<>";
  ErrorPage errorPage(domain::shared::value_objects::ErrorCode::notFound(), specialContent);
  EXPECT_EQ(specialContent, errorPage.getContent());
}

TEST_F(ErrorPageTest, UnicodeInContent) {
  std::string unicodeContent = "<h1>Erreur 404</h1> - Página não encontrada - エラー";
  ErrorPage errorPage(domain::shared::value_objects::ErrorCode::notFound(), unicodeContent);
  EXPECT_EQ(unicodeContent, errorPage.getContent());
}

TEST_F(ErrorPageTest, AllCommonErrorCodes) {
  unsigned int errorCodes[] = {400, 401, 403, 404, 405, 408, 409, 413, 500, 503};
  for (size_t i = 0; i < sizeof(errorCodes) / sizeof(errorCodes[0]); ++i) {
    ErrorPage errorPage(domain::shared::value_objects::ErrorCode(errorCodes[i]));
    EXPECT_EQ(errorCodes[i], errorPage.getErrorCode().getValue());
    EXPECT_FALSE(errorPage.getContent().empty());
  }
}

// ============================================================================
// Static Validation Method Tests
// ============================================================================

TEST_F(ErrorPageTest, IsValidErrorPage) {
  EXPECT_TRUE(ErrorPage::isValidErrorPage("<h1>Error</h1>"));
  EXPECT_TRUE(ErrorPage::isValidErrorPage(""));  // Empty content is valid for validation
  
  std::string tooLarge(ErrorPage::MAX_CONTENT_LENGTH + 1, 'x');
  EXPECT_FALSE(ErrorPage::isValidErrorPage(tooLarge));
}

TEST_F(ErrorPageTest, IsValidContentType) {
  EXPECT_TRUE(ErrorPage::isValidContentType("text/html"));
  EXPECT_TRUE(ErrorPage::isValidContentType("text/plain"));
  EXPECT_TRUE(ErrorPage::isValidContentType("text/css"));
  
  EXPECT_FALSE(ErrorPage::isValidContentType(""));
  EXPECT_FALSE(ErrorPage::isValidContentType("application/json"));
  EXPECT_FALSE(ErrorPage::isValidContentType("image/png"));
}

TEST_F(ErrorPageTest, ContentTypeCaseInsensitive) {
  ErrorPage page1(domain::shared::value_objects::ErrorCode::notFound(), "Error", "TEXT/HTML");
  ErrorPage page2(domain::shared::value_objects::ErrorCode::notFound(), "Error", "text/html");
  EXPECT_EQ("text/html", page1.getContentType());
  EXPECT_EQ("text/html", page2.getContentType());
}

// ============================================================================
// Internal Validation Tests (through public API)
// ============================================================================
// Private methods to test:
// - validate() → Test through constructor with invalid inputs
// - validateContent() → Test through setContent() with invalid content
// - validateFilePath() → Test through setFilePath() with invalid paths
// - validateContentType() → Test through setContentType() with invalid types
// - loadFileContent() → Test through file-based constructors
// - generateDefaultContent() → Test through createDefault() methods
// - normalizeContentType() → Test through content type setters/constructors

TEST_F(ErrorPageTest, ValidateThroughConstructorNoContent) {
  // Constructor should validate and throw on empty content
  ErrorPage* ep = NULL;
  EXPECT_THROW(ep = new ErrorPage(domain::shared::value_objects::ErrorCode::notFound(), ""), ErrorPageException);
  if (ep) delete ep;
}

TEST_F(ErrorPageTest, ValidateContentThroughSetContent) {
  ErrorPage errorPage(domain::shared::value_objects::ErrorCode::notFound(), "Initial");
  std::string tooLarge(ErrorPage::MAX_CONTENT_LENGTH + 1, 'x');
  EXPECT_THROW(errorPage.setContent(tooLarge), ErrorPageException);
}

TEST_F(ErrorPageTest, ValidateContentTypeThroughSetter) {
  ErrorPage errorPage(domain::shared::value_objects::ErrorCode::notFound(), "Error");
  EXPECT_THROW(errorPage.setContentType(""), ErrorPageException);
  EXPECT_THROW(errorPage.setContentType("application/json"), ErrorPageException);
}

TEST_F(ErrorPageTest, NormalizeContentTypeThroughConstructor) {
  ErrorPage page1(domain::shared::value_objects::ErrorCode::notFound(), "Error", "TEXT/PLAIN");
  EXPECT_EQ("text/plain", page1.getContentType());
  
  ErrorPage page2(domain::shared::value_objects::ErrorCode::notFound(), "Error", "  text/html  ");
  EXPECT_EQ("text/html", page2.getContentType());
}

TEST_F(ErrorPageTest, GenerateDefaultContentThroughFactory) {
  ErrorPage page404 = ErrorPage::createDefault(domain::shared::value_objects::ErrorCode::notFound());
  ErrorPage page500 = ErrorPage::createDefault(domain::shared::value_objects::ErrorCode::internalServerError());
  
  EXPECT_NE(std::string::npos, page404.getContent().find("404"));
  EXPECT_NE(std::string::npos, page500.getContent().find("500"));
  EXPECT_NE(page404.getContent(), page500.getContent());
}

TEST_F(ErrorPageTest, GenerateDefaultContentContainsPlaceholders) {
  ErrorPage errorPage = ErrorPage::createDefault(domain::shared::value_objects::ErrorCode::notFound());
  std::string content = errorPage.getContent();
  
  // Should contain error code and description, not placeholders
  EXPECT_EQ(std::string::npos, content.find("{CODE}"));
  EXPECT_EQ(std::string::npos, content.find("{MESSAGE}"));
  EXPECT_NE(std::string::npos, content.find("404"));
}

TEST_F(ErrorPageTest, ContentSizeMatchesActualContent) {
  std::string content = "Test Error Page";
  ErrorPage errorPage(domain::shared::value_objects::ErrorCode::notFound(), content);
  EXPECT_EQ(content.size(), errorPage.getContentSize().getBytes());
  EXPECT_EQ(content.size(), errorPage.getContent().size());
}


