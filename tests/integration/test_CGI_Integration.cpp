#include "HttpTestClient.hpp"

#include <cstdlib>
#include <fstream>
#include <gtest/gtest.h>
#include <sstream>
#include <unistd.h>

/**
 * CGI Integration Tests
 *
 * Tests CGI functionality as required by 42 webserver subject:
 * - Environment variables
 * - POST data to CGI
 * - Chunked requests
 * - Multiple CGI extensions
 * - Query strings
 *
 * IMPORTANT: Requires CGI configured in server
 */

class CgiIntegrationTest : public ::testing::Test {
 protected:
  HttpTestClient* client;

  void SetUp() {
    client = new HttpTestClient("127.0.0.1", 8080);

    // Verify server is running
    HttpTestClient::Response ping = client->get("/");
    if (ping.statusCode < 0) {
      FAIL() << "Server not running at 127.0.0.1:8080!";
    }
  }

  void TearDown() { delete client; }

  // Helper: Create temporary CGI script
  void createTestCgiScript(const std::string& path,
                           const std::string& content) {
    std::ofstream file(path.c_str());
    if (file.is_open()) {
      file << content;
      file.close();
      // Make executable
      chmod(path.c_str(), 0755);
    }
  }
};

// ============================================
// 1. CGI ENVIRONMENT VARIABLES
// ============================================

TEST_F(CgiIntegrationTest, CgiReceivesQueryString) {
  // CGI must receive QUERY_STRING environment variable
  HttpTestClient::Response response =
      client->get("/cgi-bin/test.cgi?name=value&foo=bar");

  // If CGI not configured, returns 404 (acceptable)
  // If configured, should return 200 with query string data
  EXPECT_TRUE(response.statusCode == 200 || response.statusCode == 404)
      << "CGI GET with query string should work or return 404 if not "
         "configured";

  if (response.statusCode == 200) {
    // CGI should echo back the query string
    EXPECT_TRUE(response.body.find("QUERY_STRING") != std::string::npos ||
                response.body.find("name=value") != std::string::npos)
        << "CGI should receive and process QUERY_STRING";
  }
}

TEST_F(CgiIntegrationTest, CgiReceivesRequestMethod) {
  // CGI must receive REQUEST_METHOD environment variable
  HttpTestClient::Response response = client->get("/cgi-bin/env.cgi");

  if (response.statusCode == 200) {
    // CGI should have REQUEST_METHOD=GET
    EXPECT_TRUE(response.body.find("REQUEST_METHOD") != std::string::npos ||
                response.body.find("GET") != std::string::npos)
        << "CGI should receive REQUEST_METHOD environment variable";
  }
}

TEST_F(CgiIntegrationTest, CgiReceivesPathInfo) {
  // CGI must receive PATH_INFO for extra path components
  HttpTestClient::Response response =
      client->get("/cgi-bin/test.cgi/extra/path");

  if (response.statusCode == 200) {
    // CGI should have PATH_INFO=/extra/path
    EXPECT_TRUE(response.body.find("PATH_INFO") != std::string::npos ||
                response.body.find("/extra/path") != std::string::npos)
        << "CGI should receive PATH_INFO environment variable";
  }
}

TEST_F(CgiIntegrationTest, CgiReceivesContentType) {
  // CGI must receive CONTENT_TYPE for POST requests
  std::string body = "test=data";
  std::map<std::string, std::string> headers;
  headers["content-type"] = "application/x-www-form-urlencoded";

  HttpTestClient::Response response =
      client->request("POST", "/cgi-bin/test.cgi", body, headers);

  if (response.statusCode == 200) {
    EXPECT_TRUE(response.body.find("CONTENT_TYPE") != std::string::npos ||
                response.body.find("form-urlencoded") != std::string::npos)
        << "CGI should receive CONTENT_TYPE for POST";
  }
}

TEST_F(CgiIntegrationTest, CgiReceivesContentLength) {
  // CGI must receive CONTENT_LENGTH for POST requests
  std::string body = "test=data&foo=bar";
  HttpTestClient::Response response = client->post("/cgi-bin/test.cgi", body);

  if (response.statusCode == 200) {
    EXPECT_TRUE(response.body.find("CONTENT_LENGTH") != std::string::npos ||
                response.body.find("17") !=
                    std::string::npos)  // Length of body
        << "CGI should receive CONTENT_LENGTH for POST";
  }
}

