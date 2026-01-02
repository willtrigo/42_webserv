/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_MockPathResolver.cpp                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: umeneses <umeneses@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/01 00:00:00 by copilot           #+#    #+#             */
/*   Updated: 2026/01/01 19:27:10 by umeneses         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <gtest/gtest.h>
#include "mocks/MockPathResolver.hpp"

using namespace domain::filesystem::value_objects;

class MockPathResolverTest : public ::testing::Test {
 protected:
  MockPathResolver* mock;

  void SetUp() {
    mock = new MockPathResolver();
  }

  void TearDown() {
    delete mock;
  }
};

// ============================================================================
// Path Resolution Tests (Based on UploadConfig.getFullPath)
// ============================================================================

TEST_F(MockPathResolverTest, ResolveBasicPath) {
  // Based on PathResolver::resolve usage
  Path requested("/uploads/file.txt");
  Path root("/var/www");
  Path index("/index.html");

  Path resolved = mock->resolve(requested, root, index);

  EXPECT_EQ("/var/www/uploads/file.txt", resolved.toString());
  EXPECT_EQ(1, mock->getCallCount("resolve"));
}

TEST_F(MockPathResolverTest, ResolveWithCustomMapping) {
  // Based on PathResolver mock resolution capability
  Path requested("/uploads/file.txt");
  Path root("/var/www");
  
  mock->setMockResolution("/uploads/file.txt", "/custom/path/file.txt");

  Path resolved = mock->resolve(requested, root, Path("/index.html"));

  EXPECT_EQ("/custom/path/file.txt", resolved.toString());
}

TEST_F(MockPathResolverTest, ResolveDirectoryWithIndexFile) {
  // Based on PathResolver handling directory requests
  Path requested("/uploads/");
  Path root("/var/www");
  Path index("/index.html");
  
  mock->setIsDirectory("/var/www/uploads/", true);

  Path resolved = mock->resolve(requested, root, index);

  EXPECT_NE(std::string::npos, resolved.toString().find("index.html"));
}

TEST_F(MockPathResolverTest, ResolveRootPath) {
  // Based on UploadConfig.getUploadDirectory() usage
  Path requested("/");
  Path root("/var/www");
  Path index("/index.html");

  Path resolved = mock->resolve(requested, root, index);

  EXPECT_EQ("/var/www/", resolved.toString());
}

TEST_F(MockPathResolverTest, ResolveEmptyPath) {
  // Path doesn't allow empty strings, use root path instead
  Path requested("/");
  Path root("/var/www");
  Path index("/index.html");

  Path resolved = mock->resolve(requested, root, index);

  EXPECT_EQ("/var/www/", resolved.toString());
}

TEST_F(MockPathResolverTest, ResolveFailureMode) {
  // Test configurable resolution failure
  Path requested("/uploads/file.txt");
  Path root("/var/www");
  
  mock->setResolveSuccess(false);

  Path resolved = mock->resolve(requested, root, Path("/index.html"));

  // When resolve fails, returns root path  
  EXPECT_EQ("/", resolved.toString());
}

// ============================================================================
// Security Validation Tests (Based on PathResolver.isPathWithinRoot)
// ============================================================================

TEST_F(MockPathResolverTest, PathWithinRootValid) {
  // Based on security check: prevent directory traversal
  Path resolved("/var/www/uploads/file.txt");
  Path root("/var/www");

  bool withinRoot = mock->isPathWithinRoot(resolved, root);

  EXPECT_TRUE(withinRoot);
  EXPECT_EQ(1, mock->getCallCount("isPathWithinRoot"));
}

TEST_F(MockPathResolverTest, PathOutsideRoot) {
  // Based on security check: detect directory traversal attempt
  Path resolved("/etc/passwd");
  Path root("/var/www");

  bool withinRoot = mock->isPathWithinRoot(resolved, root);

  EXPECT_FALSE(withinRoot);
}

TEST_F(MockPathResolverTest, PathWithinRootExactMatch) {
  // Root path itself should be within root
  Path resolved("/var/www");
  Path root("/var/www");

  bool withinRoot = mock->isPathWithinRoot(resolved, root);

  EXPECT_TRUE(withinRoot);
}

TEST_F(MockPathResolverTest, PathWithinRootSimilarPrefix) {
  // /var/www2 should not be within /var/www
  Path resolved("/var/www2/file.txt");
  Path root("/var/www");

  bool withinRoot = mock->isPathWithinRoot(resolved, root);

  EXPECT_FALSE(withinRoot);
}

TEST_F(MockPathResolverTest, PathWithinRootConfigurable) {
  // Test configurable security validation
  Path resolved("/var/www/uploads/file.txt");
  Path root("/var/www");
  
  mock->setPathWithinRoot(false);

  bool withinRoot = mock->isPathWithinRoot(resolved, root);

  EXPECT_FALSE(withinRoot);
}

// ============================================================================
// Path Normalization Tests (Based on PathResolver.normalizeRequestedPath)
// ============================================================================

TEST_F(MockPathResolverTest, NormalizePathRemovesDuplicateSlashes) {
  // Based on PathResolver cleaning up paths
  Path requested("/uploads//files///file.txt");

  Path normalized = mock->normalizeRequestedPath(requested);

  EXPECT_EQ("/uploads/files/file.txt", normalized.toString());
  EXPECT_EQ(1, mock->getCallCount("normalizeRequestedPath"));
}

TEST_F(MockPathResolverTest, NormalizePathRemovesTrailingSlash) {
  Path requested("/uploads/files/");

  Path normalized = mock->normalizeRequestedPath(requested);

  EXPECT_EQ("/uploads/files", normalized.toString());
}

TEST_F(MockPathResolverTest, NormalizePathKeepsRootSlash) {
  Path requested("/");

  Path normalized = mock->normalizeRequestedPath(requested);

  EXPECT_EQ("/", normalized.toString());
}

TEST_F(MockPathResolverTest, NormalizePathHandlesComplex) {
  Path requested("//uploads///subfolder//file.txt/");

  Path normalized = mock->normalizeRequestedPath(requested);

  EXPECT_EQ("/uploads/subfolder/file.txt", normalized.toString());
}

// ============================================================================
// Directory Detection Tests (Based on PathResolver.isDirectoryPath)
// ============================================================================

TEST_F(MockPathResolverTest, IsDirectoryPathWithTrailingSlash) {
  // Paths ending with / are directories
  Path dirPath("/uploads/");

  bool isDir = mock->isDirectoryPath(dirPath);

  EXPECT_TRUE(isDir);
  EXPECT_EQ(1, mock->getCallCount("isDirectoryPath"));
}

TEST_F(MockPathResolverTest, IsDirectoryPathWithoutTrailingSlash) {
  // By default, paths without / are files
  Path filePath("/uploads/file.txt");

  bool isDir = mock->isDirectoryPath(filePath);

  EXPECT_FALSE(isDir);
}

TEST_F(MockPathResolverTest, IsDirectoryPathConfigurable) {
  // Can configure specific paths as directories
  Path dirPath("/uploads");
  mock->setIsDirectory("/uploads", true);

  bool isDir = mock->isDirectoryPath(dirPath);

  EXPECT_TRUE(isDir);
}

TEST_F(MockPathResolverTest, IsDirectoryPathRoot) {
  Path rootPath("/");

  bool isDir = mock->isDirectoryPath(rootPath);

  EXPECT_TRUE(isDir);
}

// ============================================================================
// Mock Configuration Tests
// ============================================================================

TEST_F(MockPathResolverTest, SetMultipleMockResolutions) {
  mock->setMockResolution("/file1.txt", "/resolved/path1.txt");
  mock->setMockResolution("/file2.txt", "/resolved/path2.txt");

  Path resolved1 = mock->resolve(Path("/file1.txt"), Path("/var/www"), Path("/index.html"));
  Path resolved2 = mock->resolve(Path("/file2.txt"), Path("/var/www"), Path("/index.html"));

  EXPECT_EQ("/resolved/path1.txt", resolved1.toString());
  EXPECT_EQ("/resolved/path2.txt", resolved2.toString());
}

TEST_F(MockPathResolverTest, GetMockResolutions) {
  mock->setMockResolution("/file1.txt", "/resolved/path1.txt");
  mock->setMockResolution("/file2.txt", "/resolved/path2.txt");

  const std::map<std::string, std::string>& resolutions = 
      mock->getMockResolutions();

  EXPECT_EQ(2, resolutions.size());
  EXPECT_NE(resolutions.end(), resolutions.find("/file1.txt"));
  EXPECT_NE(resolutions.end(), resolutions.find("/file2.txt"));
}

// ============================================================================
// Call Tracking Tests
// ============================================================================

TEST_F(MockPathResolverTest, TrackResolveCalls) {
  Path requested("/uploads/file.txt");
  Path root("/var/www");

  mock->resolve(requested, root, Path("/index.html"));
  mock->resolve(requested, root, Path("/index.html"));

  EXPECT_EQ(2, mock->getCallCount("resolve"));
}

TEST_F(MockPathResolverTest, TrackMultipleMethodCalls) {
  Path requested("/uploads/file.txt");
  Path root("/var/www");
  Path resolved("/var/www/uploads/file.txt");

  mock->resolve(requested, root, Path("/index.html"));
  mock->isPathWithinRoot(resolved, root);
  mock->normalizeRequestedPath(requested);
  mock->isDirectoryPath(requested);

  EXPECT_EQ(1, mock->getCallCount("resolve"));
  EXPECT_EQ(1, mock->getCallCount("isPathWithinRoot"));
  EXPECT_EQ(1, mock->getCallCount("normalizeRequestedPath"));
  EXPECT_EQ(1, mock->getCallCount("isDirectoryPath"));
}

TEST_F(MockPathResolverTest, GetCallHistory) {
  Path requested("/uploads/file.txt");
  Path root("/var/www");

  mock->normalizeRequestedPath(requested);
  mock->resolve(requested, root, Path("/index.html"));
  mock->normalizeRequestedPath(requested);

  std::vector<std::string> history = mock->getCalledMethods();

  ASSERT_EQ(3, history.size());
  EXPECT_EQ("normalizeRequestedPath", history[0]);
  EXPECT_EQ("resolve", history[1]);
  EXPECT_EQ("normalizeRequestedPath", history[2]);
}

TEST_F(MockPathResolverTest, ClearCallHistory) {
  Path requested("/uploads/file.txt");
  Path root("/var/www");

  mock->resolve(requested, root, Path("/index.html"));
  EXPECT_EQ(1, mock->getCallCount("resolve"));

  mock->clearCallHistory();
  EXPECT_EQ(0, mock->getCallCount("resolve"));
  EXPECT_EQ(0, mock->getCalledMethods().size());
}

// ============================================================================
// Path Normalization Edge Cases
// ============================================================================

TEST_F(MockPathResolverTest, NormalizePathWithTrailingSlashConsistency) {
  // Both should normalize to same path (for lookup)
  Path path1("/var/uploads/");
  Path path2("/var/uploads");

  mock->setIsDirectory("/var/uploads", true);

  bool isDir1 = mock->isDirectoryPath(path1);
  bool isDir2 = mock->isDirectoryPath(path2);

  // path1 has trailing slash, so should be directory
  EXPECT_TRUE(isDir1);
  // path2 is configured as directory
  EXPECT_TRUE(isDir2);
}

TEST_F(MockPathResolverTest, ResolveComplexPath) {
  // Test full resolution with multiple components
  Path requested("/api/v1/uploads/user/123/file.pdf");
  Path root("/var/www/app");

  Path resolved = mock->resolve(requested, root, Path("/index.html"));

  EXPECT_EQ("/var/www/app/api/v1/uploads/user/123/file.pdf", 
            resolved.toString());
}
