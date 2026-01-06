/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpHeader.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/04 13:54:43 by dande-je          #+#    #+#             */
/*   Updated: 2026/01/04 13:55:00 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPHEADER_HPP
#define HTTPHEADER_HPP

#include <string>

namespace domain {
namespace http {
namespace value_objects {

class HttpHeader {
 public:
  static const std::size_t MAX_HEADER_NAME_LENGTH = 256;
  static const std::size_t MAX_HEADER_VALUE_LENGTH = 8192;

  static const std::string CONTENT_LENGTH;
  static const std::string CONTENT_TYPE;
  static const std::string HOST;
  static const std::string CONNECTION;
  static const std::string TRANSFER_ENCODING;
  static const std::string ACCEPT;
  static const std::string USER_AGENT;
  static const std::string SERVER;
  static const std::string DATE;
  static const std::string LOCATION;
  static const std::string CACHE_CONTROL;
  static const std::string SET_COOKIE;
  static const std::string COOKIE;

  static const std::string CONNECTION_CLOSE;
  static const std::string CONNECTION_KEEP_ALIVE;
  static const std::string TRANSFER_ENCODING_CHUNKED;

  HttpHeader();
  HttpHeader(const std::string& name, const std::string& value);
  HttpHeader(const HttpHeader& other);
  ~HttpHeader();

  HttpHeader& operator=(const HttpHeader& other);

  std::string getName() const;
  std::string getValue() const;
  std::string getNormalizedName() const;
  std::string toString() const;

  static bool isValidHeaderName(const std::string& name);
  static bool isValidHeaderValue(const std::string& value);
  static bool isValidHeaderLine(const std::string& line);

  void setValue(const std::string& value);

  bool operator==(const HttpHeader& other) const;
  bool operator!=(const HttpHeader& other) const;
  bool operator<(const HttpHeader& other) const;

  static HttpHeader fromString(const std::string& headerLine);
  static HttpHeader contentLength(std::size_t length);
  static HttpHeader contentType(const std::string& type);
  static HttpHeader host(const std::string& hostname);
  static HttpHeader connection(const std::string& value);
  static HttpHeader transferEncoding(const std::string& encoding);

  static std::string normalizeName(const std::string& name);
  static std::string trimWhitespace(const std::string& str);

 private:
  std::string m_name;
  std::string m_value;
  std::string m_normalizedName;

  void validate() const;
  void updateNormalizedName();

  static bool isValidNameCharacter(char chr);
  static bool isValidValueCharacter(char chr);
  static bool isWhitespace(char chr);
  static char toLowerCase(char chr);
};

}  // namespace value_objects
}  // namespace http
}  // namespace domain

#endif  // HTTPHEADER_HPP
