/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiConfig.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: umeneses <umeneses@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/22 13:12:24 by dande-je          #+#    #+#             */
/*   Updated: 2026/01/04 19:41:29 by umeneses         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "domain/configuration/exceptions/CgiConfigException.hpp"
#include "domain/configuration/value_objects/CgiConfig.hpp"

#include <cctype>
#include <cstring>
#include <sstream>

namespace domain {
namespace configuration {
namespace value_objects {

const std::string CgiConfig::DEFAULT_QUERY_STRING = "QUERY_STRING";
const std::string CgiConfig::DEFAULT_REQUEST_METHOD = "REQUEST_METHOD";
const std::string CgiConfig::DEFAULT_CONTENT_TYPE = "CONTENT_TYPE";
const std::string CgiConfig::DEFAULT_CONTENT_LENGTH = "CONTENT_LENGTH";
const std::string CgiConfig::DEFAULT_SCRIPT_FILENAME = "SCRIPT_FILENAME";
const std::string CgiConfig::DEFAULT_SCRIPT_NAME = "SCRIPT_NAME";
const std::string CgiConfig::DEFAULT_DOCUMENT_ROOT = "DOCUMENT_ROOT";
const std::string CgiConfig::DEFAULT_SERVER_SOFTWARE = "SERVER_SOFTWARE";
const std::string CgiConfig::DEFAULT_SERVER_NAME = "SERVER_NAME";
const std::string CgiConfig::DEFAULT_SERVER_PORT = "SERVER_PORT";
const std::string CgiConfig::DEFAULT_SERVER_PROTOCOL = "SERVER_PROTOCOL";
const std::string CgiConfig::DEFAULT_GATEWAY_INTERFACE = "GATEWAY_INTERFACE";
const std::string CgiConfig::DEFAULT_REMOTE_ADDR = "REMOTE_ADDR";
const std::string CgiConfig::DEFAULT_REMOTE_PORT = "REMOTE_PORT";
const std::string CgiConfig::DEFAULT_REQUEST_URI = "REQUEST_URI";

CgiConfig::CgiConfig()
    : m_cgiRoot(filesystem::value_objects::Path::rootDirectory()),
      m_extensionPattern(shared::value_objects::RegexPattern::phpExtension()) {
  initializeDefaultParameters();
}

CgiConfig::CgiConfig(
    const std::string& scriptPath,
    const filesystem::value_objects::Path& cgiRoot,
    const shared::value_objects::RegexPattern& extensionPattern)
    : m_scriptPath(normalizeScriptPath(scriptPath)),
      m_cgiRoot(cgiRoot),
      m_extensionPattern(extensionPattern) {
  if (m_scriptPath.empty()) {
    throw exceptions::CgiConfigException(
        "CGI script path cannot be empty",
        exceptions::CgiConfigException::INVALID_SCRIPT_PATH);
  }

  if (!isAbsolutePath(m_scriptPath)) {
    throw exceptions::CgiConfigException(
        "CGI script path must be absolute: " + m_scriptPath,
        exceptions::CgiConfigException::INVALID_SCRIPT_PATH);
  }

  initializeDefaultParameters();
}

CgiConfig::CgiConfig(const CgiConfig& other) { copyFrom(other); }

CgiConfig::~CgiConfig() {}

CgiConfig& CgiConfig::operator=(const CgiConfig& other) {
  if (this != &other) {
    copyFrom(other);
  }
  return *this;
}

void CgiConfig::copyFrom(const CgiConfig& other) {
  m_scriptPath = other.m_scriptPath;
  m_cgiRoot = other.m_cgiRoot;
  m_extensionPattern = other.m_extensionPattern;
  m_parameters = other.m_parameters;
}

const std::string& CgiConfig::getScriptPath() const { return m_scriptPath; }

const filesystem::value_objects::Path& CgiConfig::getCgiRoot() const {
  return m_cgiRoot;
}

const shared::value_objects::RegexPattern& CgiConfig::getExtensionPattern()
    const {
  return m_extensionPattern;
}

const CgiConfig::ParameterMap& CgiConfig::getParameters() const {
  return m_parameters;
}

std::string CgiConfig::getParameter(const std::string& name) const {
  ParameterMap::const_iterator iter = m_parameters.find(name);
  if (iter != m_parameters.end()) {
    return iter->second;
  }
  return "";
}

bool CgiConfig::hasParameter(const std::string& name) const {
  return m_parameters.find(name) != m_parameters.end();
}

void CgiConfig::setScriptPath(const std::string& scriptPath) {
  std::string normalizedPath = normalizeScriptPath(scriptPath);

  if (normalizedPath.empty()) {
    throw exceptions::CgiConfigException(
        "CGI script path cannot be empty",
        exceptions::CgiConfigException::EMPTY_SCRIPT_PATH);
  }

  if (!isAbsolutePath(normalizedPath)) {
    throw exceptions::CgiConfigException(
        "CGI script path must be absolute: " + normalizedPath,
        exceptions::CgiConfigException::INVALID_SCRIPT_PATH);
  }

  m_scriptPath = normalizedPath;
}

void CgiConfig::setCgiRoot(const filesystem::value_objects::Path& cgiRoot) {
  if (cgiRoot.isEmpty()) {
    throw exceptions::CgiConfigException(
        "CGI root directory cannot be empty",
        exceptions::CgiConfigException::EMPTY_CGI_ROOT);
  }

  m_cgiRoot = cgiRoot;
}

void CgiConfig::setExtensionPattern(
    const shared::value_objects::RegexPattern& pattern) {
  if (pattern.isEmpty()) {
    throw exceptions::CgiConfigException(
        "CGI extension pattern cannot be empty",
        exceptions::CgiConfigException::INVALID_EXTENSION_PATTERN);
  }

  m_extensionPattern = pattern;
}

void CgiConfig::addParameter(const std::string& name,
                             const std::string& value) {
  if (!isValidParameterName(name)) {
    std::ostringstream oss;
    oss << "Invalid CGI parameter name: '" << name << "'";
    throw exceptions::CgiConfigException(
        oss.str(), exceptions::CgiConfigException::INVALID_CGI_PARAM);
  }

  if (!isValidParameterValue(value)) {
    std::ostringstream oss;
    oss << "Invalid CGI parameter value for '" << name << "': '" << value
        << "'";
    throw exceptions::CgiConfigException(
        oss.str(), exceptions::CgiConfigException::INVALID_CGI_PARAM);
  }

  ParameterMap::iterator iter = m_parameters.find(name);
  if (iter != m_parameters.end()) {
    if (!iter->second.empty()) {
      std::ostringstream oss;
      oss << "Duplicate CGI parameter: '" << name << "'";
      throw exceptions::CgiConfigException(
          oss.str(), exceptions::CgiConfigException::DUPLICATE_CGI_PARAM);
    }
  }

  m_parameters[name] = value;
}

void CgiConfig::removeParameter(const std::string& name) {
  m_parameters.erase(name);
}

void CgiConfig::setParameters(const ParameterMap& parameters) {
  for (ParameterMap::const_iterator it = parameters.begin();
       it != parameters.end(); ++it) {
    if (!isValidParameterName(it->first)) {
      std::ostringstream oss;
      oss << "Invalid CGI parameter name in batch: '" << it->first << "'";
      throw exceptions::CgiConfigException(
          oss.str(), exceptions::CgiConfigException::INVALID_CGI_PARAM);
    }

    if (!isValidParameterValue(it->second)) {
      std::ostringstream oss;
      oss << "Invalid CGI parameter value in batch for '" << it->first << "': '"
          << it->second << "'";
      throw exceptions::CgiConfigException(
          oss.str(), exceptions::CgiConfigException::INVALID_CGI_PARAM);
    }
  }

  m_parameters = parameters;
}

CgiConfig CgiConfig::createPhpCgi(const std::string& phpBinary) {
  CgiConfig config;
  config.setScriptPath(phpBinary);
  config.setExtensionPattern(
      shared::value_objects::RegexPattern::phpExtension());

  config.addParameter("REDIRECT_STATUS", "200");
  config.addParameter("PHP_VALUE", "auto_prepend_file=none");

  return config;
}

CgiConfig CgiConfig::createPythonCgi(const std::string& pythonBinary) {
  CgiConfig config;
  config.setScriptPath(pythonBinary);
  config.setExtensionPattern(
      shared::value_objects::RegexPattern::pythonExtension());

  config.addParameter("PYTHONPATH", "/usr/lib/python3.8");

  return config;
}

CgiConfig CgiConfig::createPerlCgi(const std::string& perlBinary) {
  CgiConfig config;
  config.setScriptPath(perlBinary);

  shared::value_objects::RegexPattern perlPattern(
      "\\.(pl|cgi)$",
      shared::value_objects::RegexPattern::FLAG_CASE_INSENSITIVE);
  config.setExtensionPattern(perlPattern);

  config.addParameter("PERL5LIB", "/usr/local/lib/perl5");

  return config;
}

bool CgiConfig::isValid() const {
  try {
    validate();
    return true;
  } catch (const exceptions::CgiConfigException&) {
    return false;
  }
}

void CgiConfig::validate() const {
  if (!m_scriptPath.empty()) {
    validateScriptPath();
  }

  if (!m_cgiRoot.isEmpty()) {
    validateCgiRoot();
  }

  validateExtensionPattern();
  validateParameters();

  if (!m_scriptPath.empty()) {
    static const std::string REQUIRED_PARAMS[] = {
        DEFAULT_SCRIPT_FILENAME, DEFAULT_REQUEST_METHOD,
        DEFAULT_SERVER_PROTOCOL, DEFAULT_GATEWAY_INTERFACE};
    static const std::size_t REQUIRED_PARAMS_COUNT = 4;

    for (std::size_t i = 0; i < REQUIRED_PARAMS_COUNT; ++i) {
      if (!hasParameter(REQUIRED_PARAMS[i])) {
        std::ostringstream oss;
        oss << "Missing required CGI parameter: " << REQUIRED_PARAMS[i];
        throw exceptions::CgiConfigException(
            oss.str(), exceptions::CgiConfigException::MISSING_REQUIRED_PARAMS);
      }
    }
  }
}

void CgiConfig::validateScriptPath() const {
  if (m_scriptPath.empty()) {
    return;
  }

  if (!isAbsolutePath(m_scriptPath)) {
    throw exceptions::CgiConfigException(
        "CGI script path must be absolute: " + m_scriptPath,
        exceptions::CgiConfigException::INVALID_SCRIPT_PATH);
  }
}

void CgiConfig::validateCgiRoot() const {
  if (m_cgiRoot.isEmpty()) {
    throw exceptions::CgiConfigException(
        "CGI root directory cannot be empty",
        exceptions::CgiConfigException::EMPTY_CGI_ROOT);
  }
}

void CgiConfig::validateExtensionPattern() const {
  if (m_extensionPattern.isEmpty()) {
    throw exceptions::CgiConfigException(
        "CGI extension pattern cannot be empty",
        exceptions::CgiConfigException::INVALID_EXTENSION_PATTERN);
  }
}

void CgiConfig::validateParameters() const {
  for (ParameterMap::const_iterator it = m_parameters.begin();
       it != m_parameters.end(); ++it) {
    if (!isValidParameterName(it->first)) {
      std::ostringstream oss;
      oss << "Invalid CGI parameter name: '" << it->first << "'";
      throw exceptions::CgiConfigException(
          oss.str(), exceptions::CgiConfigException::INVALID_CGI_PARAM);
    }

    if (!isValidParameterValue(it->second)) {
      std::ostringstream oss;
      oss << "Invalid CGI parameter value for '" << it->first << "': '"
          << it->second << "'";
      throw exceptions::CgiConfigException(
          oss.str(), exceptions::CgiConfigException::INVALID_CGI_PARAM);
    }
  }
}

bool CgiConfig::matchesExtension(const std::string& filename) const {
  return m_extensionPattern.matches(filename);
}

std::string CgiConfig::resolveScriptPath(const std::string& requestPath) const {
  if (requestPath.empty()) {
    return "";
  }

  std::string result = m_cgiRoot.toString();
  if (!result.empty() && result[result.length() - 1] != '/' &&
      !requestPath.empty() && requestPath[0] != '/') {
    result += "/";
  }
  result += requestPath;

  return result;
}

CgiConfig::ParameterMap CgiConfig::buildEnvironment(
    const std::map<std::string, std::string>& requestInfo) const {
  ParameterMap env = m_parameters;

  for (std::map<std::string, std::string>::const_iterator it =
           requestInfo.begin();
       it != requestInfo.end(); ++it) {
    env[it->first] = it->second;
  }

  if (env.find(DEFAULT_SCRIPT_FILENAME) != env.end()) {
    std::string scriptName = env[DEFAULT_SCRIPT_FILENAME];
    if (!scriptName.empty() && scriptName[0] != '/') {
      env[DEFAULT_SCRIPT_FILENAME] = m_cgiRoot.toString() + "/" + scriptName;
    }
  }

  return env;
}

bool CgiConfig::operator==(const CgiConfig& other) const {
  return m_scriptPath == other.m_scriptPath && m_cgiRoot == other.m_cgiRoot &&
         m_extensionPattern == other.m_extensionPattern &&
         m_parameters == other.m_parameters;
}

bool CgiConfig::operator!=(const CgiConfig& other) const {
  return !(*this == other);
}

void CgiConfig::clear() {
  m_scriptPath.clear();
  m_cgiRoot = filesystem::value_objects::Path::rootDirectory();
  m_extensionPattern = shared::value_objects::RegexPattern::phpExtension();
  m_parameters.clear();
  initializeDefaultParameters();
}

void CgiConfig::initializeDefaultParameters() {
  m_parameters[DEFAULT_GATEWAY_INTERFACE] = "CGI/1.1";
  m_parameters[DEFAULT_SERVER_PROTOCOL] = "HTTP/1.1";
  m_parameters[DEFAULT_SERVER_SOFTWARE] = "WebServ/1.0";

  m_parameters[DEFAULT_SCRIPT_FILENAME] = "";
  m_parameters[DEFAULT_SCRIPT_NAME] = "";
  m_parameters[DEFAULT_DOCUMENT_ROOT] = "";
  m_parameters[DEFAULT_QUERY_STRING] = "";
  m_parameters[DEFAULT_REQUEST_METHOD] = "";
  m_parameters[DEFAULT_CONTENT_TYPE] = "";
  m_parameters[DEFAULT_CONTENT_LENGTH] = "";
  m_parameters[DEFAULT_SERVER_NAME] = "";
  m_parameters[DEFAULT_SERVER_PORT] = "";
  m_parameters[DEFAULT_REMOTE_ADDR] = "";
  m_parameters[DEFAULT_REMOTE_PORT] = "";
  m_parameters[DEFAULT_REQUEST_URI] = "";
}

bool CgiConfig::isValidParameterName(const std::string& name) {
  if (name.empty()) {
    return false;
  }

  for (std::size_t i = 0; i < name.length(); ++i) {
    char chr = name[i];
    if ((std::isalnum(static_cast<unsigned char>(chr)) == 0) && chr != '_') {
      return false;
    }
  }

  return true;
}

bool CgiConfig::isValidParameterValue(const std::string& value) {
  for (std::size_t i = 0; i < value.length(); ++i) {
    char chr = value[i];
    if ((std::iscntrl(static_cast<unsigned char>(chr)) != 0) || chr == '\0') {
      return false;
    }
  }

  return true;
}

std::string CgiConfig::normalizeScriptPath(const std::string& scriptPath) {
  std::string result = scriptPath;

  std::size_t start = result.find_first_not_of(" \t\n\r");
  std::size_t end = result.find_last_not_of(" \t\n\r");

  if (start == std::string::npos) {
    return "";
  }

  result = result.substr(start, end - start + 1);
  return result;
}

bool CgiConfig::isAbsolutePath(const std::string& path) {
  return !path.empty() && path[0] == '/';
}

}  // namespace value_objects
}  // namespace configuration
}  // namespace domain
