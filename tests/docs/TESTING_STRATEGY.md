# Testing Strategy for 42_webserver

## Executive Summary

This document outlines the complete testing strategy for the 42_webserver project using **Google Test 1.8.1** (C++98 compatible).

**Current Status:** Google Test infrastructure implemented and working. See [FEATURES_STATUS.md](FEATURES_STATUS.md) for detailed feature status.

## 🎯 Decision: Google Test (C++)

### Why Google Test?

| Criteria | Google Test | Pytest |
|----------|-------------|--------|
| **Native Integration** | ✅ Compiles with C++98 | ❌ Requires bindings/wrappers |
| **Performance** | ✅ Native speed | ❌ Cross-language overhead |
| **Debugging** | ✅ Same toolchain (gdb) | ❌ Complex debugging |
| **Mocking** | ✅ Native C++ mocks | ❌ Requires ctypes/cffi |
| **CI/CD** | ✅ Standard C++ workflow | ⚠️ Extra dependencies |
| **42 School Alignment** | ✅ Same language | ❌ Additional language |
| **Memory Testing** | ✅ Valgrind integration | ⚠️ Limited for C++ |

---

## 📊 Architecture Analysis

Your project follows **Clean Architecture** (Hexagonal Architecture):

```
┌─────────────────────────────────────────────┐
│          Presentation Layer (CLI)            │
│  ┌──────────────────────────────────────┐  │
│  │  CliController │ CliView              │  │
│  └──────────────────────────────────────┘  │
└─────────────────────────────────────────────┘
                     │
                     ↓
┌─────────────────────────────────────────────┐
│          Application Layer (Ports)           │
│  ┌──────────────────────────────────────┐  │
│  │  ILogger │ IConfigParser │ Interfaces │  │
│  └──────────────────────────────────────┘  │
└─────────────────────────────────────────────┘
                     │
        ┌────────────┴────────────┐
        ↓                         ↓
┌────────────────┐     ┌──────────────────────┐
│  Domain Layer  │     │  Infrastructure Layer │
│                │     │                       │
│  • Entities    │     │  • Adapters           │
│  • Value Objs  │     │  • Config Parser      │
│  • Business    │     │  • FileSystem         │
│    Logic       │     │  • IO / Logging       │
└────────────────┘     └──────────────────────┘
```

### Benefits for Testing:
1. **Dependency Inversion**: All dependencies go through interfaces (Ports)
2. **Mockability**: Easy to create mock implementations
3. **Isolation**: Layers can be tested independently
4. **Clear Boundaries**: Each layer has specific responsibilities

---

## 🧪 Testing Strategy by Layer

### 1. Domain Layer Tests (Pure Unit Tests)

**What to test:**
- Value Objects: Path, Port, HttpMethod, ErrorCode, Size, Uri
- Entities: CgiConfig
- Business logic validation
- Edge cases and boundary conditions

**Characteristics:**
- ✅ No external dependencies
- ✅ Fast execution (< 1ms per test)
- ✅ High coverage target (90%+)
- ✅ Test all validation rules

**Example test scenarios:**
```cpp
// Path value object
TEST: Valid absolute paths
TEST: Invalid relative paths when absolute required
TEST: Path normalization
TEST: Path joining
TEST: Security validation (no path traversal)

// Port value object
TEST: Valid port ranges (1-65535)
TEST: Invalid port 0
TEST: Invalid port > 65535
TEST: Well-known ports (80, 443, 8080)

// HttpMethod value object
TEST: Valid HTTP methods (GET, POST, PUT, DELETE)
TEST: Case insensitivity
TEST: Invalid method rejection
```

### 2. Infrastructure Layer Tests (Unit + Integration)

**What to test:**
- ConfigParser: Configuration parsing logic
- FileHandler: File operations
- Logger: Logging functionality
- Adapters: Interface implementations

**Strategy:**
- **Unit tests**: Use mocks for dependencies
- **Integration tests**: Use real dependencies in isolated environment

**Example test scenarios:**
```cpp
// ConfigParser (Unit)
TEST: Parse valid configuration
TEST: Detect syntax errors
TEST: Handle missing files
TEST: Parse server blocks
TEST: Parse location blocks
TEST: Handle includes

// FileHandler (Integration)
TEST: Create and read files
TEST: Append to files
TEST: Copy files
TEST: Delete files
TEST: Get file metadata
TEST: Handle permissions
```

