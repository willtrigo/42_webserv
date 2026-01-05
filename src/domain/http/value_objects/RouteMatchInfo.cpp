/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RouteMatchInfo.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/05 15:17:58 by dande-je          #+#    #+#             */
/*   Updated: 2026/01/05 15:19:27 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "domain/http/exceptions/RouteMatchInfoException.hpp"
#include "domain/http/value_objects/RouteMatchInfo.hpp"

#include <sstream>

namespace domain {
namespace http {
namespace value_objects {

RouteMatchInfo::RouteMatchInfo() : m_isDirectory(false) {
  try {
    validate();
  } catch (const std::exception& e) {
    std::ostringstream oss;
    oss << "Failed to create empty RouteMatchInfo: " << e.what();
    throw exceptions::RouteMatchInfoException(
        oss.str(), exceptions::RouteMatchInfoException::RESOLUTION_ERROR);
  }
}

RouteMatchInfo::RouteMatchInfo(
    const filesystem::value_objects::Path& resolvedPath, bool isDirectory,
    const std::string& fileToServe,
    const std::map<std::string, std::string>& queryParams)
    : m_resolvedPath(resolvedPath),
      m_isDirectory(isDirectory),
      m_fileToServe(fileToServe),
      m_queryParams(queryParams) {
  try {
    validate();
  } catch (const std::exception& e) {
    std::ostringstream oss;
    oss << "Failed to create RouteMatchInfo: " << e.what();
    throw exceptions::RouteMatchInfoException(
        oss.str(), exceptions::RouteMatchInfoException::RESOLUTION_ERROR);
  }
}

RouteMatchInfo::RouteMatchInfo(const RouteMatchInfo& other)
    : m_resolvedPath(other.m_resolvedPath),
      m_isDirectory(other.m_isDirectory),
      m_fileToServe(other.m_fileToServe),
      m_queryParams(other.m_queryParams) {}

RouteMatchInfo::~RouteMatchInfo() {}

RouteMatchInfo& RouteMatchInfo::operator=(const RouteMatchInfo& other) {
  if (this != &other) {
    m_resolvedPath = other.m_resolvedPath;
    m_isDirectory = other.m_isDirectory;
    m_fileToServe = other.m_fileToServe;
    m_queryParams = other.m_queryParams;
  }
  return *this;
}

bool RouteMatchInfo::operator==(const RouteMatchInfo& other) const {
  if (m_isDirectory != other.m_isDirectory) {
    return false;
  }

  if (m_resolvedPath != other.m_resolvedPath) {
    return false;
  }

  if (m_fileToServe != other.m_fileToServe) {
    return false;
  }

  if (m_queryParams.size() != other.m_queryParams.size()) {
    return false;
  }

  std::map<std::string, std::string>::const_iterator iter;
  std::map<std::string, std::string>::const_iterator otherIt;

  for (iter = m_queryParams.begin(); iter != m_queryParams.end(); ++iter) {
    otherIt = other.m_queryParams.find(iter->first);
    if (otherIt == other.m_queryParams.end() ||
        otherIt->second != iter->second) {
      return false;
    }
  }

  return true;
}

bool RouteMatchInfo::operator!=(const RouteMatchInfo& other) const {
  return !(*this == other);
}

const filesystem::value_objects::Path& RouteMatchInfo::getResolvedPath() const {
  return m_resolvedPath;
}

bool RouteMatchInfo::isDirectory() const { return m_isDirectory; }

const std::string& RouteMatchInfo::getFileToServe() const {
  return m_fileToServe;
}

const std::map<std::string, std::string>& RouteMatchInfo::getQueryParams()
    const {
  return m_queryParams;
}

bool RouteMatchInfo::hasQueryParam(const std::string& key) const {
  return m_queryParams.find(key) != m_queryParams.end();
}

std::string RouteMatchInfo::getQueryParam(const std::string& key) const {
  std::map<std::string, std::string>::const_iterator iter =
      m_queryParams.find(key);

  if (iter == m_queryParams.end()) {
    std::ostringstream oss;
    oss << "Query parameter not found: " << key;
    throw exceptions::RouteMatchInfoException(
        oss.str(), exceptions::RouteMatchInfoException::INVALID_QUERY_PARAMS);
  }

  return iter->second;
}

bool RouteMatchInfo::isEmpty() const {
  return m_resolvedPath.isEmpty() && m_fileToServe.empty() &&
         m_queryParams.empty();
}

RouteMatchInfo RouteMatchInfo::createEmpty() { return RouteMatchInfo(); }

RouteMatchInfo RouteMatchInfo::createForFile(
    const filesystem::value_objects::Path& resolvedPath,
    const std::string& fileToServe) {
  if (fileToServe.empty()) {
    throw exceptions::RouteMatchInfoException(
        "File to serve cannot be empty",
        exceptions::RouteMatchInfoException::INVALID_FILE_TO_SERVE);
  }

  return RouteMatchInfo(resolvedPath, false, fileToServe,
                        std::map<std::string, std::string>());
}

RouteMatchInfo RouteMatchInfo::createForDirectory(
    const filesystem::value_objects::Path& resolvedPath) {
  return RouteMatchInfo(resolvedPath, true, resolvedPath.toString(),
                        std::map<std::string, std::string>());
}

RouteMatchInfo RouteMatchInfo::createForFileWithQuery(
    const filesystem::value_objects::Path& resolvedPath,
    const std::string& fileToServe,
    const std::map<std::string, std::string>& queryParams) {
  if (fileToServe.empty()) {
    throw exceptions::RouteMatchInfoException(
        "File to serve cannot be empty",
        exceptions::RouteMatchInfoException::INVALID_FILE_TO_SERVE);
  }

  return RouteMatchInfo(resolvedPath, false, fileToServe, queryParams);
}

void RouteMatchInfo::validate() const {
  if (!m_isDirectory && !m_fileToServe.empty()) {
    if (m_resolvedPath.isEmpty()) {
      throw exceptions::RouteMatchInfoException(
          "Resolved path cannot be empty when file to serve is specified",
          exceptions::RouteMatchInfoException::INVALID_RESOLVED_PATH);
    }
  }

  std::map<std::string, std::string>::const_iterator iter;
  for (iter = m_queryParams.begin(); iter != m_queryParams.end(); ++iter) {
    if (iter->first.empty()) {
      throw exceptions::RouteMatchInfoException(
          "Query parameter key cannot be empty",
          exceptions::RouteMatchInfoException::INVALID_QUERY_PARAMS);
    }
  }
}

}  // namespace value_objects
}  // namespace http
}  // namespace domain
