/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RegexPatternException.hpp                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/21 10:50:38 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/27 02:12:21 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REGEX_PATTERN_EXCEPTION_HPP
#define REGEX_PATTERN_EXCEPTION_HPP

#include "shared/exceptions/BaseException.hpp"

namespace domain {
namespace shared {
namespace exceptions {

class RegexPatternException : public ::shared::exceptions::BaseException {
 public:
  enum ErrorCode {
    EMPTY_PATTERN,
    INVALID_CHARACTER,
    UNBALANCED_BRACKETS,
    UNBALANCED_PARENTHESES,
    UNBALANCED_BRACES,
    UNESCAPED_SPECIAL,
    INVALID_ESCAPE,
    PATTERN_TOO_LONG,
    COMPILATION_FAILED,
    CODE_COUNT
  };

  explicit RegexPatternException(const std::string& msg, ErrorCode code);
  RegexPatternException(const RegexPatternException& other);
  virtual ~RegexPatternException() throw();

  RegexPatternException& operator=(const RegexPatternException& other);

 private:
  static const std::pair<ErrorCode, std::string> K_CODE_MSGS[];

  static std::string getErrorMsg(ErrorCode code);
};

}  // namespace exceptions
}  // namespace shared
}  // namespace domain

#endif  // REGEX_PATTERN_EXCEPTION_HPP
