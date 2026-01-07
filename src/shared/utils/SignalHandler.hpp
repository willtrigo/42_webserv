/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SignalHandler.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/06 18:38:31 by dande-je          #+#    #+#             */
/*   Updated: 2026/01/07 01:43:40 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SIGNALHANDLER_HPP
#define SIGNALHANDLER_HPP

#include <csignal>

namespace shared {
namespace utils {

class SignalHandler {
 public:
  static void initialize();
  static void cleanup();
  static bool isShutdownRequested();
  static void resetShutdownFlag();
  static volatile sig_atomic_t* getShutdownFlagPtr();

  // Public static for safe, qualified access from global handler.
  static volatile sig_atomic_t s_shutdownRequested;

 private:
  SignalHandler();
  SignalHandler(const SignalHandler&);
  SignalHandler& operator=(const SignalHandler&);
  ~SignalHandler();

  static bool s_initialized;
};

} // namespace utils
} // namespace shared

// Global C-linkage for std::signal compatibility.
extern "C" void handleSignal(int signal);

#endif // SIGNALHANDLER_HPP
