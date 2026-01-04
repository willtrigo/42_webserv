/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParser.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/24 20:50:27 by dande-je          #+#    #+#             */
/*   Updated: 2026/01/04 01:05:22 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_PARSER_HPP
#define REQUEST_PARSER_HPP

#include "domain/filesystem/value_objects/Path.hpp"
#include "domain/http/value_objects/HttpMethod.hpp"
#include "domain/http/value_objects/QueryStringBuilder.hpp"

#include <map>
#include <string>
#include <vector>

namespace infrastructure {
namespace http {

struct ParsedRequest {
  domain::http::value_objects::HttpMethod method;
  domain::filesystem::value_objects::Path path;
  domain::http::value_objects::QueryStringBuilder query;
  std::string httpVersion;
  std::map<std::string, std::string> headers;
  std::vector<char> body;

  ParsedRequest();

  bool isComplete() const;
  bool hasError() const;

  std::string getHeader(const std::string& name) const;
  bool hasHeader(const std::string& name) const;
  std::size_t getContentLength() const;
  bool isChunked() const;
};

class RequestParser {
 public:
  RequestParser();
  explicit RequestParser(std::size_t maxHeaderSize, std::size_t maxBodySize);

  // Parse incoming data
  bool parse(const char* data, std::size_t length);
  bool parse(const std::vector<char>& data);

  // Get parsed request
  const ParsedRequest& getRequest() const;

  // State
  bool isComplete() const;
  bool hasError() const;
  void reset();

  // Configuration
  void setMaxHeaderSize(std::size_t size);
  void setMaxBodySize(std::size_t size);
  std::size_t getMaxHeaderSize() const;
  std::size_t getMaxBodySize() const;

 private:
  ParsedRequest m_request;
  std::string m_buffer;
  std::size_t m_maxHeaderSize;
  std::size_t m_maxBodySize;

  // Parsing state
  enum ParseState { START_LINE, HEADERS, BODY, CHUNKED_BODY, COMPLETE, ERROR };

  ParseState m_state;
  std::size_t m_bodyBytesRead;

  // Parsing methods
  bool parseStartLine();
  bool parseHeaders();
  bool parseBody();
  bool parseChunkedBody();

  // Helper methods
  std::size_t findLineEnd() const;
  std::string extractLine();
  void processStartLine(const std::string& line);
  void processHeaderLine(const std::string& line);

  // Validation
  bool validateMethod(const std::string& method) const;
  bool validatePath(const std::string& path) const;
  bool validateHttpVersion(const std::string& version) const;
};

}  // namespace http
}  // namespace infrastructure

#endif  // REQUEST_PARSER_HPP
