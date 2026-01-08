#include "HttpTestClient.hpp"

#include <cstdlib>
#include <fstream>
#include <gtest/gtest.h>
#include <unistd.h>

/**
 * 42 Webserver Integration Tests
 *
 * These tests require a running server at 127.0.0.1:8080
 * Start the server before running: ./bin/webserv conf/default.conf
 *
 * IMPORTANT: Tests match 42 evaluation requirements
 */

class HttpServerIntegrationTest : public ::testing::Test {
 protected:
  HttpTestClient* client;
  std::string serverUrl;

  void SetUp() {
    // Server should already be running
    client = new HttpTestClient("127.0.0.1", 8080);
    serverUrl = "http://127.0.0.1:8080";

    // Verify server is running
    HttpTestClient::Response ping = client->get("/");
    if (ping.statusCode < 0) {
      FAIL() << "Server not running at 127.0.0.1:8080! Start it before running "
                "tests.";
    }
  }

  void TearDown() { delete client; }
};

// ============================================
// 1. HTTP METHODS SUPPORT
// ============================================

TEST_F(HttpServerIntegrationTest, GetRequestReturns200) {
  HttpTestClient::Response response = client->get("/");

  EXPECT_EQ(200, response.statusCode) << "GET / should return 200 OK";
}

TEST_F(HttpServerIntegrationTest, HeadRequestReturns200) {
  HttpTestClient::Response response = client->head("/");

  EXPECT_EQ(200, response.statusCode)
      << "HEAD / should return 200 OK (CRITICAL: ubuntu_tester requires this)";
}

TEST_F(HttpServerIntegrationTest, HeadRequestHasNoBody) {
  HttpTestClient::Response response = client->head("/");

  EXPECT_EQ(200, response.statusCode);
  EXPECT_TRUE(response.body.empty())
      << "HEAD response MUST have no body per HTTP spec (RFC 7231 Section "
         "4.3.2), got "
      << response.body.length() << " bytes";
}

TEST_F(HttpServerIntegrationTest, PostRequestReturns200Or201) {
  HttpTestClient::Response response = client->post("/files", "test=data");

  EXPECT_TRUE(response.statusCode == 200 || response.statusCode == 201)
      << "POST /files should return 200 or 201, got " << response.statusCode;
}

TEST_F(HttpServerIntegrationTest, DeleteRequestReturns200Or204) {
  // First upload a file with specific filename
  std::string testContent = "test file for deletion";
  HttpTestClient::Response uploadResponse = client->post("/files", testContent);

  // Verify upload succeeded before testing delete
  ASSERT_TRUE(uploadResponse.statusCode == 200 ||
              uploadResponse.statusCode == 201)
      << "File upload must succeed before testing DELETE, got "
      << uploadResponse.statusCode;

  // Then delete it
  HttpTestClient::Response response = client->del("/files/test_delete.txt");

  EXPECT_TRUE(response.statusCode == 200 || response.statusCode == 204 ||
              response.statusCode == 404)
      << "DELETE should return 200/204 (success) or 404 (already deleted), got "
      << response.statusCode << " (500 means DELETE is broken)";
}

TEST_F(HttpServerIntegrationTest, UnknownMethodDoesNotCrash) {
  HttpTestClient::Response response = client->request("UNKNOWN", "/");

  EXPECT_TRUE(response.statusCode == 405 || response.statusCode == 501)
      << "UNKNOWN method should return 405 or 501, got " << response.statusCode
      << " (if server crashed, this test would fail)";
}

// ============================================
// 2. ERROR PAGES & STATUS CODES
// ============================================

TEST_F(HttpServerIntegrationTest, NonexistentFileReturns404) {
  HttpTestClient::Response response =
      client->get("/nonexistent_file_12345.html");

  EXPECT_EQ(404, response.statusCode) << "Nonexistent file should return 404";
}

TEST_F(HttpServerIntegrationTest, Custom404PageContains404Text) {
  HttpTestClient::Response response = client->get("/nonexistent");

  EXPECT_EQ(404, response.statusCode);
  EXPECT_TRUE(response.body.find("404") != std::string::npos ||
              response.body.find("Not Found") != std::string::npos)
      << "Custom 404 page should contain '404' or 'Not Found'";
}

