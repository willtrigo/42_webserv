/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Logger.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/14 12:05:55 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/14 20:10:04 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "application/ports/IStreamWriter.hpp"
#include "infrastructure/logging/Logger.hpp"
#include "shared/utils/TerminalColor.hpp"

#include <ctime>
#include <iostream>
#include <sstream>

const infrastructure::logging::Logger::LogConfig
    infrastructure::logging::Logger::LOG_CONFIG[] = {
        {"DEBUG", CYAN, std::cout},
        {"INFO", GREEN, std::cout},
        {"WARN", YELLOW, std::cout},
        {"ERROR", RED, std::cerr}};

infrastructure::logging::Logger::Logger(
    application::ports::IStreamWriter& consoleWriter, application::ports::IFileWriter& logFile)
    : m_consoleWriter(consoleWriter), m_logFile(logFile) {
  log(INFO, "Logger initialized.");
}

infrastructure::logging::Logger::Logger(const Logger& other)
    : m_consoleWriter(other.m_consoleWriter), m_logFile(other.m_logFile) {}

infrastructure::logging::Logger::~Logger() {}

void infrastructure::logging::Logger::log(LogLevel level,
                                          const std::string& msg) {
  this->m_consoleWriter.print(LOG_CONFIG[level].stream, formatMsg(msg, level ,true), true);
  this->m_logFile.write(formatMsg(msg, level, false), true);
}

void infrastructure::logging::Logger::debug(const std::string& msg) {
  log(DEBUG, msg);
}

void infrastructure::logging::Logger::info(const std::string& msg) {
  log(INFO, msg);
}

void infrastructure::logging::Logger::warn(const std::string& msg) {
  log(WARN, msg);
}

void infrastructure::logging::Logger::error(const std::string& msg) {
  log(ERROR, msg);
}

std::string infrastructure::logging::Logger::formatMsg(const std::string& msg,
                                                       LogLevel level, bool isConsole) {
  std::ostringstream oss;

  if (!isConsole) {
    oss << "[" << getTimestamp() << "] ";
    oss << "[" + LOG_CONFIG[level].name + "] ";
  } else {
    oss << TerminalColor::setColor(LOG_CONFIG[level].color,
                                   "[" + LOG_CONFIG[level].name + "] ");
  }
  oss << msg;

  return oss.str();
}

std::string infrastructure::logging::Logger::getTimestamp() {
  time_t now = time(NULL);
  char buf[K_TIME_STRING_LENGTH];
  ctime_r(&now, buf);
  return std::string(buf, K_OUTPUT_STRING_LENGTH);
}
