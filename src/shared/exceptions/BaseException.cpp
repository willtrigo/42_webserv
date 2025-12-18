/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BaseException.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/18 11:54:51 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/18 12:35:45 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shared/exceptions/BaseException.hpp"

#include <sstream>

namespace shared {
namespace exceptions {

BaseException::BaseException(const std::string& msg, int code) {
  std::ostringstream oss;
  oss << "[Error " << code << "] " << msg;
  this->m_whatMsg = oss.str();
}

BaseException::BaseException(const BaseException& /*unused*/) {}

BaseException::~BaseException() throw() {}

BaseException& BaseException::operator=(const BaseException& /*unused*/) {
  return *this;
}

const char* BaseException::what() const throw() {
  return this->m_whatMsg.c_str();
}

}  // namespace exceptions
}  // namespace shared
