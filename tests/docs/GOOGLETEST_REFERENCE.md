# Google Test Command-Line Reference

This guide explains **Google Test command-line options** for running tests.

**For test scenarios**, see [TESTING_SCENARIOS.md](TESTING_SCENARIOS.md).  
**For framework usage**, see [GOOGLETEST_GUIDE.md](GOOGLETEST_GUIDE.md).

## Quick Start

```bash
cd tests/
make              # Build tests
./bin/test_runner # Run all tests
```

---

## Building Tests

### Clean Build

Remove all compiled objects and binaries:

```bash
make clean
```

### Build Tests

Compile all test files and link with project sources:

```bash
make
```

**Expected output:**
```
[•] Compiling: ../src/infrastructure/io/StreamWriter.cpp
[•] Compiling: test_main.cpp
[•] Compiling: unit/test_HttpMethod.cpp
[•] Compiling: unit/test_Port.cpp
[•] Compiling: mocks/MockLogger.cpp
[✓] Tests compiled successfully
```

### Build Issues?

See [TESTING_STATUS.md](TESTING_STATUS.md) for list of excluded files and why.

---

## Running All Tests

### Default: Run Everything

```bash
./bin/test_runner
```

**Output example:**
```
[==========] Running 5 tests from 1 test suite.
[----------] Global test environment set-up.
[----------] 5 tests from HttpMethodTest
[ RUN      ] HttpMethodTest.ValidMethods
[       OK ] HttpMethodTest.ValidMethods (0 ms)
[ RUN      ] HttpMethodTest.InvalidMethod
[       OK ] HttpMethodTest.InvalidMethod (0 ms)
...
[----------] 5 tests from HttpMethodTest (1 ms total)

[----------] Global test environment tear-down
[==========] 5 tests from 1 test suite ran. (1 ms total)
[  PASSED  ] 5 tests.
```

### Return Codes

- `0` = All tests passed ✅
- `non-zero` = One or more tests failed ❌

---

## Running Specific Tests

### Filter by Test Suite

Run only HttpMethod tests:
```bash
./bin/test_runner --gtest_filter='HttpMethodTest.*'
```

Run only Port tests:
```bash
./bin/test_runner --gtest_filter='PortTest.*'
```

### Filter by Test Case

Run a specific test:
```bash
./bin/test_runner --gtest_filter='HttpMethodTest.ValidMethods'
```

### Multiple Patterns

Use `:` to combine patterns (OR logic):
```bash
./bin/test_runner --gtest_filter='HttpMethodTest.*:PortTest.*'
```

### Exclude Tests

Use `-` to exclude patterns:
```bash
# Run all tests EXCEPT Port tests
./bin/test_runner --gtest_filter='*:-PortTest.*'
```

---

## Test Output Options

### Verbose Output

Show detailed information:
```bash
./bin/test_runner --gtest_filter='*' --gtest_verbose
```

### Colored Output

Enable/disable colored output:
```bash
# Enable colors (default)
./bin/test_runner --gtest_color=yes

# Disable colors
./bin/test_runner --gtest_color=no

# Auto-detect based on terminal
./bin/test_runner --gtest_color=auto
```

### List Tests Without Running

See what tests are available:
```bash
./bin/test_runner --gtest_list_tests
```

**Output example:**
```
HttpMethodTest.
  ValidMethods
  InvalidMethod
  CaseInsensitive
  Equality
  CopyConstructor
PortTest.
  DefaultConstructor
  ConstructWithValidPort
  ...
```

---

## Test Results

### Reading Results

```
[==========] Running 5 tests from 1 test suite.
```
- Total number of tests to run

```
[ RUN      ] HttpMethodTest.ValidMethods
```
- Currently running test

```
[       OK ] HttpMethodTest.ValidMethods (0 ms)
```
- Test passed ✅ (with execution time)

```
[  FAILED  ] PortTest.DefaultConstructor (1 ms)
```
- Test failed ❌ (with execution time)

```
[  PASSED  ] 5 tests.
```
- Summary of successful tests