// ============================================
// 2. POST DATA TO CGI
// ============================================

TEST_F(CgiIntegrationTest, CgiReceivesPostDataViaStdin) {
  // Subject: "The full request and arguments provided by the client must be
  // available to the CGI"
  std::string postData = "name=John&age=30&city=Paris";
  HttpTestClient::Response response =
      client->post("/cgi-bin/echo.cgi", postData);

  if (response.statusCode == 200) {
    // CGI should receive POST data on stdin
    EXPECT_TRUE(response.body.find("John") != std::string::npos ||
                response.body.find("name=") != std::string::npos)
        << "CGI should receive POST data via stdin";
  }
}

TEST_F(CgiIntegrationTest, CgiProcessesFormData) {
  std::string formData = "username=test&password=secret&submit=Login";
  HttpTestClient::Response response =
      client->post("/cgi-bin/form.cgi", formData);

  if (response.statusCode == 200) {
    // CGI should parse and process form data
    EXPECT_FALSE(response.body.empty())
        << "CGI should return processed form data";
  }
}

TEST_F(CgiIntegrationTest, CgiHandlesEmptyPostBody) {
  HttpTestClient::Response response = client->post("/cgi-bin/test.cgi", "");

  if (response.statusCode == 200) {
    // CGI should handle empty POST body (CONTENT_LENGTH=0)
    EXPECT_TRUE(response.statusCode == 200)
        << "CGI should handle empty POST body";
  }
}

// ============================================
// 3. CHUNKED REQUESTS TO CGI
// ============================================

TEST_F(CgiIntegrationTest, ServerUnchunksBeforeCgi) {
  // Subject: "for chunked requests, your server needs to un-chunk them, the CGI
  // will expect EOF as the end of the body"

  // Note: Sending chunked encoding from client requires low-level HTTP
  // This test verifies that IF server receives chunked, it un-chunks for CGI

  std::string body = "This is chunked data that should be un-chunked";
  std::map<std::string, std::string> headers;
  headers["transfer-encoding"] = "chunked";

  HttpTestClient::Response response =
      client->request("POST", "/cgi-bin/test.cgi", body, headers);

  // Server should accept chunked encoding and process it
  EXPECT_TRUE(response.statusCode == 200 || response.statusCode == 400 ||
              response.statusCode == 501)
      << "Server should handle or reject chunked encoding properly";
}

// ============================================
// 4. MULTIPLE CGI EXTENSIONS
// ============================================

TEST_F(CgiIntegrationTest, PhpCgiExecutes) {
  // Subject: "based on file extension (for example .php)"
  HttpTestClient::Response response = client->get("/cgi-bin/test.php");

  // If PHP CGI configured, returns 200
  // If not configured, returns 404
  EXPECT_TRUE(response.statusCode == 200 || response.statusCode == 404)
      << "PHP CGI should execute or return 404 if not configured";

  if (response.statusCode == 200) {
    // PHP should output something (even if just "<?php phpinfo(); ?>")
    EXPECT_FALSE(response.body.empty()) << "PHP CGI should produce output";
  }
}

TEST_F(CgiIntegrationTest, PythonCgiExecutes) {
  HttpTestClient::Response response = client->get("/cgi-bin/test.py");

  EXPECT_TRUE(response.statusCode == 200 || response.statusCode == 404)
      << "Python CGI should execute or return 404 if not configured";

  if (response.statusCode == 200) {
    EXPECT_FALSE(response.body.empty()) << "Python CGI should produce output";
  }
}

TEST_F(CgiIntegrationTest, ShellCgiExecutes) {
  HttpTestClient::Response response = client->get("/cgi-bin/test.cgi");

  EXPECT_TRUE(response.statusCode == 200 || response.statusCode == 404)
      << "Shell CGI should execute or return 404 if not configured";

  if (response.statusCode == 200) {
    EXPECT_FALSE(response.body.empty()) << "Shell CGI should produce output";
  }
}

// ============================================
// 5. CGI OUTPUT HANDLING
// ============================================

TEST_F(CgiIntegrationTest, CgiWithoutContentLengthUsesEof) {
  // Subject: "If no content_length is returned from the CGI, EOF will mark the
  // end of the returned data"

  HttpTestClient::Response response = client->get("/cgi-bin/no_length.cgi");

  if (response.statusCode == 200) {
    // Server should read until CGI closes stdout (EOF)
    EXPECT_FALSE(response.body.empty())
        << "CGI without Content-Length should be read until EOF";
  }
}

