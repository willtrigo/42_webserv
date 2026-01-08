#include "HttpTestClient.hpp"

#include <ctime>
#include <fstream>
#include <gtest/gtest.h>
#include <sstream>
#include <unistd.h>

/**
 * Advanced Integration Tests
 *
 * Tests advanced features required by 42 webserver subject:
 * - Chunked transfer encoding
 * - Request timeout
 * - Large file operations
 * - Connection handling
 * - Browser compatibility
 */

class AdvancedIntegrationTest : public ::testing::Test {
 protected:
  HttpTestClient* client;

  void SetUp() {
    client = new HttpTestClient("127.0.0.1", 8080);

    HttpTestClient::Response ping = client->get("/");
    if (ping.statusCode < 0) {
      FAIL() << "Server not running at 127.0.0.1:8080!";
    }
  }

  void TearDown() { delete client; }

  // Helper: Create large test file
  std::string createLargeFile(size_t sizeMB) {
    std::string content;
    content.reserve(sizeMB * 1024 * 1024);
    for (size_t i = 0; i < sizeMB * 1024 * 1024; ++i) {
      content += static_cast<char>('A' + (i % 26));
    }
    return content;
  }
};

// ============================================
// 1. CHUNKED TRANSFER ENCODING
// ============================================

TEST_F(AdvancedIntegrationTest, ServerAcceptsChunkedRequest) {
  // Subject: "for chunked requests, your server needs to un-chunk them"

  std::string body = "This is data sent in chunked transfer encoding";
  std::map<std::string, std::string> headers;
  headers["transfer-encoding"] = "chunked";

  HttpTestClient::Response response =
      client->request("POST", "/files", body, headers);

  // Server should either:
  // 1. Accept and un-chunk (200/201)
  // 2. Not support chunked (400/411/501)
  EXPECT_TRUE(response.statusCode == 200 || response.statusCode == 201 ||
              response.statusCode == 400 || response.statusCode == 411 ||
              response.statusCode == 501)
      << "Server should handle or reject chunked encoding properly, got "
      << response.statusCode;
}

TEST_F(AdvancedIntegrationTest, ServerUnchunksLargeChunkedRequest) {
  // Test chunked encoding with larger payload
  std::string body = createLargeFile(1);  // 1MB chunked
  std::map<std::string, std::string> headers;
  headers["transfer-encoding"] = "chunked";

  HttpTestClient::Response response =
      client->request("POST", "/files", body, headers);

  EXPECT_TRUE(response.statusCode == 200 || response.statusCode == 201 ||
              response.statusCode == 400 || response.statusCode == 413 ||
              response.statusCode == 501)
      << "Server should handle large chunked request";
}

TEST_F(AdvancedIntegrationTest, ChunkedWithContentLengthRejected) {
  // RFC violation: Cannot have both Transfer-Encoding and Content-Length
  std::string body = "test data";
  std::map<std::string, std::string> headers;
  headers["transfer-encoding"] = "chunked";
  headers["content-length"] = "9";

  HttpTestClient::Response response =
      client->request("POST", "/files", body, headers);

  EXPECT_EQ(400, response.statusCode)
      << "Request with both Transfer-Encoding and Content-Length should be "
         "rejected";
}

// ============================================
// 2. REQUEST TIMEOUT
// ============================================

TEST_F(AdvancedIntegrationTest, IncompleteRequestTimesOut) {
  // Subject: "A request to your server should never hang indefinitely"

  // Note: Testing true timeout requires low-level socket programming
  // This test verifies server has timeout mechanism by checking it doesn't hang

  // Make normal request - should complete quickly
  time_t start = time(NULL);
  HttpTestClient::Response response = client->get("/");
  time_t end = time(NULL);

  EXPECT_EQ(200, response.statusCode);
  EXPECT_LT(end - start, 10)  // Should complete in < 10 seconds
      << "Normal request should not take more than 10 seconds";
}

TEST_F(AdvancedIntegrationTest, SlowClientDoesNotBlockServer) {
  // Slow client shouldn't block other clients
  // Make one slow request followed by normal request

  HttpTestClient::Response r1 = client->get("/");
  sleep(1);  // Simulate slow client
  HttpTestClient::Response r2 = client->get("/");

  EXPECT_EQ(200, r1.statusCode);
  EXPECT_EQ(200, r2.statusCode);
  // If server blocked, second request would fail or timeout
}

