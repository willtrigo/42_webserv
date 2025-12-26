/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfigException.cpp                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/22 11:47:30 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/26 15:42:20 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shared/exceptions/ServerConfigException.hpp"

#include <sstream>

namespace shared {
namespace exceptions {

const std::pair<ServerConfigException::ErrorCode, std::string>
    ServerConfigException::K_CODE_MSGS[] = {
        std::make_pair(ServerConfigException::INVALID_PORT,
                       "Invalid port number"),
        std::make_pair(ServerConfigException::INVALID_ADDRESS,
                       "Invalid IP address format"),
        std::make_pair(ServerConfigException::DUPLICATE_LISTEN,
                       "Duplicate listen directive"),
        std::make_pair(ServerConfigException::EMPTY_SERVER_NAME,
                       "Server name cannot be empty"),
        std::make_pair(ServerConfigException::INVALID_SERVER_NAME,
                       "Invalid server name format"),
        std::make_pair(ServerConfigException::DUPLICATE_SERVER_NAME,
                       "Duplicate server name"),
        std::make_pair(ServerConfigException::EMPTY_ROOT,
                       "Root path cannot be empty"),
        std::make_pair(ServerConfigException::INVALID_ROOT_PATH,
                       "Invalid root path"),
        std::make_pair(ServerConfigException::MISSING_ROOT,
                       "Missing root configuration"),
        std::make_pair(ServerConfigException::EMPTY_INDEX_FILE,
                       "Index file cannot be empty"),
        std::make_pair(ServerConfigException::DUPLICATE_INDEX_FILE,
                       "Duplicate index file"),
        std::make_pair(ServerConfigException::INVALID_ERROR_CODE,
                       "Invalid error code"),
        std::make_pair(ServerConfigException::EMPTY_ERROR_PAGE_URI,
                       "Error page URI cannot be empty"),
        std::make_pair(ServerConfigException::INVALID_ERROR_PAGE_URI,
                       "Invalid error page URI"),
        std::make_pair(ServerConfigException::NULL_LOCATION,
                       "Null location configuration"),
        std::make_pair(ServerConfigException::DUPLICATE_LOCATION_PATH,
                       "Duplicate location path"),
        std::make_pair(ServerConfigException::EMPTY_REDIRECT_URL,
                       "Redirect URL cannot be empty"),
        std::make_pair(ServerConfigException::INVALID_REDIRECT_CODE,
                       "Invalid redirect code"),
        std::make_pair(ServerConfigException::BODY_SIZE_TOO_LARGE,
                       "Client body size too large"),
        std::make_pair(ServerConfigException::MISSING_LISTEN_DIRECTIVE,
                       "Missing listen directive"),
        std::make_pair(ServerConfigException::MULTIPLE_DEFAULT_SERVERS,
                       "Multiple default servers on same port"),
        std::make_pair(ServerConfigException::MISSING_CONFIGURATION,
                       "Missing server configuration"),
        std::make_pair(ServerConfigException::INVALID_LISTEN_DIRECTIVE_FORMAT,
                       "Invalid listen directive format"),
        std::make_pair(ServerConfigException::LOCATION_VALIDATION_FAILED,
                       "Location configuration validation failed")};

ServerConfigException::ServerConfigException(const std::string& msg,
                                             ErrorCode code)
    : BaseException("", static_cast<int>(code)), m_code(code) {
  std::ostringstream oss;
  oss << getErrorMsg(code) << ": " << msg;
  this->m_whatMsg = oss.str();
}

ServerConfigException::ServerConfigException(const ServerConfigException& other)
    : BaseException(other), m_code(other.m_code) {}

ServerConfigException::~ServerConfigException() throw() {}

ServerConfigException& ServerConfigException::operator=(
    const ServerConfigException& other) {
  if (this != &other) {
    BaseException::operator=(other);
    m_code = other.m_code;
  }
  return *this;
}

ServerConfigException::ErrorCode ServerConfigException::getErrorCode() const {
  return m_code;
}

std::string ServerConfigException::getErrorMsg(
    ServerConfigException::ErrorCode code) {
  for (int i = 0; i < CODE_COUNT; ++i) {
    if (K_CODE_MSGS[i].first == code) {
      return K_CODE_MSGS[i].second;
    }
  }
  return "unknown server configuration error";
}

}  // namespace exceptions
}  // namespace shared
