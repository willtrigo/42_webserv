/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RouteMatcher.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/24 20:41:42 by dande-je          #+#    #+#             */
/*   Updated: 2026/01/05 15:44:58 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "domain/configuration/exceptions/RouteException.hpp"
#include "domain/filesystem/value_objects/Path.hpp"
#include "domain/http/value_objects/HttpMethod.hpp"
#include "domain/http/value_objects/RouteMatchInfo.hpp"
#include "infrastructure/network/exceptions/RouteMatcherException.hpp"
#include "infrastructure/network/handlers/RouteMatcher.hpp"

#include <algorithm>
#include <sstream>

namespace infrastructure {
namespace network {
namespace handlers {

RouteMatcher::RouteMatcher() {}

void RouteMatcher::addRoute(
    const domain::configuration::value_objects::Route& route) {
  validateNoDuplicate(route);
  m_routes.push_back(route);
}

void RouteMatcher::removeRoute(
    const domain::filesystem::value_objects::Path& pathPattern) {
  std::string patternStr = pathPattern.toString();

  for (std::vector<domain::configuration::value_objects::Route>::iterator it =
           m_routes.begin();
       it != m_routes.end(); ++it) {
    if (it->getPath().toString() == patternStr) {
      m_routes.erase(it);
      return;
    }
  }
}

void RouteMatcher::clear() { m_routes.clear(); }

primitives::RouteMatchResult RouteMatcher::match(
    const std::string& httpMethod,
    const domain::filesystem::value_objects::Path& requestPath,
    const std::map<std::string, std::string>& queryParams) const {
  const domain::configuration::value_objects::Route* exactRoute =
      findExactMatch(requestPath);
  if (exactRoute != NULL) {
    domain::http::value_objects::HttpMethod method =
        domain::http::value_objects::HttpMethod::fromString(httpMethod);

    if (!exactRoute->allowsMethod(method)) {
      std::ostringstream oss;
      oss << "Method " << httpMethod << " not allowed for route "
          << exactRoute->getPath().toString();
      throw exceptions::RouteMatcherException(
          oss.str(), exceptions::RouteMatcherException::METHOD_NOT_ALLOWED);
    }

    domain::http::value_objects::RouteMatchInfo matchInfo =
        exactRoute->resolveRequest(requestPath.toString(), queryParams);

    return primitives::RouteMatchResult(exactRoute, matchInfo, true);
  }

  const domain::configuration::value_objects::Route* prefixRoute =
      findPrefixMatch(requestPath);
  if (prefixRoute != NULL) {
    domain::http::value_objects::HttpMethod method =
        domain::http::value_objects::HttpMethod::fromString(httpMethod);

    if (!prefixRoute->allowsMethod(method)) {
      std::ostringstream oss;
      oss << "Method " << httpMethod << " not allowed for route "
          << prefixRoute->getPath().toString();
      throw exceptions::RouteMatcherException(
          oss.str(), exceptions::RouteMatcherException::METHOD_NOT_ALLOWED);
    }

    domain::http::value_objects::RouteMatchInfo matchInfo =
        prefixRoute->resolveRequest(requestPath.toString(), queryParams);

    return primitives::RouteMatchResult(prefixRoute, matchInfo, false);
  }

  std::ostringstream oss;
  oss << "No route found for path: " << requestPath.toString();
  throw exceptions::RouteMatcherException(
      oss.str(), exceptions::RouteMatcherException::ROUTE_NOT_FOUND);
}

bool RouteMatcher::hasRoute(
    const domain::filesystem::value_objects::Path& pathPattern) const {
  std::string patternStr = pathPattern.toString();

  for (std::vector<domain::configuration::value_objects::Route>::const_iterator
           it = m_routes.begin();
       it != m_routes.end(); ++it) {
    if (it->getPath().toString() == patternStr) {
      return true;
    }
  }

  return false;
}

size_t RouteMatcher::count() const { return m_routes.size(); }

const std::vector<domain::configuration::value_objects::Route>&
RouteMatcher::getAllRoutes() const {
  return m_routes;
}

// TODO: refactor this func
domain::configuration::value_objects::Route RouteMatcher::createRouteFromConfig(
    const std::string& pathPattern,
    const std::map<std::string, std::string>& config) {
  try {
    domain::filesystem::value_objects::Path::fromString(pathPattern, true);
  } catch (const std::exception& e) {
    std::ostringstream oss;
    oss << "Invalid route pattern: " << pathPattern << " - " << e.what();
    throw domain::configuration::exceptions::RouteException(
        oss.str(),
        domain::configuration::exceptions::RouteException::INVALID_PATH);
  }

  std::set<domain::http::value_objects::HttpMethod> allowedMethods;

  std::map<std::string, std::string>::const_iterator iter =
      config.find("allowed_methods");
  if (iter != config.end()) {
    std::string methodsStr = iter->second;
    std::istringstream iss(methodsStr);
    std::string method;
    while (std::getline(iss, method, ',') != 0) {
      method.erase(0, method.find_first_not_of(" \t"));
      method.erase(method.find_last_not_of(" \t") + 1);

      if (!domain::http::value_objects::HttpMethod::isValidMethodString(
              method)) {
        std::ostringstream oss;
        oss << "Invalid HTTP method: " << method;
        throw domain::configuration::exceptions::RouteException(
            oss.str(),
            domain::configuration::exceptions::RouteException::INVALID_METHOD);
      }

      allowedMethods.insert(
          domain::http::value_objects::HttpMethod::fromString(method));
    }
  } else {
    allowedMethods.insert(domain::http::value_objects::HttpMethod::get());
  }

  domain::configuration::value_objects::Route::HandlerType handlerType =
      domain::configuration::value_objects::Route::STATIC_FILE;

  iter = config.find("handler_type");
  if (iter != config.end()) {
    std::string typeStr = iter->second;
    if (typeStr == "cgi") {
      handlerType = domain::configuration::value_objects::Route::CGI_EXECUTION;
    } else if (typeStr == "directory") {
      handlerType =
          domain::configuration::value_objects::Route::DIRECTORY_LISTING;
    } else if (typeStr == "redirect") {
      handlerType = domain::configuration::value_objects::Route::REDIRECT;
    } else if (typeStr == "upload") {
      handlerType = domain::configuration::value_objects::Route::UPLOAD;
    } else if (typeStr != "static") {
      std::ostringstream oss;
      oss << "Invalid handler type: " << typeStr;
      throw domain::configuration::exceptions::RouteException(
          oss.str(),
          domain::configuration::exceptions::RouteException::INVALID_HANDLER);
    }
  }

  domain::configuration::value_objects::Route route(
      domain::filesystem::value_objects::Path::fromString(pathPattern, true),
      allowedMethods, handlerType);

  iter = config.find("root");
  if (iter != config.end()) {
    route.setRootDirectory(iter->second);
  }

  iter = config.find("index");
  if (iter != config.end()) {
    route.setIndexFile(iter->second);
  }

  iter = config.find("upload_dir");
  if (iter != config.end()) {
    route.setUploadDirectory(iter->second);
  }

  iter = config.find("max_body_size");
  if (iter != config.end()) {
    try {
      domain::filesystem::value_objects::Size maxSize =
          domain::filesystem::value_objects::Size::fromString(iter->second);
      route.setMaxBodySize(maxSize);
    } catch (const std::exception& e) {
      std::ostringstream oss;
      oss << "Invalid max body size: " << iter->second << " - " << e.what();
      throw domain::configuration::exceptions::RouteException(
          oss.str(), domain::configuration::exceptions::RouteException::
                         CONFIGURATION_ERROR);
    }
  }

  iter = config.find("directory_listing");
  if (iter != config.end()) {
    std::string value = iter->second;
    std::transform(value.begin(), value.end(), value.begin(), ::tolower);
    route.setDirectoryListing(value == "on" || value == "true" ||
                              value == "yes" || value == "1");
  }

  if (handlerType ==
      domain::configuration::value_objects::Route::CGI_EXECUTION) {
    iter = config.find("cgi_interpreter");
    if (iter != config.end()) {
      std::string cgiExtension = ".php";
      std::map<std::string, std::string>::const_iterator extIt =
          config.find("cgi_extension");
      if (extIt != config.end()) {
        cgiExtension = extIt->second;
      }
      route.setCgiConfig(iter->second, cgiExtension);
    } else {
      throw domain::configuration::exceptions::RouteException(
          "Missing cgi_interpreter for CGI route",
          domain::configuration::exceptions::RouteException::
              CONFIGURATION_ERROR);
    }
  }

  if (handlerType == domain::configuration::value_objects::Route::REDIRECT) {
    iter = config.find("redirect_target");
    if (iter != config.end()) {
      domain::shared::value_objects::ErrorCode redirectCode(
          domain::shared::value_objects::ErrorCode::STATUS_MOVED_PERMANENTLY);
      std::map<std::string, std::string>::const_iterator codeIt =
          config.find("redirect_code");
      if (codeIt != config.end()) {
        try {
          redirectCode = domain::shared::value_objects::ErrorCode::fromString(
              codeIt->second);
        } catch (const std::exception& e) {
          std::ostringstream oss;
          oss << "Invalid redirect code: " << codeIt->second << " - "
              << e.what();
          throw domain::configuration::exceptions::RouteException(
              oss.str(), domain::configuration::exceptions::RouteException::
                             CONFIGURATION_ERROR);
        }
      }
      route.setRedirect(iter->second, redirectCode);
    } else {
      throw domain::configuration::exceptions::RouteException(
          "Missing redirect_target for redirect route",
          domain::configuration::exceptions::RouteException::
              CONFIGURATION_ERROR);
    }
  }

  return route;
}

const domain::configuration::value_objects::Route* RouteMatcher::findExactMatch(
    const domain::filesystem::value_objects::Path& requestPath) const {
  std::string requestStr = requestPath.toString();

  for (std::vector<domain::configuration::value_objects::Route>::const_iterator
           it = m_routes.begin();
       it != m_routes.end(); ++it) {
    if (it->getPath().toString() == requestStr) {
      return &(*it);
    }

    if (it->getPath().toString() + "/" == requestStr) {
      return &(*it);
    }
  }

  return NULL;
}

const domain::configuration::value_objects::Route*
RouteMatcher::findPrefixMatch(
    const domain::filesystem::value_objects::Path& requestPath) const {
  std::string requestStr = requestPath.toString();

  std::vector<const domain::configuration::value_objects::Route*> sortedRoutes;
  for (std::vector<domain::configuration::value_objects::Route>::const_iterator
           it = m_routes.begin();
       it != m_routes.end(); ++it) {
    sortedRoutes.push_back(&(*it));
  }

  std::sort(sortedRoutes.begin(), sortedRoutes.end(), compareRouteLength);

  for (std::vector<const domain::configuration::value_objects::Route*>::
           const_iterator it = sortedRoutes.begin();
       it != sortedRoutes.end(); ++it) {
    std::string routePattern = (*it)->getPath().toString();

    if (!routePattern.empty() &&
        routePattern[routePattern.length() - 1] == '/') {
      if (requestStr.find(routePattern) == 0) {
        return *it;
      }
    }
  }

  return NULL;
}

bool RouteMatcher::compareRouteLength(
    const domain::configuration::value_objects::Route* routeA,
    const domain::configuration::value_objects::Route* routeB) {
  return routeA->getPath().toString().length() >
         routeB->getPath().toString().length();
}

void RouteMatcher::validateNoDuplicate(
    const domain::configuration::value_objects::Route& route) const {
  std::string patternStr = route.getPath().toString();

  for (std::vector<domain::configuration::value_objects::Route>::const_iterator
           it = m_routes.begin();
       it != m_routes.end(); ++it) {
    if (it->getPath().toString() == patternStr) {
      std::ostringstream oss;
      oss << "Duplicate route pattern: " << patternStr;
      throw domain::configuration::exceptions::RouteException(
          oss.str(),
          domain::configuration::exceptions::RouteException::DUPLICATE_ROUTE);
    }
  }
}

}  // namespace handlers
}  // namespace network
}  // namespace infrastructure
