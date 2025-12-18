/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigProvider.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/17 13:33:19 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/18 11:18:20 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_PROVIDER_HPP
#define CONFIG_PROVIDER_HPP

#include "application/ports/IConfigProvider.hpp"
#include "application/ports/ILogger.hpp"

#include <map>
#include <string>
#include <vector>

namespace infrastructure {
namespace adapters {

class ConfigProvider : public application::ports::IConfigProvider {
 public:
  explicit ConfigProvider(application::ports::ILogger& logger);
  virtual ~ConfigProvider();

  virtual void load(const std::string& configPath,
                    const std::string& includePath);

  // virtual const domain::entities::ServerConfig& getServerConfig(const
  // std::string& uri) const;

  // virtual const domain::entities::LocationConfig& getLocationConfig(const
  // domain::entities::LocationConfig& server, const std::string& path) const;

  // virtual std::vector<const domain::entities::ServerConfig*> getAllServers()
  // const;
  // virtual std::size_t serverCount() const;

  virtual void reload();
  // virtual bool isValid() const;

 private:
  struct Token {
    enum Type {
      DIRECTIVE,
      BLOCK_START,
      BLOCK_END,
      STRING,
      SEMICOLON,
      EOF_T
    } m_type;
    std::string m_value;

    Token(Type type, std::string value);
  };

  ConfigProvider(const ConfigProvider& other);

  ConfigProvider& operator=(const ConfigProvider& other);

  application::ports::ILogger& m_logger;

  // std::vector<std::auto_ptr<domain::entities::ServerConfig> > m_servers;
  std::map<std::string, std::string> m_globalDirectives;
  bool m_valid;

  std::vector<Token> m_tokens;
  std::size_t m_indexToken;

  void lexFile(const std::string& path);
  static void skipWhiteSpace(const std::string& line, std::size_t& col);
  static bool isDirectiveStart(const std::string& line, std::size_t& col);
  static Token extractDirective(const std::string& line, std::size_t& col);
  static Token extractString(const std::string& line, std::size_t& col);

  Token nextToken();
  void expect(Token::Type type);
  void parseGlobal();
  // void parseServerBlock(domain::entities::ServerConfig& server);
  // void parseLocationBlock(domain::entities::LocationConfig& location);
  void mergeIncludes(const std::string& includePath);
  bool validateConfig() const;
  std::string m_configPath;
  std::string m_includePath;

  static const std::size_t K_SERVER_BLOCK_LEN = 7;
  static const std::size_t K_LOCATION_BLOCK_LEN = 9;
};

}  // namespace adapters
}  // namespace infrastructure

#endif  // CONFIG_PROVIDER_HPP
