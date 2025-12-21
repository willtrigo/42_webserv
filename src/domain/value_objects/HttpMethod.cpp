/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpMethod.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/20 23:56:43 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/20 23:59:40 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "domain/value_objects/HttpMethod.hpp"
#include "shared/exceptions/HttpMethodException.hpp"

#include <algorithm>
#include <cctype>

namespace domain {
namespace value_objects {

const char* HttpMethod::METHOD_STRINGS[] = {
    "GET",     "POST",  "PUT",     "DELETE", "HEAD",
    "OPTIONS", "TRACE", "CONNECT", "PATCH",  "UNKNOWN"};

HttpMethod::HttpMethod() : m_method(METHOD_GET) {}

HttpMethod::HttpMethod(Method method) : m_method(method) { validate(); }

HttpMethod::HttpMethod(const std::string& methodString) {
  m_method = parseMethodString(methodString);
  validate();
}

HttpMethod::HttpMethod(const HttpMethod& other) : m_method(other.m_method) {}

HttpMethod::~HttpMethod() {}

HttpMethod& HttpMethod::operator=(const HttpMethod& other) {
  if (this != &other) {
    m_method = other.m_method;
  }
  return *this;
}

HttpMethod::Method HttpMethod::getMethod() const { return m_method; }

std::string HttpMethod::toString() const {
  if (m_method >= METHOD_GET && m_method < METHOD_UNKNOWN) {
    return METHOD_STRINGS[m_method];
  }
  return "UNKNOWN";
}

std::string HttpMethod::toUpperCaseString() const { return toString(); }

bool HttpMethod::isValidMethodString(const std::string& methodString) {
  try {
    parseMethodString(methodString);
    return true;
  } catch (const shared::exceptions::HttpMethodException&) {
    return false;
  }
}

bool HttpMethod::isSafeMethod(Method method) {
  return method == METHOD_GET || method == METHOD_HEAD ||
         method == METHOD_OPTIONS || method == METHOD_TRACE;
}

bool HttpMethod::isIdempotentMethod(Method method) {
  return method == METHOD_GET || method == METHOD_PUT ||
         method == METHOD_DELETE || method == METHOD_HEAD ||
         method == METHOD_OPTIONS || method == METHOD_TRACE;
}

bool HttpMethod::isCacheableMethod(Method method) {
  return method == METHOD_GET || method == METHOD_HEAD || method == METHOD_POST;
}

void HttpMethod::validate() const {
  if (m_method == METHOD_UNKNOWN) {
    throw shared::exceptions::HttpMethodException(
        "HTTP method is unknown",
        shared::exceptions::HttpMethodException::UNKNOWN_METHOD);
  }
}

bool HttpMethod::isGet() const { return m_method == METHOD_GET; }

bool HttpMethod::isPost() const { return m_method == METHOD_POST; }

bool HttpMethod::isPut() const { return m_method == METHOD_PUT; }

bool HttpMethod::isDelete() const { return m_method == METHOD_DELETE; }

bool HttpMethod::isHead() const { return m_method == METHOD_HEAD; }

bool HttpMethod::isOptions() const { return m_method == METHOD_OPTIONS; }

bool HttpMethod::isTrace() const { return m_method == METHOD_TRACE; }

bool HttpMethod::isConnect() const { return m_method == METHOD_CONNECT; }

bool HttpMethod::isPatch() const { return m_method == METHOD_PATCH; }

bool HttpMethod::isSafe() const { return isSafeMethod(m_method); }

bool HttpMethod::isIdempotent() const { return isIdempotentMethod(m_method); }

bool HttpMethod::isCacheable() const { return isCacheableMethod(m_method); }

bool HttpMethod::hasRequestBody() const {
  return m_method == METHOD_POST || m_method == METHOD_PUT ||
         m_method == METHOD_PATCH;
}

bool HttpMethod::hasResponseBody() const { return m_method != METHOD_HEAD; }

bool HttpMethod::operator==(const HttpMethod& other) const {
  return m_method == other.m_method;
}

bool HttpMethod::operator!=(const HttpMethod& other) const {
  return m_method != other.m_method;
}

bool HttpMethod::operator<(const HttpMethod& other) const {
  return m_method < other.m_method;
}

HttpMethod HttpMethod::get() { return HttpMethod(METHOD_GET); }

HttpMethod HttpMethod::post() { return HttpMethod(METHOD_POST); }

HttpMethod HttpMethod::put() { return HttpMethod(METHOD_PUT); }

HttpMethod HttpMethod::deleteMethod() { return HttpMethod(METHOD_DELETE); }

HttpMethod HttpMethod::head() { return HttpMethod(METHOD_HEAD); }

HttpMethod HttpMethod::options() { return HttpMethod(METHOD_OPTIONS); }

HttpMethod HttpMethod::trace() { return HttpMethod(METHOD_TRACE); }

HttpMethod HttpMethod::connect() { return HttpMethod(METHOD_CONNECT); }

HttpMethod HttpMethod::patch() { return HttpMethod(METHOD_PATCH); }

HttpMethod HttpMethod::fromString(const std::string& methodString) {
  return HttpMethod(methodString);
}

HttpMethod::Method HttpMethod::parseMethodString(
    const std::string& methodString) {
  if (methodString.empty()) {
    throw shared::exceptions::HttpMethodException(
        "HTTP method string cannot be empty",
        shared::exceptions::HttpMethodException::EMPTY_STRING);
  }

  if (!isValidMethodFormat(methodString)) {
    throw shared::exceptions::HttpMethodException(
        "Invalid HTTP method format: '" + methodString + "'",
        shared::exceptions::HttpMethodException::INVALID_METHOD);
  }

  return stringToMethod(methodString);
}

std::string HttpMethod::toUpperCase(const std::string& input) {
  std::string result = input;
  std::transform(result.begin(), result.end(), result.begin(), ::toupper);
  return result;
}

bool HttpMethod::isValidMethodFormat(const std::string& methodString) {
  for (std::size_t index = 0; index < methodString.size(); ++index) {
    char character = methodString[index];
    if (std::isalpha(static_cast<unsigned char>(character)) == 0) {
      return false;
    }
  }
  return !methodString.empty();
}

HttpMethod::Method HttpMethod::stringToMethod(const std::string& methodString) {
  std::string upperMethod = toUpperCase(methodString);

  if (upperMethod == "GET") return METHOD_GET;
  if (upperMethod == "POST") return METHOD_POST;
  if (upperMethod == "PUT") return METHOD_PUT;
  if (upperMethod == "DELETE") return METHOD_DELETE;
  if (upperMethod == "HEAD") return METHOD_HEAD;
  if (upperMethod == "OPTIONS") return METHOD_OPTIONS;
  if (upperMethod == "TRACE") return METHOD_TRACE;
  if (upperMethod == "CONNECT") return METHOD_CONNECT;
  if (upperMethod == "PATCH") return METHOD_PATCH;

  throw shared::exceptions::HttpMethodException(
      "Unknown HTTP method: '" + methodString + "'",
      shared::exceptions::HttpMethodException::UNKNOWN_METHOD);
}

}  // namespace value_objects
}  // namespace domain
