/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   QueryStringBuilderException.hpp                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/23 00:37:19 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/23 00:37:23 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef QUERYSTRINGBUILDEREXCEPTION_HPP
#define QUERYSTRINGBUILDEREXCEPTION_HPP

#include "shared/exceptions/BaseException.hpp"

namespace shared {
namespace exceptions {

class QueryStringBuilderException : public BaseException {
 public:
  enum ErrorCode {
    EMPTY_URL,
    INVALID_URL_FORMAT,
    INVALID_PARAMETER_NAME,
    INVALID_PARAMETER_VALUE,
    ENCODING_ERROR,
    DECODING_ERROR,
    MALFORMED_QUERY_STRING,
    DUPLICATE_PARAMETER,
    CODE_COUNT
  };

  explicit QueryStringBuilderException(const std::string& msg, ErrorCode code);
  QueryStringBuilderException(const QueryStringBuilderException& other);
  virtual ~QueryStringBuilderException() throw();

  QueryStringBuilderException& operator=(
      const QueryStringBuilderException& other);

 private:
  static const std::pair<ErrorCode, std::string> K_CODE_MSGS[];

  static std::string getErrorMsg(ErrorCode code);
};

}  // namespace exceptions
}  // namespace shared

#endif  // QUERYSTRINGBUILDEREXCEPTION_HPP
