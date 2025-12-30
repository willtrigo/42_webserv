/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Token.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/30 12:54:58 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/30 17:20:39 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <string>

namespace infrastructure {
namespace config {
namespace lexer {

class Token {
 public:
  enum Type { DIRECTIVE, BLOCK_START, BLOCK_END, STRING, SEMICOLON, EOF_T };

  Type type;
  std::string value;
  std::size_t lineNumber;

  Token(Type typ, const std::string& val, std::size_t lnNum);

  std::string typeToString() const;
  bool operator==(const Token& other) const;
  bool operator!=(const Token& other) const;
};

}  // namespace lexer
}  // namespace config
}  // namespace infrastructure

#endif  // TOKEN_HPP
