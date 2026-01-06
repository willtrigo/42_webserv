/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiRequest.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/05 17:37:21 by dande-je          #+#    #+#             */
/*   Updated: 2026/01/06 01:45:16 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "infrastructure/cgi/exceptions/CgiExecutionException.hpp"
#include "infrastructure/cgi/primitives/CgiRequest.hpp"

#include <cctype>
#include <cstdlib>
#include <sstream>

namespace infrastructure {
namespace cgi {
namespace primitives {

CgiRequest::CgiRequest() {}

CgiRequest::CgiRequest(
    const domain::http::entities::HttpRequest& httpRequest,
    const domain::configuration::value_objects::CgiConfig& cgiConfig,
    const domain::http::value_objects::RouteMatchInfo& matchInfo,
    const std::string& serverName, unsigned int serverPort)
    : m_scriptPath(cgiConfig.getScriptPath()),
      m_interpreter(cgiConfig.getScriptPath()) {
  buildStandardEnvironment(httpRequest, cgiConfig, matchInfo, serverName,
                           serverPort);

  if (httpRequest.hasBody()) {
    m_requestBody = httpRequest.getBody();
  }

  validate();
}

CgiRequest::CgiRequest(const CgiRequest& other)
    : m_environment(other.m_environment),
      m_requestBody(other.m_requestBody),
      m_scriptPath(other.m_scriptPath),
      m_interpreter(other.m_interpreter) {}

CgiRequest::~CgiRequest() {}

CgiRequest& CgiRequest::operator=(const CgiRequest& other) {
  if (this != &other) {
    m_environment = other.m_environment;
    m_requestBody = other.m_requestBody;
    m_scriptPath = other.m_scriptPath;
    m_interpreter = other.m_interpreter;
    m_envStrings.clear();
  }
  return *this;
}

const CgiRequest::EnvironmentMap& CgiRequest::getEnvironment() const {
  return m_environment;
}

const std::vector<char>& CgiRequest::getRequestBody() const {
  return m_requestBody;
}

const std::string& CgiRequest::getScriptPath() const { return m_scriptPath; }

const std::string& CgiRequest::getInterpreter() const { return m_interpreter; }

void CgiRequest::setEnvironmentVariable(const std::string& name,
                                        const std::string& value) {
  if (!isValidEnvironmentName(name)) {
    throw exceptions::CgiExecutionException(
        "Invalid environment variable name: " + name,
        exceptions::CgiExecutionException::ENVIRONMENT_ERROR);
  }

  m_environment[name] = sanitizeEnvironmentValue(value);
}

void CgiRequest::removeEnvironmentVariable(const std::string& name) {
  m_environment.erase(name);
}

void CgiRequest::setRequestBody(const std::vector<char>& body) {
  m_requestBody = body;

  std::ostringstream oss;
  oss << m_requestBody.size();
  setEnvironmentVariable("CONTENT_LENGTH", oss.str());
}

std::vector<std::string> CgiRequest::buildArgv() const {
  std::vector<std::string> argv;

  if (!m_interpreter.empty()) {
    argv.push_back(m_interpreter);
  }

  if (!m_scriptPath.empty()) {
    argv.push_back(m_scriptPath);
  }

  return argv;
}

std::vector<char*> CgiRequest::buildEnvp() const {
  m_envStrings.clear();
  m_envStrings.reserve(m_environment.size());

  std::vector<char*> envp;
  envp.reserve(m_environment.size() + 1);

  for (EnvironmentMap::const_iterator it = m_environment.begin();
       it != m_environment.end(); ++it) {
    std::string envString = it->first + "=" + it->second;
    m_envStrings.push_back(envString);
  }

  for (std::size_t i = 0; i < m_envStrings.size(); ++i) {
    envp.push_back(const_cast<char*>(m_envStrings[i].c_str()));
  }

  envp.push_back(NULL);

  return envp;
}

bool CgiRequest::isValid() const {
  try {
    validate();
    return true;
  } catch (const exceptions::CgiExecutionException&) {
    return false;
  }
}

void CgiRequest::validate() const {
  if (m_scriptPath.empty()) {
    throw exceptions::CgiExecutionException(
        "CGI script path cannot be empty",
        exceptions::CgiExecutionException::SCRIPT_NOT_FOUND);
  }

  if (m_environment.find("REQUEST_METHOD") == m_environment.end()) {
    throw exceptions::CgiExecutionException(
        "Missing REQUEST_METHOD environment variable",
        exceptions::CgiExecutionException::ENVIRONMENT_ERROR);
  }

  if (m_environment.find("SCRIPT_NAME") == m_environment.end()) {
    throw exceptions::CgiExecutionException(
        "Missing SCRIPT_NAME environment variable",
        exceptions::CgiExecutionException::ENVIRONMENT_ERROR);
  }

  if (m_environment.find("SERVER_PROTOCOL") == m_environment.end()) {
    throw exceptions::CgiExecutionException(
        "Missing SERVER_PROTOCOL environment variable",
        exceptions::CgiExecutionException::ENVIRONMENT_ERROR);
  }
}

CgiRequest CgiRequest::fromHttpRequest(
    const domain::http::entities::HttpRequest& httpRequest,
    const domain::configuration::value_objects::CgiConfig& cgiConfig,
    const domain::http::value_objects::RouteMatchInfo& matchInfo,
    const std::string& serverName, unsigned int serverPort) {
  return CgiRequest(httpRequest, cgiConfig, matchInfo, serverName, serverPort);
}

void CgiRequest::buildStandardEnvironment(
    const domain::http::entities::HttpRequest& httpRequest,
    const domain::configuration::value_objects::CgiConfig& cgiConfig,
    const domain::http::value_objects::RouteMatchInfo& matchInfo,
    const std::string& serverName, unsigned int serverPort) {
  const domain::configuration::value_objects::CgiConfig::ParameterMap& params =
      cgiConfig.getParameters();
  for (domain::configuration::value_objects::CgiConfig::ParameterMap::
           const_iterator it = params.begin();
       it != params.end(); ++it) {
    m_environment[it->first] = it->second;
  }

  addRequestMethodInfo(httpRequest);
  addServerInfo(serverName, serverPort);
  addScriptInfo(cgiConfig, matchInfo);
  addPathInfo(httpRequest, matchInfo);
  addQueryString(httpRequest);
  addContentInfo(httpRequest);
  addHttpHeaders(httpRequest);
  addRemoteInfo();
}

void CgiRequest::addRequestMethodInfo(
    const domain::http::entities::HttpRequest& httpRequest) {
  m_environment["REQUEST_METHOD"] = httpRequest.getMethod().toString();
}

void CgiRequest::addServerInfo(const std::string& serverName,
                               unsigned int serverPort) {
  m_environment["SERVER_NAME"] = serverName;

  std::ostringstream oss;
  oss << serverPort;
  m_environment["SERVER_PORT"] = oss.str();

  m_environment["SERVER_SOFTWARE"] = "WebServ/1.0";
  m_environment["SERVER_PROTOCOL"] = "HTTP/1.1";
  m_environment["GATEWAY_INTERFACE"] = "CGI/1.1";
}

void CgiRequest::addScriptInfo(
    const domain::configuration::value_objects::CgiConfig& cgiConfig,
    const domain::http::value_objects::RouteMatchInfo& matchInfo) {
  std::string fileToServe = matchInfo.getFileToServe();

  m_environment["SCRIPT_FILENAME"] = fileToServe;
  m_environment["SCRIPT_NAME"] = fileToServe;

  std::string documentRoot = cgiConfig.getCgiRoot().toString();
  m_environment["DOCUMENT_ROOT"] = documentRoot;
}

void CgiRequest::addPathInfo(
    const domain::http::entities::HttpRequest& httpRequest,
    const domain::http::value_objects::RouteMatchInfo& matchInfo) {
  std::string requestPath = httpRequest.getPath().toString();
  std::string scriptName = matchInfo.getFileToServe();

  std::string pathInfo = extractPathInfo(requestPath, scriptName);
  if (!pathInfo.empty()) {
    m_environment["PATH_INFO"] = pathInfo;

    std::string documentRoot = m_environment["DOCUMENT_ROOT"];
    m_environment["PATH_TRANSLATED"] = documentRoot + pathInfo;
  }

  m_environment["REQUEST_URI"] = requestPath;
}

void CgiRequest::addQueryString(
    const domain::http::entities::HttpRequest& httpRequest) {
  const domain::http::value_objects::QueryStringBuilder& queryBuilder =
      httpRequest.getQuery();

  if (queryBuilder.hasQueryString()) {
    std::string queryString =
        buildQueryStringFromParameters(queryBuilder.getAllParameters());
    m_environment["QUERY_STRING"] = queryString;
  } else {
    m_environment["QUERY_STRING"] = "";
  }
}

void CgiRequest::addContentInfo(
    const domain::http::entities::HttpRequest& httpRequest) {
  if (httpRequest.hasBody()) {
    std::ostringstream oss;
    oss << httpRequest.getBody().size();
    m_environment["CONTENT_LENGTH"] = oss.str();

    if (httpRequest.hasHeader("content-type")) {
      m_environment["CONTENT_TYPE"] = httpRequest.getHeader("content-type");
    }
  } else {
    m_environment["CONTENT_LENGTH"] = "0";
  }
}

void CgiRequest::addHttpHeaders(
    const domain::http::entities::HttpRequest& httpRequest) {
  const std::map<std::string, std::string>& headers = httpRequest.getHeaders();

  for (std::map<std::string, std::string>::const_iterator it = headers.begin();
       it != headers.end(); ++it) {
    std::string cgiName = httpHeaderToCgiName(it->first);
    m_environment[cgiName] = it->second;
  }
}

void CgiRequest::addRemoteInfo(const std::string& remoteAddr,
                               unsigned int remotePort) {
  m_environment["REMOTE_ADDR"] = remoteAddr;

  if (remotePort > 0) {
    std::ostringstream oss;
    oss << remotePort;
    m_environment["REMOTE_PORT"] = oss.str();
  }
}

std::string CgiRequest::buildQueryStringFromParameters(
    const std::map<std::string, std::string>& parameters) {
  if (parameters.empty()) {
    return "";
  }

  std::ostringstream queryString;
  bool first = true;

  for (std::map<std::string, std::string>::const_iterator it =
           parameters.begin();
       it != parameters.end(); ++it) {
    if (!first) {
      queryString << "&";
    }
    queryString
        << domain::http::value_objects::QueryStringBuilder::encode(it->first)
        << "="
        << domain::http::value_objects::QueryStringBuilder::encode(it->second);
    first = false;
  }

  return queryString.str();
}

std::string CgiRequest::httpHeaderToCgiName(const std::string& headerName) {
  std::string cgiName = "HTTP_";

  for (std::size_t i = 0; i < headerName.length(); ++i) {
    char chr = headerName[i];
    if (chr == '-') {
      cgiName += '_';
    } else {
      cgiName +=
          static_cast<char>(std::toupper(static_cast<unsigned char>(chr)));
    }
  }

  return cgiName;
}

std::string CgiRequest::extractPathInfo(const std::string& requestPath,
                                        const std::string& scriptName) {
  std::size_t pos = requestPath.find(scriptName);

  if (pos == std::string::npos) {
    return "";
  }

  std::size_t pathInfoStart = pos + scriptName.length();
  if (pathInfoStart >= requestPath.length()) {
    return "";
  }

  return requestPath.substr(pathInfoStart);
}

std::string CgiRequest::extractQueryString(const std::string& fullPath) {
  std::size_t pos = fullPath.find('?');

  if (pos == std::string::npos) {
    return "";
  }

  return fullPath.substr(pos + 1);
}

bool CgiRequest::isValidEnvironmentName(const std::string& name) {
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

std::string CgiRequest::sanitizeEnvironmentValue(const std::string& value) {
  std::string result;
  result.reserve(value.length());

  for (std::size_t i = 0; i < value.length(); ++i) {
    char chr = value[i];
    if ((std::iscntrl(static_cast<unsigned char>(chr)) == 0) || chr == '\t' ||
        chr == '\n') {
      result += chr;
    }
  }

  return result;
}

}  // namespace primitives
}  // namespace cgi
}  // namespace infrastructure
