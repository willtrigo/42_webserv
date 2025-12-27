/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MockResponseBuilder.hpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: umeneses <umeneses@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/27 13:02:14 by umeneses          #+#    #+#             */
/*   Updated: 2025/12/27 14:05:21 by umeneses         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MOCK_RESPONSE_BUILDER_HPP
#define MOCK_RESPONSE_BUILDER_HPP

#include <map>
#include <string>

namespace mocks {

class MockResponseBuilder {
 public:
  MockResponseBuilder();
  ~MockResponseBuilder();

  // Status Line
  void setStatus(int code, const std::string& message);
  void setStatus(int code);
  int getStatusCode() const;
  std::string getStatusMessage() const;
  void setHttpVersion(const std::string& version);
  std::string getHttpVersion() const;

  // Headers
  void addHeader(const std::string& key, const std::string& value);
  std::string getHeader(const std::string& key) const;
  bool hasHeader(const std::string& key) const;
  std::map<std::string, std::string> getAllHeaders() const;
  void removeHeader(const std::string& key);
  void clearHeaders();

  // Body
  void setBody(const std::string& body);
  void appendBody(const std::string& data);
  std::string getBody() const;
  std::size_t getBodySize() const;
  bool hasBody() const;
  void clearBody();

  // Response Building
  std::string build() const;
  std::string buildStatusLine() const;

  // Reset
  void reset();

  // Convenience Methods
  void buildOkResponse(const std::string& body, const std::string& contentType = "text/html");
  void buildNotFoundResponse(const std::string& message = "Not Found");
  void buildServerErrorResponse(const std::string& message = "Internal Server Error");
  void buildBadRequestResponse(const std::string& message = "Bad Request");

  // Test Helpers
  bool isSuccessResponse() const;
  bool isErrorResponse() const;
  bool isRedirectResponse() const;

 private:
  int m_statusCode;
  std::string m_statusMessage;
  std::string m_httpVersion;
  std::map<std::string, std::string> m_headers;
  std::string m_body;
  
  void updateContentLength();
  std::string getDefaultMessage(int code) const;
};

}  // namespace mocks

#endif  // MOCK_RESPONSE_BUILDER_HPP
