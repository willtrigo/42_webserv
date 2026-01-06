/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/04 15:10:57 by dande-je          #+#    #+#             */
/*   Updated: 2026/01/04 15:11:34 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPRESPONSE_HPP
#define HTTPRESPONSE_HPP

#include "domain/http/value_objects/HttpVersion.hpp"
#include "domain/shared/value_objects/ErrorCode.hpp"

#include <map>
#include <string>
#include <vector>

namespace domain {
namespace http {
namespace entities {

class HttpResponse {
 public:
  typedef std::map<std::string, std::string> HeaderMap;
  typedef std::vector<char> Body;

  static const std::size_t DEFAULT_MAX_BODY_SIZE = 10485760;
  static const std::string CRLF;
  static const std::string SERVER_NAME;
  static const std::string DEFAULT_CONTENT_TYPE;

  HttpResponse();
  explicit HttpResponse(const shared::value_objects::ErrorCode& statusCode);
  HttpResponse(const shared::value_objects::ErrorCode& statusCode,
               const std::string& body);
  HttpResponse(const HttpResponse& other);
  ~HttpResponse();

  HttpResponse& operator=(const HttpResponse& other);

  shared::value_objects::ErrorCode getStatusCode() const;
  value_objects::HttpVersion getVersion() const;
  const HeaderMap& getHeaders() const;
  const Body& getBody() const;
  std::string getBodyAsString() const;

  void setStatusCode(const shared::value_objects::ErrorCode& statusCode);
  void setVersion(const value_objects::HttpVersion& version);
  void setBody(const std::string& body);
  void setBody(const Body& body);
  void appendBody(const std::string& data);
  void appendBody(const Body& data);
  void clearBody();

  std::string getHeader(const std::string& name) const;
  bool hasHeader(const std::string& name) const;
  void setHeader(const std::string& name, const std::string& value);
  void addHeader(const std::string& name, const std::string& value);
  void removeHeader(const std::string& name);
  void clearHeaders();

  void setContentType(const std::string& contentType);
  void setContentLength(std::size_t length);
  void setConnection(const std::string& connection);
  void setServer(const std::string& serverName);
  void setDate();
  void setLocation(const std::string& location);

  std::string getContentType() const;
  std::size_t getContentLength() const;
  std::string getConnection() const;

  std::string serialize() const;
  std::string buildStatusLine() const;
  std::string buildHeaders() const;

  bool isValid() const;
  void validate() const;

  bool operator==(const HttpResponse& other) const;
  bool operator!=(const HttpResponse& other) const;

  static HttpResponse ok(const std::string& body = "");
  static HttpResponse created(const std::string& location = "");
  static HttpResponse noContent();
  static HttpResponse movedPermanently(const std::string& location);
  static HttpResponse found(const std::string& location);
  static HttpResponse notModified();
  static HttpResponse badRequest(const std::string& message = "");
  static HttpResponse unauthorized(const std::string& message = "");
  static HttpResponse forbidden(const std::string& message = "");
  static HttpResponse notFound(const std::string& message = "");
  static HttpResponse methodNotAllowed(const std::string& message = "");
  static HttpResponse payloadTooLarge(const std::string& message = "");
  static HttpResponse internalServerError(const std::string& message = "");
  static HttpResponse notImplemented(const std::string& message = "");
  static HttpResponse serviceUnavailable(const std::string& message = "");

 private:
  shared::value_objects::ErrorCode m_statusCode;
  value_objects::HttpVersion m_version;
  HeaderMap m_headers;
  Body m_body;

  static std::string normalizeHeaderName(const std::string& name);
  static char toLowerCase(char chr);
  static std::string getCurrentDateTime();

  void updateContentLength();
  void ensureDefaultHeaders();
  std::string buildDefaultErrorBody(const std::string& message) const;
};

}  // namespace entities
}  // namespace http
}  // namespace domain

#endif  // HTTPRESPONSE_HPP
