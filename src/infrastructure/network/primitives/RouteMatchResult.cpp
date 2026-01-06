/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RouteMatchResult.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/04 16:39:16 by dande-je          #+#    #+#             */
/*   Updated: 2026/01/05 15:47:48 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "infrastructure/network/primitives/RouteMatchResult.hpp"

namespace infrastructure {
namespace network {
namespace primitives {

RouteMatchResult::RouteMatchResult() : m_route(NULL), m_isExactMatch(false) {}

RouteMatchResult::RouteMatchResult(
    const domain::configuration::value_objects::Route* route,
    const domain::http::value_objects::RouteMatchInfo& matchInfo,
    bool isExactMatch)
    : m_route(route), m_matchInfo(matchInfo), m_isExactMatch(isExactMatch) {}

RouteMatchResult::RouteMatchResult(const RouteMatchResult& other)
    : m_route(other.m_route),
      m_matchInfo(other.m_matchInfo),
      m_isExactMatch(other.m_isExactMatch) {}

RouteMatchResult::~RouteMatchResult() {}

RouteMatchResult& RouteMatchResult::operator=(const RouteMatchResult& other) {
  if (this != &other) {
    m_route = other.m_route;
    m_matchInfo = other.m_matchInfo;
    m_isExactMatch = other.m_isExactMatch;
  }
  return *this;
}

const domain::configuration::value_objects::Route* RouteMatchResult::getRoute()
    const {
  return m_route;
}

domain::http::value_objects::RouteMatchInfo RouteMatchResult::getMatchInfo()
    const {
  return m_matchInfo;
}

bool RouteMatchResult::isExactMatch() const { return m_isExactMatch; }

bool RouteMatchResult::isPrefixMatch() const {
  return isValid() && !m_isExactMatch;
}

bool RouteMatchResult::isValid() const { return m_route != NULL; }

bool RouteMatchResult::operator==(const RouteMatchResult& other) const {
  return m_route == other.m_route && m_isExactMatch == other.m_isExactMatch;
}

bool RouteMatchResult::operator!=(const RouteMatchResult& other) const {
  return !(*this == other);
}

}  // namespace primitives
}  // namespace network
}  // namespace infrastructure
