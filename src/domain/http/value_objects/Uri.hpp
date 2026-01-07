/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Uri.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: umeneses <umeneses@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/21 12:45:08 by dande-je          #+#    #+#             */
/*   Updated: 2026/01/06 19:22:31 by umeneses         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef URI_HPP
#define URI_HPP

#include "domain/http/value_objects/Port.hpp"

#include <string>
#include <vector>

namespace domain {
namespace http {
namespace value_objects {

class Uri {
 public:
  struct UriParameters {
    std::string m_scheme;
    std::string m_host;
    Port m_port;
    std::string m_path;
    std::string m_query;
    std::string m_fragment;

    UriParameters();
    UriParameters& withScheme(const std::string& scheme);
    UriParameters& withHost(const std::string& host);
    UriParameters& withPort(const Port& port);
    UriParameters& withPath(const std::string& path);
    UriParameters& withQuery(const std::string& query);
    UriParameters& withFragment(const std::string& fragment);
  };

  static const std::size_t MAX_URI_LENGTH = 4096;

  static const std::string HTTP_SCHEME;
  static const std::string HTTPS_SCHEME;
  static const std::string FTP_SCHEME;
  static const std::string FILE_SCHEME;
  static const std::string WS_SCHEME;
  static const std::string WSS_SCHEME;

  static const Port HTTP_DEFAULT_PORT;
  static const Port HTTPS_DEFAULT_PORT;
  static const Port FTP_DEFAULT_PORT;

  static const int BASE_HEX = 16;
  static const std::size_t HEX_DIGITS_PER_BYTE = 2;
  static const int MIN_BYTE_VALUE = 0;
  static const int MAX_BYTE_VALUE = 255;

  Uri();
  explicit Uri(const std::string& uriString);
  Uri(const UriParameters& param);
  Uri(const Uri& other);
  ~Uri();

  Uri& operator=(const Uri& other);

  std::string getScheme() const;
  std::string getHost() const;
  Port getPort() const;
  std::string getPath() const;
  std::string getQuery() const;
  std::string getFragment() const;
  std::string getAuthority() const;
  std::string toString() const;
  std::string toNormalizedString() const;

  static bool isValidUri(const std::string& uriString);
  static bool isAbsoluteUri(const std::string& uriString);
  static bool isRelativeUri(const std::string& uriString);

  bool isEmpty() const;
  bool isAbsolute() const;
  bool isRelative() const;
  bool hasScheme() const;
  bool hasHost() const;
  bool hasPort() const;
  bool hasPath() const;
  bool hasQuery() const;
  bool hasFragment() const;
  bool isDefaultPort() const;

  bool isHttp() const;
  bool isHttps() const;
  bool isWebSocket() const;
  bool isSecureWebSocket() const;
  bool isFile() const;

  bool operator==(const Uri& other) const;
  bool operator!=(const Uri& other) const;
  bool operator<(const Uri& other) const;

  Uri resolve(const Uri& baseUri) const;
  Uri normalize() const;
  Uri withScheme(const std::string& scheme) const;
  Uri withHost(const std::string& host) const;
  Uri withPort(const Port& port) const;
  Uri withPath(const std::string& path) const;
  Uri withQuery(const std::string& query) const;
  Uri withFragment(const std::string& fragment) const;

  static Uri fromString(const std::string& uriString);
  static Uri parseAuthority(const std::string& authority);
  static Uri createHttp(const std::string& host, const Port& port,
                        const std::string& path = "/");
  static Uri createHttps(const std::string& host, const Port& port,
                         const std::string& path = "/");
  static Uri createFile(const std::string& path);

  static std::string encodeComponent(const std::string& component);
  static std::string decodeComponent(const std::string& component);

  std::string getBasePath() const;
  std::string getFilename() const;
  std::string getExtension() const;
  Uri getParentUri() const;

  static std::string getQueryParameter(const std::string& queryString,
                                       const std::string& parameterName);

 private:
  std::string m_scheme;
  std::string m_host;
  Port m_port;
  std::string m_path;
  std::string m_query;
  std::string m_fragment;
  bool m_isAbsolute;
  bool m_hasExplicitPort;

  void parseUriString(const std::string& uriString);
  void validateUriStringFormat(const std::string& uriString);
  void parseSchemeAndAuthority(const std::string& uriString,
                               std::size_t& position);
  void parsePathQueryFragment(const std::string& uriString,
                              std::size_t& position);
  void validate() const;
  static void validateUriComponent(const std::string& component,
                                   const std::string& componentName);
  static bool containsVariablePlaceholders(const std::string& str);
  static bool isValidScheme(const std::string& scheme);
  static bool isValidHost(const std::string& host);
  static bool isValidIPv4(const std::string& host);
  static bool isValidIPv6(const std::string& host);
  static bool isValidPath(const std::string& path);
  static bool isValidQuery(const std::string& query);
  static bool isValidFragment(const std::string& fragment);

  static std::string normalizePath(const std::string& path);
  static void processPathSegments(std::istringstream& pathStream,
                                  std::vector<std::string>& pathSegments,
                                  bool isAbsolute);
  static void handlePathSegment(const std::string& segment,
                                std::vector<std::string>& pathSegments,
                                bool isAbsolute);
  static void handleParentDirectory(std::vector<std::string>& pathSegments,
                                    bool isAbsolute);
  static std::string buildNormalizedPath(
      const std::vector<std::string>& pathSegments, bool isAbsolute,
      const std::string& originalPath);
  static bool shouldAddTrailingSeparator(
      const std::string& originalPath,
      const std::vector<std::string>& pathSegments, bool isAbsolute);

  static std::string removeDefaultPort(const std::string& scheme,
                                       const Port& port);
  static Port getDefaultPortForScheme(const std::string& scheme);
  static bool isDefaultPortForScheme(const std::string& scheme,
                                     const Port& port);

  static std::size_t findSchemeSeparator(const std::string& uriString);
  static std::size_t findAuthorityStart(const std::string& uriString,
                                        std::size_t schemeEnd);
  static std::size_t findAuthorityEnd(const std::string& uriString,
                                      std::size_t authorityStart);
  static std::size_t findPathStart(const std::string& uriString,
                                   std::size_t authorityEnd);
  static std::size_t findQueryStart(const std::string& uriString,
                                    std::size_t pathStart);
  static std::size_t findFragmentStart(const std::string& uriString,
                                       std::size_t queryStart);

  static std::string parseScheme(const std::string& uriString,
                                 std::size_t& position);
  static std::string parseAuthorityComponent(const std::string& authorityString,
                                             std::string& host, Port& port,
                                             bool& hasExplicitPort,
                                             const std::string& scheme = "");
  static std::string parsePath(const std::string& uriString,
                               std::size_t& position);
  static std::string parseQuery(const std::string& uriString,
                                std::size_t& position);
  static std::string parseFragment(const std::string& uriString,
                                   std::size_t& position);
};

}  // namespace value_objects
}  // namespace http
}  // namespace domain

#endif  // URI_HPP
