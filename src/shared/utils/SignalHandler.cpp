/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SignalHandler.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/06 18:38:58 by dande-je          #+#    #+#             */
/*   Updated: 2026/01/06 19:04:36 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shared/utils/SignalHandler.hpp"

// ============================================================================
// Static Storage (File-Scope, Accessible from C Functions)
// ============================================================================

static volatile sig_atomic_t g_shutdownRequested = 0;
static bool g_initialized = false;

// ============================================================================
// C Linkage Signal Handler Functions (OUTSIDE NAMESPACE)
// ============================================================================

extern "C" {

static void handleSignalInt(int signal) {
  (void)signal;
  g_shutdownRequested = 1;
}

static void handleSignalTerm(int signal) {
  (void)signal;
  g_shutdownRequested = 1;
}

}  // extern "C"

// ============================================================================
// SignalHandler Implementation
// ============================================================================

namespace shared {
namespace utils {

// ============================================================================
// Static Member Initialization
// ============================================================================

volatile sig_atomic_t SignalHandler::s_shutdownRequested = 0;
bool SignalHandler::s_initialized = false;

// ============================================================================
// Public Static Methods
// ============================================================================

void SignalHandler::initialize() {
  if (g_initialized) {
    return;
  }

  std::signal(SIGINT, handleSignalInt);
  std::signal(SIGTERM, handleSignalTerm);
  std::signal(SIGPIPE, SIG_IGN);

  g_initialized = true;
  g_shutdownRequested = 0;
}

void SignalHandler::cleanup() {
  if (!g_initialized) {
    return;
  }

  std::signal(SIGINT, SIG_DFL);
  std::signal(SIGTERM, SIG_DFL);
  std::signal(SIGPIPE, SIG_DFL);

  g_initialized = false;
  g_shutdownRequested = 0;
}

bool SignalHandler::isShutdownRequested() {
  return g_shutdownRequested != 0;
}

void SignalHandler::resetShutdownFlag() {
  g_shutdownRequested = 0;
}

volatile sig_atomic_t* SignalHandler::getShutdownFlagPtr() {
  return &g_shutdownRequested;
}

// ============================================================================
// Private Constructor (Prevent Instantiation)
// ============================================================================

SignalHandler::SignalHandler() {}

SignalHandler::~SignalHandler() {}

}  // namespace utils
}  // namespace shared
