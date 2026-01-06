/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiRequest.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/05 17:36:51 by dande-je          #+#    #+#             */
/*   Updated: 2026/01/06 01:43:36 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGI_REQUEST_HPP
#define CGI_REQUEST_HPP

#include "domain/configuration/value_objects/CgiConfig.hpp"
#include "domain/http/entities/HttpRequest.hpp"
#include "domain/http/value_objects/RouteMatchInfo.hpp"

#include <map>
#include <string>
#include <vector>

namespace infrastructure {
namespace cgi {
namespace primitives {

class CgiRequest {
 public:
  typedef std::map<std::string, std::string> EnvironmentMap;

  CgiRequest();
  CgiRequest(const domain::http::entities::HttpRequest& httpRequest,
             const domain::configuration::value_objects::CgiConfig& cgiConfig,
             const domain::http::value_objects::RouteMatchInfo& matchInfo,
             const std::string& serverName, unsigned int serverPort);

  CgiRequest(const CgiRequest& other);
  ~CgiRequest();

  CgiRequest& operator=(const CgiRequest& other);

  const EnvironmentMap& getEnvironment() const;
  const std::vector<char>& getRequestBody() const;
  const std::string& getScriptPath() const;
  const std::string& getInterpreter() const;

  void setEnvironmentVariable(const std::string& name,
                               const std::string& value);
  void removeEnvironmentVariable(const std::string& name);
  void setRequestBody(const std::vector<char>& body);

  std::vector<std::string> buildArgv() const;
  std::vector<char*> buildEnvp() const;

  bool isValid() const;
  void validate() const;

  static CgiRequest fromHttpRequest(
      const domain::http::entities::HttpRequest& httpRequest,
      const domain::configuration::value_objects::CgiConfig& cgiConfig,
      const domain::http::value_objects::RouteMatchInfo& matchInfo,
      const std::string& serverName, unsigned int serverPort);

 private:
  EnvironmentMap m_environment;
  std::vector<char> m_requestBody;
  std::string m_scriptPath;
  std::string m_interpreter;

  mutable std::vector<std::string> m_envStrings;

  void buildStandardEnvironment(
      const domain::http::entities::HttpRequest& httpRequest,
      const domain::configuration::value_objects::CgiConfig& cgiConfig,
      const domain::http::value_objects::RouteMatchInfo& matchInfo,
      const std::string& serverName, unsigned int serverPort);

  void addRequestMethodInfo(
      const domain::http::entities::HttpRequest& httpRequest);
  void addServerInfo(const std::string& serverName, unsigned int serverPort);
  void addScriptInfo(
      const domain::configuration::value_objects::CgiConfig& cgiConfig,
      const domain::http::value_objects::RouteMatchInfo& matchInfo);
  void addPathInfo(
      const domain::http::entities::HttpRequest& httpRequest,
      const domain::http::value_objects::RouteMatchInfo& matchInfo);
  void addQueryString(const domain::http::entities::HttpRequest& httpRequest);
  void addContentInfo(const domain::http::entities::HttpRequest& httpRequest);
  void addHttpHeaders(const domain::http::entities::HttpRequest& httpRequest);
  void addRemoteInfo(const std::string& remoteAddr = "127.0.0.1",
                     unsigned int remotePort = 0);

  static std::string buildQueryStringFromParameters(
      const std::map<std::string, std::string>& parameters);
  static std::string httpHeaderToCgiName(const std::string& headerName);
  static std::string extractPathInfo(const std::string& requestPath,
                                      const std::string& scriptName);
  static std::string extractQueryString(const std::string& fullPath);
  static bool isValidEnvironmentName(const std::string& name);
  static std::string sanitizeEnvironmentValue(const std::string& value);
};

}  // namespace primitives
}  // namespace cgi
}  // namespace infrastructure

#endif  // CGI_REQUEST_HPP
