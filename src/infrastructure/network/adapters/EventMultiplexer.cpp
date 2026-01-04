/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   EventMultiplexer.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/03 11:34:44 by dande-je          #+#    #+#             */
/*   Updated: 2026/01/04 00:55:41 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "infrastructure/network/adapters/EventMultiplexer.hpp"
#include "infrastructure/network/exceptions/MultiplexerException.hpp"

#include <cerrno>
#include <cstring>
#include <sstream>
#include <sys/epoll.h>
#include <unistd.h>

namespace infrastructure {
namespace network {
namespace adapters {

EventMultiplexer::EventMultiplexer(application::ports::ILogger& logger)
    : m_logger(logger), m_epollFd(K_INVALID_EPOLL_FD) {
  initializeEpoll();
}

EventMultiplexer::~EventMultiplexer() { cleanupEpoll(); }

void EventMultiplexer::registerSocket(int fileDescriptor, int eventMask) {
  if (fileDescriptor < 0) {
    throw exceptions::MultiplexerException(
        "File descriptor must be non-negative",
        exceptions::MultiplexerException::INVALID_FILE_DESCRIPTOR);
  }

  if (isRegistered(fileDescriptor)) {
    std::ostringstream oss;
    oss << "File descriptor " << fileDescriptor << " already registered";
    throw exceptions::MultiplexerException(
        oss.str(), exceptions::MultiplexerException::ALREADY_REGISTERED);
  }

  m_registrations[fileDescriptor] = eventMask;

  struct epoll_event event;
  std::memset(&event, 0, sizeof(event));
  event.events = convertToEpollEvents(eventMask);
  event.data.fd = fileDescriptor;

  if (epoll_ctl(m_epollFd, EPOLL_CTL_ADD, fileDescriptor, &event) ==
      K_SYSTEM_ERROR) {
    const int savedErrno = errno;
    m_registrations.erase(fileDescriptor);
    std::ostringstream oss;
    oss << "Failed to add fd=" << fileDescriptor << " to epoll";
    throw exceptions::MultiplexerException(
        oss.str(), exceptions::MultiplexerException::EPOLL_CTL_ADD_FAILED,
        savedErrno);
  }

  std::ostringstream oss;
  oss << "Registered fd=" << fileDescriptor << " with events=0x" << std::hex
      << eventMask;
  m_logger.debug(oss.str());
}

void EventMultiplexer::modifySocket(int fileDescriptor, int eventMask) {
  if (!isRegistered(fileDescriptor)) {
    std::ostringstream oss;
    oss << "File descriptor " << fileDescriptor << " not registered";
    throw exceptions::MultiplexerException(
        oss.str(), exceptions::MultiplexerException::NOT_REGISTERED);
  }

  m_registrations[fileDescriptor] = eventMask;

  struct epoll_event event;
  std::memset(&event, 0, sizeof(event));
  event.events = convertToEpollEvents(eventMask);
  event.data.fd = fileDescriptor;

  if (epoll_ctl(m_epollFd, EPOLL_CTL_MOD, fileDescriptor, &event) ==
      K_SYSTEM_ERROR) {
    std::ostringstream oss;
    oss << "Failed to modify fd=" << fileDescriptor << " in epoll";
    throw exceptions::MultiplexerException(
        oss.str(), exceptions::MultiplexerException::EPOLL_CTL_MOD_FAILED,
        errno);
  }

  std::ostringstream oss;
  oss << "Modified fd=" << fileDescriptor << " to events=0x" << std::hex
      << eventMask;
  m_logger.debug(oss.str());
}

void EventMultiplexer::deregisterSocket(int fileDescriptor) {
  if (!isRegistered(fileDescriptor)) {
    std::ostringstream oss;
    oss << "File descriptor " << fileDescriptor << " not registered; ignoring";
    m_logger.warn(oss.str());
    return;
  }

  if (epoll_ctl(m_epollFd, EPOLL_CTL_DEL, fileDescriptor, NULL) ==
      K_SYSTEM_ERROR) {
    std::ostringstream oss;
    oss << "Failed to delete fd=" << fileDescriptor << " from epoll";
    m_logger.warn(oss.str());
  }

  m_registrations.erase(fileDescriptor);

  std::ostringstream oss;
  oss << "Deregistered fd=" << fileDescriptor;
  m_logger.debug(oss.str());
}

std::vector<primitives::SocketEvent> EventMultiplexer::wait(int timeoutMs) {
  std::vector<primitives::SocketEvent> events;

  if (m_registrations.empty()) {
    return events;
  }

  struct epoll_event epollEvents[K_DEFAULT_MAX_EVENTS];
  std::memset(epollEvents, 0, sizeof(epollEvents));

  int readyCount;
  do {
    readyCount =
        epoll_wait(m_epollFd, epollEvents, K_DEFAULT_MAX_EVENTS, timeoutMs);
  } while (readyCount == K_SYSTEM_ERROR && errno == EINTR);

  if (readyCount == K_SYSTEM_ERROR) {
    throw exceptions::MultiplexerException(
        "epoll_wait failed",
        exceptions::MultiplexerException::EPOLL_WAIT_FAILED, errno);
  }

  events.reserve(static_cast<size_t>(readyCount));
  for (int i = 0; i < readyCount; ++i) {
    const int fileDescriptor = epollEvents[i].data.fd;
    const int eventMask = convertFromEpollEvents(epollEvents[i].events);
    events.push_back(primitives::SocketEvent(fileDescriptor, eventMask));
  }

  if (!events.empty()) {
    std::ostringstream oss;
    oss << "epoll_wait returned " << events.size() << " event(s)";
    m_logger.debug(oss.str());
  }

  return events;
}

size_t EventMultiplexer::getRegisteredCount() const {
  return m_registrations.size();
}

bool EventMultiplexer::isRegistered(int fileDescriptor) const {
  return m_registrations.find(fileDescriptor) != m_registrations.end();
}

void EventMultiplexer::initializeEpoll() {
  m_epollFd = epoll_create1(0);
  if (m_epollFd == K_INVALID_EPOLL_FD) {
    throw exceptions::MultiplexerException(
        "epoll_create1 failed",
        exceptions::MultiplexerException::EPOLL_CREATE_FAILED, errno);
  }
  m_logger.info("EventMultiplexer initialized with epoll");
}

void EventMultiplexer::cleanupEpoll() {
  m_registrations.clear();

  if (m_epollFd != K_INVALID_EPOLL_FD) {
    if (::close(m_epollFd) == K_SYSTEM_ERROR) {
      m_logger.warn("Failed to close epoll file descriptor");
    }
    m_epollFd = K_INVALID_EPOLL_FD;
  }
}

int EventMultiplexer::convertToEpollEvents(int eventMask) {
  int epollFlags = 0;

  if ((eventMask & primitives::SocketEvent::EVENT_READ) != 0) {
    epollFlags |= EPOLLIN;
  }
  if ((eventMask & primitives::SocketEvent::EVENT_WRITE) != 0) {
    epollFlags |= EPOLLOUT;
  }
  if ((eventMask & primitives::SocketEvent::EVENT_ERROR) != 0) {
    epollFlags |= EPOLLERR;
  }
  if ((eventMask & primitives::SocketEvent::EVENT_HUP) != 0) {
    epollFlags |= EPOLLHUP;
  }

  return epollFlags;
}

int EventMultiplexer::convertFromEpollEvents(int epollEvents) {
  int eventMask = primitives::SocketEvent::EVENT_NONE;

  if ((epollEvents & EPOLLIN) != 0) {
    eventMask |= primitives::SocketEvent::EVENT_READ;
  }
  if ((epollEvents & EPOLLOUT) != 0) {
    eventMask |= primitives::SocketEvent::EVENT_WRITE;
  }
  if ((epollEvents & EPOLLERR) != 0) {
    eventMask |= primitives::SocketEvent::EVENT_ERROR;
  }
  if ((epollEvents & EPOLLHUP) != 0) {
    eventMask |= primitives::SocketEvent::EVENT_HUP;
  }

  return eventMask;
}

std::string EventMultiplexer::formatRegistrationState() const {
  std::ostringstream oss;
  oss << "Registrations: " << m_registrations.size() << " fd(s) [";

  size_t count = 0;
  const size_t maxDisplay = 10;
  for (std::map<int, int>::const_iterator it = m_registrations.begin();
       it != m_registrations.end(); ++it) {
    if (count++ > 0) oss << ", ";
    oss << it->first << "=0x" << std::hex << it->second;
    if (count >= maxDisplay) {
      oss << ", ...";
      break;
    }
  }

  oss << "]";
  return oss.str();
}

}  // namespace adapters
}  // namespace network
}  // namespace infrastructure
