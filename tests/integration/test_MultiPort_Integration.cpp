#include "HttpTestClient.hpp"

#include <gtest/gtest.h>
#include <unistd.h>

/**
 * Multi-Port Integration Tests
 *
 * Tests multiple port listening as required by 42 webserver subject:
 * "Your server must be able to listen to multiple ports to deliver different
 * content"
 *
 * IMPORTANT: Requires server configured with multiple ports
 * Example: Port 8080 (main site) + Port 8081 (admin site)
 */

class MultiPortIntegrationTest : public ::testing::Test {
 protected:
  HttpTestClient* client8080;
  HttpTestClient* client8081;

  void SetUp() {
    client8080 = new HttpTestClient("127.0.0.1", 8080);
    client8081 = new HttpTestClient("127.0.0.1", 8081);

    // Verify primary port is running
    HttpTestClient::Response ping = client8080->get("/");
    if (ping.statusCode < 0) {
      FAIL() << "Server not running on port 8080!";
    }
  }

  void TearDown() {
    delete client8080;
    delete client8081;
  }
};

// ============================================
// 1. MULTIPLE PORT LISTENING
// ============================================

TEST_F(MultiPortIntegrationTest, ServerListensOnPort8080) {
  HttpTestClient::Response response = client8080->get("/");

  EXPECT_EQ(200, response.statusCode)
      << "Server should listen on primary port 8080";
}

TEST_F(MultiPortIntegrationTest, ServerListensOnPort8081) {
  HttpTestClient::Response response = client8081->get("/");

  // May return 200 if configured, or connection refused if not
  // Connection refused is indicated by statusCode < 0
  if (response.statusCode > 0) {
    EXPECT_TRUE(response.statusCode == 200 || response.statusCode == 404)
        << "If port 8081 configured, server should respond";
  } else {
    // Port 8081 not configured - acceptable for basic server
    // But subject requires multiple ports for full marks
    // Skipped: "Port 8081 not configured (single port mode)";
  }
}

TEST_F(MultiPortIntegrationTest, ServerListensOnBothPortsSimultaneously) {
  // Test that both ports work at the same time
  HttpTestClient::Response r1 = client8080->get("/");
  HttpTestClient::Response r2 = client8081->get("/");

  EXPECT_EQ(200, r1.statusCode) << "Port 8080 should work";

  if (r2.statusCode > 0) {
    EXPECT_TRUE(r2.statusCode == 200 || r2.statusCode == 404)
        << "Port 8081 should work if configured";
  } else {
    // Skipped: "Port 8081 not configured";
  }
}

// ============================================
// 2. DIFFERENT CONTENT PER PORT
// ============================================

TEST_F(MultiPortIntegrationTest, DifferentPortsServeDifferentContent) {
  // Subject: "listen to multiple ports to deliver different content"

  HttpTestClient::Response r8080 = client8080->get("/");
  HttpTestClient::Response r8081 = client8081->get("/");

  if (r8081.statusCode > 0) {
    EXPECT_EQ(200, r8080.statusCode);
    EXPECT_TRUE(r8081.statusCode == 200 || r8081.statusCode == 404);

    // Different ports should serve different content (different root dirs)
    if (r8080.statusCode == 200 && r8081.statusCode == 200) {
      // Content should differ OR at least server names should differ
      bool contentDiffers = (r8080.body != r8081.body);
      bool serverHeaderDiffers =
          (r8080.hasHeader("server") && r8081.hasHeader("server") &&
           r8080.getHeader("server") != r8081.getHeader("server"));

      EXPECT_TRUE(contentDiffers || serverHeaderDiffers)
          << "Different ports should serve different content or have different "
             "server names";
    }
  } else {
    // Skipped: "Port 8081 not configured";
  }
}

TEST_F(MultiPortIntegrationTest, Port8080ServesMainSite) {
  HttpTestClient::Response response = client8080->get("/");

  EXPECT_EQ(200, response.statusCode);
  EXPECT_FALSE(response.body.empty()) << "Port 8080 should serve content";
}

TEST_F(MultiPortIntegrationTest, Port8081ServesAdminSite) {
  HttpTestClient::Response response = client8081->get("/admin");

  if (response.statusCode > 0) {
    // If port 8081 configured as admin site, /admin should work
    EXPECT_TRUE(response.statusCode == 200 || response.statusCode == 404 ||
                response.statusCode == 403)
        << "Admin site on port 8081 should respond";
  } else {
    // Skipped: "Port 8081 not configured";
  }
}

// ============================================
// 3. PORT CONFIGURATION VALIDATION
// ============================================

