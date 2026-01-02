/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_MockDirectoryLister.cpp                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: umeneses <umeneses@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/01 00:00:00 by copilot           #+#    #+#             */
/*   Updated: 2026/01/01 19:21:23 by umeneses         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <gtest/gtest.h>
#include "mocks/MockDirectoryLister.hpp"

using namespace domain::filesystem::value_objects;
using namespace infrastructure::filesystem::adapters;

class MockDirectoryListerTest : public ::testing::Test {
 protected:
  MockDirectoryLister* mock;

  void SetUp() {
    mock = new MockDirectoryLister();
  }

  void TearDown() {
    delete mock;
  }

  DirectoryEntry createFileEntry(const std::string& name, size_t sizeBytes) {
    return DirectoryEntry(
        name, 
        false,  // not a directory
        Size(sizeBytes),
        "2026-01-01 12:00:00",
        Permission(0644),
        Path("/var/uploads/" + name)
    );
  }

  DirectoryEntry createDirEntry(const std::string& name) {
    return DirectoryEntry(
        name, 
        true,  // is a directory
        Size(4096),
        "2026-01-01 12:00:00",
        Permission(0755),
        Path("/var/uploads/" + name)
    );
  }
};

// ============================================================================
// Directory Listing Tests (Based on UploadConfig.listUploadedFiles)
// ============================================================================

TEST_F(MockDirectoryListerTest, ListDirectoryEmpty) {
  // Based on UploadConfigTest scenario: empty upload directory
  Path uploadDir("/var/uploads");
  mock->setDirectoryExists("/var/uploads", true);

  std::vector<DirectoryEntry> entries = mock->listDirectory(uploadDir, false);

  EXPECT_EQ(0, entries.size());
  EXPECT_EQ(1, mock->getCallCount("listDirectory"));
}

TEST_F(MockDirectoryListerTest, ListDirectoryWithFiles) {
  // Based on UploadConfigTest.listUploadedFiles scenario
  Path uploadDir("/var/uploads");
  
  mock->addMockEntry("/var/uploads", createFileEntry("document.pdf", 1024000));
  mock->addMockEntry("/var/uploads", createFileEntry("image.jpg", 512000));
  mock->addMockEntry("/var/uploads", createFileEntry("report.txt", 8192));

  std::vector<DirectoryEntry> entries = mock->listDirectory(uploadDir, false);

  EXPECT_EQ(3, entries.size());
  EXPECT_EQ("document.pdf", entries[0].m_name);
  EXPECT_EQ("image.jpg", entries[1].m_name);
  EXPECT_EQ("report.txt", entries[2].m_name);
}

TEST_F(MockDirectoryListerTest, ListDirectoryHidesHiddenFiles) {
  // Based on UploadConfig filtering: skip hidden files by default
  Path uploadDir("/var/uploads");
  
  mock->addMockEntry("/var/uploads", createFileEntry("visible.txt", 1024));
  mock->addMockEntry("/var/uploads", createFileEntry(".hidden", 512));
  mock->addMockEntry("/var/uploads", createFileEntry("..config", 256));

  std::vector<DirectoryEntry> entries = mock->listDirectory(uploadDir, false);

  EXPECT_EQ(1, entries.size());
  EXPECT_EQ("visible.txt", entries[0].m_name);
}

TEST_F(MockDirectoryListerTest, ListDirectoryShowsHiddenFilesWhenEnabled) {
  // Based on UploadConfig.listUploadedFiles(showHidden=true)
  Path uploadDir("/var/uploads");
  
  mock->addMockEntry("/var/uploads", createFileEntry("visible.txt", 1024));
  mock->addMockEntry("/var/uploads", createFileEntry(".hidden", 512));

  std::vector<DirectoryEntry> entries = mock->listDirectory(uploadDir, true);

  EXPECT_EQ(2, entries.size());
}

TEST_F(MockDirectoryListerTest, ListDirectorySkipsDirectories) {
  // Based on UploadConfig: skip subdirectories in file listing
  Path uploadDir("/var/uploads");
  
  mock->addMockEntry("/var/uploads", createFileEntry("file1.txt", 1024));
  mock->addMockEntry("/var/uploads", createDirEntry("subdir"));
  mock->addMockEntry("/var/uploads", createFileEntry("file2.txt", 2048));

  std::vector<DirectoryEntry> entries = mock->listDirectory(uploadDir, false);

  // Note: Mock returns all entries; UploadConfig filters directories
  // After sorting by name: file1.txt, file2.txt, subdir
  EXPECT_EQ(3, entries.size());
  EXPECT_FALSE(entries[0].m_isDirectory);  // file1.txt
  EXPECT_FALSE(entries[1].m_isDirectory);  // file2.txt
  EXPECT_TRUE(entries[2].m_isDirectory);   // subdir
}

TEST_F(MockDirectoryListerTest, ListDirectorySortsByName) {
  // Based on DirectoryLister::listDirectory sortBy parameter
  Path uploadDir("/var/uploads");
  
  mock->addMockEntry("/var/uploads", createFileEntry("zebra.txt", 1024));
  mock->addMockEntry("/var/uploads", createFileEntry("apple.txt", 2048));
  mock->addMockEntry("/var/uploads", createFileEntry("banana.txt", 512));

  std::vector<DirectoryEntry> entries = 
      mock->listDirectory(uploadDir, false, "name", true);

  EXPECT_EQ(3, entries.size());
  EXPECT_EQ("apple.txt", entries[0].m_name);
  EXPECT_EQ("banana.txt", entries[1].m_name);
  EXPECT_EQ("zebra.txt", entries[2].m_name);
}

TEST_F(MockDirectoryListerTest, ListDirectorySortsByNameDescending) {
  Path uploadDir("/var/uploads");
  
  mock->addMockEntry("/var/uploads", createFileEntry("zebra.txt", 1024));
  mock->addMockEntry("/var/uploads", createFileEntry("apple.txt", 2048));
  mock->addMockEntry("/var/uploads", createFileEntry("banana.txt", 512));

  std::vector<DirectoryEntry> entries = 
      mock->listDirectory(uploadDir, false, "name", false);

  EXPECT_EQ(3, entries.size());
  EXPECT_EQ("zebra.txt", entries[0].m_name);
  EXPECT_EQ("banana.txt", entries[1].m_name);
  EXPECT_EQ("apple.txt", entries[2].m_name);
}

// ============================================================================
// Directory Validation Tests (Based on UploadConfig.validateUploadDirectory)
// ============================================================================

TEST_F(MockDirectoryListerTest, ValidateDirectoryExists) {
  // Based on UploadConfig validation before listing
  Path uploadDir("/var/uploads");
  mock->setDirectoryExists("/var/uploads", true);

  bool valid = mock->validateDirectoryForListing(uploadDir);

  EXPECT_TRUE(valid);
  EXPECT_EQ(1, mock->getCallCount("validateDirectoryForListing"));
}

TEST_F(MockDirectoryListerTest, ValidateDirectoryNotFound) {
  // Based on DirectoryLister exception: DIRECTORY_NOT_FOUND
  Path uploadDir("/nonexistent");
  mock->setDirectoryExists("/nonexistent", false);

  bool valid = mock->validateDirectoryForListing(uploadDir);

  EXPECT_FALSE(valid);
}

TEST_F(MockDirectoryListerTest, ValidateDirectoryWithMockEntries) {
  // Directory is valid if we have mock entries for it
  Path uploadDir("/var/uploads");
  mock->addMockEntry("/var/uploads", createFileEntry("test.txt", 1024));

  bool valid = mock->validateDirectoryForListing(uploadDir);

  EXPECT_TRUE(valid);
}

TEST_F(MockDirectoryListerTest, ValidateDirectoryFailureMode) {
  // Test configurable validation failure
  Path uploadDir("/var/uploads");
  mock->setDirectoryExists("/var/uploads", true);
  mock->setValidationSuccess(false);

  bool valid = mock->validateDirectoryForListing(uploadDir);

  EXPECT_FALSE(valid);
}

// ============================================================================
// HTML Generation Tests (Based on DirectoryLister.generateHtmlListing)
// ============================================================================

TEST_F(MockDirectoryListerTest, GenerateHtmlListingBasic) {
  Path uploadDir("/var/uploads");
  Path requestPath("/uploads");
  
  mock->addMockEntry("/var/uploads", createFileEntry("test.txt", 1024));

  std::string html = mock->generateHtmlListing(uploadDir, requestPath, false);

  EXPECT_NE(std::string::npos, html.find("<html>"));
  EXPECT_NE(std::string::npos, html.find("Directory Listing"));
  EXPECT_NE(std::string::npos, html.find("test.txt"));
  EXPECT_NE(std::string::npos, html.find("1024 bytes"));
}

TEST_F(MockDirectoryListerTest, GenerateHtmlListingMarksDirectories) {
  Path uploadDir("/var/uploads");
  Path requestPath("/uploads");
  
  mock->addMockEntry("/var/uploads", createFileEntry("file.txt", 1024));
  mock->addMockEntry("/var/uploads", createDirEntry("subdir"));

  std::string html = mock->generateHtmlListing(uploadDir, requestPath, false);

  EXPECT_NE(std::string::npos, html.find("file.txt"));
  EXPECT_NE(std::string::npos, html.find("subdir/"));  // Directory marked with /
}

// ============================================================================
// Mock Configuration Tests
// ============================================================================

TEST_F(MockDirectoryListerTest, ConfigurableListFailure) {
  Path uploadDir("/var/uploads");
  mock->addMockEntry("/var/uploads", createFileEntry("test.txt", 1024));
  mock->setListSuccess(false);

  std::vector<DirectoryEntry> entries = mock->listDirectory(uploadDir, false);

  EXPECT_EQ(0, entries.size());
}

TEST_F(MockDirectoryListerTest, ClearMockEntries) {
  Path uploadDir("/var/uploads");
  mock->addMockEntry("/var/uploads", createFileEntry("test.txt", 1024));
  
  mock->clearMockEntries("/var/uploads");
  std::vector<DirectoryEntry> entries = mock->listDirectory(uploadDir, false);

  EXPECT_EQ(0, entries.size());
}

TEST_F(MockDirectoryListerTest, GetMockDirectories) {
  mock->addMockEntry("/var/uploads", createFileEntry("file1.txt", 1024));
  mock->addMockEntry("/var/data", createFileEntry("file2.txt", 2048));

  const std::map<std::string, std::vector<DirectoryEntry> >& dirs = 
      mock->getMockDirectories();

  EXPECT_EQ(2, dirs.size());
  EXPECT_NE(dirs.end(), dirs.find("/var/uploads"));
  EXPECT_NE(dirs.end(), dirs.find("/var/data"));
}

// ============================================================================
// Call Tracking Tests
// ============================================================================

TEST_F(MockDirectoryListerTest, TrackListDirectoryCalls) {
  Path uploadDir("/var/uploads");
  mock->addMockEntry("/var/uploads", createFileEntry("test.txt", 1024));

  mock->listDirectory(uploadDir, false);
  mock->listDirectory(uploadDir, true);

  EXPECT_EQ(2, mock->getCallCount("listDirectory"));
}

TEST_F(MockDirectoryListerTest, TrackMultipleMethodCalls) {
  Path uploadDir("/var/uploads");
  mock->addMockEntry("/var/uploads", createFileEntry("test.txt", 1024));

  mock->listDirectory(uploadDir, false);
  mock->validateDirectoryForListing(uploadDir);
  mock->generateHtmlListing(uploadDir, uploadDir, false);

  EXPECT_EQ(1, mock->getCallCount("validateDirectoryForListing"));
  EXPECT_EQ(1, mock->getCallCount("generateHtmlListing"));
  
  // generateHtmlListing internally calls listDirectory, so total should be 2
  EXPECT_EQ(2, mock->getCallCount("listDirectory"));
}

TEST_F(MockDirectoryListerTest, GetCallHistory) {
  Path uploadDir("/var/uploads");
  mock->addMockEntry("/var/uploads", createFileEntry("test.txt", 1024));

  mock->validateDirectoryForListing(uploadDir);
  mock->listDirectory(uploadDir, false);
  mock->validateDirectoryForListing(uploadDir);

  std::vector<std::string> history = mock->getCalledMethods();

  ASSERT_EQ(3, history.size());
  EXPECT_EQ("validateDirectoryForListing", history[0]);
  EXPECT_EQ("listDirectory", history[1]);
  EXPECT_EQ("validateDirectoryForListing", history[2]);
}

TEST_F(MockDirectoryListerTest, ClearCallHistory) {
  Path uploadDir("/var/uploads");
  mock->addMockEntry("/var/uploads", createFileEntry("test.txt", 1024));

  mock->listDirectory(uploadDir, false);
  EXPECT_EQ(1, mock->getCallCount("listDirectory"));

  mock->clearCallHistory();
  EXPECT_EQ(0, mock->getCallCount("listDirectory"));
  EXPECT_EQ(0, mock->getCalledMethods().size());
}

// ============================================================================
// Path Normalization Tests
// ============================================================================

TEST_F(MockDirectoryListerTest, NormalizePathWithTrailingSlash) {
  // Both paths should resolve to same mock entries
  Path uploadDir1("/var/uploads/");
  Path uploadDir2("/var/uploads");
  
  mock->addMockEntry("/var/uploads", createFileEntry("test.txt", 1024));

  std::vector<DirectoryEntry> entries1 = mock->listDirectory(uploadDir1, false);
  std::vector<DirectoryEntry> entries2 = mock->listDirectory(uploadDir2, false);

  EXPECT_EQ(1, entries1.size());
  EXPECT_EQ(1, entries2.size());
  EXPECT_EQ(entries1[0].m_name, entries2[0].m_name);
}
