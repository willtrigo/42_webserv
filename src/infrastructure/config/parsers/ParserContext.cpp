/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParserContext.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/30 14:47:24 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/30 17:50:47 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "infrastructure/config/exceptions/ParserException.hpp"
#include "infrastructure/config/exceptions/SyntaxException.hpp"
#include "infrastructure/config/parsers/ParserContext.hpp"

#include <sstream>

namespace infrastructure {
namespace config {
namespace parser {

ParserContext::ParserContext(const std::vector<lexer::Token>& tokens,
                             const std::string& sourceFile)
    : m_tokens(tokens), m_currentIndex(0), m_sourceFile(sourceFile) {
  m_stateStack.push(ParserState(ParserState::GLOBAL, "", 0));
}

const lexer::Token& ParserContext::currentToken() const {
  if (m_currentIndex >= m_tokens.size()) {
    throw exceptions::ParserException(
        "No more tokens available",
        exceptions::ParserException::UNEXPECTED_ERROR);
  }
  return m_tokens[m_currentIndex];
}

const lexer::Token& ParserContext::peekToken() const {
  std::size_t nextIndex = m_currentIndex + 1;
  if (nextIndex >= m_tokens.size()) {
    throw exceptions::ParserException(
        "Cannot peek beyond available tokens",
        exceptions::ParserException::UNEXPECTED_ERROR);
  }
  return m_tokens[nextIndex];
}

bool ParserContext::hasMoreTokens() const {
  if (m_currentIndex >= m_tokens.size()) {
    return false;
  }
  return m_tokens[m_currentIndex].type != lexer::Token::EOF_T;
}

void ParserContext::advance() {
  if (m_currentIndex >= m_tokens.size()) {
    throw exceptions::ParserException(
        "Cannot advance beyond token list",
        exceptions::ParserException::UNEXPECTED_ERROR);
  }
  ++m_currentIndex;
}

void ParserContext::expect(lexer::Token::Type type,
                           const std::string& context) const {
  if (!hasMoreTokens()) {
    std::ostringstream oss;
    oss << "Unexpected end of file " << context;
    throw exceptions::SyntaxException(
        oss.str(), exceptions::SyntaxException::UNEXPECTED_EOF);
  }

  const lexer::Token& token = currentToken();
  if (token.type != type) {
    std::ostringstream oss;
    oss << "Expected " << tokenTypeToString(type) << " but found "
        << token.typeToString() << " at line " << token.lineNumber << " "
        << context;

    throw exceptions::SyntaxException(
        oss.str(), exceptions::SyntaxException::UNEXPECTED_TOKEN);
  }
}

void ParserContext::expectString(const std::string& context) const {
  expect(lexer::Token::STRING, context);
}

std::string ParserContext::consumeString(const std::string& context) {
  expectString(context);
  std::string value = currentToken().value;
  advance();
  return value;
}

void ParserContext::pushState(ParserState::Context context,
                              const std::string& name) {
  std::size_t line = currentToken().lineNumber;
  m_stateStack.push(ParserState(context, name, line));
}

void ParserContext::popState() {
  if (m_stateStack.size() <= 1) {
    throw exceptions::ParserException(
        "Cannot pop the global state",
        exceptions::ParserException::UNEXPECTED_ERROR);
  }
  m_stateStack.pop();
}

ParserState ParserContext::currentState() const {
  if (m_stateStack.empty()) {
    throw exceptions::ParserException(
        "Parser state stack is empty",
        exceptions::ParserException::UNEXPECTED_ERROR);
  }
  return m_stateStack.top();
}

std::size_t ParserContext::currentLine() const {
  if (!hasMoreTokens()) {
    if (m_tokens.empty()) {
      return 0;
    }
    return m_tokens.back().lineNumber;
  }
  return currentToken().lineNumber;
}

std::string ParserContext::sourceFile() const { return m_sourceFile; }

std::string ParserContext::currentContextDescription() const {
  ParserState state = currentState();
  std::ostringstream oss;

  oss << "in " << state.contextToString() << " context";
  if (!state.blockName.empty()) {
    oss << " (" << state.blockName << ")";
  }
  if (state.startLine > 0) {
    oss << " starting at line " << state.startLine;
  }

  return oss.str();
}

std::string ParserContext::tokenTypeToString(lexer::Token::Type type) {
  switch (type) {
    case lexer::Token::DIRECTIVE:
      return "directive";
    case lexer::Token::BLOCK_START:
      return "block start '{'";
    case lexer::Token::BLOCK_END:
      return "block end '}'";
    case lexer::Token::STRING:
      return "string";
    case lexer::Token::SEMICOLON:
      return "semicolon ';'";
    case lexer::Token::EOF_T:
      return "end of file";
    default:
      return "unknown token";
  }
}

}  // namespace parser
}  // namespace config
}  // namespace infrastructure
