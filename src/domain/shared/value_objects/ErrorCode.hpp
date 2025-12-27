/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ErrorCode.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/20 11:08:29 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/27 17:19:51 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ERROR_CODE_HPP
#define ERROR_CODE_HPP

#include <string>

namespace domain {
namespace shared {
namespace value_objects {

class ErrorCode {
 public:
  enum StatusCode {
    MIN_CODE = 100,
    MAX_CODE = 599,

    INFORMATIONAL_MIN = 100,
    INFORMATIONAL_MAX = 199,
    SUCCESS_MIN = 200,
    SUCCESS_MAX = 299,
    REDIRECTION_MIN = 300,
    REDIRECTION_MAX = 399,
    CLIENT_ERROR_MIN = 400,
    CLIENT_ERROR_MAX = 499,
    SERVER_ERROR_MIN = 500,
    SERVER_ERROR_MAX = 599,

    STATUS_BAD_REQUEST = 400,
    STATUS_UNAUTHORIZED = 401,
    STATUS_FORBIDDEN = 403,
    STATUS_NOT_FOUND = 404,
    STATUS_METHOD_NOT_ALLOWED = 405,
    STATUS_REQUEST_TIMEOUT = 408,
    STATUS_CONFLICT = 409,
    STATUS_GONE = 410,
    STATUS_LENGTH_REQUIRED = 411,
    STATUS_PAYLOAD_TOO_LARGE = 413,
    STATUS_URI_TOO_LONG = 414,
    STATUS_UNSUPPORTED_MEDIA_TYPE = 415,
    STATUS_RANGE_NOT_SATISFIABLE = 416,
    STATUS_EXPECTATION_FAILED = 417,
    STATUS_IM_A_TEAPOT = 418,
    STATUS_UPGRADE_REQUIRED = 426,
    STATUS_PRECONDITION_REQUIRED = 428,
    STATUS_TOO_MANY_REQUESTS = 429,
    STATUS_REQUEST_HEADER_FIELDS_TOO_LARGE = 431,
    STATUS_UNAVAILABLE_FOR_LEGAL_REASONS = 451,

    STATUS_INTERNAL_SERVER_ERROR = 500,
    STATUS_NOT_IMPLEMENTED = 501,
    STATUS_BAD_GATEWAY = 502,
    STATUS_SERVICE_UNAVAILABLE = 503,
    STATUS_GATEWAY_TIMEOUT = 504,
    STATUS_HTTP_VERSION_NOT_SUPPORTED = 505,
    STATUS_VARIANT_ALSO_NEGOTIATES = 506,
    STATUS_INSUFFICIENT_STORAGE = 507,
    STATUS_LOOP_DETECTED = 508,
    STATUS_NOT_EXTENDED = 510,
    STATUS_NETWORK_AUTHENTICATION_REQUIRED = 511,

    STATUS_OK = 200,
    STATUS_CREATED = 201,
    STATUS_ACCEPTED = 202,
    STATUS_NO_CONTENT = 204,
    STATUS_RESET_CONTENT = 205,
    STATUS_PARTIAL_CONTENT = 206,

    STATUS_MULTIPLE_CHOICES = 300,
    STATUS_MOVED_PERMANENTLY = 301,
    STATUS_FOUND = 302,
    STATUS_SEE_OTHER = 303,
    STATUS_NOT_MODIFIED = 304,
    STATUS_TEMPORARY_REDIRECT = 307,
    STATUS_PERMANENT_REDIRECT = 308,

    STATUS_CONTINUE = 100,
    STATUS_SWITCHING_PROTOCOLS = 101,
    STATUS_PROCESSING = 102,
    STATUS_EARLY_HINTS = 103
  };

  static const int BASE_DECIMAL = 10;

  ErrorCode();
  explicit ErrorCode(unsigned int code);
  explicit ErrorCode(const std::string& codeString);
  ErrorCode(const ErrorCode& other);
  ~ErrorCode();

  ErrorCode& operator=(const ErrorCode& other);

  unsigned int getValue() const;
  std::string toString() const;
  std::string getDescription() const;

  static bool isValidErrorCode(unsigned int code);
  static bool isClientError(unsigned int code);
  static bool isServerError(unsigned int code);
  static bool isErrorCode(unsigned int code);
  static bool isSuccessCode(unsigned int code);
  static bool isRedirectionCode(unsigned int code);
  static bool isInformationalCode(unsigned int code);

  bool operator==(const ErrorCode& other) const;
  bool operator!=(const ErrorCode& other) const;
  bool operator<(const ErrorCode& other) const;
  bool operator<=(const ErrorCode& other) const;
  bool operator>(const ErrorCode& other) const;
  bool operator>=(const ErrorCode& other) const;

  static ErrorCode badRequest();
  static ErrorCode notFound();
  static ErrorCode methodNotAllowed();
  static ErrorCode payloadTooLarge();
  static ErrorCode internalServerError();
  static ErrorCode serviceUnavailable();

  static ErrorCode ok();
  static ErrorCode created();
  static ErrorCode noContent();

  static ErrorCode movedPermanently();
  static ErrorCode found();
  static ErrorCode notModified();

  static ErrorCode forbidden();
  static ErrorCode unauthorized();
  static ErrorCode requestTimeout();
  static ErrorCode conflict();

  static ErrorCode fromString(const std::string& codeString);

  bool isClientError() const;
  bool isServerError() const;
  bool isError() const;
  bool isSuccess() const;
  bool isRedirection() const;
  bool isInformational() const;

  bool isBadRequest() const;
  bool isNotFound() const;
  bool isMethodNotAllowed() const;
  bool isPayloadTooLarge() const;
  bool isInternalServerError() const;
  bool isServiceUnavailable() const;

  bool isOk() const;
  bool isCreated() const;
  bool isMovedPermanently() const;
  bool isFound() const;

  bool isForbidden() const;
  bool isUnauthorized() const;
  bool isRequestTimeout() const;
  bool isConflict() const;

 private:
  unsigned int m_value;

  typedef const std::pair<ErrorCode::StatusCode, std::string> StatusCodeEntry;
  typedef StatusCodeEntry* StatusCodeIterator;
  static const StatusCodeEntry K_STATUS_CODE_MSGS[];

  void validate() const;
  static unsigned int parseCodeString(const std::string& codeString);
  static bool isAllDigits(const std::string& inputString);
  static std::string getStatusCodeMsg(StatusCode statusCode);

  static std::string getCodeDescription(unsigned int code);
  static size_t statusCodeCount();
  static StatusCodeIterator statusCodesBegin();
  static StatusCodeIterator statusCodesEnd();
};

}  // namespace value_objects
}  // namespace shared
}  // namespace domain

#endif  // ERROR_CODE_HPP
