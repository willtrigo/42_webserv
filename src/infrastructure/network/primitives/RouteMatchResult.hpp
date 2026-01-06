/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RouteMatchResult.hpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/04 16:33:54 by dande-je          #+#    #+#             */
/*   Updated: 2026/01/05 15:46:57 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ROUTEMATCH_RESULT_HPP
#define ROUTEMATCH_RESULT_HPP

#include "domain/configuration/value_objects/Route.hpp"
#include "domain/http/value_objects/RouteMatchInfo.hpp"

namespace infrastructure {
namespace network {
namespace primitives {

class RouteMatchResult {
 public:
  RouteMatchResult();
  RouteMatchResult(const domain::configuration::value_objects::Route* route,
                   const domain::http::value_objects::RouteMatchInfo& matchInfo,
                   bool isExactMatch);
  RouteMatchResult(const RouteMatchResult& other);
  ~RouteMatchResult();

  RouteMatchResult& operator=(const RouteMatchResult& other);

  const domain::configuration::value_objects::Route* getRoute() const;
  domain::http::value_objects::RouteMatchInfo getMatchInfo() const;
  bool isExactMatch() const;
  bool isPrefixMatch() const;
  bool isValid() const;

  bool operator==(const RouteMatchResult& other) const;
  bool operator!=(const RouteMatchResult& other) const;

 private:
  const domain::configuration::value_objects::Route* m_route;
  domain::http::value_objects::RouteMatchInfo m_matchInfo;
  bool m_isExactMatch;
};

}  // namespace primitives
}  // namespace network
}  // namespace infrastructure

#endif  // ROUTEMATCH_RESULT_HPP
