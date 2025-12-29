/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RouteConfigAdapter.cpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/24 23:17:18 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/28 15:44:27 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "domain/entities/Route.hpp" // TODO: this implementation is not used yet
#include "domain/value_objects/ErrorCode.hpp"
#include "domain/value_objects/HttpMethod.hpp"
#include "domain/value_objects/Path.hpp"
#include "domain/value_objects/Permission.hpp"
#include "domain/value_objects/Size.hpp"
#include "infrastructure/adapters/RouteConfigAdapter.hpp"
#include "shared/exceptions/RouteException.hpp"

#include <algorithm>
#include <cctype>
#include <sstream>

namespace infrastructure {
namespace adapters {

domain::entities::Route RouteConfigAdapter::createRoute(
    const std::string& pathPattern,
    const std::map<std::string, std::string>& config) {
  if (!validateConfig(config)) {
    throw shared::exceptions::RouteException(
        "Invalid route configuration",
        shared::exceptions::RouteException::CONFIGURATION_ERROR);
  }

  domain::entities::Route::HandlerType handlerType = parseHandlerType(config);

  std::set<domain::value_objects::HttpMethod> allowedMethods;
  std::map<std::string, std::string>::const_iterator it =
      config.find("allowed_methods");
  if (it != config.end()) {
    allowedMethods = parseMethods(it->second);
  } else {
    allowedMethods.insert(domain::value_objects::HttpMethod::get());
    if (handlerType == domain::entities::Route::POST ||
        handlerType == domain::entities::Route::UPLOAD) {
      allowedMethods.insert(domain::value_objects::HttpMethod::post());
    }
    if (handlerType == domain::entities::Route::DELETE) {
      allowedMethods.insert(domain::value_objects::HttpMethod::deleteMethod());
    }
  }

  domain::entities::Route route(
      domain::value_objects::Path::fromString(pathPattern, true),
      allowedMethods, handlerType);

  applyCommonConfig(route, config);

  switch (handlerType) {
    case domain::entities::Route::CGI_EXECUTION:
      applyCgiConfig(route, config);
      break;
    case domain::entities::Route::UPLOAD:
      applyUploadConfig(route, config);
      break;
    case domain::entities::Route::REDIRECT:
      applyRedirectConfig(route, config);
      break;
    default:
      break;
  }

  return route;
}

std::vector<domain::entities::Route> RouteConfigAdapter::createRoutes(
    const std::map<std::string, std::map<std::string, std::string> >&
        serverConfig) {
  std::vector<domain::entities::Route> routes;

  for (std::map<std::string,
                std::map<std::string, std::string> >::const_iterator it =
           serverConfig.begin();
       it != serverConfig.end(); ++it) {
    try {
      domain::entities::Route route = createRoute(it->first, it->second);
      routes.push_back(route);
    } catch (const std::exception& e) {
      std::ostringstream oss;
      oss << "Failed to create route for pattern '" << it->first
          << "': " << e.what();
      throw shared::exceptions::RouteException(
          oss.str(), shared::exceptions::RouteException::CONFIGURATION_ERROR);
    }
  }

  return routes;
}

bool RouteConfigAdapter::validateConfig(
    const std::map<std::string, std::string>& config) {
  domain::entities::Route::HandlerType handlerType = parseHandlerType(config);

  switch (handlerType) {
    case domain::entities::Route::CGI_EXECUTION:
      if (config.find("cgi_interpreter") == config.end()) {
        return false;
      }
      break;

    case domain::entities::Route::REDIRECT:
      if (config.find("redirect_target") == config.end()) {
        return false;
      }
      break;

    case domain::entities::Route::UPLOAD:
      if (config.find("upload_dir") == config.end()) {
        return false;
      }
      break;

    default:
      break;
  }

  std::map<std::string, std::string>::const_iterator it =
      config.find("max_body_size");
  if (it != config.end()) {
    try {
      parseSize(it->second);
    } catch (const std::exception&) {
      return false;
    }
  }

  it = config.find("file_permissions");
  if (it != config.end()) {
    try {
      parsePermissions(it->second);
    } catch (const std::exception&) {
      return false;
    }
  }

  it = config.find("allowed_methods");
  if (it != config.end()) {
    try {
      parseMethods(it->second);
    } catch (const std::exception&) {
      return false;
    }
  }

  return true;
}

std::set<domain::value_objects::HttpMethod> RouteConfigAdapter::parseMethods(
    const std::string& methodsStr) {
  std::set<domain::value_objects::HttpMethod> methods;
  std::istringstream iss(methodsStr);
  std::string methodToken;

  while (std::getline(iss, methodToken, ',')) {
    methodToken.erase(0, methodToken.find_first_not_of(" \t"));
    methodToken.erase(methodToken.find_last_not_of(" \t") + 1);

    if (methodToken.empty()) {
      continue;
    }

    if (!domain::value_objects::HttpMethod::isValidMethodString(methodToken)) {
      std::ostringstream oss;
      oss << "Invalid HTTP method: " << methodToken;
      throw shared::exceptions::RouteException(
          oss.str(), shared::exceptions::RouteException::INVALID_METHOD);
    }

    methods.insert(domain::value_objects::HttpMethod::fromString(methodToken));
  }

  if (methods.empty()) {
    methods.insert(domain::value_objects::HttpMethod::get());
  }

  return methods;
}

domain::value_objects::Size RouteConfigAdapter::parseSize(
    const std::string& sizeStr) {
  try {
    return domain::value_objects::Size::fromString(sizeStr);
  } catch (const std::exception& e) {
    std::ostringstream oss;
    oss << "Invalid size format: " << sizeStr << " - " << e.what();
    throw shared::exceptions::RouteException(
        oss.str(), shared::exceptions::RouteException::CONFIGURATION_ERROR);
  }
}

domain::value_objects::Permission RouteConfigAdapter::parsePermissions(
    const std::string& permStr) {
  try {
    if (permStr.find_first_not_of("01234567") == std::string::npos) {
      std::istringstream iss(permStr);
      unsigned int octalValue;
      iss >> std::oct >> octalValue;
      return domain::value_objects::Permission::fromOctal(octalValue);
    }

    return domain::value_objects::Permission::fromSymbolicString(permStr);
  } catch (const std::exception& e) {
    std::ostringstream oss;
    oss << "Invalid permission format: " << permStr << " - " << e.what();
    throw shared::exceptions::RouteException(
        oss.str(), shared::exceptions::RouteException::CONFIGURATION_ERROR);
  }
}

domain::entities::Route::HandlerType RouteConfigAdapter::parseHandlerType(
    const std::map<std::string, std::string>& config) {
  std::map<std::string, std::string>::const_iterator it =
      config.find("handler_type");
  if (it != config.end()) {
    std::string typeStr = it->second;
    std::transform(typeStr.begin(), typeStr.end(), typeStr.begin(), ::tolower);

    if (typeStr == "static" || typeStr == "file") {
      return domain::entities::Route::STATIC_FILE;
    } else if (typeStr == "directory" || typeStr == "dir") {
      return domain::entities::Route::DIRECTORY_LISTING;
    } else if (typeStr == "cgi") {
      return domain::entities::Route::CGI_EXECUTION;
    } else if (typeStr == "redirect") {
      return domain::entities::Route::REDIRECT;
    } else if (typeStr == "upload") {
      return domain::entities::Route::UPLOAD;
    } else {
      std::ostringstream oss;
      oss << "Unknown handler type: " << typeStr;
      throw shared::exceptions::RouteException(
          oss.str(), shared::exceptions::RouteException::INVALID_HANDLER);
    }
  }

  // Default handler type based on configuration hints
  if (config.find("cgi_interpreter") != config.end()) {
    return domain::entities::Route::CGI_EXECUTION;
  }
  if (config.find("upload_dir") != config.end()) {
    return domain::entities::Route::UPLOAD;
  }
  if (config.find("redirect_target") != config.end()) {
    return domain::entities::Route::REDIRECT;
  }
  if (config.find("directory_listing") != config.end()) {
    std::string listing = config.find("directory_listing")->second;
    std::transform(listing.begin(), listing.end(), listing.begin(), ::tolower);
    if (listing == "on" || listing == "true" || listing == "yes" ||
        listing == "1") {
      return domain::entities::Route::DIRECTORY_LISTING;
    }
  }

  // Default to static file
  return domain::entities::Route::STATIC_FILE;
}

void RouteConfigAdapter::applyCommonConfig(
    domain::entities::Route& route,
    const std::map<std::string, std::string>& config) {
  std::map<std::string, std::string>::const_iterator it;

  // Root directory
  it = config.find("root");
  if (it != config.end()) {
    route.setRootDirectory(it->second);
  }

  // Index file
  it = config.find("index");
  if (it != config.end()) {
    route.setIndexFile(it->second);
  }

  // Max body size
  it = config.find("max_body_size");
  if (it != config.end()) {
    try {
      domain::value_objects::Size maxSize = parseSize(it->second);
      route.setMaxBodySize(maxSize);
    } catch (const std::exception& e) {
      std::ostringstream oss;
      oss << "Invalid max_body_size: " << e.what();
      throw shared::exceptions::RouteException(
          oss.str(), shared::exceptions::RouteException::CONFIGURATION_ERROR);
    }
  }

  // Directory listing
  it = config.find("directory_listing");
  if (it != config.end()) {
    std::string value = it->second;
    std::transform(value.begin(), value.end(), value.begin(), ::tolower);
    bool enable =
        (value == "on" || value == "true" || value == "yes" || value == "1");
    route.setDirectoryListing(enable);
  }

  // File permissions
  it = config.find("file_permissions");
  if (it != config.end()) {
    try {
      domain::value_objects::Permission perms = parsePermissions(it->second);
      route.setFilePermissions(perms);
    } catch (const std::exception& e) {
      std::ostringstream oss;
      oss << "Invalid file_permissions: " << e.what();
      throw shared::exceptions::RouteException(
          oss.str(), shared::exceptions::RouteException::CONFIGURATION_ERROR);
    }
  }

  // Auto index (alias for directory_listing)
  it = config.find("autoindex");
  if (it != config.end()) {
    std::string value = it->second;
    std::transform(value.begin(), value.end(), value.begin(), ::tolower);
    bool enable =
        (value == "on" || value == "true" || value == "yes" || value == "1");
    route.setDirectoryListing(enable);
  }
}

void RouteConfigAdapter::applyCgiConfig(
    domain::entities::Route& route,
    const std::map<std::string, std::string>& config) {
  std::map<std::string, std::string>::const_iterator it;

  // CGI interpreter (required)
  it = config.find("cgi_interpreter");
  if (it == config.end()) {
    throw shared::exceptions::RouteException(
        "Missing cgi_interpreter for CGI route",
        shared::exceptions::RouteException::CONFIGURATION_ERROR);
  }

  // CGI extension (optional, default .php)
  std::string cgiExtension = ".php";
  it = config.find("cgi_extension");
  if (it != config.end()) {
    cgiExtension = it->second;
  }

  // Set CGI config
  route.setCgiConfig(config.find("cgi_interpreter")->second, cgiExtension);

  // Additional CGI parameters
  it = config.find("cgi_timeout");
  if (it != config.end()) {
    // Would need to extend Route to support CGI timeout
  }
}

void RouteConfigAdapter::applyUploadConfig(
    domain::entities::Route& route,
    const std::map<std::string, std::string>& config) {
  std::map<std::string, std::string>::const_iterator it;

  // Upload directory (required)
  it = config.find("upload_dir");
  if (it == config.end()) {
    throw shared::exceptions::RouteException(
        "Missing upload_dir for upload route",
        shared::exceptions::RouteException::CONFIGURATION_ERROR);
  }

  route.setUploadDirectory(it->second);

  // Max upload size (optional, uses max_body_size if not specified)
  it = config.find("max_upload_size");
  if (it != config.end()) {
    try {
      domain::value_objects::Size maxSize = parseSize(it->second);
      route.setMaxBodySize(maxSize);
    } catch (const std::exception& e) {
      std::ostringstream oss;
      oss << "Invalid max_upload_size: " << e.what();
      throw shared::exceptions::RouteException(
          oss.str(), shared::exceptions::RouteException::CONFIGURATION_ERROR);
    }
  }

  // Allowed upload extensions (would need to extend Route to support this)
  it = config.find("allowed_extensions");
  if (it != config.end()) {
    // Parse extensions
  }
}

void RouteConfigAdapter::applyRedirectConfig(
    domain::entities::Route& route,
    const std::map<std::string, std::string>& config) {
  std::map<std::string, std::string>::const_iterator it;

  it = config.find("redirect_target");
  if (it == config.end()) {
    throw shared::exceptions::RouteException(
        "Missing redirect_target for redirect route",
        shared::exceptions::RouteException::CONFIGURATION_ERROR);
  }

  domain::value_objects::ErrorCode redirectCode(
      domain::value_objects::ErrorCode::
          STATUS_MOVED_PERMANENTLY);  // Moved Permanently
  it = config.find("redirect_code");
  if (it != config.end()) {
    try {
      redirectCode = domain::value_objects::ErrorCode::fromString(it->second);
    } catch (const std::exception& e) {
      std::ostringstream oss;
      oss << "Invalid redirect_code: " << e.what();
      throw shared::exceptions::RouteException(
          oss.str(), shared::exceptions::RouteException::CONFIGURATION_ERROR);
    }
  }

  route.setRedirect(it->second, redirectCode);

  // Redirect type (permanent/temporary - would need to extend Route)
  it = config.find("redirect_type");
  if (it != config.end()) {
    std::string type = it->second;
    std::transform(type.begin(), type.end(), type.begin(), ::tolower);
    if (type == "temporary" || type == "temp" || type == "302") {
      route.setRedirect(
          it->second,
          domain::value_objects::ErrorCode(
              domain::value_objects::ErrorCode::STATUS_FOUND));  // Found
    }
  }
}

}  // namespace adapters
}  // namespace infrastructure
