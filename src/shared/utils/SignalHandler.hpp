/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SignalHandler.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/06 18:38:31 by dande-je          #+#    #+#             */
/*   Updated: 2026/01/06 19:04:04 by dande-je         ###   ########.fr       */
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

 private:
  SignalHandler();
  SignalHandler(const SignalHandler&);
  SignalHandler& operator=(const SignalHandler&);
  ~SignalHandler();

  static volatile sig_atomic_t s_shutdownRequested;
  static bool s_initialized;
};

}  // namespace utils
}  // namespace shared

#endif  // SIGNALHANDLER_HPP