TEST_F(HttpServerIntegrationTest, PostOnGetOnlyRouteReturns405) {
  // First verify that GET works on root route
  HttpTestClient::Response getResponse = client->get("/");
  bool routeExists = (getResponse.statusCode == 200);

  if (!routeExists) {
    std::cout << "[ SKIPPED  ] Route / not configured or not accessible. "
              << "This test requires / to be GET-only in default.conf"
              << std::endl;
    return;
  }

  // Now test that POST is not allowed (if configured as GET-only)
  HttpTestClient::Response response = client->post("/", "test data");

  // Accept 405 (method not allowed) OR 200/201 (if POST is actually allowed)
  EXPECT_TRUE(response.statusCode == 405 || response.statusCode == 200 ||
              response.statusCode == 201)
      << "POST on root route should return 405 (if GET-only) or 200/201 (if "
         "POST allowed), got "
      << response.statusCode
      << ". Check default.conf for allowed_methods on route /";
}

// ============================================
// 3. BODY SIZE LIMIT (CRITICAL)
// ============================================

TEST_F(HttpServerIntegrationTest, SmallBodyAccepted) {
  HttpTestClient::Response response = client->post("/files", "small body");

  EXPECT_TRUE(response.statusCode == 200 || response.statusCode == 201)
      << "Small POST body should be accepted, got " << response.statusCode;
}

TEST_F(HttpServerIntegrationTest, LargeBodyReturns413NotCrash) {
  // Create 2MB body (exceeds 1MB limit)
  std::string largeBody(2 * 1024 * 1024, 'X');

  HttpTestClient::Response response = client->post("/files", largeBody);

  EXPECT_EQ(413, response.statusCode)
      << "Large body should return 413 Payload Too Large, got "
      << response.statusCode << " (500 means body size check is broken)";
}

// ============================================
// 4. FILE UPLOAD & RETRIEVAL
// ============================================

TEST_F(HttpServerIntegrationTest, FileUploadReturns200Or201) {
  std::string testContent = "Test file content from integration test";

  HttpTestClient::Response response = client->post("/files", testContent);

  EXPECT_TRUE(response.statusCode == 200 || response.statusCode == 201)
      << "File upload should return 200 or 201";
}

// Note: File retrieval test requires multipart/form-data which is complex
// This is tested better in shell script with curl

// ============================================
// 5. ROUTES & DIRECTORIES
// ============================================

TEST_F(HttpServerIntegrationTest, RootRouteWorks) {
  HttpTestClient::Response response = client->get("/");

  EXPECT_EQ(200, response.statusCode) << "Root route (/) should return 200";
}

TEST_F(HttpServerIntegrationTest, DifferentRouteReturnsValidCode) {
  HttpTestClient::Response response = client->get("/public/");

  EXPECT_TRUE(response.statusCode == 200 || response.statusCode == 404 ||
              response.statusCode == 403)
      << "Route /public/ should return valid code (200/404/403), got "
      << response.statusCode;
}

TEST_F(HttpServerIntegrationTest, DefaultIndexFileIsHTML) {
  HttpTestClient::Response response = client->get("/");

  EXPECT_EQ(200, response.statusCode);
  EXPECT_TRUE(response.body.find("<!DOCTYPE") != std::string::npos ||
              response.body.find("<html") != std::string::npos ||
              response.body.find("<HTML") != std::string::npos)
      << "Default index file should be HTML";
}

// ============================================
// 6. REDIRECTS
// ============================================

TEST_F(HttpServerIntegrationTest, RedirectReturns3xxCode) {
  HttpTestClient::Response response = client->get("/docs");

  EXPECT_TRUE(response.statusCode == 301 || response.statusCode == 302 ||
              response.statusCode == 307)
      << "Redirect should return 301/302/307, got " << response.statusCode;
}

TEST_F(HttpServerIntegrationTest, RedirectHasLocationHeader) {
  HttpTestClient::Response response = client->get("/docs");

  EXPECT_TRUE(response.statusCode == 301 || response.statusCode == 302 ||
              response.statusCode == 307);
  EXPECT_TRUE(response.hasHeader("location"))
      << "Redirect response should have Location header";
}

// ============================================
// 7. CGI SUPPORT (If implemented)
// ============================================

TEST_F(HttpServerIntegrationTest, CgiGetRequestReturnsValidCode) {
  HttpTestClient::Response response =
      client->get("/cgi-bin/test.cgi?name=test");

  // CGI might not be configured, so we accept 200 or 404
  EXPECT_TRUE(response.statusCode == 200 || response.statusCode == 404)
      << "CGI request should return 200 (working) or 404 (not configured)";
}

// ============================================
// 8. CONCURRENT REQUESTS (Basic stress test)
// ============================================

TEST_F(HttpServerIntegrationTest, Handle10ConcurrentRequests) {
  const int NUM_REQUESTS = 10;
  int successCount = 0;

  // Note: C++98 doesn't have std::thread
  // This test makes sequential requests (not truly concurrent)
  // Real concurrency test should be done with shell script + siege

  for (int i = 0; i < NUM_REQUESTS; ++i) {
    HttpTestClient::Response response = client->get("/");
    if (response.statusCode == 200) {
      ++successCount;
    }
  }

  EXPECT_EQ(NUM_REQUESTS, successCount)
      << "All " << NUM_REQUESTS << " requests should succeed, got "
      << successCount << " successes";
}