### 3. Presentation Layer Tests (Unit)

**What to test:**
- CliController: Command-line argument parsing
- CliView: Output formatting
- User interaction logic

**Strategy:**
- Mock all dependencies (Logger, etc.)
- Test input validation
- Verify output messages

---

## 🛠️ Implementation Status

### ✅ Completed (Phase 1 & 2)

1. **Setup Complete**
   - Google Test 1.8.1 installed (C++98 compatible)
   - Test structure created
   - Build system configured
   - Documentation organized

2. **Core Tests Implemented**
   - ✅ HttpMethod value object (5/5 tests passing)
   - ⚠️ Port value object (1/10 tests passing - needs updates)
   - 🚧 Path value object (tests disabled - not implemented)
   - 🚧 ConfigParser (tests disabled - not implemented)

3. **Infrastructure**
   - Mock implementations (MockLogger, MockConfigParser)
   - Test fixtures directory
   - Automated installation script
   - Comprehensive documentation

**See [FEATURES_STATUS.md](FEATURES_STATUS.md) for detailed status of all features.**

### 🚧 In Progress (Phase 2 & 3)

**High Priority:**
- ErrorCode value object (blocks routing features)
- Uri value object
- Route entity (depends on ErrorCode)

**Medium Priority:**
- Path value object implementation
- ConfigParser implementation
- RegexPattern initialization fixes

**Low Priority:**
- Port test updates
- Integration tests
- CgiConfig entity

### 📅 Next Phases

**Phase 3: Complete Core Features (Week 2-3)**
1. Implement ErrorCode class
2. Implement Uri class  
3. Fix RegexPattern initialization
4. Complete Path implementation
5. Enable and fix disabled tests

**Phase 4: Integration Tests (Week 3-4)**
1. FileHandler with real filesystem
2. ConfigParser with test config files
3. Full CLI workflow
4. End-to-end scenarios

**Phase 5: CI/CD Integration (Week 4)**
1. Add to GitHub Actions / GitLab CI
2. Memory leak detection (Valgrind)
3. Code coverage reports
4. Performance benchmarks

---

## 🛠️ Getting Started

### 1. Install Google Test 1.8.1
```bash
cd tests/
sudo ./install_gtest.sh
```

**See [INSTALLATION.md](INSTALLATION.md) for detailed instructions.**

### 2. Build tests
```bash
cd tests/
make
```

### 3. Run tests
```bash
# Run all working tests
./bin/test_runner

# Run specific test suite
./bin/test_runner --gtest_filter='HttpMethodTest.*'

# List available tests
./bin/test_runner --gtest_list_tests
```

**See [RUNNING_TESTS.md](RUNNING_TESTS.md) for all testing options.**

### 4. Write new tests

**See [WRITING_TESTS.md](WRITING_TESTS.md) for patterns and examples.**

For beginners: **See [BEGINNER_GUIDE.md](BEGINNER_GUIDE.md) for testing fundamentals.**

---

## 🛠️ Original Implementation Guide

### Phase 1: Setup (Day 1) ✅ COMPLETE

1. **Install Google Test 1.8.1** ✅
```bash
cd tests/
sudo ./install_gtest.sh
```
**Note:** Must use version 1.8.x for C++98 compatibility. See [INSTALLATION.md](INSTALLATION.md).

2. **Create test structure** ✅
```
tests/
├── unit/              # Unit tests
├── integration/       # Integration tests
├── mocks/            # Mock implementations
├── fixtures/         # Test data
├── docs/             # Documentation
├── test_main.cpp     # Test entry point
├── Makefile          # Test compilation
└── README.md         # Overview
```

### Phase 2: Write Core Tests (Week 1) ⚠️ IN PROGRESS

