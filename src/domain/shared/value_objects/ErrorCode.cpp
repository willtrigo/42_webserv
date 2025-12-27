/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ErrorCode.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/20 11:20:04 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/27 01:55:13 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "domain/shared/exceptions/ErrorCodeException.hpp"
#include "domain/shared/value_objects/ErrorCode.hpp"

#include <cctype>
#include <cstdlib>
#include <sstream>

namespace domain {
namespace shared {
namespace value_objects {

ErrorCode::StatusCodeEntry ErrorCode::K_STATUS_CODE_MSGS[] = {
    std::make_pair(STATUS_BAD_REQUEST, "Bad Request"),
    std::make_pair(STATUS_UNAUTHORIZED, "Unauthorized"),
    std::make_pair(STATUS_FORBIDDEN, "Forbidden"),
    std::make_pair(STATUS_NOT_FOUND, "Not Found"),
    std::make_pair(STATUS_METHOD_NOT_ALLOWED, "Method Not Allowed"),
    std::make_pair(STATUS_REQUEST_TIMEOUT, "Request Timeout"),
    std::make_pair(STATUS_CONFLICT, "Conflict"),
    std::make_pair(STATUS_GONE, "Gone"),
    std::make_pair(STATUS_LENGTH_REQUIRED, "Length Required"),
    std::make_pair(STATUS_PAYLOAD_TOO_LARGE, "Payload Too Large"),
    std::make_pair(STATUS_URI_TOO_LONG, "URI Too Long"),
    std::make_pair(STATUS_UNSUPPORTED_MEDIA_TYPE, "Unsupported Media Type"),
    std::make_pair(STATUS_RANGE_NOT_SATISFIABLE, "Range Not Satisfiable"),
    std::make_pair(STATUS_EXPECTATION_FAILED, "Expectation Failed"),
    std::make_pair(STATUS_IM_A_TEAPOT, "I'm a teapot"),
    std::make_pair(STATUS_UPGRADE_REQUIRED, "Upgrade Required"),
    std::make_pair(STATUS_PRECONDITION_REQUIRED, "Precondition Required"),
    std::make_pair(STATUS_TOO_MANY_REQUESTS, "Too Many Requests"),
    std::make_pair(STATUS_REQUEST_HEADER_FIELDS_TOO_LARGE,
                   "Request Header Fields Too Large"),
    std::make_pair(STATUS_UNAVAILABLE_FOR_LEGAL_REASONS,
                   "Unavailable For Legal Reasons"),

    std::make_pair(STATUS_INTERNAL_SERVER_ERROR, "Internal Server Error"),
    std::make_pair(STATUS_NOT_IMPLEMENTED, "Not Implemented"),
    std::make_pair(STATUS_BAD_GATEWAY, "Bad Gateway"),
    std::make_pair(STATUS_SERVICE_UNAVAILABLE, "Service Unavailable"),
    std::make_pair(STATUS_GATEWAY_TIMEOUT, "Gateway Timeout"),
    std::make_pair(STATUS_HTTP_VERSION_NOT_SUPPORTED,
                   "HTTP Version Not Supported"),
    std::make_pair(STATUS_VARIANT_ALSO_NEGOTIATES, "Variant Also Negotiates"),
    std::make_pair(STATUS_INSUFFICIENT_STORAGE, "Insufficient Storage"),
    std::make_pair(STATUS_LOOP_DETECTED, "Loop Detected"),
    std::make_pair(STATUS_NOT_EXTENDED, "Not Extended"),
    std::make_pair(STATUS_NETWORK_AUTHENTICATION_REQUIRED,
                   "Network Authentication Required"),

    std::make_pair(STATUS_OK, "OK"),
    std::make_pair(STATUS_CREATED, "Created"),
    std::make_pair(STATUS_ACCEPTED, "Accepted"),
    std::make_pair(STATUS_NO_CONTENT, "No Content"),
    std::make_pair(STATUS_RESET_CONTENT, "Reset Content"),
    std::make_pair(STATUS_PARTIAL_CONTENT, "Partial Content"),

    std::make_pair(STATUS_MULTIPLE_CHOICES, "Multiple Choices"),
    std::make_pair(STATUS_MOVED_PERMANENTLY, "Moved Permanently"),
    std::make_pair(STATUS_FOUND, "Found"),
    std::make_pair(STATUS_SEE_OTHER, "See Other"),
    std::make_pair(STATUS_NOT_MODIFIED, "Not Modified"),
    std::make_pair(STATUS_TEMPORARY_REDIRECT, "Temporary Redirect"),
    std::make_pair(STATUS_PERMANENT_REDIRECT, "Permanent Redirect"),

    std::make_pair(STATUS_CONTINUE, "Continue"),
    std::make_pair(STATUS_SWITCHING_PROTOCOLS, "Switching Protocols"),
    std::make_pair(STATUS_PROCESSING, "Processing"),
    std::make_pair(STATUS_EARLY_HINTS, "Early Hints")};

ErrorCode::ErrorCode() : m_value(STATUS_OK) {}

ErrorCode::ErrorCode(unsigned int code) : m_value(code) { validate(); }

ErrorCode::ErrorCode(const std::string& codeString) : m_value(STATUS_OK) {
  m_value = parseCodeString(codeString);
  validate();
}

ErrorCode::ErrorCode(const ErrorCode& other) : m_value(other.m_value) {}

ErrorCode::~ErrorCode() {}

ErrorCode& ErrorCode::operator=(const ErrorCode& other) {
  if (this != &other) {
    m_value = other.m_value;
  }
  return *this;
}

unsigned int ErrorCode::getValue() const { return m_value; }

std::string ErrorCode::toString() const {
  std::ostringstream oss;
  oss << m_value;
  return oss.str();
}

std::string ErrorCode::getDescription() const {
  return getCodeDescription(m_value);
}

bool ErrorCode::isValidErrorCode(unsigned int code) {
  return code >= MIN_CODE && code <= MAX_CODE;
}

bool ErrorCode::isClientError(unsigned int code) {
  return code >= CLIENT_ERROR_MIN && code <= CLIENT_ERROR_MAX;
}

bool ErrorCode::isServerError(unsigned int code) {
  return code >= SERVER_ERROR_MIN && code <= SERVER_ERROR_MAX;
}

bool ErrorCode::isErrorCode(unsigned int code) {
  return code >= CLIENT_ERROR_MIN && code <= SERVER_ERROR_MAX;
}

bool ErrorCode::isSuccessCode(unsigned int code) {
  return code >= SUCCESS_MIN && code <= SUCCESS_MAX;
}

bool ErrorCode::isRedirectionCode(unsigned int code) {
  return code >= REDIRECTION_MIN && code <= REDIRECTION_MAX;
}

bool ErrorCode::isInformationalCode(unsigned int code) {
  return code >= INFORMATIONAL_MIN && code <= INFORMATIONAL_MAX;
}

void ErrorCode::validate() const {
  if (!isValidErrorCode(m_value)) {
    std::ostringstream oss;
    oss << "Error code out of range: " << m_value
        << " (valid range: " << MIN_CODE << "-" << MAX_CODE << ")";
    throw shared::exceptions::ErrorCodeException(
        oss.str(), shared::exceptions::ErrorCodeException::OUT_OF_RANGE);
  }
}

bool ErrorCode::operator==(const ErrorCode& other) const {
  return m_value == other.m_value;
}

bool ErrorCode::operator!=(const ErrorCode& other) const {
  return m_value != other.m_value;
}

bool ErrorCode::operator<(const ErrorCode& other) const {
  return m_value < other.m_value;
}

bool ErrorCode::operator<=(const ErrorCode& other) const {
  return m_value <= other.m_value;
}

bool ErrorCode::operator>(const ErrorCode& other) const {
  return m_value > other.m_value;
}

bool ErrorCode::operator>=(const ErrorCode& other) const {
  return m_value >= other.m_value;
}

ErrorCode ErrorCode::badRequest() { return ErrorCode(STATUS_BAD_REQUEST); }

ErrorCode ErrorCode::notFound() { return ErrorCode(STATUS_NOT_FOUND); }

ErrorCode ErrorCode::methodNotAllowed() {
  return ErrorCode(STATUS_METHOD_NOT_ALLOWED);
}

ErrorCode ErrorCode::payloadTooLarge() {
  return ErrorCode(STATUS_PAYLOAD_TOO_LARGE);
}

ErrorCode ErrorCode::internalServerError() {
  return ErrorCode(STATUS_INTERNAL_SERVER_ERROR);
}

ErrorCode ErrorCode::serviceUnavailable() {
  return ErrorCode(STATUS_SERVICE_UNAVAILABLE);
}

ErrorCode ErrorCode::ok() { return ErrorCode(STATUS_OK); }

ErrorCode ErrorCode::created() { return ErrorCode(STATUS_CREATED); }

ErrorCode ErrorCode::noContent() { return ErrorCode(STATUS_NO_CONTENT); }

ErrorCode ErrorCode::movedPermanently() {
  return ErrorCode(STATUS_MOVED_PERMANENTLY);
}

ErrorCode ErrorCode::found() { return ErrorCode(STATUS_FOUND); }

ErrorCode ErrorCode::notModified() { return ErrorCode(STATUS_NOT_MODIFIED); }

ErrorCode ErrorCode::forbidden() { return ErrorCode(STATUS_FORBIDDEN); }

ErrorCode ErrorCode::unauthorized() { return ErrorCode(STATUS_UNAUTHORIZED); }

ErrorCode ErrorCode::requestTimeout() {
  return ErrorCode(STATUS_REQUEST_TIMEOUT);
}

ErrorCode ErrorCode::conflict() { return ErrorCode(STATUS_CONFLICT); }

ErrorCode ErrorCode::fromString(const std::string& codeString) {
  return ErrorCode(codeString);
}

bool ErrorCode::isClientError() const { return isClientError(m_value); }

bool ErrorCode::isServerError() const { return isServerError(m_value); }

bool ErrorCode::isError() const { return isErrorCode(m_value); }

bool ErrorCode::isSuccess() const { return isSuccessCode(m_value); }

bool ErrorCode::isRedirection() const { return isRedirectionCode(m_value); }

bool ErrorCode::isInformational() const { return isInformationalCode(m_value); }

bool ErrorCode::isBadRequest() const { return m_value == STATUS_BAD_REQUEST; }

bool ErrorCode::isNotFound() const { return m_value == STATUS_NOT_FOUND; }

bool ErrorCode::isMethodNotAllowed() const {
  return m_value == STATUS_METHOD_NOT_ALLOWED;
}

bool ErrorCode::isPayloadTooLarge() const {
  return m_value == STATUS_PAYLOAD_TOO_LARGE;
}

bool ErrorCode::isInternalServerError() const {
  return m_value == STATUS_INTERNAL_SERVER_ERROR;
}

bool ErrorCode::isServiceUnavailable() const {
  return m_value == STATUS_SERVICE_UNAVAILABLE;
}

bool ErrorCode::isOk() const { return m_value == STATUS_OK; }

bool ErrorCode::isCreated() const { return m_value == STATUS_CREATED; }

bool ErrorCode::isMovedPermanently() const {
  return m_value == STATUS_MOVED_PERMANENTLY;
}

bool ErrorCode::isFound() const { return m_value == STATUS_FOUND; }

bool ErrorCode::isForbidden() const { return m_value == STATUS_FORBIDDEN; }

bool ErrorCode::isUnauthorized() const {
  return m_value == STATUS_UNAUTHORIZED;
}

bool ErrorCode::isRequestTimeout() const {
  return m_value == STATUS_REQUEST_TIMEOUT;
}

bool ErrorCode::isConflict() const { return m_value == STATUS_CONFLICT; }

unsigned int ErrorCode::parseCodeString(const std::string& codeString) {
  if (codeString.empty()) {
    throw shared::exceptions::ErrorCodeException(
        "Error code string cannot be empty",
        shared::exceptions::ErrorCodeException::EMPTY_STRING);
  }

  if (!isAllDigits(codeString)) {
    throw shared::exceptions::ErrorCodeException(
        "Error code contains non-digit characters: '" + codeString + "'",
        shared::exceptions::ErrorCodeException::INVALID_CODE);
  }

  char* endPointer = NULL;
  unsigned long result =
      std::strtoul(codeString.c_str(), &endPointer, BASE_DECIMAL);

  if (endPointer == codeString.c_str() || *endPointer != '\0') {
    throw shared::exceptions::ErrorCodeException(
        "Failed to convert error code string to number: '" + codeString + "'",
        shared::exceptions::ErrorCodeException::CONVERSION_FAILED);
  }

  if (result > static_cast<unsigned long>(MAX_CODE)) {
    std::ostringstream oss;
    oss << "Error code value out of range: '" << codeString
        << "' (max: " << MAX_CODE << ")";
    throw shared::exceptions::ErrorCodeException(
        oss.str(), shared::exceptions::ErrorCodeException::OUT_OF_RANGE);
  }

  return static_cast<unsigned int>(result);
}

bool ErrorCode::isAllDigits(const std::string& inputString) {
  for (std::size_t index = 0; index < inputString.size(); ++index) {
    if (std::isdigit(static_cast<unsigned char>(inputString[index])) == 0) {
      return false;
    }
  }
  return true;
}

std::string ErrorCode::getCodeDescription(unsigned int code) {
  for (StatusCodeIterator iter = statusCodesBegin(); iter != statusCodesEnd();
       ++iter) {
    if (static_cast<unsigned int>(iter->first) == code) {
      return iter->second;
    }
  }

  if (code >= INFORMATIONAL_MIN && code <= INFORMATIONAL_MAX) {
    return "Informational";
  }
  if (code >= SUCCESS_MIN && code <= SUCCESS_MAX) return "Success";
  if (code >= REDIRECTION_MIN && code <= REDIRECTION_MAX) {
    return "Redirection";
  }
  if (code >= CLIENT_ERROR_MIN && code <= CLIENT_ERROR_MAX) {
    return "Client Error";
  }
  if (code >= SERVER_ERROR_MIN && code <= SERVER_ERROR_MAX) {
    return "Server Error";
  }
  return "Unknown Status Code";
}

size_t ErrorCode::statusCodeCount() {
  return sizeof(K_STATUS_CODE_MSGS) / sizeof(K_STATUS_CODE_MSGS[0]);
}

ErrorCode::StatusCodeIterator ErrorCode::statusCodesBegin() {
  return K_STATUS_CODE_MSGS;
}

ErrorCode::StatusCodeIterator ErrorCode::statusCodesEnd() {
  return K_STATUS_CODE_MSGS + statusCodeCount();
}

}  // namespace value_objects
}  // namespace shared
}  // namespace domain
