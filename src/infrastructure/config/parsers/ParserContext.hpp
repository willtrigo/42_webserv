/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParserContext.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/30 12:55:05 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/30 18:10:36 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_CONTEXT_HPP
#define PARSER_CONTEXT_HPP

#include "infrastructure/config/lexer/Token.hpp"
#include "infrastructure/config/parsers/ParserState.hpp"

#include <stack>
#include <string>
#include <vector>

namespace infrastructure {
namespace config {
namespace parser {

class ParserContext {
 public:
  ParserContext(const std::vector<lexer::Token>& tokens,
                const std::string& sourceFile);

  const lexer::Token& currentToken() const;
  const lexer::Token& peekToken() const;
  bool hasMoreTokens() const;

  void advance();
  void expect(lexer::Token::Type type, const std::string& context) const;

  void pushState(ParserState::Context context, const std::string& name = "");
  void popState();
  ParserState currentState() const;

  std::size_t currentLine() const;
  std::string sourceFile() const;

  std::string currentContextDescription() const;
  void expectString(const std::string& context) const;
  std::string consumeString(const std::string& context);

 private:
  const std::vector<lexer::Token>& m_tokens;
  std::size_t m_currentIndex;
  std::stack<ParserState> m_stateStack;
  std::string m_sourceFile;

  static std::string tokenTypeToString(lexer::Token::Type type);
};

}  // namespace parser
}  // namespace config
}  // namespace infrastructure

#endif  // PARSER_CONTEXT_HPP
