/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IConfigProvider.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/17 12:52:21 by dande-je          #+#    #+#             */
/*   Updated: 2026/01/02 13:39:52 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ICONFIG_PROVIDER_HPP
#define ICONFIG_PROVIDER_HPP

#include "domain/configuration/entities/HttpConfig.hpp"
#include "domain/configuration/entities/LocationConfig.hpp"
#include "domain/configuration/entities/ServerConfig.hpp"

#include <string>
#include <vector>

namespace application {
namespace ports {

class IConfigProvider {
 public:
  virtual ~IConfigProvider() {}
  virtual void load(const std::string& configPath) = 0;

  virtual const domain::configuration::entities::HttpConfig& getConfiguration()
      const = 0;

  virtual const domain::configuration::entities::ServerConfig* findServer(
      const std::string& host, unsigned int port) const = 0;

  virtual const domain::configuration::entities::LocationConfig* findLocation(
      const domain::configuration::entities::ServerConfig& server,
      const std::string& path) const = 0;

  virtual const std::vector<
      const domain::configuration::entities::ServerConfig*>&
  getAllServers() const = 0;

  virtual void reload() = 0;
  virtual bool isValid() const = 0;
};

}  // namespace ports
}  // namespace application

#endif  // ICONFIG_PROVIDER_HPP
