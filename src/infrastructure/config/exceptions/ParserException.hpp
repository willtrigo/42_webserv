/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParserException.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/30 14:07:24 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/30 14:11:42 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_EXCEPTION_HPP
#define PARSER_EXCEPTION_HPP

#include "shared/exceptions/BaseException.hpp"

namespace infrastructure {
namespace config {
namespace exceptions {

class ParserException : public ::shared::exceptions::BaseException {
 public:
  enum ErrorCode {
    FILE_NOT_FOUND,
    PERMISSION_DENIED,
    INCLUDE_RECURSION,
    UNEXPECTED_ERROR,
    CODE_COUNT
  };

  explicit ParserException(const std::string& msg, ErrorCode code);
  ParserException(const ParserException& other);
  virtual ~ParserException() throw();

  ParserException& operator=(const ParserException& other);

 private:
  static const std::pair<ErrorCode, std::string> K_CODE_MSGS[];

  static std::string getErrorMsg(ErrorCode code);
};

}  // namespace exceptions
}  // namespace config
}  // namespace infrastructure

#endif  // PARSER_EXCEPTION_HPP
