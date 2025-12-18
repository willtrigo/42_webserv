/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigException.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/18 12:05:20 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/18 12:38:28 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_EXCEPTION_HPP
#define CONFIG_EXCEPTION_HPP

#include "shared/exceptions/BaseException.hpp"

namespace shared {
namespace exceptions {

class ConfigException : public BaseException {
 public:
  enum ErrorCode {
    PARSE_SYNTAX = 100,
    PARSE_BLOCK_MISMATCH = 101,
    LOAD_FILE_NOT_FOUND = 102,
    VALIDATION_DUPLICATE_PORT = 103,
    VALIDATION_INVALID_PATH = 104,
    VALIDATION_MISSING_DIRECTIVE = 105,
    INCLUDE_RECURSION = 106,
    CODE_COUNT = 107,
    STATUS_BEGIN_COUNT = 100
  };

  ConfigException(const std::string& msg, ErrorCode code);
  ConfigException(const ConfigException&);
  ~ConfigException() throw();

  ConfigException& operator=(const ConfigException&);

 private:
  static const char* m_codeMsgs[CODE_COUNT - STATUS_BEGIN_COUNT];
};

}  // namespace exceptions
}  // namespace shared

#endif  // CONFIG_EXCEPTION_HPP
