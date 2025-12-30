/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Token.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/30 14:24:24 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/30 17:21:12 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "infrastructure/config/lexer/Token.hpp"

namespace infrastructure {
namespace config {
namespace lexer {

Token::Token(Type typ, const std::string& val, std::size_t lnNum)
    : type(typ), value(val), lineNumber(lnNum) {}

std::string Token::typeToString() const {
  switch (type) {
    case DIRECTIVE:
      return "DIRECTIVE";
    case BLOCK_START:
      return "BLOCK_START";
    case BLOCK_END:
      return "BLOCK_END";
    case STRING:
      return "STRING";
    case SEMICOLON:
      return "SEMICOLON";
    case EOF_T:
      return "EOF";
    default:
      return "UNKNOWN";
  }
}

bool Token::operator==(const Token& other) const {
  return type == other.type && value == other.value &&
         lineNumber == other.lineNumber;
}

bool Token::operator!=(const Token& other) const { return !(*this == other); }

}  // namespace lexer
}  // namespace config
}  // namespace infrastructure
