/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiConfig.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/22 13:11:08 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/22 13:12:08 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGI_CONFIG_HPP
#define CGI_CONFIG_HPP

#include "domain/value_objects/Path.hpp"
#include "domain/value_objects/RegexPattern.hpp"

#include <map>
#include <string>

namespace domain {
namespace entities {

class CgiConfig {
 public:
  typedef std::map<std::string, std::string> ParameterMap;

  static const std::string DEFAULT_QUERY_STRING;
  static const std::string DEFAULT_REQUEST_METHOD;
  static const std::string DEFAULT_CONTENT_TYPE;
  static const std::string DEFAULT_CONTENT_LENGTH;
  static const std::string DEFAULT_SCRIPT_FILENAME;
  static const std::string DEFAULT_SCRIPT_NAME;
  static const std::string DEFAULT_DOCUMENT_ROOT;
  static const std::string DEFAULT_SERVER_SOFTWARE;
  static const std::string DEFAULT_SERVER_NAME;
  static const std::string DEFAULT_SERVER_PORT;
  static const std::string DEFAULT_SERVER_PROTOCOL;
  static const std::string DEFAULT_GATEWAY_INTERFACE;
  static const std::string DEFAULT_REMOTE_ADDR;
  static const std::string DEFAULT_REMOTE_PORT;
  static const std::string DEFAULT_REQUEST_URI;

  CgiConfig();

  CgiConfig(const std::string& scriptPath, const value_objects::Path& cgiRoot,
            const value_objects::RegexPattern& extensionPattern);

  CgiConfig(const CgiConfig& other);

  ~CgiConfig();

  CgiConfig& operator=(const CgiConfig& other);

  const std::string& getScriptPath() const;
  const value_objects::Path& getCgiRoot() const;
  const value_objects::RegexPattern& getExtensionPattern() const;
  const ParameterMap& getParameters() const;
  std::string getParameter(const std::string& name) const;
  bool hasParameter(const std::string& name) const;

  void setScriptPath(const std::string& scriptPath);
  void setCgiRoot(const value_objects::Path& cgiRoot);
  void setExtensionPattern(const value_objects::RegexPattern& pattern);
  void addParameter(const std::string& name, const std::string& value);
  void removeParameter(const std::string& name);
  void setParameters(const ParameterMap& parameters);

  static CgiConfig createPhpCgi(
      const std::string& phpBinary = "/usr/bin/php-cgi");
  static CgiConfig createPythonCgi(
      const std::string& pythonBinary = "/usr/bin/python3");
  static CgiConfig createPerlCgi(
      const std::string& perlBinary = "/usr/bin/perl");

  bool isValid() const;
  void validate() const;

  bool matchesExtension(const std::string& filename) const;
  std::string resolveScriptPath(const std::string& requestPath) const;
  ParameterMap buildEnvironment(
      const std::map<std::string, std::string>& requestInfo) const;

  bool operator==(const CgiConfig& other) const;
  bool operator!=(const CgiConfig& other) const;

  void clear();

 private:
  std::string m_scriptPath;
  value_objects::Path m_cgiRoot;
  value_objects::RegexPattern m_extensionPattern;
  ParameterMap m_parameters;

  void copyFrom(const CgiConfig& other);
  void validateScriptPath() const;
  void validateCgiRoot() const;
  void validateExtensionPattern() const;
  void validateParameters() const;

  void initializeDefaultParameters();
  static bool isValidParameterName(const std::string& name);
  static bool isValidParameterValue(const std::string& value);

  static std::string normalizeScriptPath(const std::string& scriptPath);
  static bool isAbsolutePath(const std::string& path);
};

}  // namespace entities
}  // namespace domain

#endif  // CGI_CONFIG_HPP
