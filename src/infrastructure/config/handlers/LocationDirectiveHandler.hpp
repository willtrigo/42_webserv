/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationDirectiveHandler.hpp                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/30 14:39:01 by dande-je          #+#    #+#             */
/*   Updated: 2026/01/11 16:11:47 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOCATION_DIRECTIVE_HANDLER_HPP
#define LOCATION_DIRECTIVE_HANDLER_HPP

#include "domain/configuration/entities/LocationConfig.hpp"
#include "infrastructure/config/handlers/ADirectiveHandler.hpp"

namespace infrastructure {
namespace config {
namespace handlers {

class LocationDirectiveHandler : public ADirectiveHandler {
 public:
  LocationDirectiveHandler(
      application::ports::ILogger& logger,
      domain::configuration::entities::LocationConfig& location);

  virtual void handle(const std::string& directive,
                      const std::vector<std::string>& args,
                      std::size_t lineNumber);

 private:
  domain::configuration::entities::LocationConfig& m_location;

  void handleRoot(const std::vector<std::string>& args, std::size_t lineNumber);
  void handleAlias(const std::vector<std::string>& args,
                   std::size_t lineNumber);
  void handleIndex(const std::vector<std::string>& args,
                   std::size_t lineNumber);
  void handleLimitExcept(const std::vector<std::string>& args,
                         std::size_t lineNumber);
  void handleAutoIndex(const std::vector<std::string>& args,
                       std::size_t lineNumber);
  void handleTryFiles(const std::vector<std::string>& args,
                      std::size_t lineNumber);
  void handleReturn(const std::vector<std::string>& args,
                    std::size_t lineNumber);
  void handleUploadStore(const std::vector<std::string>& args,
                         std::size_t lineNumber);
  void handleUploadPermissions(const std::vector<std::string>& args,
                               std::size_t lineNumber);
  void handleUploadAccess(const std::vector<std::string>& args,
                          std::size_t lineNumber);
  void handleScript(const std::vector<std::string>& args,
                    std::size_t lineNumber);
  void handleCgiRoot(const std::vector<std::string>& args,
                     std::size_t lineNumber);
  void handleFastcgiParam(const std::vector<std::string>& args,
                          std::size_t lineNumber);
  void handleUploadSizeLimits(const std::string& directive,
                              const std::vector<std::string>& args,
                              std::size_t lineNumber);
  void handleAddHeader(const std::vector<std::string>& args,
                       std::size_t lineNumber);
  void handleClientMaxBodySize(const std::vector<std::string>& args,
                               std::size_t lineNumber);
  void handleErrorPage(const std::vector<std::string>& args,
                       std::size_t lineNumber);
};

}  // namespace handlers
}  // namespace config
}  // namespace infrastructure

#endif  // LOCATION_DIRECTIVE_HANDLER_HPP
