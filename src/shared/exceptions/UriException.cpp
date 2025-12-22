/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   UriException.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/21 12:44:17 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/21 12:44:22 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shared/exceptions/UriException.hpp"

#include <sstream>

namespace shared {
namespace exceptions {

const std::pair<UriException::ErrorCode, std::string>
    UriException::K_CODE_MSGS[] = {
        std::make_pair(UriException::EMPTY_URI,
                       "URI cannot be empty"),
        std::make_pair(UriException::INVALID_FORMAT,
                       "Invalid URI format"),
        std::make_pair(UriException::INVALID_SCHEME,
                       "Invalid URI scheme"),
        std::make_pair(UriException::INVALID_HOST,
                       "Invalid URI host"),
        std::make_pair(UriException::INVALID_PORT,
                       "Invalid URI port"),
        std::make_pair(UriException::INVALID_PATH,
                       "Invalid URI path"),
        std::make_pair(UriException::INVALID_QUERY,
                       "Invalid URI query string"),
        std::make_pair(UriException::INVALID_FRAGMENT,
                       "Invalid URI fragment"),
        std::make_pair(UriException::MISSING_SCHEME,
                       "URI scheme is missing"),
        std::make_pair(UriException::MISSING_HOST,
                       "URI host is missing"),
        std::make_pair(UriException::PORT_OUT_OF_RANGE,
                       "URI port out of range"),
        std::make_pair(UriException::ENCODING_ERROR,
                       "Failed to encode URI component"),
        std::make_pair(UriException::DECODING_ERROR,
                       "Failed to decode URI component")};

UriException::UriException(const std::string& msg, ErrorCode code)
    : BaseException("", static_cast<int>(code)) {
  std::ostringstream oss;
  oss << getErrorMsg(code) << ": " << msg;
  this->m_whatMsg = oss.str();
}

UriException::UriException(const UriException& other)
    : BaseException(other) {}

UriException::~UriException() throw() {}

UriException& UriException::operator=(const UriException& other) {
  if (this != &other) {
    BaseException::operator=(other);
  }
  return *this;
}

std::string UriException::getErrorMsg(UriException::ErrorCode code) {
  for (int index = 0; index < CODE_COUNT; ++index) {
    if (K_CODE_MSGS[index].first == code) {
      return K_CODE_MSGS[index].second;
    }
  }
  return "unknown URI error";
}

}  // namespace exceptions
}  // namespace shared
