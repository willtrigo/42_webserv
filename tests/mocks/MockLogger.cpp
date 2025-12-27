/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MockLogger.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: umeneses <umeneses@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/27 12:24:22 by umeneses          #+#    #+#             */
/*   Updated: 2025/12/27 14:03:35 by umeneses         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "MockLogger.hpp"

namespace tests {
namespace mocks {

// LogEntry constructor implementation
MockLogger::LogEntry::LogEntry(LogLevel lvl, const std::string& msg) 
  : level(lvl), message(msg) {
}

// MockLogger constructor
MockLogger::MockLogger() {
  // Nothing to initialize - vector is empty by default
}

// MockLogger destructor
MockLogger::~MockLogger() {
  // Nothing to clean up - vector cleans itself up
}

// ILogger interface implementations

void MockLogger::debug(const std::string& msg) {
  log(DEBUG, msg);
}

void MockLogger::info(const std::string& msg) {
  log(INFO, msg);
}

void MockLogger::warn(const std::string& msg) {
  log(WARN, msg);
}

void MockLogger::error(const std::string& msg) {
  log(ERROR, msg);
}

// Getter methods implementations

const std::vector<MockLogger::LogEntry>& MockLogger::getLogs() const {
  return m_logs;
}

std::size_t MockLogger::getLogCount() const {
  return m_logs.size();
}

std::size_t MockLogger::getLogCountByLevel(LogLevel level) const {
  std::size_t count = 0;
  
  // Loop through all logs
  for (std::size_t i = 0; i < m_logs.size(); ++i) {
    // If this log matches the level we're looking for
    if (m_logs[i].level == level) {
      count++;  // Count it
    }
  }
  
  return count;
}

bool MockLogger::hasLog(LogLevel level, const std::string& msg) const {
  // Loop through all logs
  for (std::size_t i = 0; i < m_logs.size(); ++i) {
    // Check if this log matches what we're looking for
    bool levelMatches = (m_logs[i].level == level);
    bool messageContains = (m_logs[i].message.find(msg) != std::string::npos);
    
    if (levelMatches && messageContains) {
      return true;  // Found it!
    }
  }
  
  return false;  // Didn't find it
}

void MockLogger::clear() {
  m_logs.clear();
}

// Private method implementation

void MockLogger::log(LogLevel level, const std::string& msg) {
  // Create a new log entry and add it to our vector
  m_logs.push_back(LogEntry(level, msg));
}

}  // namespace mocks
}  // namespace tests
