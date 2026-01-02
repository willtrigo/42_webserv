/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MockFileHandler.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: umeneses <umeneses@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/01 00:00:00 by copilot           #+#    #+#             */
/*   Updated: 2026/01/01 19:06:25 by umeneses         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "MockFileHandler.hpp"
#include <sstream>

MockFileHandler::MockFileHandler()
    : m_writeSuccess(true),
      m_readSuccess(true),
      m_availableDiskSpace(1024 * 1024 * 1024) {  // 1GB default
}

MockFileHandler::~MockFileHandler() {
  m_mockFiles.clear();
  m_callCounts.clear();
  m_callHistory.clear();
}

// ============================================================================
// File Write Operations
// ============================================================================

bool MockFileHandler::writeFile(const std::string& path, 
                                 const std::vector<char>& data) {
  trackCall("writeFile");
  
  if (!m_writeSuccess) {
    return false;
  }
  
  m_mockFiles[path] = data;
  return true;
}

bool MockFileHandler::writeFile(const std::string& path, 
                                 const std::string& data) {
  std::vector<char> vec(data.begin(), data.end());
  return writeFile(path, vec);
}

size_t MockFileHandler::writeFileChunk(const std::string& path,
                                        const std::vector<char>& chunk,
                                        size_t offset) {
  trackCall("writeFileChunk");
  
  if (!m_writeSuccess) {
    return 0;
  }
  
  // Resize file if needed
  if (m_mockFiles[path].size() < offset + chunk.size()) {
    m_mockFiles[path].resize(offset + chunk.size());
  }
  
  // Copy chunk data
  for (size_t i = 0; i < chunk.size(); ++i) {
    m_mockFiles[path][offset + i] = chunk[i];
  }
  
  return chunk.size();
}

// ============================================================================
// File Read Operations
// ============================================================================

std::vector<char> MockFileHandler::readFile(const std::string& path) {
  trackCall("readFile");
  
  if (!m_readSuccess) {
    return std::vector<char>();
  }
  
  std::map<std::string, std::vector<char> >::const_iterator it = 
      m_mockFiles.find(path);
  if (it != m_mockFiles.end()) {
    return it->second;
  }
  
  return std::vector<char>();
}

std::string MockFileHandler::readFileAsString(const std::string& path) {
  std::vector<char> data = readFile(path);
  return std::string(data.begin(), data.end());
}

std::vector<char> MockFileHandler::readFileChunk(const std::string& path,
                                                  size_t offset,
                                                  size_t length) {
  trackCall("readFileChunk");
  
  std::vector<char> fullData = readFile(path);
  
  if (offset >= fullData.size()) {
    return std::vector<char>();
  }
  
  size_t actualLength = length;
  if (offset + length > fullData.size()) {
    actualLength = fullData.size() - offset;
  }
  
  std::vector<char> chunk(fullData.begin() + offset, 
                          fullData.begin() + offset + actualLength);
  return chunk;
}

// ============================================================================
// File Metadata Operations
// ============================================================================

bool MockFileHandler::fileExists(const std::string& path) const {
  // Check explicit configuration first
  std::map<std::string, bool>::const_iterator it = m_fileExistence.find(path);
  if (it != m_fileExistence.end()) {
    return it->second;
  }
  
  // Otherwise check mock storage
  return m_mockFiles.find(path) != m_mockFiles.end();
}

size_t MockFileHandler::getFileSize(const std::string& path) const {
  std::map<std::string, std::vector<char> >::const_iterator it = 
      m_mockFiles.find(path);
  if (it != m_mockFiles.end()) {
    return it->second.size();
  }
  return 0;
}

std::map<std::string, std::string> MockFileHandler::getMetadata(
    const std::string& path) const {
  std::map<std::string, std::string> metadata;
  
  if (!fileExists(path)) {
    return metadata;
  }
  
  std::ostringstream sizeStr;
  sizeStr << getFileSize(path);
  metadata["size"] = sizeStr.str();
  metadata["mimeType"] = detectMimeType(path);
  metadata["permissions"] = "644";
  metadata["modified"] = "2026-01-01T00:00:00Z";
  
  return metadata;
}

std::string MockFileHandler::detectMimeType(const std::string& path) const {
  std::string ext = getExtension(path);
  
  if (ext == ".txt") return "text/plain";
  if (ext == ".html" || ext == ".htm") return "text/html";
  if (ext == ".css") return "text/css";
  if (ext == ".js") return "application/javascript";
  if (ext == ".json") return "application/json";
  if (ext == ".pdf") return "application/pdf";
  if (ext == ".jpg" || ext == ".jpeg") return "image/jpeg";
  if (ext == ".png") return "image/png";
  if (ext == ".gif") return "image/gif";
  if (ext == ".zip") return "application/zip";
  
  return "application/octet-stream";
}

std::string MockFileHandler::calculateChecksum(const std::string& path) const {
  // Simple mock checksum - just return a fixed value for testing
  if (fileExists(path)) {
    std::ostringstream oss;
    oss << "checksum_" << getFileSize(path);
    return oss.str();
  }
  return "";
}

// ============================================================================
// File Management Operations
// ============================================================================

bool MockFileHandler::deleteFile(const std::string& path) {
  trackCall("deleteFile");
  
  std::map<std::string, std::vector<char> >::iterator it = m_mockFiles.find(path);
  if (it != m_mockFiles.end()) {
    m_mockFiles.erase(it);
    return true;
  }
  
  return false;
}

bool MockFileHandler::renameFile(const std::string& oldPath, 
                                  const std::string& newPath) {
  trackCall("renameFile");
  
  std::map<std::string, std::vector<char> >::iterator it = m_mockFiles.find(oldPath);
  if (it != m_mockFiles.end()) {
    m_mockFiles[newPath] = it->second;
    m_mockFiles.erase(it);
    return true;
  }
  
  return false;
}

size_t MockFileHandler::getAvailableDiskSpace(const std::string&) const {
  return m_availableDiskSpace;
}

// ============================================================================
// Mock Configuration & Verification
// ============================================================================

void MockFileHandler::setWriteSuccess(bool success) {
  m_writeSuccess = success;
}

void MockFileHandler::setReadSuccess(bool success) {
  m_readSuccess = success;
}

void MockFileHandler::setAvailableDiskSpace(size_t bytes) {
  m_availableDiskSpace = bytes;
}

void MockFileHandler::setFileExists(const std::string& path, bool exists) {
  m_fileExistence[path] = exists;
}

int MockFileHandler::getCallCount(const std::string& methodName) const {
  std::map<std::string, int>::const_iterator it = m_callCounts.find(methodName);
  if (it != m_callCounts.end()) {
    return it->second;
  }
  return 0;
}

std::vector<std::string> MockFileHandler::getCalledMethods() const {
  return m_callHistory;
}

void MockFileHandler::clearCallHistory() {
  m_callCounts.clear();
  m_callHistory.clear();
}

const std::map<std::string, std::vector<char> >& 
MockFileHandler::getMockFiles() const {
  return m_mockFiles;
}

void MockFileHandler::setMockFile(const std::string& path, 
                                   const std::vector<char>& data) {
  m_mockFiles[path] = data;
}

// ============================================================================
// Private Helper Methods
// ============================================================================

void MockFileHandler::trackCall(const std::string& methodName) {
  m_callCounts[methodName]++;
  m_callHistory.push_back(methodName);
}

std::string MockFileHandler::getExtension(const std::string& path) const {
  size_t dotPos = path.rfind('.');
  if (dotPos != std::string::npos && dotPos < path.length() - 1) {
    return path.substr(dotPos);
  }
  return "";
}
