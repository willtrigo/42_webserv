/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ListenDirective.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/26 03:21:07 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/26 03:22:24 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LISTEN_DIRECTIVE_HPP
#define LISTEN_DIRECTIVE_HPP

#include "domain/value_objects/Host.hpp"
#include "domain/value_objects/Port.hpp"

#include <string>

namespace domain {
namespace entities {

class ListenDirective {
 public:
  static const std::string DEFAULT_HOST;
  static const value_objects::Port DEFAULT_PORT;

  ListenDirective();
  ListenDirective(const value_objects::Host& host, const value_objects::Port& port);
  explicit ListenDirective(const std::string& directiveString);
  ListenDirective(const ListenDirective& other);
  ~ListenDirective();

  ListenDirective& operator=(const ListenDirective& other);

  value_objects::Host getHost() const;
  value_objects::Port getPort() const;
  std::string toString() const;
  std::string toCanonicalString() const;

  static bool isValidDirective(const std::string& directiveString);
  static bool isDefaultListen(const ListenDirective& directive);

  bool operator==(const ListenDirective& other) const;
  bool operator!=(const ListenDirective& other) const;
  bool operator<(const ListenDirective& other) const;

  bool isEmpty() const;
  bool isWildcard() const;
  bool isLocalhost() const;
  bool isDefaultPort() const;

  static ListenDirective fromString(const std::string& directiveString);
  static ListenDirective createDefault();
  static ListenDirective createWildcard(const value_objects::Port& port);
  static ListenDirective createLocalhost(const value_objects::Port& port);

  static std::pair<std::string, std::string> splitDirective(
      const std::string& directiveString);

 private:
  value_objects::Host m_host;
  value_objects::Port m_port;

  void validate() const;
  static void validateDirectiveString(const std::string& directiveString);
  static std::string normalizeHostString(const std::string& hostString);
  static std::string normalizePortString(const std::string& portString);
  static bool hasPort(const std::string& directiveString);
};

}  // namespace value_objects
}  // namespace domain

#endif  // LISTEN_DIRECTIVE_HPP
