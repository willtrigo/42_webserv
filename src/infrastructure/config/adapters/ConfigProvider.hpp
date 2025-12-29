/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigProvider.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/17 13:33:19 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/28 16:55:45 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_PROVIDER_HPP
#define CONFIG_PROVIDER_HPP

#include "application/ports/IConfigParser.hpp"
#include "application/ports/IConfigProvider.hpp"
#include "application/ports/ILogger.hpp"
#include "domain/configuration/entities/ServerConfig.hpp"

#include <memory>
#include <string>
#include <vector>

namespace infrastructure {
namespace config {
namespace adapters {

class ConfigProvider : public application::ports::IConfigProvider {
 public:
  explicit ConfigProvider(application::ports::ILogger& logger);
  virtual ~ConfigProvider();

  virtual void load(const std::string& configPath,
                    const std::string& includePath);

  virtual const domain::configuration::entities::HttpConfig& getConfiguration()
      const;

  virtual const domain::configuration::entities::ServerConfig* findServer(
      const std::string& host, unsigned int port) const;

  virtual const domain::configuration::entities::LocationConfig* findLocation(
      const domain::configuration::entities::ServerConfig& server,
      const std::string& path) const;

  virtual const std::vector<
      const domain::configuration::entities::ServerConfig*>&
  getAllServers() const;

  virtual void reload();
  virtual bool isValid() const;

 private:
  ConfigProvider(const ConfigProvider& other);
  ConfigProvider& operator=(const ConfigProvider& other);

  application::ports::ILogger& m_logger;
  std::auto_ptr<application::ports::IConfigParser> m_parser;
  std::auto_ptr<domain::configuration::entities::HttpConfig> m_httpConfig;

  std::vector<const domain::configuration::entities::ServerConfig*>
      m_serverPtrs;
  bool m_valid;

  std::string m_configPath;
  std::string m_includePath;

  void updateServerPointers();
};

}  // namespace adapters
}  // namespace config
}  // namespace infrastructure

#endif  // CONFIG_PROVIDER_HPP