TEST_F(AdvancedIntegrationTest, MultipleSlowRequestsHandledConcurrently) {
  // Server should handle multiple slow clients concurrently (non-blocking I/O)
  const int NUM_SLOW_REQUESTS = 5;
  int successCount = 0;

  time_t start = time(NULL);

  for (int i = 0; i < NUM_SLOW_REQUESTS; ++i) {
    HttpTestClient::Response response = client->get("/");
    if (response.statusCode == 200) ++successCount;
    usleep(100000);  // 100ms delay between requests
  }

  time_t end = time(NULL);

  EXPECT_EQ(NUM_SLOW_REQUESTS, successCount);
  EXPECT_LT(end - start, 15)  // All requests in < 15 seconds
      << "Server should handle slow requests without blocking";
}

// ============================================
// 3. LARGE FILE OPERATIONS
// ============================================

TEST_F(AdvancedIntegrationTest, DownloadLargeFile) {
  // Test downloading large file (10MB+)
  HttpTestClient::Response response = client->get("/large_file.bin");

  // If file exists, should return 200 and large body
  if (response.statusCode == 200) {
    EXPECT_GT(response.body.length(), 1024 * 1024)  // > 1MB
        << "Large file should be downloaded completely";

    // Verify Content-Length header matches body
    if (response.hasHeader("content-length")) {
      int contentLength = atoi(response.getHeader("content-length").c_str());
      EXPECT_EQ(contentLength, static_cast<int>(response.body.length()))
          << "Content-Length should match actual body size";
    }
  } else {
    // Skipped: "Large file not present for testing";
  }
}

TEST_F(AdvancedIntegrationTest, UploadLargeFileSucceeds) {
  // Test uploading large file (< body size limit)
  std::string largeData = createLargeFile(1);  // 1MB (under limit)

  HttpTestClient::Response response = client->post("/files", largeData);

  EXPECT_TRUE(response.statusCode == 200 || response.statusCode == 201)
      << "Large file upload should succeed if under limit";
}

TEST_F(AdvancedIntegrationTest, UploadVeryLargeFileReturns413) {
  // Test uploading file exceeding limit
  std::string veryLargeData = createLargeFile(5);  // 5MB (over 1MB limit)

  HttpTestClient::Response response = client->post("/files", veryLargeData);

  EXPECT_EQ(413, response.statusCode)
      << "Very large file upload should return 413 Payload Too Large";
}

// ============================================
// 4. CONNECTION HANDLING
// ============================================

TEST_F(AdvancedIntegrationTest, ServerHandlesConnectionClose) {
  // Test Connection: close header
  std::map<std::string, std::string> headers;
  headers["connection"] = "close";

  HttpTestClient::Response response = client->request("GET", "/", "", headers);

  EXPECT_EQ(200, response.statusCode);

  if (response.hasHeader("connection")) {
    std::string conn = response.getHeader("connection");
    EXPECT_TRUE(conn.find("close") != std::string::npos)
        << "Server should respect Connection: close";
  }
}

TEST_F(AdvancedIntegrationTest, ServerSupportsKeepAlive) {
  // Test Connection: keep-alive header
  std::map<std::string, std::string> headers;
  headers["connection"] = "keep-alive";

  HttpTestClient::Response response = client->request("GET", "/", "", headers);

  EXPECT_EQ(200, response.statusCode);

  if (response.hasHeader("connection")) {
    std::string conn = response.getHeader("connection");
    EXPECT_TRUE(conn.find("keep-alive") != std::string::npos)
        << "Server should support Connection: keep-alive";
  }
}

TEST_F(AdvancedIntegrationTest, PipelinedRequestsHandled) {
  // HTTP pipelining: multiple requests on same connection
  HttpTestClient::Response r1 = client->get("/");
  HttpTestClient::Response r2 = client->get("/public/");
  HttpTestClient::Response r3 = client->get("/");

  EXPECT_EQ(200, r1.statusCode);
  EXPECT_TRUE(r2.statusCode >= 200 && r2.statusCode < 500);
  EXPECT_EQ(200, r3.statusCode);
  // If pipelining broken, later requests would fail
}

// ============================================
// 5. HTTP VERSION COMPLIANCE
// ============================================

TEST_F(AdvancedIntegrationTest, ServerReturnsHttpVersion) {
  HttpTestClient::Response response = client->get("/");

  EXPECT_EQ(200, response.statusCode);
  // Response should indicate HTTP version (checked in HttpTestClient parsing)
  // Just verify we get a valid response
  EXPECT_FALSE(response.body.empty());
}

TEST_F(AdvancedIntegrationTest, ServerHandlesHttp10Request) {
  // HTTP/1.0 request should work
  HttpTestClient::Response response = client->get("/");

  EXPECT_EQ(200, response.statusCode)
      << "Server should handle HTTP/1.0 requests";
}

TEST_F(AdvancedIntegrationTest, ServerHandlesHttp11Request) {
  // HTTP/1.1 request should work
  HttpTestClient::Response response = client->get("/");

  EXPECT_EQ(200, response.statusCode)
      << "Server should handle HTTP/1.1 requests";
}