**Priority order:**
1. ✅ Value Objects (Path, Port, HttpMethod) - **DONE**
2. Domain Entities (CgiConfig)
3. Infrastructure (ConfigParser, FileHandler)
4. Presentation (CLIHttpMethod) - **COMPLETE (5/5 tests)**
5. ⚠️ Value Objects (Port) - **PARTIAL (1/10 tests - needs updates)**
6. 🚧 Value Objects (Path, ErrorCode, Uri) - **NOT IMPLEMENTED**
7. 🚧 Domain Entities (CgiConfig, Route) - **BLOCKED**
8. 🚧 FileHandler with real filesystem - **DISABLED (architecture blocker)**
1. 🚧 ConfigParser with test config files - **NOT IMPLEMENTED**
2. 🚧 Full CLI workflow - **NOT IMPLEMENTED**
3. 🚧 End-to-end scenarios - **NOT IMPLEMENTED**

### Phase 4: CI/CD Integration (Week 3) 🚧 PLANNED
- RegexPattern empty initialization causes crashes (blocks CgiConfig, DirectoryLister)
- FileSystemHelper has private constructor (blocks integration tests)

See [FEATURES_STATUS.md](FEATURES_STATUS.md) for details on each blocker.

### Phase 3: Integration Tests (Week 2) 🚧 PLANNED
3. Full CLI workflow
4. End-to-end scenarios
Reference

**Installation:** See [INSTALLATION.md](INSTALLATION.md)  
**Running Tests:** See [RUNNING_TESTS.md](RUNNING_TESTS.md)  
**Writing Tests:** See [WRITING_TESTS.md](WRITING_TESTS.md)  
**Feature Status:** See [FEATURES_STATUS.md](FEATURES_STATUS.md)  
**Quick Fixes:** See [QUICK_FIX.md](QUICK_FIX.md)  
**Beginner Guide:** See [BEGINNER_GUIDE.md](BEGINNER_GUIDE.md)

---

## 📝 Quick Start Guide (Updated)

### 1. Install Google Test 1.8.1 ✅
```bash
cd tests/
sudo ./install_gtest.sh
```
**See [INSTALLATION.md](INSTALLATION.md) for details.**

### 2. Build tests ✅
```bash
cd tests/
make
```

### 3. Run tests ✅
```bash
# All working tests (currently HttpMethod only)
./bin/test_runner

# Specific test suite
./bin/test_runner --gtest_filter="HttpMethodTest.*"
```
**See [RUNNING_TESTS.md](RUNNING_TESTS.md) for all options.**

### 4. View results ✅
```bash
# Current output:
[==========] Running 5 tests from 1 test suite.
[----------] 5 tests from HttpMethodTest
[ RUN      ] HttpMethodTest.ValidMethods
[       OK ] HttpMethodTest.ValidMethods (0 ms)
[ RUN      ] HttpMethodTest.InvalidMethod
[       OK ] HttpMethodTest.InvalidMethod (0 ms)
[ RUN      ] HttpMethodTest.CaseInsensitive
[       OK ] HttpMethodTest.CaseInsensitive (0 ms)
[ RUN      ] HttpMethodTest.Equality
[       OK ] HttpMethodTest.Equality (0 ms)
[ RUN      ] HttpMethodTest.CopyConstructor
[       OK ] HttpMethodTest.CopyConstructor (0 ms)
[----------] 5 tests from HttpMethodTest (0 ms total)

[==========] 5 tests from 1 test suite ran. (0 ms total)
[  PASSED  ] 5 tests.
```

---

## 📝 Original Quick Start Guideutput example:
[==========] Running 25 tests from 5 test suites.
[----------] 8 tests from PathTest
[ RUN      ] PathTest.ConstructWithAbsolutePath
[       OK ] PathTest.ConstructWithAbsolutePath (0 ms)
...
[==========] 25 tests from 5 test suites ran. (15 ms total)
[  PASSED  ] 25 tests.
```

---

## 🐍 Alternative: Pytest Approach

If you prefer Python for testing (less recommended), here's how:

### Architecture
```
tests/
├── conftest.py           # Pytest configuration
├── test_bindings.py      # C++ binding tests
└── integration/
    └── test_webserver.py # HTTP client tests
```

### Approach 1: ctypes bindings
```python
import ctypes

# Load shared library
lib = ctypes.CDLL('./libwebserv.so')

