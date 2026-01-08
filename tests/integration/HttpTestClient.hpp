#ifndef HTTP_CLIENT_TEST_HELPER_HPP
#define HTTP_CLIENT_TEST_HELPER_HPP

#include <arpa/inet.h>
#include <cstdlib>
#include <cstring>
#include <map>
#include <netinet/in.h>
#include <sstream>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>

/**
 * Simple HTTP Client for Integration Testing
 * C++98 compatible - uses raw sockets
 */
class HttpTestClient {
 public:
  struct Response {
    int statusCode;
    std::string statusText;
    std::map<std::string, std::string> headers;
    std::string body;

    Response() : statusCode(0) {}

    bool hasHeader(const std::string& key) const {
      return headers.find(key) != headers.end();
    }

    std::string getHeader(const std::string& key) const {
      std::map<std::string, std::string>::const_iterator it = headers.find(key);
      if (it != headers.end()) {
        return it->second;
      }
      return "";
    }
  };

 private:
  std::string m_host;
  int m_port;

  int connectToServer() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
      return -1;
    }

    struct sockaddr_in server_addr;
    std::memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(m_port);

    if (inet_pton(AF_INET, m_host.c_str(), &server_addr.sin_addr) <= 0) {
      close(sockfd);
      return -1;
    }

    if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) <
        0) {
      close(sockfd);
      return -1;
    }

    return sockfd;
  }

  Response parseResponse(const std::string& raw) {
    Response response;

    // Split into lines
    std::vector<std::string> lines;
    std::string line;
    std::istringstream stream(raw);

    while (std::getline(stream, line)) {
      // Remove \r if present
      if (!line.empty() && line[line.length() - 1] == '\r') {
        line = line.substr(0, line.length() - 1);
      }
      lines.push_back(line);
    }

    if (lines.empty()) {
      return response;
    }

    // Parse status line: HTTP/1.1 200 OK
    std::istringstream statusStream(lines[0]);
    std::string httpVersion;
    statusStream >> httpVersion >> response.statusCode;
    std::getline(statusStream, response.statusText);
    if (!response.statusText.empty() && response.statusText[0] == ' ') {
      response.statusText = response.statusText.substr(1);
    }

    // Parse headers
    size_t i = 1;
    for (; i < lines.size(); ++i) {
      if (lines[i].empty()) {
        ++i;  // Skip empty line between headers and body
        break;
      }

      size_t colonPos = lines[i].find(':');
      if (colonPos != std::string::npos) {
        std::string key = lines[i].substr(0, colonPos);
        std::string value = lines[i].substr(colonPos + 1);

        // Trim leading space from value
        if (!value.empty() && value[0] == ' ') {
          value = value.substr(1);
        }

        // Convert key to lowercase for case-insensitive comparison
        for (size_t j = 0; j < key.length(); ++j) {
          key[j] = std::tolower(key[j]);
        }

        response.headers[key] = value;
      }
    }

    // Parse body (everything after empty line)
    // Reconstruct body preserving empty lines and line breaks
    bool firstLine = true;
    for (; i < lines.size(); ++i) {
      if (!firstLine) {
        response.body += "\n";
      }
      response.body += lines[i];
      firstLine = false;
    }

    return response;
  }

 public:
  HttpTestClient(const std::string& host, int port)
      : m_host(host), m_port(port) {}

  Response request(const std::string& method, const std::string& path,
                   const std::string& body = "",
                   const std::map<std::string, std::string>& headers =
                       std::map<std::string, std::string>()) {
    Response response;

    int sockfd = connectToServer();
    if (sockfd < 0) {
      response.statusCode = -1;
      response.statusText = "Connection failed";
      return response;
    }

    // Build request
    std::ostringstream request;
    request << method << " " << path << " HTTP/1.1\r\n";
    request << "Host: " << m_host << "\r\n";

    // Check if Content-Length already provided by caller
    bool hasContentLength = false;
    for (std::map<std::string, std::string>::const_iterator it =
             headers.begin();
         it != headers.end(); ++it) {
      std::string lowerKey = it->first;
      for (size_t i = 0; i < lowerKey.length(); ++i) {
        lowerKey[i] = std::tolower(lowerKey[i]);
      }
      if (lowerKey == "content-length") {
        hasContentLength = true;
        break;
      }
    }

    // Add content-length if body present and not already provided
    if (!body.empty() && !hasContentLength) {
      request << "Content-Length: " << body.length() << "\r\n";
    }

    // Add custom headers
    for (std::map<std::string, std::string>::const_iterator it =
             headers.begin();
         it != headers.end(); ++it) {
      request << it->first << ": " << it->second << "\r\n";
    }

    request << "\r\n";

    if (!body.empty()) {
      request << body;
    }

    std::string requestStr = request.str();

    // Send request
    ssize_t sent = send(sockfd, requestStr.c_str(), requestStr.length(), 0);
    if (sent < 0) {
      close(sockfd);
      response.statusCode = -1;
      response.statusText = "Send failed";
      return response;
    }

    // Receive response
    std::string rawResponse;
    char buffer[4096];
    ssize_t received;
    const size_t MAX_RESPONSE_SIZE = 10 * 1024 * 1024;  // 10MB safety limit

    while ((received = recv(sockfd, buffer, sizeof(buffer) - 1, 0)) > 0) {
      buffer[received] = '\0';
      rawResponse += buffer;

      // Safety check: prevent infinite loop on malformed responses
      if (rawResponse.length() > MAX_RESPONSE_SIZE) {
        close(sockfd);
        response.statusCode = -1;
        response.statusText = "Response too large (>10MB)";
        return response;
      }

      // Check if we received complete response (simple heuristic)
      if (rawResponse.find("\r\n\r\n") != std::string::npos) {
        // For HEAD requests or responses without body
        if (method == "HEAD" ||
            rawResponse.find("Content-Length: 0") != std::string::npos) {
          break;
        }

        // Check if we have complete body
        size_t headerEnd = rawResponse.find("\r\n\r\n");
        if (headerEnd != std::string::npos) {
          // Parse content-length
          size_t clPos = rawResponse.find("Content-Length:");
          if (clPos != std::string::npos && clPos < headerEnd) {
            size_t clEnd = rawResponse.find("\r\n", clPos);
            std::string clValue =
                rawResponse.substr(clPos + 15, clEnd - clPos - 15);

            // Trim spaces
            while (!clValue.empty() && clValue[0] == ' ') {
              clValue = clValue.substr(1);
            }

            int contentLength = atoi(clValue.c_str());
            size_t bodyStart = headerEnd + 4;

            if (rawResponse.length() >= bodyStart + contentLength) {
              break;  // Complete response received
            }
          } else {
            // No Content-Length header found after seeing headers
            // For safety in tests, break after reasonable wait (1 more recv)
            // This prevents hanging on chunked/streaming responses in tests
            break;
          }
        }
      }
    }

    close(sockfd);

    if (received < 0) {
      response.statusCode = -1;
      response.statusText = "Receive failed";
      return response;
    }

    return parseResponse(rawResponse);
  }

  // Convenience methods
  Response get(const std::string& path) { return request("GET", path); }

  Response head(const std::string& path) { return request("HEAD", path); }

  Response post(const std::string& path, const std::string& body) {
    return request("POST", path, body);
  }

  Response del(const std::string& path) { return request("DELETE", path); }
};

#endif  // HTTP_CLIENT_TEST_HELPER_HPP