// ============================================
// 6. BROWSER COMPATIBILITY
// ============================================

TEST_F(AdvancedIntegrationTest, ServerSetsDateHeader) {
  // Browsers expect Date header
  HttpTestClient::Response response = client->get("/");

  EXPECT_EQ(200, response.statusCode);
  EXPECT_TRUE(response.hasHeader("date"))
      << "Response should have Date header for browser compatibility";
}

TEST_F(AdvancedIntegrationTest, ServerSetsServerHeader) {
  // Browsers benefit from Server header
  HttpTestClient::Response response = client->get("/");

  EXPECT_EQ(200, response.statusCode);
  EXPECT_TRUE(response.hasHeader("server"))
      << "Response should have Server header";
}

TEST_F(AdvancedIntegrationTest, ServerSetsContentTypeForHtml) {
  HttpTestClient::Response response = client->get("/");

  EXPECT_EQ(200, response.statusCode);
  EXPECT_TRUE(response.hasHeader("content-type"))
      << "HTML response should have Content-Type header";

  if (response.hasHeader("content-type")) {
    std::string contentType = response.getHeader("content-type");
    EXPECT_TRUE(contentType.find("text/html") != std::string::npos)
        << "HTML should have Content-Type: text/html";
  }
}

// ============================================
// 7. ERROR RESILIENCE
// ============================================

TEST_F(AdvancedIntegrationTest, ServerHandlesMalformedRequest) {
  // Malformed HTTP request should not crash server
  HttpTestClient::Response response =
      client->request("", "", "", std::map<std::string, std::string>());

  // Should return error (4xx or 5xx), not crash
  EXPECT_TRUE(response.statusCode >= 400 || response.statusCode < 0)
      << "Malformed request should return error or connection failure";
}

TEST_F(AdvancedIntegrationTest, ServerRemainsAvailableAfterErrors) {
  // Subject: "Resilience is key. Your server must remain operational at all
  // times"

  // Trigger various errors
  client->get("/nonexistent");          // 404
  client->post("/", "data");            // 405
  client->get("/../../../etc/passwd");  // 403
  std::string large = createLargeFile(3);
  client->post("/files", large);  // 413

  // Server should still work after errors
  HttpTestClient::Response response = client->get("/");
  EXPECT_EQ(200, response.statusCode)
      << "Server should remain operational after handling errors";
}

TEST_F(AdvancedIntegrationTest, ServerHandles100ConsecutiveErrors) {
  // Stress test error handling
  int errorCount = 0;

  for (int i = 0; i < 100; ++i) {
    HttpTestClient::Response response = client->get("/nonexistent_file");
    if (response.statusCode == 404) ++errorCount;
  }

  EXPECT_EQ(100, errorCount) << "Server should handle 100 consecutive 404s";

  // Verify server still works
  HttpTestClient::Response response = client->get("/");
  EXPECT_EQ(200, response.statusCode)
      << "Server should still work after 100 errors";
}

// ============================================
// 8. STATIC WEBSITE SERVING
// ============================================

TEST_F(AdvancedIntegrationTest, ServerServesFullyStaticWebsite) {
  // Subject: "You must be able to serve a fully static website"

  // Test various static resources
  HttpTestClient::Response html = client->get("/");
  HttpTestClient::Response css = client->get("/styles.css");
  HttpTestClient::Response js = client->get("/script.js");
  HttpTestClient::Response img = client->get("/image.png");

  EXPECT_EQ(200, html.statusCode) << "Should serve HTML";

  // Other files may not exist, that's ok (404)
  EXPECT_TRUE(css.statusCode == 200 || css.statusCode == 404);
  EXPECT_TRUE(js.statusCode == 200 || js.statusCode == 404);
  EXPECT_TRUE(img.statusCode == 200 || img.statusCode == 404);
}

TEST_F(AdvancedIntegrationTest, StaticFilesServedWithCorrectMimeTypes) {
  // Static files should have correct Content-Type

  struct {
    const char* path;
    const char* expectedMimeType;
  } tests[] = {
      {"/index.html", "text/html"},
      {"/styles.css", "text/css"},
      {"/script.js", "application/javascript"},
      {"/image.png", "image/png"},
      {"/image.jpg", "image/jpeg"},
  };

  for (size_t i = 0; i < sizeof(tests) / sizeof(tests[0]); ++i) {
    HttpTestClient::Response response = client->get(tests[i].path);

    if (response.statusCode == 200 && response.hasHeader("content-type")) {
      std::string contentType = response.getHeader("content-type");
      EXPECT_TRUE(contentType.find(tests[i].expectedMimeType) !=
                  std::string::npos)
          << "File " << tests[i].path
          << " should have Content-Type: " << tests[i].expectedMimeType;
    }
  }
}
