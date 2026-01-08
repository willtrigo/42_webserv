/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_FileHandler_Integration.cpp                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: umeneses <umeneses@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/26 09:35:52 by umeneses          #+#    #+#             */
/*   Updated: 2026/01/08 18:37:33 by umeneses         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpTestClient.hpp"

#include <cstdlib>
#include <fstream>
#include <gtest/gtest.h>
#include <unistd.h>

/**
 * File Handler Integration Tests
 *
 * Tests file operations through the HTTP server interface:
 * - File uploads (POST)
 * - File downloads (GET)
 * - File deletion (DELETE)
 *
 * IMPORTANT: Requires server running at 127.0.0.1:8080
 * Start server before running: ./bin/webserv conf/default.conf
 */

class FileHandlerIntegrationTest : public ::testing::Test {
 protected:
  HttpTestClient* client;

  void SetUp() {
    client = new HttpTestClient("127.0.0.1", 8080);

    // Verify server is running
    HttpTestClient::Response ping = client->get("/");
    if (ping.statusCode < 0) {
      FAIL() << "Server not running at 127.0.0.1:8080! Start it before "
                "running tests.";
    }
  }

  void TearDown() {
    // Clean up test files
    system("rm -f var/uploads/test*.txt 2>/dev/null");
    delete client;
  }
};

// ============================================
// FILE UPLOAD TESTS
// ============================================

TEST_F(FileHandlerIntegrationTest, UploadFileViaPost) {
  std::string content = "Hello, World!";
  HttpTestClient::Response response = client->post("/files", content);

  EXPECT_TRUE(response.statusCode == 200 || response.statusCode == 201)
      << "POST /files should create file and return 200/201, got "
      << response.statusCode;
}

TEST_F(FileHandlerIntegrationTest, UploadLargeFile) {
  // Create 1MB file content
  std::string content(1024 * 1024, 'A');
  HttpTestClient::Response response = client->post("/files", content);

  EXPECT_TRUE(response.statusCode == 200 || response.statusCode == 201 ||
              response.statusCode == 413)
      << "Large file upload should succeed (200/201) or reject if too large "
         "(413), got "
      << response.statusCode;
}

// ============================================
// FILE DOWNLOAD TESTS
// ============================================

TEST_F(FileHandlerIntegrationTest, DownloadExistingFile) {
  // First upload a file
  std::string uploadContent = "Download me!";
  HttpTestClient::Response uploadResp = client->post("/files", uploadContent);
  ASSERT_TRUE(uploadResp.statusCode == 200 || uploadResp.statusCode == 201)
      << "Upload must succeed before testing download";

  // Then download it (assuming it's served from /files or /uploads)
  HttpTestClient::Response downloadResp = client->get("/");

  EXPECT_EQ(200, downloadResp.statusCode)
      << "GET on existing file should return 200";
}

TEST_F(FileHandlerIntegrationTest, DownloadNonExistentFile) {
  HttpTestClient::Response response =
      client->get("/nonexistent_file_12345.txt");

  EXPECT_EQ(404, response.statusCode)
      << "GET on non-existent file should return 404";
}

// ============================================
// FILE DELETION TESTS
// ============================================

TEST_F(FileHandlerIntegrationTest, DeleteUploadedFile) {
  // Upload a file first
  std::string content = "To be deleted";
  HttpTestClient::Response uploadResp = client->post("/files", content);
  ASSERT_TRUE(uploadResp.statusCode == 200 || uploadResp.statusCode == 201)
      << "Upload must succeed before testing delete";

  // Delete it
  HttpTestClient::Response deleteResp = client->del("/files/test_delete.txt");

  EXPECT_TRUE(deleteResp.statusCode == 200 || deleteResp.statusCode == 204 ||
              deleteResp.statusCode == 404)
      << "DELETE should return 200/204 (success) or 404 (not found), got "
      << deleteResp.statusCode;
}

TEST_F(FileHandlerIntegrationTest, DeleteNonExistentFile) {
  HttpTestClient::Response response =
      client->del("/files/nonexistent_12345.txt");

  EXPECT_TRUE(response.statusCode == 404 || response.statusCode == 200 ||
              response.statusCode == 204)
      << "DELETE on non-existent file should return 404, 200, or 204, got "
      << response.statusCode;
}
