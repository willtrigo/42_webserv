/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpVersion.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/04 13:43:29 by dande-je          #+#    #+#             */
/*   Updated: 2026/01/04 13:53:24 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPVERSION_HPP
#define HTTPVERSION_HPP

#include <string>

namespace domain {
namespace http {
namespace value_objects {

class HttpVersion {
 public:
  static const unsigned int MIN_MAJOR_VERSION = 0;
  static const unsigned int MAX_MAJOR_VERSION = 2;
  static const unsigned int MIN_MINOR_VERSION = 0;
  static const unsigned int MAX_MINOR_VERSION = 9;

  static const unsigned int HTTP_1_0_MAJOR = 1;
  static const unsigned int HTTP_1_0_MINOR = 0;
  static const unsigned int HTTP_1_1_MAJOR = 1;
  static const unsigned int HTTP_1_1_MINOR = 1;
  static const unsigned int HTTP_2_0_MAJOR = 2;
  static const unsigned int HTTP_2_0_MINOR = 0;

  static const std::size_t VERSION_STRING_PREFIX_LENGTH = 5;
  static const std::size_t MIN_VERSION_STRING_LENGTH = 8;
  static const std::size_t MAX_VERSION_STRING_LENGTH = 10;

  HttpVersion();
  HttpVersion(unsigned int major, unsigned int minor);
  explicit HttpVersion(const std::string& versionString);
  HttpVersion(const HttpVersion& other);
  ~HttpVersion();

  HttpVersion& operator=(const HttpVersion& other);

  unsigned int getMajor() const;
  unsigned int getMinor() const;
  std::string toString() const;

  static bool isValidVersion(unsigned int major, unsigned int minor);
  static bool isValidVersionString(const std::string& versionString);

  bool isHttp10() const;
  bool isHttp11() const;
  bool isHttp20() const;
  bool supportsKeepAlive() const;
  bool supportsPipelining() const;
  bool supportsChunkedEncoding() const;

  bool operator==(const HttpVersion& other) const;
  bool operator!=(const HttpVersion& other) const;
  bool operator<(const HttpVersion& other) const;
  bool operator<=(const HttpVersion& other) const;
  bool operator>(const HttpVersion& other) const;
  bool operator>=(const HttpVersion& other) const;

  static HttpVersion http10();
  static HttpVersion http11();
  static HttpVersion http20();
  static HttpVersion fromString(const std::string& versionString);

 private:
  unsigned int m_major;
  unsigned int m_minor;

  void validate() const;
  static void parseVersionString(const std::string& versionString,
                                 unsigned int& major, unsigned int& minor);
  static bool isDigit(char chr);
};

}  // namespace value_objects
}  // namespace http
}  // namespace domain

#endif  // HTTPVERSION_HPP
