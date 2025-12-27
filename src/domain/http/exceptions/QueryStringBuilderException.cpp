/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   QueryStringBuilderException.cpp                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/23 00:37:42 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/27 03:11:10 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "domain/http/exceptions/QueryStringBuilderException.hpp"

#include <sstream>

namespace domain {
namespace http {
namespace exceptions {

const std::pair<QueryStringBuilderException::ErrorCode, std::string>
    QueryStringBuilderException::K_CODE_MSGS[] = {
        std::make_pair(QueryStringBuilderException::EMPTY_URL,
                       "URL cannot be empty"),
        std::make_pair(QueryStringBuilderException::INVALID_URL_FORMAT,
                       "Invalid URL format"),
        std::make_pair(QueryStringBuilderException::INVALID_PARAMETER_NAME,
                       "Invalid parameter name"),
        std::make_pair(QueryStringBuilderException::INVALID_PARAMETER_VALUE,
                       "Invalid parameter value"),
        std::make_pair(QueryStringBuilderException::ENCODING_ERROR,
                       "Failed to encode value"),
        std::make_pair(QueryStringBuilderException::DECODING_ERROR,
                       "Failed to decode value"),
        std::make_pair(QueryStringBuilderException::MALFORMED_QUERY_STRING,
                       "Malformed query string"),
        std::make_pair(QueryStringBuilderException::DUPLICATE_PARAMETER,
                       "Duplicate parameter found")};

QueryStringBuilderException::QueryStringBuilderException(const std::string& msg,
                                                         ErrorCode code)
    : BaseException("", static_cast<int>(code)) {
  std::ostringstream oss;
  oss << getErrorMsg(code) << ": " << msg;
  this->m_whatMsg = oss.str();
}

QueryStringBuilderException::QueryStringBuilderException(
    const QueryStringBuilderException& other)
    : BaseException(other) {}

QueryStringBuilderException::~QueryStringBuilderException() throw() {}

QueryStringBuilderException& QueryStringBuilderException::operator=(
    const QueryStringBuilderException& other) {
  if (this != &other) {
    BaseException::operator=(other);
  }
  return *this;
}

std::string QueryStringBuilderException::getErrorMsg(
    QueryStringBuilderException::ErrorCode code) {
  for (int index = 0; index < CODE_COUNT; ++index) {
    if (K_CODE_MSGS[index].first == code) {
      return K_CODE_MSGS[index].second;
    }
  }
  return "unknown query string builder error";
}

}  // namespace exceptions
}  // namespace http
}  // namespace domain
