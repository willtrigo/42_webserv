/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   QueryStringBuilder.cpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: umeneses <umeneses@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/23 00:39:57 by dande-je          #+#    #+#             */
/*   Updated: 2026/01/04 15:50:20 by umeneses         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "domain/http/exceptions/QueryStringBuilderException.hpp"
#include "domain/http/value_objects/QueryStringBuilder.hpp"
#include "domain/http/value_objects/Uri.hpp"

#include <cctype>
#include <cstdlib>
#include <iomanip>
#include <sstream>

namespace domain {
namespace http {
namespace value_objects {

QueryStringBuilder::QueryStringBuilder() {}

QueryStringBuilder::QueryStringBuilder(const std::string& baseUrl)
    : m_baseUrl(baseUrl) {
  validateBaseUrl(baseUrl);
  
  std::size_t questionPos = baseUrl.find('?');
  if (questionPos != std::string::npos) {
    m_baseUrl = baseUrl.substr(0, questionPos);
    std::string queryString = baseUrl.substr(questionPos + 1);
    
    if (!queryString.empty()) {
      m_parameters = parseQueryString(queryString).m_parameters;
    }
  }
}

QueryStringBuilder::~QueryStringBuilder() {}

QueryStringBuilder::QueryStringBuilder(const QueryStringBuilder& other)
    : m_baseUrl(other.m_baseUrl), m_parameters(other.m_parameters) {}

QueryStringBuilder& QueryStringBuilder::operator=(
    const QueryStringBuilder& other) {
  if (this != &other) {
    m_baseUrl = other.m_baseUrl;
    m_parameters = other.m_parameters;
  }
  return *this;
}

void QueryStringBuilder::validateBaseUrl(const std::string& url) {
  if (url.empty()) {
    throw exceptions::QueryStringBuilderException(
        "Base URL cannot be empty",
        exceptions::QueryStringBuilderException::EMPTY_URL);
  }

  if (url.length() > MAX_URL_LENGTH) {
    std::ostringstream oss;
    oss << "URL too long: " << url.length()
        << " characters (max: " << MAX_URL_LENGTH << ")";
    throw exceptions::QueryStringBuilderException(
        oss.str(), exceptions::QueryStringBuilderException::INVALID_URL_FORMAT);
  }

  bool hasScheme = url.find("://") != std::string::npos;
  bool isRelative = !url.empty() && url[0] == '/';
  
  if (!hasScheme && !isRelative) {
    throw exceptions::QueryStringBuilderException(
        "Invalid URL format: must be absolute (with ://) or relative (starting with /)",
        exceptions::QueryStringBuilderException::INVALID_URL_FORMAT);
  }

  if (hasScheme) {
    try {
      Uri uri(url);
    } catch (const std::exception& e) {
      throw exceptions::QueryStringBuilderException(
          std::string("Invalid URL format: ") + e.what(),
          exceptions::QueryStringBuilderException::INVALID_URL_FORMAT);
    }
  }
}

void QueryStringBuilder::validateParameterName(const std::string& name) {
  if (name.empty()) {
    throw exceptions::QueryStringBuilderException(
        "Parameter name cannot be empty",
        exceptions::QueryStringBuilderException::INVALID_PARAMETER_NAME);
  }

  if (name.length() > MAX_PARAMETER_NAME_LENGTH) {
    std::ostringstream oss;
    oss << "Parameter name too long: '" << name << "' (" << name.length()
        << " characters, max: " << MAX_PARAMETER_NAME_LENGTH << ")";
    throw exceptions::QueryStringBuilderException(
        oss.str(),
        exceptions::QueryStringBuilderException::INVALID_PARAMETER_NAME);
  }

  for (std::size_t i = 0; i < name.length(); ++i) {
    unsigned char chr = static_cast<unsigned char>(name[i]);
    if (chr < ' ' || chr == '=' || chr == '&' || chr == '?' || chr == '#') {
      std::ostringstream oss;
      oss << "Invalid character in parameter name: '" << name << "'";
      throw exceptions::QueryStringBuilderException(
          oss.str(),
          exceptions::QueryStringBuilderException::INVALID_PARAMETER_NAME);
    }
  }
}

void QueryStringBuilder::validateParameterValue(const std::string& value) {
  if (value.length() > MAX_PARAMETER_VALUE_LENGTH) {
    std::ostringstream oss;
    oss << "Parameter value too long: " << value.length()
        << " characters (max: " << MAX_PARAMETER_VALUE_LENGTH << ")";
    throw exceptions::QueryStringBuilderException(
        oss.str(),
        exceptions::QueryStringBuilderException::INVALID_PARAMETER_VALUE);
  }
}

void QueryStringBuilder::validateParameterCount() const {
  if (m_parameters.size() > MAX_PARAMETERS_COUNT) {
    std::ostringstream oss;
    oss << "Too many parameters: " << m_parameters.size()
        << " (max: " << MAX_PARAMETERS_COUNT << ")";
    throw exceptions::QueryStringBuilderException(
        oss.str(), exceptions::QueryStringBuilderException::INVALID_URL_FORMAT);
  }
}

void QueryStringBuilder::validateUrlLength() const {
  std::string builtUrl = build();
  if (builtUrl.length() > MAX_URL_LENGTH) {
    std::ostringstream oss;
    oss << "Resulting URL too long: " << builtUrl.length()
        << " characters (max: " << MAX_URL_LENGTH << ")";
    throw exceptions::QueryStringBuilderException(
        oss.str(), exceptions::QueryStringBuilderException::INVALID_URL_FORMAT);
  }
}

void QueryStringBuilder::setBaseUrl(const std::string& baseUrl) {
  validateBaseUrl(baseUrl);
  m_baseUrl = baseUrl;
}

std::string QueryStringBuilder::getBaseUrl() const { return m_baseUrl; }

template <typename T>
void QueryStringBuilder::addParameterInternal(const std::string& key,
                                              const T& value) {
  validateParameterName(key);

  std::ostringstream ossValue;
  ossValue << value;
  std::string valueStr = ossValue.str();

  validateParameterValue(valueStr);

  if (m_parameters.find(key) != m_parameters.end()) {
    std::ostringstream ossKey;
    ossKey << "Parameter '" << key << "' already exists";
    throw exceptions::QueryStringBuilderException(
        ossKey.str(),
        exceptions::QueryStringBuilderException::DUPLICATE_PARAMETER);
  }

  m_parameters[key] = valueStr;
  validateParameterCount();
}

void QueryStringBuilder::addParameter(const std::string& key,
                                      const std::string& value) {
  addParameterInternal(key, value);
}

void QueryStringBuilder::addParameter(const std::string& key,
                                      const char* value) {
  addParameterInternal(key, std::string(value));
}

void QueryStringBuilder::addParameter(const std::string& key, int value) {
  addParameterInternal(key, value);
}

void QueryStringBuilder::addParameter(const std::string& key, bool value) {
  addParameterInternal(key, value ? "true" : "false");
}

void QueryStringBuilder::addParameter(const std::string& key, long value) {
  addParameterInternal(key, value);
}

void QueryStringBuilder::addParameter(const std::string& key, double value) {
  addParameterInternal(key, value);
}

void QueryStringBuilder::setParameter(const std::string& key,
                                      const std::string& value) {
  validateParameterName(key);
  validateParameterValue(value);
  m_parameters[key] = value;
  validateParameterCount();
}

void QueryStringBuilder::setParameter(const std::string& key,
                                      const char* value) {
  setParameter(key, std::string(value));
}

void QueryStringBuilder::setParameter(const std::string& key, int value) {
  validateParameterName(key);
  std::ostringstream oss;
  oss << value;
  validateParameterValue(oss.str());
  m_parameters[key] = oss.str();
  validateParameterCount();
}

void QueryStringBuilder::setParameter(const std::string& key, bool value) {
  validateParameterName(key);
  std::string val = (value ? "true" : "false");
  validateParameterValue(val);
  m_parameters[key] = val;
  validateParameterCount();
}

void QueryStringBuilder::setParameter(const std::string& key, long value) {
  validateParameterName(key);
  std::ostringstream oss;
  oss << value;
  validateParameterValue(oss.str());
  m_parameters[key] = oss.str();
  validateParameterCount();
}

void QueryStringBuilder::setParameter(const std::string& key, double value) {
  validateParameterName(key);
  std::ostringstream oss;
  oss << value;
  validateParameterValue(oss.str());
  m_parameters[key] = oss.str();
  validateParameterCount();
}

void QueryStringBuilder::removeParameter(const std::string& key) {
  std::map<std::string, std::string>::iterator iter = m_parameters.find(key);
  if (iter != m_parameters.end()) {
    m_parameters.erase(iter);
  }
}

void QueryStringBuilder::clearParameters() { m_parameters.clear(); }

bool QueryStringBuilder::hasParameter(const std::string& key) const {
  return m_parameters.find(key) != m_parameters.end();
}

std::string QueryStringBuilder::getParameter(const std::string& key) const {
  std::map<std::string, std::string>::const_iterator iter =
      m_parameters.find(key);
  if (iter != m_parameters.end()) {
    return iter->second;
  }
  return "";
}

std::string QueryStringBuilder::getParameterDecoded(
    const std::string& key) const {
  std::string encoded = getParameter(key);
  if (!encoded.empty()) {
    return decode(encoded);
  }
  return "";
}

int QueryStringBuilder::getParameterAsInt(const std::string& key,
                                          int defaultValue) const {
  return getParameterAs<int>(key, defaultValue);
}

bool QueryStringBuilder::getParameterAsBool(const std::string& key,
                                            bool defaultValue) const {
  std::string value = getParameter(key);
  if (value.empty()) {
    return defaultValue;
  }

  std::string lowerValue = value;
  for (std::size_t i = 0; i < lowerValue.size(); ++i) {
    lowerValue[i] =
        static_cast<char>(::tolower(static_cast<unsigned char>(lowerValue[i])));
  }

  return lowerValue == "true" || lowerValue == "1" || lowerValue == "yes" ||
         lowerValue == "on";
}

long QueryStringBuilder::getParameterAsLong(const std::string& key,
                                            long defaultValue) const {
  return getParameterAs<long>(key, defaultValue);
}

double QueryStringBuilder::getParameterAsDouble(const std::string& key,
                                                double defaultValue) const {
  return getParameterAs<double>(key, defaultValue);
}

template <typename T>
T QueryStringBuilder::getParameterAs(const std::string& key,
                                     T defaultValue) const {
  std::string value = getParameter(key);
  if (value.empty()) {
    return defaultValue;
  }

  std::istringstream iss(value);
  T result;
  if (!(iss >> result)) {
    return defaultValue;
  }

  return result;
}

std::size_t QueryStringBuilder::getParameterCount() const {
  return m_parameters.size();
}

std::map<std::string, std::string> QueryStringBuilder::getAllParameters()
    const {
  return m_parameters;
}

std::vector<std::pair<std::string, std::string> >
QueryStringBuilder::getParametersVector() const {
  std::vector<std::pair<std::string, std::string> > result;
  for (std::map<std::string, std::string>::const_iterator it =
           m_parameters.begin();
       it != m_parameters.end(); ++it) {
    result.push_back(std::make_pair(it->first, it->second));
  }
  return result;
}

std::string QueryStringBuilder::build() const {
  std::ostringstream result;
  result << m_baseUrl;

  if (!m_parameters.empty()) {
    result << "?";

    std::map<std::string, std::string>::const_iterator iter;
    bool first = true;

    for (iter = m_parameters.begin(); iter != m_parameters.end(); ++iter) {
      if (!first) {
        result << "&";
      }
      result << encode(iter->first) << "=" << encode(iter->second);
      first = false;
    }
  }

  return result.str();
}

std::string QueryStringBuilder::buildWithParameters(
    const std::map<std::string, std::string>& additionalParams) const {
  std::map<std::string, std::string> allParams = m_parameters;

  for (std::map<std::string, std::string>::const_iterator it =
           additionalParams.begin();
       it != additionalParams.end(); ++it) {
    validateParameterName(it->first);
    validateParameterValue(it->second);
    allParams[it->first] = it->second;
  }

  QueryStringBuilder tempBuilder(m_baseUrl);
  tempBuilder.m_parameters = allParams;
  return tempBuilder.build();
}

std::string QueryStringBuilder::buildWithOverrides(
    const std::map<std::string, std::string>& overrideParams) const {
  std::map<std::string, std::string> mergedParams = m_parameters;

  for (std::map<std::string, std::string>::const_iterator it =
           overrideParams.begin();
       it != overrideParams.end(); ++it) {
    validateParameterName(it->first);
    validateParameterValue(it->second);
    mergedParams[it->first] = it->second;
  }

  QueryStringBuilder tempBuilder(m_baseUrl);
  tempBuilder.m_parameters = mergedParams;
  return tempBuilder.build();
}

bool QueryStringBuilder::isEmpty() const {
  return m_baseUrl.empty() && m_parameters.empty();
}

bool QueryStringBuilder::hasQueryString() const {
  return !m_parameters.empty();
}

std::string QueryStringBuilder::encode(const std::string& value) {
  if (value.empty()) {
    return "";
  }

  std::ostringstream encoded;

  for (std::size_t i = 0; i < value.length(); ++i) {
    unsigned char chr = static_cast<unsigned char>(value[i]);

    if (isUnreservedCharacter(static_cast<char>(chr))) {
      encoded << chr;
    } else if (chr == ' ') {
      encoded << '+';
    } else {
      try {
        encoded << '%' << std::hex << std::uppercase << std::setw(2)
                << std::setfill('0') << static_cast<int>(chr);
      } catch (...) {
        std::ostringstream oss;
        oss << "Failed to encode character at position " << i;
        throw exceptions::QueryStringBuilderException(
            oss.str(), exceptions::QueryStringBuilderException::ENCODING_ERROR);
      }
    }
  }

  return encoded.str();
}

// TODO: remove magic number
std::string QueryStringBuilder::decode(const std::string& value) {
  if (value.empty()) {
    return "";
  }

  std::ostringstream decoded;

  for (std::size_t i = 0; i < value.length(); ++i) {
    if (value[i] == '%' && i + 2 < value.length()) {
      std::string hex = value.substr(i + 1, 2);

      char* endPtr = NULL;
      long charCode = std::strtol(hex.c_str(), &endPtr, 16);

      if (endPtr == hex.c_str() + 2 && charCode >= 0 && charCode <= 255) {
        decoded << static_cast<char>(charCode);
        i += 2;
      } else {
        throw exceptions::QueryStringBuilderException(
            "Invalid percent encoding: '%" + hex + "'",
            exceptions::QueryStringBuilderException::DECODING_ERROR);
      }
    } else if (value[i] == '+') {
      decoded << ' ';
    } else {
      decoded << value[i];
    }
  }

  return decoded.str();
}

QueryStringBuilder QueryStringBuilder::fromString(const std::string& url) {
  if (url.empty()) {
    throw exceptions::QueryStringBuilderException(
        "URL string cannot be empty",
        exceptions::QueryStringBuilderException::EMPTY_URL);
  }

  if (url.length() > MAX_URL_LENGTH) {
    std::ostringstream oss;
    oss << "URL too long: " << url.length()
        << " characters (max: " << MAX_URL_LENGTH << ")";
    throw exceptions::QueryStringBuilderException(
        oss.str(), exceptions::QueryStringBuilderException::INVALID_URL_FORMAT);
  }

  std::size_t questionPos = url.find('?');

  if (questionPos == std::string::npos) {
    return QueryStringBuilder(url);
  }

  QueryStringBuilder builder(url.substr(0, questionPos));
  std::string queryString = url.substr(questionPos + 1);

  if (!queryString.empty()) {
    builder.m_parameters = parseQueryString(queryString).m_parameters;
  }

  return builder;
}

// TODO: refactor this func
QueryStringBuilder QueryStringBuilder::parseQueryString(
    const std::string& queryString) {
  QueryStringBuilder builder;

  if (queryString.empty()) {
    return builder;
  }

  std::size_t start = 0;
  while (start < queryString.length()) {
    std::size_t ampPos = queryString.find('&', start);
    std::string pair;

    if (ampPos == std::string::npos) {
      pair = queryString.substr(start);
      start = queryString.length();
    } else {
      pair = queryString.substr(start, ampPos - start);
      start = ampPos + 1;
    }

    if (pair.empty()) {
      continue;
    }

    std::size_t equalPos = pair.find('=');
    if (equalPos != std::string::npos) {
      std::string key = decode(pair.substr(0, equalPos));
      std::string value = decode(pair.substr(equalPos + 1));

      if (!isValidParameterName(key)) {
        continue;
      }

      if (builder.m_parameters.find(key) == builder.m_parameters.end()) {
        builder.m_parameters[key] = value;
      }
    } else {
      continue;
    }
  }

  return builder;
}

bool QueryStringBuilder::isValidParameterName(const std::string& name) {
  if (name.empty()) {
    return false;
  }

  if (name.length() > MAX_PARAMETER_NAME_LENGTH) {
    return false;
  }

  for (std::size_t i = 0; i < name.length(); ++i) {
    unsigned char chr = static_cast<unsigned char>(name[i]);
    if (chr < ' ' || chr == '=' || chr == '&' || chr == '?' || chr == '#') {
      return false;
    }
  }

  return true;
}

bool QueryStringBuilder::isValidParameterValue(const std::string& value) {
  return value.length() <= MAX_PARAMETER_VALUE_LENGTH;
}

bool QueryStringBuilder::isReservedCharacter(char chr) {
  switch (chr) {
    case '!':
    case '#':
    case '$':
    case '&':
    case '\'':
    case '(':
    case ')':
    case '*':
    case '+':
    case ',':
    case '/':
    case ':':
    case ';':
    case '=':
    case '?':
    case '@':
    case '[':
    case ']':
      return true;
    default:
      return false;
  }
}

bool QueryStringBuilder::isUnreservedCharacter(char chr) {
  unsigned char uchr = static_cast<unsigned char>(chr);
  return (std::isalnum(uchr) != 0) || chr == '-' || chr == '_' || chr == '.' ||
         chr == '~';
}

// TODO: remove magic number
char QueryStringBuilder::hexToChar(const std::string& hex) {
  char* endPtr = NULL;
  long value = std::strtol(hex.c_str(), &endPtr, 16);
  if (endPtr == hex.c_str() + hex.length() && value >= 0 && value <= 255) {
    return static_cast<char>(value);
  }
  return '\0';
}

std::string QueryStringBuilder::charToHex(char chr) {
  std::ostringstream oss;
  oss << std::hex << std::uppercase << std::setw(2) << std::setfill('0')
      << static_cast<int>(static_cast<unsigned char>(chr));
  return oss.str();
}

}  // namespace value_objects
}  // namespace http
}  // namespace domain
