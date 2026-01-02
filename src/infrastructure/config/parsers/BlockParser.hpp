/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BlockParser.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/30 12:55:02 by dande-je          #+#    #+#             */
/*   Updated: 2026/01/01 17:32:01 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BLOCK_PARSER_HPP
#define BLOCK_PARSER_HPP

#include "application/ports/ILogger.hpp"
#include "domain/configuration/entities/HttpConfig.hpp"
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

  BlockParser(const BlockParser&);
  BlockParser& operator=(const BlockParser&);

  void handleNestedBlock(
      ParserContext& context, const std::string& blockName,
      domain::configuration::entities::HttpConfig* httpConfig,
      domain::configuration::entities::ServerConfig* server,
      domain::configuration::entities::LocationConfig* location);

  void handleDirective(
      ParserContext& context, const lexer::Token& directiveToken,
      domain::configuration::entities::HttpConfig* httpConfig,
      domain::configuration::entities::ServerConfig* server,
      domain::configuration::entities::LocationConfig* location);

  void handleLimitExceptBlock(
      ParserContext& context,
      domain::configuration::entities::LocationConfig& location);
};

}  // namespace parser
}  // namespace config
}  // namespace infrastructure

#endif  // BLOCK_PARSER_HPP
