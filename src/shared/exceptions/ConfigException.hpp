/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigException.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/18 12:05:20 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/18 20:24:17 by dande-je         ###   ########.fr       */
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
    LOAD_UNEXPECTED = 107,
    INVALID_STATE = 108,
    CODE_COUNT = 109,
    STATUS_BEGIN_COUNT = 100
  };

  explicit ConfigException(const std::string& msg, ErrorCode code);
  ConfigException(const ConfigException& other);
  virtual ~ConfigException() throw();

  ConfigException& operator=(const ConfigException& other);

 private:
  static const char* m_codeMsgs[CODE_COUNT - STATUS_BEGIN_COUNT];
};

}  // namespace exceptions
}  // namespace shared

#endif  // CONFIG_EXCEPTION_HPP
