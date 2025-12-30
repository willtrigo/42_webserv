/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SyntaxException.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/30 13:30:16 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/30 14:06:27 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SYNTAX_EXCEPTION_HPP
#define SYNTAX_EXCEPTION_HPP

#include "shared/exceptions/BaseException.hpp"

namespace infrastructure {
namespace config {
namespace exceptions {

class SyntaxException : public ::shared::exceptions::BaseException {
 public:
  enum ErrorCode {
    UNEXPECTED_TOKEN,
    UNEXPECTED_EOF,
    MISSING_SEMICOLON,
    MISSING_BRACE,
    INVALID_DIRECTIVE,
    CODE_COUNT
  };

  explicit SyntaxException(const std::string& msg, ErrorCode code);
  SyntaxException(const SyntaxException& other);
  virtual ~SyntaxException() throw();

  SyntaxException& operator=(const SyntaxException& other);

 private:
  static const std::pair<ErrorCode, std::string> K_CODE_MSGS[];

  static std::string getErrorMsg(ErrorCode code);
};

}  // namespace exceptions
}  // namespace config
}  // namespace infrastructure

#endif  // SYNTAX_EXCEPTION_HPP
