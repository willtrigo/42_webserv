/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ADirectiveHandler.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/30 12:54:54 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/30 15:56:50 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DIRECTIVE_HANDLER_HPP
#define DIRECTIVE_HANDLER_HPP

#include "application/ports/ILogger.hpp"

#include <string>
#include <vector>

namespace infrastructure {
namespace config {
namespace handlers {

class ADirectiveHandler {
 public:
  explicit ADirectiveHandler(application::ports::ILogger& logger);
  virtual ~ADirectiveHandler() {}

  virtual void handle(const std::string& directive,
                      const std::vector<std::string>& args,
                      std::size_t lineNumber) = 0;

 protected:
  application::ports::ILogger& m_logger;

  static void validateArgumentCount(const std::string& directive,
                                    const std::vector<std::string>& args,
                                    std::size_t expectedCount,
                                    std::size_t lineNumber);
  static void validateMinimumArguments(const std::string& directive,
                                       const std::vector<std::string>& args,
                                       std::size_t minCount,
                                       std::size_t lineNumber);
  static unsigned int parseUnsignedInt(const std::string& str,
                                       const std::string& context,
                                       std::size_t lineNumber);
  static unsigned int parseOctalPermissions(const std::string& str,
                                            const std::string& context,
                                            std::size_t lineNumber);
};

}  // namespace handlers
}  // namespace config
}  // namespace infrastructure

#endif  // DIRECTIVE_HANDLER_HPP
