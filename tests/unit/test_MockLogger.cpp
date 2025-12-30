/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_MockLogger.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: umeneses <umeneses@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/27 10:35:58 by umeneses          #+#    #+#             */
/*   Updated: 2025/12/29 12:50:54 by umeneses         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "mocks/MockLogger.hpp"
#include "application/ports/ILogger.hpp"
#include <gtest/gtest.h>

using namespace tests::mocks;

class MockLoggerTest : public ::testing::Test {
 protected:
  virtual void SetUp() {}
  virtual void TearDown() {}
};

// ============================================================================
// Construction Tests
// ============================================================================

TEST_F(MockLoggerTest, DefaultConstructorShouldHaveNoLogs) {
  MockLogger logger;
  
  EXPECT_EQ(0, logger.getLogCount());
}

// ============================================================================
// Logging Tests
// ============================================================================

TEST_F(MockLoggerTest, DebugShouldStoreDebugLevelLog) {
  MockLogger logger;
  
  logger.debug("Debug message");
  
  EXPECT_EQ(1, logger.getLogCount());
  const std::vector<MockLogger::LogEntry>& logs = logger.getLogs();
  EXPECT_EQ(DEBUG, logs[0].level);
  EXPECT_EQ("Debug message", logs[0].message);
}

TEST_F(MockLoggerTest, InfoShouldStoreInfoLevelLog) {
  MockLogger logger;
  
  logger.info("Info message");
  
  EXPECT_EQ(1, logger.getLogCount());
  const std::vector<MockLogger::LogEntry>& logs = logger.getLogs();
  EXPECT_EQ(INFO, logs[0].level);
  EXPECT_EQ("Info message", logs[0].message);
}

TEST_F(MockLoggerTest, WarnShouldStoreWarnLevelLog) {
  MockLogger logger;
  
  logger.warn("Warning message");
  
  EXPECT_EQ(1, logger.getLogCount());
  const std::vector<MockLogger::LogEntry>& logs = logger.getLogs();
  EXPECT_EQ(WARN, logs[0].level);
  EXPECT_EQ("Warning message", logs[0].message);
}

TEST_F(MockLoggerTest, ErrorShouldStoreErrorLevelLog) {
  MockLogger logger;
  
  logger.error("Error message");
  
  EXPECT_EQ(1, logger.getLogCount());
  const std::vector<MockLogger::LogEntry>& logs = logger.getLogs();
  EXPECT_EQ(ERROR, logs[0].level);
  EXPECT_EQ("Error message", logs[0].message);
}

TEST_F(MockLoggerTest, MultipleLogsShouldStoreInOrder) {
  MockLogger logger;
  
  logger.debug("First");
  logger.info("Second");
  logger.warn("Third");
  logger.error("Fourth");
  
  EXPECT_EQ(4, logger.getLogCount());
  const std::vector<MockLogger::LogEntry>& logs = logger.getLogs();
  EXPECT_EQ(DEBUG, logs[0].level);
  EXPECT_EQ("First", logs[0].message);
  EXPECT_EQ(INFO, logs[1].level);
  EXPECT_EQ("Second", logs[1].message);
  EXPECT_EQ(WARN, logs[2].level);
  EXPECT_EQ("Third", logs[2].message);
  EXPECT_EQ(ERROR, logs[3].level);
  EXPECT_EQ("Fourth", logs[3].message);
}

// ============================================================================
// Query Tests
// ============================================================================

TEST_F(MockLoggerTest, GetLogsShouldReturnAllLogs) {
  MockLogger logger;
  logger.info("Message 1");
  logger.error("Message 2");
  
  const std::vector<MockLogger::LogEntry>& logs = logger.getLogs();
  
  EXPECT_EQ(2, logs.size());
}

TEST_F(MockLoggerTest, GetLogCountShouldReturnCorrectCount) {
  MockLogger logger;
  logger.debug("1");
  logger.info("2");
  logger.warn("3");
  
  EXPECT_EQ(3, logger.getLogCount());
}

// ============================================================================
// Clear Tests
// ============================================================================

TEST_F(MockLoggerTest, ClearShouldRemoveAllLogs) {
  MockLogger logger;
  logger.debug("Message 1");
  logger.info("Message 2");
  
  logger.clear();
  
  EXPECT_EQ(0, logger.getLogCount());
}

TEST_F(MockLoggerTest, ClearShouldAllowNewLogs) {
  MockLogger logger;
  logger.debug("Before clear");
  logger.clear();
  
  logger.info("After clear");
  
  EXPECT_EQ(1, logger.getLogCount());
  const std::vector<MockLogger::LogEntry>& logs = logger.getLogs();
  EXPECT_EQ("After clear", logs[0].message);
}

// ============================================================================
// Level Filter Tests
// ============================================================================

TEST_F(MockLoggerTest, GetLogCountByLevelShouldCountDebugLogs) {
  MockLogger logger;
  logger.debug("Debug 1");
  logger.info("Info");
  logger.debug("Debug 2");
  
  EXPECT_EQ(2, logger.getLogCountByLevel(DEBUG));
}

TEST_F(MockLoggerTest, GetLogCountByLevelShouldCountErrorLogs) {
  MockLogger logger;
  logger.info("Info");
  logger.error("Error 1");
  logger.error("Error 2");
  
  EXPECT_EQ(2, logger.getLogCountByLevel(ERROR));
}

TEST_F(MockLoggerTest, HasLogShouldFindSpecificLogEntry) {
  MockLogger logger;
  logger.info("Specific message");
  logger.error("Another message");
  
  EXPECT_TRUE(logger.hasLog(INFO, "Specific message"));
  EXPECT_FALSE(logger.hasLog(ERROR, "Specific message"));
}
