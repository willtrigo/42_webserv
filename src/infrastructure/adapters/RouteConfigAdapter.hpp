/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RouteConfigAdapter.hpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/24 21:56:24 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/24 21:56:31 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ROUTE_CONFIG_ADAPTER_HPP
#define ROUTE_CONFIG_ADAPTER_HPP

#include "domain/entities/Route.hpp"
#include "domain/value_objects/Permission.hpp"
#include "domain/value_objects/Size.hpp"

#include <map>
#include <string>
#include <vector>

namespace infrastructure {
namespace adapters {

class RouteConfigAdapter {
 public:
  // Convert configuration map to Route entity
  static domain::entities::Route createRoute(
      const std::string& pathPattern,
      const std::map<std::string, std::string>& config);

  // Create multiple routes from server configuration
  static std::vector<domain::entities::Route> createRoutes(
      const std::map<std::string, std::map<std::string, std::string> >&
          serverConfig);

  // Validate configuration
  static bool validateConfig(const std::map<std::string, std::string>& config);

  // Parse specific configuration values
  static std::set<domain::value_objects::HttpMethod> parseMethods(
      const std::string& methodsStr);
  static domain::value_objects::Size parseSize(const std::string& sizeStr);
  static domain::value_objects::Permission parsePermissions(
      const std::string& permStr);
  static domain::entities::Route::HandlerType parseHandlerType(
      const std::map<std::string, std::string>& config);

 private:
  // Helper methods for parsing specific config options
  static void applyCommonConfig(
      domain::entities::Route& route,
      const std::map<std::string, std::string>& config);
  static void applyCgiConfig(domain::entities::Route& route,
                             const std::map<std::string, std::string>& config);
  static void applyUploadConfig(
      domain::entities::Route& route,
      const std::map<std::string, std::string>& config);
  static void applyRedirectConfig(
      domain::entities::Route& route,
      const std::map<std::string, std::string>& config);
};

}  // namespace adapters
}  // namespace infrastructure

#endif  // ROUTE_CONFIG_ADAPTER_HPP
