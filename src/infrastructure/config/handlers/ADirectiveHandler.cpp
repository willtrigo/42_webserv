/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ADirectiveHandler.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/30 15:30:17 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/30 15:54:34 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "domain/filesystem/value_objects/Permission.hpp"
#include "domain/shared/utils/StringUtils.hpp"
#include "infrastructure/config/exceptions/SyntaxException.hpp"
#include "infrastructure/config/handlers/ADirectiveHandler.hpp"

#include <climits>
#include <sstream>

namespace infrastructure {
namespace config {
namespace handlers {

ADirectiveHandler::ADirectiveHandler(application::ports::ILogger& logger)
    : m_logger(logger) {}

void ADirectiveHandler::validateArgumentCount(
    const std::string& directive, const std::vector<std::string>& args,
    std::size_t expectedCount, std::size_t lineNumber) {
  if (args.size() != expectedCount) {
    std::ostringstream oss;
    oss << "Directive '" << directive << "' requires exactly " << expectedCount
        << " argument(s), but got " << args.size() << " at line " << lineNumber;
    throw exceptions::SyntaxException(
        oss.str(), exceptions::SyntaxException::INVALID_DIRECTIVE);
  }
}

void ADirectiveHandler::validateMinimumArguments(
    const std::string& directive, const std::vector<std::string>& args,
    std::size_t minCount, std::size_t lineNumber) {
  if (args.size() < minCount) {
    std::ostringstream oss;
    oss << "Directive '" << directive << "' requires at least " << minCount
        << " argument(s), but got " << args.size() << " at line " << lineNumber;
    throw exceptions::SyntaxException(
        oss.str(), exceptions::SyntaxException::INVALID_DIRECTIVE);
  }
}

unsigned int ADirectiveHandler::parseUnsignedInt(const std::string& str,
                                                 const std::string& context,
                                                 std::size_t lineNumber) {
  try {
    if (!domain::shared::utils::StringUtils::isAllDigits(str)) {
      std::ostringstream oss;
      oss << "Invalid number '" << str << "' for " << context << " at line "
          << lineNumber;
      throw exceptions::SyntaxException(
          oss.str(), exceptions::SyntaxException::INVALID_DIRECTIVE);
    }

    unsigned long result = domain::shared::utils::StringUtils::toUnsignedLong(
        str, domain::shared::utils::StringUtils::BASE_DECIMAL);

    if (result > ULONG_MAX) {
      std::ostringstream oss;
      oss << "Number '" << str << "' too large for " << context << " at line "
          << lineNumber;
      throw exceptions::SyntaxException(
          oss.str(), exceptions::SyntaxException::INVALID_DIRECTIVE);
    }

    return static_cast<unsigned int>(result);

  } catch (const std::invalid_argument& e) {
    std::ostringstream oss;
    oss << "Invalid number '" << str << "' for " << context << " at line "
        << lineNumber << ": " << e.what();
    throw exceptions::SyntaxException(
        oss.str(), exceptions::SyntaxException::INVALID_DIRECTIVE);
  } catch (const std::out_of_range& e) {
    std::ostringstream oss;
    oss << "Number '" << str << "' out of range for " << context << " at line "
        << lineNumber << ": " << e.what();
    throw exceptions::SyntaxException(
        oss.str(), exceptions::SyntaxException::INVALID_DIRECTIVE);
  }
}

unsigned int ADirectiveHandler::parseOctalPermissions(
    const std::string& str, const std::string& context,
    std::size_t lineNumber) {
  try {
    if (domain::shared::utils::StringUtils::isAllOctalDigits(str)) {
      unsigned long result = domain::shared::utils::StringUtils::toUnsignedLong(
          str, domain::shared::utils::StringUtils::BASE_OCTAL);

      if (result >
          domain::filesystem::value_objects::Permission::ALL_PERMISSIONS) {
        std::ostringstream oss;
        oss << "Octal permissions '" << str << "' out of range (0-0777) for "
            << context << " at line " << lineNumber;
        throw exceptions::SyntaxException(
            oss.str(), exceptions::SyntaxException::INVALID_DIRECTIVE);
      }

      return static_cast<unsigned int>(result);
    }

    if (domain::shared::utils::StringUtils::isAllDigits(str)) {
      unsigned int result = parseUnsignedInt(str, context, lineNumber);

      if (result >
          domain::filesystem::value_objects::Permission::ALL_PERMISSIONS) {
        std::ostringstream oss;
        oss << "Permissions '" << str << "' out of range for " << context
            << " at line " << lineNumber
            << " (expected octal like 755 or decimal up to 511)";
        throw exceptions::SyntaxException(
            oss.str(), exceptions::SyntaxException::INVALID_DIRECTIVE);
      }

      return result;
    }

    std::ostringstream oss;
    oss << "Invalid permissions format '" << str << "' for " << context
        << " at line " << lineNumber
        << " (expected octal like 755 or decimal like 493)";
    throw exceptions::SyntaxException(
        oss.str(), exceptions::SyntaxException::INVALID_DIRECTIVE);

  } catch (const exceptions::SyntaxException&) {
    throw;
  } catch (const std::exception& e) {
    std::ostringstream oss;
    oss << "Invalid permissions '" << str << "' for " << context << " at line "
        << lineNumber << ": " << e.what();
    throw exceptions::SyntaxException(
        oss.str(), exceptions::SyntaxException::INVALID_DIRECTIVE);
  }
}

}  // namespace handlers
}  // namespace config
}  // namespace infrastructure
