/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   GlobalDirectiveHandler.hpp                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/30 12:54:49 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/30 15:56:43 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GLOBAL_DIRECTIVE_HANDLER_HPP
#define GLOBAL_DIRECTIVE_HANDLER_HPP

#include "domain/configuration/entities/HttpConfig.hpp"
#include "infrastructure/config/handlers/ADirectiveHandler.hpp"

namespace infrastructure {
namespace config {
namespace handlers {

class GlobalDirectiveHandler : public ADirectiveHandler {
 public:
  GlobalDirectiveHandler(
      application::ports::ILogger& logger,
      domain::configuration::entities::HttpConfig& httpConfig);

  virtual void handle(const std::string& directive,
                      const std::vector<std::string>& args,
                      std::size_t lineNumber);

 private:
  domain::configuration::entities::HttpConfig& m_httpConfig;

  void handleWorkerProcesses(const std::vector<std::string>& args,
                             std::size_t lineNumber);
  void handleWorkerConnections(const std::vector<std::string>& args,
                               std::size_t lineNumber);
  void handleClientMaxBodySize(const std::vector<std::string>& args,
                               std::size_t lineNumber);
  void handleErrorLog(const std::vector<std::string>& args,
                      std::size_t lineNumber);
  void handleAccessLog(const std::vector<std::string>& args,
                       std::size_t lineNumber);
  void handleErrorPage(const std::vector<std::string>& args,
                       std::size_t lineNumber);
  void handleInclude(const std::vector<std::string>& args,
                     std::size_t lineNumber);
};

}  // namespace handlers
}  // namespace config
}  // namespace infrastructure

#endif  // GLOBAL_DIRECTIVE_HANDLER_HPP
