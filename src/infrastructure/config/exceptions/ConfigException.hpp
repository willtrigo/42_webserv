/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigException.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/18 12:05:20 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/28 15:59:18 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_EXCEPTION_HPP
#define CONFIG_EXCEPTION_HPP

#include "shared/exceptions/BaseException.hpp"

namespace infrastructure {
namespace config {
namespace exceptions {

class ConfigException : public ::shared::exceptions::BaseException {
 public:
  enum ErrorCode {
    PARSE_SYNTAX,
    PARSE_BLOCK_MISMATCH,
    LOAD_FILE_NOT_FOUND,
    VALIDATION_DUPLICATE_PORT,
    VALIDATION_INVALID_PATH,
    VALIDATION_MISSING_DIRECTIVE,
    INCLUDE_RECURSION,
    LOAD_UNEXPECTED,
    INVALID_STATE,
    CODE_COUNT
  };

  explicit ConfigException(const std::string& msg, ErrorCode code);
  ConfigException(const ConfigException& other);
  virtual ~ConfigException() throw();

  ConfigException& operator=(const ConfigException& other);

 private:
  static const std::pair<ErrorCode, std::string> K_CODE_MSGS[];

  static std::string getErrorMsg(ErrorCode code);
};

}  // namespace exceptions
}  // namespace config
}  // namespace infrastructure

#endif  // CONFIG_EXCEPTION_HPP
