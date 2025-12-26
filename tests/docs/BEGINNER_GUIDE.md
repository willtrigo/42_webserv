# Quick Start Guide - Building Tests for 42_webserver

This guide shows you **how to build tests faster** for the webserver project, following the implementation strategy.

**Goal:** Get from "I need to test this component" to "tests are passing" as quickly as possible.

---

## 🚀 Quick Start Workflow

### 1. Check What to Test Next

```bash
# See implementation priority
cat tests/docs/TESTING_STRATEGY.md

# Check current status
cat tests/docs/TESTING_STATUS.md
```

**Priority order** (from TESTING_STRATEGY.md):
1. 🔴 **ErrorCode** (blocks 10+ components) → Start here!
2. 🔴 **Path** (enables file operations)
3. 🔴 **Port** (update existing tests)
4. 🟡 **ConfigParser** (tests ready, need implementation)
5. 🟡 **FileHandler** (fix architecture blocker)

### 2. Copy Test Template

**For Value Objects** (ErrorCode, Path, Port, Uri, Size):
```bash
# Create test file
touch tests/unit/test_ErrorCode.cpp

# Copy template
cat > tests/unit/test_ErrorCode.cpp << 'EOF'
#include "domain/value_objects/ErrorCode.hpp"
#include "shared/exceptions/InvalidStatusCodeException.hpp"
#include <gtest/gtest.h>

class ErrorCodeTest : public ::testing::Test {
 protected:
  virtual void SetUp() {}
  virtual void TearDown() {}
};

TEST_F(ErrorCodeTest, ValidStatusCodes) {
  EXPECT_NO_THROW(ErrorCode(200));
  EXPECT_NO_THROW(ErrorCode(404));
  EXPECT_NO_THROW(ErrorCode(500));
}

TEST_F(ErrorCodeTest, InvalidStatusCodes) {
  EXPECT_THROW(ErrorCode(0), InvalidStatusCodeException);
  EXPECT_THROW(ErrorCode(999), InvalidStatusCodeException);
}
EOF
```

**For Integration Tests** (FileHandler, ConfigParser):
```bash
# Create integration test
touch tests/integration/test_FileHandler_Integration.cpp

# Copy template with setup/teardown
cat > tests/integration/test_FileHandler_Integration.cpp << 'EOF'
#include "infrastructure/filesystem/FileHandler.hpp"
#include <gtest/gtest.h>

class FileHandlerIntegrationTest : public ::testing::Test {
 protected:
  std::string testDir;
  
  virtual void SetUp() {
    testDir = "/tmp/webserver_test/";
    system(("mkdir -p " + testDir).c_str());
  }
  
  virtual void TearDown() {
    system(("rm -rf " + testDir).c_str());
  }
};

TEST_F(FileHandlerIntegrationTest, CreateAndReadFile) {
  // Test implementation here
}
EOF
```

### 3. Get Test Scenarios

```bash
# Open test scenarios for specific component
# Example: ErrorCode scenarios
grep -A 30 "## ErrorCode" tests/docs/TESTING_SCENARIOS.md
```

Or open [TESTING_SCENARIOS.md](TESTING_SCENARIOS.md) and navigate to your component.

### 4. Build and Run

```bash
cd tests/
make                    # Compile
./bin/test_runner       # Run all tests

# Run only your new tests
./bin/test_runner --gtest_filter='ErrorCodeTest.*'
```

### 5. Fix Failures

```bash
# Test output shows:
# test_ErrorCode.cpp:28: Failure
# Expected equality of these values:
#   200
#   code.getValue()
#     Which is: 0

# → Fix implementation or test expectation
```

---

## 📋 Component-Specific Quick Starts

### 🔴 ErrorCode (Start Here - Highest Priority!)

**Why first?** Blocks Route, RouteMatcher, RouteConfigAdapter, error pages (10+ components)

**Quick setup:**
```bash
# 1. Create test file
touch tests/unit/test_ErrorCode.cpp

# 2. Add to Makefile (tests/Makefile)
# Add to TEST_SRCS: unit/test_ErrorCode.cpp

# 3. Copy scenarios from TESTING_SCENARIOS.md
# Section: "Phase 1: Domain Layer - Value Objects → ErrorCode"

# 4. Run tests (they'll fail - implementation not ready)
make && ./bin/test_runner --gtest_filter='ErrorCodeTest.*'
```

