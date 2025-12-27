/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigProvider.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/17 13:33:19 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/26 23:09:08 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_PROVIDER_HPP
#define CONFIG_PROVIDER_HPP

#include "application/ports/IConfigParser.hpp"
#include "application/ports/IConfigProvider.hpp"
#include "application/ports/ILogger.hpp"
#include "domain/entities/ServerConfig.hpp"

#include <map>
#include <memory>
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

  virtual const domain::entities::ServerConfig& getServerConfig(
      const std::string& uri) const;

  virtual const domain::entities::LocationConfig& getLocationConfig(
      const domain::entities::ServerConfig& server,
      const std::string& path) const;

  virtual std::vector<const domain::entities::ServerConfig*> getAllServers()
      const;
  virtual std::size_t serverCount() const;

  virtual void reload();
  virtual bool isValid() const;

 private:
  ConfigProvider(const ConfigProvider& other);

  ConfigProvider& operator=(const ConfigProvider& other);

  application::ports::ILogger& m_logger;
  std::auto_ptr<application::ports::IConfigParser> m_parser;

  std::vector<std::auto_ptr<domain::entities::ServerConfig> > m_servers;
  std::map<std::string, std::string> m_globalDirectives;
  bool m_valid;

  std::string m_configPath;
  std::string m_includePath;

  bool validateConfig() const;
};

}  // namespace adapters
}  // namespace infrastructure

#endif  // CONFIG_PROVIDER_HPP
