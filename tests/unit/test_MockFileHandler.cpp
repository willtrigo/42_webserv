/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_MockFileHandler.cpp                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: umeneses <umeneses@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/01 00:00:00 by copilot           #+#    #+#             */
/*   Updated: 2026/01/01 19:06:45 by umeneses         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <gtest/gtest.h>
#include "mocks/MockFileHandler.hpp"

/**
 * Tests based on UploadConfig test scenarios from test_UploadConfig.cpp
 * 
 * These tests verify that MockFileHandler provides the correct behavior
 * needed to support UploadConfig operations:
 * 
 * From test_UploadConfig.cpp sections:
 * - Construction Tests → File system initialization
 * - Upload Operations Tests → Write, read, validate files
 * - Extension Management → MIME type detection
 * - Validation Tests → Size checks, existence checks
 */

class MockFileHandlerTest : public ::testing::Test {
 protected:
  void SetUp() {
    handler = new MockFileHandler();
  }
  
  void TearDown() {
    delete handler;
    handler = NULL;
  }
  
  MockFileHandler* handler;
};

// ============================================================================
// Write Operations Tests (supports processUpload scenarios)
// ============================================================================

TEST_F(MockFileHandlerTest, WriteFileBasic) {
  // From: UploadConfigTest.ProcessUploadBasic
  std::vector<char> data;
  data.push_back('t');
  data.push_back('e');
  data.push_back('s');
  data.push_back('t');
  
  bool result = handler->writeFile("/var/uploads/test.txt", data);
  
  EXPECT_TRUE(result);
  EXPECT_EQ(1, handler->getCallCount("writeFile"));
  EXPECT_TRUE(handler->fileExists("/var/uploads/test.txt"));
  EXPECT_EQ(4u, handler->getFileSize("/var/uploads/test.txt"));
}

TEST_F(MockFileHandlerTest, WriteFileString) {
  // Support string-based writes
  std::string data = "test content";
  
  bool result = handler->writeFile("/var/uploads/file.txt", data);
  
  EXPECT_TRUE(result);
  EXPECT_EQ(12u, handler->getFileSize("/var/uploads/file.txt"));
}

TEST_F(MockFileHandlerTest, WriteFileFailure) {
  // Test write failure scenario (disk full, permissions, etc.)
  handler->setWriteSuccess(false);
  
  std::vector<char> data(100, 'x');
  bool result = handler->writeFile("/var/uploads/fail.txt", data);
  
  EXPECT_FALSE(result);
  EXPECT_FALSE(handler->fileExists("/var/uploads/fail.txt"));
}

TEST_F(MockFileHandlerTest, WriteFileChunk) {
  // For large file uploads
  std::vector<char> chunk1(100, 'a');
  std::vector<char> chunk2(100, 'b');
  
  size_t written1 = handler->writeFileChunk("/var/uploads/large.dat", chunk1, 0);
  size_t written2 = handler->writeFileChunk("/var/uploads/large.dat", chunk2, 100);
  
  EXPECT_EQ(100u, written1);
  EXPECT_EQ(100u, written2);
  EXPECT_EQ(200u, handler->getFileSize("/var/uploads/large.dat"));
  EXPECT_EQ(2, handler->getCallCount("writeFileChunk"));
}

// ============================================================================
// Read Operations Tests (supports validation scenarios)
// ============================================================================

TEST_F(MockFileHandlerTest, ReadFileBasic) {
  // Setup: Write file first
  std::vector<char> original;
  original.push_back('d');
  original.push_back('a');
  original.push_back('t');
  original.push_back('a');
  handler->writeFile("/var/uploads/read.txt", original);
  
  // Test: Read it back
  std::vector<char> read = handler->readFile("/var/uploads/read.txt");
  
  EXPECT_EQ(original, read);
  EXPECT_EQ(1, handler->getCallCount("readFile"));
}

TEST_F(MockFileHandlerTest, ReadFileAsString) {
  // Support string-based reads
  std::string data = "hello world";
  handler->writeFile("/var/uploads/text.txt", data);
  
  std::string read = handler->readFileAsString("/var/uploads/text.txt");
  
  EXPECT_EQ(data, read);
}

TEST_F(MockFileHandlerTest, ReadFileNotFound) {
  // Test read non-existent file
  std::vector<char> data = handler->readFile("/nonexistent.txt");
  
  EXPECT_TRUE(data.empty());
}

TEST_F(MockFileHandlerTest, ReadFileChunk) {
  // For partial reads
  std::string data = "0123456789abcdef";
  handler->writeFile("/var/uploads/chunk.txt", data);
  
  std::vector<char> chunk = handler->readFileChunk("/var/uploads/chunk.txt", 5, 5);
  
  EXPECT_EQ(5u, chunk.size());
  EXPECT_EQ('5', chunk[0]);
  EXPECT_EQ('9', chunk[4]);
}

