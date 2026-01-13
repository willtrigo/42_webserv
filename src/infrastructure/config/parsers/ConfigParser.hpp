/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/18 18:10:12 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/30 20:35:32 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_PARSER_HPP
#define CONFIG_PARSER_HPP

#include "application/ports/IConfigParser.hpp"
#include "application/ports/ILogger.hpp"
#include "infrastructure/config/lexer/ConfigLexer.hpp"
#include "infrastructure/config/parsers/BlockParser.hpp"
#include "infrastructure/config/parsers/ParserContext.hpp"

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
  application::ports::ILogger& m_logger;
  lexer::ConfigLexer m_lexer;
  parser::BlockParser m_blockParser;

  void initializeParser(const std::string& configPath);
  void parseTokens(parser::ParserContext& context,
                   domain::configuration::entities::HttpConfig& httpConfig);
  void handleBlockDeclaration(
      parser::ParserContext& context,
      domain::configuration::entities::HttpConfig& httpConfig,
      const std::string& blockName);

  void handleSingleDirective(
      parser::ParserContext& context,
      domain::configuration::entities::HttpConfig& httpConfig);
};

}  // namespace parsers
}  // namespace config
}  // namespace infrastructure

#endif  // CONFIG_PARSER_HPP
