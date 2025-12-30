/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/18 18:26:50 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/30 17:09:09 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "infrastructure/config/exceptions/ConfigException.hpp"
#include "infrastructure/config/exceptions/SyntaxException.hpp"
#include "infrastructure/config/exceptions/ValidationException.hpp"
#include "infrastructure/config/parsers/ConfigParser.hpp"

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
  try {
    initializeParser(configPath);

    std::vector<lexer::Token> tokens = m_lexer.tokenizeFile(configPath);
    parser::ParserContext context(tokens, configPath);

    domain::configuration::entities::HttpConfig* httpConfig =
        new domain::configuration::entities::HttpConfig(configPath);

    parseTokens(context, *httpConfig);
    validateConfiguration(*httpConfig);

    std::ostringstream oss;
    oss << "Successfully parsed configuration from " << configPath << " with "
        << httpConfig->getServerConfigs().size() << " servers";
    m_logger.info(oss.str());

    return httpConfig;

  } catch (const exceptions::SyntaxException& e) {
    throw;
  } catch (const exceptions::ValidationException& e) {
    throw;
  } catch (const exceptions::ConfigException& e) {
    throw;
  } catch (const std::exception& e) {
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
  handleTopLevelDirectives(context, httpConfig);

  while (context.hasMoreTokens()) {
    const lexer::Token& token = context.currentToken();

    if (token.type == lexer::Token::EOF_T) {
      break;
    }

    if (token.type == lexer::Token::BLOCK_START) {
      std::string blockName = token.value;

      if (blockName == "http") {
        context.pushState(parser::ParserState::HTTP, "http");
        m_blockParser.parseHttpBlock(context, httpConfig);
      } else if (blockName == "server") {
        context.pushState(parser::ParserState::SERVER, "server");
        m_blockParser.parseServerBlock(context, httpConfig);
      } else {
        std::ostringstream oss;
        oss << "Unknown block type '" << blockName << "'";
        throw exceptions::SyntaxException(
            oss.str(), exceptions::SyntaxException::INVALID_DIRECTIVE);
      }
    } else if (token.type == lexer::Token::STRING) {
      handleTopLevelDirectives(context, httpConfig);
    } else {
      std::ostringstream oss;
      oss << "Unexpected token at top level: " << token.typeToString();
      throw exceptions::SyntaxException(
          oss.str(), exceptions::SyntaxException::UNEXPECTED_TOKEN);
    }
  }
}

void ConfigParser::handleTopLevelDirectives(
    parser::ParserContext& context,
    domain::configuration::entities::HttpConfig& httpConfig) {
  while (context.hasMoreTokens()) {
    const lexer::Token& token = context.peekToken();

    if (token.type == lexer::Token::EOF_T ||
        token.type == lexer::Token::BLOCK_START) {
      break;
    }

    if (token.type == lexer::Token::STRING) {
      context.advance();  // Move to directive token

      std::string directive = token.value;
      std::vector<std::string> args;

      // Collect arguments
      while (context.hasMoreTokens()) {
        const lexer::Token& argToken = context.peekToken();
        if (argToken.type == lexer::Token::STRING) {
          args.push_back(argToken.value);
          context.advance();
        } else {
          break;
        }
      }

      // Expect semicolon
      context.expect(lexer::Token::SEMICOLON, "directive termination");

      // Handle directive
      handlers::GlobalDirectiveHandler handler(m_logger, httpConfig);
      handler.handle(directive, args, token.lineNumber);
    } else {
      throw exceptions::SyntaxException(
          "Expected directive at top level",
          exceptions::SyntaxException::UNEXPECTED_TOKEN);
    }
  }
}

void ConfigParser::mergeIncludes(
    domain::configuration::entities::HttpConfig& httpConfig,
    const std::string& includePath) {
  (void)httpConfig;
  m_logger.debug("Merge includes called with path: " + includePath);
  // Implementation would go here
}

