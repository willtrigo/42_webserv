/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_UploadConfig.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: umeneses <umeneses@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/30 15:27:25 by umeneses          #+#    #+#             */
/*   Updated: 2025/12/30 15:44:55 by umeneses         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <gtest/gtest.h>
#include <sstream>
#include "domain/configuration/value_objects/UploadConfig.hpp"
#include "domain/configuration/exceptions/UploadConfigException.hpp"

using namespace domain::configuration::value_objects;
using namespace domain::configuration::exceptions;

class UploadConfigTest : public ::testing::Test {
 protected:
  void SetUp() {}
  void TearDown() {}
};

// ============================================================================
// Construction Tests
// ============================================================================

TEST_F(UploadConfigTest, ConstructWithDirectory) {
  domain::filesystem::value_objects::Path uploadDir("/var/uploads");
  
  UploadConfig* config = NULL;
  EXPECT_NO_THROW(config = new UploadConfig(uploadDir));
  if (config) delete config;
}

TEST_F(UploadConfigTest, ConstructWithDirectoryAndSizes) {
  domain::filesystem::value_objects::Path uploadDir("/var/uploads");
  domain::filesystem::value_objects::Size maxFileSize = domain::filesystem::value_objects::Size::fromMegabytes(50);
  domain::filesystem::value_objects::Size maxTotalSize = domain::filesystem::value_objects::Size::fromMegabytes(500);
  
  UploadConfig config(uploadDir, maxFileSize, maxTotalSize);
  EXPECT_EQ("/var/uploads", config.getUploadDirectory().toString());
  EXPECT_EQ(50 * 1024 * 1024, config.getMaxFileSize().getBytes());
  EXPECT_EQ(500 * 1024 * 1024, config.getMaxTotalSize().getBytes());
}

TEST_F(UploadConfigTest, CopyConstructor) {
  domain::filesystem::value_objects::Path uploadDir("/var/uploads");
  UploadConfig original(uploadDir);
  original.setMaxFilesPerUpload(5);
  original.addAllowedExtension(".txt");
  
  UploadConfig copy(original);
  EXPECT_EQ(original.getUploadDirectory().toString(), copy.getUploadDirectory().toString());
  EXPECT_EQ(original.getMaxFilesPerUpload(), copy.getMaxFilesPerUpload());
}

TEST_F(UploadConfigTest, AssignmentOperator) {
  domain::filesystem::value_objects::Path uploadDir1("/var/uploads1");
  domain::filesystem::value_objects::Path uploadDir2("/var/uploads2");
  
  UploadConfig config1(uploadDir1);
  UploadConfig config2(uploadDir2);
  config2.setMaxFilesPerUpload(20);
  
  config1 = config2;
  EXPECT_EQ(config2.getUploadDirectory().toString(), config1.getUploadDirectory().toString());
  EXPECT_EQ(20u, config1.getMaxFilesPerUpload());
}

TEST_F(UploadConfigTest, SelfAssignment) {
  domain::filesystem::value_objects::Path uploadDir("/var/uploads");
  UploadConfig config(uploadDir);
  config.setMaxFilesPerUpload(15);
  
  config = config;
  EXPECT_EQ(15u, config.getMaxFilesPerUpload());
}

// ============================================================================
// Getter Tests
// ============================================================================

TEST_F(UploadConfigTest, GetUploadDirectory) {
  domain::filesystem::value_objects::Path uploadDir("/var/uploads");
  UploadConfig config(uploadDir);
  
  EXPECT_EQ("/var/uploads", config.getUploadDirectory().toString());
}

TEST_F(UploadConfigTest, GetMaxFileSize) {
  domain::filesystem::value_objects::Path uploadDir("/var/uploads");
  UploadConfig config(uploadDir);
  
  EXPECT_EQ(10 * 1024 * 1024, config.getMaxFileSize().getBytes());
}

TEST_F(UploadConfigTest, GetMaxTotalSize) {
  domain::filesystem::value_objects::Path uploadDir("/var/uploads");
  UploadConfig config(uploadDir);
  
  EXPECT_EQ(100 * 1024 * 1024, config.getMaxTotalSize().getBytes());
}

TEST_F(UploadConfigTest, GetMaxFilesPerUpload) {
  domain::filesystem::value_objects::Path uploadDir("/var/uploads");
  UploadConfig config(uploadDir);
  
  EXPECT_EQ(10u, config.getMaxFilesPerUpload());
}

TEST_F(UploadConfigTest, GetMaxFilenameLength) {
  domain::filesystem::value_objects::Path uploadDir("/var/uploads");
  UploadConfig config(uploadDir);
  
  EXPECT_EQ(255u, config.getMaxFilenameLength());
}

TEST_F(UploadConfigTest, GetMaxUploadsPerHour) {
  domain::filesystem::value_objects::Path uploadDir("/var/uploads");
  UploadConfig config(uploadDir);
  
  EXPECT_EQ(100u, config.getMaxUploadsPerHour());
}

TEST_F(UploadConfigTest, GetConfigurationFlags) {
  domain::filesystem::value_objects::Path uploadDir("/var/uploads");
  UploadConfig config(uploadDir);
  
  EXPECT_FALSE(config.getOverwriteExisting());
  EXPECT_FALSE(config.getGenerateThumbnails());
  EXPECT_FALSE(config.getApplyWatermark());
  EXPECT_FALSE(config.getEncryptFiles());
  EXPECT_FALSE(config.getCompressFiles());
}

// ============================================================================
// Setter Tests
// ============================================================================

TEST_F(UploadConfigTest, SetMaxFileSize) {
  domain::filesystem::value_objects::Path uploadDir("/var/uploads");
  UploadConfig config(uploadDir);
  
  domain::filesystem::value_objects::Size newSize = domain::filesystem::value_objects::Size::fromMegabytes(25);
  EXPECT_NO_THROW(config.setMaxFileSize(newSize));
  EXPECT_EQ(25 * 1024 * 1024, config.getMaxFileSize().getBytes());
}

TEST_F(UploadConfigTest, SetMaxTotalSize) {
  domain::filesystem::value_objects::Path uploadDir("/var/uploads");
  UploadConfig config(uploadDir);
  
  domain::filesystem::value_objects::Size newSize = domain::filesystem::value_objects::Size::fromGigabytes(1);
  EXPECT_NO_THROW(config.setMaxTotalSize(newSize));
  EXPECT_EQ(1024 * 1024 * 1024, config.getMaxTotalSize().getBytes());
}

TEST_F(UploadConfigTest, SetMaxFilesPerUpload) {
  domain::filesystem::value_objects::Path uploadDir("/var/uploads");
  UploadConfig config(uploadDir);
  
  EXPECT_NO_THROW(config.setMaxFilesPerUpload(25));
  EXPECT_EQ(25u, config.getMaxFilesPerUpload());
}

TEST_F(UploadConfigTest, SetMaxFilenameLength) {
  domain::filesystem::value_objects::Path uploadDir("/var/uploads");
  UploadConfig config(uploadDir);
  
  EXPECT_NO_THROW(config.setMaxFilenameLength(128));
  EXPECT_EQ(128u, config.getMaxFilenameLength());
}

TEST_F(UploadConfigTest, SetMaxUploadsPerHour) {
  domain::filesystem::value_objects::Path uploadDir("/var/uploads");
  UploadConfig config(uploadDir);
  
  EXPECT_NO_THROW(config.setMaxUploadsPerHour(200));
  EXPECT_EQ(200u, config.getMaxUploadsPerHour());
}

TEST_F(UploadConfigTest, SetUploadDirectory) {
  domain::filesystem::value_objects::Path uploadDir("/var/uploads");
  UploadConfig config(uploadDir);
  
  domain::filesystem::value_objects::Path newDir("/tmp/uploads");
  EXPECT_NO_THROW(config.setUploadDirectory(newDir));
  EXPECT_EQ("/tmp/uploads", config.getUploadDirectory().toString());
}

TEST_F(UploadConfigTest, SetPermissions) {
  domain::filesystem::value_objects::Path uploadDir("/var/uploads");
  UploadConfig config(uploadDir);
  
  domain::filesystem::value_objects::Permission perm = domain::filesystem::value_objects::Permission::readWrite();
  EXPECT_NO_THROW(config.setPermissions(perm));
  EXPECT_EQ(perm.getOctalValue(), config.getPermissions().getOctalValue());
}

TEST_F(UploadConfigTest, SetUploadAccess) {
  domain::filesystem::value_objects::Path uploadDir("/var/uploads");
  UploadConfig config(uploadDir);
  
  EXPECT_NO_THROW(config.setUploadAccess("authenticated"));
  EXPECT_EQ("authenticated", config.getUploadAccess());
}

// ============================================================================
// Validation Tests
// ============================================================================

TEST_F(UploadConfigTest, ValidateFileSize) {
  domain::filesystem::value_objects::Path uploadDir("/var/uploads");
  UploadConfig config(uploadDir);
  
  domain::filesystem::value_objects::Size validSize = domain::filesystem::value_objects::Size::fromMegabytes(5);
  domain::filesystem::value_objects::Size invalidSize = domain::filesystem::value_objects::Size::fromMegabytes(15);
  
  EXPECT_TRUE(config.validateFileSize(validSize));
  EXPECT_FALSE(config.validateFileSize(invalidSize));
}

TEST_F(UploadConfigTest, ValidateTotalSize) {
  domain::filesystem::value_objects::Path uploadDir("/var/uploads");
  UploadConfig config(uploadDir);
  
  domain::filesystem::value_objects::Size validSize = domain::filesystem::value_objects::Size::fromMegabytes(50);
  domain::filesystem::value_objects::Size invalidSize = domain::filesystem::value_objects::Size::fromMegabytes(150);
  
  EXPECT_TRUE(config.validateTotalSize(validSize));
  EXPECT_FALSE(config.validateTotalSize(invalidSize));
}

TEST_F(UploadConfigTest, ValidateFilename) {
  domain::filesystem::value_objects::Path uploadDir("/var/uploads");
  UploadConfig config(uploadDir);
  
  EXPECT_TRUE(config.validateFilename("valid_file.txt"));
  EXPECT_TRUE(config.validateFilename("document-2023.pdf"));
  EXPECT_FALSE(config.validateFilename(""));
  EXPECT_FALSE(config.validateFilename("file/with/slashes.txt"));
}

TEST_F(UploadConfigTest, ValidateFilenameWithInvalidCharacters) {
  domain::filesystem::value_objects::Path uploadDir("/var/uploads");
  UploadConfig config(uploadDir);
  
  EXPECT_FALSE(config.validateFilename("file\\name.txt"));
  EXPECT_FALSE(config.validateFilename("file?name.txt"));
  EXPECT_FALSE(config.validateFilename("file*name.txt"));
  EXPECT_FALSE(config.validateFilename("file|name.txt"));
  EXPECT_FALSE(config.validateFilename("file\"name.txt"));
  EXPECT_FALSE(config.validateFilename("file<name>.txt"));
}

TEST_F(UploadConfigTest, ValidateFilenameTooLong) {
  domain::filesystem::value_objects::Path uploadDir("/var/uploads");
  UploadConfig config(uploadDir);
  config.setMaxFilenameLength(10);
  
  EXPECT_TRUE(config.validateFilename("short.txt"));
  EXPECT_FALSE(config.validateFilename("very_long_filename_that_exceeds_limit.txt"));
}

TEST_F(UploadConfigTest, ValidateFilenameReservedNames) {
  domain::filesystem::value_objects::Path uploadDir("/var/uploads");
  UploadConfig config(uploadDir);
  
  EXPECT_FALSE(config.validateFilename("."));
  EXPECT_FALSE(config.validateFilename(".."));
}

TEST_F(UploadConfigTest, ValidateExtension) {
  domain::filesystem::value_objects::Path uploadDir("/var/uploads");
  UploadConfig config(uploadDir);
  
  EXPECT_TRUE(config.validateExtension("document.pdf"));
  EXPECT_TRUE(config.validateExtension("image.jpg"));
  EXPECT_TRUE(config.validateExtension("spreadsheet.xlsx"));
}

TEST_F(UploadConfigTest, ValidateExtensionBlocked) {
  domain::filesystem::value_objects::Path uploadDir("/var/uploads");
  UploadConfig config(uploadDir);
  
  EXPECT_FALSE(config.validateExtension("malware.exe"));
  EXPECT_FALSE(config.validateExtension("script.php"));
  EXPECT_FALSE(config.validateExtension("batch.bat"));
}

// ============================================================================
// Extension Management Tests
// ============================================================================

TEST_F(UploadConfigTest, AddAllowedExtension) {
  domain::filesystem::value_objects::Path uploadDir("/var/uploads");
  UploadConfig config(uploadDir);
  
  EXPECT_NO_THROW(config.addAllowedExtension(".md"));
  EXPECT_TRUE(config.validateExtension("readme.md"));
}

TEST_F(UploadConfigTest, AddAllowedExtensionWithoutDot) {
  domain::filesystem::value_objects::Path uploadDir("/var/uploads");
  UploadConfig config(uploadDir);
  
  EXPECT_NO_THROW(config.addAllowedExtension("log"));
  EXPECT_TRUE(config.validateExtension("server.log"));
}

TEST_F(UploadConfigTest, AddAllowedExtensionCaseInsensitive) {
  domain::filesystem::value_objects::Path uploadDir("/var/uploads");
  UploadConfig config(uploadDir);
  
  config.addAllowedExtension("TXT");
  EXPECT_TRUE(config.validateExtension("file.txt"));
  EXPECT_TRUE(config.validateExtension("file.TXT"));
}

TEST_F(UploadConfigTest, AddBlockedExtension) {
  domain::filesystem::value_objects::Path uploadDir("/var/uploads");
  UploadConfig config(uploadDir);
  
  EXPECT_NO_THROW(config.addBlockedExtension(".dangerous"));
  EXPECT_FALSE(config.validateExtension("file.dangerous"));
}

TEST_F(UploadConfigTest, AddBlockedExtensionRemovesFromAllowed) {
  domain::filesystem::value_objects::Path uploadDir("/var/uploads");
  UploadConfig config(uploadDir);
  
  config.addAllowedExtension("exe");
  EXPECT_TRUE(config.validateExtension("program.exe"));
  
  config.addBlockedExtension("exe");
  EXPECT_FALSE(config.validateExtension("program.exe"));
}

TEST_F(UploadConfigTest, AddAllowedExtensionWhenBlocked) {
  domain::filesystem::value_objects::Path uploadDir("/var/uploads");
  UploadConfig config(uploadDir);
  
  config.addBlockedExtension("test");
  config.addAllowedExtension("test");
  
  EXPECT_FALSE(config.validateExtension("file.test"));
}

TEST_F(UploadConfigTest, AddMultipleExtensions) {
  domain::filesystem::value_objects::Path uploadDir("/var/uploads");
  UploadConfig config(uploadDir);
  
  config.addAllowedExtension("cpp");
  config.addAllowedExtension("hpp");
  config.addAllowedExtension("c");
  config.addAllowedExtension("h");
  
  EXPECT_TRUE(config.validateExtension("main.cpp"));
  EXPECT_TRUE(config.validateExtension("header.hpp"));
  EXPECT_TRUE(config.validateExtension("source.c"));
  EXPECT_TRUE(config.validateExtension("include.h"));
}

// ============================================================================
// MIME Type Management Tests
// ============================================================================

TEST_F(UploadConfigTest, AddAllowedMimeType) {
  domain::filesystem::value_objects::Path uploadDir("/var/uploads");
  UploadConfig config(uploadDir);
  
  EXPECT_NO_THROW(config.addAllowedMimeType("application/json"));
  EXPECT_TRUE(config.validateMimeType("application/json"));
}

TEST_F(UploadConfigTest, AddBlockedMimeType) {
  domain::filesystem::value_objects::Path uploadDir("/var/uploads");
  UploadConfig config(uploadDir);
  
  EXPECT_NO_THROW(config.addBlockedMimeType("application/x-executable"));
  EXPECT_FALSE(config.validateMimeType("application/x-executable"));
}

TEST_F(UploadConfigTest, AddBlockedMimeTypeRemovesFromAllowed) {
  domain::filesystem::value_objects::Path uploadDir("/var/uploads");
  UploadConfig config(uploadDir);
  
  config.addAllowedMimeType("text/html");
  EXPECT_TRUE(config.validateMimeType("text/html"));
  
  config.addBlockedMimeType("text/html");
  EXPECT_FALSE(config.validateMimeType("text/html"));
}

TEST_F(UploadConfigTest, AddAllowedMimeTypeWhenBlocked) {
  domain::filesystem::value_objects::Path uploadDir("/var/uploads");
  UploadConfig config(uploadDir);
  
  config.addBlockedMimeType("application/javascript");
  config.addAllowedMimeType("application/javascript");
  
  EXPECT_FALSE(config.validateMimeType("application/javascript"));
}

TEST_F(UploadConfigTest, ValidateMimeTypeWithEmptyLists) {
  domain::filesystem::value_objects::Path uploadDir("/var/uploads");
  UploadConfig config(uploadDir);
  
  EXPECT_TRUE(config.validateMimeType("image/png"));
  EXPECT_TRUE(config.validateMimeType("text/plain"));
}

TEST_F(UploadConfigTest, AddMultipleMimeTypes) {
  domain::filesystem::value_objects::Path uploadDir("/var/uploads");
  UploadConfig config(uploadDir);
  
  config.addAllowedMimeType("image/png");
  config.addAllowedMimeType("image/jpeg");
  config.addAllowedMimeType("image/gif");
  
  EXPECT_TRUE(config.validateMimeType("image/png"));
  EXPECT_TRUE(config.validateMimeType("image/jpeg"));
  EXPECT_TRUE(config.validateMimeType("image/gif"));
}

// ============================================================================
// Configuration Flag Tests
// ============================================================================

TEST_F(UploadConfigTest, SetOverwriteExisting) {
  domain::filesystem::value_objects::Path uploadDir("/var/uploads");
  UploadConfig config(uploadDir);
  
  EXPECT_FALSE(config.getOverwriteExisting());
  config.setOverwriteExisting(true);
  EXPECT_TRUE(config.getOverwriteExisting());
  config.setOverwriteExisting(false);
  EXPECT_FALSE(config.getOverwriteExisting());
}

TEST_F(UploadConfigTest, SetGenerateThumbnails) {
  domain::filesystem::value_objects::Path uploadDir("/var/uploads");
  UploadConfig config(uploadDir);
  
  EXPECT_FALSE(config.getGenerateThumbnails());
  config.setGenerateThumbnails(true);
  EXPECT_TRUE(config.getGenerateThumbnails());
}

TEST_F(UploadConfigTest, SetApplyWatermark) {
  domain::filesystem::value_objects::Path uploadDir("/var/uploads");
  UploadConfig config(uploadDir);
  
  EXPECT_FALSE(config.getApplyWatermark());
  config.setApplyWatermark(true);
  EXPECT_TRUE(config.getApplyWatermark());
}

TEST_F(UploadConfigTest, SetEncryptFiles) {
  domain::filesystem::value_objects::Path uploadDir("/var/uploads");
  UploadConfig config(uploadDir);
  
  EXPECT_FALSE(config.getEncryptFiles());
  config.setEncryptFiles(true);
  EXPECT_TRUE(config.getEncryptFiles());
}

TEST_F(UploadConfigTest, SetCompressFiles) {
  domain::filesystem::value_objects::Path uploadDir("/var/uploads");
  UploadConfig config(uploadDir);
  
  EXPECT_FALSE(config.getCompressFiles());
  config.setCompressFiles(true);
  EXPECT_TRUE(config.getCompressFiles());
}

TEST_F(UploadConfigTest, SetMultipleFlags) {
  domain::filesystem::value_objects::Path uploadDir("/var/uploads");
  UploadConfig config(uploadDir);
  
  config.setOverwriteExisting(true);
  config.setGenerateThumbnails(true);
  config.setEncryptFiles(true);
  
  EXPECT_TRUE(config.getOverwriteExisting());
  EXPECT_TRUE(config.getGenerateThumbnails());
  EXPECT_FALSE(config.getApplyWatermark());
  EXPECT_TRUE(config.getEncryptFiles());
  EXPECT_FALSE(config.getCompressFiles());
}

// ============================================================================
// Upload Operations Tests (These will likely fail - need FileHandler)
// ============================================================================

TEST_F(UploadConfigTest, ProcessUploadBasic) {
  domain::filesystem::value_objects::Path uploadDir("/var/uploads");
  UploadConfig config(uploadDir);
  
  std::vector<char> fileData;
  fileData.push_back('t');
  fileData.push_back('e');
  fileData.push_back('s');
  fileData.push_back('t');
  
  EXPECT_NO_THROW({
    UploadFileInfo info = config.processUpload("test.txt", fileData);
    EXPECT_EQ("test.txt", info.originalFilename);
  });
}

TEST_F(UploadConfigTest, ProcessUploadWithUploader) {
  domain::filesystem::value_objects::Path uploadDir("/var/uploads");
  UploadConfig config(uploadDir);
  
  std::vector<char> fileData;
  fileData.push_back('d');
  fileData.push_back('a');
  fileData.push_back('t');
  fileData.push_back('a');
  
  EXPECT_NO_THROW({
    UploadFileInfo info = config.processUpload("document.pdf", fileData, "user123");
    EXPECT_EQ("user123", info.uploader);
  });
}

TEST_F(UploadConfigTest, ProcessUploadInvalidFilename) {
  domain::filesystem::value_objects::Path uploadDir("/var/uploads");
  UploadConfig config(uploadDir);
  
  std::vector<char> fileData;
  fileData.push_back('d');
  
  EXPECT_THROW(
    config.processUpload("", fileData),
    UploadConfigException
  );
}

TEST_F(UploadConfigTest, ProcessUploadBlockedExtension) {
  domain::filesystem::value_objects::Path uploadDir("/var/uploads");
  UploadConfig config(uploadDir);
  
  std::vector<char> fileData;
  fileData.push_back('x');
  
  EXPECT_THROW(
    config.processUpload("malware.exe", fileData),
    UploadConfigException
  );
}

TEST_F(UploadConfigTest, ProcessUploadFileTooLarge) {
  domain::filesystem::value_objects::Path uploadDir("/var/uploads");
  UploadConfig config(uploadDir);
  config.setMaxFileSize(domain::filesystem::value_objects::Size(10));
  
  std::vector<char> fileData(1000, 'x');
  
  EXPECT_THROW(
    config.processUpload("large.txt", fileData),
    UploadConfigException
  );
}

TEST_F(UploadConfigTest, ProcessMultiUpload) {
  domain::filesystem::value_objects::Path uploadDir("/var/uploads");
  UploadConfig config(uploadDir);
  
  std::map<std::string, std::vector<char> > files;
  
  std::vector<char> file1;
  file1.push_back('1');
  files["file1.txt"] = file1;
  
  std::vector<char> file2;
  file2.push_back('2');
  files["file2.txt"] = file2;
  
  EXPECT_NO_THROW({
    std::vector<UploadFileInfo> results = config.processMultiUpload(files);
    EXPECT_EQ(2u, results.size());
  });
}

TEST_F(UploadConfigTest, ProcessMultiUploadTooManyFiles) {
  domain::filesystem::value_objects::Path uploadDir("/var/uploads");
  UploadConfig config(uploadDir);
  config.setMaxFilesPerUpload(2);
  
  std::map<std::string, std::vector<char> > files;
  for (int i = 0; i < 5; ++i) {
    std::vector<char> data;
    data.push_back('x');
    std::ostringstream oss;
    oss << "file" << i << ".txt";
    files[oss.str()] = data;
  }
  
  EXPECT_THROW(
    config.processMultiUpload(files),
    UploadConfigException
  );
}


