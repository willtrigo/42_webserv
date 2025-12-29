/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_Path.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: umeneses <umeneses@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/24 13:10:18 by umeneses          #+#    #+#             */
/*   Updated: 2025/12/29 12:50:33 by umeneses         ###   ########.fr       */
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
