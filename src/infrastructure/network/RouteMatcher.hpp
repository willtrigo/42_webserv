/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RouteMatcher.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/24 20:39:25 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/24 23:12:52 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ROUTE_MATCHER_HPP
#define ROUTE_MATCHER_HPP

#include "domain/entities/Route.hpp"

#include <vector>

namespace infrastructure {
namespace network {

struct MatchResult {
  const domain::entities::Route* route;
  domain::entities::Route::MatchInfo matchInfo;
  bool isExactMatch;

  MatchResult();
  bool isValid() const;
};

class RouteMatcher {
 public:
  RouteMatcher();

  void addRoute(const domain::entities::Route& route);
  void removeRoute(const domain::value_objects::Path& pathPattern);
  void clear();

  MatchResult match(const std::string& httpMethod,
                    const domain::value_objects::Path& requestPath,
                    const std::map<std::string, std::string>& queryParams =
                        std::map<std::string, std::string>()) const;

  bool hasRoute(const domain::value_objects::Path& pathPattern) const;
  size_t count() const;
  const std::vector<domain::entities::Route>& getAllRoutes() const;

  static domain::entities::Route createRouteFromConfig(
      const std::string& pathPattern,
      const std::map<std::string, std::string>& config);

 private:
  std::vector<domain::entities::Route> m_routes;

  const domain::entities::Route* findExactMatch(
      const domain::value_objects::Path& requestPath) const;
  const domain::entities::Route* findPrefixMatch(
      const domain::value_objects::Path& requestPath) const;
  static bool compareRouteLength(const domain::entities::Route* a,
                                 const domain::entities::Route* b);

  void validateNoDuplicate(const domain::entities::Route& route) const;
};

}  // namespace network
}  // namespace infrastructure

#endif  // ROUTE_MATCHER_HPP
