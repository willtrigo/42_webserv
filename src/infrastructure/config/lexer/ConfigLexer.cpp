/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigLexer.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/30 14:25:44 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/30 18:23:00 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "infrastructure/config/exceptions/ParserException.hpp"
#include "infrastructure/config/exceptions/SyntaxException.hpp"
#include "infrastructure/config/lexer/ConfigLexer.hpp"

#include <cctype>
#include <fstream>
#include <sstream>

namespace infrastructure {
namespace config {
namespace lexer {

ConfigLexer::ConfigLexer(application::ports::ILogger& logger)
    : m_logger(logger) {
  m_logger.debug("ConfigLexer initialized.");
}

std::vector<Token> ConfigLexer::tokenizeFile(const std::string& filePath) {
  std::ifstream file(filePath.c_str());
  if (!file.is_open()) {
    throw exceptions::ParserException(
        "Cannot open config file: " + filePath,
        exceptions::ParserException::FILE_NOT_FOUND);
  }

  std::vector<Token> tokens;
  std::string line;
  std::size_t lineNumber = 0;

  while (std::getline(file, line) != 0) {
    ++lineNumber;
    processLine(line, lineNumber, tokens);
  }

  tokens.push_back(Token(Token::EOF_T, "", lineNumber));
  validateTokens(tokens);

  std::ostringstream oss;
  oss << "Tokenized " << tokens.size() << " tokens from " << filePath;
  m_logger.debug(oss.str());
  currentTokens(tokens);

  return tokens;
}

std::vector<Token> ConfigLexer::tokenizeString(const std::string& content) {
  std::vector<Token> tokens;
  std::istringstream stream(content);
  std::string line;
  std::size_t lineNumber = 0;

  while (std::getline(stream, line) != 0) {
    ++lineNumber;
    processLine(line, lineNumber, tokens);
  }

  tokens.push_back(Token(Token::EOF_T, "", lineNumber));
  validateTokens(tokens);

  std::ostringstream oss;
  oss << "Tokenized " << tokens.size() << " tokens from string content";
  m_logger.debug(oss.str());

  return tokens;
}

void ConfigLexer::currentTokens(std::vector<Token>& tokens) {
  std::ostringstream oss;
  oss << "Current tokens (" << tokens.size() << "):\n";

  for (std::vector<Token>::const_iterator it = tokens.begin();
       it != tokens.end(); ++it) {
    oss << "  [Line " << it->lineNumber << "] ";

    switch (it->type) {
      case Token::BLOCK_START:
        oss << "BLOCK_START: '" << it->value << "'";
        break;
      case Token::BLOCK_END:
        oss << "BLOCK_END: '" << it->value << "'";
        break;
      case Token::SEMICOLON:
        oss << "SEMICOLON: '" << it->value << "'";
        break;
      case Token::STRING:
        oss << "STRING: '" << it->value << "'";
        break;
      case Token::EOF_T:
        oss << "EOF";
        break;
      default:
        oss << "UNKNOWN: '" << it->value << "'";
        break;
    }

    oss << "\n";
  }

  m_logger.debug(oss.str());
}

void ConfigLexer::processLine(const std::string& line, std::size_t lineNumber,
                              std::vector<Token>& tokens) {
  std::size_t col = 0;
  const std::size_t lineLength = line.length();

  skipWhitespace(line, col);

  if (col < lineLength && line[col] == '#') {
    return;
  }

  while (col < lineLength) {
    skipWhitespace(line, col);
    if (col >= lineLength) break;

    if (handleSpecialCharacter(line[col], lineNumber, tokens)) {
      ++col;
      continue;
    }

    extractWord(line, col, lineNumber, tokens);
  }
}

void ConfigLexer::skipWhitespace(const std::string& line, std::size_t& col) {
  const std::size_t length = line.length();
  while (col < length &&
         (std::isspace(static_cast<unsigned char>(line[col])) != 0)) {
    ++col;
  }
}

bool ConfigLexer::handleSpecialCharacter(char chr, std::size_t lineNumber,
                                         std::vector<Token>& tokens) {
  switch (chr) {
    case '{':
      tokens.push_back(Token(Token::BLOCK_START, "{", lineNumber));
      return true;
    case '}':
      tokens.push_back(Token(Token::BLOCK_END, "}", lineNumber));
      return true;
    case ';':
      tokens.push_back(Token(Token::SEMICOLON, ";", lineNumber));
      return true;
    default:
      return false;
  }
}

void ConfigLexer::extractWord(const std::string& line, std::size_t& col,
                              std::size_t lineNumber,
                              std::vector<Token>& tokens) {
  const std::size_t start = col;
  const std::size_t length = line.length();

  if (col < length && isQuote(line[col])) {
    char quote = line[col];
    ++col;

    while (col < length && line[col] != quote) {
      if (line[col] == '\\' && col + 1 < length) {
        ++col;
      }
      ++col;
    }

    if (col >= length) {
      std::ostringstream oss;
      oss << "Unterminated quoted string at line " << lineNumber;
      throw exceptions::SyntaxException(
          oss.str(), exceptions::SyntaxException::UNEXPECTED_EOF);
    }

    ++col;
    std::string value = line.substr(start + 1, col - start - 2);
    tokens.push_back(Token(Token::STRING, value, lineNumber));
    return;
  }

  while (col < length &&
         (std::isspace(static_cast<unsigned char>(line[col])) == 0) &&
         !isSpecialCharacter(line[col])) {
    ++col;
  }

  std::string value = line.substr(start, col - start);
  if (!value.empty()) {
    tokens.push_back(Token(Token::STRING, value, lineNumber));
  }
}

void ConfigLexer::validateTokens(const std::vector<Token>& tokens) {
  int braceBalance = 0;

  for (std::vector<Token>::const_iterator it = tokens.begin();
       it != tokens.end(); ++it) {
    if (it->type == Token::BLOCK_START) {
      ++braceBalance;
    } else if (it->type == Token::BLOCK_END) {
      --braceBalance;
      if (braceBalance < 0) {
        std::ostringstream oss;
        oss << "Unexpected '}' at line " << it->lineNumber;
        throw exceptions::SyntaxException(
            oss.str(), exceptions::SyntaxException::UNEXPECTED_TOKEN);
      }
    }
  }

  if (braceBalance > 0) {
    std::ostringstream oss;
    oss << "Unclosed block (missing '}') at line " << tokens.back().lineNumber;
    throw exceptions::SyntaxException(
        oss.str(), exceptions::SyntaxException::MISSING_BRACE);
  }
}

bool ConfigLexer::isSpecialCharacter(char chr) {
  return chr == '{' || chr == '}' || chr == ';';
}

bool ConfigLexer::isQuote(char chr) { return chr == '"' || chr == '\''; }

}  // namespace lexer
}  // namespace config
}  // namespace infrastructure
