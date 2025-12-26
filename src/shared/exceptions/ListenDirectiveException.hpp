/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ListenDirectiveException.hpp                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/26 03:18:52 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/26 03:18:53 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LISTEN_DIRECTIVE_EXCEPTION_HPP
#define LISTEN_DIRECTIVE_EXCEPTION_HPP

#include "shared/exceptions/BaseException.hpp"

namespace shared {
namespace exceptions {

class ListenDirectiveException : public shared::exceptions::BaseException {
 public:
  enum ErrorCode {
    INVALID_FORMAT,
    MISSING_PORT,
    INVALID_HOST,
    INVALID_PORT,
    EMPTY_STRING,
    DUPLICATE_DIRECTIVE,
    CODE_COUNT
  };

  explicit ListenDirectiveException(const std::string& msg, ErrorCode code);
  ListenDirectiveException(const ListenDirectiveException& other);
  virtual ~ListenDirectiveException() throw();

  ListenDirectiveException& operator=(const ListenDirectiveException& other);

  ErrorCode getErrorCode() const;

 private:
  static const std::pair<ErrorCode, std::string> K_CODE_MSGS[];
  ErrorCode m_errorCode;

  static std::string getErrorMsg(ErrorCode code);
};

}  // namespace exceptions
}  // namespace shared

#endif  // LISTEN_DIRECTIVE_EXCEPTION_HPP
