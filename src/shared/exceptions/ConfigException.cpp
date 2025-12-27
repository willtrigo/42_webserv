/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigException.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/18 12:15:54 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/19 23:46:36 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shared/exceptions/ConfigException.hpp"

#include <sstream>

namespace shared {
namespace exceptions {

const std::pair<ConfigException::ErrorCode, std::string>
    ConfigException::K_CODE_MSGS[] = {
        std::make_pair(ConfigException::PARSE_SYNTAX, "syntax error in config"),
        std::make_pair(ConfigException::PARSE_BLOCK_MISMATCH,
                       "mismatched block delimiters { }"),
        std::make_pair(ConfigException::LOAD_FILE_NOT_FOUND,
                       "config file not found or unreadable"),
        std::make_pair(ConfigException::VALIDATION_DUPLICATE_PORT,
                       "duplicate listen port across servers"),
        std::make_pair(ConfigException::VALIDATION_INVALID_PATH,
                       "invalid path directive (nonexistent dir)"),
        std::make_pair(ConfigException::VALIDATION_MISSING_DIRECTIVE,
                       "missing required directive (e.g., listen/root)"),
        std::make_pair(ConfigException::INCLUDE_RECURSION,
                       "recursive include detected"),
        std::make_pair(ConfigException::LOAD_UNEXPECTED,
                       "unexpected error during configuration load"),
        std::make_pair(
            ConfigException::INVALID_STATE,
            "invalid configuration state (e.g., unloaded or unvalidated)")};

ConfigException::ConfigException(const std::string& msg, ErrorCode code)
    : BaseException("", static_cast<int>(code)) {
  std::ostringstream oss;
  oss << getErrorMsg(code) << ": " << msg;
  this->m_whatMsg = oss.str();
}

ConfigException::ConfigException(const ConfigException& other)
    : BaseException(other) {}

ConfigException::~ConfigException() throw() {}

ConfigException& ConfigException::operator=(const ConfigException& other) {
  if (this != &other) {
    BaseException::operator=(other);
  }
  return *this;
}

std::string ConfigException::getErrorMsg(ConfigException::ErrorCode code) {
  for (int i = 0; i < CODE_COUNT; ++i) {
    if (K_CODE_MSGS[i].first == code) {
      return K_CODE_MSGS[i].second;
    }
  }
  return "unknown config error";
}

}  // namespace exceptions
}  // namespace shared
