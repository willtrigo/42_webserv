/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SocketEvent.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/03 23:59:14 by dande-je          #+#    #+#             */
/*   Updated: 2026/01/04 00:05:40 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SOCKET_EVENT_HPP
#define SOCKET_EVENT_HPP

namespace infrastructure {
namespace network {
namespace primitives {

class SocketEvent {
 public:
  enum EventType {
    EVENT_NONE = 0x00,
    EVENT_READ = 0x01,
    EVENT_WRITE = 0x02,
    EVENT_ERROR = 0x04,
    EVENT_HUP = 0x08
  };

  static const int K_INVALID_FD = -1;
  static const int K_EVENT_MASK_ALL = 0xFF;

  SocketEvent();

  SocketEvent(int fileDescriptor, int events);

  SocketEvent(const SocketEvent& other);

  ~SocketEvent();

  SocketEvent& operator=(const SocketEvent& other);

  int getFd() const;
  int getEvents() const;

  bool isValid() const;
  bool isReadable() const;
  bool isWritable() const;

  bool hasError() const;
  bool hasHangup() const;

  bool operator==(const SocketEvent& other) const;
  bool operator!=(const SocketEvent& other) const;

 private:
  int m_fd;
  int m_events;
};

}  // namespace primitives
}  // namespace network
}  // namespace infrastructure

#endif  // SOCKET_EVENT_HPP