**What to test** (from TESTING_SCENARIOS.md):
- Valid HTTP status codes (200, 301, 404, 500)
- fromString() parsing ("404" → 404)
- isRedirection() for 3xx codes
- movedPermanently() returns 301
- Default error page mapping
- Invalid status codes (0, 999)
- Status code categories (success, redirect, client error, server error)

**Time to complete:** 1-2 days (implementation + tests)

---

### 🔴 Path (Second Priority)

**Why important?** Enables FileHandler, static files, uploads, directory listing

**Quick setup:**
```bash
# 1. Tests already exist but disabled
mv tests/unit/test_Path.cpp.disabled tests/unit/test_Path.cpp

# 2. Update Makefile
# Add: unit/test_Path.cpp to TEST_SRCS

# 3. Run tests
make && ./bin/test_runner --gtest_filter='PathTest.*'
```

**What to test** (from TESTING_SCENARIOS.md):
- Valid absolute paths
- Reject relative paths
- Path normalization (/foo/../bar → /bar)
- Security validation (reject path traversal)
- Extract directory, filename, extension
- Path joining

**Time to complete:** 2-3 days

---

### ⚠️ Port (Quick Fix - Update Tests)

**Why now?** Tests exist but expectations don't match implementation

**Quick setup:**
```bash
# Tests already running, just failing
./bin/test_runner --gtest_filter='PortTest.*'

# Shows: 1/10 passing
```

**What to fix:**
- Decide: Should default port be 0 or 80?
- Update tests to match implementation OR
- Update implementation to match test expectations

**Time to complete:** 1 day

---

### 🟡 ConfigParser (Tests Ready!)

**Why later?** Depends on Port, Path, ErrorCode being done first

**Quick setup:**
```bash
# 1. Tests already exist but disabled
mv tests/unit/test_ConfigParser.cpp.disabled tests/unit/test_ConfigParser.cpp

# 2. Update Makefile

# 3. Implement ConfigParser class

# 4. Run tests
make && ./bin/test_runner --gtest_filter='ConfigParserTest.*'
```

**What to test** (from TESTING_SCENARIOS.md):
- Parse single/multiple server blocks
- Parse location blocks with routes
- Handle syntax errors gracefully
- Load default error pages
- Parse CGI configuration
- Validate max body size

**Time to complete:** 3-4 days

---

## 🎯 Fast Test Development Tips

### Tip 1: Start with Simplest Test

Don't write all tests at once. Start with one:

```cpp
// ✅ Start here
TEST_F(ErrorCodeTest, ValidStatusCode) {
  EXPECT_NO_THROW(ErrorCode(200));
}

// Add after first test passes
TEST_F(ErrorCodeTest, InvalidStatusCode) {
  EXPECT_THROW(ErrorCode(0), InvalidStatusCodeException);
}
```

### Tip 2: Use Test Scenarios as Checklist

Open TESTING_SCENARIOS.md and copy test names directly:

```cpp
// From TESTING_SCENARIOS.md:
// TEST: Valid HTTP status codes (200, 301, 404, 500)
TEST_F(ErrorCodeTest, ValidHttpStatusCodes) {
  EXPECT_NO_THROW(ErrorCode(200));
  EXPECT_NO_THROW(ErrorCode(301));
  EXPECT_NO_THROW(ErrorCode(404));
  EXPECT_NO_THROW(ErrorCode(500));
}
```

### Tip 3: Run Tests Frequently

```bash
# Don't wait to write all tests
# Run after each test you add:
make && ./bin/test_runner --gtest_filter='ErrorCodeTest.ValidHttpStatusCodes'
```

### Tip 4: Follow the Dependency Chain

From TESTING_STRATEGY.md:

```
HttpMethod ✅ → (nothing blocked)
ErrorCode 🚧 → Route → RouteMatcher → HTTP routing
Path 🚧 → FileHandler → Static files, uploads
Port ⚠️ → ConfigParser → Multi-server configuration
```

**Implement in order:** ErrorCode → Path → Port → ConfigParser

### Tip 5: Use Existing Tests as Examples

