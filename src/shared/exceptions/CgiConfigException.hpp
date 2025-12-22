/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiConfigException.hpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/22 13:10:17 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/22 13:12:28 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGI_CONFIG_EXCEPTION_HPP
#define CGI_CONFIG_EXCEPTION_HPP

#include "shared/exceptions/BaseException.hpp"

namespace shared {
namespace exceptions {

class CgiConfigException : public BaseException {
 public:
  enum ErrorCode {
    EMPTY_SCRIPT_PATH,
    INVALID_SCRIPT_PATH,
    EMPTY_CGI_ROOT,
    INVALID_CGI_ROOT,
    INVALID_EXTENSION_PATTERN,
    DUPLICATE_CGI_PARAM,
    INVALID_CGI_PARAM,
    MISSING_REQUIRED_PARAMS,
    CODE_COUNT
  };

  explicit CgiConfigException(const std::string& msg, ErrorCode code);
  CgiConfigException(const CgiConfigException& other);
  virtual ~CgiConfigException() throw();

  CgiConfigException& operator=(const CgiConfigException& other);

 private:
  static const std::pair<ErrorCode, std::string> K_CODE_MSGS[];

  static std::string getErrorMsg(ErrorCode code);
};

}  // namespace exceptions
}  // namespace shared

#endif  // CGI_CONFIG_EXCEPTION_HPP
