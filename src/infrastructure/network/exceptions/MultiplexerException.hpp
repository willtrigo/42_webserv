/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MultiplexerException.hpp                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/04 00:08:44 by dande-je          #+#    #+#             */
/*   Updated: 2026/01/04 00:08:53 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MULTIPLEXEREXCEPTION_HPP
#define MULTIPLEXEREXCEPTION_HPP

#include "shared/exceptions/BaseException.hpp"

namespace infrastructure {
namespace network {
namespace exceptions {

class MultiplexerException : public ::shared::exceptions::BaseException {
 public:
  enum ErrorCode {
    EPOLL_CREATE_FAILED,
    EPOLL_CTL_ADD_FAILED,
    EPOLL_CTL_MOD_FAILED,
    EPOLL_CTL_DEL_FAILED,
    EPOLL_WAIT_FAILED,
    INVALID_FILE_DESCRIPTOR,
    ALREADY_REGISTERED,
    NOT_REGISTERED,
    INVALID_EVENT_MASK,
    REGISTRATION_LIMIT_EXCEEDED,
    TIMEOUT,
    CODE_COUNT
  };

  explicit MultiplexerException(const std::string& msg, ErrorCode code);
  explicit MultiplexerException(const std::string& msg, ErrorCode code,
                                int systemError);
  MultiplexerException(const MultiplexerException& other);
  virtual ~MultiplexerException() throw();

  MultiplexerException& operator=(const MultiplexerException& other);

  int getSystemError() const;

 private:
  static const std::pair<ErrorCode, std::string> K_CODE_MSGS[];

  int m_systemError;

  static std::string getErrorMsg(ErrorCode code);
  static std::string formatSystemError(int systemError);
};

}  // namespace exceptions
}  // namespace network
}  // namespace infrastructure

#endif  // MULTIPLEXEREXCEPTION_HPP
