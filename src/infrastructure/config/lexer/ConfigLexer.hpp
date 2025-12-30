/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigLexer.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/30 12:47:09 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/30 14:30:38 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_LEXER_HPP
#define CONFIG_LEXER_HPP

#include "application/ports/ILogger.hpp"
#include "infrastructure/config/lexer/Token.hpp"

#include <string>
#include <vector>

namespace infrastructure {
namespace config {
namespace lexer {

class ConfigLexer {
 public:
  explicit ConfigLexer(application::ports::ILogger& logger);

  std::vector<Token> tokenizeFile(const std::string& filePath);
  std::vector<Token> tokenizeString(const std::string& content);

 private:
  application::ports::ILogger& m_logger;

  static void processLine(const std::string& line, std::size_t lineNumber,
                   std::vector<Token>& tokens);
  static void skipWhitespace(const std::string& line, std::size_t& col);
  static bool handleSpecialCharacter(char chr, std::size_t lineNumber,
                              std::vector<Token>& tokens);
  static void extractWord(const std::string& line, std::size_t& col,
                   std::size_t lineNumber, std::vector<Token>& tokens);
  static void validateTokens(const std::vector<Token>& tokens);

  static bool isSpecialCharacter(char chr);
  static bool isQuote(char chr);
};

}  // namespace lexer
}  // namespace config
}  // namespace infrastructure

#endif  // CONFIG_LEXER_HPP
