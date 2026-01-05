/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   QueryStringBuilder.hpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: umeneses <umeneses@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/23 00:38:11 by dande-je          #+#    #+#             */
/*   Updated: 2026/01/04 13:34:15 by umeneses         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef QUERY_STRING_BUILDER_HPP
#define QUERY_STRING_BUILDER_HPP

#include <map>
#include <string>
#include <vector>

namespace domain {
namespace http {
namespace value_objects {

class QueryStringBuilder {
 public:
  static const std::size_t MAX_URL_LENGTH = 4096;
  static const std::size_t MAX_PARAMETER_NAME_LENGTH = 256;
  static const std::size_t MAX_PARAMETER_VALUE_LENGTH = 1024;
  static const std::size_t MAX_PARAMETERS_COUNT = 100;

  QueryStringBuilder();
  explicit QueryStringBuilder(const std::string& baseUrl);
  ~QueryStringBuilder();

  QueryStringBuilder(const QueryStringBuilder& other);
  QueryStringBuilder& operator=(const QueryStringBuilder& other);

  void setBaseUrl(const std::string& baseUrl);
  std::string getBaseUrl() const;

  void addParameter(const std::string& key, const std::string& value);
  void addParameter(const std::string& key, const char* value);
  void addParameter(const std::string& key, int value);
  void addParameter(const std::string& key, bool value);
  void addParameter(const std::string& key, long value);
  void addParameter(const std::string& key, double value);

  void setParameter(const std::string& key, const std::string& value);
  void setParameter(const std::string& key, const char* value);
  void setParameter(const std::string& key, int value);
  void setParameter(const std::string& key, bool value);
  void setParameter(const std::string& key, long value);
  void setParameter(const std::string& key, double value);

  void removeParameter(const std::string& key);
  void clearParameters();

  bool hasParameter(const std::string& key) const;
  std::string getParameter(const std::string& key) const;
  int getParameterAsInt(const std::string& key, int defaultValue = 0) const;
  bool getParameterAsBool(const std::string& key,
                          bool defaultValue = false) const;
  long getParameterAsLong(const std::string& key, long defaultValue = 0) const;
  double getParameterAsDouble(const std::string& key,
                              double defaultValue = 0.0) const;

  std::string getParameterDecoded(const std::string& key) const;

  std::size_t getParameterCount() const;
  std::map<std::string, std::string> getAllParameters() const;
  std::vector<std::pair<std::string, std::string> > getParametersVector() const;

  std::string build() const;
  std::string buildWithParameters(
      const std::map<std::string, std::string>& additionalParams) const;
  std::string buildWithOverrides(
      const std::map<std::string, std::string>& overrideParams) const;

  bool isEmpty() const;
  bool hasQueryString() const;

  static std::string encode(const std::string& value);
  static std::string decode(const std::string& value);

  static QueryStringBuilder fromString(const std::string& url);
  static QueryStringBuilder parseQueryString(const std::string& queryString);

  static bool isValidParameterName(const std::string& name);
  static bool isValidParameterValue(const std::string& value);

 private:
  std::string m_baseUrl;
  std::map<std::string, std::string> m_parameters;

  static void validateBaseUrl(const std::string& url);
  static void validateParameterName(const std::string& name);
  static void validateParameterValue(const std::string& value);
  void validateParameterCount() const;
  void validateUrlLength() const;

  static bool isReservedCharacter(char chr);
  static bool isUnreservedCharacter(char chr);
  static char hexToChar(const std::string& hex);
  static std::string charToHex(char chr);

  template <typename T>
  void addParameterInternal(const std::string& key, const T& value);

  template <typename T>
  T getParameterAs(const std::string& key, T defaultValue) const;
};

}  // namespace value_objects
}  // namespace http
}  // namespace domain

#endif  // QUERY_STRING_BUILDER_HPP
