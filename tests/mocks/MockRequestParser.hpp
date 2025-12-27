/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MockRequestParser.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: umeneses <umeneses@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/27 13:03:45 by umeneses          #+#    #+#             */
/*   Updated: 2025/12/27 14:05:10 by umeneses         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MOCK_REQUEST_PARSER_HPP
#define MOCK_REQUEST_PARSER_HPP

#include <map>
#include <string>
#include <vector>

namespace mocks {

class MockRequestParser {
 public:
  MockRequestParser();
  ~MockRequestParser();

  // Parsing Control
  bool parse(const std::string& rawRequest);
  bool parse(const char* data, std::size_t length);
  void reset();

  // State Query
  bool isComplete() const;
  bool hasError() const;
  bool isValid() const;

  // Request Line - Method, URI, HTTP Version
  void setMethod(const std::string& method);
  std::string getMethod() const;
  void setUri(const std::string& uri);
  std::string getUri() const;
  void setHttpVersion(const std::string& version);
  std::string getHttpVersion() const;

  // Headers - HTTP header management
  void addHeader(const std::string& key, const std::string& value);
  std::string getHeader(const std::string& key) const;
  bool hasHeader(const std::string& key) const;
  std::map<std::string, std::string> getAllHeaders() const;
  void clearHeaders();

  // Body - Request body data
  void setBody(const std::string& body);
  std::string getBody() const;
  std::size_t getBodySize() const;
  bool hasBody() const;

  // Error Simulation
  void setParseError(bool hasError);
  void setComplete(bool complete);

  // Test Helpers - Convenient setups
  void setupGetRequest(const std::string& uri);
  void setupPostRequest(const std::string& uri, const std::string& body);
  void setupDeleteRequest(const std::string& uri);

 private:
  std::string m_method;
  std::string m_uri;
  std::string m_httpVersion;
  std::map<std::string, std::string> m_headers;
  std::string m_body;
  std::string m_rawData;
  bool m_isComplete;
  bool m_hasError;
  std::size_t m_bodySize;
};

}  // namespace mocks

#endif  // MOCK_REQUEST_PARSER_HPP