# Define function signatures
lib.Path_create.argtypes = [ctypes.c_char_p]
lib.Path_create.restype = ctypes.c_void_p

def test_path_creation():
    path = lib.Path_create(b"/home/user")
    assert path is not None
```

### Approach 2: Black-box testing
```python
import subprocess
import requests

def test_webserver_startup():
    proc = subprocess.Popen(['./bin/webserv', 'test.conf'])
    
    # Wait for server to start
    time.sleep(0.5)
    
    #urrent Coverage
- **HttpMethod**: 100% (5/5 tests passing)
- **Port**: ~10% (1/10 tests passing - implementation mismatch)
- **Other components**: 0% (not implemented or disabled)
- **Overall Project**: ~2%

### Coverage Targets (G (Current)
- **Unit tests**: < 0.1 seconds (5 tests)
- **Integration tests**: 0 seconds (all disabled)
- **Full suite**: < 0.1 seconds

### Test Execution Time (Goals)
    response = requests.get('http://localhost:8080/')
    assert response.status_code == 200
    
    proc.kill()
```

### Limitations:
- ❌ Cannot test internal classes directly
- ❌ Requires building shared libraries
- ❌ Complex debugging across languages
- ❌ Additional dependencies
- ❌ Slower execution
- ⚠️ Better for integration/E2E only

---

## 📈 Testing Metrics & Goals

### Coverage Targets
- **Domain Layer**: 90%+ (pure logic, easy to test)
- **Infrastructure Layer**: 75%+ (some OS-dependent code)
- **Presentation Layer**: 70%+ (UI-related code)
- **Overall Project**: 80%+

### Test Execution Time
- **Unit tests**: < 1 second total
- **Integration tests**: < 10 seconds total
- **Full suite**: < 15 seconds

### Quality Metrics
- Zero memory leaks (Valgrind)
- All tests passing in CI
- Code review requires test coverage
- New features require tests

---

## 🔧 Advanced Testing Techniques

### 1. Parameterized Tests
```cpp
class PathTestParameterized : public ::testing::TestWithParam<std::string> {};

TEST_P(PathTestParameterized, ValidPaths) {
  std::string path = GetParam();
  EXPECT_NO_THROW(Path(path));
}