// ============================================================================
// File Existence Tests (supports validation scenarios)
// ============================================================================

TEST_F(MockFileHandlerTest, FileExistsAfterWrite) {
  std::vector<char> data(10, 'x');
  handler->writeFile("/var/uploads/exists.txt", data);
  
  EXPECT_TRUE(handler->fileExists("/var/uploads/exists.txt"));
  EXPECT_FALSE(handler->fileExists("/var/uploads/notexists.txt"));
}

TEST_F(MockFileHandlerTest, FileExistsConfigurable) {
  // Allow tests to configure file existence independently
  handler->setFileExists("/configured/file.txt", true);
  
  EXPECT_TRUE(handler->fileExists("/configured/file.txt"));
  EXPECT_EQ(0u, handler->getFileSize("/configured/file.txt"));
}

// ============================================================================
// File Size Tests (supports ProcessUploadFileTooLarge scenario)
// ============================================================================

TEST_F(MockFileHandlerTest, GetFileSize) {
  // From: UploadConfigTest.ProcessUploadFileTooLarge
  std::vector<char> smallFile(10, 'x');
  std::vector<char> largeFile(1000, 'x');
  
  handler->writeFile("/var/uploads/small.txt", smallFile);
  handler->writeFile("/var/uploads/large.txt", largeFile);
  
  EXPECT_EQ(10u, handler->getFileSize("/var/uploads/small.txt"));
  EXPECT_EQ(1000u, handler->getFileSize("/var/uploads/large.txt"));
  EXPECT_EQ(0u, handler->getFileSize("/nonexistent.txt"));
}

TEST_F(MockFileHandlerTest, GetAvailableDiskSpace) {
  // Default is 1GB
  EXPECT_EQ(1024u * 1024u * 1024u, 
            handler->getAvailableDiskSpace("/var/uploads"));
  
  // Can be configured
  handler->setAvailableDiskSpace(100 * 1024 * 1024);  // 100MB
  EXPECT_EQ(100u * 1024u * 1024u, 
            handler->getAvailableDiskSpace("/var/uploads"));
}

// ============================================================================
// MIME Type Detection Tests (supports extension validation)
// ============================================================================

TEST_F(MockFileHandlerTest, DetectMimeTypeText) {
  // From: Extension Management Tests in test_UploadConfig.cpp
  EXPECT_EQ("text/plain", handler->detectMimeType("file.txt"));
  EXPECT_EQ("text/html", handler->detectMimeType("page.html"));
  EXPECT_EQ("text/css", handler->detectMimeType("style.css"));
  EXPECT_EQ("application/javascript", handler->detectMimeType("script.js"));
}

TEST_F(MockFileHandlerTest, DetectMimeTypeImages) {
  EXPECT_EQ("image/jpeg", handler->detectMimeType("photo.jpg"));
  EXPECT_EQ("image/jpeg", handler->detectMimeType("photo.jpeg"));
  EXPECT_EQ("image/png", handler->detectMimeType("logo.png"));
  EXPECT_EQ("image/gif", handler->detectMimeType("animation.gif"));
}

TEST_F(MockFileHandlerTest, DetectMimeTypeBinary) {
  EXPECT_EQ("application/pdf", handler->detectMimeType("document.pdf"));
  EXPECT_EQ("application/zip", handler->detectMimeType("archive.zip"));
  EXPECT_EQ("application/octet-stream", handler->detectMimeType("file.unknown"));
}

TEST_F(MockFileHandlerTest, DetectMimeTypeNoExtension) {
  EXPECT_EQ("application/octet-stream", handler->detectMimeType("README"));
  EXPECT_EQ("application/octet-stream", handler->detectMimeType("Makefile"));
}

// ============================================================================
// File Metadata Tests (supports validation scenarios)
// ============================================================================

TEST_F(MockFileHandlerTest, GetMetadata) {
  std::string data = "test content";
  handler->writeFile("/var/uploads/meta.txt", data);
  
  std::map<std::string, std::string> meta = 
      handler->getMetadata("/var/uploads/meta.txt");
  
  EXPECT_EQ("12", meta["size"]);
  EXPECT_EQ("text/plain", meta["mimeType"]);
  EXPECT_EQ("644", meta["permissions"]);
  EXPECT_FALSE(meta["modified"].empty());
}

TEST_F(MockFileHandlerTest, GetMetadataNotFound) {
  std::map<std::string, std::string> meta = 
      handler->getMetadata("/nonexistent.txt");
  
  EXPECT_TRUE(meta.empty());
}

TEST_F(MockFileHandlerTest, CalculateChecksum) {
  std::vector<char> data(100, 'x');
  handler->writeFile("/var/uploads/checksum.txt", data);
  
  std::string checksum = handler->calculateChecksum("/var/uploads/checksum.txt");
  
  EXPECT_FALSE(checksum.empty());
  EXPECT_EQ("checksum_100", checksum);
}

