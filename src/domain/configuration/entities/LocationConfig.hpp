/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationConfig.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/22 12:21:54 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/28 01:32:33 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOCATION_CONFIG_HPP
#define LOCATION_CONFIG_HPP

#include "domain/configuration/value_objects/CgiConfig.hpp"
#include "domain/configuration/value_objects/Route.hpp"
#include "domain/configuration/value_objects/UploadConfig.hpp"
#include "domain/shared/value_objects/ErrorCode.hpp"
#include "domain/http/value_objects/HttpMethod.hpp"
#include "domain/filesystem/value_objects/Path.hpp"
#include "domain/shared/value_objects/RegexPattern.hpp"
#include "domain/filesystem/value_objects/Size.hpp"
#include "domain/http/value_objects/Uri.hpp"

#include <map>
#include <set>
#include <string>
#include <vector>

namespace domain {
namespace configuration {
namespace entities {

class LocationConfig {
 public:
  typedef std::set<http::value_objects::HttpMethod> AllowedMethods;
  typedef std::vector<std::string> TryFiles;
  typedef std::map<shared::value_objects::ErrorCode, std::string> ErrorPageMap;

  enum LocationMatchType {
    MATCH_EXACT,
    MATCH_PREFIX,
    MATCH_REGEX_CASE_SENSITIVE,
    MATCH_REGEX_CASE_INSENSITIVE
  };

  LocationConfig();
  explicit LocationConfig(const std::string& path,
                          LocationMatchType matchType = MATCH_PREFIX);

  LocationConfig(const LocationConfig& other);
  ~LocationConfig();

  LocationConfig& operator=(const LocationConfig& other);

  const std::string& getPath() const;
  LocationMatchType getMatchType() const;
  const filesystem::value_objects::Path& getRoot() const;
  const std::vector<std::string>& getIndexFiles() const;
  const AllowedMethods& getAllowedMethods() const;
  bool getAutoIndex() const;
  const TryFiles& getTryFiles() const;
  bool hasReturnRedirect() const;
  const http::value_objects::Uri& getReturnRedirect() const;
  const shared::value_objects::ErrorCode& getReturnCode() const;
  bool hasUploadConfig() const;
  const UploadConfig& getUploadConfig() const;
  UploadConfig& getUploadConfigMutable();
  const value_objects::CgiConfig& getCgiConfig() const;
  const ErrorPageMap& getErrorPages() const;
  const filesystem::value_objects::Size& getClientMaxBodySize() const;
  bool hasProxyPass() const;
  const http::value_objects::Uri& getProxyPass() const;
  const filesystem::value_objects::Path& getAlias() const;
  bool getClientBodyBufferSizeSet() const;
  const filesystem::value_objects::Size& getClientBodyBufferSize() const;

  void setPath(const std::string& path, LocationMatchType matchType);
  void setRoot(const filesystem::value_objects::Path& root);
  void setRoot(const std::string& root);
  void addIndexFile(const std::string& index);
  void clearIndexFiles();
  void addAllowedMethod(const http::value_objects::HttpMethod& method);
  void removeAllowedMethod(const http::value_objects::HttpMethod& method);
  void setAutoIndex(bool autoIndex);
  void setTryFiles(const TryFiles& tryFiles);
  void addTryFile(const std::string& tryFile);
  void setReturnRedirect(const http::value_objects::Uri& redirect,
                         const shared::value_objects::ErrorCode& code);
  void setReturnRedirect(const std::string& redirect, unsigned int code);
  void setUploadConfig(const UploadConfig& config);
  void enableUpload(const filesystem::value_objects::Path& uploadDirectory);
  void enableUpload(const filesystem::value_objects::Path& uploadDirectory,
                    const filesystem::value_objects::Size& maxFileSize,
                    const filesystem::value_objects::Size& maxTotalSize);
  void disableUpload();
  void setCgiConfig(const value_objects::CgiConfig& config);
  void clearCgiConfig();
  void addErrorPage(const shared::value_objects::ErrorCode& code,
                    const std::string& uri);
  void removeErrorPage(const shared::value_objects::ErrorCode& code);
  void setClientMaxBodySize(const filesystem::value_objects::Size& size);
  void setClientMaxBodySize(const std::string& sizeString);
  void setProxyPass(const http::value_objects::Uri& proxyPass);
  void setProxyPass(const std::string& proxyPass);
  void clearProxyPass();
  void setAlias(const filesystem::value_objects::Path& alias);
  void setAlias(const std::string& alias);
  void setClientBodyBufferSize(const filesystem::value_objects::Size& size);
  void setClientBodyBufferSize(const std::string& sizeString);

  bool isValid() const;
  void validate() const;

  bool matchesPath(const std::string& requestPath) const;
  bool isMethodAllowed(const http::value_objects::HttpMethod& method) const;
  bool isUploadEnabled() const;
  bool hasCgiConfig() const;
  bool hasAlias() const;
  bool hasClientBodyBufferSize() const;

  bool allowsGet() const;
  bool allowsPost() const;
  bool allowsDelete() const;
  bool allowsPut() const;
  bool allowsHead() const;
  bool isUploadRoute() const;

  filesystem::value_objects::Path resolvePath(const std::string& requestPath) const;

  bool validateUploadFile(const std::string& filename,
                          const filesystem::value_objects::Size& fileSize,
                          const std::string& mimeType = "") const;
  bool canHandleUpload() const;

  value_objects::Route toRoute() const;

  void clear();

 private:
  std::string m_path;
  LocationMatchType m_matchType;
  filesystem::value_objects::Path m_root;
  std::vector<std::string> m_indexFiles;
  AllowedMethods m_allowedMethods;
  bool m_autoIndex;
  TryFiles m_tryFiles;
  http::value_objects::Uri m_returnRedirect;
  shared::value_objects::ErrorCode m_returnCode;
  UploadConfig m_uploadConfig;
  bool m_hasUploadConfig;
  value_objects::CgiConfig m_cgiConfig;
  ErrorPageMap m_errorPages;
  filesystem::value_objects::Size m_clientMaxBodySize;
  http::value_objects::Uri m_proxyPass;
  filesystem::value_objects::Path m_alias;
  filesystem::value_objects::Size m_clientBodyBufferSize;
  bool m_clientBodyBufferSizeSet;

  mutable shared::value_objects::RegexPattern m_regexPattern;
  mutable bool m_regexPatternValid;

  static const size_t DEFAULT_CLIENT_BODY_BUFFER_SIZE = 8;
  static const size_t DEFAULT_CLIENT_MAX_BODY_SIZE = 1;
  static const size_t MAX_ALLOWED_CLIENT_BODY_SIZE = 100;
  static const size_t MAX_ALLOWED_BUFFER_SIZE = 10;

  void validatePath() const;
  void validateRoot() const;
  void validateAllowedMethods() const;
  void validateTryFiles() const;
  void validateReturnRedirect() const;
  void validateUploadConfig() const;
  void validateCgiConfig() const;
  void validateErrorPages() const;
  void validateClientMaxBodySize() const;
  void validateProxyPass() const;
  void validateAlias() const;
  void validateClientBodyBufferSize() const;

  void compileRegexPattern() const;
  bool shouldUseAlias() const;

  static LocationMatchType parseMatchType(const std::string& path);
  static std::string stripMatchPrefix(const std::string& path);
};

}  // namespace entities
}  // namespace configuration
}  // namespace domain

#endif  // LOCATION_CONFIG_HPP
