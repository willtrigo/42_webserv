/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MockFileHandler.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: umeneses <umeneses@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/01 00:00:00 by copilot           #+#    #+#             */
/*   Updated: 2026/01/01 19:06:25 by umeneses         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MOCK_FILE_HANDLER_HPP
#define MOCK_FILE_HANDLER_HPP

#include <map>
#include <string>
#include <vector>

/**
 * MockFileHandler - Test double for file system operations
 * 
 * Designed to support UploadConfig test scenarios from test_UploadConfig.cpp:
 * - File write operations (processUpload)
 * - File read operations (validation)
 * - File metadata (size, mime type)
 * - File deletion (cleanup)
 * - File existence checks
 * 
 * Call tracking allows verification of:
 * - How many times each method was called
 * - What parameters were passed
 * - Sequence of operations
 */
class MockFileHandler {
 public:
  MockFileHandler();
  ~MockFileHandler();

  // =========================================================================
  // File Write Operations (needed for processUpload tests)
  // =========================================================================
  
  /**
   * Write data to file
   * Returns: true if successful, false otherwise
   * Tracks: call count, filename, data size
   */
  bool writeFile(const std::string& path, const std::vector<char>& data);
  bool writeFile(const std::string& path, const std::string& data);
  
  /**
   * Write file chunk (for large uploads)
   * Returns: bytes written
   * Tracks: call count, chunk sizes
   */
  size_t writeFileChunk(const std::string& path, 
                        const std::vector<char>& chunk,
                        size_t offset);

  // =========================================================================
  // File Read Operations (needed for validation)
  // =========================================================================
  
  /**
   * Read entire file content
   * Returns: file data or empty vector on error
   * Tracks: call count, filenames
   */
  std::vector<char> readFile(const std::string& path);
  std::string readFileAsString(const std::string& path);
  
  /**
   * Read file chunk
   * Returns: bytes read
   * Tracks: call count, chunk sizes
   */
  std::vector<char> readFileChunk(const std::string& path, 
                                   size_t offset, 
                                   size_t length);

  // =========================================================================
  // File Metadata Operations (needed for size/type validation)
  // =========================================================================
  
  /**
   * Check if file exists
   * Returns: true if file exists in mock storage
   */
  bool fileExists(const std::string& path) const;
  
  /**
   * Get file size
   * Returns: size in bytes, 0 if not found
   */
  size_t getFileSize(const std::string& path) const;
  
  /**
   * Get file metadata
   * Returns: map with "size", "mimeType", "permissions", "modified"
   */
  std::map<std::string, std::string> getMetadata(const std::string& path) const;
  
  /**
   * Detect MIME type from extension
   * Returns: mime type string (e.g., "text/plain")
   */
  std::string detectMimeType(const std::string& path) const;
  
  /**
   * Calculate file checksum (for integrity validation)
   * Returns: checksum string
   */
  std::string calculateChecksum(const std::string& path) const;

  // =========================================================================
  // File Management Operations (needed for cleanup/rename)
  // =========================================================================
  
  /**
   * Delete file
   * Returns: true if successful
   */
  bool deleteFile(const std::string& path);
  
  /**
   * Rename/move file
   * Returns: true if successful
   */
  bool renameFile(const std::string& oldPath, const std::string& newPath);
  
  /**
   * Get available disk space (for upload size validation)
   * Returns: bytes available
   */
  size_t getAvailableDiskSpace(const std::string& path) const;

  // =========================================================================
  // Mock Configuration & Verification
  // =========================================================================
  
  /**
   * Configure mock behavior
   */
  void setWriteSuccess(bool success);
  void setReadSuccess(bool success);
  void setAvailableDiskSpace(size_t bytes);
  void setFileExists(const std::string& path, bool exists);
  
  /**
   * Verify mock was called correctly
   */
  int getCallCount(const std::string& methodName) const;
  std::vector<std::string> getCalledMethods() const;
  void clearCallHistory();
  
  /**
   * Get mock file storage (for test assertions)
   */
  const std::map<std::string, std::vector<char> >& getMockFiles() const;
  void setMockFile(const std::string& path, const std::vector<char>& data);

 private:
  // Mock file storage (path -> data)
  std::map<std::string, std::vector<char> > m_mockFiles;
  
  // Call tracking (method name -> call count)
  std::map<std::string, int> m_callCounts;
  std::vector<std::string> m_callHistory;
  
  // Configurable behavior
  bool m_writeSuccess;
  bool m_readSuccess;
  size_t m_availableDiskSpace;
  std::map<std::string, bool> m_fileExistence;
  
  // Helper methods
  void trackCall(const std::string& methodName);
  std::string getExtension(const std::string& path) const;
};

#endif  // MOCK_FILE_HANDLER_HPP
