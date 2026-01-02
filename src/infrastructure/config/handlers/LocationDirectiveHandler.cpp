/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationDirectiveHandler.cpp                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/30 16:13:59 by dande-je          #+#    #+#             */
/*   Updated: 2026/01/02 02:01:45 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "domain/filesystem/value_objects/UploadAccess.hpp"
#include "domain/http/value_objects/HttpMethod.hpp"
#include "infrastructure/config/exceptions/SyntaxException.hpp"
#include "infrastructure/config/handlers/LocationDirectiveHandler.hpp"

#include <cstring>
#include <sstream>

namespace infrastructure {
namespace config {
namespace handlers {

LocationDirectiveHandler::LocationDirectiveHandler(
    application::ports::ILogger& logger,
    domain::configuration::entities::LocationConfig& location)
    : ADirectiveHandler(logger), m_location(location) {}

void LocationDirectiveHandler::handle(const std::string& directive,
                                      const std::vector<std::string>& args,
                                      std::size_t lineNumber) {
  if (directive == "root") {
    handleRoot(args, lineNumber);
  } else if (directive == "alias") {
    handleAlias(args, lineNumber);
  } else if (directive == "index") {
    handleIndex(args, lineNumber);
  } else if (directive == "limit_except") {
    handleLimitExcept(args, lineNumber);
  } else if (directive == "autoindex") {
    handleAutoIndex(args, lineNumber);
  } else if (directive == "try_files") {
    handleTryFiles(args, lineNumber);
  } else if (directive == "return") {
    handleReturn(args, lineNumber);
  } else if (directive == "upload_store") {
    handleUploadStore(args, lineNumber);
  } else if (directive == "upload_store_permissions") {
    handleUploadPermissions(args, lineNumber);
  } else if (directive == "upload_store_access") {
    handleUploadAccess(args, lineNumber);
  } else if (directive == "script") {
    handleScript(args, lineNumber);
  } else if (directive == "cgi_root") {
    handleCgiRoot(args, lineNumber);
  } else if (directive == "fastcgi_param") {
    handleFastcgiParam(args, lineNumber);
  } else if (directive == "upload_max_file_size" ||
             directive == "upload_max_total_size") {
    handleUploadSizeLimits(directive, args, lineNumber);
  } else if (directive == "add_header") {
    handleAddHeader(args, lineNumber);
  } else if (directive == "include") {
    std::ostringstream oss;
    oss << "Include directive in location context (not yet implemented): "
        << args[0] << " at line " << lineNumber;
    m_logger.info(oss.str());
  } else {
    std::ostringstream oss;
    oss << "Unknown or unsupported location directive: '" << directive
        << "' at line " << lineNumber;
    m_logger.warn(oss.str());
  }
}

void LocationDirectiveHandler::handleRoot(const std::vector<std::string>& args,
                                          std::size_t lineNumber) {
  validateArgumentCount("root", args, 1, lineNumber);

  try {
    m_location.setRoot(args[0]);

    std::ostringstream oss;
    oss << "Set root to '" << args[0] << "' at line " << lineNumber;
    m_logger.debug(oss.str());

  } catch (const std::exception& e) {
    std::ostringstream oss;
    oss << "Invalid root path '" << args[0] << "': " << e.what() << " at line "
        << lineNumber;
    throw exceptions::SyntaxException(
        oss.str(), exceptions::SyntaxException::INVALID_DIRECTIVE);
  }
}

void LocationDirectiveHandler::handleAlias(const std::vector<std::string>& args,
                                           std::size_t lineNumber) {
  validateArgumentCount("alias", args, 1, lineNumber);

  try {
    m_location.setAlias(args[0]);

    std::ostringstream oss;
    oss << "Set alias to '" << args[0] << "' at line " << lineNumber;
    m_logger.debug(oss.str());

  } catch (const std::exception& e) {
    std::ostringstream oss;
    oss << "Invalid alias path '" << args[0] << "': " << e.what() << " at line "
        << lineNumber;
    throw exceptions::SyntaxException(
        oss.str(), exceptions::SyntaxException::INVALID_DIRECTIVE);
  }
}

void LocationDirectiveHandler::handleIndex(const std::vector<std::string>& args,
                                           std::size_t lineNumber) {
  validateMinimumArguments("index", args, 1, lineNumber);

  for (std::size_t i = 0; i < args.size(); ++i) {
    try {
      m_location.addIndexFile(args[i]);

      std::ostringstream oss;
      oss << "Added index file '" << args[i] << "' at line " << lineNumber;
      m_logger.debug(oss.str());

    } catch (const std::exception& e) {
      std::ostringstream oss;
      oss << "Invalid index file '" << args[i] << "': " << e.what()
          << " at line " << lineNumber;
      throw exceptions::SyntaxException(
          oss.str(), exceptions::SyntaxException::INVALID_DIRECTIVE);
    }
  }
}

void LocationDirectiveHandler::handleLimitExcept(
    const std::vector<std::string>& args, std::size_t lineNumber) {
  validateMinimumArguments("limit_except", args, 1, lineNumber);

  std::vector<std::string> methodStrings;
  std::size_t i = 0;

  while (i < args.size() && args[i] != "{") {
    methodStrings.push_back(args[i]);
    ++i;
  }

  if (methodStrings.empty()) {
    throw exceptions::SyntaxException(
        "limit_except requires at least one HTTP method before '{'",
        exceptions::SyntaxException::INVALID_DIRECTIVE);
  }

  for (std::size_t j = 0; j < methodStrings.size(); ++j) {
    try {
      domain::http::value_objects::HttpMethod method(methodStrings[j]);
      m_location.addAllowedMethod(method);

      std::ostringstream oss;
      oss << "Added allowed method '" << methodStrings[j]
          << "' via limit_except at line " << lineNumber;
      m_logger.debug(oss.str());

    } catch (const std::exception& e) {
      std::ostringstream oss;
      oss << "Invalid HTTP method in limit_except: '" << methodStrings[j]
          << "': " << e.what() << " at line " << lineNumber;
      throw exceptions::SyntaxException(
          oss.str(), exceptions::SyntaxException::INVALID_DIRECTIVE);
    }
  }

  if (i < args.size() && args[i] == "{") {
    if (i + 2 < args.size() && args[i + 1] == "deny" && args[i + 2] == "all") {
      std::ostringstream oss;
      oss << "Found 'deny all' pattern in limit_except at line " << lineNumber;
      m_logger.debug(oss.str());
    } else {
      m_logger.warn(
          "limit_except block doesn't follow expected 'deny all' pattern");
    }
  }
}

void LocationDirectiveHandler::handleAutoIndex(
    const std::vector<std::string>& args, std::size_t lineNumber) {
  validateArgumentCount("autoindex", args, 1, lineNumber);

  std::string value = args[0];
  if (value == "on") {
    m_location.setAutoIndex(true);
  } else if (value == "off") {
    m_location.setAutoIndex(false);
  } else {
    throw exceptions::SyntaxException(
        "autoindex requires 'on' or 'off', got '" + value + "'",
        exceptions::SyntaxException::INVALID_DIRECTIVE);
  }

  std::ostringstream oss;
  oss << "Set autoindex to '" << value << "' at line " << lineNumber;
  m_logger.debug(oss.str());
}

void LocationDirectiveHandler::handleTryFiles(
    const std::vector<std::string>& args, std::size_t lineNumber) {
  validateMinimumArguments("try_files", args, 1, lineNumber);

  try {
    m_location.setTryFiles(args);

    std::ostringstream oss;
    oss << "Set try_files with " << args.size() << " arguments at line "
        << lineNumber;
    m_logger.debug(oss.str());

  } catch (const std::exception& e) {
    std::ostringstream oss;
    oss << "Invalid try_files directive: " << e.what() << " at line "
        << lineNumber;
    throw exceptions::SyntaxException(
        oss.str(), exceptions::SyntaxException::INVALID_DIRECTIVE);
  }
}

void LocationDirectiveHandler::handleReturn(
    const std::vector<std::string>& args, std::size_t lineNumber) {
  validateMinimumArguments("return", args, 2, lineNumber);

  try {
    unsigned int code = parseUnsignedInt(args[0], "return code", lineNumber);
    domain::shared::value_objects::ErrorCode errorCode(code);

    // Validate HTTP status code range using ErrorCode constants
    if (!domain::shared::value_objects::ErrorCode::isValidErrorCode(code)) {
      std::ostringstream oss;
      oss << "return code " << code << " is not a valid HTTP status code ("
          << domain::shared::value_objects::ErrorCode::MIN_CODE << "-"
          << domain::shared::value_objects::ErrorCode::MAX_CODE << ") at line "
          << lineNumber;
      throw exceptions::SyntaxException(
          oss.str(), exceptions::SyntaxException::INVALID_DIRECTIVE);
    }

    // For redirect codes (3xx), use setReturnRedirect
    if (errorCode.isRedirection()) {
      // Join all remaining arguments as the redirect URL
      std::string redirectTarget;
      for (std::size_t i = 1; i < args.size(); ++i) {
        if (i > 1) redirectTarget += " ";
        redirectTarget += args[i];
      }

      m_location.setReturnRedirect(redirectTarget, code);

      std::ostringstream oss;
      oss << "Set return " << code << " -> '" << redirectTarget << "' at line "
          << lineNumber;
      m_logger.debug(oss.str());
    }
    // For success (2xx), client error (4xx), or server error (5xx) codes, use
    // setReturnContent
    else if (errorCode.isSuccess() || errorCode.isClientError() ||
             errorCode.isServerError()) {
      // Join all remaining arguments as the response content
      std::string content;
      for (std::size_t i = 1; i < args.size(); ++i) {
        if (i > 1) content += " ";
        content += args[i];
      }

      // Remove surrounding quotes if present
      if (content.size() >= 2 && content[0] == '"' &&
          content[content.size() - 1] == '"') {
        content = content.substr(1, content.size() - 2);
      }

      m_location.setReturnContent(content, code);

      std::ostringstream oss;
      oss << "Set return content " << code << " -> '" << content << "' at line "
          << lineNumber;
      m_logger.debug(oss.str());
    } else {
      // This should not happen due to the range check above, but handle it
      std::ostringstream oss;
      oss << "return code " << code
          << " is not a valid return code (must be 2xx, 3xx, 4xx, or 5xx) at "
             "line "
          << lineNumber;
      throw exceptions::SyntaxException(
          oss.str(), exceptions::SyntaxException::INVALID_DIRECTIVE);
    }

  } catch (const std::exception& e) {
    std::ostringstream oss;
    oss << "Invalid return directive: " << e.what() << " at line "
        << lineNumber;
    throw exceptions::SyntaxException(
        oss.str(), exceptions::SyntaxException::INVALID_DIRECTIVE);
  }
}

void LocationDirectiveHandler::handleUploadStore(
    const std::vector<std::string>& args, std::size_t lineNumber) {
  validateArgumentCount("upload_store", args, 1, lineNumber);

  try {
    m_location.setUploadDirectory(args[0]);

    std::ostringstream oss;
    oss << "Set upload_store to '" << args[0] << "' at line " << lineNumber;
    m_logger.debug(oss.str());

  } catch (const std::exception& e) {
    std::ostringstream oss;
    oss << "Invalid upload_store path '" << args[0] << "': " << e.what()
        << " at line " << lineNumber;
    throw exceptions::SyntaxException(
        oss.str(), exceptions::SyntaxException::INVALID_DIRECTIVE);
  }
}

void LocationDirectiveHandler::handleUploadPermissions(
    const std::vector<std::string>& args, std::size_t lineNumber) {
  validateArgumentCount("upload_store_permissions", args, 1, lineNumber);

  try {
    unsigned int permissions =
        parseOctalPermissions(args[0], "upload_store_permissions", lineNumber);
    m_location.setUploadPermissions(permissions);

    std::ostringstream oss;
    oss << "Set upload_store_permissions to " << std::oct << permissions
        << std::dec << " (" << permissions << " decimal) at line "
        << lineNumber;
    m_logger.debug(oss.str());

  } catch (const std::exception& e) {
    std::ostringstream oss;
    oss << "Invalid upload_store_permissions '" << args[0] << "': " << e.what()
        << " at line " << lineNumber;
    throw exceptions::SyntaxException(
        oss.str(), exceptions::SyntaxException::INVALID_DIRECTIVE);
  }
}

void LocationDirectiveHandler::handleUploadAccess(
    const std::vector<std::string>& args, std::size_t lineNumber) {
  validateMinimumArguments("upload_store_access", args, 1, lineNumber);

  try {
    for (std::size_t i = 0; i < args.size(); ++i) {
      domain::filesystem::value_objects::UploadAccess access =
          domain::filesystem::value_objects::UploadAccess::fromString(args[i]);

      m_location.setUploadAccess(args[i]);

      std::ostringstream oss;
      oss << "Set upload_store_access component '" << args[i] << "' at line "
          << lineNumber;
      m_logger.debug(oss.str());
    }

  } catch (const std::exception& e) {
    std::ostringstream oss;
    oss << "Invalid upload_store_access: " << e.what() << " at line "
        << lineNumber;
    throw exceptions::SyntaxException(
        oss.str(), exceptions::SyntaxException::INVALID_DIRECTIVE);
  }
}

void LocationDirectiveHandler::handleScript(
    const std::vector<std::string>& args, std::size_t lineNumber) {
  validateArgumentCount("script", args, 1, lineNumber);

  try {
    domain::configuration::value_objects::CgiConfig cgiConfig;
    try {
      cgiConfig = m_location.getCgiConfig();
      cgiConfig.setScriptPath(args[0]);
    } catch (...) {
      domain::filesystem::value_objects::Path emptyRoot;
      domain::shared::value_objects::RegexPattern defaultPattern(
          "\\.(php|py|pl|cgi)$");
      cgiConfig = domain::configuration::value_objects::CgiConfig(
          args[0], emptyRoot, defaultPattern);
    }

    m_location.setCgiConfig(cgiConfig);

    std::ostringstream oss;
    oss << "Set script path to '" << args[0] << "' at line " << lineNumber;
    m_logger.debug(oss.str());

  } catch (const std::exception& e) {
    std::ostringstream oss;
    oss << "Invalid script path '" << args[0] << "': " << e.what()
        << " at line " << lineNumber;
    throw exceptions::SyntaxException(
        oss.str(), exceptions::SyntaxException::INVALID_DIRECTIVE);
  }
}

void LocationDirectiveHandler::handleCgiRoot(
    const std::vector<std::string>& args, std::size_t lineNumber) {
  validateArgumentCount("cgi_root", args, 1, lineNumber);

  try {
    domain::configuration::value_objects::CgiConfig cgiConfig;
    try {
      cgiConfig = m_location.getCgiConfig();
      domain::filesystem::value_objects::Path cgiRoot(args[0], false);
      cgiConfig.setCgiRoot(cgiRoot);
    } catch (...) {
      domain::filesystem::value_objects::Path cgiRoot(args[0], false);
      domain::shared::value_objects::RegexPattern defaultPattern(
          "\\.(php|py|pl|cgi)$");
      cgiConfig = domain::configuration::value_objects::CgiConfig(
          "", cgiRoot, defaultPattern);
    }

    m_location.setCgiConfig(cgiConfig);

    std::ostringstream oss;
    oss << "Set cgi_root to '" << args[0] << "' at line " << lineNumber;
    m_logger.debug(oss.str());

  } catch (const std::exception& e) {
    std::ostringstream oss;
    oss << "Invalid cgi_root path '" << args[0] << "': " << e.what()
        << " at line " << lineNumber;
    throw exceptions::SyntaxException(
        oss.str(), exceptions::SyntaxException::INVALID_DIRECTIVE);
  }
}

void LocationDirectiveHandler::handleFastcgiParam(
    const std::vector<std::string>& args, std::size_t lineNumber) {
  validateMinimumArguments("fastcgi_param", args, 2, lineNumber);

  std::string paramName = args[0];
  std::string paramValue;

  for (std::size_t i = 1; i < args.size(); ++i) {
    if (i > 1) paramValue += " ";
    paramValue += args[i];
  }

  try {
    domain::configuration::value_objects::CgiConfig cgiConfig;
    try {
      cgiConfig = m_location.getCgiConfig();
    } catch (...) {
      domain::filesystem::value_objects::Path emptyRoot;
      domain::shared::value_objects::RegexPattern defaultPattern(
          "\\.(php|py|pl|cgi)$");
      cgiConfig = domain::configuration::value_objects::CgiConfig(
          "", emptyRoot, defaultPattern);
    }

    cgiConfig.addParameter(paramName, paramValue);
    m_location.setCgiConfig(cgiConfig);

    std::ostringstream oss;
    oss << "Added fastcgi_param '" << paramName << "' = '" << paramValue
        << "' at line " << lineNumber;
    m_logger.debug(oss.str());

  } catch (const std::exception& e) {
    std::ostringstream oss;
    oss << "Invalid fastcgi_param: " << e.what() << " at line " << lineNumber;
    throw exceptions::SyntaxException(
        oss.str(), exceptions::SyntaxException::INVALID_DIRECTIVE);
  }
}

void LocationDirectiveHandler::handleUploadSizeLimits(
    const std::string& directive, const std::vector<std::string>& args,
    std::size_t lineNumber) {
  validateArgumentCount(directive, args, 1, lineNumber);

  try {
    if (directive == "upload_max_file_size") {
      m_location.setUploadMaxFileSize(args[0]);
    } else if (directive == "upload_max_total_size") {
      m_location.setUploadMaxTotalSize(args[0]);
    }

    std::ostringstream oss;
    oss << "Set " << directive << " to '" << args[0] << "' at line "
        << lineNumber;
    m_logger.debug(oss.str());

  } catch (const std::exception& e) {
    std::ostringstream oss;
    oss << "Invalid " << directive << " value '" << args[0] << "': " << e.what()
        << " at line " << lineNumber;
    throw exceptions::SyntaxException(
        oss.str(), exceptions::SyntaxException::INVALID_DIRECTIVE);
  }
}

void LocationDirectiveHandler::handleAddHeader(
    const std::vector<std::string>& args, std::size_t lineNumber) {
  validateMinimumArguments("add_header", args, 2, lineNumber);

  std::string headerName = args[0];
  std::string headerValue;

  for (std::size_t i = 1; i < args.size(); ++i) {
    if (i > 1) headerValue += " ";
    headerValue += args[i];
  }

  try {
    m_location.addCustomHeader(headerName, headerValue);

    std::ostringstream oss;
    oss << "Added custom header '" << headerName << ": " << headerValue
        << "' at line " << lineNumber;
    m_logger.debug(oss.str());

  } catch (const std::exception& e) {
    std::ostringstream oss;
    oss << "Invalid add_header directive: " << e.what() << " at line "
        << lineNumber;
    throw exceptions::SyntaxException(
        oss.str(), exceptions::SyntaxException::INVALID_DIRECTIVE);
  }
}

}  // namespace handlers
}  // namespace config
}  // namespace infrastructure
