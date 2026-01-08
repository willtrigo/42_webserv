/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConnectionHandler.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/03 11:57:13 by dande-je          #+#    #+#             */
/*   Updated: 2026/01/08 10:20:23 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONNECTIONHANDLER_HPP
#define CONNECTIONHANDLER_HPP

#include "application/ports/IConfigProvider.hpp"
#include "application/ports/ILogger.hpp"
#include "domain/configuration/entities/LocationConfig.hpp"
#include "domain/configuration/entities/ServerConfig.hpp"
#include "domain/configuration/value_objects/Route.hpp"
#include "domain/filesystem/value_objects/Path.hpp"
#include "domain/http/entities/HttpRequest.hpp"
#include "domain/http/entities/HttpResponse.hpp"
#include "domain/shared/value_objects/ErrorCode.hpp"
#include "infrastructure/cgi/primitives/CgiResponse.hpp"
#include "infrastructure/network/adapters/TcpSocket.hpp"

#include <ctime>
#include <map>
#include <string>
#include <vector>

namespace infrastructure {
namespace network {
namespace adapters {

class ConnectionHandler {
 public:
  enum State {
    STATE_READING_REQUEST,
    STATE_PROCESSING,
    STATE_WRITING_RESPONSE,
    STATE_KEEP_ALIVE,
    STATE_CLOSING
  };

  static const size_t K_READ_BUFFER_SIZE = 8192;
  static const time_t K_CONNECTION_TIMEOUT = 60;
  static const time_t K_KEEPALIVE_TIMEOUT = 5;
  static const size_t K_MAX_REQUEST_SIZE = 1048576;

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

  const domain::configuration::entities::LocationConfig* findMatchingLocation(
      const domain::configuration::entities::ServerConfig* serverConfig,
      const std::string& requestPath) const;

  void handleGetRequest(
      const domain::configuration::value_objects::Route& route,
      const domain::configuration::entities::LocationConfig& location,
      const domain::filesystem::value_objects::Path& requestPath);

  void handlePostRequest(
      const domain::configuration::value_objects::Route& route,
      const domain::configuration::entities::LocationConfig& location,
      const domain::filesystem::value_objects::Path& requestPath);

  void handleDeleteRequest(
      const domain::configuration::value_objects::Route& route,
      const domain::configuration::entities::LocationConfig& location,
      const domain::filesystem::value_objects::Path& requestPath);

  void handleDirectoryRequest(
      const domain::configuration::entities::LocationConfig& location,
      const domain::filesystem::value_objects::Path& directoryPath,
      const domain::filesystem::value_objects::Path& requestPath);

  void handleStaticFileRequest(
      const domain::configuration::entities::LocationConfig& location,
      const domain::filesystem::value_objects::Path& filePath);

  void handleDirectoryListing(
      const domain::filesystem::value_objects::Path& directoryPath,
      const domain::filesystem::value_objects::Path& requestPath);

  void handleCgiRequest(
      const domain::configuration::entities::LocationConfig& location,
      const domain::filesystem::value_objects::Path& scriptPath);

  void handleFileUpload(
      const domain::configuration::entities::LocationConfig& location,
      const domain::filesystem::value_objects::Path& requestPath);

  void handleRedirect(
      const domain::configuration::entities::LocationConfig& location);

  void handleReturnContent(
      const domain::configuration::entities::LocationConfig& location);

  void generateErrorResponse(
      const domain::shared::value_objects::ErrorCode& statusCode,
      const std::string& message);

  void handleNotFound(
      const domain::configuration::entities::LocationConfig& location);

  void serveErrorPage(
      const std::string& errorPagePath,
      const domain::shared::value_objects::ErrorCode& statusCode,
      const domain::configuration::entities::LocationConfig& location);

  domain::filesystem::value_objects::Path resolvePathWithServerFallback(
      const domain::configuration::entities::LocationConfig& location,
      const std::string& requestPath) const;

  bool validateRequestBodySize(
      const domain::configuration::entities::LocationConfig& location) const;

  domain::filesystem::value_objects::Path tryFindFile(
      const domain::configuration::entities::LocationConfig& location,
      const domain::filesystem::value_objects::Path& requestPath) const;

  void buildCgiEnvironment(
      std::map<std::string, std::string>& env,
      const domain::configuration::entities::LocationConfig& location,
      const domain::filesystem::value_objects::Path& scriptPath) const;

  void buildHttpResponseFromCgi(
      const infrastructure::cgi::primitives::CgiResponse& cgiResponse);

  void applyCustomHeaders(
      const domain::configuration::entities::LocationConfig& location);

  bool shouldKeepAlive() const;

  void resetForNextRequest();

  std::string formatState() const;

  void logRequest(const domain::http::entities::HttpRequest& request,
                  const domain::http::entities::HttpResponse& response);

  std::string extractBoundary(const std::string& contentType) const;

  bool parseMultipartFormData(const std::string& body,
                              const std::string& boundary,
                              std::string& outFilename,
                              std::vector<char>& outContent) const;

  std::string extractFilenameFromHeaders(const std::string& headers) const;

  std::string sanitizeFilename(const std::string& filename) const;

  void ensureDirectoryExists(
      const domain::filesystem::value_objects::Path& dirPath) const;

  void writeUploadedFile(
      const domain::filesystem::value_objects::Path& filePath,
      const std::vector<char>& content) const;

  application::ports::ILogger& m_logger;
  application::ports::IConfigProvider& m_configProvider;

  TcpSocket* m_socket;
  const domain::configuration::entities::ServerConfig* m_serverConfig;

  State m_state;
  time_t m_lastActivityTime;

  std::string m_requestBuffer;
  domain::http::entities::HttpRequest m_request;
  domain::http::entities::HttpResponse m_response;
  std::string m_responseBuffer;
  size_t m_responseOffset;
};

}  // namespace adapters
}  // namespace network
}  // namespace infrastructure

#endif  // CONNECTIONHANDLER_HPP
