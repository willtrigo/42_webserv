/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerDirectiveHandler.cpp                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/30 16:29:22 by dande-je          #+#    #+#             */
/*   Updated: 2026/01/01 19:21:39 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "domain/configuration/exceptions/ServerConfigException.hpp"
#include "domain/shared/value_objects/ErrorCode.hpp"
#include "infrastructure/config/exceptions/SyntaxException.hpp"
#include "infrastructure/config/handlers/ServerDirectiveHandler.hpp"

#include <sstream>

namespace infrastructure {
namespace config {
namespace handlers {

ServerDirectiveHandler::ServerDirectiveHandler(
    application::ports::ILogger& logger,
    domain::configuration::entities::ServerConfig& server)
    : ADirectiveHandler(logger), m_server(server) {}

void ServerDirectiveHandler::handle(const std::string& directive,
                                    const std::vector<std::string>& args,
                                    std::size_t lineNumber) {
  if (directive == "listen") {
    handleListen(args, lineNumber);
  } else if (directive == "server_name") {
    handleServerName(args, lineNumber);
  } else if (directive == "root") {
    handleRoot(args, lineNumber);
  } else if (directive == "index") {
    handleIndex(args, lineNumber);
  } else if (directive == "error_page") {
    handleErrorPage(args, lineNumber);
  } else if (directive == "client_max_body_size") {
    handleClientMaxBodySize(args, lineNumber);
  } else if (directive == "return") {
    handleReturn(args, lineNumber);
  } else {
    std::ostringstream oss;
    oss << "Unknown server directive: '" << directive << "' at line "
        << lineNumber;
    m_logger.warn(oss.str());
  }
}

void ServerDirectiveHandler::handleListen(const std::vector<std::string>& args,
                                          std::size_t lineNumber) {
  validateMinimumArguments("listen", args, 1, lineNumber);

  std::ostringstream log;
  log << args.size();

  for (std::size_t i = 0; i < args.size(); ++i) {
    try {
      m_server.addListenDirective(args[i]);

      std::ostringstream oss;
      oss << "Added listen directive '" << args[i] << "' at line "
          << lineNumber;
      m_logger.debug(oss.str());

    } catch (const exceptions::SyntaxException&) {
      throw;
    } catch (
        const domain::configuration::exceptions::ServerConfigException& e) {
      std::ostringstream oss;
      oss << e.what() << " at line " << lineNumber;
      throw exceptions::SyntaxException(
          oss.str(), exceptions::SyntaxException::INVALID_DIRECTIVE);
    } catch (const std::exception& e) {
      std::ostringstream oss;
      oss << "Invalid listen directive '" << args[i] << "': " << e.what()
          << " at line " << lineNumber;
      throw exceptions::SyntaxException(
          oss.str(), exceptions::SyntaxException::INVALID_DIRECTIVE);
    }
  }
}

void ServerDirectiveHandler::handleServerName(
    const std::vector<std::string>& args, std::size_t lineNumber) {
  validateMinimumArguments("server_name", args, 1, lineNumber);

  for (std::size_t i = 0; i < args.size(); ++i) {
    try {
      m_server.addServerName(args[i]);

      std::ostringstream oss;
      oss << "Added server name '" << args[i] << "' at line " << lineNumber;
      m_logger.debug(oss.str());

    } catch (const exceptions::SyntaxException&) {
      throw;
    } catch (
        const domain::configuration::exceptions::ServerConfigException& e) {
      std::ostringstream oss;
      oss << "Invalid server name '" << args[i] << "': " << e.what()
          << " at line " << lineNumber;
      throw exceptions::SyntaxException(
          oss.str(), exceptions::SyntaxException::INVALID_DIRECTIVE);
    } catch (const std::exception& e) {
      std::ostringstream oss;
      oss << "Invalid server name '" << args[i] << "': " << e.what()
          << " at line " << lineNumber;
      throw exceptions::SyntaxException(
          oss.str(), exceptions::SyntaxException::INVALID_DIRECTIVE);
    }
  }
}

void ServerDirectiveHandler::handleRoot(const std::vector<std::string>& args,
                                        std::size_t lineNumber) {
  validateArgumentCount("root", args, 1, lineNumber);

  try {
    m_server.setRoot(args[0]);

    std::ostringstream oss;
    oss << "Set root to '" << args[0] << "' at line " << lineNumber;
    m_logger.debug(oss.str());

  } catch (const exceptions::SyntaxException&) {
    throw;
  } catch (const domain::configuration::exceptions::ServerConfigException& e) {
    std::ostringstream oss;
    oss << "Invalid root path '" << args[0] << "': " << e.what() << " at line "
        << lineNumber;
    throw exceptions::SyntaxException(
        oss.str(), exceptions::SyntaxException::INVALID_DIRECTIVE);
  } catch (const std::exception& e) {
    std::ostringstream oss;
    oss << "Invalid root path '" << args[0] << "': " << e.what() << " at line "
        << lineNumber;
    throw exceptions::SyntaxException(
        oss.str(), exceptions::SyntaxException::INVALID_DIRECTIVE);
  }
}

void ServerDirectiveHandler::handleIndex(const std::vector<std::string>& args,
                                         std::size_t lineNumber) {
  validateMinimumArguments("index", args, 1, lineNumber);

  for (std::size_t i = 0; i < args.size(); ++i) {
    try {
      m_server.addIndexFile(args[i]);

      std::ostringstream oss;
      oss << "Added index file '" << args[i] << "' at line " << lineNumber;
      m_logger.debug(oss.str());
    } catch (const exceptions::SyntaxException&) {
      throw;
    } catch (
        const domain::configuration::exceptions::ServerConfigException& e) {
      std::ostringstream oss;
      oss << "Invalid index file '" << args[i] << "': " << e.what()
          << " at line " << lineNumber;
      throw exceptions::SyntaxException(
          oss.str(), exceptions::SyntaxException::INVALID_DIRECTIVE);
    } catch (const std::exception& e) {
      std::ostringstream oss;
      oss << "Invalid index file '" << args[i] << "': " << e.what()
          << " at line " << lineNumber;
      throw exceptions::SyntaxException(
          oss.str(), exceptions::SyntaxException::INVALID_DIRECTIVE);
    }
  }
}

void ServerDirectiveHandler::handleErrorPage(
    const std::vector<std::string>& args, std::size_t lineNumber) {
  validateMinimumArguments("error_page", args, 2, lineNumber);

  std::string uri = args.back();

  if (uri.empty()) {
    throw exceptions::SyntaxException(
        "error_page URI cannot be empty",
        exceptions::SyntaxException::INVALID_DIRECTIVE);
  }

  for (std::size_t i = 0; i < args.size() - 1; ++i) {
    try {
      unsigned int codeValue =
          parseUnsignedInt(args[i], "error_page code", lineNumber);

      if (codeValue <
              domain::shared::value_objects::ErrorCode::MIN_VALID_CODE ||
          codeValue >
              domain::shared::value_objects::ErrorCode::MAX_VALID_CODE) {
        std::ostringstream oss;
        oss << "error_page code " << codeValue
            << " is not a valid HTTP error code (400-599) at line "
            << lineNumber;
        throw exceptions::SyntaxException(
            oss.str(), exceptions::SyntaxException::INVALID_DIRECTIVE);
      }

      domain::shared::value_objects::ErrorCode errorCode(codeValue);
      m_server.addErrorPage(errorCode, uri);

      std::ostringstream oss;
      oss << "Set error_page " << codeValue << " -> '" << uri << "' at line "
          << lineNumber;
      m_logger.debug(oss.str());

    } catch (const exceptions::SyntaxException&) {
      throw;
    } catch (
        const domain::configuration::exceptions::ServerConfigException& e) {
      std::ostringstream oss;
      oss << "Failed to set error_page for code '" << args[i]
          << "': " << e.what() << " at line " << lineNumber;
      throw exceptions::SyntaxException(
          oss.str(), exceptions::SyntaxException::INVALID_DIRECTIVE);
    } catch (const std::exception& e) {
      std::ostringstream oss;
      oss << "Failed to set error_page for code '" << args[i]
          << "': " << e.what() << " at line " << lineNumber;
      throw exceptions::SyntaxException(
          oss.str(), exceptions::SyntaxException::INVALID_DIRECTIVE);
    }
  }
}

void ServerDirectiveHandler::handleClientMaxBodySize(
    const std::vector<std::string>& args, std::size_t lineNumber) {
  validateArgumentCount("client_max_body_size", args, 1, lineNumber);

  try {
    m_server.setClientMaxBodySize(args[0]);

    std::ostringstream oss;
    oss << "Set client_max_body_size to '" << args[0] << "' at line "
        << lineNumber;
    m_logger.debug(oss.str());

  } catch (const exceptions::SyntaxException&) {
    throw;
  } catch (const domain::configuration::exceptions::ServerConfigException& e) {
    std::ostringstream oss;
    oss << "Invalid client_max_body_size '" << args[0] << "': " << e.what()
        << " at line " << lineNumber;
    throw exceptions::SyntaxException(
        oss.str(), exceptions::SyntaxException::INVALID_DIRECTIVE);
  } catch (const std::exception& e) {
    std::ostringstream oss;
    oss << "Invalid client_max_body_size '" << args[0] << "': " << e.what()
        << " at line " << lineNumber;
    throw exceptions::SyntaxException(
        oss.str(), exceptions::SyntaxException::INVALID_DIRECTIVE);
  }
}

void ServerDirectiveHandler::handleReturn(const std::vector<std::string>& args,
                                          std::size_t lineNumber) {
  validateMinimumArguments("return", args, 2, lineNumber);

  try {
    unsigned int code = parseUnsignedInt(args[0], "return code", lineNumber);
    domain::shared::value_objects::ErrorCode errorCode(code);

    if (!domain::shared::value_objects::ErrorCode::isValidErrorCode(code)) {
      std::ostringstream oss;
      oss << "return code " << code
          << " is not a valid HTTP status code ("
          << domain::shared::value_objects::ErrorCode::MIN_CODE << "-"
          << domain::shared::value_objects::ErrorCode::MAX_CODE << ") at line "
          << lineNumber;
      throw exceptions::SyntaxException(
          oss.str(), exceptions::SyntaxException::INVALID_DIRECTIVE);
    }

    if (errorCode.isRedirection()) {
      std::string redirectTarget;
      for (std::size_t i = 1; i < args.size(); ++i) {
        if (i > 1) redirectTarget += " ";
        redirectTarget += args[i];
      }
      
      m_server.setReturnRedirect(redirectTarget, code);

      std::ostringstream oss;
      oss << "Set return redirect " << code << " -> '" << redirectTarget 
          << "' at line " << lineNumber;
      m_logger.debug(oss.str());
    } 
    else if (errorCode.isSuccess() || errorCode.isClientError() || errorCode.isServerError()) {
      std::string content;
      for (std::size_t i = 1; i < args.size(); ++i) {
        if (i > 1) content += " ";
        content += args[i];
      }
      
      if (content.size() >= 2 && 
          content[0] == '"' && content[content.size() - 1] == '"') {
        content = content.substr(1, content.size() - 2);
      }
      
      m_server.setReturnContent(content, code);

      std::ostringstream oss;
      oss << "Set return content " << code << " -> '" << content << "' at line "
          << lineNumber;
      m_logger.debug(oss.str());
    } else {
      std::ostringstream oss;
      oss << "return code " << code
          << " is not a valid return code (must be 2xx, 3xx, 4xx, or 5xx) at line "
          << lineNumber;
      throw exceptions::SyntaxException(
          oss.str(), exceptions::SyntaxException::INVALID_DIRECTIVE);
    }

  } catch (const exceptions::SyntaxException&) {
    throw;
  } catch (const domain::configuration::exceptions::ServerConfigException& e) {
    std::ostringstream oss;
    oss << "Invalid return directive: " << e.what() << " at line "
        << lineNumber;
    throw exceptions::SyntaxException(
        oss.str(), exceptions::SyntaxException::INVALID_DIRECTIVE);
  } catch (const std::exception& e) {
    std::ostringstream oss;
    oss << "Invalid return directive: " << e.what() << " at line "
        << lineNumber;
    throw exceptions::SyntaxException(
        oss.str(), exceptions::SyntaxException::INVALID_DIRECTIVE);
  }
}

}  // namespace handlers
}  // namespace config
}  // namespace infrastructure
