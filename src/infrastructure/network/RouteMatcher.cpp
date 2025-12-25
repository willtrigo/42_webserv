/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RouteMatcher.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/24 20:41:42 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/24 23:13:21 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "domain/value_objects/HttpMethod.hpp"
#include "domain/value_objects/Path.hpp"
#include "infrastructure/network/RouteMatcher.hpp"
#include "shared/exceptions/RouteException.hpp"

#include <algorithm>
#include <sstream>

namespace infrastructure {
namespace network {

MatchResult::MatchResult() : route(NULL), isExactMatch(false) {}

bool MatchResult::isValid() const { return route != NULL; }

RouteMatcher::RouteMatcher() {}

void RouteMatcher::addRoute(const domain::entities::Route& route) {
  validateNoDuplicate(route);

  m_routes.push_back(route);
}

void RouteMatcher::removeRoute(const domain::value_objects::Path& pathPattern) {
  std::string patternStr = pathPattern.toString();

  for (std::vector<domain::entities::Route>::iterator it = m_routes.begin();
       it != m_routes.end(); ++it) {
    if (it->getPath().toString() == patternStr) {
      m_routes.erase(it);
      return;
    }
  }
}

void RouteMatcher::clear() { m_routes.clear(); }

MatchResult RouteMatcher::match(
    const std::string& httpMethod,
    const domain::value_objects::Path& requestPath,
    const std::map<std::string, std::string>& queryParams) const {
  MatchResult result;

  const domain::entities::Route* exactRoute = findExactMatch(requestPath);
  if (exactRoute) {
    domain::value_objects::HttpMethod method =
        domain::value_objects::HttpMethod::fromString(httpMethod);

    if (!exactRoute->allowsMethod(method)) {
      std::ostringstream oss;
      oss << "Method " << httpMethod << " not allowed for route "
          << exactRoute->getPath().toString();
      throw shared::exceptions::RouteException(
          oss.str(), shared::exceptions::RouteException::METHOD_NOT_ALLOWED);
    }

    result.route = exactRoute;
    result.isExactMatch = true;
    result.matchInfo =
        exactRoute->resolveRequest(requestPath.toString(), queryParams);
    return result;
  }

  const domain::entities::Route* prefixRoute = findPrefixMatch(requestPath);
  if (prefixRoute != 0) {
    domain::value_objects::HttpMethod method =
        domain::value_objects::HttpMethod::fromString(httpMethod);

    if (!prefixRoute->allowsMethod(method)) {
      std::ostringstream oss;
      oss << "Method " << httpMethod << " not allowed for route "
          << prefixRoute->getPath().toString();
      throw shared::exceptions::RouteException(
          oss.str(), shared::exceptions::RouteException::METHOD_NOT_ALLOWED);
    }

    result.route = prefixRoute;
    result.isExactMatch = false;
    result.matchInfo =
        prefixRoute->resolveRequest(requestPath.toString(), queryParams);
    return result;
  }

  // No route found
  std::ostringstream oss;
  oss << "No route found for path: " << requestPath.toString();
  throw shared::exceptions::RouteException(
      oss.str(), shared::exceptions::RouteException::ROUTE_NOT_FOUND);
}

bool RouteMatcher::hasRoute(
    const domain::value_objects::Path& pathPattern) const {
  std::string patternStr = pathPattern.toString();

  for (std::vector<domain::entities::Route>::const_iterator it =
           m_routes.begin();
       it != m_routes.end(); ++it) {
    if (it->getPath().toString() == patternStr) {
      return true;
    }
  }

  return false;
}

size_t RouteMatcher::count() const { return m_routes.size(); }

const std::vector<domain::entities::Route>& RouteMatcher::getAllRoutes() const {
  return m_routes;
}

domain::entities::Route RouteMatcher::createRouteFromConfig(
    const std::string& pathPattern,
    const std::map<std::string, std::string>& config) {
  try {
    domain::value_objects::Path::fromString(pathPattern, true);
  } catch (const std::exception& e) {
    std::ostringstream oss;
    oss << "Invalid route pattern: " << pathPattern << " - " << e.what();
    throw shared::exceptions::RouteException(
        oss.str(), shared::exceptions::RouteException::INVALID_PATH);
  }

  std::set<domain::value_objects::HttpMethod> allowedMethods;

  std::map<std::string, std::string>::const_iterator iter =
      config.find("allowed_methods");
  if (iter != config.end()) {
    std::string methodsStr = iter->second;
    std::istringstream iss(methodsStr);
    std::string method;
    while (std::getline(iss, method, ',') != 0) {
      // Trim whitespace
      method.erase(0, method.find_first_not_of(" \t"));
      method.erase(method.find_last_not_of(" \t") + 1);

      if (!domain::value_objects::HttpMethod::isValidMethodString(method)) {
        std::ostringstream oss;
        oss << "Invalid HTTP method: " << method;
        throw shared::exceptions::RouteException(
            oss.str(), shared::exceptions::RouteException::INVALID_METHOD);
      }

      allowedMethods.insert(
          domain::value_objects::HttpMethod::fromString(method));
    }
  } else {
    allowedMethods.insert(domain::value_objects::HttpMethod::get());
  }

  domain::entities::Route::HandlerType handlerType =
      domain::entities::Route::STATIC_FILE;

  iter = config.find("handler_type");
  if (iter != config.end()) {
    std::string typeStr = iter->second;
    if (typeStr == "cgi") {
      handlerType = domain::entities::Route::CGI_EXECUTION;
    } else if (typeStr == "directory") {
      handlerType = domain::entities::Route::DIRECTORY_LISTING;
    } else if (typeStr == "redirect") {
      handlerType = domain::entities::Route::REDIRECT;
    } else if (typeStr == "upload") {
      handlerType = domain::entities::Route::UPLOAD;
    } else if (typeStr != "static") {
      std::ostringstream oss;
      oss << "Invalid handler type: " << typeStr;
      throw shared::exceptions::RouteException(
          oss.str(), shared::exceptions::RouteException::INVALID_HANDLER);
    }
  }

  domain::entities::Route route(
      domain::value_objects::Path::fromString(pathPattern, true),
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
      domain::value_objects::Size maxSize =
          domain::value_objects::Size::fromString(iter->second);
      route.setMaxBodySize(maxSize);
    } catch (const std::exception& e) {
      std::ostringstream oss;
      oss << "Invalid max body size: " << iter->second << " - " << e.what();
      throw shared::exceptions::RouteException(
          oss.str(), shared::exceptions::RouteException::CONFIGURATION_ERROR);
    }
  }