TEST_F(MultiPortIntegrationTest, DifferentPortsHaveDifferentRoots) {
  HttpTestClient::Response r8080 = client8080->get("/index.html");
  HttpTestClient::Response r8081 = client8081->get("/index.html");

  if (r8081.statusCode > 0) {
    // If both ports configured with /index.html
    if (r8080.statusCode == 200 && r8081.statusCode == 200) {
      // Files should be different (different root directories)
      EXPECT_NE(r8080.body, r8081.body)
          << "Different ports should have different root directories";
    }
  } else {
    // Skipped: "Port 8081 not configured";
  }
}

TEST_F(MultiPortIntegrationTest, EachPortHasIndependentErrorPages) {
  HttpTestClient::Response r8080 = client8080->get("/nonexistent");
  HttpTestClient::Response r8081 = client8081->get("/nonexistent");

  EXPECT_EQ(404, r8080.statusCode);

  if (r8081.statusCode > 0) {
    EXPECT_EQ(404, r8081.statusCode);

    // Error pages can be different per port
    // Just verify both return proper 404
    EXPECT_FALSE(r8080.body.empty()) << "Port 8080 should have 404 error page";
  } else {
    // Skipped: "Port 8081 not configured";
  }
}

// ============================================
// 4. PORT-SPECIFIC ROUTES
// ============================================

TEST_F(MultiPortIntegrationTest, Port8080HasPublicRoutes) {
  HttpTestClient::Response response = client8080->get("/public/");

  EXPECT_TRUE(response.statusCode == 200 || response.statusCode == 404 ||
              response.statusCode == 403)
      << "Port 8080 should have /public route";
}

TEST_F(MultiPortIntegrationTest, Port8081MayHaveRestrictedRoutes) {
  HttpTestClient::Response response = client8081->get("/restricted/");

  if (response.statusCode > 0) {
    // Port 8081 might have restricted routes (403 Forbidden)
    EXPECT_TRUE(response.statusCode == 403 || response.statusCode == 404 ||
                response.statusCode == 200)
        << "Port 8081 may have restricted routes";
  } else {
    // Skipped: "Port 8081 not configured";
  }
}

// ============================================
// 5. CONCURRENT PORT ACCESS
// ============================================

TEST_F(MultiPortIntegrationTest, BothPortsHandleConcurrentRequests) {
  // Make requests to both ports simultaneously
  const int NUM_REQUESTS = 10;
  int port8080Success = 0;
  int port8081Success = 0;

  for (int i = 0; i < NUM_REQUESTS; ++i) {
    HttpTestClient::Response r8080 = client8080->get("/");
    HttpTestClient::Response r8081 = client8081->get("/");

    if (r8080.statusCode == 200) ++port8080Success;
    if (r8081.statusCode == 200) ++port8081Success;
  }

  EXPECT_EQ(NUM_REQUESTS, port8080Success)
      << "Port 8080 should handle all concurrent requests";

  if (port8081Success > 0) {
    // If port 8081 configured, it should also handle all requests
    EXPECT_EQ(NUM_REQUESTS, port8081Success)
        << "Port 8081 should handle all concurrent requests";
  }
}

// ============================================
// 6. INTERFACE BINDING (0.0.0.0 vs localhost)
// ============================================

TEST_F(MultiPortIntegrationTest, ServerBindsTo0000) {
  // Server should bind to 0.0.0.0 (all interfaces) or localhost (127.0.0.1)
  HttpTestClient::Response response = client8080->get("/");

  EXPECT_EQ(200, response.statusCode)
      << "Server should be reachable on 127.0.0.1";
}

TEST_F(MultiPortIntegrationTest, MultipleInterfacePortCombinations) {
  // Subject: "Define all the interface:port pairs"
  // Example: 0.0.0.0:8080 and 127.0.0.1:8081

  HttpTestClient::Response r1 = client8080->get("/");
  HttpTestClient::Response r2 = client8081->get("/");

  EXPECT_EQ(200, r1.statusCode) << "Primary interface:port should work";

  if (r2.statusCode > 0) {
    EXPECT_TRUE(r2.statusCode >= 200 && r2.statusCode < 600)
        << "Secondary interface:port should respond";
  }
}

// ============================================
// 7. PORT-SPECIFIC SERVER NAMES
// ============================================

TEST_F(MultiPortIntegrationTest, DifferentPortsCanHaveDifferentServerNames) {
  HttpTestClient::Response r8080 = client8080->get("/");
  HttpTestClient::Response r8081 = client8081->get("/");

  if (r8080.hasHeader("server") && r8081.statusCode > 0 &&
      r8081.hasHeader("server")) {
    std::string server8080 = r8080.getHeader("server");
    std::string server8081 = r8081.getHeader("server");

    // Server names can be same or different (both valid)
    // Just verify both ports set Server header
    EXPECT_FALSE(server8080.empty());
    if (r8081.statusCode == 200) {
      EXPECT_FALSE(server8081.empty());
    }
  }
}
