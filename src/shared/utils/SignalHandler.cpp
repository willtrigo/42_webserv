/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SignalHandler.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/06 18:38:58 by dande-je          #+#    #+#             */
/*   Updated: 2026/01/07 01:43:49 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shared/utils/SignalHandler.hpp"
#include <cstdlib>  // std::exit

namespace shared {
namespace utils {

// Static initialization.
volatile sig_atomic_t SignalHandler::s_shutdownRequested = 0;
bool SignalHandler::s_initialized = false;

extern "C" void handleSignal(int signal) {
  // Async-signal-safe: only atomic ops, no complex logic.
  if (signal == SIGINT || signal == SIGTERM || signal == SIGQUIT) {
    SignalHandler::s_shutdownRequested = 1;
  }
}

void SignalHandler::initialize() {
  if (s_initialized) return;
  std::signal(SIGINT, handleSignal);
  std::signal(SIGTERM, handleSignal);
  std::signal(SIGQUIT, handleSignal);  // Handle Ctrl+\ for graceful quit.
  std::signal(SIGPIPE, SIG_IGN);       // Ignore broken pipes (per RFC).
  s_initialized = true;
  s_shutdownRequested = 0;
}

void SignalHandler::cleanup() {
  if (!s_initialized) return;
  std::signal(SIGINT, SIG_DFL);
  std::signal(SIGTERM, SIG_DFL);
  std::signal(SIGQUIT, SIG_DFL);
  std::signal(SIGPIPE, SIG_DFL);
  s_initialized = false;
  s_shutdownRequested = 0;
}

bool SignalHandler::isShutdownRequested() {
  return s_shutdownRequested != 0;
}

void SignalHandler::resetShutdownFlag() {
  s_shutdownRequested = 0;
}

volatile sig_atomic_t* SignalHandler::getShutdownFlagPtr() {
  return &s_shutdownRequested;
}

// Prevent instantiation.
SignalHandler::SignalHandler() {}
SignalHandler::~SignalHandler() {}

} // utils
} // shared
