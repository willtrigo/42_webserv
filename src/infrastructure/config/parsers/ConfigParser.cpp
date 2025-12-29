/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/18 18:26:50 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/28 20:48:32 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "domain/configuration/value_objects/CgiConfig.hpp"
#include "domain/filesystem/value_objects/UploadAccess.hpp"
#include "domain/shared/value_objects/ErrorCode.hpp"
#include "infrastructure/config/exceptions/ConfigException.hpp"
#include "infrastructure/config/parsers/ConfigParser.hpp"

#include <fstream>
#include <cstdlib>
#include <dirent.h>
#include <sstream>
#include <sys/stat.h>

namespace infrastructure {
namespace config {
namespace parsers {

ConfigParser::Token::Token(Type type, const std::string& value,
                           std::size_t numLine)
    : m_type(type), m_value(value), m_numLine(numLine) {}

ConfigParser::ConfigParser(application::ports::ILogger& logger)
    : m_logger(logger), m_indexToken(0) {
  this->m_logger.debug("ConfigParser initialized.");
}

ConfigParser::~ConfigParser() { this->m_tokens.clear(); }

domain::configuration::entities::HttpConfig* ConfigParser::parseFile(
    const std::string& configPath) {
  try {
    this->m_tokens.clear();
    this->m_indexToken = 0;
    this->m_stateStack = std::stack<ParserState>();
    this->m_currentFilePath = configPath;

    // Start with GLOBAL context
    this->m_stateStack.push(ParserState(ParserState::GLOBAL));

    lexFile(configPath);

    domain::configuration::entities::HttpConfig* httpConfig =
        new domain::configuration::entities::HttpConfig(configPath);

    while (true) {
      Token token = nextToken();
      if (token.m_type == Token::EOF_T) {
        break;
      }

      if (token.m_type == Token::STRING) {
        // Handle directive or block start
        std::string directive = token.m_value;

        // Collect arguments
        std::vector<std::string> args;
        Token argToken = nextToken();
        while (argToken.m_type == Token::STRING) {
          args.push_back(argToken.m_value);
          argToken = nextToken();
        }

        if (argToken.m_type == Token::SEMICOLON) {
          // It's a directive
          ParserState currentState = this->m_stateStack.top();
          switch (currentState.context) {
            case ParserState::GLOBAL:
              handleGlobalDirective(*httpConfig, directive, args);
              break;
            case ParserState::HTTP:
              // HTTP block directives
              if (directive == "server") {
                parseServerBlock(*httpConfig);
              } else {
                handleGlobalDirective(*httpConfig, directive, args);
              }
              break;
            case ParserState::SERVER:
              // Should not happen - server directives are handled in
              // parseServerBlock
              break;
            case ParserState::LOCATION:
              // Should not happen - location directives are handled in
              // parseLocationBlock
              break;
          }
        } else if (argToken.m_type == Token::BLOCK_START &&
                   argToken.m_value == directive) {
          // It's a block
          if (directive == "http") {
            this->m_stateStack.push(ParserState(ParserState::HTTP, "http"));
            parseHttpBlock(*httpConfig);
          } else if (directive == "server") {
            this->m_stateStack.push(ParserState(ParserState::SERVER, "server"));
            parseServerBlock(*httpConfig);
          }
        } else {
          std::ostringstream oss;
          oss << "Unexpected token after directive '" << directive
              << "' at line " << token.m_numLine;
          throw exceptions::ConfigException(
              oss.str(), exceptions::ConfigException::PARSE_SYNTAX);
        }
      }
    }

    // Validate the parsed configuration
    httpConfig->validate();

    std::ostringstream oss;
    oss << "Successfully parsed configuration from " << configPath << " with "
        << httpConfig->getServerConfigs().size() << " servers";
    this->m_logger.info(oss.str());

    return httpConfig;

  } catch (const exceptions::ConfigException& e) {
    throw;
  } catch (const std::exception& e) {
    throw exceptions::ConfigException(
        std::string("Unexpected error during configuration parse: ") + e.what(),
        exceptions::ConfigException::LOAD_UNEXPECTED);
  }
}

void ConfigParser::parseHttpBlock(
    domain::configuration::entities::HttpConfig& httpConfig) {
  while (true) {
    Token token = nextToken();
    if (token.m_type == Token::BLOCK_END) {
      // End of http block
      this->m_stateStack.pop();
      break;
    }
    if (token.m_type == Token::EOF_T) {
      throw exceptions::ConfigException(
          "Unexpected EOF in http block",
          exceptions::ConfigException::PARSE_SYNTAX);
    }
    if (token.m_type == Token::STRING) {
      std::string directive = token.m_value;
      std::vector<std::string> args;

      Token argToken = nextToken();
      while (argToken.m_type == Token::STRING) {
        args.push_back(argToken.m_value);
        argToken = nextToken();
      }

      if (argToken.m_type == Token::SEMICOLON) {
        // HTTP block directive
        handleGlobalDirective(httpConfig, directive, args);
      } else if (argToken.m_type == Token::BLOCK_START &&
                 argToken.m_value == "server") {
        // Nested server block
        this->m_stateStack.push(ParserState(ParserState::SERVER, "server"));
        parseServerBlock(httpConfig);
      } else {
        std::ostringstream oss;
        oss << "Unexpected token in http block at line " << token.m_numLine;
        throw exceptions::ConfigException(
            oss.str(), exceptions::ConfigException::PARSE_SYNTAX);
      }
    }
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
        // End of server block
        this->m_stateStack.pop();
        break;
      }
      if (token.m_type == Token::EOF_T) {
        throw exceptions::ConfigException(
            "Unexpected EOF in server block",
            exceptions::ConfigException::PARSE_SYNTAX);
      }
      if (token.m_type == Token::STRING) {
        std::string directive = token.m_value;
        std::vector<std::string> args;

        Token argToken = nextToken();
        while (argToken.m_type == Token::STRING) {
          args.push_back(argToken.m_value);
          argToken = nextToken();
        }

        if (argToken.m_type == Token::SEMICOLON) {
          // Server directive
          handleServerDirective(*server, directive, args);
        } else if (argToken.m_type == Token::BLOCK_START &&
                   argToken.m_value == "location") {
          // Location block
          if (args.size() != 1) {
            throw exceptions::ConfigException(
                "Location block requires exactly one path argument",
                exceptions::ConfigException::PARSE_SYNTAX);
          }

          this->m_stateStack.push(
              ParserState(ParserState::LOCATION, "location"));
          parseLocationBlock(*server);
        } else {
          std::ostringstream oss;
          oss << "Unexpected token in server block at line " << token.m_numLine;
          throw exceptions::ConfigException(
              oss.str(), exceptions::ConfigException::PARSE_SYNTAX);
        }
      }
    }

