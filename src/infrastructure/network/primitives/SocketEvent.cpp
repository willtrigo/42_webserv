/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SocketEvent.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/04 00:06:33 by dande-je          #+#    #+#             */
/*   Updated: 2026/01/04 00:19:50 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "infrastructure/network/primitives/SocketEvent.hpp"

namespace infrastructure {
namespace network {
namespace primitives {

SocketEvent::SocketEvent() : m_fd(K_INVALID_FD), m_events(EVENT_NONE) {}

SocketEvent::SocketEvent(int fileDescriptor, int events)
    : m_fd(fileDescriptor), m_events(events) {}

SocketEvent::SocketEvent(const SocketEvent& other)
    : m_fd(other.m_fd), m_events(other.m_events) {}

SocketEvent::~SocketEvent() {}

SocketEvent& SocketEvent::operator=(const SocketEvent& other) {
  if (this != &other) {
    m_fd = other.m_fd;
    m_events = other.m_events;
  }
  return *this;
}

int SocketEvent::getFd() const { return m_fd; }

int SocketEvent::getEvents() const { return m_events; }

bool SocketEvent::isValid() const {
  return m_fd >= 0 && m_events != EVENT_NONE;
}

bool SocketEvent::isReadable() const { return (m_events & EVENT_READ) != 0; }

bool SocketEvent::isWritable() const { return (m_events & EVENT_WRITE) != 0; }

bool SocketEvent::hasError() const { return (m_events & EVENT_ERROR) != 0; }

bool SocketEvent::hasHangup() const { return (m_events & EVENT_HUP) != 0; }

bool SocketEvent::operator==(const SocketEvent& other) const {
  return m_fd == other.m_fd && m_events == other.m_events;
}

bool SocketEvent::operator!=(const SocketEvent& other) const {
  return !(*this == other);
}

}  // namespace primitives
}  // namespace network
}  // namespace infrastructure
