/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigException.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/18 12:15:54 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/18 12:27:24 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shared/exceptions/ConfigException.hpp"

#include <sstream>

namespace shared {
namespace exceptions {

const char* ConfigException::m_codeMsgs[] = {
    "Syntax error in config",
    "Mismatched block delimiters { }",
    "Config file not found or unreadable",
    "Duplicate listen port across servers",
    "Invalid path directive (nonexistent dir)",
    "Missing required directive (e.g., listen/root)",
    "Recursive include detected"};

ConfigException::ConfigException(const std::string& msg, ErrorCode code)
    : BaseException("", static_cast<int>(code)) {
  std::ostringstream oss;
  oss << shared::exceptions::ConfigException::m_codeMsgs[code -
                                                         STATUS_BEGIN_COUNT]
      << ": " << msg;
  this->m_whatMsg = oss.str();
}

ConfigException::~ConfigException() throw() {}

}  // namespace exceptions
}  // namespace shared
