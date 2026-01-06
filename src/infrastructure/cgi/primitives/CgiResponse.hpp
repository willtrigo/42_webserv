/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiResponse.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/05 17:37:17 by dande-je          #+#    #+#             */
/*   Updated: 2026/01/05 21:40:29 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGI_RESPONSE_HPP
#define CGI_RESPONSE_HPP

#include "domain/http/entities/HttpResponse.hpp"
#include "domain/shared/value_objects/ErrorCode.hpp"

#include <map>
#include <string>
#include <vector>

namespace infrastructure {
namespace cgi {
namespace primitives {

class CgiResponse {
 public:
  CgiResponse();
  explicit CgiResponse(const std::vector<char>& rawOutput);

  CgiResponse(const CgiResponse& other);
  ~CgiResponse();

  CgiResponse& operator=(const CgiResponse& other);

  const std::map<std::string, std::string>& getHeaders() const;
  const std::vector<char>& getBody() const;
  bool hasStatus() const;
  domain::shared::value_objects::ErrorCode getStatus() const;
  bool hasLocation() const;
  const std::string& getLocation() const;
  const std::string& getContentType() const;

  void parse(const std::vector<char>& rawOutput);
  void clear();

  bool isValid() const;
  void validate() const;

  domain::http::entities::HttpResponse toHttpResponse() const;

  static CgiResponse fromRawOutput(const std::vector<char>& rawOutput);

 private:
  std::map<std::string, std::string> m_headers;
  std::vector<char> m_body;
  bool m_hasStatus;
  domain::shared::value_objects::ErrorCode m_status;
  bool m_hasLocation;
  std::string m_location;
  std::string m_contentType;

  bool m_parsed;

  void parseHeaders(const std::string& headerSection);
  void parseBody(const std::vector<char>& rawOutput, std::size_t bodyStart);
  void extractSpecialHeaders();

  static std::size_t findHeaderBodySeparator(const std::vector<char>& data);
  static std::string normalizeHeaderName(const std::string& name);
  static std::string trimWhitespace(const std::string& str);
  static bool isValidHeaderName(const std::string& name);
  static unsigned int parseStatusCode(const std::string& statusLine);
};

}  // namespace primitives
}  // namespace cgi
}  // namespace infrastructure

#endif  // CGI_RESPONSE_HPP
