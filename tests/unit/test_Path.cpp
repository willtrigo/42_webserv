/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_Path.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: umeneses <umeneses@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/24 13:10:18 by umeneses          #+#    #+#             */
/*   Updated: 2025/12/29 21:53:36 by umeneses         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "domain/filesystem/value_objects/Path.hpp"
#include "domain/filesystem/exceptions/PathException.hpp"
#include <gtest/gtest.h>

using namespace domain::filesystem::value_objects;

// Test fixture for Path tests
class PathTest : public ::testing::Test {
 protected:
  virtual void SetUp() {}
  virtual void TearDown() {}
};

// Basic construction tests
TEST_F(PathTest, DefaultConstructor) {
  Path path;
  EXPECT_TRUE(path.isEmpty());
}

TEST_F(PathTest, ConstructWithAbsolutePath) {
  Path path("/home/user/file.txt");
  EXPECT_FALSE(path.isEmpty());
  EXPECT_TRUE(path.isAbsolute());
  EXPECT_FALSE(path.isRelative());
  EXPECT_EQ("/home/user/file.txt", path.toString());
}

TEST_F(PathTest, ConstructWithRelativePath) {
  EXPECT_THROW(Path("relative/path"), domain::filesystem::exceptions::PathException);
}

TEST_F(PathTest, ConstructRelativePathWithFlag) {
  Path path("relative/path", false);
  EXPECT_FALSE(path.isEmpty());
  EXPECT_FALSE(path.isAbsolute());
  EXPECT_TRUE(path.isRelative());
}

// Path validation tests
TEST_F(PathTest, ValidateAbsolutePath) {
  EXPECT_TRUE(Path::isValidPath("/valid/path"));
  EXPECT_TRUE(Path::isValidPath("/"));
  EXPECT_TRUE(Path::isValidPath("/home/user/file.txt"));
}

TEST_F(PathTest, ValidateInvalidAbsolutePath) {
  EXPECT_FALSE(Path::isValidPath(""));
  EXPECT_FALSE(Path::isValidPath("relative/path"));
}

TEST_F(PathTest, ValidateRelativePath) {
  EXPECT_TRUE(Path::isValidPath("relative/path", false));
  EXPECT_TRUE(Path::isValidPath("./relative", false));
  EXPECT_TRUE(Path::isValidPath("../parent", false));
}

// Path component tests
TEST_F(PathTest, GetDirectory) {
  Path path("/home/user/file.txt");
  EXPECT_EQ("/home/user", path.getDirectory());
}

TEST_F(PathTest, GetDirectoryForRootFile) {
  Path path("/file.txt");
  EXPECT_EQ("/", path.getDirectory());  // Root directory, not empty string
}

TEST_F(PathTest, GetFilename) {
  Path path("/home/user/file.txt");
  EXPECT_EQ("file.txt", path.getFilename());
}

TEST_F(PathTest, GetExtension) {
  Path path("/home/user/file.txt");
  EXPECT_EQ(".txt", path.getExtension());
}

TEST_F(PathTest, GetExtensionNoExtension) {
  Path path("/home/user/file");
  EXPECT_EQ("", path.getExtension());
}

// Path operation tests
TEST_F(PathTest, JoinPaths) {
  Path base("/home/user");
  Path result = base.join("documents/file.txt");
  EXPECT_EQ("/home/user/documents/file.txt", result.toString());
}

TEST_F(PathTest, JoinPathWithPath) {
  Path base("/home/user");
  Path suffix("documents", false);
  Path result = base.join(suffix);
  EXPECT_TRUE(result.isAbsolute());
}

// Operator tests
TEST_F(PathTest, EqualityOperator) {
  Path path1("/home/user");
  Path path2("/home/user");
  Path path3("/home/other");
  
  EXPECT_TRUE(path1 == path2);
  EXPECT_FALSE(path1 == path3);
}

TEST_F(PathTest, InequalityOperator) {
  Path path1("/home/user");
  Path path2("/home/other");
  
  EXPECT_TRUE(path1 != path2);
}

TEST_F(PathTest, LessThanOperator) {
  Path path1("/home/a");
  Path path2("/home/b");
  
  EXPECT_TRUE(path1 < path2);
  EXPECT_FALSE(path2 < path1);
}

// Copy and assignment tests
TEST_F(PathTest, CopyConstructor) {
  Path original("/home/user/file.txt");
  Path copy(original);
  
  EXPECT_EQ(original.toString(), copy.toString());
  EXPECT_EQ(original.isAbsolute(), copy.isAbsolute());
}

TEST_F(PathTest, AssignmentOperator) {
  Path original("/home/user/file.txt");
  Path assigned;
  assigned = original;
  
  EXPECT_EQ(original.toString(), assigned.toString());
}

// Edge cases
TEST_F(PathTest, RootDirectory) {
  Path root = Path::rootDirectory();
  EXPECT_TRUE(root.isAbsolute());
  EXPECT_EQ("/", root.toString());
}

TEST_F(PathTest, PathTooLong) {
  std::string longPath(Path::MAX_PATH_LENGTH + 1, 'a');
  longPath[0] = '/';
  
  Path* pathObj = NULL;
  EXPECT_THROW(pathObj = new Path(longPath), domain::filesystem::exceptions::PathException);
  if (pathObj) delete pathObj;
}

// Directory detection
TEST_F(PathTest, IsDirectory) {
  Path dirPath("/home/user/");
  EXPECT_TRUE(dirPath.isDirectory());
  
  Path filePath("/home/user/file.txt");
  EXPECT_FALSE(filePath.isDirectory());
}

// Normalize tests
TEST_F(PathTest, NormalizeRootDirectory) {
  Path root("/");
  Path normalized = root.normalize();
  EXPECT_EQ("/", normalized.toString());
  EXPECT_TRUE(normalized.isAbsolute());
}

TEST_F(PathTest, NormalizePathToRoot) {
  Path path("/a/../");
  Path normalized = path.normalize();
  EXPECT_EQ("/", normalized.toString());
  EXPECT_TRUE(normalized.isAbsolute());
}

TEST_F(PathTest, NormalizeMultipleLevelsToRoot) {
  Path path("/home/user/../../");
  Path normalized = path.normalize();
  EXPECT_EQ("/", normalized.toString());
  EXPECT_TRUE(normalized.isAbsolute());
}

// ============================================================================
// Internal Validation Tests (through public API)
// ============================================================================

TEST_F(PathTest, ValidateShouldRejectNullBytes) {
  std::string pathWithNull = "/path/to";
  pathWithNull += '\0';
  pathWithNull += "file.txt";
  Path* p = NULL;
  EXPECT_THROW(p = new Path(pathWithNull), domain::filesystem::exceptions::PathException);
  if (p) delete p;
}

TEST_F(PathTest, ValidateShouldRejectEmptyPath) {
  EXPECT_THROW(Path(""), domain::filesystem::exceptions::PathException);
}

TEST_F(PathTest, ValidateShouldRejectRelativeWithoutFlag) {
  EXPECT_THROW(Path("relative/path"), domain::filesystem::exceptions::PathException);
  EXPECT_THROW(Path("./relative"), domain::filesystem::exceptions::PathException);
  EXPECT_THROW(Path("../parent"), domain::filesystem::exceptions::PathException);
}

TEST_F(PathTest, ValidateShouldAcceptRelativeWithFlag) {
  EXPECT_NO_THROW(Path("relative/path", false));
  EXPECT_NO_THROW(Path("./relative", false));
  EXPECT_NO_THROW(Path("../parent", false));
}

// normalizePath() testing through normalize() and various path formats
TEST_F(PathTest, NormalizePathShouldRemoveSingleDot) {
  Path path("/home/./user/./file.txt");
  Path normalized = path.normalize();
  EXPECT_EQ("/home/user/file.txt", normalized.toString());
}

TEST_F(PathTest, NormalizePathShouldRemoveTrailingSlash) {
  Path path("/home/user/");
  Path normalized = path.normalize();
  EXPECT_EQ("/home/user", normalized.toString());
}

TEST_F(PathTest, NormalizePathShouldKeepRootSlash) {
  Path path("/");
  Path normalized = path.normalize();
  EXPECT_EQ("/", normalized.toString());
}

TEST_F(PathTest, NormalizePathShouldCollapseDoubleSlashes) {
  Path path("/home//user///file.txt");
  Path normalized = path.normalize();
  EXPECT_EQ("/home/user/file.txt", normalized.toString());
}

TEST_F(PathTest, NormalizePathShouldHandleComplexCase) {
  Path path("/home/./user//documents/../file.txt");
  Path normalized = path.normalize();
  EXPECT_EQ("/home/user/file.txt", normalized.toString());
}

TEST_F(PathTest, NormalizePathWithOnlySlashesAndDots) {
  Path path("///.//.//");
  Path normalized = path.normalize();
  EXPECT_EQ("/", normalized.toString());
}

TEST_F(PathTest, NormalizePathShouldHandleRootWithDots) {
  Path path("/./");
  Path normalized = path.normalize();
  EXPECT_EQ("/", normalized.toString());
}

// isAbsolutePath() testing through isAbsolute() and isRelative()
TEST_F(PathTest, IsAbsolutePathShouldDetectRoot) {
  Path path("/");
  EXPECT_TRUE(path.isAbsolute());
  EXPECT_FALSE(path.isRelative());
}

TEST_F(PathTest, IsAbsolutePathShouldDetectAbsolutePaths) {
  Path path1("/home/user");
  Path path2("/var/www");
  Path path3("/etc");
  
  EXPECT_TRUE(path1.isAbsolute());
  EXPECT_TRUE(path2.isAbsolute());
  EXPECT_TRUE(path3.isAbsolute());
}

TEST_F(PathTest, IsAbsolutePathShouldDetectRelativePaths) {
  Path path1("relative/path", false);
  Path path2("./current", false);
  Path path3("../parent", false);
  
  EXPECT_FALSE(path1.isAbsolute());
  EXPECT_FALSE(path2.isAbsolute());
  EXPECT_FALSE(path3.isAbsolute());
  
  EXPECT_TRUE(path1.isRelative());
  EXPECT_TRUE(path2.isRelative());
  EXPECT_TRUE(path3.isRelative());
}

// hasDirectoryTraversal() testing through isSafePath() static method
TEST_F(PathTest, IsSafePathShouldDetectSimpleTraversal) {
  EXPECT_FALSE(Path::isSafePath("../etc/passwd"));
}

TEST_F(PathTest, IsSafePathShouldDetectMiddleTraversal) {
  EXPECT_FALSE(Path::isSafePath("/home/user/../../../etc/passwd"));
}

TEST_F(PathTest, IsSafePathShouldDetectTrailingTraversal) {
  EXPECT_FALSE(Path::isSafePath("/home/user/.."));
}

TEST_F(PathTest, IsSafePathShouldAcceptNormalPath) {
  EXPECT_TRUE(Path::isSafePath("/home/user/documents"));
}

TEST_F(PathTest, IsSafePathShouldHandleDotsInFilename) {
  // Should NOT be considered traversal - just dots in filename
  EXPECT_TRUE(Path::isSafePath("/home/user/my..file.txt"));
}

TEST_F(PathTest, IsSafePathShouldDetectEncodedTraversal) {
  // Testing if implementation handles URL-encoded traversal
  EXPECT_FALSE(Path::isSafePath("/home/user/%2e%2e/etc"));
}

// extractComponents() testing through getDirectory(), getFilename(), getExtension()
TEST_F(PathTest, ExtractComponentsFromSimplePath) {
  Path path("/home/file.txt");
  EXPECT_EQ("/home", path.getDirectory());
  EXPECT_EQ("file.txt", path.getFilename());
  EXPECT_EQ(".txt", path.getExtension());
}

TEST_F(PathTest, ExtractComponentsWithNoExtension) {
  Path path("/home/user/README");
  EXPECT_EQ("/home/user", path.getDirectory());
  EXPECT_EQ("README", path.getFilename());
  EXPECT_EQ("", path.getExtension());
}

TEST_F(PathTest, ExtractComponentsWithMultipleDots) {
  Path path("/home/user/file.tar.gz");
  EXPECT_EQ("/home/user", path.getDirectory());
  EXPECT_EQ("file.tar.gz", path.getFilename());
  EXPECT_EQ(".gz", path.getExtension()); // Should return last extension
}

TEST_F(PathTest, ExtractComponentsFromHiddenFile) {
  Path path("/home/user/.bashrc");
  EXPECT_EQ("/home/user", path.getDirectory());
  EXPECT_EQ(".bashrc", path.getFilename());
  EXPECT_EQ("", path.getExtension()); // Hidden file without extension
}

TEST_F(PathTest, ExtractComponentsFromHiddenFileWithExtension) {
  Path path("/home/user/.config.yaml");
  EXPECT_EQ("/home/user", path.getDirectory());
  EXPECT_EQ(".config.yaml", path.getFilename());
  EXPECT_EQ(".yaml", path.getExtension());
}

TEST_F(PathTest, ExtractComponentsFromRootFile) {
  Path path("/file.txt");
  EXPECT_EQ("/", path.getDirectory());
  EXPECT_EQ("file.txt", path.getFilename());
  EXPECT_EQ(".txt", path.getExtension());
}

TEST_F(PathTest, ExtractComponentsFromDirectoryOnly) {
  Path path("/home/user/documents/");
  EXPECT_EQ("/home/user", path.getDirectory());
  EXPECT_EQ("documents", path.getFilename()); // Trailing slash ignored
  EXPECT_EQ("", path.getExtension());
}

TEST_F(PathTest, ExtractComponentsWithDotsInDirectory) {
  Path path("/home/user.name/file.txt");
  EXPECT_EQ("/home/user.name", path.getDirectory());
  EXPECT_EQ("file.txt", path.getFilename());
  EXPECT_EQ(".txt", path.getExtension());
}

TEST_F(PathTest, ExtractComponentsFileEndingWithDot) {
  Path path("/home/user/file.");
  EXPECT_EQ("/home/user", path.getDirectory());
  EXPECT_EQ("file.", path.getFilename());
  EXPECT_EQ(".", path.getExtension()); // Trailing dot
}

// Path joining comprehensive tests
TEST_F(PathTest, JoinAbsoluteWithRelative) {
  Path base("/home/user");
  Path result = base.join("documents/file.txt");
  EXPECT_EQ("/home/user/documents/file.txt", result.toString());
  EXPECT_TRUE(result.isAbsolute());
}

TEST_F(PathTest, JoinWithTrailingSlashOnBase) {
  Path base("/home/user/");
  Path result = base.join("documents");
  EXPECT_EQ("/home/user/documents", result.toString());
}

TEST_F(PathTest, JoinWithLeadingSlashOnRelative) {
  Path base("/home/user");
  // This should either strip the leading slash or handle it correctly
  Path result = base.join("/documents");
  // Expected: either "/home/user/documents" or throw exception
  // Testing current behavior
}

TEST_F(PathTest, JoinEmptyPath) {
  Path base("/home/user");
  Path result = base.join("");
  EXPECT_EQ("/home/user", result.toString());
}

TEST_F(PathTest, JoinRootWithPath) {
  Path root("/");
  Path result = root.join("home/user");
  EXPECT_EQ("/home/user", result.toString());
}

TEST_F(PathTest, JoinMultipleLevels) {
  Path base("/home");
  Path result = base.join("user/documents/work/file.txt");
  EXPECT_EQ("/home/user/documents/work/file.txt", result.toString());
}

TEST_F(PathTest, JoinWithDots) {
  Path base("/home/user");
  Path result = base.join("./documents/../file.txt");
  // Should normalize after joining
  EXPECT_EQ("/home/user/file.txt", result.normalize().toString());
}

TEST_F(PathTest, JoinRelativeToRelative) {
  Path base("home/user", false);
  Path result = base.join("documents");
  EXPECT_TRUE(result.isRelative());
  EXPECT_EQ("home/user/documents", result.toString());
}