  iter = config.find("directory_listing");
  if (iter != config.end()) {
    std::string value = iter->second;
    std::transform(value.begin(), value.end(), value.begin(), ::tolower);
    route.setDirectoryListing(value == "on" || value == "true" ||
                              value == "yes" || value == "1");
  }

  if (handlerType == domain::entities::Route::CGI_EXECUTION) {
    iter = config.find("cgi_interpreter");
    if (iter != config.end()) {
      std::string cgiExtension = ".php";  // Default
      std::map<std::string, std::string>::const_iterator extIt =
          config.find("cgi_extension");
      if (extIt != config.end()) {
        cgiExtension = extIt->second;
      }
      route.setCgiConfig(iter->second, cgiExtension);
    } else {
      throw shared::exceptions::RouteException(
          "Missing cgi_interpreter for CGI route",
          shared::exceptions::RouteException::CONFIGURATION_ERROR);
    }
  }

  if (handlerType == domain::entities::Route::REDIRECT) {
    iter = config.find("redirect_target");
    if (iter != config.end()) {
      domain::value_objects::ErrorCode redirectCode(
          301);  // Default: Moved Permanently
      std::map<std::string, std::string>::const_iterator codeIt =
          config.find("redirect_code");
      if (codeIt != config.end()) {
        try {
          redirectCode =
              domain::value_objects::ErrorCode::fromString(codeIt->second);
        } catch (const std::exception& e) {
          std::ostringstream oss;
          oss << "Invalid redirect code: " << codeIt->second << " - "
              << e.what();
          throw shared::exceptions::RouteException(
              oss.str(),
              shared::exceptions::RouteException::CONFIGURATION_ERROR);
        }
      }
      route.setRedirect(iter->second, redirectCode);
    } else {
      throw shared::exceptions::RouteException(
          "Missing redirect_target for redirect route",
          shared::exceptions::RouteException::CONFIGURATION_ERROR);
    }
  }

  return route;
}

const domain::entities::Route* RouteMatcher::findExactMatch(
    const domain::value_objects::Path& requestPath) const {
  std::string requestStr = requestPath.toString();

  for (std::vector<domain::entities::Route>::const_iterator it =
           m_routes.begin();
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


const domain::entities::Route* RouteMatcher::findPrefixMatch(
    const domain::value_objects::Path& requestPath) const {
  
  std::string requestStr = requestPath.toString();
  
  // Create vector of route pointers
  std::vector<const domain::entities::Route*> sortedRoutes;
  for (std::vector<domain::entities::Route>::const_iterator it = m_routes.begin();
       it != m_routes.end(); ++it) {
    sortedRoutes.push_back(&(*it));
  }
  
  // Sort by path length (longest first) using function pointer
  std::sort(sortedRoutes.begin(), sortedRoutes.end(), compareRouteLength);

  for (std::vector<const domain::entities::Route*>::const_iterator it =
           sortedRoutes.begin();
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

bool RouteMatcher::compareRouteLength(const domain::entities::Route* a, 
                               const domain::entities::Route* b) {
  return a->getPath().toString().length() > b->getPath().toString().length();
}

void RouteMatcher::validateNoDuplicate(
    const domain::entities::Route& route) const {
  std::string patternStr = route.getPath().toString();

  for (std::vector<domain::entities::Route>::const_iterator it =
           m_routes.begin();
       it != m_routes.end(); ++it) {
    if (it->getPath().toString() == patternStr) {
      std::ostringstream oss;
      oss << "Duplicate route pattern: " << patternStr;
      throw shared::exceptions::RouteException(
          oss.str(), shared::exceptions::RouteException::DUPLICATE_ROUTE);
    }
  }
}

}  // namespace network
}  // namespace infrastructure
