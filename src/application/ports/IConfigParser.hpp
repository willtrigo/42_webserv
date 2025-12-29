/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IConfigParser.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/18 18:05:45 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/28 16:34:18 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ICONFIG_PARSER_HPP
#define ICONFIG_PARSER_HPP

#include "domain/configuration/entities/HttpConfig.hpp"

#include <string>

namespace application {
namespace ports {

class IConfigParser {
 public:
  virtual ~IConfigParser() {}
  virtual domain::configuration::entities::HttpConfig* parseFile(
      const std::string& configPath) = 0;
  virtual void mergeIncludes(
      domain::configuration::entities::HttpConfig& httpConfig,
      const std::string& includePath) = 0;
  virtual void validateConfiguration(
      const domain::configuration::entities::HttpConfig& httpConfig) = 0;
};

}  // namespace ports
}  // namespace application

#endif  // ICONFIG_PARSER_HPP
