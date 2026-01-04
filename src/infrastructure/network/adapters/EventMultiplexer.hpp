/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   EventMultiplexer.hpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/03 10:04:24 by dande-je          #+#    #+#             */
/*   Updated: 2026/01/04 00:55:35 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EVENTMULTIPLEXER_HPP
#define EVENTMULTIPLEXER_HPP

#include "application/ports/ILogger.hpp"
#include "infrastructure/network/primitives/SocketEvent.hpp"

#include <map>
#include <vector>

namespace infrastructure {
namespace network {
namespace adapters {

class EventMultiplexer {
 public:
  static const int K_INVALID_EPOLL_FD = -1;
  static const int K_SYSTEM_ERROR = -1;
  static const int K_DEFAULT_MAX_EVENTS = 128;
  static const int K_INFINITE_TIMEOUT = -1;
  static const int K_NO_TIMEOUT = 0;

  explicit EventMultiplexer(application::ports::ILogger& logger);

  ~EventMultiplexer();

  void registerSocket(int fileDescriptor, int eventMask);
  void modifySocket(int fileDescriptor, int eventMask);
  void deregisterSocket(int fileDescriptor);
  std::vector<primitives::SocketEvent> wait(int timeoutMs);

  size_t getRegisteredCount() const;

  bool isRegistered(int fileDescriptor) const;

 private:
  EventMultiplexer(const EventMultiplexer&);
  EventMultiplexer& operator=(const EventMultiplexer&);

  void initializeEpoll();

  void cleanupEpoll();

  static int convertToEpollEvents(int eventMask);

  static int convertFromEpollEvents(int epollEvents);

  std::string formatRegistrationState() const;

  application::ports::ILogger& m_logger;
  int m_epollFd;
  std::map<int, int> m_registrations;
};

}  // namespace adapters
}  // namespace network
}  // namespace infrastructure

#endif  // EVENTMULTIPLEXER_HPP