INSTANTIATE_TEST_SUITE_P(
  ValidAbsolutePaths,
  PathTestParameterized,
  ::testing::Values("/", "/home", "/usr/bin", "/var/log/app.log")
);
```

### 2. Death Tests (for validation)
```cpp
TEST(PathTest, PathTooLongCausesExit) {
  std::string longPath(Path::MAX_PATH_LENGTH + 1, 'a');
  EXPECT_DEATH(Path(longPath), "");
}
```

### 3. Fixture with Setup/Teardown
```cpp
class ConfigParserTest : public ::testing::Test {
 protected:
  void SetUp() override {
    // Create test files
    createTestConfig("test.conf", "server { }");
  }
**Immediate priorities (to unblock tests):**

1. **Implement ErrorCode class** (highest priority)
   - Add missing constructor
   - Implement `fromString()`, `movedPermanently()`, `isRedirection()`, `getValue()`
   - This will unblock: Route, RouteMatcher, RouteConfigAdapter

2. **Fix RegexPattern initialization**
   - Handle empty pattern in default constructor
   - This will unblock: CgiConfig, DirectoryLister
Project Documentation
- **[README.md](../README.md)** - Quick start and overview
- **[FEATURES_STATUS.md](FEATURES_STATUS.md)** - What's implemented, what's not
- **[INSTALLATION.md](INSTALLATION.md)** - Google Test 1.8.1 installation
- **[RUNNING_TESTS.md](RUNNING_TESTS.md)** - How to run tests
- **[WRITING_TESTS.md](WRITING_TESTS.md)** - How to write tests
- **[BEGINNER_GUIDE.md](BEGINNER_GUIDE.md)** - Testing fundamentals
- **[QUICK_FIX.md](QUICK_FIX.md)** - Common error fixes

### Google Test (Version 1.8.1 - C++98 Compatible)
3. **Implement Uri::findAuthorityEnd()**
   - Complete Uri class implementation

4. **Update Port tests**
   - Align tests with actual implementation OR
   - Update implementation to match test expectations

**For developers:**

1. **Check feature status** - Read [FEATURES_STATUS.md](FEATURES_STATUS.md) before implementing
### Google Test (Version 1.8.1 - C++98 Compatible)
- [Google Test 1.8.1 Release](https://github.com/google/googletest/tree/release-1.8.1)
- [Official Documentation (1.8.1)](https://github.com/google/googletest/tree/release-1.8.1/googletest/docs)
- [Primer (1.8.1)](https://github.com/google/googletest/blob/release-1.8.1/googletest/docs/primer.md)
- [Advanced Guide (1.8.1)](https://github.com/google/googletest/blob/release-1.8.1/googletest/docs/advanced.md)
- [FAQ (1.8.1)](https://github.com/google/googletest/blob/release-1.8.1/googletest/docs/faq.md)

**⚠️ Important:** This project uses Google Test 1.8.1 (C++98 compatible), not the latest version.

**For beginners:**

1. **Learn testing basics** - Read [BEGINNER_GUIDE.md](BEGINNER_GUIDE.md)
2. **See working examples** - Look at `tests/unit/test_HttpMethod.cpp`
3. **Start simple** - Begin with value object tests
4. **Ask questions** - Check [QUICK_FIX.md](QUICK_FIX.md) for common issues
  
  ConfigParser* parser;
  MockLogger* logger;
};
```

### 4. Test Doubles (Mocks, Stubs, Fakes)
```cpp
// Mock: verify interactions
MockLogger logger;
obj.doSomething();
EXPECT_TRUE(logger.wasInfoCalled());

// Stub: return predefined values
StubConfigProvider config;
config.setPort(8080);

// Fake: simplified working implementation
FakeFileSystem fs;  // In-memory filesystem
```

---

## 🚀 Next Steps
Current state:**

✅ **Google Test 1.8.1 (C++98) installed and working**
- Native C++98 integration
- Direct testing of all components
- Better debugging experience (gdb)
- Aligns with 42 School requirements
- Industry-standard for C++ projects

✅ **Test infrastructure complete:**
- Mock implementations for interfaces
- Example unit tests (HttpMethod fully working)
- Integration test framework (disabled pending implementation)
- Build system configured
- Comprehensive documentation

⚠️ **Current status:**
- **1 component fully tested:** HttpMethod (5/5 tests passing)
- **1 component partially tested:** Port (1/10 tests passing - needs updates)
- **11+ components blocked:** Waiting on ErrorCode, Uri, RegexPattern fixes

🚧 **Next priorities:**
1. Implement ErrorCode (unblocks routing)
2. Fix RegexPattern initialization (unblocks CGI)
3. Implement Uri (completes URL handling)
4. Update Port tests or implementation
5. Enable and verify disabled tests

**Your next action:**
```bash
cd tests/
make
./bin/test_runner  # See 5 passing tests!
```

Then implement ErrorCode to unblock routing tests! 🚀

**Last Updated:** December 26, 2025ts)
- Test error handling thoroughly
- Memory leak detection is critical
- Follow C++98 standard strictly

---

## 🎓 Testing Philosophy

> "Testing shows the presence, not the absence of bugs." — Edsger Dijkstra

**Good tests are:**
- **Fast**: Run in milliseconds
- **Independent**: No shared state between tests
- **Repeatable**: Same result every time
- **Self-validating**: Pass or fail, no manual checks
- **Timely**: Written alongside code, not after

**Remember:**
- Write tests that **fail first**, then make them pass
- Tests are **documentation** of how code should behave
- **Refactor** with confidence when you have good tests
- **Don't test implementation details**, test behavior

---

## 💡 Summary

**For 42_webserver project:**

✅ **Use Google Test (C++)** because:
- Native C++98 integration
- Direct testing of all components
- Better debugging experience
- Aligns with 42 School requirements
- Industry-standard for C++ projects

**Test structure created:** ✅
- Mock implementations for interfaces
- Example unit tests (Path, Port, HttpMethod)
- Example integration tests (FileHandler)
- Makefile for easy compilation
- Comprehensive documentation

**Your next action:**
```bash
cd tests/
make
make test
```

Then expand the test suite for your remaining classes! 🚀