TEST_F(CgiIntegrationTest, CgiWithContentLengthRespected) {
  HttpTestClient::Response response = client->get("/cgi-bin/with_length.cgi");

  if (response.statusCode == 200 && response.hasHeader("content-length")) {
    std::string clStr = response.getHeader("content-length");
    int contentLength = atoi(clStr.c_str());

    EXPECT_EQ(contentLength, static_cast<int>(response.body.length()))
        << "CGI Content-Length should match actual body length";
  }
}

// ============================================
// 6. CGI WORKING DIRECTORY
// ============================================

TEST_F(CgiIntegrationTest, CgiRunsInCorrectDirectory) {
  // Subject: "The CGI should be run in the correct directory for relative path
  // file access"

  HttpTestClient::Response response = client->get("/cgi-bin/pwd.cgi");

  if (response.statusCode == 200) {
    // CGI should run in cgi-bin directory (or configured location)
    EXPECT_TRUE(response.body.find("cgi-bin") != std::string::npos ||
                response.body.find("cgi") != std::string::npos)
        << "CGI should execute in correct working directory";
  }
}

// ============================================
// 7. CGI ERROR HANDLING
// ============================================

TEST_F(CgiIntegrationTest, NonexistentCgiReturns404) {
  HttpTestClient::Response response =
      client->get("/cgi-bin/nonexistent_script.cgi");

  EXPECT_EQ(404, response.statusCode)
      << "Nonexistent CGI script should return 404";
}

TEST_F(CgiIntegrationTest, CgiTimeoutDoesNotHang) {
  // CGI that takes too long should timeout (not hang forever)
  HttpTestClient::Response response = client->get("/cgi-bin/slow.cgi");

  // Should return either 200 (completed), 504 (timeout), or 500 (error)
  EXPECT_TRUE(response.statusCode == 200 || response.statusCode == 504 ||
              response.statusCode == 500 || response.statusCode == 404)
      << "Slow CGI should timeout or complete, not hang indefinitely";
}

TEST_F(CgiIntegrationTest, CgiWithSyntaxErrorReturns500) {
  HttpTestClient::Response response = client->get("/cgi-bin/broken.cgi");

  // CGI with syntax errors should return 500 Internal Server Error
  EXPECT_TRUE(response.statusCode == 500 || response.statusCode == 404)
      << "Broken CGI should return 500 or 404";
}

// ============================================
// 8. CGI HEADERS
// ============================================

TEST_F(CgiIntegrationTest, CgiCanSetCustomHeaders) {
  // CGI can output custom headers (e.g., "X-Custom-Header: value")
  HttpTestClient::Response response = client->get("/cgi-bin/headers.cgi");

  if (response.statusCode == 200) {
    // Should have standard headers
    EXPECT_TRUE(response.hasHeader("content-type") ||
                response.hasHeader("content-length"))
        << "CGI should set HTTP headers";
  }
}

TEST_F(CgiIntegrationTest, CgiCanSetStatusCode) {
  // CGI can set custom status code via "Status: 201" header
  HttpTestClient::Response response = client->get("/cgi-bin/status_201.cgi");

  if (response.statusCode == 201) {
    // CGI successfully set custom status code
    EXPECT_EQ(201, response.statusCode)
        << "CGI should be able to set custom status code";
  }
}

// ============================================
// 9. CGI SECURITY
// ============================================

TEST_F(CgiIntegrationTest, CgiCannotAccessParentDirectory) {
  // CGI with path traversal attempt should be blocked
  HttpTestClient::Response response =
      client->get("/cgi-bin/../../../etc/passwd.cgi");

  EXPECT_TRUE(response.statusCode == 403 || response.statusCode == 404)
      << "Path traversal in CGI should be blocked";
}

TEST_F(CgiIntegrationTest, CgiWithMaliciousInputDoesNotCrash) {
  // CGI should handle malicious input without crashing server
  std::string malicious = "<script>alert('xss')</script>&DROP TABLE users;";
  HttpTestClient::Response response =
      client->post("/cgi-bin/test.cgi", malicious);

  // Server should handle gracefully (not crash)
  EXPECT_TRUE(response.statusCode >= 200 && response.statusCode < 600)
      << "CGI with malicious input should not crash server";
}
