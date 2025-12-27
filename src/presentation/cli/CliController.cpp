/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CliController.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/12 16:08:17 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/27 16:47:50 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// #include "application/ports/IConfigProvider.hpp"
// #include "infrastructure/adapters/ConfigProvider.hpp"
#include "presentation/cli/CliController.hpp"
#include "presentation/cli/CliView.hpp"

#include <cstdlib>
#include <memory>
#include <stdexcept>
#include <string>

CliController::CliController(CliView& view) : m_view(view) {}

CliController::CliController(const CliController& other)
    : m_view(other.m_view) {}

CliController::~CliController() {}

CliController& CliController::operator=(const CliController& other) {
  if (this == &other) {
    return *this;
  }
  throw std::runtime_error("CliController assignment is not allowed");
}

bool CliController::run(int argc, char** argv) {
  if (!parseArguments(argc, argv)) {
    return false;
  }

  try {
    const std::string configPath =
        ((argv[K_LITERAL_ARGUMENT_INDEX] != 0)
             ? std::string(argv[K_LITERAL_ARGUMENT_INDEX])
             : "default.conf");

    // std::auto_ptr<application::ports::IConfigProvider> configProvider( new
    // infrastructure::adapters::ConfigProvider(this->m_view.getLogger()));
    // configProvider->load(configPath, "default.conf");

  } catch (const std::exception& exception) {
    this->m_view.displayError(exception.what());
    return false;
  }
  return true;
}

bool CliController::parseArguments(int argc, char** argv) {
  if (argc > K_MAX_SIZE_ARGS) {
    this->m_view.displayUsage(std::string(argv[K_NAME_PROGRAM]));
    return false;
  }

  return true;
}
