/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IConfigParser.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/18 18:05:45 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/18 18:49:48 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ICONFIG_PARSER_HPP
#define ICONFIG_PARSER_HPP

#include <string>

namespace application {
namespace ports {

class IConfigParser {
 public:
  virtual ~IConfigParser() {}
  virtual void parsePath(const std::string& configPath) = 0;
  virtual void mergeIncludes(const std::string& includePath) = 0;
};

}  // namespace ports
}  // namespace application

#endif  // ICONFIG_PARSER_HPP
