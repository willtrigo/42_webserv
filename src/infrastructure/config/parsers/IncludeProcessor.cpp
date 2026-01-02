/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IncludeProcessor.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/30 21:45:14 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/31 03:47:37 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "infrastructure/config/exceptions/ConfigException.hpp"
#include "infrastructure/config/parsers/ConfigParser.hpp"
#include "infrastructure/config/parsers/IncludeProcessor.hpp"

#include <algorithm>
#include <dirent.h>
#include <sstream>
#include <sys/stat.h>

namespace infrastructure {
namespace config {
namespace parsers {

IncludeProcessor::IncludeProcessor(application::ports::ILogger& logger)
    : m_logger(logger) {}

IncludeProcessor::~IncludeProcessor() {}

void IncludeProcessor::processIncludes(
    const std::string& pattern,
    domain::configuration::entities::HttpConfig& httpConfig,
    std::size_t currentDepth) {
  if (currentDepth >= MAX_INCLUDE_DEPTH) {
    std::ostringstream oss;
    oss << "Maximum include depth (" << MAX_INCLUDE_DEPTH
        << ") exceeded for pattern: " << pattern;
    throw exceptions::ConfigException(
        oss.str(), exceptions::ConfigException::INCLUDE_RECURSION);
  }

  std::vector<std::string> files = expandGlobPattern(pattern);

  if (files.empty()) {
    std::ostringstream oss;
    oss << "No files matched include pattern: " << pattern;
    m_logger.warn(oss.str());
    return;
  }

  std::sort(files.begin(), files.end());

  std::ostringstream oss;
  oss << "Processing include pattern '" << pattern << "' (depth "
      << currentDepth << "), found " << files.size() << " files";
  m_logger.debug(oss.str());

  for (std::size_t i = 0; i < files.size(); ++i) {
    const std::string& filePath = files[i];

    try {
      std::ostringstream logMsg;
      logMsg << "Including file: " << filePath;
      m_logger.debug(logMsg.str());

      ConfigParser includeParser(m_logger);
      domain::configuration::entities::HttpConfig* includedConfig =
          includeParser.parseFile(filePath);

      const domain::configuration::entities::HttpConfig::ServerConfigs&
          servers = includedConfig->getServerConfigs();

      for (std::size_t j = 0; j < servers.size(); ++j) {
        domain::configuration::entities::ServerConfig* serverCopy =
            new domain::configuration::entities::ServerConfig(*servers[j]);
        httpConfig.addServerConfig(serverCopy);
      }

      std::ostringstream successMsg;
      successMsg << "Successfully included " << servers.size()
                 << " server(s) from " << filePath;
      m_logger.info(successMsg.str());

      delete includedConfig;

    } catch (const exceptions::ConfigException& e) {
      std::ostringstream errorMsg;
      errorMsg << "Failed to include file '" << filePath << "': " << e.what();
      m_logger.error(errorMsg.str());
      throw exceptions::ConfigException(
          errorMsg.str(), exceptions::ConfigException::LOAD_UNEXPECTED);
    }
  }
}

std::vector<std::string> IncludeProcessor::expandGlobPattern(
    const std::string& pattern) {
  std::vector<std::string> result;

  if (pattern.find('*') == std::string::npos &&
      pattern.find('?') == std::string::npos) {
    if (fileExists(pattern)) {
      result.push_back(pattern);
    }
    return result;
  }

  std::string dirPath = extractDirectory(pattern);
  std::string filePattern = extractFilename(pattern);

  if (dirPath.empty()) {
    dirPath = ".";
  }

  std::vector<std::string> dirEntries = listDirectory(dirPath);

  for (std::size_t i = 0; i < dirEntries.size(); ++i) {
    if (matchesPattern(dirEntries[i], filePattern)) {
      std::string fullPath = dirPath + "/" + dirEntries[i];
      result.push_back(fullPath);
    }
  }

  return result;
}

// TODO: maybe use compoment of filesystem
std::vector<std::string> IncludeProcessor::listDirectory(
    const std::string& dirPath) {
  std::vector<std::string> result;

  DIR* dir = opendir(dirPath.c_str());
  if (dir == NULL) {
    std::ostringstream oss;
    oss << "Cannot open directory: " << dirPath;
    m_logger.warn(oss.str());
    return result;
  }

  struct dirent* entry;
  while ((entry = readdir(dir)) != NULL) {
    std::string filename = entry->d_name;

    if (filename == "." || filename == "..") {
      continue;
    }

    std::string fullPath = dirPath + "/" + filename;
    struct stat statbuf;
    if (stat(fullPath.c_str(), &statbuf) == 0) {
      if (S_ISREG(statbuf.st_mode)) {
        result.push_back(filename);
      }
    }
  }

  closedir(dir);
  return result;
}

// TODO: maybe use RegexPattern.cpp
bool IncludeProcessor::matchesPattern(const std::string& filename,
                                      const std::string& pattern) {
  std::size_t fileIdx = 0;
  std::size_t patternIdx = 0;
  std::size_t starIdx = std::string::npos;
  std::size_t matchIdx = 0;

  while (fileIdx < filename.length()) {
    if (patternIdx < pattern.length() &&
        (pattern[patternIdx] == '?' ||
         pattern[patternIdx] == filename[fileIdx])) {
      ++fileIdx;
      ++patternIdx;
    } else if (patternIdx < pattern.length() && pattern[patternIdx] == '*') {
      starIdx = patternIdx;
      matchIdx = fileIdx;
      ++patternIdx;
    } else if (starIdx != std::string::npos) {
      patternIdx = starIdx + 1;
      ++matchIdx;
      fileIdx = matchIdx;
    } else {
      return false;
    }
  }

  while (patternIdx < pattern.length() && pattern[patternIdx] == '*') {
    ++patternIdx;
  }

  return patternIdx == pattern.length();
}

std::string IncludeProcessor::extractDirectory(const std::string& path) {
  std::size_t lastSlash = path.find_last_of('/');
  if (lastSlash == std::string::npos) {
    return "";
  }
  return path.substr(0, lastSlash);
}

std::string IncludeProcessor::extractFilename(const std::string& path) {
  std::size_t lastSlash = path.find_last_of('/');
  if (lastSlash == std::string::npos) {
    return path;
  }
  return path.substr(lastSlash + 1);
}

bool IncludeProcessor::isAbsolutePath(const std::string& path) {
  return !path.empty() && path[0] == '/';
}

bool IncludeProcessor::fileExists(const std::string& path) {
  struct stat statbuf;
  return stat(path.c_str(), &statbuf) == 0 && S_ISREG(statbuf.st_mode);
}

}  // namespace parsers
}  // namespace config
}  // namespace infrastructure
