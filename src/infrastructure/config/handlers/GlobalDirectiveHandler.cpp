/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   GlobalDirectiveHandler.cpp                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/30 16:00:52 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/30 16:10:37 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "domain/shared/value_objects/ErrorCode.hpp"
#include "infrastructure/config/exceptions/SyntaxException.hpp"
#include "infrastructure/config/handlers/GlobalDirectiveHandler.hpp"

#include <sstream>

namespace infrastructure {
namespace config {
namespace handlers {

GlobalDirectiveHandler::GlobalDirectiveHandler(
    application::ports::ILogger& logger,
    domain::configuration::entities::HttpConfig& httpConfig)
    : ADirectiveHandler(logger), m_httpConfig(httpConfig) {}

void GlobalDirectiveHandler::handle(const std::string& directive,
                                    const std::vector<std::string>& args,
                                    std::size_t lineNumber) {
  if (directive == "worker_processes") {
    handleWorkerProcesses(args, lineNumber);
  } else if (directive == "worker_connections") {
    handleWorkerConnections(args, lineNumber);
  } else if (directive == "client_max_body_size") {
    handleClientMaxBodySize(args, lineNumber);
  } else if (directive == "error_log") {
    handleErrorLog(args, lineNumber);
  } else if (directive == "access_log") {
    handleAccessLog(args, lineNumber);
  } else if (directive == "error_page") {
    handleErrorPage(args, lineNumber);
  } else if (directive == "include") {
    handleInclude(args, lineNumber);
  } else {
    std::ostringstream oss;
    oss << "Unknown global directive: '" << directive << "' at line "
        << lineNumber;
    m_logger.warn(oss.str());
  }
}

void GlobalDirectiveHandler::handleWorkerProcesses(
    const std::vector<std::string>& args, std::size_t lineNumber) {
  validateArgumentCount("worker_processes", args, 1, lineNumber);

  unsigned int processes =
      parseUnsignedInt(args[0], "worker_processes", lineNumber);

  if (processes == 0) {
    throw exceptions::SyntaxException(
        "worker_processes must be greater than 0",
        exceptions::SyntaxException::INVALID_DIRECTIVE);
  }

  m_httpConfig.setWorkerProcesses(processes);

  std::ostringstream oss;
  oss << "Set worker_processes to " << processes << " at line " << lineNumber;
  m_logger.debug(oss.str());
}

void GlobalDirectiveHandler::handleWorkerConnections(
    const std::vector<std::string>& args, std::size_t lineNumber) {
  validateArgumentCount("worker_connections", args, 1, lineNumber);

  unsigned int connections =
      parseUnsignedInt(args[0], "worker_connections", lineNumber);

  if (connections == 0) {
    throw exceptions::SyntaxException(
        "worker_connections must be greater than 0",
        exceptions::SyntaxException::INVALID_DIRECTIVE);
  }

  m_httpConfig.setWorkerConnections(connections);

  std::ostringstream oss;
  oss << "Set worker_connections to " << connections << " at line "
      << lineNumber;
  m_logger.debug(oss.str());
}

void GlobalDirectiveHandler::handleClientMaxBodySize(
    const std::vector<std::string>& args, std::size_t lineNumber) {
  validateArgumentCount("client_max_body_size", args, 1, lineNumber);

  try {
    m_httpConfig.setClientMaxBodySize(args[0]);

    std::ostringstream oss;
    oss << "Set client_max_body_size to " << args[0] << " at line "
        << lineNumber;
    m_logger.debug(oss.str());

  } catch (const std::exception& e) {
    std::ostringstream oss;
    oss << "Invalid client_max_body_size '" << args[0] << "': " << e.what()
        << " at line " << lineNumber;
    throw exceptions::SyntaxException(
        oss.str(), exceptions::SyntaxException::INVALID_DIRECTIVE);
  }
}

void GlobalDirectiveHandler::handleErrorLog(
    const std::vector<std::string>& args, std::size_t lineNumber) {
  validateArgumentCount("error_log", args, 1, lineNumber);

  m_httpConfig.setErrorLogPath(args[0]);

  std::ostringstream oss;
  oss << "Set error_log to '" << args[0] << "' at line " << lineNumber;
  m_logger.debug(oss.str());
}

void GlobalDirectiveHandler::handleAccessLog(
    const std::vector<std::string>& args, std::size_t lineNumber) {
  validateArgumentCount("access_log", args, 1, lineNumber);

  m_httpConfig.setAccessLogPath(args[0]);

  std::ostringstream oss;
  oss << "Set access_log to '" << args[0] << "' at line " << lineNumber;
  m_logger.debug(oss.str());
}

void GlobalDirectiveHandler::handleErrorPage(
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
      m_httpConfig.setErrorPage(errorCode, uri);

      std::ostringstream oss;
      oss << "Set error_page " << codeValue << " -> '" << uri << "' at line "
          << lineNumber;
      m_logger.debug(oss.str());

    } catch (const exceptions::SyntaxException&) {
      throw;
    } catch (const std::exception& e) {
      std::ostringstream oss;
      oss << "Failed to set error_page for code '" << args[i]
          << "': " << e.what() << " at line " << lineNumber;
      throw exceptions::SyntaxException(
          oss.str(), exceptions::SyntaxException::INVALID_DIRECTIVE);
    }
  }
}

void GlobalDirectiveHandler::handleInclude(const std::vector<std::string>& args,
                                           std::size_t lineNumber) {
  validateArgumentCount("include", args, 1, lineNumber);

  // Log the include directive but don't process it yet
  // TODO: make a full implementation, this would trigger recursive parsing
  std::ostringstream oss;
  oss << "Found include directive: '" << args[0] << "' at line " << lineNumber
      << " (include processing not yet implemented)";
  m_logger.info(oss.str());

  // For now, just log a warning that includes aren't processed
  m_logger.warn(
      "Include directives are logged but not processed in current "
      "implementation");
}

}  // namespace handlers
}  // namespace config
}  // namespace infrastructure