void ConfigParser::validateConfiguration(
    const domain::configuration::entities::HttpConfig& httpConfig) {
  httpConfig.validate();
}

}  // namespace parsers
}  // namespace config
}  // namespace infrastructure
/*
#include "domain/configuration/value_objects/CgiConfig.hpp"
#include "domain/filesystem/value_objects/UploadAccess.hpp"
#include "domain/shared/value_objects/ErrorCode.hpp"
#include "infrastructure/config/exceptions/ConfigException.hpp"
#include "infrastructure/config/parsers/ConfigParser.hpp"

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <dirent.h>
#include <fstream>
#include <sstream>
#include <sys/stat.h>
#include <vector>

namespace infrastructure {
namespace config {
namespace parsers {

ConfigParser::Token::Token(Type type, const std::string& value,
                           std::size_t numLine)
    : m_type(type), m_value(value), m_numLine(numLine) {}

ConfigParser::ConfigParser(application::ports::ILogger& logger)
    : m_logger(logger), m_indexToken(0) {
  m_logger.debug("ConfigParser initialized.");
}

ConfigParser::~ConfigParser() { m_tokens.clear(); }

domain::configuration::entities::HttpConfig* ConfigParser::parseFile(
    const std::string& configPath) {
  m_tokens.clear();
  m_indexToken = 0;
  m_stateStack = std::stack<ParserState>();
  m_currentFilePath = configPath;

  m_stateStack.push(ParserState(ParserState::GLOBAL));

  lexFile(configPath);

  domain::configuration::entities::HttpConfig* httpConfig =
      new domain::configuration::entities::HttpConfig(configPath);

  try {
    while (true) {
      Token token = nextToken();
      if (token.m_type == Token::EOF_T) break;

      if (token.m_type == Token::STRING) {
        std::string name = token.m_value;

        if (peekToken().m_type == Token::BLOCK_START) {
          nextToken();

          if (name == "http") {
            m_stateStack.push(ParserState(ParserState::HTTP));
            parseHttpBlock(*httpConfig);
          } else {
            throw exceptions::ConfigException(
                "Unknown top-level block: " + name,
                exceptions::ConfigException::PARSE_SYNTAX);
          }
          continue;
        }
      }

      throw exceptions::ConfigException(
          "Unexpected token at top-level at line " +
              static_cast<std::ostringstream&>(std::ostringstream()
                                               << token.m_numLine)
                  .str(),
          exceptions::ConfigException::PARSE_SYNTAX);
    }

    httpConfig->validate();
    return httpConfig;

  } catch (...) {
    delete httpConfig;
    throw;
  }
}

void ConfigParser::parseHttpBlock(
    domain::configuration::entities::HttpConfig& httpConfig) {
  while (true) {
    Token token = nextToken();

    if (token.m_type == Token::BLOCK_END) {
      m_stateStack.pop();
      return;
    }

    if (token.m_type == Token::EOF_T) {
      throw exceptions::ConfigException(
          "Unexpected EOF in http block",
          exceptions::ConfigException::PARSE_SYNTAX);
    }

    if (token.m_type == Token::STRING && token.m_value == "server" &&
        peekToken().m_type == Token::BLOCK_START) {
      nextToken();
      m_stateStack.push(ParserState(ParserState::SERVER));
      parseServerBlock(httpConfig);
      continue;
    }

    if (token.m_type == Token::STRING) {
      std::string directive = token.m_value;
      std::vector<std::string> args;

      Token argToken = peekToken();
      while (argToken.m_type == Token::STRING) {
        args.push_back(nextToken().m_value);
        argToken = peekToken();
      }

      if (peekToken().m_type == Token::BLOCK_START) {
        // Handle block directives like include, but include is not a block
        if (directive == "include") {
          if (args.size() != 1) {
            throw exceptions::ConfigException(
                "include directive requires one argument",
                exceptions::ConfigException::PARSE_SYNTAX);
          }
          processInclude(args[0], httpConfig, 1);
          nextToken();  // Skip the non-existent block, since include isn't a
                        // block
          continue;
        }
      }

      expect(Token::SEMICOLON, "end of directive");

      handleGlobalDirective(httpConfig, directive, args);
      continue;
    }

    throw exceptions::ConfigException(
        "Invalid token in http block",
        exceptions::ConfigException::PARSE_SYNTAX);
  }
}

void ConfigParser::parseServerBlock(
    domain::configuration::entities::HttpConfig& httpConfig) {
  domain::configuration::entities::ServerConfig* server =
      new domain::configuration::entities::ServerConfig();

  try {
    while (true) {
      Token token = nextToken();

      if (token.m_type == Token::BLOCK_END) {
        m_stateStack.pop();
        server->validate();
        httpConfig.addServerConfig(server);
        return;
      }

      if (token.m_type == Token::EOF_T) {
        throw exceptions::ConfigException(
            "Unexpected EOF in server block",
            exceptions::ConfigException::PARSE_SYNTAX);
      }

      if (token.m_type == Token::STRING && token.m_value == "location") {
        std::string locationPath = nextToken().m_value;
        expect(Token::BLOCK_START, "location block start");
        domain::configuration::entities::LocationConfig::LocationMatchType
            matchType = parseLocationMatchType(locationPath);
        std::string cleanPath = normalizePath(stripMatchPrefix(locationPath));
        domain::configuration::entities::LocationConfig* location =
            new domain::configuration::entities::LocationConfig(cleanPath,
                                                                matchType);
        m_stateStack.push(ParserState(ParserState::LOCATION));
        parseLocationBlock(*location);
        server->addLocation(location);
        continue;
      }

      if (token.m_type == Token::STRING) {
        std::string directive = token.m_value;
        std::vector<std::string> args;

        Token argToken = peekToken();
        while (argToken.m_type == Token::STRING) {
          args.push_back(nextToken().m_value);
          argToken = peekToken();
        }

        expect(Token::SEMICOLON, "end of server directive");

        handleServerDirective(*server, directive, args);
        continue;
      }

      throw exceptions::ConfigException(
          "Invalid token in server block",
          exceptions::ConfigException::PARSE_SYNTAX);
    }
  } catch (...) {
    delete server;
    throw;
  }
}

void ConfigParser::parseLocationBlock(
    domain::configuration::entities::LocationConfig& location) {
  while (true) {
    Token token = nextToken();

    if (token.m_type == Token::BLOCK_END) {
      m_stateStack.pop();
      location.validate();
      return;
    }

    if (token.m_type == Token::EOF_T) {
      throw exceptions::ConfigException(
          "Unexpected EOF in location block",
          exceptions::ConfigException::PARSE_SYNTAX);
    }

    if (token.m_type == Token::STRING && token.m_value == "location") {
      // Nested location
      std::string nestedPath = nextToken().m_value;
      expect(Token::BLOCK_START, "nested location block start");
      domain::configuration::entities::LocationConfig::LocationMatchType
          matchType = parseLocationMatchType(nestedPath);
      std::string cleanPath = normalizePath(stripMatchPrefix(nestedPath));
      domain::configuration::entities::LocationConfig* nestedLocation =
          new domain::configuration::entities::LocationConfig(cleanPath,
                                                              matchType);
      m_stateStack.push(ParserState(ParserState::LOCATION));
      parseLocationBlock(*nestedLocation);
      // For simplicity, add to parent location if needed; but in model,
      // locations are per server Assume nested is handled as child, but for
      // now, warn or ignore nesting beyond one level
      throw exceptions::ConfigException(
          "Nested locations beyond one level not supported yet",
          exceptions::ConfigException::PARSE_SYNTAX);
      continue;
    }

    if (token.m_type == Token::STRING && token.m_value == "limit_except") {
      std::vector<std::string> methods;
      Token argToken = peekToken();
      while (argToken.m_type == Token::STRING) {
        methods.push_back(nextToken().m_value.toUpperCase());  // Assume toUpper
        argToken = peekToken();
      }
      expect(Token::BLOCK_START, "limit_except block start");
      // Skip the block: expect "deny all;" then BLOCK_END
      while (true) {
        Token inner = nextToken();
        if (inner.m_type == Token::STRING && inner.m_value == "deny" &&
            nextToken().m_value == "all") {
          expect(Token::SEMICOLON, "deny all;");
        }
        if (inner.m_type == Token::BLOCK_END) break;
      }
      // Set allowed methods
      for (std::size_t i = 0; i < methods.size(); ++i) {
        try {
          domain::http::value_objects::HttpMethod method =
              parseHttpMethod(methods[i]);
          location.addAllowedMethod(method);
        } catch (...) {
          // Ignore invalid methods
        }
      }
      continue;
    }

    if (token.m_type == Token::STRING) {
      std::string directive = token.m_value;
      std::vector<std::string> args;

      Token argToken = peekToken();
      while (argToken.m_type == Token::STRING) {
        args.push_back(nextToken().m_value);
        argToken = peekToken();
      }

      if (peekToken().m_type == Token::BLOCK_START) {
        // Handle block directives if any
      } else {
        expect(Token::SEMICOLON, "end of location directive");
      }

      handleLocationDirective(location, directive, args);
      continue;
    }

    throw exceptions::ConfigException(
        "Invalid token in location block",
        exceptions::ConfigException::PARSE_SYNTAX);
  }
}

void ConfigParser::handleGlobalDirective(
    domain::configuration::entities::HttpConfig& httpConfig,
    const std::string& directive, const std::vector<std::string>& args) {
  if (directive == "error_page") {
    if (args.size() < 2) {
      throw exceptions::ConfigException(
          "error_page requires code and uri",
          exceptions::ConfigException::PARSE_SYNTAX);
    }
    unsigned int code = static_cast<unsigned int>(std::atoi(args[0].c_str()));
    std::string uri = args[1];
    httpConfig.setErrorPage(domain::shared::value_objects::ErrorCode(code),
                            uri);
  } else if (directive == "client_max_body_size") {
    if (args.empty()) {
      throw exceptions::ConfigException(
          "client_max_body_size requires size",
          exceptions::ConfigException::PARSE_SYNTAX);
    }
    httpConfig.setClientMaxBodySize(args[0]);
  } else {
    m_logger.warn("Unknown global directive: " + directive);
  }
}

void ConfigParser::handleServerDirective(
    domain::configuration::entities::ServerConfig& server,
    const std::string& directive, const std::vector<std::string>& args) {
  if (directive == "listen") {
    if (args.empty()) {
      throw exceptions::ConfigException(
          "listen requires address:port",
          exceptions::ConfigException::PARSE_SYNTAX);
    }
    std::string listenStr = args[0];
    if (listenStr.find("default_server") != std::string::npos) {
      listenStr = listenStr.substr(0, listenStr.find("default_server") - 1);
    }
    server.addListenDirective(listenStr);
  } else if (directive == "server_name") {
    for (std::size_t i = 0; i < args.size(); ++i) {
      server.addServerName(args[i]);
    }
  } else if (directive == "root") {
    if (args.empty()) {
      throw exceptions::ConfigException(
          "root requires path", exceptions::ConfigException::PARSE_SYNTAX);
    }
    server.setRoot(args[0]);
  } else if (directive == "index") {
    for (std::size_t i = 0; i < args.size(); ++i) {
      server.addIndexFile(args[i]);
    }
  } else if (directive == "error_page") {
    if (args.size() < 2) {
      throw exceptions::ConfigException(
          "error_page requires code and uri",
          exceptions::ConfigException::PARSE_SYNTAX);
    }
    unsigned int code = static_cast<unsigned int>(std::atoi(args[0].c_str()));
    std::string uri = args[1];
    server.addErrorPage(domain::shared::value_objects::ErrorCode(code), uri);
  } else {
    m_logger.warn("Unknown server directive: " + directive);
  }
}

void ConfigParser::handleLocationDirective(
    domain::configuration::entities::LocationConfig& location,
    const std::string& directive, const std::vector<std::string>& args) {
  if (directive == "root") {
    if (args.empty()) {
      throw exceptions::ConfigException(
          "root requires path", exceptions::ConfigException::PARSE_SYNTAX);
    }
    location.setRoot(args[0]);
  } else if (directive == "index") {
    for (std::size_t i = 0; i < args.size(); ++i) {
      location.addIndexFile(args[i]);
    }
  } else if (directive == "autoindex") {
    bool on = (args.empty() || args[0] == "on");
    location.setAutoIndex(on);
  } else if (directive == "try_files") {
    location.setTryFiles(args);
  } else if (directive == "return") {
    if (args.size() < 2) {
      throw exceptions::ConfigException(
          "return requires code and uri",
          exceptions::ConfigException::PARSE_SYNTAX);
    }
    unsigned int code = static_cast<unsigned int>(std::atoi(args[0].c_str()));
    std::string uri = args[1];
    location.setReturnRedirect(uri, code);
  } else if (directive == "upload_store") {
    if (args.empty()) {
      throw exceptions::ConfigException(
          "upload_store requires path",
          exceptions::ConfigException::PARSE_SYNTAX);
    }
    location.enableUpload(domain::filesystem::value_objects::Path(args[0]));
  } else if (directive == "upload_store_permissions") {
    if (args.empty()) {
      throw exceptions::ConfigException(
          "upload_store_permissions requires mode",
          exceptions::ConfigException::PARSE_SYNTAX);
    }
    unsigned int perms =
        static_cast<unsigned int>(std::strtoul(args[0].c_str(), NULL, 8));
    location.setUploadPermissions(perms);
  } else if (directive == "upload_store_access") {
    if (args.empty()) {
      throw exceptions::ConfigException(
          "upload_store_access requires access string",
          exceptions::ConfigException::PARSE_SYNTAX);
    }
    location.setUploadAccess(args[0]);
  } else if (directive == "location") {
    // Already handled in parseLocationBlock
  } else if (directive == "script") {
    // For CGI
    if (args.empty()) return;
    domain::configuration::value_objects::CgiConfig cgi;
    cgi.setInterpreter(args[0]);
    location.setCgiConfig(cgi);
  } else if (directive == "cgi_root") {
    if (args.empty()) return;
    // Set cgi root
    domain::configuration::value_objects::CgiConfig& cgi =
        location.getUploadConfigMutable();
    wait, cgi
    // Assume set in cgi config
  } else if (directive == "fastcgi_param") {
    // Add to cgi params
  } else {
    m_logger.warn("Unknown location directive: " + directive);
  }
}

void ConfigParser::lexFile(const std::string& path) {
  std::ifstream file(path.c_str());
  if (!file.is_open()) {
    throw exceptions::ConfigException(
        "Cannot open config file: " + path,
        exceptions::ConfigException::FILE_NOT_FOUND);
  }

  std::string line;
  std::size_t numLine = 0;
  while (std::getline(file, line)) {
    ++numLine;
    std::size_t col = 0;
    skipWhiteSpace(line, col);

    while (col < line.size()) {
      if (std::isspace(static_cast<unsigned char>(line[col]))) {
        skipWhiteSpace(line, col);
        continue;
      }

      if (handleSingleCharToken(line, col, numLine)) {
        continue;
      }

      if (line[col] == '#') {
        // Skip comment
        while (col < line.size() && line[col] != '\n') ++col;
        continue;
      }

      extractWordToken(line, col, numLine);
      handleAttachedSpecialChar(line, col, numLine);
    }
  }

  file.close();

  std::ostringstream oss;
  oss << "Lexed " << m_tokens.size() << " tokens from " << path;
  m_logger.debug(oss.str());
}

void ConfigParser::skipWhiteSpace(const std::string& line, std::size_t& col) {
  while (col < line.size() &&
         (std::isspace(static_cast<unsigned char>(line[col])) != 0)) {
    ++col;
  }
}

bool ConfigParser::handleSingleCharToken(const std::string& line,
                                         std::size_t& col,
                                         std::size_t numLine) {
  if (col >= line.size()) return false;
  if (line[col] == '{') {
    m_tokens.push_back(Token(Token::BLOCK_START, "", numLine));
    ++col;
    return true;
  }
  if (line[col] == '}') {
    m_tokens.push_back(Token(Token::BLOCK_END, "", numLine));
    ++col;
    return true;
  }
  if (line[col] == ';') {
    m_tokens.push_back(Token(Token::SEMICOLON, "", numLine));
    ++col;
    return true;
  }

  return false;
}

void ConfigParser::extractWordToken(const std::string& line, std::size_t& col,
                                    std::size_t numLine) {
  std::size_t start = col;
  while (col < line.size() &&
         (std::isspace(static_cast<unsigned char>(line[col])) == 0) &&
         line[col] != ';' && line[col] != '{' && line[col] != '}') {
    ++col;
  }
  std::string word = line.substr(start, col - start);
  m_tokens.push_back(Token(Token::STRING, word, numLine));
}

void ConfigParser::handleAttachedSpecialChar(const std::string& line,
                                             std::size_t& col,
                                             std::size_t numLine) {
  if (col < line.size()) {
    if (line[col] == ';') {
      m_tokens.push_back(Token(Token::SEMICOLON, "", numLine));
      ++col;
    } else if (line[col] == '{') {
      if (!m_tokens.empty() && m_tokens.back().m_type == Token::STRING) {
        std::string blockName = m_tokens.back().m_value;
        m_tokens.pop_back();
        m_tokens.push_back(Token(Token::BLOCK_START, blockName, numLine));
      } else {
        m_tokens.push_back(Token(Token::BLOCK_START, "", numLine));
      }
      ++col;
    } else if (line[col] == '}') {
      m_tokens.push_back(Token(Token::BLOCK_END, "", numLine));
      ++col;
    }
  }
}

ConfigParser::Token ConfigParser::nextToken() {
  if (m_indexToken < m_tokens.size()) {
    return m_tokens[m_indexToken++];
  }
  return Token(Token::EOF_T, "", 0);
}

ConfigParser::Token ConfigParser::peekToken() {
  if (m_indexToken < m_tokens.size()) {
    return m_tokens[m_indexToken];
  }
  return Token(Token::EOF_T, "", 0);
}

void ConfigParser::expect(Token::Type type, const std::string& context) {
  Token token = nextToken();
  if (token.m_type != type) {
    std::ostringstream oss;
    oss << "Expected " << tokenTypeToString(type) << " in " << context
        << " (got " << tokenTypeToString(token.m_type) << ") at line "
        << token.m_numLine;
    throw exceptions::ConfigException(
        oss.str(), exceptions::ConfigException::PARSE_SYNTAX);
  }
}

void ConfigParser::processInclude(
    const std::string& pattern,
    domain::configuration::entities::HttpConfig& httpConfig,
    std::size_t depth) {
  if (depth > K_MAX_INCLUDE_DEPTH) {
    throw exceptions::ConfigException(
        "Maximum include depth exceeded",
        exceptions::ConfigException::INCLUDE_RECURSION);
  }

  std::vector<std::string> files = expandGlob(pattern);
  for (std::size_t i = 0; i < files.size(); ++i) {
    // Recurse parse the included file and merge servers
    ConfigParser includeParser(m_logger);
    domain::configuration::entities::HttpConfig* includeConfig =
        includeParser.parseFile(files[i]);
    const domain::configuration::entities::HttpConfig::ServerConfigs& servers =
        includeConfig->getServerConfigs();
    for (std::size_t j = 0; j < servers.size(); ++j) {
      httpConfig.addServerConfig(
          new domain::configuration::entities::ServerConfig(*servers[j]));
    }
    delete includeConfig;
  }

  std::ostringstream oss;
  oss << "Processed include: " << pattern << " (depth " << depth << ")";
  m_logger.debug(oss.str());
}

std::string ConfigParser::tokenTypeToString(Token::Type type) {
  switch (type) {
    case Token::DIRECTIVE:
      return "DIRECTIVE";
    case Token::BLOCK_START:
      return "BLOCK_START";
    case Token::BLOCK_END:
      return "BLOCK_END";
    case Token::STRING:
      return "STRING";
    case Token::SEMICOLON:
      return "SEMICOLON";
    case Token::EOF_T:
      return "EOF";
    default:
      return "UNKNOWN";
  }
}

std::vector<std::string> ConfigParser::expandGlob(const std::string& pattern) {
  std::vector<std::string> result;
  // Simple glob expansion for *.conf
  std::string dirPath = pattern.substr(0, pattern.find_last_of('/'));
  std::string glob = pattern.substr(pattern.find_last_of('/') + 1);
  DIR* dir = opendir(dirPath.c_str());
  if (dir != 0) {
    struct dirent* ent;
    while ((ent = readdir(dir)) != NULL) {
      std::string filename = ent->d_name;
      if (filename.size() >= glob.size() &&
          filename.substr(filename.size() - glob.size()) == glob) {
        result.push_back(dirPath + "/" + filename);
      }
    }
    closedir(dir);
  }
  if (result.empty()) {
    result.push_back(pattern);  // Fallback
  }
  return result;
}

std::string ConfigParser::normalizePath(const std::string& path) {
  if (path.empty()) {
    return "/";
  }

  std::string result = path;

  if (result[0] != '/') {
    result = "/" + result;
  }

  if (result.size() > 1 && result[result.size() - 1] == '/') {
    result.erase(result.size() - 1);
  }

  return result;
}

domain::configuration::entities::LocationConfig::LocationMatchType
ConfigParser::parseLocationMatchType(const std::string& path) {
  if (path.empty()) {
    return domain::configuration::entities::LocationConfig::MATCH_PREFIX;
  }

  if (path[0] == '=') {
    return domain::configuration::entities::LocationConfig::MATCH_EXACT;
  }
  if (path[0] == '~') {
    if (path.size() > 1 && path[1] == '*') {
      return domain::configuration::entities::LocationConfig::
          MATCH_REGEX_CASE_INSENSITIVE;
    }
    return domain::configuration::entities::LocationConfig::
        MATCH_REGEX_CASE_SENSITIVE;
  }

  return domain::configuration::entities::LocationConfig::MATCH_PREFIX;
}

domain::http::value_objects::HttpMethod ConfigParser::parseHttpMethod(
    const std::string& method) {
  std::string upperMethod = method;
  std::transform(upperMethod.begin(), upperMethod.end(), upperMethod.begin(),
                 (int (*)(int))std::toupper);
  if (!domain::http::value_objects::HttpMethod::isValidMethodString(
          upperMethod)) {
    throw exceptions::ConfigException(
        "Invalid HTTP method: " + method,
        exceptions::ConfigException::PARSE_SYNTAX);
  }

  return domain::http::value_objects::HttpMethod(upperMethod);
}

std::string ConfigParser::stripMatchPrefix(const std::string& path) {
  // Reuse from LocationConfig if possible, but implement here
  domain::configuration::entities::LocationConfig::LocationMatchType type =
      parseLocationMatchType(path);

  switch (type) {
    case domain::configuration::entities::LocationConfig::MATCH_EXACT:
      return path.substr(1);
    case domain::configuration::entities::LocationConfig::
        MATCH_REGEX_CASE_SENSITIVE:
      if (path[0] == '~') return path.substr(1);
      return path;
    case domain::configuration::entities::LocationConfig::
        MATCH_REGEX_CASE_INSENSITIVE:
      if (path.size() > 1 && path[1] == '*') return path.substr(2);
      return path.substr(1);
    default:
      return path;
  }
}

void ConfigParser::mergeIncludes(
    domain::configuration::entities::HttpConfig& httpConfig,
    const std::string& includePath) {
  // Not used in this refactor, but placeholder
  (void)httpConfig;
  (void)includePath;
}

void ConfigParser::validateConfiguration(
    const domain::configuration::entities::HttpConfig& httpConfig) {
  // Delegate to model
  httpConfig.validate();
}

}  // namespace parsers
}  // namespace config
}  // namespace infrastructure
*/
