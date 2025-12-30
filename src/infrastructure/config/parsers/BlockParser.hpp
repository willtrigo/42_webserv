/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BlockParser.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/30 12:55:02 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/30 16:57:19 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BLOCK_PARSER_HPP
#define BLOCK_PARSER_HPP

#include "application/ports/ILogger.hpp"
#include "infrastructure/config/handlers/GlobalDirectiveHandler.hpp"
#include "infrastructure/config/handlers/LocationDirectiveHandler.hpp"
#include "infrastructure/config/handlers/ServerDirectiveHandler.hpp"
#include "infrastructure/config/parsers/ParserContext.hpp"

namespace infrastructure {
namespace config {
namespace parser {

class BlockParser {
 public:
  explicit BlockParser(application::ports::ILogger& logger);

  void parseHttpBlock(ParserContext& context,
                      domain::configuration::entities::HttpConfig& httpConfig);
  void parseServerBlock(
      ParserContext& context,
      domain::configuration::entities::HttpConfig& httpConfig);
  void parseLocationBlock(
      ParserContext& context,
      domain::configuration::entities::ServerConfig& server);

 private:
  application::ports::ILogger& m_logger;

  void parseDirective(
      ParserContext& context, const lexer::Token& directiveToken,
      domain::configuration::entities::HttpConfig* httpConfig = NULL,
      domain::configuration::entities::ServerConfig* server = NULL,
      domain::configuration::entities::LocationConfig* location = NULL);
  void parseNestedBlocks(
      ParserContext& context,
      domain::configuration::entities::HttpConfig& httpConfig,
      ParserState::Context allowedContext);

  handlers::GlobalDirectiveHandler* createGlobalHandler(
      domain::configuration::entities::HttpConfig& httpConfig);
  handlers::ServerDirectiveHandler* createServerHandler(
      domain::configuration::entities::ServerConfig& server);
  handlers::LocationDirectiveHandler* createLocationHandler(
      domain::configuration::entities::LocationConfig& location);

  static bool isAllowedInContext(const std::string& directive,
                                 ParserState::Context context);
  static std::vector<std::string> collectArguments(ParserContext& context);
};

}  // namespace parser
}  // namespace config
}  // namespace infrastructure

#endif  // BLOCK_PARSER_HPP
