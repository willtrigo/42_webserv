/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ILogger.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/13 23:37:46 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/18 11:16:13 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ILOGGER_HPP
#define ILOGGER_HPP

#include <string>

enum LogLevel { DEBUG, INFO, WARN, ERROR };

namespace application {
namespace ports {

class ILogger {
 public:
  virtual ~ILogger() {}

  virtual void debug(const std::string& msg) = 0;
  virtual void info(const std::string& msg) = 0;
  virtual void warn(const std::string& msg) = 0;
  virtual void error(const std::string& msg) = 0;

private:
  virtual void log(LogLevel level, const std::string& msg) = 0;
};

}  // namespace ports
}  // namespace application

#endif  // ILOGGER_HPP
