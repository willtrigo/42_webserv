/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Logger.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/14 00:26:18 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/14 20:03:12 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOGGER_HPP
#define LOGGER_HPP

#include "application/ports/IFileWriter.hpp"
#include "application/ports/ILogger.hpp"
#include "application/ports/IStreamWriter.hpp"
#include "shared/utils/TerminalColor.hpp"

#include <string>
#include <iostream>
#include <ostream>

namespace infrastructure {
namespace logging {

class Logger : public application::ports::ILogger {
 public:
  explicit Logger(application::ports::IStreamWriter& consoleWriter, application::ports::IFileWriter& logFile);
  virtual ~Logger();

  void debug(const std::string& msg);
  void info(const std::string& msg);
  void warn(const std::string& msg);
  void error(const std::string& msg);

 private:
  struct LogConfig {
    std::string name;
    StrColor color;
    std::ostream& stream;
  };

  Logger(const Logger& other);

  Logger& operator=(const Logger&);

  void log(LogLevel level, const std::string& msg);
  static std::string formatMsg(const std::string& msg, LogLevel level, bool isConsole);
  static std::string getTimestamp();

  application::ports::IStreamWriter& m_consoleWriter;
  application::ports::IFileWriter& m_logFile;

  static const LogConfig LOG_CONFIG[];
  static const size_t K_TIME_STRING_LENGTH = 26;
  static const size_t K_OUTPUT_STRING_LENGTH = 24;
};

}  // namespace logging
}  // namespace infrastructure

#endif  // LOGGER_HPP
