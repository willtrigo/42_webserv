/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_Route.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: umeneses <umeneses@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/30 12:43:15 by umeneses          #+#    #+#             */
/*   Updated: 2026/01/06 19:39:51 by umeneses         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <gtest/gtest.h>
#include "domain/configuration/value_objects/Route.hpp"
#include "domain/configuration/exceptions/RouteException.hpp"

using namespace domain::configuration::value_objects;
using namespace domain::configuration::exceptions;

class RouteTest : public ::testing::Test {
 protected:
  void SetUp() {}
  void TearDown() {}
};

// ============================================================================
// Construction Tests
// ============================================================================

TEST_F(RouteTest, DefaultConstructor) {
  Route* route = NULL;
  EXPECT_NO_THROW(route = new Route());
  if (route != NULL) delete route;
}

TEST_F(RouteTest, ConstructWithPathAndMethods) {
  domain::filesystem::value_objects::Path path("/api");
  std::set<domain::http::value_objects::HttpMethod> methods;
  methods.insert(domain::http::value_objects::HttpMethod::get());
  methods.insert(domain::http::value_objects::HttpMethod::post());
  
  Route route(path, methods, Route::STATIC_FILE);
  EXPECT_EQ("/api", route.getPath().toString());
  EXPECT_EQ(2u, route.getAllowedMethods().size());
}

TEST_F(RouteTest, ConstructWithDifferentHandlerTypes) {
  domain::filesystem::value_objects::Path path("/");
  std::set<domain::http::value_objects::HttpMethod> methods;
  methods.insert(domain::http::value_objects::HttpMethod::get());
  
  EXPECT_NO_THROW(Route(path, methods, Route::STATIC_FILE));
  EXPECT_NO_THROW(Route(path, methods, Route::DIRECTORY_LISTING));
  EXPECT_NO_THROW(Route(path, methods, Route::POST));
  EXPECT_NO_THROW(Route(path, methods, Route::DELETE));
  EXPECT_NO_THROW(Route(path, methods, Route::REDIRECT));
  EXPECT_NO_THROW(Route(path, methods, Route::UPLOAD));
}

// ============================================================================
// Getter Tests
// ============================================================================

TEST_F(RouteTest, GetPath) {
  domain::filesystem::value_objects::Path path("/api/users");
  std::set<domain::http::value_objects::HttpMethod> methods;
  methods.insert(domain::http::value_objects::HttpMethod::get());
  
  Route route(path, methods, Route::STATIC_FILE);
  EXPECT_EQ("/api/users", route.getPath().toString());
}

TEST_F(RouteTest, GetAllowedMethods) {
  domain::filesystem::value_objects::Path path("/");
  std::set<domain::http::value_objects::HttpMethod> methods;
  methods.insert(domain::http::value_objects::HttpMethod::get());
  methods.insert(domain::http::value_objects::HttpMethod::post());
  methods.insert(domain::http::value_objects::HttpMethod::deleteMethod());
  
  Route route(path, methods, Route::STATIC_FILE);
  EXPECT_EQ(3u, route.getAllowedMethods().size());
}

TEST_F(RouteTest, GetHandlerType) {
  domain::filesystem::value_objects::Path path("/");
  std::set<domain::http::value_objects::HttpMethod> methods;
  methods.insert(domain::http::value_objects::HttpMethod::get());
  
  Route route(path, methods, Route::CGI_EXECUTION);
  EXPECT_EQ(Route::CGI_EXECUTION, route.getHandlerType());
}

TEST_F(RouteTest, GetHandlerTarget) {
  domain::filesystem::value_objects::Path path("/");
  std::set<domain::http::value_objects::HttpMethod> methods;
  methods.insert(domain::http::value_objects::HttpMethod::get());
  
  Route route(path, methods, Route::STATIC_FILE);
  route.setRootDirectory("/var/www");
  
  EXPECT_EQ("/var/www", route.getHandlerTarget());
}

// ============================================================================
// Setter Tests
// ============================================================================

TEST_F(RouteTest, SetRootDirectory) {
  domain::filesystem::value_objects::Path path("/");
  std::set<domain::http::value_objects::HttpMethod> methods;
  methods.insert(domain::http::value_objects::HttpMethod::get());
  
  Route route(path, methods, Route::STATIC_FILE);
  EXPECT_NO_THROW(route.setRootDirectory("/var/www"));
  EXPECT_EQ("/var/www", route.getRootDirectory());
}

TEST_F(RouteTest, SetRootDirectoryEmpty) {
  domain::filesystem::value_objects::Path path("/");
  std::set<domain::http::value_objects::HttpMethod> methods;
  methods.insert(domain::http::value_objects::HttpMethod::get());
  
  Route route(path, methods, Route::STATIC_FILE);
  EXPECT_THROW(route.setRootDirectory(""), RouteException);
}

TEST_F(RouteTest, SetIndexFile) {
  domain::filesystem::value_objects::Path path("/");
  std::set<domain::http::value_objects::HttpMethod> methods;
  methods.insert(domain::http::value_objects::HttpMethod::get());
  
  Route route(path, methods, Route::STATIC_FILE);
  EXPECT_NO_THROW(route.setIndexFile("index.html"));
  EXPECT_EQ("index.html", route.getIndexFile());
}

TEST_F(RouteTest, SetIndexFileEmpty) {
  domain::filesystem::value_objects::Path path("/");
  std::set<domain::http::value_objects::HttpMethod> methods;
  methods.insert(domain::http::value_objects::HttpMethod::get());
  
  Route route(path, methods, Route::STATIC_FILE);
  EXPECT_NO_THROW(route.setIndexFile(""));
  EXPECT_TRUE(route.getIndexFile().empty());
}

TEST_F(RouteTest, SetUploadDirectory) {
  domain::filesystem::value_objects::Path path("/upload");
  std::set<domain::http::value_objects::HttpMethod> methods;
  methods.insert(domain::http::value_objects::HttpMethod::post());
  
  Route route(path, methods, Route::UPLOAD);
  EXPECT_NO_THROW(route.setUploadDirectory("/var/uploads"));
  EXPECT_EQ("/var/uploads", route.getUploadDirectory());
}

TEST_F(RouteTest, SetUploadDirectoryEmpty) {
  domain::filesystem::value_objects::Path path("/upload");
  std::set<domain::http::value_objects::HttpMethod> methods;
  methods.insert(domain::http::value_objects::HttpMethod::post());
  
  Route route(path, methods, Route::UPLOAD);
  EXPECT_THROW(route.setUploadDirectory(""), RouteException);
}

TEST_F(RouteTest, SetMaxBodySize) {
  domain::filesystem::value_objects::Path path("/");
  std::set<domain::http::value_objects::HttpMethod> methods;
  methods.insert(domain::http::value_objects::HttpMethod::post());
  
  Route route(path, methods, Route::POST);
  domain::filesystem::value_objects::Size size = domain::filesystem::value_objects::Size::fromMegabytes(10);
  
  EXPECT_NO_THROW(route.setMaxBodySize(size));
  EXPECT_EQ(size.getBytes(), route.getMaxBodySize().getBytes());
}

TEST_F(RouteTest, SetMaxBodySizeZero) {
  domain::filesystem::value_objects::Path path("/");
  std::set<domain::http::value_objects::HttpMethod> methods;
  methods.insert(domain::http::value_objects::HttpMethod::post());
  
  Route route(path, methods, Route::POST);
  domain::filesystem::value_objects::Size zeroSize(0);
  
  EXPECT_THROW(route.setMaxBodySize(zeroSize), RouteException);
}

TEST_F(RouteTest, SetDirectoryListing) {
  domain::filesystem::value_objects::Path path("/");
  std::set<domain::http::value_objects::HttpMethod> methods;
  methods.insert(domain::http::value_objects::HttpMethod::get());
  
  Route route(path, methods, Route::DIRECTORY_LISTING);
  route.setDirectoryListing(true);
  
  EXPECT_TRUE(route.allowsDirectoryListing());
}

TEST_F(RouteTest, SetCgiConfig) {
  domain::filesystem::value_objects::Path path("/cgi-bin");
  std::set<domain::http::value_objects::HttpMethod> methods;
  methods.insert(domain::http::value_objects::HttpMethod::get());
  
  Route route(path, methods, Route::CGI_EXECUTION);
  EXPECT_NO_THROW(route.setCgiConfig("/usr/bin/php-cgi", ".php"));
  EXPECT_EQ("/usr/bin/php-cgi", route.getCgiInterpreter());
  EXPECT_EQ(".php", route.getCgiExtension());
}

TEST_F(RouteTest, SetCgiConfigEmptyInterpreter) {
  domain::filesystem::value_objects::Path path("/cgi-bin");
  std::set<domain::http::value_objects::HttpMethod> methods;
  methods.insert(domain::http::value_objects::HttpMethod::get());
  
  Route route(path, methods, Route::CGI_EXECUTION);
  EXPECT_THROW(route.setCgiConfig("", ".php"), RouteException);
}

TEST_F(RouteTest, SetCgiConfigInvalidExtension) {
  domain::filesystem::value_objects::Path path("/cgi-bin");
  std::set<domain::http::value_objects::HttpMethod> methods;
  methods.insert(domain::http::value_objects::HttpMethod::get());
  
  Route route(path, methods, Route::CGI_EXECUTION);
  EXPECT_THROW(route.setCgiConfig("/usr/bin/php-cgi", "php"), RouteException);
}

TEST_F(RouteTest, SetRedirect) {
  domain::filesystem::value_objects::Path path("/old");
  std::set<domain::http::value_objects::HttpMethod> methods;
  methods.insert(domain::http::value_objects::HttpMethod::get());
  
  Route route(path, methods, Route::REDIRECT);
  domain::shared::value_objects::ErrorCode code = domain::shared::value_objects::ErrorCode::movedPermanently();
  
  EXPECT_NO_THROW(route.setRedirect("/new", code));
  EXPECT_EQ("/new", route.getRedirectTarget());
}

TEST_F(RouteTest, SetRedirectEmptyTarget) {
  domain::filesystem::value_objects::Path path("/old");
  std::set<domain::http::value_objects::HttpMethod> methods;
  methods.insert(domain::http::value_objects::HttpMethod::get());
  
  Route route(path, methods, Route::REDIRECT);
  domain::shared::value_objects::ErrorCode code = domain::shared::value_objects::ErrorCode::movedPermanently();
  
  EXPECT_THROW(route.setRedirect("", code), RouteException);
}

TEST_F(RouteTest, SetFilePermissions) {
  domain::filesystem::value_objects::Path path("/");
  std::set<domain::http::value_objects::HttpMethod> methods;
  methods.insert(domain::http::value_objects::HttpMethod::get());
  
  Route route(path, methods, Route::STATIC_FILE);
  domain::filesystem::value_objects::Permission perm = domain::filesystem::value_objects::Permission::ownerReadWriteGroupReadOtherRead();
  
  EXPECT_NO_THROW(route.setFilePermissions(perm));
}

// ============================================================================
// Method Validation Tests
// ============================================================================

TEST_F(RouteTest, AllowsMethodGet) {
  domain::filesystem::value_objects::Path path("/");
  std::set<domain::http::value_objects::HttpMethod> methods;
  methods.insert(domain::http::value_objects::HttpMethod::get());
  
  Route route(path, methods, Route::STATIC_FILE);
  EXPECT_TRUE(route.allowsMethod(domain::http::value_objects::HttpMethod::get()));
  EXPECT_FALSE(route.allowsMethod(domain::http::value_objects::HttpMethod::post()));
}

TEST_F(RouteTest, AllowsMultipleMethods) {
  domain::filesystem::value_objects::Path path("/api");
  std::set<domain::http::value_objects::HttpMethod> methods;
  methods.insert(domain::http::value_objects::HttpMethod::get());
  methods.insert(domain::http::value_objects::HttpMethod::post());
  methods.insert(domain::http::value_objects::HttpMethod::deleteMethod());
  
  Route route(path, methods, Route::STATIC_FILE);
  EXPECT_TRUE(route.allowsMethod(domain::http::value_objects::HttpMethod::get()));
  EXPECT_TRUE(route.allowsMethod(domain::http::value_objects::HttpMethod::post()));
  EXPECT_TRUE(route.allowsMethod(domain::http::value_objects::HttpMethod::deleteMethod()));
  EXPECT_FALSE(route.allowsMethod(domain::http::value_objects::HttpMethod::put()));
}

// ============================================================================
// Path Matching Tests
// ============================================================================

TEST_F(RouteTest, MatchesExactPath) {
  domain::filesystem::value_objects::Path path("/api/users");
  std::set<domain::http::value_objects::HttpMethod> methods;
  methods.insert(domain::http::value_objects::HttpMethod::get());
  
  Route route(path, methods, Route::STATIC_FILE);
  EXPECT_TRUE(route.matches("/api/users"));
  EXPECT_FALSE(route.matches("/api/posts"));
}

TEST_F(RouteTest, MatchesEmptyPath) {
  domain::filesystem::value_objects::Path path("/");
  std::set<domain::http::value_objects::HttpMethod> methods;
  methods.insert(domain::http::value_objects::HttpMethod::get());
  
  Route route(path, methods, Route::STATIC_FILE);
  EXPECT_THROW(route.matches(""), RouteException);
}

TEST_F(RouteTest, MatchesDirectoryPath) {
  domain::filesystem::value_objects::Path path("/api/");
  std::set<domain::http::value_objects::HttpMethod> methods;
  methods.insert(domain::http::value_objects::HttpMethod::get());
  
  Route route(path, methods, Route::STATIC_FILE);
  EXPECT_TRUE(route.matches("/api/"));
}

// ============================================================================
// Handler Type Tests
// ============================================================================

TEST_F(RouteTest, IsCgiRoute) {
  domain::filesystem::value_objects::Path path("/cgi-bin");
  std::set<domain::http::value_objects::HttpMethod> methods;
  methods.insert(domain::http::value_objects::HttpMethod::get());
  
  Route cgiRoute(path, methods, Route::CGI_EXECUTION);
  EXPECT_TRUE(cgiRoute.isCgiRoute());
  
  Route staticRoute(path, methods, Route::STATIC_FILE);
  EXPECT_FALSE(staticRoute.isCgiRoute());
}

TEST_F(RouteTest, IsRedirect) {
  domain::filesystem::value_objects::Path path("/old");
  std::set<domain::http::value_objects::HttpMethod> methods;
  methods.insert(domain::http::value_objects::HttpMethod::get());
  
  Route redirectRoute(path, methods, Route::REDIRECT);
  EXPECT_TRUE(redirectRoute.isRedirect());
  
  Route staticRoute(path, methods, Route::STATIC_FILE);
  EXPECT_FALSE(staticRoute.isRedirect());
}

TEST_F(RouteTest, IsUploadRoute) {
  domain::filesystem::value_objects::Path path("/upload");
  std::set<domain::http::value_objects::HttpMethod> methods;
  methods.insert(domain::http::value_objects::HttpMethod::post());
  
  Route uploadRoute(path, methods, Route::UPLOAD);
  EXPECT_TRUE(uploadRoute.isUploadRoute());
  
  Route staticRoute(path, methods, Route::STATIC_FILE);
  EXPECT_FALSE(staticRoute.isUploadRoute());
}

TEST_F(RouteTest, AllowsDirectoryListing) {
  domain::filesystem::value_objects::Path path("/");
  std::set<domain::http::value_objects::HttpMethod> methods;
  methods.insert(domain::http::value_objects::HttpMethod::get());
  
  Route route(path, methods, Route::DIRECTORY_LISTING);
  route.setDirectoryListing(true);
  
  EXPECT_TRUE(route.allowsDirectoryListing());
}

TEST_F(RouteTest, AllowsDirectoryListingWrongHandler) {
  domain::filesystem::value_objects::Path path("/");
  std::set<domain::http::value_objects::HttpMethod> methods;
  methods.insert(domain::http::value_objects::HttpMethod::get());
  
  Route route(path, methods, Route::STATIC_FILE);
  route.setDirectoryListing(true);
  
  EXPECT_FALSE(route.allowsDirectoryListing());
}

// ============================================================================
// State Query Tests
// ============================================================================

TEST_F(RouteTest, IsStaticFileRoute) {
  domain::filesystem::value_objects::Path path("/");
  std::set<domain::http::value_objects::HttpMethod> methods;
  methods.insert(domain::http::value_objects::HttpMethod::get());
  
  Route route(path, methods, Route::STATIC_FILE);
  EXPECT_TRUE(route.isStaticFileRoute());
  EXPECT_FALSE(route.isPostRoute());
  EXPECT_FALSE(route.isDeleteRoute());
}

TEST_F(RouteTest, IsPostRoute) {
  domain::filesystem::value_objects::Path path("/api");
  std::set<domain::http::value_objects::HttpMethod> methods;
  methods.insert(domain::http::value_objects::HttpMethod::post());
  
  Route route(path, methods, Route::POST);
  EXPECT_TRUE(route.isPostRoute());
  EXPECT_FALSE(route.isStaticFileRoute());
}

TEST_F(RouteTest, IsDeleteRoute) {
  domain::filesystem::value_objects::Path path("/api");
  std::set<domain::http::value_objects::HttpMethod> methods;
  methods.insert(domain::http::value_objects::HttpMethod::deleteMethod());
  
  Route route(path, methods, Route::DELETE);
  EXPECT_TRUE(route.isDeleteRoute());
  EXPECT_FALSE(route.isStaticFileRoute());
}

TEST_F(RouteTest, HasRootDirectory) {
  domain::filesystem::value_objects::Path path("/");
  std::set<domain::http::value_objects::HttpMethod> methods;
  methods.insert(domain::http::value_objects::HttpMethod::get());
  
  Route route(path, methods, Route::STATIC_FILE);
  EXPECT_FALSE(route.hasRootDirectory());
  
  route.setRootDirectory("/var/www");
  EXPECT_TRUE(route.hasRootDirectory());
}

TEST_F(RouteTest, HasIndexFile) {
  domain::filesystem::value_objects::Path path("/");
  std::set<domain::http::value_objects::HttpMethod> methods;
  methods.insert(domain::http::value_objects::HttpMethod::get());
  
  Route route(path, methods, Route::STATIC_FILE);
  EXPECT_FALSE(route.hasIndexFile());
  
  route.setIndexFile("index.html");
  EXPECT_TRUE(route.hasIndexFile());
}

TEST_F(RouteTest, HasCgiConfig) {
  domain::filesystem::value_objects::Path path("/cgi");
  std::set<domain::http::value_objects::HttpMethod> methods;
  methods.insert(domain::http::value_objects::HttpMethod::get());
  
  Route route(path, methods, Route::CGI_EXECUTION);
  EXPECT_FALSE(route.hasCgiConfig());
  
  route.setCgiConfig("/usr/bin/php-cgi", ".php");
  EXPECT_TRUE(route.hasCgiConfig());
}

TEST_F(RouteTest, HasRedirectConfig) {
  domain::filesystem::value_objects::Path path("/old");
  std::set<domain::http::value_objects::HttpMethod> methods;
  methods.insert(domain::http::value_objects::HttpMethod::get());
  
  Route route(path, methods, Route::REDIRECT);
  EXPECT_FALSE(route.hasRedirectConfig());
  
  route.setRedirect("/new", domain::shared::value_objects::ErrorCode::movedPermanently());
  EXPECT_TRUE(route.hasRedirectConfig());
}

TEST_F(RouteTest, HasUploadConfig) {
  domain::filesystem::value_objects::Path path("/upload");
  std::set<domain::http::value_objects::HttpMethod> methods;
  methods.insert(domain::http::value_objects::HttpMethod::post());
  
  Route route(path, methods, Route::UPLOAD);
  EXPECT_FALSE(route.hasUploadConfig());
  
  route.setUploadDirectory("/var/uploads");
  EXPECT_TRUE(route.hasUploadConfig());
}

// ============================================================================
// Configuration Tests
// ============================================================================

TEST_F(RouteTest, CgiConfiguration) {
  domain::filesystem::value_objects::Path path("/cgi-bin");
  std::set<domain::http::value_objects::HttpMethod> methods;
  methods.insert(domain::http::value_objects::HttpMethod::get());
  
  Route route(path, methods, Route::CGI_EXECUTION);
  route.setCgiConfig("/usr/bin/python3", ".py");
  
  EXPECT_TRUE(route.isCgiRoute());
  EXPECT_TRUE(route.hasCgiConfig());
  EXPECT_EQ("/usr/bin/python3", route.getCgiInterpreter());
  EXPECT_EQ(".py", route.getCgiExtension());
}

TEST_F(RouteTest, RedirectConfiguration) {
  domain::filesystem::value_objects::Path path("/old-api");
  std::set<domain::http::value_objects::HttpMethod> methods;
  methods.insert(domain::http::value_objects::HttpMethod::get());
  
  Route route(path, methods, Route::REDIRECT);
  domain::shared::value_objects::ErrorCode code = domain::shared::value_objects::ErrorCode::found();
  route.setRedirect("/new-api", code);
  
  EXPECT_TRUE(route.isRedirect());
  EXPECT_TRUE(route.hasRedirectConfig());
  EXPECT_EQ("/new-api", route.getRedirectTarget());
  EXPECT_EQ(302, route.getRedirectCode().getValue());
}

TEST_F(RouteTest, UploadConfiguration) {
  domain::filesystem::value_objects::Path path("/upload");
  std::set<domain::http::value_objects::HttpMethod> methods;
  methods.insert(domain::http::value_objects::HttpMethod::post());
  
  Route route(path, methods, Route::UPLOAD);
  route.setUploadDirectory("/var/uploads");
  route.setMaxBodySize(domain::filesystem::value_objects::Size::fromMegabytes(100));
  
  EXPECT_TRUE(route.isUploadRoute());
  EXPECT_TRUE(route.hasUploadConfig());
  EXPECT_EQ("/var/uploads", route.getUploadDirectory());
}

// ============================================================================
// Edge Cases
// ============================================================================

TEST_F(RouteTest, RootPath) {
  domain::filesystem::value_objects::Path path("/");
  std::set<domain::http::value_objects::HttpMethod> methods;
  methods.insert(domain::http::value_objects::HttpMethod::get());
  
  Route* route = NULL;
  EXPECT_NO_THROW(route = new Route(path, methods, Route::STATIC_FILE));
  if (route) delete route;
}

TEST_F(RouteTest, LongPath) {
  const std::string longPath = "/very/long/path/with/many/segments/to/test/maximum/length";
  domain::filesystem::value_objects::Path path(longPath.c_str());
  std::set<domain::http::value_objects::HttpMethod> methods;
  methods.insert(domain::http::value_objects::HttpMethod::get());
  
  Route route(path, methods, Route::STATIC_FILE);
  EXPECT_EQ(longPath, route.getPath().toString());
}

TEST_F(RouteTest, AllMethodsAllowed) {
  domain::filesystem::value_objects::Path path("/api");
  std::set<domain::http::value_objects::HttpMethod> methods;
  methods.insert(domain::http::value_objects::HttpMethod::get());
  methods.insert(domain::http::value_objects::HttpMethod::post());
  methods.insert(domain::http::value_objects::HttpMethod::put());
  methods.insert(domain::http::value_objects::HttpMethod::deleteMethod());
  methods.insert(domain::http::value_objects::HttpMethod::head());
  methods.insert(domain::http::value_objects::HttpMethod::options());
  
  Route route(path, methods, Route::STATIC_FILE);
  EXPECT_EQ(6u, route.getAllowedMethods().size());
}

TEST_F(RouteTest, MaxBodySizeLarge) {
  domain::filesystem::value_objects::Path path("/upload");
  std::set<domain::http::value_objects::HttpMethod> methods;
  methods.insert(domain::http::value_objects::HttpMethod::post());
  
  Route route(path, methods, Route::UPLOAD);
  domain::filesystem::value_objects::Size largeSize = domain::filesystem::value_objects::Size::fromGigabytes(5);
  
  EXPECT_NO_THROW(route.setMaxBodySize(largeSize));
}

// ============================================================================
// Boundary Tests
// ============================================================================

TEST_F(RouteTest, SingleMethod) {
  domain::filesystem::value_objects::Path path("/");
  std::set<domain::http::value_objects::HttpMethod> methods;
  methods.insert(domain::http::value_objects::HttpMethod::get());
  
  Route route(path, methods, Route::STATIC_FILE);
  EXPECT_EQ(1u, route.getAllowedMethods().size());
}

TEST_F(RouteTest, MinimalConfiguration) {
  domain::filesystem::value_objects::Path path("/");
  std::set<domain::http::value_objects::HttpMethod> methods;
  methods.insert(domain::http::value_objects::HttpMethod::get());
  
  Route route(path, methods, Route::STATIC_FILE);
  EXPECT_FALSE(route.hasRootDirectory());
  EXPECT_FALSE(route.hasIndexFile());
  EXPECT_FALSE(route.hasCgiConfig());
  EXPECT_FALSE(route.hasRedirectConfig());
  EXPECT_FALSE(route.hasUploadConfig());
}

// ============================================================================
// Exception Handling Tests
// ============================================================================

TEST_F(RouteTest, InvalidRootDirectory) {
  domain::filesystem::value_objects::Path path("/");
  std::set<domain::http::value_objects::HttpMethod> methods;
  methods.insert(domain::http::value_objects::HttpMethod::get());
  
  Route route(path, methods, Route::STATIC_FILE);
  EXPECT_THROW(route.setRootDirectory(""), RouteException);
}

TEST_F(RouteTest, InvalidUploadDirectory) {
  domain::filesystem::value_objects::Path path("/upload");
  std::set<domain::http::value_objects::HttpMethod> methods;
  methods.insert(domain::http::value_objects::HttpMethod::post());
  
  Route route(path, methods, Route::UPLOAD);
  EXPECT_THROW(route.setUploadDirectory(""), RouteException);
}

TEST_F(RouteTest, InvalidCgiInterpreter) {
  domain::filesystem::value_objects::Path path("/cgi");
  std::set<domain::http::value_objects::HttpMethod> methods;
  methods.insert(domain::http::value_objects::HttpMethod::get());
  
  Route route(path, methods, Route::CGI_EXECUTION);
  EXPECT_THROW(route.setCgiConfig("", ".php"), RouteException);
}

TEST_F(RouteTest, InvalidCgiExtension) {
  domain::filesystem::value_objects::Path path("/cgi");
  std::set<domain::http::value_objects::HttpMethod> methods;
  methods.insert(domain::http::value_objects::HttpMethod::get());
  
  Route route(path, methods, Route::CGI_EXECUTION);
  EXPECT_THROW(route.setCgiConfig("/usr/bin/php", ""), RouteException);
  EXPECT_THROW(route.setCgiConfig("/usr/bin/php", "php"), RouteException);
}

TEST_F(RouteTest, InvalidRedirectTarget) {
  domain::filesystem::value_objects::Path path("/old");
  std::set<domain::http::value_objects::HttpMethod> methods;
  methods.insert(domain::http::value_objects::HttpMethod::get());
  
  Route route(path, methods, Route::REDIRECT);
  EXPECT_THROW(route.setRedirect("", domain::shared::value_objects::ErrorCode::movedPermanently()), RouteException);
}

TEST_F(RouteTest, InvalidMaxBodySize) {
  domain::filesystem::value_objects::Path path("/");
  std::set<domain::http::value_objects::HttpMethod> methods;
  methods.insert(domain::http::value_objects::HttpMethod::post());
  
  Route route(path, methods, Route::POST);
  EXPECT_THROW(route.setMaxBodySize(domain::filesystem::value_objects::Size(0)), RouteException);
}

TEST_F(RouteTest, EmptyPathMatch) {
  domain::filesystem::value_objects::Path path("/api");
  std::set<domain::http::value_objects::HttpMethod> methods;
  methods.insert(domain::http::value_objects::HttpMethod::get());
  
  Route route(path, methods, Route::STATIC_FILE);
  EXPECT_THROW(route.matches(""), RouteException);
}

// ============================================================================
// Internal Validation Tests (through public API)
// ============================================================================

TEST_F(RouteTest, ValidateHandlerTypeConsistency) {
  domain::filesystem::value_objects::Path path("/");
  std::set<domain::http::value_objects::HttpMethod> methods;
  methods.insert(domain::http::value_objects::HttpMethod::get());
  
  Route staticRoute(path, methods, Route::STATIC_FILE);
  EXPECT_TRUE(staticRoute.isStaticFileRoute());
  EXPECT_FALSE(staticRoute.isCgiRoute());
  
  Route cgiRoute(path, methods, Route::CGI_EXECUTION);
  EXPECT_TRUE(cgiRoute.isCgiRoute());
  EXPECT_FALSE(cgiRoute.isStaticFileRoute());
}

TEST_F(RouteTest, ValidateMethodAllowance) {
  domain::filesystem::value_objects::Path path("/");
  std::set<domain::http::value_objects::HttpMethod> methods;
  methods.insert(domain::http::value_objects::HttpMethod::get());
  
  Route route(path, methods, Route::STATIC_FILE);
  EXPECT_TRUE(route.allowsMethod(domain::http::value_objects::HttpMethod::get()));
  EXPECT_FALSE(route.allowsMethod(domain::http::value_objects::HttpMethod::post()));
  EXPECT_FALSE(route.allowsMethod(domain::http::value_objects::HttpMethod::put()));
  EXPECT_FALSE(route.allowsMethod(domain::http::value_objects::HttpMethod::deleteMethod()));
}

TEST_F(RouteTest, ValidateConfigurationState) {
  domain::filesystem::value_objects::Path path("/");
  std::set<domain::http::value_objects::HttpMethod> methods;
  methods.insert(domain::http::value_objects::HttpMethod::get());
  
  Route route(path, methods, Route::STATIC_FILE);
  
  EXPECT_FALSE(route.hasRootDirectory());
  route.setRootDirectory("/var/www");
  EXPECT_TRUE(route.hasRootDirectory());
  
  EXPECT_FALSE(route.hasIndexFile());
  route.setIndexFile("index.html");
  EXPECT_TRUE(route.hasIndexFile());
}

TEST_F(RouteTest, ValidateGettersAfterSetters) {
  domain::filesystem::value_objects::Path path("/");
  std::set<domain::http::value_objects::HttpMethod> methods;
  methods.insert(domain::http::value_objects::HttpMethod::get());
  
  Route route(path, methods, Route::STATIC_FILE);
  
  const std::string rootDir = "/var/www/html";
  route.setRootDirectory(rootDir);
  EXPECT_EQ(rootDir, route.getRootDirectory());
  EXPECT_EQ(rootDir, route.getHandlerTarget());
  
  const std::string indexFile = "default.html";
  route.setIndexFile(indexFile);
  EXPECT_EQ(indexFile, route.getIndexFile());
}

TEST_F(RouteTest, ValidateMultipleConfigurations) {
  domain::filesystem::value_objects::Path path("/api");
  std::set<domain::http::value_objects::HttpMethod> methods;
  methods.insert(domain::http::value_objects::HttpMethod::post());
  
  Route route(path, methods, Route::POST);
  
  route.setRootDirectory("/var/www/api");
  route.setMaxBodySize(domain::filesystem::value_objects::Size::fromMegabytes(50));
  route.setFilePermissions(domain::filesystem::value_objects::Permission::ownerReadWriteGroupReadOtherNone());
  
  EXPECT_TRUE(route.hasRootDirectory());
  EXPECT_EQ("/var/www/api", route.getRootDirectory());
  EXPECT_EQ(50 * 1024 * 1024, route.getMaxBodySize().getBytes());
}

TEST_F(RouteTest, ValidatePathMatching) {
  domain::filesystem::value_objects::Path path("/api/v1");
  std::set<domain::http::value_objects::HttpMethod> methods;
  methods.insert(domain::http::value_objects::HttpMethod::get());
  
  Route route(path, methods, Route::STATIC_FILE);
  
  EXPECT_TRUE(route.matches("/api/v1"));
  EXPECT_FALSE(route.matches("/api/v2"));
  EXPECT_FALSE(route.matches("/api"));
}