```
[  FAILED  ] 2 tests, listed below:
[  FAILED  ] PortTest.DefaultConstructor
[  FAILED  ] PortTest.ValidatePortRange
```
- Summary of failed tests (if any)

---

## Understanding Test Failures

### Example Failure Output

```
[ RUN      ] PortTest.DefaultConstructor
test_Port.cpp:28: Failure
Expected equality of these values:
  80
  port.getValue()
    Which is: 0
[  FAILED  ] PortTest.DefaultConstructor (0 ms)
```

**Reading the failure:**
1. `test_Port.cpp:28` - File and line number of the assertion
2. `Expected equality of these values:` - What went wrong
3. `80` - Expected value
4. `port.getValue()` → `0` - Actual value returned

**What to do:**
- Check the test to see if expectations are correct
- Check the implementation to see if behavior is correct
- Update either tests or implementation to match

---

## Running Tests in Different Modes

### Shuffle Tests

Run tests in random order (helps detect test dependencies):
```bash
./bin/test_runner --gtest_shuffle
```

### Repeat Tests

Run tests multiple times:
```bash
# Run tests 10 times
./bin/test_runner --gtest_repeat=10
```

### Break on Failure

Stop immediately when a test fails:
```bash
./bin/test_runner --gtest_break_on_failure
```

### Run Failed Tests Only

First run (some tests fail):
```bash
./bin/test_runner
```

Re-run only failed tests:
```bash
./bin/test_runner --gtest_filter='*Port*'  # If Port tests failed
```

---

## Continuous Integration

### CI/CD Pipeline Usage

The test suite is designed for automation:

```bash
#!/bin/bash
cd tests/
make clean
make

# Run tests
./bin/test_runner

# Capture exit code
if [ $? -eq 0 ]; then
    echo "✅ All tests passed"
    exit 0
else
    echo "❌ Tests failed"
    exit 1
fi
```

### GitHub Actions Example

The project includes ready-to-use GitHub Actions workflows:

**`.github/workflows/tests.yml`** - Automated testing:
```yaml
name: Tests
on: [push, pull_request]
jobs:
  test:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v3
      - name: Install Google Test 1.8.1
        run: |
          cd tests/
          sudo ./install_gtest.sh
      - name: Build Tests
        run: |
          cd tests/
          make
      - name: Run Tests
        run: |
          cd tests/
          ./bin/test_runner --gtest_output=xml:test-results.xml
```

**`.github/workflows/build.yml`** - Build verification:
```yaml
name: Build
on: [push, pull_request]
jobs:
  build:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v3
      - name: Build project
        run: make
```

### Custom CI Script Example

## Performance Testing

### Measure Test Time

```bash
time ./bin/test_runner
```

**Output:**
```
[==========] 5 tests from 1 test suite ran. (2 ms total)
[  PASSED  ] 5 tests.

real    0m0.042s
user    0m0.012s
sys     0m0.028s
```

### Profile Slow Tests

Add timing to each test:
```bash
./bin/test_runner --gtest_print_time=1
```

---

## Debug Mode

### Run with GDB

```bash
gdb ./bin/test_runner

# In GDB:
(gdb) run
(gdb) bt          # Show backtrace if crash
(gdb) break test_HttpMethod.cpp:45  # Set breakpoint
(gdb) run --gtest_filter='HttpMethodTest.ValidMethods'
```

### Run with Valgrind

Check for memory leaks:
```bash
valgrind --leak-check=full ./bin/test_runner
```

---

## Test Organization

### Current Test Structure

```
tests/
├── unit/                           # Unit tests
│   ├── test_HttpMethod.cpp         ✅ Working
│   ├── test_Port.cpp               ⚠️  Partial (implementation mismatch)
│   ├── test_Path.cpp.disabled      🚧 Disabled (unimplemented)
│   └── test_ConfigParser.cpp.disabled  🚧 Disabled (unimplemented)
│
├── integration/                    # Integration tests
│   └── test_FileHandler_Integration.cpp.disabled  🚧 Disabled
│
└── mocks/                          # Mock objects
    ├── MockLogger.hpp
    ├── MockLogger.cpp
    └── MockConfigParser.hpp
```

