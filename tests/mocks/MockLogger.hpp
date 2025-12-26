/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MockLogger.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: test                                        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/24 00:00:00 by test              #+#    #+#             */
/*   Updated: 2025/12/26 00:00:00 by test             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MOCK_LOGGER_HPP
#define MOCK_LOGGER_HPP

// This is a FAKE logger for testing
// Instead of writing to a file or console, it saves messages in memory
// So we can check what was logged during tests

#include "application/ports/ILogger.hpp"
#include <vector>
#include <string>

namespace tests {
namespace mocks {

// A Mock Logger pretends to be a real logger
// It implements the ILogger interface but stores logs in a vector instead
class MockLogger : public application::ports::ILogger {
 public:
  // Structure to hold one log entry
  struct LogEntry {
    LogLevel level;        // DEBUG, INFO, WARN, or ERROR
    std::string message;   // The log message

    // Simple constructor (can stay in header - it's just initialization)
    LogEntry(LogLevel lvl, const std::string& msg);
  };

  // Constructor and destructor declarations
  MockLogger();
  virtual ~MockLogger();

  // ILogger interface - declarations only
  virtual void debug(const std::string& msg);
  virtual void info(const std::string& msg);
  virtual void warn(const std::string& msg);
  virtual void error(const std::string& msg);

  // Getter methods - declarations only
  const std::vector<LogEntry>& getLogs() const;
  std::size_t getLogCount() const;
  std::size_t getLogCountByLevel(LogLevel level) const;
  bool hasLog(LogLevel level, const std::string& msg) const;
  void clear();

 private:
  // Vector that stores all our fake logs
  std::vector<LogEntry> m_logs;

  // Private method - declaration only
  virtual void log(LogLevel level, const std::string& msg);
};

}  // namespace mocks
}  // namespace tests

#endif  // MOCK_LOGGER_HPP
