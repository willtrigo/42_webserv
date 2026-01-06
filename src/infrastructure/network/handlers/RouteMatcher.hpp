/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RouteMatcher.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/24 20:39:25 by dande-je          #+#    #+#             */
/*   Updated: 2026/01/04 16:49:42 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ROUTE_MATCHER_HPP
#define ROUTE_MATCHER_HPP

#include "domain/configuration/value_objects/Route.hpp"
#include "domain/filesystem/value_objects/Path.hpp"
#include "infrastructure/network/primitives/RouteMatchResult.hpp"

#include <map>
#include <string>
#include <vector>

namespace infrastructure {
namespace network {
namespace handlers {

class RouteMatcher {
 public:
  RouteMatcher();

  void addRoute(const domain::configuration::value_objects::Route& route);
  void removeRoute(const domain::filesystem::value_objects::Path& pathPattern);
  void clear();

  primitives::RouteMatchResult match(
      const std::string& httpMethod,
      const domain::filesystem::value_objects::Path& requestPath,
      const std::map<std::string, std::string>& queryParams =
          std::map<std::string, std::string>()) const;

  bool hasRoute(
      const domain::filesystem::value_objects::Path& pathPattern) const;
  size_t count() const;
  const std::vector<domain::configuration::value_objects::Route>& getAllRoutes()
      const;

  static domain::configuration::value_objects::Route createRouteFromConfig(
      const std::string& pathPattern,
      const std::map<std::string, std::string>& config);

 private:
  std::vector<domain::configuration::value_objects::Route> m_routes;

  const domain::configuration::value_objects::Route* findExactMatch(
      const domain::filesystem::value_objects::Path& requestPath) const;
  const domain::configuration::value_objects::Route* findPrefixMatch(
      const domain::filesystem::value_objects::Path& requestPath) const;
  static bool compareRouteLength(
      const domain::configuration::value_objects::Route* routeA,
      const domain::configuration::value_objects::Route* routeB);

  void validateNoDuplicate(
      const domain::configuration::value_objects::Route& route) const;
};

}  // namespace handlers
}  // namespace network
}  // namespace infrastructure

#endif  // ROUTE_MATCHER_HPP
