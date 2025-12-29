/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/18 18:10:12 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/28 20:20:37 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_PARSER_HPP
#define CONFIG_PARSER_HPP

#include "application/ports/IConfigParser.hpp"
#include "application/ports/ILogger.hpp"
#include "domain/configuration/entities/HttpConfig.hpp"
#include "domain/configuration/entities/LocationConfig.hpp"
#include "domain/configuration/entities/ServerConfig.hpp"
#include "domain/http/value_objects/HttpMethod.hpp"

#include <stack>
#include <string>
#include <vector>

namespace infrastructure {
namespace config {
namespace parsers {

class ConfigParser : public application::ports::IConfigParser {
 public:
  explicit ConfigParser(application::ports::ILogger& logger);
  virtual ~ConfigParser();

  virtual domain::configuration::entities::HttpConfig* parseFile(
      const std::string& configPath);
  virtual void mergeIncludes(
      domain::configuration::entities::HttpConfig& httpConfig,
      const std::string& includePath);
  virtual void validateConfiguration(
      const domain::configuration::entities::HttpConfig& httpConfig);

 private:
  struct Token {
    enum Type { DIRECTIVE, BLOCK_START, BLOCK_END, STRING, SEMICOLON, EOF_T };
    Type m_type;
    std::string m_value;
    std::size_t m_numLine;

    explicit Token(Type type, const std::string& value, std::size_t numLine);
  };

  struct ParserState {
    enum Context { GLOBAL, HTTP, SERVER, LOCATION };
    Context context;
    std::string blockName;

    ParserState(Context ctx, const std::string& name = "")
        : context(ctx), blockName(name) {}
  };

  application::ports::ILogger& m_logger;
  std::vector<Token> m_tokens;
  std::size_t m_indexToken;
  std::stack<ParserState> m_stateStack;
  std::string m_currentFilePath;

  static const std::size_t K_MAX_INCLUDE_DEPTH = 10;

  // Lexical analysis
  void lexFile(const std::string& path);
  static void skipWhiteSpace(const std::string& line, std::size_t& col);
  bool handleSingleCharToken(const std::string& line, std::size_t& col,
                             std::size_t numLine);
  void extractWordToken(const std::string& line, std::size_t& col,
                        std::size_t numLine);
  void handleAttachedSpecialChar(const std::string& line, std::size_t& col,
                                 std::size_t numLine);

  // Parser methods
  Token nextToken();
  Token peekToken();
  void expect(Token::Type type, const std::string& context);

  // Block parsing
  void parseHttpBlock(domain::configuration::entities::HttpConfig& httpConfig);
  void parseServerBlock(
      domain::configuration::entities::HttpConfig& httpConfig);
  void parseLocationBlock(
      domain::configuration::entities::ServerConfig& server);

  // Directive handlers
  void handleGlobalDirective(
      domain::configuration::entities::HttpConfig& httpConfig,
      const std::string& directive, const std::vector<std::string>& args);
  void handleServerDirective(
      domain::configuration::entities::ServerConfig& server,
      const std::string& directive, const std::vector<std::string>& args);
  void handleLocationDirective(
      domain::configuration::entities::LocationConfig& location,
      const std::string& directive, const std::vector<std::string>& args);

  // Helper methods
  void processInclude(const std::string& pattern,
                      domain::configuration::entities::HttpConfig& httpConfig,
                      std::size_t depth);
  static std::string tokenTypeToString(Token::Type type);
  static std::vector<std::string> expandGlob(const std::string& pattern);

  // Value parsing
  static domain::http::value_objects::HttpMethod parseHttpMethod(
      const std::string& method);
  static domain::configuration::entities::LocationConfig::LocationMatchType
  parseLocationMatchType(const std::string& path);
  static std::string normalizePath(const std::string& path);
};

}  // namespace parsers
}  // namespace config
}  // namespace infrastructure

#endif  // CONFIG_PARSER_HPP
