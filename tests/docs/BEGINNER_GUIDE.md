# Beginner's Guide to Testing with Google Test

Welcome! This guide explains how testing works in this project in simple terms.

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
