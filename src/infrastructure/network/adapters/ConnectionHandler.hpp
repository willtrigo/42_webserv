/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConnectionHandler.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/03 11:57:13 by dande-je          #+#    #+#             */
/*   Updated: 2026/01/04 01:11:20 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef INFRASTRUCTURE_NETWORK_CONNECTIONHANDLER_HPP
#define INFRASTRUCTURE_NETWORK_CONNECTIONHANDLER_HPP

#include "application/ports/IConfigProvider.hpp"
#include "application/ports/ILogger.hpp"
#include "domain/configuration/entities/ServerConfig.hpp"

#include <string>

namespace infrastructure {
namespace network {
namespace adapters {

class TcpSocket;

class ConnectionHandler {
 public:
  enum State {
    STATE_READING_REQUEST,
    STATE_PROCESSING,
    STATE_WRITING_RESPONSE,
    STATE_KEEP_ALIVE,
    STATE_CLOSING
  };

  ConnectionHandler(
      TcpSocket* socket,
      const domain::configuration::entities::ServerConfig* serverConfig,
      application::ports::ILogger& logger,
      application::ports::IConfigProvider& configProvider);

  ~ConnectionHandler();

  void processEvent();

  bool shouldClose() const;

  State getState() const;

  int getFd() const;

  std::string getRemoteAddress() const;

  bool isTimedOut(time_t currentTime) const;

  void updateLastActivity(time_t currentTime);

 private:
  ConnectionHandler(const ConnectionHandler&);
  ConnectionHandler& operator=(const ConnectionHandler&);

  void handleRead();

  void handleWrite();

  bool parseRequest();

  void processRequest();

  const domain::configuration::entities::ServerConfig* resolveVirtualHost();

  void generateErrorResponse(int statusCode, const std::string& message);

  bool shouldKeepAlive() const;

  std::string formatState() const;

  void logRequest(const std::string& method, const std::string& uri,
                  int statusCode);

  application::ports::ILogger& m_logger;
  application::ports::IConfigProvider& m_configProvider;

  TcpSocket* m_socket;
  const domain::configuration::entities::ServerConfig* m_serverConfig;

  State m_state;
  time_t m_lastActivityTime;

  std::string m_requestBuffer;
  std::string m_responseBuffer;
  size_t m_responseOffset;

  std::string m_requestMethod;
  std::string m_requestUri;
  std::string m_requestProtocol;

  static const size_t K_READ_BUFFER_SIZE = 8192;
  static const time_t K_CONNECTION_TIMEOUT = 60;
};

}  // namespace adapters
}  // namespace network
}  // namespace infrastructure

#endif  // INFRASTRUCTURE_NETWORK_CONNECTIONHANDLER_HPP