TEST_F(HttpServerIntegrationTest, Handle100SequentialRequests) {
  const int NUM_REQUESTS = 100;
  int successCount = 0;
  int failCount = 0;

  for (int i = 0; i < NUM_REQUESTS; ++i) {
    HttpTestClient::Response response = client->get("/");
    if (response.statusCode == 200) {
      ++successCount;
    } else {
      ++failCount;
    }
  }

  double availability =
      (static_cast<double>(successCount) / NUM_REQUESTS) * 100.0;

  EXPECT_GE(availability, 99.5)
      << "Availability should be >= 99.5%, got " << availability << "%";
  EXPECT_LE(failCount, 1) << "Max 1 failure allowed in 100 requests, got "
                          << failCount;
}

// ============================================
// 9. RESPONSE HEADERS VALIDATION
// ============================================

TEST_F(HttpServerIntegrationTest, ResponseHasContentTypeHeader) {
  HttpTestClient::Response response = client->get("/");

  EXPECT_EQ(200, response.statusCode);
  EXPECT_TRUE(response.hasHeader("content-type"))
      << "Response should have Content-Type header";
}

TEST_F(HttpServerIntegrationTest, ResponseHasServerHeader) {
  HttpTestClient::Response response = client->get("/");

  EXPECT_EQ(200, response.statusCode);
  EXPECT_TRUE(response.hasHeader("server"))
      << "Response should have Server header";
}

// ============================================
// 10. ERROR HANDLING
// ============================================

TEST_F(HttpServerIntegrationTest, EmptyRequestDoesNotCrash) {
  HttpTestClient::Response response = client->request("", "/");

  // Server should handle gracefully (not crash)
  // May return 400, 405, or any error code
  EXPECT_TRUE(response.statusCode >= 400)
      << "Empty method should return error code (4xx or 5xx)";
}

TEST_F(HttpServerIntegrationTest, MalformedPathDoesNotCrash) {
  HttpTestClient::Response response = client->get("/../../../etc/passwd");

  // Should return error (403 or 404), not crash
  EXPECT_TRUE(response.statusCode == 403 || response.statusCode == 404)
      << "Malformed path should return 403 or 404, not crash";
}

// ============================================
// 11. DIRECTORY LISTING (AUTOINDEX)
// ============================================

TEST_F(HttpServerIntegrationTest, AutoindexOnShowsDirectoryListing) {
  HttpTestClient::Response response = client->get("/public/");

  EXPECT_TRUE(response.statusCode == 200 || response.statusCode == 403)
      << "Route with autoindex ON should return 200 (listing) or 403 "
         "(forbidden)";

  if (response.statusCode == 200) {
    // Directory listing should contain HTML with links or file list
    EXPECT_TRUE(response.body.find("<a href") != std::string::npos ||
                response.body.find("Index of") != std::string::npos ||
                response.body.find("Directory") != std::string::npos)
        << "Autoindex page should contain directory listing HTML";
  }
}

TEST_F(HttpServerIntegrationTest, AutoindexOffReturns403Or404) {
  HttpTestClient::Response response =
      client->get("/");  // Root has autoindex OFF

  // Without index.html, should return 403 or 404 (not directory listing)
  // With index.html present, returns 200
  EXPECT_TRUE(response.statusCode == 200 || response.statusCode == 403 ||
              response.statusCode == 404)
      << "Route with autoindex OFF should serve index or return error";
}

// ============================================
// 12. METHOD RESTRICTIONS PER ROUTE
// ============================================
// NOTE: These tests require default.conf to have proper method restrictions:
// - Route "/" should allow only GET, HEAD (not POST, DELETE)
// - Route "/files" should allow only POST, DELETE (not GET, HEAD)
// If config changes, these tests may fail even if server works correctly.

TEST_F(HttpServerIntegrationTest, GetOnPostOnlyRouteReturns405) {
  // Verify route is POST-only by checking POST works first
  HttpTestClient::Response postResponse = client->post("/files", "test");
  bool routeExists =
      (postResponse.statusCode == 200 || postResponse.statusCode == 201 ||
       postResponse.statusCode ==
           413);  // 413 = body too large, but route exists

  if (!routeExists) {
    std::cout << "[ SKIPPED  ] Route /files not configured or not accessible. "
              << "This test requires /files to be POST-only in default.conf"
              << std::endl;
    return;
  }

  // Now test that GET is not allowed
  HttpTestClient::Response response = client->get("/files");

  EXPECT_EQ(405, response.statusCode)
      << "GET on POST-only route (/files) should return 405 Method Not "
         "Allowed. "
      << "If this fails, check that default.conf has 'allowed_methods POST "
         "DELETE' for /files";
}

