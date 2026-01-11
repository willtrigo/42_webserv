/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BlockParser.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/30 16:35:11 by dande-je          #+#    #+#             */
/*   Updated: 2026/01/11 16:52:15 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "domain/configuration/entities/HttpConfig.hpp"
#include "domain/configuration/entities/LocationConfig.hpp"
#include "domain/configuration/entities/ServerConfig.hpp"
#include "infrastructure/config/exceptions/SyntaxException.hpp"
#include "infrastructure/config/handlers/GlobalDirectiveHandler.hpp"
#include "infrastructure/config/handlers/LocationDirectiveHandler.hpp"
#include "infrastructure/config/handlers/ServerDirectiveHandler.hpp"
#include "infrastructure/config/parsers/BlockParser.hpp"

#include <sstream>

namespace infrastructure {
namespace config {
namespace parser {

BlockParser::BlockParser(application::ports::ILogger& logger)
    : m_logger(logger) {}

void BlockParser::parseHttpBlock(
    ParserContext& context,
    domain::configuration::entities::HttpConfig& httpConfig) {
  m_logger.debug("Parsing http block content");

  while (context.hasMoreTokens()) {
    const lexer::Token& token = context.currentToken();

    if (token.type == lexer::Token::BLOCK_END) {
      context.advance();
      context.popState();
      m_logger.debug("End of http block");
      return;
    }

    if (token.type == lexer::Token::EOF_T) {
      throw exceptions::SyntaxException(
          "Unexpected end of file in http block",
          exceptions::SyntaxException::UNEXPECTED_EOF);
    }

    if (token.type == lexer::Token::STRING) {
      std::string tokenValue = token.value;

      if (context.currentIndex() + 1 < context.tokenCount()) {
        const lexer::Token& nextToken = context.peekToken();

        if (nextToken.type == lexer::Token::BLOCK_START) {
          handleNestedBlock(context, tokenValue, &httpConfig, NULL, NULL);
        } else {
          handleDirective(context, token, &httpConfig, NULL, NULL);
        }
      } else {
        std::ostringstream oss;
        oss << "Unexpected end of file after token '" << tokenValue
            << "' in http block";
        throw exceptions::SyntaxException(
            oss.str(), exceptions::SyntaxException::UNEXPECTED_EOF);
      }
    } else {
      std::ostringstream oss;
      oss << "Unexpected token in http block: " << token.typeToString()
          << " at line " << token.lineNumber;
      throw exceptions::SyntaxException(
          oss.str(), exceptions::SyntaxException::UNEXPECTED_TOKEN);
    }
  }

  throw exceptions::SyntaxException("Http block not properly closed",
                                    exceptions::SyntaxException::MISSING_BRACE);
}

void BlockParser::parseServerBlock(
    ParserContext& context,
    domain::configuration::entities::HttpConfig& httpConfig) {
  m_logger.debug("Parsing server block content");

  domain::configuration::entities::ServerConfig* server =
      new domain::configuration::entities::ServerConfig();

  try {
    server->setClientMaxBodySize(httpConfig.getClientMaxBodySize());

    const domain::configuration::entities::HttpConfig::ErrorPagesMap&
        httpErrorPages = httpConfig.getErrorPages();
    for (domain::configuration::entities::HttpConfig::ErrorPagesMap::
             const_iterator it = httpErrorPages.begin();
         it != httpErrorPages.end(); ++it) {
      domain::shared::value_objects::ErrorCode errorCode(it->first);
      server->addErrorPage(errorCode, it->second);
    }

    while (context.hasMoreTokens()) {
      const lexer::Token& token = context.currentToken();

      if (token.type == lexer::Token::BLOCK_END) {
        context.advance();
        context.popState();

        server->validate();
        httpConfig.addServerConfig(server);

        std::ostringstream oss;
        oss << "End of server block, added server with "
            << server->getListenDirectives().size() << " listen directives";
        m_logger.debug(oss.str());
        return;
      }

      if (token.type == lexer::Token::EOF_T) {
        delete server;
        throw exceptions::SyntaxException(
            "Unexpected end of file in server block",
            exceptions::SyntaxException::UNEXPECTED_EOF);
      }

      if (token.type == lexer::Token::STRING) {
        std::string tokenValue = token.value;

        if (tokenValue == "location") {
          context.advance();
          context.pushState(ParserState::LOCATION, "location");
          parseLocationBlock(context, *server);
        } else if (context.currentIndex() + 1 < context.tokenCount()) {
          const lexer::Token& nextToken = context.peekToken();

          if (nextToken.type == lexer::Token::BLOCK_START) {
            handleNestedBlock(context, tokenValue, NULL, server, NULL);
          } else {
            handleDirective(context, token, NULL, server, NULL);
          }
        } else {
          delete server;
          std::ostringstream oss;
          oss << "Unexpected end of file after token '" << tokenValue
              << "' in server block";
          throw exceptions::SyntaxException(
              oss.str(), exceptions::SyntaxException::UNEXPECTED_EOF);
        }
      } else {
        delete server;
        std::ostringstream oss;
        oss << "Unexpected token in server block: " << token.typeToString()
            << " at line " << token.lineNumber;
        throw exceptions::SyntaxException(
            oss.str(), exceptions::SyntaxException::UNEXPECTED_TOKEN);
      }
    }

    delete server;
    throw exceptions::SyntaxException(
        "Server block not properly closed",
        exceptions::SyntaxException::MISSING_BRACE);

  } catch (...) {
    delete server;
    throw;
  }
}

void BlockParser::parseLocationBlock(
    ParserContext& context,
    domain::configuration::entities::ServerConfig& server) {
  m_logger.debug("Parsing location block content");

  const lexer::Token& firstToken = context.currentToken();
  std::string firstArg = firstToken.value;

  std::string path;
  domain::configuration::entities::LocationConfig::LocationMatchType matchType =
      domain::configuration::entities::LocationConfig::MATCH_PREFIX;

  if (!firstArg.empty() && firstArg[0] == '@') {
    path = firstArg;
    matchType = domain::configuration::entities::LocationConfig::MATCH_EXACT;
    context.advance();
  } else if (firstArg == "=" || firstArg == "~" || firstArg == "~*" ||
             firstArg == "^~") {
    context.advance();

    if (!context.hasMoreTokens() ||
        context.currentToken().type != lexer::Token::STRING) {
      std::ostringstream oss;
      oss << "Expected path after location modifier '" << firstArg
          << "' at line " << firstToken.lineNumber;
      throw exceptions::SyntaxException(
          oss.str(), exceptions::SyntaxException::UNEXPECTED_TOKEN);
    }

    path = context.currentToken().value;

    if (firstArg == "=") {
      matchType = domain::configuration::entities::LocationConfig::MATCH_EXACT;
    } else if (firstArg == "~") {
      matchType = domain::configuration::entities::LocationConfig::
          MATCH_REGEX_CASE_SENSITIVE;
    } else if (firstArg == "~*") {
      matchType = domain::configuration::entities::LocationConfig::
          MATCH_REGEX_CASE_INSENSITIVE;
    } else if (firstArg == "^~") {
      matchType =
          domain::configuration::entities::LocationConfig::MATCH_PREFIX;
    }

    context.advance();
  } else {
    path = firstArg;
    matchType = domain::configuration::entities::LocationConfig::MATCH_PREFIX;
    context.advance();
  }

  std::ostringstream debugMsg;
  debugMsg << "Parsing location block for path: " << path;
  m_logger.debug(debugMsg.str());

  if (!context.hasMoreTokens() ||
      context.currentToken().type != lexer::Token::BLOCK_START) {
    std::ostringstream oss;
    oss << "Expected '{' after location path '" << path << "' at line "
        << firstToken.lineNumber;
    throw exceptions::SyntaxException(
        oss.str(), exceptions::SyntaxException::MISSING_BRACE);
  }

  context.advance();

  domain::configuration::entities::LocationConfig* location =
      new domain::configuration::entities::LocationConfig(path, matchType);

  try {
    location->setClientMaxBodySize(server.getClientMaxBodySize());

    // Also inherit server-level error pages
    const domain::configuration::entities::ServerConfig::ErrorPageMap&
        serverErrorPages = server.getErrorPages();
    for (domain::configuration::entities::ServerConfig::ErrorPageMap::
             const_iterator it = serverErrorPages.begin();
         it != serverErrorPages.end(); ++it) {
      location->addErrorPage(it->first, it->second);
    }

    // Also inherit server-level root and index files if not set in location
    location->setRoot(server.getRoot());
    const std::vector<std::string>& serverIndexFiles = server.getIndexFiles();
    if (!serverIndexFiles.empty()) {
      location->clearIndexFiles();
      for (std::size_t i = 0; i < serverIndexFiles.size(); ++i) {
        location->addIndexFile(serverIndexFiles[i]);
      }
    }

    // Parse location block content
    while (context.hasMoreTokens()) {
      const lexer::Token& token = context.currentToken();

      if (token.type == lexer::Token::BLOCK_END) {
        context.advance();
        context.popState();

        location->validate();
        server.addLocation(location);

        std::ostringstream oss;
        oss << "End of location block, added location for path: " << path;
        m_logger.debug(oss.str());
        return;
      }

      if (token.type == lexer::Token::EOF_T) {
        delete location;
        throw exceptions::SyntaxException(
            "Unexpected end of file in location block",
            exceptions::SyntaxException::UNEXPECTED_EOF);
      }

      if (token.type == lexer::Token::STRING) {
        std::string tokenValue = token.value;

        if (tokenValue == "limit_except") {
          handleNestedBlock(context, tokenValue, NULL, NULL, location);
        } else if (context.currentIndex() + 1 < context.tokenCount()) {
          const lexer::Token& nextToken = context.peekToken();

          if (nextToken.type == lexer::Token::BLOCK_START) {
            handleNestedBlock(context, tokenValue, NULL, NULL, location);
          } else {
            handleDirective(context, token, NULL, NULL, location);
          }
        } else {
          delete location;
          std::ostringstream oss;
          oss << "Unexpected end of file after token '" << tokenValue
              << "' in location block";
          throw exceptions::SyntaxException(
              oss.str(), exceptions::SyntaxException::UNEXPECTED_EOF);
        }
      } else {
        delete location;
        std::ostringstream oss;
        oss << "Unexpected token in location block: " << token.typeToString()
            << " at line " << token.lineNumber;
        throw exceptions::SyntaxException(
            oss.str(), exceptions::SyntaxException::UNEXPECTED_TOKEN);
      }
    }

    delete location;
    throw exceptions::SyntaxException(
        "Location block not properly closed",
        exceptions::SyntaxException::MISSING_BRACE);

  } catch (...) {
    delete location;
    throw;
  }
}

void BlockParser::handleNestedBlock(
    ParserContext& context, const std::string& blockName,
    domain::configuration::entities::HttpConfig* httpConfig,
    domain::configuration::entities::ServerConfig* server,
    domain::configuration::entities::LocationConfig* location) {
  std::size_t lineNumber = context.currentToken().lineNumber;

  if (blockName == "server" && httpConfig != NULL) {
    context.advance();
    context.expect(lexer::Token::BLOCK_START, "block start");
    context.advance();
    context.pushState(ParserState::SERVER, "server");
    parseServerBlock(context, *httpConfig);
  } else if (blockName == "location" && server != NULL) {
    std::ostringstream oss;
    oss << "Location blocks should be handled specially, not as nested blocks "
           "at line "
        << lineNumber;
    throw exceptions::SyntaxException(
        oss.str(), exceptions::SyntaxException::INVALID_DIRECTIVE);
  } else if (blockName == "limit_except" && location != NULL) {
    handleLimitExceptBlock(context, *location);
  } else {
    std::ostringstream oss;
    oss << "Unknown or misplaced block type '" << blockName << "' at line "
        << lineNumber;
    throw exceptions::SyntaxException(
        oss.str(), exceptions::SyntaxException::INVALID_DIRECTIVE);
  }
}

void BlockParser::handleDirective(
    ParserContext& context, const lexer::Token& directiveToken,
    domain::configuration::entities::HttpConfig* httpConfig,
    domain::configuration::entities::ServerConfig* server,
    domain::configuration::entities::LocationConfig* location) {
  std::string directive = directiveToken.value;
  std::size_t lineNumber = directiveToken.lineNumber;

  context.advance();

  std::vector<std::string> args;

  while (context.hasMoreTokens()) {
    const lexer::Token& token = context.currentToken();

    if (token.type == lexer::Token::SEMICOLON) {
      context.advance();
      break;
    }
    if (token.type == lexer::Token::STRING) {
      args.push_back(token.value);
      context.advance();
    } else if (token.type == lexer::Token::BLOCK_START) {
      std::ostringstream oss;
      oss << "Unexpected block start in directive '" << directive
          << "' at line " << token.lineNumber
          << ". Did you mean to declare a block?";
      throw exceptions::SyntaxException(
          oss.str(), exceptions::SyntaxException::UNEXPECTED_TOKEN);
    } else if (token.type == lexer::Token::BLOCK_END) {
      std::ostringstream oss;
      oss << "Unexpected block end in directive '" << directive
          << "' at line " << token.lineNumber
          << ". Expected semicolon to terminate directive.";
      throw exceptions::SyntaxException(
          oss.str(), exceptions::SyntaxException::UNEXPECTED_TOKEN);
    } else {
      std::ostringstream oss;
      oss << "Expected argument or semicolon after directive '" << directive
          << "' at line " << token.lineNumber << ", but found "
          << token.typeToString();
      throw exceptions::SyntaxException(
          oss.str(), exceptions::SyntaxException::UNEXPECTED_TOKEN);
    }
  }

  if (httpConfig != NULL) {
    handlers::GlobalDirectiveHandler handler(m_logger, *httpConfig);
    handler.handle(directive, args, lineNumber);
  } else if (server != NULL) {
    handlers::ServerDirectiveHandler handler(m_logger, *server);
    handler.handle(directive, args, lineNumber);
  } else if (location != NULL) {
    handlers::LocationDirectiveHandler handler(m_logger, *location);
    handler.handle(directive, args, lineNumber);
  }

  std::ostringstream oss;
  oss << "Processed directive '" << directive << "' with " << args.size()
      << " arguments at line " << lineNumber;
  m_logger.debug(oss.str());
}

void BlockParser::handleLimitExceptBlock(
    ParserContext& context,
    domain::configuration::entities::LocationConfig& location) {
  const lexer::Token& limitExceptToken = context.currentToken();
  std::size_t lineNumber = limitExceptToken.lineNumber;

  context.advance();

  std::vector<std::string> methods;
  while (context.hasMoreTokens()) {
    const lexer::Token& token = context.currentToken();

    if (token.type == lexer::Token::BLOCK_START) {
      break;
    }
    if (token.type == lexer::Token::STRING) {
      methods.push_back(token.value);
      context.advance();
    } else {
      std::ostringstream oss;
      oss << "Expected HTTP method or '{' in limit_except at line "
          << token.lineNumber;
      throw exceptions::SyntaxException(
          oss.str(), exceptions::SyntaxException::UNEXPECTED_TOKEN);
    }
  }

  if (methods.empty()) {
    std::ostringstream oss;
    oss << "limit_except requires at least one HTTP method at line "
        << lineNumber;
    throw exceptions::SyntaxException(
        oss.str(), exceptions::SyntaxException::INVALID_DIRECTIVE);
  }

  context.expect(lexer::Token::BLOCK_START, "limit_except block start");
  context.advance();

  location.clearAllowedMethods();

  for (std::size_t i = 0; i < methods.size(); ++i) {
    try {
      domain::http::value_objects::HttpMethod method(methods[i]);
      location.addAllowedMethod(method);

      std::ostringstream oss;
      oss << "Added allowed method '" << methods[i]
          << "' via limit_except at line " << lineNumber;
      m_logger.debug(oss.str());

    } catch (const std::exception& e) {
      std::ostringstream oss;
      oss << "Invalid HTTP method in limit_except: '" << methods[i]
          << "': " << e.what() << " at line " << lineNumber;
      throw exceptions::SyntaxException(
          oss.str(), exceptions::SyntaxException::INVALID_DIRECTIVE);
    }
  }

  while (context.hasMoreTokens()) {
    const lexer::Token& token = context.currentToken();

    if (token.type == lexer::Token::BLOCK_END) {
      context.advance();

      std::ostringstream oss;
      oss << "Completed limit_except block with " << methods.size()
          << " allowed methods";
      m_logger.debug(oss.str());
      return;
    }

    if (token.type == lexer::Token::STRING) {
      std::string word = token.value;
      context.advance();

      if (word == "deny") {
        if (context.hasMoreTokens() &&
            context.currentToken().type == lexer::Token::STRING &&
            context.currentToken().value == "all") {
          context.advance();
          context.expect(lexer::Token::SEMICOLON, "deny all statement");
          context.advance();
        } else {
          std::ostringstream oss;
          oss << "Expected 'all' after 'deny' in limit_except block at line "
              << token.lineNumber;
          throw exceptions::SyntaxException(
              oss.str(), exceptions::SyntaxException::UNEXPECTED_TOKEN);
        }
      } else {
        std::ostringstream oss;
        oss << "Unknown statement '" << word
            << "' in limit_except block at line " << token.lineNumber;
        m_logger.warn(oss.str());
      }
    } else {
      std::ostringstream oss;
      oss << "Unexpected token in limit_except block: "
          << token.typeToString() << " at line " << token.lineNumber;
      throw exceptions::SyntaxException(
          oss.str(), exceptions::SyntaxException::UNEXPECTED_TOKEN);
    }
  }

  throw exceptions::SyntaxException("limit_except block not properly closed",
                                    exceptions::SyntaxException::MISSING_BRACE);
}

}  // namespace parser
}  // namespace config
}  // namespace infrastructure
