/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiResponse.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/05 17:37:51 by dande-je          #+#    #+#             */
/*   Updated: 2026/01/06 02:07:08 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "infrastructure/cgi/exceptions/CgiExecutionException.hpp"
#include "infrastructure/cgi/primitives/CgiResponse.hpp"

#include <cctype>
#include <cstdlib>
#include <sstream>

namespace infrastructure {
namespace cgi {
namespace primitives {

CgiResponse::CgiResponse()
    : m_hasStatus(false),
      m_status(domain::shared::value_objects::ErrorCode::ok()),
      m_hasLocation(false),
      m_contentType("text/html"),
      m_parsed(false) {}

CgiResponse::CgiResponse(const std::vector<char>& rawOutput)
    : m_hasStatus(false),
      m_status(domain::shared::value_objects::ErrorCode::ok()),
      m_hasLocation(false),
      m_contentType("text/html"),
      m_parsed(false) {
  parse(rawOutput);
}

CgiResponse::CgiResponse(const CgiResponse& other)
    : m_headers(other.m_headers),
      m_body(other.m_body),
      m_hasStatus(other.m_hasStatus),
      m_status(other.m_status),
      m_hasLocation(other.m_hasLocation),
      m_location(other.m_location),
      m_contentType(other.m_contentType),
      m_parsed(other.m_parsed) {}

CgiResponse::~CgiResponse() {}

CgiResponse& CgiResponse::operator=(const CgiResponse& other) {
  if (this != &other) {
    m_headers = other.m_headers;
    m_body = other.m_body;
    m_hasStatus = other.m_hasStatus;
    m_status = other.m_status;
    m_hasLocation = other.m_hasLocation;
    m_location = other.m_location;
    m_contentType = other.m_contentType;
    m_parsed = other.m_parsed;
  }
  return *this;
}

const std::map<std::string, std::string>& CgiResponse::getHeaders() const {
  return m_headers;
}

const std::vector<char>& CgiResponse::getBody() const { return m_body; }

bool CgiResponse::hasStatus() const { return m_hasStatus; }

domain::shared::value_objects::ErrorCode CgiResponse::getStatus() const {
  return m_status;
}

bool CgiResponse::hasLocation() const { return m_hasLocation; }

const std::string& CgiResponse::getLocation() const { return m_location; }

const std::string& CgiResponse::getContentType() const { return m_contentType; }

void CgiResponse::parse(const std::vector<char>& rawOutput) {
  if (rawOutput.empty()) {
    throw exceptions::CgiExecutionException(
        "CGI script produced no output",
        exceptions::CgiExecutionException::INVALID_OUTPUT);
  }

  std::size_t separator = findHeaderBodySeparator(rawOutput);

  if (separator == std::string::npos) {
    throw exceptions::CgiExecutionException(
        "CGI script output missing header/body separator",
        exceptions::CgiExecutionException::INVALID_OUTPUT);
  }

  std::string headerSection(
      rawOutput.begin(),
      rawOutput.begin() + static_cast<std::ptrdiff_t>(separator));
  parseHeaders(headerSection);

  std::size_t bodyStart = separator + 4;
  parseBody(rawOutput, bodyStart);

  extractSpecialHeaders();
  m_parsed = true;

  validate();
}

void CgiResponse::clear() {
  m_headers.clear();
  m_body.clear();
  m_hasStatus = false;
  m_status = domain::shared::value_objects::ErrorCode::ok();
  m_hasLocation = false;
  m_location.clear();
  m_contentType = "text/html";
  m_parsed = false;
}

bool CgiResponse::isValid() const {
  try {
    validate();
    return true;
  } catch (const exceptions::CgiExecutionException&) {
    return false;
  }
}

void CgiResponse::validate() const {
  if (!m_parsed) {
    throw exceptions::CgiExecutionException(
        "CGI response not yet parsed",
        exceptions::CgiExecutionException::INVALID_OUTPUT);
  }

  if (m_headers.find("content-type") == m_headers.end() && !m_hasLocation) {
    throw exceptions::CgiExecutionException(
        "CGI response missing Content-Type header (required unless "
        "redirecting)",
        exceptions::CgiExecutionException::INVALID_OUTPUT);
  }
}

domain::http::entities::HttpResponse CgiResponse::toHttpResponse() const {
  validate();

  domain::http::entities::HttpResponse response;

  if (m_hasStatus) {
    response.setStatusCode(m_status);
  } else {
    response.setStatusCode(domain::shared::value_objects::ErrorCode::ok());
  }

  for (std::map<std::string, std::string>::const_iterator it =
           m_headers.begin();
       it != m_headers.end(); ++it) {
    response.setHeader(it->first, it->second);
  }

  if (m_hasLocation) {
    response.setHeader("Location", m_location);
    if (!m_hasStatus) {
      response.setStatusCode(domain::shared::value_objects::ErrorCode::found());
    }
  }

  if (!m_body.empty()) {
    response.setBody(m_body);
  }

  return response;
}

CgiResponse CgiResponse::fromRawOutput(const std::vector<char>& rawOutput) {
  return CgiResponse(rawOutput);
}

void CgiResponse::parseHeaders(const std::string& headerSection) {
  std::istringstream iss(headerSection);
  std::string line;

  while (std::getline(iss, line) != 0) {
    if (line.empty() ||
        (line.length() == 1 && (line[0] == '\r' || line[0] == '\n'))) {
      continue;
    }

    if (!line.empty() && line[line.length() - 1] == '\r') {
      line.erase(line.length() - 1);
    }

    std::size_t colonPos = line.find(':');
    if (colonPos == std::string::npos) {
      continue;
    }

    std::string name = line.substr(0, colonPos);
    std::string value =
        (colonPos + 1 < line.length()) ? line.substr(colonPos + 1) : "";

    name = trimWhitespace(name);
    value = trimWhitespace(value);

    if (!isValidHeaderName(name)) {
      continue;
    }

    std::string normalizedName = normalizeHeaderName(name);
    m_headers[normalizedName] = value;
  }
}

void CgiResponse::parseBody(const std::vector<char>& rawOutput,
                            std::size_t bodyStart) {
  if (bodyStart >= rawOutput.size()) {
    m_body.clear();
    return;
  }

  m_body.assign(rawOutput.begin() + static_cast<std::ptrdiff_t>(bodyStart),
                rawOutput.end());
}

void CgiResponse::extractSpecialHeaders() {
  std::map<std::string, std::string>::iterator iter;

  iter = m_headers.find("status");
  if (iter != m_headers.end()) {
    unsigned int statusCode = parseStatusCode(iter->second);
    if (statusCode >= 100 &&
        statusCode < 600) {  // TODO using ErrorCode to remove magic numbers
      m_status = domain::shared::value_objects::ErrorCode(statusCode);
      m_hasStatus = true;
    }
  }

  iter = m_headers.find("location");
  if (iter != m_headers.end()) {
    m_location = iter->second;
    m_hasLocation = true;
  }

  iter = m_headers.find("content-type");
  if (iter != m_headers.end()) {
    m_contentType = iter->second;
  }
}

std::size_t CgiResponse::findHeaderBodySeparator(
    const std::vector<char>& data) {
  for (std::size_t i = 0; i < data.size(); ++i) {
    if (i + 3 < data.size()) {
      if (data[i] == '\r' && data[i + 1] == '\n' && data[i + 2] == '\r' &&
          data[i + 3] == '\n') {
        return i;
      }
    }

    if (i + 1 < data.size()) {
      if (data[i] == '\n' && data[i + 1] == '\n') {
        return i;
      }
    }
  }

  return std::string::npos;
}

std::string CgiResponse::normalizeHeaderName(const std::string& name) {
  std::string result = name;

  for (std::size_t i = 0; i < result.length(); ++i) {
    result[i] =
        static_cast<char>(std::tolower(static_cast<unsigned char>(result[i])));
  }

  return result;
}

std::string CgiResponse::trimWhitespace(const std::string& str) {
  std::size_t start = str.find_first_not_of(" \t\r\n");
  if (start == std::string::npos) {
    return "";
  }

  std::size_t end = str.find_last_not_of(" \t\r\n");
  return str.substr(start, end - start + 1);
}

bool CgiResponse::isValidHeaderName(const std::string& name) {
  if (name.empty()) {
    return false;
  }

  for (std::size_t i = 0; i < name.length(); ++i) {
    char chr = name[i];
    if ((std::isalnum(static_cast<unsigned char>(chr)) == 0) && chr != '-' &&
        chr != '_') {
      return false;
    }
  }

  return true;
}

unsigned int CgiResponse::parseStatusCode(const std::string& statusLine) {
  std::istringstream iss(statusLine);
  unsigned int code = 0;

  iss >> code;

  if (iss.fail() || code < 100 || code >= 600) {
    return 200;
  }

  return code;
}

}  // namespace primitives
}  // namespace cgi
}  // namespace infrastructure
