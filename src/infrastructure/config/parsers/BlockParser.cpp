/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BlockParser.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/30 16:35:11 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/30 16:54:59 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "domain/configuration/entities/HttpConfig.hpp"
#include "domain/configuration/entities/LocationConfig.hpp"
#include "domain/configuration/entities/ServerConfig.hpp"
#include "infrastructure/config/exceptions/SyntaxException.hpp"
#include "infrastructure/config/parsers/BlockParser.hpp"

#include <map>
#include <sstream>

namespace infrastructure {
namespace config {
namespace parser {

BlockParser::BlockParser(application::ports::ILogger& logger)
    : m_logger(logger) {}

void BlockParser::parseHttpBlock(
    ParserContext& context,
    domain::configuration::entities::HttpConfig& httpConfig) {
  m_logger.debug("Parsing http block");

  while (context.hasMoreTokens()) {
    const lexer::Token& token = context.currentToken();

    if (token.type == lexer::Token::BLOCK_END) {
      context.popState();
      m_logger.debug("End of http block");
      break;
    }

    if (token.type == lexer::Token::EOF_T) {
      throw exceptions::SyntaxException(
          "Unexpected end of file in http block",
          exceptions::SyntaxException::UNEXPECTED_EOF);
    }

    if (token.type == lexer::Token::BLOCK_START) {
      std::string blockName = token.value;

      if (blockName == "server") {
        context.pushState(ParserState::SERVER, "server");
        parseServerBlock(context, httpConfig);
      } else {
        std::ostringstream oss;
        oss << "Unknown block '" << blockName << "' in http context";
        throw exceptions::SyntaxException(
            oss.str(), exceptions::SyntaxException::INVALID_DIRECTIVE);
      }
      continue;
    }

    if (token.type == lexer::Token::STRING) {
      parseDirective(context, token);
    } else {
      std::ostringstream oss;
      oss << "Unexpected token in http block: " << token.typeToString();
      throw exceptions::SyntaxException(
          oss.str(), exceptions::SyntaxException::UNEXPECTED_TOKEN);
    }
  }
}

void BlockParser::parseServerBlock(
    ParserContext& context,
    domain::configuration::entities::HttpConfig& httpConfig) {
  m_logger.debug("Parsing server block");

  domain::configuration::entities::ServerConfig* server =
      new domain::configuration::entities::ServerConfig();

  try {
    while (context.hasMoreTokens()) {
      const lexer::Token& token = context.currentToken();

      if (token.type == lexer::Token::BLOCK_END) {
        context.popState();

        // Validate and add server to http config
        server->validate();
        httpConfig.addServerConfig(server);

        std::ostringstream oss;
        oss << "End of server block, added server with "
            << server->getListenDirectives().size() << " listen directives";
        m_logger.debug(oss.str());
        break;
      }

      if (token.type == lexer::Token::EOF_T) {
        delete server;
        throw exceptions::SyntaxException(
            "Unexpected end of file in server block",
            exceptions::SyntaxException::UNEXPECTED_EOF);
      }

      if (token.type == lexer::Token::BLOCK_START) {
        std::string blockName = token.value;

        if (blockName == "location") {
          context.pushState(ParserState::LOCATION, "location");
          parseLocationBlock(context, *server);
        } else {
          delete server;
          std::ostringstream oss;
          oss << "Unknown block '" << blockName << "' in server context";
          throw exceptions::SyntaxException(
              oss.str(), exceptions::SyntaxException::INVALID_DIRECTIVE);
        }
        continue;
      }

      if (token.type == lexer::Token::STRING) {
        parseDirective(context, token);
      } else {
        delete server;
        std::ostringstream oss;
        oss << "Unexpected token in server block: " << token.typeToString();
        throw exceptions::SyntaxException(
            oss.str(), exceptions::SyntaxException::UNEXPECTED_TOKEN);
      }
    }

  } catch (...) {
    delete server;
    throw;
  }
}

void BlockParser::parseLocationBlock(
    ParserContext& context,
    domain::configuration::entities::ServerConfig& server) {
  // Expect location path
  std::string path = context.consumeString("location path");

  // Expect block start
  context.expect(lexer::Token::BLOCK_START, "location block");
  std::string blockName = context.currentToken().value;

  if (blockName != "location") {
    std::ostringstream oss;
    oss << "Expected location block start but got '" << blockName << "'";
    throw exceptions::SyntaxException(
        oss.str(), exceptions::SyntaxException::UNEXPECTED_TOKEN);
  }

  // Parse match type from path prefix
  domain::configuration::entities::LocationConfig::LocationMatchType matchType;
  std::string cleanPath = path;

  if (path.empty()) {
    matchType = domain::configuration::entities::LocationConfig::MATCH_PREFIX;
  } else if (path[0] == '=') {
    matchType = domain::configuration::entities::LocationConfig::MATCH_EXACT;
    cleanPath = path.substr(1);
  } else if (path[0] == '~') {
    if (path.size() > 1 && path[1] == '*') {
      matchType = domain::configuration::entities::LocationConfig::
          MATCH_REGEX_CASE_INSENSITIVE;
      cleanPath = path.substr(2);
    } else {
      matchType = domain::configuration::entities::LocationConfig::
          MATCH_REGEX_CASE_SENSITIVE;
      cleanPath = path.substr(1);
    }
  } else {
    matchType = domain::configuration::entities::LocationConfig::MATCH_PREFIX;
  }

  m_logger.debug("Parsing location block for path: " + path);

  domain::configuration::entities::LocationConfig* location =
      new domain::configuration::entities::LocationConfig(cleanPath, matchType);

  try {
    while (context.hasMoreTokens()) {
      const lexer::Token& token = context.currentToken();

      if (token.type == lexer::Token::BLOCK_END) {
        context.popState();

        // Validate and add location to server
        location->validate();
        server.addLocation(location);

        std::ostringstream oss;
        oss << "End of location block, added location for path: " << cleanPath;
        m_logger.debug(oss.str());
        break;
      }

      if (token.type == lexer::Token::EOF_T) {
        delete location;
        throw exceptions::SyntaxException(
            "Unexpected end of file in location block",
            exceptions::SyntaxException::UNEXPECTED_EOF);
      }

      if (token.type == lexer::Token::BLOCK_START) {
        // Nested location blocks (not common but possible)
        std::string nestedBlockName = token.value;

        if (nestedBlockName == "location") {
          context.pushState(ParserState::LOCATION, "location");
          parseLocationBlock(context, server);
        } else {
          delete location;
          std::ostringstream oss;
          oss << "Unknown block '" << nestedBlockName
              << "' in location context";
          throw exceptions::SyntaxException(
              oss.str(), exceptions::SyntaxException::INVALID_DIRECTIVE);
        }
        continue;
      }

      if (token.type == lexer::Token::STRING) {
        parseDirective(context, token);
      } else {
        delete location;
        std::ostringstream oss;
        oss << "Unexpected token in location block: " << token.typeToString();
        throw exceptions::SyntaxException(
            oss.str(), exceptions::SyntaxException::UNEXPECTED_TOKEN);
      }
    }

  } catch (...) {
    delete location;
    throw;
  }
}

void BlockParser::parseDirective(
    ParserContext& context, const lexer::Token& directiveToken,
    domain::configuration::entities::HttpConfig* httpConfig,
    domain::configuration::entities::ServerConfig* server,
    domain::configuration::entities::LocationConfig* location) {
  std::string directive = directiveToken.value;
  std::vector<std::string> args = collectArguments(context);

  // Expect semicolon
  context.expect(lexer::Token::SEMICOLON, "directive termination");

  ParserState currentState = context.currentState();
  std::size_t lineNumber = directiveToken.lineNumber;

  try {
    if (currentState.context == ParserState::HTTP && httpConfig != NULL) {
      handlers::GlobalDirectiveHandler handler(m_logger, *httpConfig);
      handler.handle(directive, args, lineNumber);
    } else if (currentState.context == ParserState::SERVER && server != NULL) {
      handlers::ServerDirectiveHandler handler(m_logger, *server);
      handler.handle(directive, args, lineNumber);
    } else if (currentState.context == ParserState::LOCATION &&
               location != NULL) {
      handlers::LocationDirectiveHandler handler(m_logger, *location);
      handler.handle(directive, args, lineNumber);
    } else {
      std::ostringstream oss;
      oss << "Directive '" << directive << "' not allowed in "
          << currentState.contextToString() << " context";
      throw exceptions::SyntaxException(
          oss.str(), exceptions::SyntaxException::INVALID_DIRECTIVE);
    }

    std::ostringstream oss;
    oss << "Parsed directive '" << directive << "' with " << args.size()
        << " arguments at line " << lineNumber;
    m_logger.debug(oss.str());

  } catch (const exceptions::SyntaxException&) {
    throw;
  } catch (const std::exception& e) {
    std::ostringstream oss;
    oss << "Error processing directive '" << directive << "': " << e.what()
        << " at line " << lineNumber;
    throw exceptions::SyntaxException(
        oss.str(), exceptions::SyntaxException::INVALID_DIRECTIVE);
  }
}

handlers::GlobalDirectiveHandler* BlockParser::createGlobalHandler(
    domain::configuration::entities::HttpConfig& httpConfig) {
  return new handlers::GlobalDirectiveHandler(m_logger, httpConfig);
}

handlers::ServerDirectiveHandler* BlockParser::createServerHandler(
    domain::configuration::entities::ServerConfig& server) {
  return new handlers::ServerDirectiveHandler(m_logger, server);
}

handlers::LocationDirectiveHandler* BlockParser::createLocationHandler(
    domain::configuration::entities::LocationConfig& location) {
  return new handlers::LocationDirectiveHandler(m_logger, location);
}

bool BlockParser::isAllowedInContext(const std::string& directive,
                                     ParserState::Context context) {
  // Define which directives are allowed in which contexts
  static std::map<std::string, ParserState::Context> directiveContexts;

  if (directiveContexts.empty()) {
    // Global/HTTP context directives
    directiveContexts["worker_processes"] = ParserState::GLOBAL;
    directiveContexts["worker_connections"] = ParserState::GLOBAL;
    directiveContexts["client_max_body_size"] = ParserState::GLOBAL;
    directiveContexts["error_log"] = ParserState::GLOBAL;
    directiveContexts["access_log"] = ParserState::GLOBAL;
    directiveContexts["error_page"] = ParserState::GLOBAL;
    directiveContexts["include"] = ParserState::GLOBAL;

    // Server context directives
    directiveContexts["listen"] = ParserState::SERVER;
    directiveContexts["server_name"] = ParserState::SERVER;
    directiveContexts["root"] = ParserState::SERVER;
    directiveContexts["index"] = ParserState::SERVER;
    directiveContexts["return"] = ParserState::SERVER;

    // Location context directives
    directiveContexts["alias"] = ParserState::LOCATION;
    directiveContexts["limit_except"] = ParserState::LOCATION;
    directiveContexts["autoindex"] = ParserState::LOCATION;
    directiveContexts["try_files"] = ParserState::LOCATION;
    directiveContexts["upload_store"] = ParserState::LOCATION;
    directiveContexts["upload_store_permissions"] = ParserState::LOCATION;
    directiveContexts["upload_store_access"] = ParserState::LOCATION;
    directiveContexts["script"] = ParserState::LOCATION;
    directiveContexts["cgi_root"] = ParserState::LOCATION;
    directiveContexts["fastcgi_param"] = ParserState::LOCATION;
    directiveContexts["upload_max_file_size"] = ParserState::LOCATION;
    directiveContexts["upload_max_total_size"] = ParserState::LOCATION;

    // Shared directives (allowed in multiple contexts)
    directiveContexts["root"] = ParserState::SERVER;        // Also in LOCATION
    directiveContexts["index"] = ParserState::SERVER;       // Also in LOCATION
    directiveContexts["error_page"] = ParserState::SERVER;  // Also in GLOBAL
    directiveContexts["client_max_body_size"] =
        ParserState::SERVER;                              // Also in GLOBAL
    directiveContexts["return"] = ParserState::LOCATION;  // Also in SERVER
  }

  std::map<std::string, ParserState::Context>::iterator it =
      directiveContexts.find(directive);

  if (it == directiveContexts.end()) {
    return false;  // Unknown directive
  }

  // Check if directive is allowed in current context
  ParserState::Context allowedContext = it->second;

  // Some directives are allowed in multiple contexts
  if (directive == "root" || directive == "index") {
    return context == ParserState::SERVER || context == ParserState::LOCATION;
  } else if (directive == "error_page" || directive == "client_max_body_size") {
    return context == ParserState::GLOBAL || context == ParserState::SERVER;
  } else if (directive == "return") {
    return context == ParserState::SERVER || context == ParserState::LOCATION;
  }

  return context == allowedContext;
}

std::vector<std::string> BlockParser::collectArguments(ParserContext& context) {
  std::vector<std::string> args;

  while (context.hasMoreTokens()) {
    const lexer::Token& token = context.peekToken();

    if (token.type == lexer::Token::STRING) {
      args.push_back(token.value);
      context.advance();
    } else {
      break;
    }
  }

  return args;
}

}  // namespace parser
}  // namespace config
}  // namespace infrastructure