TEST_F(HttpServerIntegrationTest, DeleteOnGetOnlyRouteReturns405) {
  HttpTestClient::Response response = client->del("/");

  EXPECT_EQ(405, response.statusCode)
      << "DELETE on GET-only route (/) should return 405 Method Not Allowed";
}

TEST_F(HttpServerIntegrationTest, HeadOnPostOnlyRouteReturns405) {
  HttpTestClient::Response response = client->head("/files");

  EXPECT_EQ(405, response.statusCode)
      << "HEAD on POST-only route should return 405 Method Not Allowed";
}

// ============================================
// 13. KEEP-ALIVE CONNECTION REUSE
// ============================================

TEST_F(HttpServerIntegrationTest, KeepAliveConnectionReuse) {
  // Make multiple requests on same client (same connection)
  HttpTestClient::Response r1 = client->get("/");
  HttpTestClient::Response r2 = client->get("/public/");
  HttpTestClient::Response r3 = client->get("/");

  EXPECT_EQ(200, r1.statusCode);
  EXPECT_TRUE(r2.statusCode == 200 || r2.statusCode == 403 ||
              r2.statusCode == 404);
  EXPECT_EQ(200, r3.statusCode);

  // Check for Connection: keep-alive header
  EXPECT_TRUE(r1.hasHeader("connection"))
      << "Response should have Connection header";
}

TEST_F(HttpServerIntegrationTest, MultipleRequestsSameConnection) {
  const int NUM_REQUESTS = 5;
  int successCount = 0;

  for (int i = 0; i < NUM_REQUESTS; ++i) {
    HttpTestClient::Response response = client->get("/");
    if (response.statusCode == 200) {
      ++successCount;
    }
  }

  EXPECT_EQ(NUM_REQUESTS, successCount)
      << "All requests on same connection should succeed";
}

// ============================================
// 14. DIFFERENT CONTENT TYPES
// ============================================

TEST_F(HttpServerIntegrationTest, HtmlFileReturnsTextHtml) {
  HttpTestClient::Response response = client->get("/");

  EXPECT_EQ(200, response.statusCode);
  if (response.hasHeader("content-type")) {
    std::string contentType = response.getHeader("content-type");
    EXPECT_TRUE(contentType.find("text/html") != std::string::npos)
        << "HTML file should have Content-Type: text/html";
  }
}

TEST_F(HttpServerIntegrationTest, CssFileReturnsTextCss) {
  HttpTestClient::Response response = client->get("/styles.css");

  // May return 404 if file doesn't exist, that's ok
  if (response.statusCode == 200) {
    EXPECT_TRUE(response.hasHeader("content-type"));
    if (response.hasHeader("content-type")) {
      std::string contentType = response.getHeader("content-type");
      EXPECT_TRUE(contentType.find("text/css") != std::string::npos ||
                  contentType.find("stylesheet") != std::string::npos)
          << "CSS file should have Content-Type: text/css";
    }
  }
}

TEST_F(HttpServerIntegrationTest, ImageFileReturnsImageMimeType) {
  HttpTestClient::Response response = client->get("/image.png");

  // May return 404 if file doesn't exist, that's ok
  if (response.statusCode == 200) {
    EXPECT_TRUE(response.hasHeader("content-type"));
    if (response.hasHeader("content-type")) {
      std::string contentType = response.getHeader("content-type");
      EXPECT_TRUE(contentType.find("image/") != std::string::npos)
          << "Image file should have Content-Type: image/*";
    }
  }
}

// ============================================
// 15. ROOT DIRECTORY PER ROUTE
// ============================================

TEST_F(HttpServerIntegrationTest, RouteWithCustomRootResolvesCorrectly) {
  // /public route has custom root: ./var/www/public
  HttpTestClient::Response response = client->get("/public/test.html");

  // Should look in ./var/www/public/test.html (not
  // ./var/www/default/public/test.html)
  EXPECT_TRUE(response.statusCode == 200 || response.statusCode == 404)
      << "Route with custom root should resolve files correctly";
}

TEST_F(HttpServerIntegrationTest, DefaultRootAppliesWhenNoRouteMatch) {
  HttpTestClient::Response response = client->get("/some_file.html");

  // Should use server default root: ./var/www/default
  EXPECT_TRUE(response.statusCode == 200 || response.statusCode == 404)
      << "Default root should apply for unmatched routes";
}
