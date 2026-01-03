/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Port.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: umeneses <umeneses@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/19 19:12:04 by dande-je          #+#    #+#             */
/*   Updated: 2026/01/02 22:02:53 by umeneses         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PORT_HPP
#define PORT_HPP

#include <string>

namespace domain {
namespace http {
namespace value_objects {

class Port {
 public:
  static const unsigned int MIN_PORT = 1;
  static const unsigned int DEFAULT_PORT = 80;
  static const unsigned int MAX_PORT = 65535;

  static const unsigned int HTTP_PORT = 80;
  static const unsigned int HTTPS_PORT = 443;
  static const unsigned int FTP_PORT = 21;
  static const unsigned int SSH_PORT = 22;
  static const unsigned int SMTP_PORT = 25;
  static const unsigned int DNS_PORT = 53;

  static const unsigned int WELL_KNOWN_MAX = 1023;
  static const unsigned int REGISTERED_MIN = 1024;
  static const unsigned int REGISTERED_MAX = 49151;
  static const unsigned int DYNAMIC_MIN = 49152;
  static const unsigned int DYNAMIC_MAX = MAX_PORT;

  static const int BASE_DECIMAL = 10;

  Port();
  explicit Port(unsigned int port);
  explicit Port(const std::string& portString);
  Port(const Port& other);
  ~Port();

  Port& operator=(const Port& other);

  unsigned int getValue() const;

  static bool isValidPort(unsigned int port);

  bool operator==(const Port& other) const;
  bool operator!=(const Port& other) const;
  bool operator<(const Port& other) const;
  bool operator<=(const Port& other) const;
  bool operator>(const Port& other) const;
  bool operator>=(const Port& other) const;

  static Port httpPort();
  static Port httpsPort();
  static Port ftpPort();
  static Port sshPort();
  static Port smtpPort();
  static Port dnsPort();
  static Port fromString(const std::string& portString);

  bool isWellKnown() const;
  bool isRegistered() const;
  bool isDynamic() const;

 private:
  unsigned int m_value;

  void validate() const;
  static unsigned int parsePortString(const std::string& portString);
  static bool isAllDigits(const std::string& str);
};

}  // namespace value_objects
}  // namespace http
}  // namespace domain

#endif  // PORT_HPP
