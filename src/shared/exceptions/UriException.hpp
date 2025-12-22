/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   UriException.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/21 12:40:19 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/21 12:40:21 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef URI_EXCEPTION_HPP
#define URI_EXCEPTION_HPP

#include "shared/exceptions/BaseException.hpp"

namespace shared {
namespace exceptions {

class UriException : public BaseException {
 public:
  enum ErrorCode {
    EMPTY_URI,
    INVALID_FORMAT,
    INVALID_SCHEME,
    INVALID_HOST,
    INVALID_PORT,
    INVALID_PATH,
    INVALID_QUERY,
    INVALID_FRAGMENT,
    MISSING_SCHEME,
    MISSING_HOST,
    PORT_OUT_OF_RANGE,
    ENCODING_ERROR,
    DECODING_ERROR,
    CODE_COUNT
  };

  explicit UriException(const std::string& msg, ErrorCode code);
  UriException(const UriException& other);
  virtual ~UriException() throw();

  UriException& operator=(const UriException& other);

 private:
  static const std::pair<ErrorCode, std::string> K_CODE_MSGS[];

  static std::string getErrorMsg(ErrorCode code);
};

}  // namespace exceptions
}  // namespace shared

#endif  // URI_EXCEPTION_HPP
