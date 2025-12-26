# Writing Tests Guide

This guide explains how to write tests for the webserver project using Google Test.

## Table of Contents

- [Test Naming Conventions](#test-naming-conventions)
- [Writing Unit Tests](#writing-unit-tests)
- [Writing Integration Tests](#writing-integration-tests)
- [Using Mocks](#using-mocks)
- [Test Organization](#test-organization)
- [Best Practices](#best-practices)
- [Common Assertions](#common-assertions)
- [Testing Patterns](#testing-patterns)

---

## Test Naming Conventions

Follow these conventions for consistency:

### File Naming

```
unit/test_ClassName.cpp          # Unit tests
integration/test_Feature_Integration.cpp  # Integration tests
```

**Examples:**
- `test_HttpMethod.cpp` - Tests for HttpMethod class
- `test_ConfigParser.cpp` - Tests for ConfigParser class
- `test_FileHandler_Integration.cpp` - Integration tests for file handling

### Test Suite Naming

```cpp
class ClassNameTest : public ::testing::Test {
  // Test suite for ClassName
};
```

**Examples:**
```cpp
class HttpMethodTest : public ::testing::Test { };
class PortTest : public ::testing::Test { };
class RouteTest : public ::testing::Test { };
```

### Test Case Naming

Use descriptive names that explain what's being tested:

```cpp
TEST_F(ClassNameTest, DescriptiveTestName)
```

**Good names:**
```cpp
TEST_F(HttpMethodTest, ValidMethods)
TEST_F(HttpMethodTest, InvalidMethod)
TEST_F(PortTest, ValidatePortRange)
TEST_F(PathTest, GetDirectoryFromAbsolutePath)
```

**Bad names:**
```cpp
TEST_F(HttpMethodTest, Test1)        // ❌ Not descriptive
TEST_F(HttpMethodTest, TestMethod)   // ❌ Too vague
TEST_F(PortTest, Check)              // ❌ What are we checking?
```

---

## Writing Unit Tests

Unit tests focus on testing individual components in isolation.

### Basic Unit Test Structure

```cpp
#include "domain/value_objects/YourClass.hpp"
#include "shared/exceptions/YourException.hpp"
#include <gtest/gtest.h>

using namespace domain::value_objects;

// Test fixture
class YourClassTest : public ::testing::Test {
 protected:
  // Runs before EACH test
  virtual void SetUp() {
    // Initialize test data
  }
  
  // Runs after EACH test
  virtual void TearDown() {
    // Clean up
  }
  
  // Helper methods (optional)
  bool isValid(const std::string& value) {
    // Reusable test logic
    return !value.empty();
  }
};

// Test case
TEST_F(YourClassTest, DescriptiveName) {
  // Arrange: Set up test data
  YourClass obj("value");
  
  // Act: Execute the code under test
  std::string result = obj.getValue();
  
  // Assert: Verify the results
  EXPECT_EQ("value", result);
}
```

### Example: Testing Value Objects

Value objects should be:
- Immutable
- Validated on construction
- Comparable

```cpp
#include "domain/value_objects/Port.hpp"
#include "shared/exceptions/PortException.hpp"
#include <gtest/gtest.h>

class PortTest : public ::testing::Test {
 protected:
  virtual void SetUp() {}
  virtual void TearDown() {}
};

// Test valid construction
TEST_F(PortTest, ConstructWithValidPort) {
  EXPECT_NO_THROW(Port(8080));
  EXPECT_NO_THROW(Port(80));
  EXPECT_NO_THROW(Port(443));
}

// Test validation
TEST_F(PortTest, RejectInvalidPort) {
  EXPECT_THROW(Port(0), shared::exceptions::PortException);
  EXPECT_THROW(Port(65536), shared::exceptions::PortException);
  EXPECT_THROW(Port(-1), shared::exceptions::PortException);
}

// Test value retrieval
TEST_F(PortTest, GetValue) {
  Port port(8080);
  EXPECT_EQ(8080, port.getValue());
}

// Test equality
TEST_F(PortTest, Equality) {
  Port port1(8080);
  Port port2(8080);
  Port port3(8081);
  
  EXPECT_TRUE(port1 == port2);
  EXPECT_FALSE(port1 == port3);
}

// Test copy semantics
TEST_F(PortTest, CopyConstructor) {
  Port original(8080);
  Port copy(original);
  
  EXPECT_EQ(original.getValue(), copy.getValue());
}
```

---

## Writing Integration Tests

Integration tests verify that components work together correctly.

### Basic Integration Test Structure

```cpp
#include "infrastructure/filesystem/FileHandler.hpp"
#include "infrastructure/logging/Logger.hpp"
#include <gtest/gtest.h>
#include <fstream>

class FileHandlerIntegrationTest : public ::testing::Test {
 protected:
  std::string testDir;
  std::string testFile;
  
  virtual void SetUp() {
    // Create temporary test directory
    testDir = "/tmp/webserver_test/";
    testFile = testDir + "test.txt";
    
    system(("mkdir -p " + testDir).c_str());
  }
  
  virtual void TearDown() {
    // Clean up test directory
    system(("rm -rf " + testDir).c_str());
  }
};

TEST_F(FileHandlerIntegrationTest, ReadExistingFile) {
  // Arrange: Create a test file
  std::ofstream file(testFile.c_str());
  file << "test content";
  file.close();
  
  // Act: Read file using FileHandler
  FileHandler handler;
  std::string content = handler.read(testFile);
  
  // Assert: Verify content
  EXPECT_EQ("test content", content);
}
```

### Integration Test Guidelines

1. **Use real dependencies** (filesystem, network) not mocks
2. **Set up clean test environment** in SetUp()
3. **Clean up** in TearDown() - don't leave artifacts
4. **Use temporary locations** (e.g., /tmp/)
5. **Test realistic scenarios** - not just happy paths

---

## Using Mocks

Mocks simulate dependencies for isolated testing.

### When to Use Mocks

- **Unit tests** - Mock all external dependencies
- **Testing error handling** - Mock errors you can't easily trigger
- **Testing interfaces** - When concrete implementation isn't ready

### Creating a Mock

#### Step 1: Define Mock Interface

```cpp
// tests/mocks/MockLogger.hpp
#ifndef TESTS_MOCKS_MOCK_LOGGER_HPP_
#define TESTS_MOCKS_MOCK_LOGGER_HPP_

#include "application/ports/ILogger.hpp"
#include <string>
#include <vector>

namespace tests {
namespace mocks {

class MockLogger : public application::ports::ILogger {
 public:
  MockLogger();
  virtual ~MockLogger();
  
  // ILogger interface implementation
  virtual void debug(const std::string& message);
  virtual void info(const std::string& message);
  virtual void warn(const std::string& message);
  virtual void error(const std::string& message);
  
  // Test helper methods
  bool hasLog(LogLevel level, const std::string& message) const;
  int getLogCount(LogLevel level) const;
  void clear();
  
 private:
  struct LogEntry {
    LogLevel level;
    std::string message;
  };
  
  std::vector<LogEntry> logs_;
};

}  // namespace mocks
}  // namespace tests

#endif  // TESTS_MOCKS_MOCK_LOGGER_HPP_
```

#### Step 2: Implement Mock

```cpp
// tests/mocks/MockLogger.cpp
#include "tests/mocks/MockLogger.hpp"

namespace tests {
namespace mocks {

MockLogger::MockLogger() {}
MockLogger::~MockLogger() {}

void MockLogger::debug(const std::string& message) {
  LogEntry entry = {DEBUG, message};
  logs_.push_back(entry);
}

void MockLogger::info(const std::string& message) {
  LogEntry entry = {INFO, message};
  logs_.push_back(entry);
}

bool MockLogger::hasLog(LogLevel level, const std::string& message) const {
  for (size_t i = 0; i < logs_.size(); ++i) {
    if (logs_[i].level == level && logs_[i].message == message) {
      return true;
    }
  }
  return false;
}

void MockLogger::clear() {
  logs_.clear();
}

}  // namespace mocks
}  // namespace tests
```

#### Step 3: Use Mock in Tests

```cpp
#include "tests/mocks/MockLogger.hpp"
#include "infrastructure/config/ConfigParser.hpp"
#include <gtest/gtest.h>

class ConfigParserTest : public ::testing::Test {
 protected:
  tests::mocks::MockLogger mockLogger;
  
  virtual void SetUp() {
    mockLogger.clear();
  }
};

TEST_F(ConfigParserTest, ParseValidConfig) {
  ConfigParser parser(mockLogger);
  
  EXPECT_TRUE(parser.parse("config.conf"));
  
  // Verify logger was called
  EXPECT_TRUE(mockLogger.hasLog(INFO, "Configuration loaded"));
}

TEST_F(ConfigParserTest, ParseInvalidConfig) {
  ConfigParser parser(mockLogger);
  
  EXPECT_FALSE(parser.parse("invalid.conf"));
  
  // Verify error was logged
  EXPECT_TRUE(mockLogger.hasLog(ERROR, "Failed to parse configuration"));
}
```

---

## Test Organization

### Arrange-Act-Assert Pattern

Always organize tests in three sections:

```cpp
TEST_F(MyTest, ExampleTest) {
  // ARRANGE: Set up test data and conditions
  MyClass obj;
  std::string input = "test";
  
  // ACT: Execute the code under test
  std::string result = obj.process(input);
  
  // ASSERT: Verify the results
  EXPECT_EQ("TEST", result);
}
```

### One Assertion Per Concept

❌ **Don't do this:**
```cpp
TEST_F(HttpMethodTest, Everything) {
  EXPECT_NO_THROW(HttpMethod("GET"));
  EXPECT_THROW(HttpMethod("INVALID"), HttpMethodException);
  EXPECT_TRUE(HttpMethod("GET") == HttpMethod("get"));
  // Testing too many things!
}
```

✅ **Do this instead:**
```cpp
TEST_F(HttpMethodTest, ValidMethods) {
  EXPECT_NO_THROW(HttpMethod("GET"));
  EXPECT_NO_THROW(HttpMethod("POST"));
}

TEST_F(HttpMethodTest, InvalidMethod) {
  EXPECT_THROW(HttpMethod("INVALID"), HttpMethodException);
}

TEST_F(HttpMethodTest, CaseInsensitive) {
  EXPECT_TRUE(HttpMethod("GET") == HttpMethod("get"));
}
```

### Test Independence

Each test should be independent:

❌ **Bad:**
```cpp
Port* globalPort = NULL;

TEST_F(PortTest, Test1) {
  globalPort = new Port(8080);  // ❌ Side effect
}

TEST_F(PortTest, Test2) {
  EXPECT_EQ(8080, globalPort->getValue());  // ❌ Depends on Test1
}
```

✅ **Good:**
```cpp
TEST_F(PortTest, Test1) {
  Port port(8080);
  EXPECT_EQ(8080, port.getValue());
  // port destroyed automatically
}

TEST_F(PortTest, Test2) {
  Port port(443);
  EXPECT_EQ(443, port.getValue());
  // Completely independent
}
```

---

## Best Practices

### 1. Test Edge Cases

Don't just test the happy path:

```cpp
TEST_F(PortTest, EdgeCases) {
  // Minimum valid value
  EXPECT_NO_THROW(Port(1));
  
  // Maximum valid value
  EXPECT_NO_THROW(Port(65535));
  
  // Just below minimum
  EXPECT_THROW(Port(0), PortException);
  
  // Just above maximum
  EXPECT_THROW(Port(65536), PortException);
}
```

### 2. Test Error Conditions

```cpp
TEST_F(ConfigParserTest, HandleMissingFile) {
  ConfigParser parser;
  
  EXPECT_THROW(parser.parse("nonexistent.conf"), FileNotFoundException);
}

TEST_F(ConfigParserTest, HandleCorruptedFile) {
  ConfigParser parser;
  
  EXPECT_THROW(parser.parse("corrupted.conf"), ParseException);
}
```

### 3. Use Descriptive Failure Messages

```cpp
// Default message
EXPECT_EQ(expected, actual);

// Custom message
EXPECT_EQ(expected, actual) << "Failed to parse HTTP method: " << input;
```

### 4. Keep Tests Simple

If a test is complex, break it down:

```cpp
// Helper method for complex setup
Port createTestPort() {
  // Complex initialization
  return Port(8080);
}

TEST_F(PortTest, SimpleTest) {
  Port port = createTestPort();
  EXPECT_EQ(8080, port.getValue());
}
```

### 5. Test Behavior, Not Implementation

❌ **Bad (testing implementation details):**
```cpp
TEST_F(ParserTest, CheckInternalState) {
  Parser p;
  p.parse("input");
  EXPECT_EQ(5, p.tokenCount_);  // ❌ Depends on internal implementation
}
```

✅ **Good (testing observable behavior):**
```cpp
TEST_F(ParserTest, ParsesInputCorrectly) {
  Parser p;
  Result result = p.parse("input");
  EXPECT_TRUE(result.isValid());  // ✅ Tests public interface
}
```

---

## Common Assertions

### Equality

```cpp
EXPECT_EQ(expected, actual);   // Equal (works with ==)
EXPECT_NE(val1, val2);         // Not equal
```

### Boolean

```cpp
EXPECT_TRUE(condition);        // Condition is true
EXPECT_FALSE(condition);       // Condition is false
```

### Comparisons

```cpp
EXPECT_LT(val1, val2);         // Less than (<)
EXPECT_LE(val1, val2);         // Less or equal (<=)
EXPECT_GT(val1, val2);         // Greater than (>)
EXPECT_GE(val1, val2);         // Greater or equal (>=)
```

### Strings

```cpp
EXPECT_STREQ(str1, str2);      // C strings equal
EXPECT_STRNE(str1, str2);      // C strings not equal
EXPECT_STRCASEEQ(str1, str2);  // Case-insensitive equal
```

### Floating Point

```cpp
EXPECT_FLOAT_EQ(expected, actual);   // Float equality
EXPECT_DOUBLE_EQ(expected, actual);  // Double equality
EXPECT_NEAR(val1, val2, abs_error);  // Within tolerance
```

### Exceptions

```cpp
EXPECT_THROW(statement, exception_type);     // Throws specific exception
EXPECT_ANY_THROW(statement);                 // Throws any exception
EXPECT_NO_THROW(statement);                  // Doesn't throw
```

### Pointers

```cpp
EXPECT_EQ(nullptr, ptr);       // Pointer is null
EXPECT_NE(nullptr, ptr);       // Pointer is not null
```

### EXPECT vs ASSERT

```cpp
EXPECT_*  // Test continues if fails (non-fatal)
ASSERT_*  // Test stops if fails (fatal)
```

Use `EXPECT_*` by default. Use `ASSERT_*` when continuing makes no sense:

```cpp
TEST_F(FileTest, ReadFile) {
  File* file = openFile("test.txt");
  ASSERT_NE(nullptr, file);  // Stop if file is null
  
  // These only run if file isn't null
  EXPECT_GT(file->size(), 0);
  EXPECT_TRUE(file->isReadable());
}
```

---

## Testing Patterns

### Testing Constructors

```cpp
TEST_F(MyClassTest, DefaultConstructor) {
  MyClass obj;
  EXPECT_TRUE(obj.isInitialized());
}

TEST_F(MyClassTest, ParameterizedConstructor) {
  MyClass obj("param");
  EXPECT_EQ("param", obj.getValue());
}

TEST_F(MyClassTest, CopyConstructor) {
  MyClass original("value");
  MyClass copy(original);
  
  EXPECT_EQ(original.getValue(), copy.getValue());
}
```

### Testing Operators

```cpp
TEST_F(MyClassTest, EqualityOperator) {
  MyClass obj1("value");
  MyClass obj2("value");
  MyClass obj3("other");
  
  EXPECT_TRUE(obj1 == obj2);
  EXPECT_FALSE(obj1 == obj3);
}

TEST_F(MyClassTest, LessThanOperator) {
  MyClass small("a");
  MyClass large("z");
  
  EXPECT_TRUE(small < large);
  EXPECT_FALSE(large < small);
}
```

### Testing Getters/Setters

```cpp
TEST_F(MyClassTest, GettersAndSetters) {
  MyClass obj;
  
  obj.setValue("test");
  EXPECT_EQ("test", obj.getValue());
  
  obj.setValue("changed");
  EXPECT_EQ("changed", obj.getValue());
}
```

### Testing Validation

```cpp
TEST_F(MyClassTest, ValidatesInput) {
  // Valid inputs
  EXPECT_TRUE(MyClass::isValid("valid"));
  EXPECT_TRUE(MyClass::isValid("also-valid"));
  
  // Invalid inputs
  EXPECT_FALSE(MyClass::isValid(""));
  EXPECT_FALSE(MyClass::isValid("invalid@"));
}
```

### Testing State Changes

```cpp
TEST_F(ServerTest, StateTransitions) {
  Server server;
  
  EXPECT_EQ(Server::STOPPED, server.getState());
  
  server.start();
  EXPECT_EQ(Server::RUNNING, server.getState());
  
  server.stop();
  EXPECT_EQ(Server::STOPPED, server.getState());
}
```

---

## Adding New Tests

### Step-by-Step Process

1. **Create test file** in appropriate directory
   ```bash
   touch tests/unit/test_MyClass.cpp
   ```

2. **Write test skeleton**
   ```cpp
   #include "domain/MyClass.hpp"
   #include <gtest/gtest.h>
   
   class MyClassTest : public ::testing::Test {
    protected:
     virtual void SetUp() {}
     virtual void TearDown() {}
   };
   
   TEST_F(MyClassTest, FirstTest) {
     // TODO: Implement
     EXPECT_TRUE(true);
   }
   ```

3. **Compile and verify**
   ```bash
   cd tests/
   make
   ```

4. **Run test**
   ```bash
   ./bin/test_runner --gtest_filter='MyClassTest.*'
   ```

5. **Implement actual test logic**

6. **Verify test passes**

---

## C++98 Considerations

Remember this project uses C++98, so:

❌ **Don't use C++11+ features:**
```cpp
auto value = obj.getValue();           // ❌ C++11
std::vector<int> v = {1, 2, 3};       // ❌ C++11
for (auto& item : container) {}        // ❌ C++11
nullptr                                // ❌ C++11
```

✅ **Use C++98 alternatives:**
```cpp
int value = obj.getValue();            // ✅ C++98
std::vector<int> v;                    // ✅ C++98
v.push_back(1);
for (size_t i = 0; i < v.size(); ++i) {} // ✅ C++98
NULL                                   // ✅ C++98
```

---

## Resources

- **Google Test 1.8.1 Documentation:** https://github.com/google/googletest/tree/release-1.8.1/googletest/docs
- **Google Test Primer:** https://github.com/google/googletest/blob/release-1.8.1/googletest/docs/primer.md
- **Advanced Guide:** https://github.com/google/googletest/blob/release-1.8.1/googletest/docs/advanced.md

---

## Next Steps

1. ✅ Read [BEGINNER_GUIDE.md](BEGINNER_GUIDE.md) for testing fundamentals
2. ✅ Look at existing tests in `unit/` directory for examples
3. ✅ Start with simple value object tests
4. ✅ Gradually add more complex tests
5. ✅ Run tests frequently: `make && ./bin/test_runner`

**Last Updated:** December 26, 2025
