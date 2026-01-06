/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CliController.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/12 16:08:05 by dande-je          #+#    #+#             */
/*   Updated: 2026/01/06 18:18:11 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLI_CONTRLLER_HPP
#define CLI_CONTRLLER_HPP

#include "application/ports/IConfigProvider.hpp"
#include "application/ports/ISocketOrchestrator.hpp"
#include "presentation/cli/CliView.hpp"

namespace presentation {
namespace cli {

class CliController {
 public:
  CliController(CliView& view);
  ~CliController();

  bool run(int argc, char** argv);

 private:
  CliController(const CliController&);

  CliController& operator=(const CliController& other);

  CliView& m_view;
  application::ports::IConfigProvider* m_configProvider;
  application::ports::ISocketOrchestrator* m_orchestrator;

  static const int K_MAX_SIZE_ARGS = 2;
  static const int K_NAME_PROGRAM = 0;
  static const int K_LITERAL_ARGUMENT_INDEX = 1;

  bool parseArguments(int argc, char** argv);
  bool loadConfiguration(const std::string& configPath);
  void displayConfigurationSummary(
      const application::ports::IConfigProvider& configProvider) const;
  bool startServer();
  void runServerLoop();
  void stopServer();
};

}  // namespace cli
}  // namespace presentation

#endif  // CLI_CONTRLLER_HPP
