/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Host.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/24 14:44:40 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/24 14:56:07 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HOST_HPP
#define HOST_HPP

#include <string>

namespace domain {
namespace value_objects {

class Host {
 public:
  static const std::size_t MAX_HOST_LENGTH = 253;
  static const std::size_t MAX_LABEL_LENGTH = 63;
  static const std::size_t MIN_IPV4_OCTET = 0;
  static const std::size_t MAX_IPV4_OCTET = 255;

  static const std::size_t MAX_IPV6_LENGTH = 45;
  static const std::size_t MIN_IPV6_COLONS = 2;
  static const std::size_t MAX_IPV6_COLONS = 7;
  static const std::size_t MAX_DOUBLE_COLONS = 1;

  static const std::string LOCALHOST_IPV4;
  static const std::string LOCALHOST_IPV6;
  static const std::string WILDCARD_IPV4;
  static const std::string WILDCARD_IPV6;
  static const std::string WILDCARD_SHORT;

  static const std::string VALID_HOSTNAME_CHARS;
  static const char LABEL_SEPARATOR = '.';
  static const char IPV6_SEPARATOR = ':';

  enum HostType {
    TYPE_UNKNOWN,
    TYPE_IPV4,
    TYPE_IPV6,
    TYPE_HOSTNAME,
    TYPE_WILDCARD
  };

  Host();
  explicit Host(const std::string& host);
  Host(const Host& other);
  ~Host();

  Host& operator=(const Host& other);

  std::string getValue() const;
  HostType getType() const;
  std::string getTypeString() const;

  static bool isValidHost(const std::string& host);
  static bool isValidIpv4(const std::string& ipv4);
  static bool isValidIpv6(const std::string& ipv6);
  static bool isValidHostname(const std::string& hostname);

  bool isIpv4() const;
  bool isIpv6() const;
  bool isLocalhost() const;
  bool isWildcard() const;
  bool isHostname() const;

  bool operator==(const Host& other) const;
  bool operator!=(const Host& other) const;
  bool operator<(const Host& other) const;

  static Host fromString(const std::string& host);
  static Host localhost();
  static Host wildcard();

 private:
  std::string m_value;
  HostType m_type;

  void parseHost(const std::string& host);
  void validate() const;
  static void validateHostString(const std::string& host);

  static HostType determineType(const std::string& host);
  static bool parseIpv4(const std::string& ipv4);
  static bool parseIpv6(const std::string& ipv6);
  static bool parseHostname(const std::string& hostname);

  static bool isValidIpv4Octet(const std::string& octet);
  static bool isValidHostnameLabel(const std::string& label);
  static bool isAllDigits(const std::string& str);
};

}  // namespace value_objects
}  // namespace domain

#endif  // HOST_HPP
