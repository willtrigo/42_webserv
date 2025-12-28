/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpConfigException.cpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/26 16:28:16 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/27 15:38:32 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "domain/configuration/exceptions/HttpConfigException.hpp"

#include <sstream>

namespace domain {
namespace configuration {
namespace exceptions {

const std::pair<HttpConfigException::ErrorCode, std::string>
    HttpConfigException::K_CODE_MSGS[] = {
        std::make_pair(EMPTY_CONFIGURATION,
                       "HTTP configuration cannot be empty"),
        std::make_pair(DUPLICATE_SERVER, "Duplicate server configuration"),
        std::make_pair(INVALID_SERVER_CONFIG, "Invalid server configuration"),
        std::make_pair(NO_DEFAULT_SERVER,
                       "No default server found for the requested port"),
        std::make_pair(MULTIPLE_DEFAULT_SERVERS,
                       "Multiple default servers found for the same port"),
        std::make_pair(INVALID_WORKER_PROCESSES,
                       "Invalid number of worker processes"),
        std::make_pair(INVALID_WORKER_CONNECTIONS,
                       "Invalid number of worker connections"),
        std::make_pair(INVALID_KEEPALIVE_TIMEOUT, "Invalid keepalive timeout"),
        std::make_pair(INVALID_SEND_TIMEOUT, "Invalid send timeout"),
        std::make_pair(INVALID_CLIENT_MAX_BODY_SIZE,
                       "Invalid client max body size"),
        std::make_pair(INVALID_ERROR_LOG_PATH, "Invalid error log path"),
        std::make_pair(INVALID_ACCESS_LOG_PATH, "Invalid access log path"),
        std::make_pair(INVALID_MIME_TYPES_PATH, "Invalid MIME types file path"),
        std::make_pair(INVALID_CONFIG_FILE, "Invalid configuration file"),
        std::make_pair(FILE_NOT_FOUND, "Configuration file not found"),
        std::make_pair(FILE_READ_ERROR, "Error reading configuration file"),
        std::make_pair(PARSE_ERROR, "Error parsing configuration file"),
        std::make_pair(MISSING_REQUIRED_DIRECTIVE,
                       "Missing required configuration directive"),
        std::make_pair(CONFIG_VALIDATION_FAILED,
                       "HTTP configuration validation failed"),
        std::make_pair(SERVER_SELECTION_FAILED,
                       "Failed to select server for request"),
        std::make_pair(PORT_CONFLICT, "Port conflict detected"),
        std::make_pair(ADDRESS_CONFLICT, "Address conflict detected"),
        std::make_pair(CODE_COUNT, "")  // Sentinel value
};

HttpConfigException::HttpConfigException(const std::string& msg, ErrorCode code)
    : BaseException("", static_cast<int>(code)) {
  std::ostringstream oss;
  oss << getErrorMsg(code) << ": " << msg;
  this->m_whatMsg = oss.str();
}

HttpConfigException::HttpConfigException(const HttpConfigException& other)
    : BaseException(other) {}

HttpConfigException::~HttpConfigException() throw() {}

HttpConfigException& HttpConfigException::operator=(
    const HttpConfigException& other) {
  if (this != &other) {
    BaseException::operator=(other);
  }
  return *this;
}

std::string HttpConfigException::getErrorMsg(ErrorCode code) {
  for (size_t i = 0; i < CODE_COUNT; ++i) {
    if (K_CODE_MSGS[i].first == code) {
      return K_CODE_MSGS[i].second;
    }
  }
  return "Unknown http config error";
}

}  // namespace exceptions
}  // namespace configuration
}  // namespace domain