    // Add server to http config
    server->validate();
    httpConfig.addServerConfig(server);

  } catch (...) {
    delete server;
    throw;
  }
}

void ConfigParser::parseLocationBlock(
    domain::configuration::entities::ServerConfig& server) {
  Token pathToken = nextToken();
  if (pathToken.m_type != Token::STRING) {
    expect(Token::STRING, "location path");
  }

  std::string path = pathToken.m_value;
  domain::configuration::entities::LocationConfig::LocationMatchType matchType =
      parseLocationMatchType(path);

  // Remove match prefix for exact path
  std::string cleanPath = path;
  if (matchType == domain::configuration::entities::LocationConfig::
                       MATCH_REGEX_CASE_SENSITIVE ||
      matchType == domain::configuration::entities::LocationConfig::
                       MATCH_REGEX_CASE_INSENSITIVE) {
    cleanPath = path.substr(1);  // Remove ~ or ~*
  } else if (matchType ==
             domain::configuration::entities::LocationConfig::MATCH_EXACT) {
    cleanPath = path.substr(1);  // Remove =
  }

  domain::configuration::entities::LocationConfig* location =
      new domain::configuration::entities::LocationConfig(cleanPath, matchType);

  try {
    while (true) {
      Token token = nextToken();
      if (token.m_type == Token::BLOCK_END) {
        // End of location block
        this->m_stateStack.pop();
        break;
      }
      if (token.m_type == Token::EOF_T) {
        throw exceptions::ConfigException(
            "Unexpected EOF in location block",
            exceptions::ConfigException::PARSE_SYNTAX);
      }
      if (token.m_type == Token::STRING) {
        std::string directive = token.m_value;
        std::vector<std::string> args;

        Token argToken = nextToken();
        while (argToken.m_type == Token::STRING) {
          args.push_back(argToken.m_value);
          argToken = nextToken();
        }

        if (argToken.m_type == Token::SEMICOLON) {
          // Location directive
          handleLocationDirective(*location, directive, args);
        } else {
          std::ostringstream oss;
          oss << "Unexpected token in location block at line "
              << token.m_numLine;
          throw exceptions::ConfigException(
              oss.str(), exceptions::ConfigException::PARSE_SYNTAX);
        }
      }
    }

    // Add location to server
    location->validate();
    server.addLocation(location);

  } catch (...) {
    delete location;
    throw;
  }
}

void ConfigParser::handleGlobalDirective(
    domain::configuration::entities::HttpConfig& httpConfig,
    const std::string& directive, const std::vector<std::string>& args) {
  if (directive == "worker_processes") {
    if (args.size() != 1) {
      throw exceptions::ConfigException(
          "worker_processes requires exactly one argument",
          exceptions::ConfigException::PARSE_SYNTAX);
    }
    unsigned int processes =
        static_cast<unsigned int>(std::atoi(args[0].c_str()));
    httpConfig.setWorkerProcesses(processes);
  } else if (directive == "worker_connections") {
    if (args.size() != 1) {
      throw exceptions::ConfigException(
          "worker_connections requires exactly one argument",
          exceptions::ConfigException::PARSE_SYNTAX);
    }
    unsigned int connections =
        static_cast<unsigned int>(std::atoi(args[0].c_str()));
    httpConfig.setWorkerConnections(connections);
  } else if (directive == "client_max_body_size") {
    if (args.size() != 1) {
      throw exceptions::ConfigException(
          "client_max_body_size requires exactly one argument",
          exceptions::ConfigException::PARSE_SYNTAX);
    }
    httpConfig.setClientMaxBodySize(args[0]);
  } else if (directive == "error_log") {
    if (args.size() != 1) {
      throw exceptions::ConfigException(
          "error_log requires exactly one argument",
          exceptions::ConfigException::PARSE_SYNTAX);
    }
    httpConfig.setErrorLogPath(args[0]);
  } else if (directive == "access_log") {
    if (args.size() != 1) {
      throw exceptions::ConfigException(
          "access_log requires exactly one argument",
          exceptions::ConfigException::PARSE_SYNTAX);
    }
    httpConfig.setAccessLogPath(args[0]);
  } else if (directive == "include") {
    // Includes are handled separately in mergeIncludes
    // Just log for now
    std::ostringstream oss;
    oss << "Found include directive: ";
    for (size_t i = 0; i < args.size(); ++i) {
      if (i > 0) oss << " ";
      oss << args[i];
    }
    this->m_logger.debug(oss.str());
  } else {
    std::ostringstream oss;
    oss << "Unknown global directive: " << directive;
    this->m_logger.warn(oss.str());
  }
}

void ConfigParser::handleServerDirective(
    domain::configuration::entities::ServerConfig& server,
    const std::string& directive, const std::vector<std::string>& args) {
  if (directive == "listen") {
    for (size_t i = 0; i < args.size(); ++i) {
      server.addListenDirective(args[i]);
    }
  } else if (directive == "server_name") {
    for (size_t i = 0; i < args.size(); ++i) {
      server.addServerName(args[i]);
    }
  } else if (directive == "root") {
    if (args.size() != 1) {
      throw exceptions::ConfigException(
          "root requires exactly one argument",
          exceptions::ConfigException::PARSE_SYNTAX);
    }
    server.setRoot(args[0]);
  } else if (directive == "index") {
    for (size_t i = 0; i < args.size(); ++i) {
      server.addIndexFile(args[i]);
    }
  } else if (directive == "error_page") {
    if (args.size() < 2) {
      throw exceptions::ConfigException(
          "error_page requires at least two arguments",
          exceptions::ConfigException::PARSE_SYNTAX);
    }
    // Last argument is the URI
    std::string uri = args.back();
    // All but last are error codes
    for (size_t i = 0; i < args.size() - 1; ++i) {
      domain::shared::value_objects::ErrorCode code(
          static_cast<unsigned int>(std::atoi(args[i].c_str())));
      server.addErrorPage(code, uri);
    }
  } else if (directive == "client_max_body_size") {
    if (args.size() != 1) {
      throw exceptions::ConfigException(
          "client_max_body_size requires exactly one argument",
          exceptions::ConfigException::PARSE_SYNTAX);
    }
    server.setClientMaxBodySize(args[0]);
  } else if (directive == "return") {
    if (args.size() != 2) {
      throw exceptions::ConfigException(
          "return requires exactly two arguments (code and redirect)",
          exceptions::ConfigException::PARSE_SYNTAX);
    }
    server.setReturnRedirect(
        args[1], static_cast<unsigned int>(std::atoi(args[0].c_str())));
  } else {
    std::ostringstream oss;
    oss << "Unknown server directive: " << directive;
    this->m_logger.warn(oss.str());
  }
}

void ConfigParser::handleLocationDirective(
    domain::configuration::entities::LocationConfig& location,
    const std::string& directive, const std::vector<std::string>& args) {
  if (directive == "root") {
    if (args.size() != 1) {
      throw exceptions::ConfigException(
          "root requires exactly one argument",
          exceptions::ConfigException::PARSE_SYNTAX);
    }
    location.setRoot(args[0]);
  } else if (directive == "alias") {
    if (args.size() != 1) {
      throw exceptions::ConfigException(
          "alias requires exactly one argument",
          exceptions::ConfigException::PARSE_SYNTAX);
    }
    location.setAlias(args[0]);
  } else if (directive == "index") {
    for (size_t i = 0; i < args.size(); ++i) {
      location.addIndexFile(args[i]);
    }
  } else if (directive == "limit_except") {
    // Parse HTTP methods from limit_except directive
    // Format: limit_except GET POST { deny all; }

    if (args.empty()) {
      throw exceptions::ConfigException(
          "limit_except requires at least one argument",
          exceptions::ConfigException::PARSE_SYNTAX);
    }

    // Find the opening brace
    size_t braceIndex = args.size();
    for (size_t i = 0; i < args.size(); ++i) {
      if (args[i] == "{") {
        braceIndex = i;
        break;
      }
    }

    // Extract methods (everything before the brace)
    std::vector<std::string> methodStrings;
    for (size_t i = 0; i < braceIndex && i < args.size(); ++i) {
      methodStrings.push_back(args[i]);
    }

    if (methodStrings.empty()) {
      throw exceptions::ConfigException(
          "limit_except requires at least one HTTP method before {",
          exceptions::ConfigException::PARSE_SYNTAX);
    }

    // Verify the block contains "deny all;"
    if (braceIndex < args.size() - 2) {
      if (args[braceIndex] == "{" && args[braceIndex + 1] == "deny" &&
          args[braceIndex + 2] == "all") {
        // Valid format
      } else {
        this->m_logger.warn(
            "limit_except block doesn't follow expected 'deny all' pattern");
      }
    }

    // Add the specified methods as allowed (in nginx, limit_except specifies
    // methods that are NOT denied)
    for (size_t i = 0; i < methodStrings.size(); ++i) {
      try {
        domain::http::value_objects::HttpMethod method =
            domain::http::value_objects::HttpMethod(methodStrings[i]);
        location.addAllowedMethod(method);
      } catch (const std::exception& e) {
        std::ostringstream oss;
        oss << "Invalid HTTP method in limit_except: " << methodStrings[i]
            << " - " << e.what();
        throw exceptions::ConfigException(
            oss.str(), exceptions::ConfigException::PARSE_SYNTAX);
      }
    }
  } else if (directive == "autoindex") {
    if (args.size() != 1 || (args[0] != "on" && args[0] != "off")) {
      throw exceptions::ConfigException(
          "autoindex requires exactly one argument: 'on' or 'off'",
          exceptions::ConfigException::PARSE_SYNTAX);
    }
    location.setAutoIndex(args[0] == "on");
  } else if (directive == "try_files") {
    location.setTryFiles(args);
  } else if (directive == "return") {
    if (args.size() != 2) {
      throw exceptions::ConfigException(
          "return requires exactly two arguments (code and redirect)",
          exceptions::ConfigException::PARSE_SYNTAX);
    }
    location.setReturnRedirect(
        args[1], static_cast<unsigned int>(std::atoi(args[0].c_str())));
  } else if (directive == "upload_store") {
    if (args.size() != 1) {
      throw exceptions::ConfigException(
          "upload_store requires exactly one argument",
          exceptions::ConfigException::PARSE_SYNTAX);
    }

    location.setUploadDirectory(args[0]);

  } else if (directive == "upload_store_permissions") {
    if (args.size() != 1) {
      throw exceptions::ConfigException(
          "upload_store_permissions requires exactly one argument",
          exceptions::ConfigException::PARSE_SYNTAX);
    }

    // Parse octal permissions (e.g., "0660")
    unsigned int permissions;
    if (args[0].find_first_not_of("01234567") == std::string::npos) {
      // It's an octal string
      permissions = std::strtoul(args[0].c_str(), NULL, 8);
    } else {
      // Try to parse as decimal
      permissions = static_cast<unsigned int>(std::atoi(args[0].c_str()));
    }

    // Use the new setUploadPermissions method
    location.setUploadPermissions(permissions);

  } else if (directive == "upload_store_access") {
    if (args.size() != 1) {
      throw exceptions::ConfigException(
          "upload_store_access requires exactly one argument",
          exceptions::ConfigException::PARSE_SYNTAX);
    }

    // Parse using the UploadAccess class
    domain::filesystem::value_objects::UploadAccess access =
        domain::filesystem::value_objects::UploadAccess::fromString(args[0]);

    // Store the access string
    location.setUploadAccess(args[0]);

    // If you want to convert to Permission for the upload config
    // You might need a method to convert UploadAccess to Permission
    // For now, we'll store it as a string and handle conversion elsewhere
  } else if (directive == "script") {
    if (args.size() != 1) {
      throw exceptions::ConfigException(
          "script requires exactly one argument",
          exceptions::ConfigException::PARSE_SYNTAX);
    }

    // Get or create CgiConfig
    domain::configuration::value_objects::CgiConfig cgiConfig;

    try {
      cgiConfig = location.getCgiConfig();
      // Update script path
      cgiConfig.setScriptPath(args[0]);
    } catch (...) {
      // Create new CgiConfig with script path
      // We need to provide root and pattern - use defaults for now
      domain::filesystem::value_objects::Path emptyRoot;
      domain::shared::value_objects::RegexPattern defaultPattern(
          "\\.(php|py|pl|cgi)$");

      cgiConfig = domain::configuration::value_objects::CgiConfig(
          args[0], emptyRoot, defaultPattern);
    }

    location.setCgiConfig(cgiConfig);

  } else if (directive == "cgi_root") {
    if (args.size() != 1) {
      throw exceptions::ConfigException(
          "cgi_root requires exactly one argument",
          exceptions::ConfigException::PARSE_SYNTAX);
    }

    domain::configuration::value_objects::CgiConfig cgiConfig;

    try {
      cgiConfig = location.getCgiConfig();
      // Update CGI root
      domain::filesystem::value_objects::Path cgiRoot(args[0]);
      cgiConfig.setCgiRoot(cgiRoot);
    } catch (...) {
      // Create new CgiConfig with CGI root
      domain::filesystem::value_objects::Path cgiRoot(args[0]);
      domain::shared::value_objects::RegexPattern defaultPattern(
          "\\.(php|py|pl|cgi)$");
      cgiConfig = domain::configuration::value_objects::CgiConfig(
          "", cgiRoot, defaultPattern);
    }

    location.setCgiConfig(cgiConfig);

  } else if (directive == "fastcgi_param") {
    if (args.size() < 2) {
      throw exceptions::ConfigException(
          "fastcgi_param requires at least two arguments",
          exceptions::ConfigException::PARSE_SYNTAX);
    }

    std::string paramName = args[0];
    std::string paramValue;

    for (size_t i = 1; i < args.size(); ++i) {
      if (i > 1) paramValue += " ";
      paramValue += args[i];
    }

    domain::configuration::value_objects::CgiConfig cgiConfig;

    try {
      cgiConfig = location.getCgiConfig();
    } catch (...) {
      // Create new CgiConfig with defaults
      domain::filesystem::value_objects::Path emptyRoot;
      domain::shared::value_objects::RegexPattern defaultPattern(
          "\\.(php|py|pl|cgi)$");
      cgiConfig = domain::configuration::value_objects::CgiConfig(
          "", emptyRoot, defaultPattern);
    }

    cgiConfig.addParameter(paramName, paramValue);
    location.setCgiConfig(cgiConfig);

  } else if (directive == "upload_max_file_size") {
    // Custom directive for upload max file size
    if (args.size() != 1) {
      throw exceptions::ConfigException(
          "upload_max_file_size requires exactly one argument",
          exceptions::ConfigException::PARSE_SYNTAX);
    }

    location.setUploadMaxFileSize(args[0]);

  } else if (directive == "upload_max_total_size") {
    // Custom directive for upload max total size
    if (args.size() != 1) {
      throw exceptions::ConfigException(
          "upload_max_total_size requires exactly one argument",
          exceptions::ConfigException::PARSE_SYNTAX);
    }

    location.setUploadMaxTotalSize(args[0]);

  } else if (directive == "include") {
    if (args.size() != 1) {
      throw exceptions::ConfigException(
          "include requires exactly one argument in location context",
          exceptions::ConfigException::PARSE_SYNTAX);
    }

    std::ostringstream oss;
    oss << "Location include directive found: " << args[0];
    this->m_logger.info(oss.str());

  } else if (directive == "add_header") {
    if (args.size() < 2) {
      throw exceptions::ConfigException(
          "add_header requires at least two arguments",
          exceptions::ConfigException::PARSE_SYNTAX);
    }

    std::ostringstream oss;
    oss << "Custom header directive: " << args[0] << " = ";
    for (size_t i = 1; i < args.size(); ++i) {
      if (i > 1) oss << " ";
      oss << args[i];
    }
    this->m_logger.info(oss.str());

  } else {
    std::ostringstream oss;
    oss << "Unknown or unsupported location directive: " << directive;
    this->m_logger.warn(oss.str());
  }
}

void ConfigParser::mergeIncludes(
    domain::configuration::entities::HttpConfig& httpConfig,
    const std::string& includePath) {
  // TODO: implement mergeIncludes
  // This is a simplified implementation
  // In a real implementation, you would:
  // 1. Parse included files
  // 2. Merge their configurations into httpConfig
  // 3. Handle conflicts appropriately
  (void)httpConfig;

  this->m_logger.debug("Merge includes called with path: " + includePath);
  // For now, we'll assume includes are handled during main parsing
}

void ConfigParser::validateConfiguration(
    const domain::configuration::entities::HttpConfig& httpConfig) {
  httpConfig.validate();
}

void ConfigParser::lexFile(const std::string& path) {
  std::ifstream file(path.c_str());
  if (!file.is_open()) {
    throw exceptions::ConfigException(
        "Config load failed: Cannot open config: " + path,
        exceptions::ConfigException::LOAD_FILE_NOT_FOUND);
  }

  std::string line;
  std::size_t numLine = 0;
  while (std::getline(file, line)) {
    ++numLine;
    std::size_t col = 0;
    skipWhiteSpace(line, col);
    if (col < line.size() && line[col] == '#') continue;
    while (col < line.size()) {
      skipWhiteSpace(line, col);
      if (col >= line.size()) break;

      if (handleSingleCharToken(line, col, numLine)) continue;

      extractWordToken(line, col, numLine);

      handleAttachedSpecialChar(line, col, numLine);
    }
  }

  this->m_tokens.push_back(Token(Token::EOF_T, "", numLine));

  std::ostringstream oss;
  oss << "Lexed " << this->m_tokens.size() << " tokens from " << path;
  this->m_logger.debug(oss.str());
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
  if (line[col] == '{') {
    this->m_tokens.push_back(Token(Token::BLOCK_START, "", numLine));
    ++col;
    return true;
  }
  if (line[col] == '}') {
    this->m_tokens.push_back(Token(Token::BLOCK_END, "", numLine));
    ++col;
    return true;
  }
  if (line[col] == ';') {
    this->m_tokens.push_back(Token(Token::SEMICOLON, "", numLine));
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
  this->m_tokens.push_back(Token(Token::STRING, word, numLine));
}

void ConfigParser::handleAttachedSpecialChar(const std::string& line,
                                             std::size_t& col,
                                             std::size_t numLine) {
  if (col < line.size()) {
    if (line[col] == ';') {
      this->m_tokens.push_back(Token(Token::SEMICOLON, "", numLine));
      ++col;
    } else if (line[col] == '{') {
      if (!this->m_tokens.empty() &&
          this->m_tokens.back().m_type == Token::STRING) {
        std::string block_name = this->m_tokens.back().m_value;
        this->m_tokens.pop_back();
        this->m_tokens.push_back(
            Token(Token::BLOCK_START, block_name, numLine));
      } else {
        this->m_tokens.push_back(Token(Token::BLOCK_START, "", numLine));
      }
      ++col;
    } else if (line[col] == '}') {
      this->m_tokens.push_back(Token(Token::BLOCK_END, "", numLine));
      ++col;
    }
  }
}

// Parser methods
ConfigParser::Token ConfigParser::nextToken() {
  if (this->m_indexToken < this->m_tokens.size()) {
    return this->m_tokens[this->m_indexToken++];
  }
  return Token(Token::EOF_T, "", 0);
}

ConfigParser::Token ConfigParser::peekToken() {
  if (this->m_indexToken < this->m_tokens.size()) {
    return this->m_tokens[this->m_indexToken];
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

// Helper methods
void ConfigParser::processInclude(
    const std::string& pattern,
    domain::configuration::entities::HttpConfig& httpConfig,
    std::size_t depth) {
  (void)httpConfig;
  if (depth > K_MAX_INCLUDE_DEPTH) {
    throw exceptions::ConfigException(
        "Maximum include depth exceeded",
        exceptions::ConfigException::INCLUDE_RECURSION);
  }

  std::ostringstream oss;
  oss << "Process include directive: " << pattern << " (depth " << depth << ")";
  this->m_logger.debug(oss.str());

  // TODO: Implement actual include file processing
  // For now, just log the include directive
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
  // Simple implementation - for now just return the pattern
  // In a real implementation, you would expand wildcards
  std::vector<std::string> result;
  result.push_back(pattern);
  return result;
}

std::string ConfigParser::normalizePath(const std::string& path) {
  if (path.empty()) {
    return "/";
  }

  std::string result = path;

  // Ensure path starts with /
  if (result[0] != '/') {
    result = "/" + result;
  }

  // Remove trailing slashes (except for root)
  if (result.size() > 1 && result[result.size() - 1] == '/') {
    result.erase(result.size() - 1);
  }

  // TODO: Handle .. and . components
  // For now, just return the normalized path

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
  if (!domain::http::value_objects::HttpMethod::isValidMethodString(method)) {
    throw exceptions::ConfigException(
        "Invalid HTTP method: " + method,
        exceptions::ConfigException::PARSE_SYNTAX);
  }

  return domain::http::value_objects::HttpMethod(method);
}

}  // namespace parsers
}  // namespace config
}  // namespace infrastructure