### Test Categories

**Unit Tests** (`unit/`)
- Test individual components in isolation
- Fast execution
- No external dependencies
- Mock all dependencies

**Integration Tests** (`integration/`)
- Test component interactions
- May use real filesystem/network
- Slower execution
- Test realistic scenarios

---

## Currently Working Tests

### ✅ HttpMethod Tests (All Passing)

```bash
./bin/test_runner --gtest_filter='HttpMethodTest.*'
```

**Tests:**
- ValidMethods (5 assertions)
- InvalidMethod (2 assertions)
- CaseInsensitive (4 assertions)
- Equality (3 assertions)
- CopyConstructor (2 assertions)

**Total:** 5 tests, 16 assertions, all passing ✅

---

## Tests Needing Attention

### ⚠️ Port Tests (9/10 Failing)

```bash
./bin/test_runner --gtest_filter='PortTest.*'
```

**Issue:** Implementation differs from test expectations

See [TESTING_STATUS.md](TESTING_STATUS.md#port-domainvalue_objects) for details.

---

## Disabled Tests

Tests are disabled (renamed to `.cpp.disabled`) when:
- Feature not yet implemented
- Architectural blockers exist
- Dependency issues prevent compilation

To enable a test:
```bash
mv unit/test_Path.cpp.disabled unit/test_Path.cpp
make clean && make
```

See [TESTING_STATUS.md](TESTING_STATUS.md) for complete list and reasons.

---

## Makefile Targets

The Makefile provides several convenient targets:

```bash
make            # Build tests (default)
make clean      # Remove compiled files
make re         # Clean and rebuild
make test       # Build and run all tests
make unit       # Run only unit tests (when implemented)
make integration # Run only integration tests (when implemented)
```

---

## Tips & Best Practices

### 1. Run Tests Frequently
```bash
# After every change:
make && ./bin/test_runner
```

### 2. Run Specific Tests While Developing
```bash
# Test only what you're working on:
./bin/test_runner --gtest_filter='HttpMethodTest.*'
```

### 3. Check Test Status Before Implementing
```bash
# See what's disabled and why:
cat docs/TESTING_STATUS.md
```

### 4. Clean Build When Changing Code Structure
```bash
make clean && make
```

### 5. Use Filters to Focus
```bash
# Only test the feature you're debugging:
./bin/test_runner --gtest_filter='*YourFeature*'
```

---

## Troubleshooting

### Tests Won't Compile

1. Check Google Test installation:
   ```bash
   ls /usr/local/lib/libgtest*
   ```

2. Verify C++98 compatibility:
   ```bash
   grep "std::tuple" /usr/local/include/gtest/ || echo "OK"
   ```

3. See [INSTALLATION.md](INSTALLATION.md) for reinstallation

### Tests Crash at Startup

Some features cause crashes due to unimplemented code. Check:
- [TESTING_STATUS.md](TESTING_STATUS.md) - List of excluded features
- `Makefile` - PROJECT_SRCS exclusions

### Linking Errors

```
undefined reference to `SomeClass::someMethod()'
```

The method isn't implemented yet. See [TESTING_STATUS.md](TESTING_STATUS.md).

---

## Additional Resources

- **Installation:** [INSTALLATION.md](INSTALLATION.md)
- **Feature Status:** [TESTING_STATUS.md](TESTING_STATUS.md)
- **Quick Fixes:** [GOOGLETEST_QUICKFIX.md](GOOGLETEST_QUICKFIX.md)
- **Learning Guide:** [BEGINNER_GUIDE.md](BEGINNER_GUIDE.md)
- **Google Test Guide:** [GOOGLETEST_GUIDE.md](GOOGLETEST_GUIDE.md)
- **Test Scenarios:** [TESTING_SCENARIOS.md](TESTING_SCENARIOS.md)

**Last Updated:** December 26, 2025