```bash
# See working test example
cat tests/unit/test_HttpMethod.cpp

# Copy structure for your component
```

---

## 🔧 Common Workflows

### Workflow: Adding a New Value Object Test

1. **Check if blocked** - See TESTING_STRATEGY.md dependencies
2. **Create test file** - `tests/unit/test_ComponentName.cpp`
3. **Copy scenarios** - From TESTING_SCENARIOS.md
4. **Write simplest test first** - One EXPECT line
5. **Build** - `make`
6. **Run** - `./bin/test_runner --gtest_filter='ComponentTest.*'`
7. **Fix** - Update implementation or test
8. **Repeat** - Add next test

### Workflow: Enabling Disabled Tests

```bash
# 1. Find disabled test
ls tests/unit/*.disabled
# Output: test_Path.cpp.disabled

# 2. Check why disabled
grep "disabled\|blocker" tests/docs/TESTING_STATUS.md

# 3. If dependencies ready, enable
mv tests/unit/test_Path.cpp.disabled tests/unit/test_Path.cpp

# 4. Update Makefile
# Add to TEST_SRCS

# 5. Run
make && ./bin/test_runner --gtest_filter='PathTest.*'
```

### Workflow: Debugging Test Failures

```bash
# 1. Run failing test
./bin/test_runner --gtest_filter='PortTest.DefaultConstructor'

# 2. Read output
# test_Port.cpp:28: Failure
# Expected: 80
# Actual: 0

# 3. Check implementation
cat src/domain/value_objects/Port.cpp

# 4. Fix (implementation or test expectation)

# 5. Re-run
make && ./bin/test_runner --gtest_filter='PortTest.DefaultConstructor'
```

---

## 📚 Reference Documentation

**When you need:**