// ============================================================================
// File Management Tests (supports cleanup scenarios)
// ============================================================================

TEST_F(MockFileHandlerTest, DeleteFile) {
  std::vector<char> data(10, 'x');
  handler->writeFile("/var/uploads/delete.txt", data);
  
  EXPECT_TRUE(handler->fileExists("/var/uploads/delete.txt"));
  
  bool deleted = handler->deleteFile("/var/uploads/delete.txt");
  
  EXPECT_TRUE(deleted);
  EXPECT_FALSE(handler->fileExists("/var/uploads/delete.txt"));
  EXPECT_EQ(1, handler->getCallCount("deleteFile"));
}

TEST_F(MockFileHandlerTest, DeleteFileNotFound) {
  bool deleted = handler->deleteFile("/nonexistent.txt");
  EXPECT_FALSE(deleted);
}

TEST_F(MockFileHandlerTest, RenameFile) {
  std::string data = "content";
  handler->writeFile("/var/uploads/old.txt", data);
  
  bool renamed = handler->renameFile("/var/uploads/old.txt", 
                                      "/var/uploads/new.txt");
  
  EXPECT_TRUE(renamed);
  EXPECT_FALSE(handler->fileExists("/var/uploads/old.txt"));
  EXPECT_TRUE(handler->fileExists("/var/uploads/new.txt"));
  EXPECT_EQ(data, handler->readFileAsString("/var/uploads/new.txt"));
}

// ============================================================================
// Call Tracking Tests (verify mock behavior)
// ============================================================================

TEST_F(MockFileHandlerTest, TrackMultipleCalls) {
  std::vector<char> data(10, 'x');
  
  handler->writeFile("/file1.txt", data);
  handler->writeFile("/file2.txt", data);
  handler->readFile("/file1.txt");
  handler->deleteFile("/file1.txt");
  
  EXPECT_EQ(2, handler->getCallCount("writeFile"));
  EXPECT_EQ(1, handler->getCallCount("readFile"));
  EXPECT_EQ(1, handler->getCallCount("deleteFile"));
}

TEST_F(MockFileHandlerTest, GetCallHistory) {
  std::vector<char> data(10, 'x');
  
  handler->writeFile("/test.txt", data);
  handler->readFile("/test.txt");
  handler->deleteFile("/test.txt");
  
  std::vector<std::string> history = handler->getCalledMethods();
  
  EXPECT_EQ(3u, history.size());
  EXPECT_EQ("writeFile", history[0]);
  EXPECT_EQ("readFile", history[1]);
  EXPECT_EQ("deleteFile", history[2]);
}

TEST_F(MockFileHandlerTest, ClearCallHistory) {
  std::vector<char> data(10, 'x');
  handler->writeFile("/test.txt", data);
  
  EXPECT_EQ(1, handler->getCallCount("writeFile"));
  
  handler->clearCallHistory();
  
  EXPECT_EQ(0, handler->getCallCount("writeFile"));
  EXPECT_TRUE(handler->getCalledMethods().empty());
}

// ============================================================================
// Mock Configuration Tests
// ============================================================================

TEST_F(MockFileHandlerTest, ConfigurableWriteFailure) {
  handler->setWriteSuccess(false);
  
  std::vector<char> data(10, 'x');
  bool result = handler->writeFile("/test.txt", data);
  
  EXPECT_FALSE(result);
}

TEST_F(MockFileHandlerTest, ConfigurableReadFailure) {
  std::vector<char> data(10, 'x');
  handler->writeFile("/test.txt", data);
  
  handler->setReadSuccess(false);
  std::vector<char> read = handler->readFile("/test.txt");
  
  EXPECT_TRUE(read.empty());
}

TEST_F(MockFileHandlerTest, GetMockFiles) {
  std::vector<char> data1(10, 'a');
  std::vector<char> data2(20, 'b');
  
  handler->writeFile("/file1.txt", data1);
  handler->writeFile("/file2.txt", data2);
  
  const std::map<std::string, std::vector<char> >& files = 
      handler->getMockFiles();
  
  EXPECT_EQ(2u, files.size());
  EXPECT_EQ(10u, files.find("/file1.txt")->second.size());
  EXPECT_EQ(20u, files.find("/file2.txt")->second.size());
}

TEST_F(MockFileHandlerTest, SetMockFile) {
  // Directly inject file into mock storage
  std::vector<char> data(100, 'x');
  handler->setMockFile("/injected.txt", data);
  
  EXPECT_TRUE(handler->fileExists("/injected.txt"));
  EXPECT_EQ(100u, handler->getFileSize("/injected.txt"));
  EXPECT_EQ(0, handler->getCallCount("writeFile"));  // Not counted as write
}
