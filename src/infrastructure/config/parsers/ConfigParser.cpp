/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: umeneses <umeneses@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/18 18:26:50 by dande-je          #+#    #+#             */
/*   Updated: 2026/01/06 20:33:02 by umeneses         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "infrastructure/config/exceptions/ConfigException.hpp"
#include "infrastructure/config/exceptions/SyntaxException.hpp"
#include "infrastructure/config/exceptions/ValidationException.hpp"
#include "infrastructure/config/handlers/GlobalDirectiveHandler.hpp"
#include "infrastructure/config/parsers/ConfigParser.hpp"
#include "infrastructure/config/parsers/IncludeProcessor.hpp"

#include <sstream>

namespace infrastructure {
namespace config {
namespace parsers {

ConfigParser::ConfigParser(application::ports::ILogger& logger)
    : m_logger(logger), m_lexer(logger), m_blockParser(logger) {
  m_logger.debug("ConfigParser initialized.");
}

ConfigParser::~ConfigParser() {}

domain::configuration::entities::HttpConfig* ConfigParser::parseFile(
    const std::string& configPath) {
  domain::configuration::entities::HttpConfig* httpConfig = NULL;

  try {
    initializeParser(configPath);

    std::vector<lexer::Token> tokens = m_lexer.tokenizeFile(configPath);
    parser::ParserContext context(tokens, configPath);

    httpConfig = new domain::configuration::entities::HttpConfig(configPath);

    parseTokens(context, *httpConfig);
    validateConfiguration(*httpConfig);

    std::ostringstream oss;
    oss << "Successfully parsed configuration from " << configPath << " with "
        << httpConfig->getServerConfigs().size() << " servers";
    m_logger.info(oss.str());

    return httpConfig;

  } catch (const exceptions::SyntaxException& e) {
    delete httpConfig;
    throw;
  } catch (const exceptions::ValidationException& e) {
    delete httpConfig;
    throw;
  } catch (const exceptions::ConfigException& e) {
    delete httpConfig;
    throw;
  } catch (const std::exception& e) {
    delete httpConfig;
    throw exceptions::ConfigException(
        std::string("Unexpected error during configuration parse: ") + e.what(),
        exceptions::ConfigException::LOAD_UNEXPECTED);
  }
}

void ConfigParser::initializeParser(const std::string& configPath) {
  m_logger.debug("Initializing parser for: " + configPath);
}

void ConfigParser::parseTokens(
    parser::ParserContext& context,
    domain::configuration::entities::HttpConfig& httpConfig) {
  while (context.hasMoreTokens()) {
    const lexer::Token& token = context.currentToken();

    if (token.type == lexer::Token::EOF_T) {
      break;
    }

    if (token.type == lexer::Token::STRING) {
      std::string tokenValue = token.value;

      if (context.currentIndex() + 1 < context.tokenCount()) {
        const lexer::Token& nextToken = context.peekToken();

        if (nextToken.type == lexer::Token::BLOCK_START) {
          handleBlockDeclaration(context, httpConfig, tokenValue);
        } else {
          handleSingleDirective(context, httpConfig);
        }
      } else {
        std::ostringstream oss;
        oss << "Unexpected end of file after token '" << tokenValue
            << "' at line " << token.lineNumber;
        throw exceptions::SyntaxException(
            oss.str(), exceptions::SyntaxException::UNEXPECTED_EOF);
      }
    } else if (token.type == lexer::Token::BLOCK_START) {
      std::ostringstream oss;
      oss << "Unexpected block start without block name at line "
          << token.lineNumber;
      throw exceptions::SyntaxException(
          oss.str(), exceptions::SyntaxException::UNEXPECTED_TOKEN);
    } else if (token.type == lexer::Token::SEMICOLON) {
      std::ostringstream oss;
      oss << "Unexpected semicolon at top level at line " << token.lineNumber;
      throw exceptions::SyntaxException(
          oss.str(), exceptions::SyntaxException::UNEXPECTED_TOKEN);
    } else {
      std::ostringstream oss;
      oss << "Unexpected token at top level: " << token.typeToString()
          << " at line " << token.lineNumber;
      throw exceptions::SyntaxException(
          oss.str(), exceptions::SyntaxException::UNEXPECTED_TOKEN);
    }
  }
}

void ConfigParser::handleBlockDeclaration(
    parser::ParserContext& context,
    domain::configuration::entities::HttpConfig& httpConfig,
    const std::string& blockName) {
  std::size_t lineNumber = context.currentToken().lineNumber;

  context.advance();

  context.expect(lexer::Token::BLOCK_START, "block start");
  context.advance();

  if (blockName == "http") {
    context.pushState(parser::ParserState::HTTP, "http");

    std::ostringstream oss;
    oss << "Entering http block at line " << lineNumber;
    m_logger.debug(oss.str());

    m_blockParser.parseHttpBlock(context, httpConfig);

  } else if (blockName == "server") {
    context.pushState(parser::ParserState::SERVER, "server");

    std::ostringstream oss;
    oss << "Entering server block at line " << lineNumber;
    m_logger.debug(oss.str());

    m_blockParser.parseServerBlock(context, httpConfig);

  } else {
    std::ostringstream oss;
    oss << "Unknown block type '" << blockName << "' at line " << lineNumber;
    throw exceptions::SyntaxException(
        oss.str(), exceptions::SyntaxException::INVALID_DIRECTIVE);
  }
}

void ConfigParser::handleSingleDirective(
    parser::ParserContext& context,
    domain::configuration::entities::HttpConfig& httpConfig) {
  const lexer::Token& directiveToken = context.currentToken();
  std::string directive = directiveToken.value;
  std::size_t lineNumber = directiveToken.lineNumber;

  context.advance();

  std::vector<std::string> args;

  while (context.hasMoreTokens()) {
    const lexer::Token& token = context.currentToken();

    if (token.type == lexer::Token::SEMICOLON) {
      context.advance();  // Consume the semicolon
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
    } else {
      std::ostringstream oss;
      oss << "Expected argument or semicolon after directive '" << directive
          << "' at line " << token.lineNumber << ", but found "
          << token.typeToString();
      throw exceptions::SyntaxException(
          oss.str(), exceptions::SyntaxException::UNEXPECTED_TOKEN);
    }
  }

  handlers::GlobalDirectiveHandler handler(m_logger, httpConfig);
  handler.handle(directive, args, lineNumber);

  std::ostringstream oss;
  oss << "Processed top-level directive '" << directive << "' with "
      << args.size() << " arguments at line " << lineNumber;
  m_logger.debug(oss.str());
}

void ConfigParser::mergeIncludes(
    domain::configuration::entities::HttpConfig& httpConfig,
    const std::string& includePath) {
  if (includePath.empty()) {
    m_logger.debug("No include path specified, skipping merge");
    return;
  }

  try {
    std::ostringstream oss;
    oss << "Merging includes from path: " << includePath;
    m_logger.info(oss.str());

    IncludeProcessor processor(m_logger);
    processor.processIncludes(includePath, httpConfig, 0);

    std::ostringstream successMsg;
    successMsg << "Successfully merged includes from: " << includePath;
    m_logger.info(successMsg.str());

  } catch (const exceptions::ConfigException& e) {
    std::ostringstream errorMsg;
    errorMsg << "Failed to merge includes from '" << includePath
             << "': " << e.what();
    m_logger.warn(errorMsg.str());
  }
}

void ConfigParser::validateConfiguration(
    const domain::configuration::entities::HttpConfig& httpConfig) {
  httpConfig.validate();
}

}  // namespace parsers
}  // namespace config
}  // namespace infrastructure
