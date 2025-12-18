/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigProvider.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/17 13:53:39 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/18 13:23:11 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "application/ports/ILogger.hpp"
#include "infrastructure/adapters/ConfigProvider.hpp"
#include "shared/exceptions/ConfigException.hpp"

#include <cctype>
#include <fstream>
#include <sstream>
#include <string>

namespace infrastructure {
namespace adapters {

ConfigProvider::ConfigProvider(application::ports::ILogger& logger)
    : m_logger(logger), m_valid(false) {
  this->m_logger.info("ConfigProvider initialized.");
}

ConfigProvider::ConfigProvider(const ConfigProvider& other)
    : m_logger(other.m_logger),
      m_globalDirectives(other.m_globalDirectives),
      m_valid(other.m_valid),
      m_tokens(other.m_tokens),
      m_indexToken(other.m_indexToken),
      m_configPath(other.m_configPath),
      m_includePath(other.m_includePath) {}

ConfigProvider::~ConfigProvider() {
  this->m_tokens.clear();
  this->m_globalDirectives.clear();
}

ConfigProvider& ConfigProvider::operator=(const ConfigProvider& other) {
  if (this != &other) {
    this->m_logger = other.m_logger;
    this->m_globalDirectives = other.m_globalDirectives;
    this->m_valid = other.m_valid;
    this->m_tokens = other.m_tokens;
    this->m_indexToken = other.m_indexToken;
    this->m_configPath = other.m_configPath;
    this->m_includePath = other.m_includePath;
  }
  return *this;
}

ConfigProvider::Token::Token(Type type, std::string value)
    : m_type(type), m_value(value) {}

void ConfigProvider::load(const std::string& configPath,
                          const std::string& includePath) {
  this->m_configPath = configPath;
  this->m_includePath = includePath;

  try {
    this->m_tokens.clear();
    std::vector<Token>(this->m_tokens).swap(this->m_tokens);
    this->m_indexToken = 0;
    this->m_globalDirectives.clear();
    // this->m_servers.clear();
    this->m_valid = false;

    lexFile(configPath);
    parseGlobal();
    mergeIncludes(includePath);
    // parserServerBlock();

    if (validateConfig()) {
      this->m_valid = true;
      // this->m_logger.info("Config loaded successfully: " +
      // std::to_string(this->m_servers.size()) + " servers");
    } else {
      throw shared::exceptions::ConfigException(
          "Validation failed post-load",
          shared::exceptions::ConfigException::VALIDATION_MISSING_DIRECTIVE);
    }
  } catch (const std::exception& exception) {
    throw;
  }
}

// const domain::entities::ServerConfig& ConfigProvider::getServerConfig(const
// std::string& uri) const {
//   for (std::size_t i = 0; i < this->m_servers.size(); ++i) {
//     if (this->m_servers[i]->matches_uri(uri)) {
//       return *this->m_server[i];
//     }
//   }
//   throw "No server config for URI: " + uri;
// }

// const domain::entities::LocationConfig&
// ConfigProvider::getLocationConfig(const domain::entities::ServerConfig&
// server, const std::string& path) const {
//   const domain::entities::LocationConfig* best = nullptr;
//   std::size_t bestLen = 0;
//
//   * make logic of get the location
//   if (best) {
//     return *best;
//   }
//
//   throw "No location for path: " + path;
// }

// std::vector<const domain::entities::ServerConfig*>
// ConfigProvider::getAllServers() const {
//   std::vector<const domain::entities::ServerConfig*> allServers;
//   for (std::size_t i = 0; i < this->m_servers.size(); ++i) {
//     allServers.push_back(this->me_servers[i].get());
//   }
//   return allServers;
// }

void ConfigProvider::reload() { load(this->m_configPath, this->m_includePath); }

void ConfigProvider::lexFile(const std::string& path) {
  std::ifstream file(path.c_str());
  if (!file.is_open()) {
    throw shared::exceptions::ConfigException(
        "Config load failed: Cannot open config: " + path,
        shared::exceptions::ConfigException::LOAD_FILE_NOT_FOUND);
  }

  std::string line;
  while (std::getline(file, line) != 0) {
    std::size_t col = 0;

    skipWhiteSpace(line, col);
    if (line[col] == '#') {
      continue;
    }

    while (col < line.size()) {
      if (isDirectiveStart(line, col)) {
        this->m_tokens.push_back(extractDirective(line, col));
      } else if (line.substr(col, K_SERVER_BLOCK_LEN) == "server {") {
        this->m_tokens.push_back(Token(Token::BLOCK_START, "server"));
        col += K_SERVER_BLOCK_LEN;
      } else if (line.substr(col, K_LOCATION_BLOCK_LEN) == "location {") {
        this->m_tokens.push_back(Token(Token::BLOCK_START, "location"));
        col += K_LOCATION_BLOCK_LEN;
      } else if (line[col] == '}') {
        this->m_tokens.push_back(Token(Token::BLOCK_END, ""));
        ++col;
      } else if (line[col] == ';') {
        this->m_tokens.push_back(Token(Token::SEMICOLON, ""));
        ++col;
      } else {
        this->m_tokens.push_back(extractString(line, col));
      }
      skipWhiteSpace(line, col);
    }
  }
  this->m_tokens.push_back(Token(Token::EOF_T, ""));

  std::ostringstream str;
  str << "Lexed " << this->m_tokens.size() << " tokens from " << path;
  this->m_logger.debug(str.str());
}

void ConfigProvider::skipWhiteSpace(const std::string& line, std::size_t& col) {
  while (col < line.size() &&
         (std::isspace(static_cast<unsigned char>(line[col])) != 0)) {
    ++col;
  }
}

bool ConfigProvider::isDirectiveStart(const std::string& line,
                                      std::size_t& col) {
  if (col >= line.size()) {
    return false;
  }
  return std::isalpha(static_cast<unsigned char>(line[col])) != 0;
}

ConfigProvider::Token ConfigProvider::extractDirective(const std::string& line,
                                                       std::size_t& col) {
  std::size_t start = col;
  while (col < line.size() &&
         (std::isspace(static_cast<unsigned char>(line[col])) == 0)) {
    ++col;
  }
  std::string dirName = line.substr(start, col - start);
  return Token(Token::DIRECTIVE, dirName);
}

ConfigProvider::Token ConfigProvider::extractString(const std::string& line,
                                                    std::size_t& col) {
  std::size_t start = col;
  while (col < line.size() &&
         (std::isspace(static_cast<unsigned char>(line[col])) == 0)) {
    ++col;
  }
  std::string value = line.substr(start, col - start);
  return Token(Token::STRING, value);
}

ConfigProvider::Token ConfigProvider::nextToken() {
  if (this->m_indexToken < this->m_tokens.size()) {
    return this->m_tokens[this->m_indexToken++];
  }
  return Token(Token::EOF_T, "");
}

void ConfigProvider::expect(Token::Type type) {
  Token token = nextToken();
  if (token.m_type != type) {
    std::ostringstream str;
    str << "Expected " << type << " at line " << token.m_value;
    throw shared::exceptions::ConfigException(
        str.str(), shared::exceptions::ConfigException::PARSE_SYNTAX);
  }
}

bool ConfigProvider::validateConfig() const {
  // std::set<int> ports;
  // for (std::size_t i = 0; this->m_servers.size(); ++i) {
  //   int port = this->m_servers[i]->listen_port();
  //   if (ports.count(port)) {
  //     return false;
  //   }
  //   ports.insert(port);
  //   // root.path.validation.via.filesystem::PathResolver
  // }
  return true;
}

void ConfigProvider::parseGlobal() {
  while (true) {
    Token token = nextToken();
    if (token.m_type == Token::EOF_T || token.m_type == Token::BLOCK_START) {
      if (token.m_type == Token::BLOCK_START) {
        this->m_indexToken--;
      }
      break;
    }
    if (token.m_type != Token::DIRECTIVE) {
      expect(Token::DIRECTIVE);
      continue;
    }

    std::string key = token.m_value;

    Token valueToken = nextToken();
    if (valueToken.m_type != Token::STRING) {
      expect(Token::STRING);
    }
    std::string value = valueToken.m_value;

    expect(Token::SEMICOLON);

    this->m_globalDirectives[key] = value;
    this->m_logger.debug("Global directive: " + key + " = " + value);
  }

  std::ostringstream oss;
  oss << "Parsed " << this->m_globalDirectives.size() << " global directives.";
  this->m_logger.debug(oss.str());
}

void ConfigProvider::mergeIncludes(const std::string& includePath) {
  (void)includePath;
}

// void ConfigProvider::parseServerBlock() {}

// void ConfigProvider::parseLocationBlock() {}

// std::size_t ConfigProvider::serverCount() const {
//   return this->m_server.size();
// }

// bool ConfigProvider::isValid() const {
//   return this->m_valid && !this->m_servers.empty() && validateConfig();
// }

}  // namespace adapters
}  // namespace infrastructure
