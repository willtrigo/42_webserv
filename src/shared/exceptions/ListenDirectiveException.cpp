/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ListenDirectiveException.cpp                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/26 03:19:15 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/26 16:48:50 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shared/exceptions/ListenDirectiveException.hpp"

#include <sstream>

namespace shared {
namespace exceptions {

const std::pair<ListenDirectiveException::ErrorCode, std::string>
    ListenDirectiveException::K_CODE_MSGS[] = {
        std::make_pair(ListenDirectiveException::INVALID_FORMAT,
                       "Invalid listen directive format"),
        std::make_pair(ListenDirectiveException::MISSING_PORT,
                       "Port is missing in listen directive"),
        std::make_pair(ListenDirectiveException::INVALID_HOST,
                       "Invalid host in listen directive"),
        std::make_pair(ListenDirectiveException::INVALID_PORT,
                       "Invalid port in listen directive"),
        std::make_pair(ListenDirectiveException::EMPTY_STRING,
                       "Listen directive string is empty"),
        std::make_pair(ListenDirectiveException::DUPLICATE_DIRECTIVE,
                       "Duplicate listen directive detected")};

ListenDirectiveException::ListenDirectiveException(const std::string& msg,
                                                   ErrorCode code)
    : BaseException("", static_cast<int>(code)) {
  std::ostringstream oss;
  oss << getErrorMsg(code) << ": " << msg;
  this->m_whatMsg = oss.str();
}

ListenDirectiveException::ListenDirectiveException(
    const ListenDirectiveException& other)
    : BaseException(other) {}

ListenDirectiveException::~ListenDirectiveException() throw() {}

ListenDirectiveException& ListenDirectiveException::operator=(
    const ListenDirectiveException& other) {
  if (this != &other) {
    BaseException::operator=(other);
  }
  return *this;
}

std::string ListenDirectiveException::getErrorMsg(
    ListenDirectiveException::ErrorCode code) {
  for (int i = 0; i < CODE_COUNT; ++i) {
    if (K_CODE_MSGS[i].first == code) {
      return K_CODE_MSGS[i].second;
    }
  }
  return "Unknown listen directive error";
}

}  // namespace exceptions
}  // namespace shared
