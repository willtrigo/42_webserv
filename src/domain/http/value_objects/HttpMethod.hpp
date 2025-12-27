/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpMethod.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/20 23:55:05 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/27 03:25:26 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTP_METHOD_HPP
#define HTTP_METHOD_HPP

#include <string>

namespace domain {
namespace http {
namespace value_objects {

class HttpMethod {
 public:
  enum Method {
    METHOD_GET,
    METHOD_POST,
    METHOD_PUT,
    METHOD_DELETE,
    METHOD_HEAD,
    METHOD_OPTIONS,
    METHOD_TRACE,
    METHOD_CONNECT,
    METHOD_PATCH,
    METHOD_UNKNOWN
  };

  static const char* METHOD_STRINGS[];

  HttpMethod();
  explicit HttpMethod(Method method);
  explicit HttpMethod(const std::string& methodString);
  HttpMethod(const HttpMethod& other);
  ~HttpMethod();

  HttpMethod& operator=(const HttpMethod& other);

  Method getMethod() const;
  std::string toString() const;
  std::string toUpperCaseString() const;

  static bool isValidMethodString(const std::string& methodString);
  static bool isSafeMethod(Method method);
  static bool isIdempotentMethod(Method method);
  static bool isCacheableMethod(Method method);

  bool isGet() const;
  bool isPost() const;
  bool isPut() const;
  bool isDelete() const;
  bool isHead() const;
  bool isOptions() const;
  bool isTrace() const;
  bool isConnect() const;
  bool isPatch() const;

  bool isSafe() const;
  bool isIdempotent() const;
  bool isCacheable() const;
  bool hasRequestBody() const;
  bool hasResponseBody() const;

  bool operator==(const HttpMethod& other) const;
  bool operator!=(const HttpMethod& other) const;
  bool operator<(const HttpMethod& other) const;

  static HttpMethod get();
  static HttpMethod post();
  static HttpMethod put();
  static HttpMethod deleteMethod();
  static HttpMethod head();
  static HttpMethod options();
  static HttpMethod trace();
  static HttpMethod connect();
  static HttpMethod patch();

  static HttpMethod fromString(const std::string& methodString);

  static Method parseMethodString(const std::string& methodString);

 private:
  Method m_method;

  void validate() const;

  static std::string toUpperCase(const std::string& input);
  static bool isValidMethodFormat(const std::string& methodString);
  static Method stringToMethod(const std::string& methodString);
};

}  // namespace value_objects
}  // namespace http
}  // namespace domain

#endif  // HTTP_METHOD_HPP
