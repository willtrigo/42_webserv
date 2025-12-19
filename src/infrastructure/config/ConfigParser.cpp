/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/18 18:26:50 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/18 21:23:13 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "infrastructure/config/ConfigParser.hpp"
#include "shared/exceptions/ConfigException.hpp"

#include <fstream>
#include <sstream>

namespace infrastructure {
namespace config {

ConfigParser::Token::Token(Type type, std::string value, std::size_t numLine)
    : m_type(type), m_value(value), m_numLine(numLine) {}

ConfigParser::ConfigParser(application::ports::ILogger& logger)
    : m_logger(logger) {
  this->m_logger.debug("ConfigParser initialized.");
}

ConfigParser::ConfigParser(const ConfigParser& other)
    : m_logger(other.m_logger),
      m_tokens(other.m_tokens),
      m_indexToken(other.m_indexToken),
      m_globalDirectives(other.m_globalDirectives) {}

ConfigParser::~ConfigParser() {
  this->m_tokens.clear();
  this->m_globalDirectives.clear();
}

ConfigParser& ConfigParser::operator=(const ConfigParser& other) {
  if (this != &other) {
    this->m_logger = other.m_logger;
    this->m_tokens = other.m_tokens;
    this->m_indexToken = other.m_indexToken;
    this->m_globalDirectives = other.m_globalDirectives;
  }
  return *this;
}

void ConfigParser::parsePath(const std::string& configPath) {
  try {
    this->m_tokens.clear();
    this->m_indexToken = 0;
    this->m_globalDirectives.clear();
    // servers.clear();

    lexFile(configPath);

    parseGlobal();

    // while (true) {
    //   Token token = nextToken();
    //   if (token.m_type == Token::EOF_T) {
    //     break;
    //   } else if (token.m_type == Token::BLOCK_START &&
    //              token.m_value == "server") {
    //     std::auto_ptr<domain::entities::ServerConfig> server(
    //         new domain::entities::ServerConfig());
    //     parseServerBlock(*server);
    //     servers.push_back(
    //         std::auto_ptr<domain::entities::ServerConfig>(server.release()));
    //   } else {
    //     std::ostringstream oss;
    //     oss << "Unexpected token at start of server parse: "
    //         << tokenTypeToString(token.m_type);
    //     throw shared::exceptions::ConfigException(
    //         oss.str(), shared::exceptions::ConfigException::PARSE_SYNTAX);
    //   }
    // }

    // std::ostringstream oss;
    // oss << "Parsed " << servers.size() << " server configurations from "
    //     << configPath;
    // m_logger.info(oss.str());
  } catch (const shared::exceptions::ConfigException& e) {
    throw;
  } catch (const std::exception& e) {
    throw shared::exceptions::ConfigException(
        "Unexpected error during configuration parse",
        shared::exceptions::ConfigException::LOAD_UNEXPECTED);
  }
}

void ConfigParser::lexFile(const std::string& path) {
  std::ifstream file(path.c_str());
  if (!file.is_open()) {
    throw shared::exceptions::ConfigException(
        "Config load failed: Cannot open config: " + path,
        shared::exceptions::ConfigException::LOAD_FILE_NOT_FOUND);
  }

  std::string line;
  std::size_t numLine = 0;
  while (std::getline(file, line) != 0) {
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

ConfigParser::Token ConfigParser::nextToken() {
  if (this->m_indexToken < this->m_tokens.size()) {
    return this->m_tokens[this->m_indexToken++];
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
    throw shared::exceptions::ConfigException(
        oss.str(), shared::exceptions::ConfigException::PARSE_SYNTAX);
  }
}

void ConfigParser::parseGlobal() {
  std::size_t parsedCount = 0;
  while (true) {
    Token token = nextToken();
    if (token.m_type == Token::EOF_T || token.m_type == Token::BLOCK_START) {
      if (token.m_type == Token::BLOCK_START) this->m_indexToken--;
      break;
    }
    if (token.m_type != Token::STRING) {
      expect(Token::STRING, "global directive key");
      continue;
    }

    std::string key = token.m_value;

    std::string value;
    Token valueToken = nextToken();
    while (valueToken.m_type == Token::STRING) {
      if (!value.empty()) {
        value += " ";
      }
      value += valueToken.m_value;
      valueToken = nextToken();
    }
    if (value.empty()) {
      expect(Token::STRING, "global directive value");
    }

    if (valueToken.m_type == Token::SEMICOLON) {
      this->m_globalDirectives[key] = value;
      parsedCount++;
    } else if (valueToken.m_type == Token::BLOCK_START) {
      this->m_indexToken -= 2;
      break;
    } else {
      expect(Token::SEMICOLON, "directive terminator");
    }

    this->m_globalDirectives[key] = value;
    this->m_logger.debug("Global directive: " + key + " = " + value);
  }

  std::ostringstream oss;
  oss << "Parsed " << parsedCount << " global directives.";
  this->m_logger.debug(oss.str());
}

void ConfigParser::mergeIncludes(const std::string& includePath) {
  (void)includePath;
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

}  // namespace config
}  // namespace infrastructure
