/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerDirectiveHandler.hpp                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/30 12:55:44 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/30 16:27:40 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_DIRECTIVE_HANDLER_HPP
#define SERVER_DIRECTIVE_HANDLER_HPP

#include "domain/configuration/entities/ServerConfig.hpp"
#include "infrastructure/config/handlers/ADirectiveHandler.hpp"

namespace infrastructure {
namespace config {
namespace handlers {

class ServerDirectiveHandler : public ADirectiveHandler {
 public:
  ServerDirectiveHandler(application::ports::ILogger& logger,
                         domain::configuration::entities::ServerConfig& server);

  virtual void handle(const std::string& directive,
                      const std::vector<std::string>& args,
                      std::size_t lineNumber);

 private:
  domain::configuration::entities::ServerConfig& m_server;

  void handleListen(const std::vector<std::string>& args,
                    std::size_t lineNumber);
  void handleServerName(const std::vector<std::string>& args,
                        std::size_t lineNumber);
  void handleRoot(const std::vector<std::string>& args, std::size_t lineNumber);
  void handleIndex(const std::vector<std::string>& args,
                   std::size_t lineNumber);
  void handleErrorPage(const std::vector<std::string>& args,
                       std::size_t lineNumber);
  void handleClientMaxBodySize(const std::vector<std::string>& args,
                               std::size_t lineNumber);
  void handleReturn(const std::vector<std::string>& args,
                    std::size_t lineNumber);
};

}  // namespace handlers
}  // namespace config
}  // namespace infrastructure

#endif  // SERVER_DIRECTIVE_HANDLER_HPP