| Need | See Document |
|------|--------------|
| What to implement next? | [TESTING_STRATEGY.md](TESTING_STRATEGY.md) |
| Test scenarios for ErrorCode? | [TESTING_SCENARIOS.md](TESTING_SCENARIOS.md#phase-1) |
| Google Test syntax? | [GOOGLETEST_GUIDE.md](GOOGLETEST_GUIDE.md) |
| Command-line options? | [GOOGLETEST_REFERENCE.md](GOOGLETEST_REFERENCE.md) |
| Which features work? | [TESTING_STATUS.md](TESTING_STATUS.md) |
| Installation issues? | [GOOGLETEST_QUICKFIX.md](GOOGLETEST_QUICKFIX.md) |

---

## 🚨 Common Mistakes to Avoid

### ❌ Testing Blocked Components

```bash
# Don't start with Route if ErrorCode isn't done!
# Check dependencies in TESTING_STRATEGY.md first
```

### ❌ Writing All Tests Before Running Any

```bash
# ❌ Bad: Write 50 tests, then compile
# ✅ Good: Write 1 test, run it, then next
```

### ❌ Ignoring Test Failures

```bash
# ❌ Bad: "1 test failing, I'll add more tests"
# ✅ Good: "Fix the 1 failing test before adding more"
```

### ❌ Not Following Naming Conventions

```cpp
// ❌ Bad
TEST_F(ErrorCodeTest, Test1) { }

// ✅ Good (matches TESTING_SCENARIOS.md)
TEST_F(ErrorCodeTest, ValidHttpStatusCodes) { }
```

---

## 🎯 Success Metrics

You're doing it right if:

[ ] Tests compile on first try (check dependencies first)  
[ ] Each test covers one specific behavior  
[ ] Test names match TESTING_SCENARIOS.md  
[ ] You run tests after every change  
[ ] All tests pass before moving to next component  

---

## 🚀 Next Steps

**Start building tests now:**

```bash
# 1. Check priority
grep "Phase 1" tests/docs/TESTING_STRATEGY.md

# 2. Start with ErrorCode
touch tests/unit/test_ErrorCode.cpp

# 3. Copy scenarios
grep -A 50 "ErrorCode" tests/docs/TESTING_SCENARIOS.md

# 4. Write first test
# (Copy from TESTING_SCENARIOS.md)

# 5. Build and run
cd tests && make && ./bin/test_runner
```

**Follow the chain:** ErrorCode → Path → Port → ConfigParser → FileHandler → CGI

**Tests guide implementation!** Write failing tests, then implement until they pass.

---

**Last Updated:** December 26, 2025  
**For:** 42_webserver Test-Driven Development

## What is Testing?

Testing means writing code that checks if your other code works correctly. It's like having a robot that tests your homework before you submit it!

## Why Test?

1. **Catch bugs early** - Find problems before users do
2. **Confidence** - Know your code works when you change things
3. **Documentation** - Tests show how your code should be used
4. **Better design** - Code that's easy to test is usually better designed

---

## Google Test Basics

### Key Concepts

#### 1. Test Fixture (Test Class)
A test fixture is a class that sets up everything you need for testing.

```cpp
class MyTest : public ::testing::Test {
 protected:
  // SetUp runs BEFORE each test
  virtual void SetUp() {
    // Create objects, open files, etc.
  }
  
  // TearDown runs AFTER each test
  virtual void TearDown() {
    // Clean up, close files, etc.
  }
};
```

**Why use fixtures?**
- Each test starts with a clean state
- No test affects another test
- Reuse setup code for multiple tests

#### 2. Writing a Test

```cpp
TEST_F(MyTest, TestName) {
  // Arrange: Set up what you need
  int x = 5;
  int y = 3;
  
  // Act: Do the thing you're testing
  int result = x + y;
  
  // Assert: Check if it worked
  EXPECT_EQ(8, result);
}
```

This pattern is called **Arrange-Act-Assert** or **AAA**.

#### 3. Assertions (Checks)

Assertions check if something is true. If it fails, the test fails.

| Assertion | What it checks | Example |
|-----------|---------------|---------|
| `EXPECT_TRUE(x)` | x is true | `EXPECT_TRUE(file.exists())` |
| `EXPECT_FALSE(x)` | x is false | `EXPECT_FALSE(error)` |
| `EXPECT_EQ(a, b)` | a equals b | `EXPECT_EQ(5, result)` |
| `EXPECT_NE(a, b)` | a NOT equals b | `EXPECT_NE(0, size)` |
| `EXPECT_LT(a, b)` | a less than b | `EXPECT_LT(0, count)` |
| `EXPECT_GT(a, b)` | a greater than b | `EXPECT_GT(100, age)` |
| `EXPECT_THROW(code, exception)` | code throws exception | `EXPECT_THROW(divide(1,0), Error)` |
| `EXPECT_NO_THROW(code)` | code doesn't throw | `EXPECT_NO_THROW(readFile())` |

**EXPECT vs ASSERT:**
- `EXPECT_*` - Test continues even if it fails
- `ASSERT_*` - Test stops immediately if it fails

Use `EXPECT` most of the time. Use `ASSERT` only when continuing makes no sense.

---

## Understanding Our Tests

### Example 1: Simple Value Object Test

Let's look at [test_HttpMethod.cpp](../unit/test_HttpMethod.cpp):

```cpp
TEST_F(HttpMethodTest, ValidMethods) {
  EXPECT_NO_THROW(HttpMethod("GET"));
  EXPECT_NO_THROW(HttpMethod("POST"));
}
```

**What's happening?**
1. We create an `HttpMethod` object with "GET"
2. `EXPECT_NO_THROW` means "this should work without errors"
3. If it throws an exception, the test fails

**Why test this?**
- Makes sure only valid HTTP methods are accepted
- Prevents bugs like accepting "BANANA" as an HTTP method

### Example 2: File Operations Test

From [test_FileHandler_Integration.cpp](integration/test_FileHandler_Integration.cpp):

```cpp
TEST_F(FileHandlerIntegrationTest, CreateAndReadFile) {
  // Step 1: Create a file path
  Path testFile = getTestFilePath("test.txt");
  
  // Step 2: Write some content
  std::string content = "Hello, World!";
  bool success = fileHandler->writeFile(testFile, content);
  EXPECT_TRUE(success);
  
  // Step 3: Read it back
  std::vector<char> readContent = fileHandler->readFile(testFile);
  
  // Step 4: Convert to string
  std::string readStr(readContent.begin(), readContent.end());
  
  // Step 5: Check they match
  EXPECT_EQ(content, readStr);
}
```

**What's being tested?**
- Can we write a file?
- Can we read it back?
- Is the content the same?

This is called an **integration test** because it tests multiple components working together.

### Example 3: Mock Objects

From [MockLogger.hpp](mocks/MockLogger.hpp):

A mock is a **fake object** that pretends to be real but gives us control for testing.

```cpp
// Real logger writes to console/file
// Mock logger saves messages in memory so we can check them

MockLogger* logger = new MockLogger();
logger->info("Test message");

// Later, we can check:
EXPECT_EQ(1, logger->getLogCount());
EXPECT_TRUE(logger->hasLog(INFO, "Test message"));
```

**Why use mocks?**
- Test without side effects (no real files/network/database)
- Check if functions were called correctly
- Control what dependencies return

---

## Common Testing Patterns

### Pattern 1: Testing Constructors

```cpp
TEST_F(MyTest, ConstructorWorks) {
  // Create the object - if constructor is broken, this throws
  EXPECT_NO_THROW(MyClass obj(param1, param2));
}
```

### Pattern 2: Testing Error Handling

```cpp
TEST_F(MyTest, HandlesInvalidInput) {
  MyClass obj;
  
  // This SHOULD throw an exception because input is invalid
  EXPECT_THROW(obj.process(""), InvalidInputException);
}
```

### Pattern 3: Testing File Operations

```cpp
TEST_F(FileTest, WriteThenRead) {
  // Write
  fileHandler->write(path, data);
  
  // Read
  std::string result = fileHandler->read(path);
  
  // Compare
  EXPECT_EQ(data, result);
}
```

### Pattern 4: Testing with Multiple Cases

```cpp
TEST_F(MyTest, MultipleInputs) {
  EXPECT_EQ(0, factorial(0));
  EXPECT_EQ(1, factorial(1));
  EXPECT_EQ(2, factorial(2));
  EXPECT_EQ(6, factorial(3));
}
```

---

## Project Structure

```
tests/
├── unit/                    # Test single classes in isolation
│   ├── test_HttpMethod.cpp  # Tests HttpMethod class
│   ├── test_Path.cpp        # Tests Path class
│   └── test_Port.cpp        # Tests Port class
│
├── integration/             # Test multiple classes working together
│   └── test_FileHandler_Integration.cpp
│
└── mocks/                   # Fake objects for testing
    └── MockLogger.hpp       # Fake logger
```

**Unit Tests:**
- Test one class/function
- Fast and focused
- Use mocks for dependencies

**Integration Tests:**
- Test multiple components together
- Slower but more realistic
- Test how things work as a system

---

## Running Tests

### Build and Run All Tests
```bash
cd tests
make
./test_runner
```

### Run Specific Test
```bash
./test_runner --gtest_filter=HttpMethodTest.*
```

### Run One Specific Test
```bash
./test_runner --gtest_filter=HttpMethodTest.ValidMethods
```

### See Verbose Output
```bash
./test_runner --gtest_verbose
```

---

## Writing Your First Test

Let's say you have a class `Calculator` with an `add` function:

```cpp
// 1. Create test file: test_Calculator.cpp

#include "Calculator.hpp"
#include <gtest/gtest.h>

// 2. Create test fixture (optional for simple tests)
class CalculatorTest : public ::testing::Test {
 protected:
  Calculator calc;  // Calculator object for all tests
};

// 3. Write your test
TEST_F(CalculatorTest, AddTwoPositiveNumbers) {
  // Arrange: prepare inputs
  int a = 5;
  int b = 3;
  
  // Act: do the operation
  int result = calc.add(a, b);
  
  // Assert: check result
  EXPECT_EQ(8, result);
}

// 4. Test edge cases
TEST_F(CalculatorTest, AddNegativeNumbers) {
  int result = calc.add(-5, -3);
  EXPECT_EQ(-8, result);
}

TEST_F(CalculatorTest, AddZero) {
  int result = calc.add(5, 0);
  EXPECT_EQ(5, result);
}
```

---

## Tips for Good Tests

### 1. One Test, One Thing
Bad:
```cpp
TEST_F(MyTest, TestEverything) {
  EXPECT_TRUE(obj.method1());
  EXPECT_TRUE(obj.method2());
  EXPECT_TRUE(obj.method3());
}
```

Good:
```cpp
TEST_F(MyTest, Method1Works) {
  EXPECT_TRUE(obj.method1());
}

TEST_F(MyTest, Method2Works) {
  EXPECT_TRUE(obj.method2());
}
```

### 2. Clear Test Names
Use names that explain what you're testing:
- `CreateAndReadFile` ✅
- `Test1` ❌

### 3. Test Edge Cases
Don't just test the happy path:
- Empty strings
- Null pointers
- Maximum values
- Invalid input

### 4. Clean Up After Tests
Always clean up in `TearDown`:
```cpp
virtual void TearDown() {
  delete myObject;
  removeTestFiles();
}
```

---

## Common Mistakes to Avoid

### ❌ Don't test implementation details
```cpp
// Bad: testing private implementation
TEST(Bad, InternalCounter) {
  // This breaks when you refactor
  EXPECT_EQ(5, obj.internalCounter);
}
```

```cpp
// Good: testing public behavior
TEST(Good, ProcessFiveTimes) {
  obj.process();
  obj.process();
  // ... 5 times
  EXPECT_EQ(5, obj.getProcessCount());
}
```

### ❌ Don't make tests depend on each other
```cpp
// Bad: Test2 depends on Test1 running first
TEST(Bad, Test1) { globalVar = 5; }
TEST(Bad, Test2) { EXPECT_EQ(5, globalVar); } // Fails if Test1 didn't run
```

```cpp
// Good: Each test is independent
TEST(Good, Test1) {
  int var = 5;
  EXPECT_EQ(5, var);
}
TEST(Good, Test2) {
  int var = 5;
  EXPECT_EQ(5, var);
}
```

### ❌ Don't ignore failing tests
If a test fails, fix it! Don't just comment it out.

---

## C++ Best Practice: Header vs Implementation Files

### Why Separate .hpp and .cpp?

In C++, it's good practice to **declare** functions in header files (.hpp) and **implement** them in source files (.cpp).

#### Header File (.hpp) - The Interface
```cpp
// MyClass.hpp
class MyClass {
 public:
  void doSomething();      // Declaration only
  int getValue() const;    // Declaration only
  
 private:
  int m_value;
};
```

#### Source File (.cpp) - The Implementation
```cpp
// MyClass.cpp
#include "MyClass.hpp"

void MyClass::doSomething() {
  // Implementation here
  m_value = 42;
}

int MyClass::getValue() const {
  // Implementation here
  return m_value;
}
```

### Benefits

1. **Faster Compilation** - Changing implementation doesn't require recompiling everything
2. **Cleaner Code** - Headers show the interface clearly without implementation details
3. **Smaller Binary** - Avoids code duplication
4. **42 School Standard** - Follows C++98 best practices

### When to Keep Functions in Headers?

Only in these cases:
- **Templates** - Templates must be in headers
- **Inline functions** - Very small functions (1-2 lines)
- **Constructors with initialization lists** - Simple initialization

### Example from Our Tests

**Before (everything in header):**
```cpp
// MockLogger.hpp
class MockLogger {
 public:
  void clear() { m_logs.clear(); }  // ❌ Implementation in header
  std::size_t getLogCount() const { return m_logs.size(); }  // ❌
};
```

**After (proper separation):**
```cpp
// MockLogger.hpp
class MockLogger {
 public:
  void clear();                     // ✅ Declaration only
  std::size_t getLogCount() const;  // ✅ Declaration only
};

// MockLogger.cpp
void MockLogger::clear() {          // ✅ Implementation in .cpp
  m_logs.clear();
}

std::size_t MockLogger::getLogCount() const {  // ✅
  return m_logs.size();
}
```

---

## Need Help?


- **Google Test Docs:** https://google.github.io/googletest/
- **Our Examples:** Look at existing tests in `unit/` and `integration/`
- **Ask Questions:** Testing is a skill - it gets easier with practice!

## Next Steps

1. **Read existing tests** - Start with `test_HttpMethod.cpp` (simplest)
2. **Run the tests** - See them pass and fail
3. **Modify a test** - Change an EXPECT and see what happens
4. **Write a simple test** - Pick a small class and test it
5. **Ask for code review** - Get feedback on your tests

Remember: **Good tests make good code!** 🚀
