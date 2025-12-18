/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IConfigProvider.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/17 12:52:21 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/18 11:16:24 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ICONFIG_PROVIDER_HPP
#define ICONFIG_PROVIDER_HPP

#include <string>

namespace application {
namespace ports {

class IConfigProvider {
 public:
  virtual ~IConfigProvider() {}
  virtual void load(const std::string& configPath,
                    const std::string& includePath) = 0;

  // virtual const domain::entities::ServerConfig& getServerConfig(const
  // std::string& uri) const = 0;

  // virtual const domain : entities::LocationConfig& getLocationConfig(
  //                            const domain::entities::ServerConfig& server,
  //                            const std::string& path) const = 0;

  // virtual std::vector<const domain::entities::ServerConfig*> getAllServers()
  // const = 0;
  // virtual std::size_t serverCount() const = 0;

  virtual void reload() = 0;

  // virtual bool isValid() const = 0;
};

}  // namespace ports
}  // namespace application

#endif  // ICONFIG_PROVIDER_HPP
