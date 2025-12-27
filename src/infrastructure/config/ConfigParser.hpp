/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/18 18:10:12 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/26 23:01:23 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_PARSER_HPP
#define CONFIG_PARSER_HPP

#include "application/ports/IConfigParser.hpp"
#include "application/ports/ILogger.hpp"
#include "domain/entities/LocationConfig.hpp"
#include "domain/entities/ServerConfig.hpp"

#include <map>
#include <string>
#include <vector>

namespace infrastructure {
namespace config {

class ConfigParser : public application::ports::IConfigParser {
 public:
  explicit ConfigParser(application::ports::ILogger& logger);
  ConfigParser(const ConfigParser& other);
  virtual ~ConfigParser();

  ConfigParser& operator=(const ConfigParser& other);

  virtual void parseFile(const std::string& configPath);
  virtual void mergeIncludes(const std::string& includePath);

 private:
  struct Token {
    enum Type { DIRECTIVE, BLOCK_START, BLOCK_END, STRING, SEMICOLON, EOF_T };

    Type m_type;
    std::string m_value;
    std::size_t m_numLine;

    explicit Token(Type type, std::string value, std::size_t numLine);
  };

  application::ports::ILogger& m_logger;
  std::vector<Token> m_tokens;
  std::size_t m_indexToken;
  std::map<std::string, std::string> m_globalDirectives;

  static const std::size_t K_MAX_INCLUDE_DEPTH = 10;

  void lexFile(const std::string& path);
  static void skipWhiteSpace(const std::string& line, std::size_t& col);
  bool handleSingleCharToken(const std::string& line, std::size_t& col,
                             std::size_t numLine);
  void extractWordToken(const std::string& line, std::size_t& col,
                        std::size_t numLine);
  void handleAttachedSpecialChar(const std::string& line, std::size_t& col,
                                 std::size_t numLine);
  Token nextToken();
  void expect(Token::Type type, const std::string& context);

  void parseGlobal();
  void parseServerBlock(domain::entities::ServerConfig& server);
  void parseLocationBlock(domain::entities::LocationConfig& location);

  static std::string tokenTypeToString(Token::Type type);
};

}  // namespace config
}  // namespace infrastructure

#endif  // CONFIG_PARSER_HPP
