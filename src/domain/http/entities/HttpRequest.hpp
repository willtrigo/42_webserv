/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/04 02:03:12 by dande-je          #+#    #+#             */
/*   Updated: 2026/01/04 14:25:27 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPREQUEST_HPP
#define HTTPREQUEST_HPP

#include "domain/filesystem/value_objects/Path.hpp"
#include "domain/http/value_objects/HttpMethod.hpp"
#include "domain/http/value_objects/HttpVersion.hpp"
#include "domain/http/value_objects/QueryStringBuilder.hpp"
#include "domain/http/value_objects/Uri.hpp"

#include <map>
#include <string>
#include <vector>

namespace domain {
namespace http {
namespace entities {

class HttpRequest {
 public:
  typedef std::map<std::string, std::string> HeaderMap;
  typedef std::vector<char> Body;

  static const std::size_t DEFAULT_MAX_BODY_SIZE = 10485760;
  static const std::size_t MAX_URI_LENGTH = 8192;

  HttpRequest();
  HttpRequest(const HttpRequest& other);
  ~HttpRequest();

  HttpRequest& operator=(const HttpRequest& other);

  value_objects::HttpMethod getMethod() const;
  value_objects::Uri getUri() const;
  filesystem::value_objects::Path getPath() const;
  value_objects::QueryStringBuilder getQuery() const;
  value_objects::HttpVersion getVersion() const;
  const HeaderMap& getHeaders() const;
  const Body& getBody() const;

  void setMethod(const value_objects::HttpMethod& method);
  void setUri(const value_objects::Uri& uri);
  void setPath(const filesystem::value_objects::Path& path);
  void setQuery(const value_objects::QueryStringBuilder& query);
  void setVersion(const value_objects::HttpVersion& version);
  void setBody(const Body& body);

  std::string getHeader(const std::string& name) const;
  bool hasHeader(const std::string& name) const;
  void addHeader(const std::string& name, const std::string& value);
  void removeHeader(const std::string& name);
  void clearHeaders();

  std::size_t getContentLength() const;
  std::string getContentType() const;
  std::string getHost() const;
  std::string getConnection() const;
  std::string getTransferEncoding() const;

  bool isChunked() const;
  bool isKeepAlive() const;
  bool hasBody() const;
  bool isComplete() const;
  bool isValid() const;

  void validate() const;

  bool operator==(const HttpRequest& other) const;
  bool operator!=(const HttpRequest& other) const;

 private:
  value_objects::HttpMethod m_method;
  value_objects::Uri m_uri;
  filesystem::value_objects::Path m_path;
  value_objects::QueryStringBuilder m_query;
  value_objects::HttpVersion m_version;
  HeaderMap m_headers;
  Body m_body;

  static std::string normalizeHeaderName(const std::string& name);
  static bool isWhitespace(char chr);
  static char toLowerCase(char chr);

  void validateContentLength() const;
  void validateHostHeader() const;
  void validateTransferEncoding() const;
};

}  // namespace entities
}  // namespace http
}  // namespace domain

#endif  // HTTPREQUEST_HPP
