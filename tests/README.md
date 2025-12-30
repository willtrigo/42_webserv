# Webserver Test Suite

[![Tests](https://github.com/biralavor/42_webserver/actions/workflows/tests.yml/badge.svg)](https://github.com/biralavor/42_webserver/actions/workflows/tests.yml)

Comprehensive testing infrastructure for the 42_webserver project using Google Test 1.8.1 (C++98 compatible).

## Quick Start

```bash
cd tests/

# Install Google Test 1.8.1 (first time only)
sudo ./install_gtest.sh

# Build tests
make

# Run tests
./bin/test_runner
```

## 📊 Current Status

**Test Suite: 187 tests | 185 passing (98.93%) | 2 failing**

| Component | Status | Tests | Coverage |
|-----------|--------|-------|----------|
| ErrorCode | ✅ Complete | 50/50 passing | 100% |
| HttpMethod | ✅ Complete | 5/5 passing | 100% |
| Path | ✅ Complete | 25/25 passing | 100% |
| Size | ✅ Complete | 44/44 passing | 100% |
| MockLogger | ✅ Complete | 13/13 passing | 100% |
| MockServer | ✅ Complete | 16/16 passing | 100% |
| MockResponseBuilder | ✅ Complete | 21/21 passing | 100% |
| MockRequestParser | ✅ Complete | 3/3 passing | 100% |
| Port | ⚠️ Nearly Complete | 8/10 passing | 80% - 2 test expectation issues |

**Last test run:** December 29, 2025  
**Known Issues:** 
- PortTest.DefaultConstructor: Expected default port 80, got 0
- PortTest.InvalidPortZero: Port(0) should throw exception but doesn't

## 📁 Structure

```
tests/
├── README.md               # This file
├── Makefile                # Build configuration
├── test_main.cpp           # Test entry point
├── install_gtest.sh        # Google Test installer
├── check_gtest.sh          # Version checker
│
├── unit/                   # Unit tests
│   ├── test_ErrorCode.cpp             ✅ 50 tests passing
│   ├── test_HttpMethod.cpp            ✅ 5 tests passing
│   ├── test_Path.cpp                  ✅ 25 tests passing
│   ├── test_Port.cpp                  ⚠️ 8/10 passing
│   ├── test_Size.cpp                  ✅ 44 tests passing
│   └── test_MockLogger.cpp            ✅ 13 tests passing
│
├── integration/            # Integration tests
│   └── test_FileHandler_Integration.cpp.disabled  🚧 Disabled
│
├── mocks/                  # Mock implementations
│   ├── MockLogger.hpp
│   ├── MockLogger.cpp
│   └── MockConfigParser.hpp
│
├── fixtures/               # Test data files
│
└── docs/                   # Documentation
    ├── FEATURES_STATUS.md      # Detailed feature status
    ├── INSTALLATION.md         # Installation guide
    ├── RUNNING_TESTS.md        # How to run tests
    ├── WRITING_TESTS.md        # How to write tests
    ├── BEGINNER_GUIDE.md       # Testing fundamentals
    └── QUICK_FIX.md            # Common error fixes
```

## 🚀 Common Commands

```bash
# Build tests
make

# Run all tests
./bin/test_runner

# Run specific test suite
./bin/test_runner --gtest_filter='HttpMethodTest.*'

# Run specific test
./bin/test_runner --gtest_filter='HttpMethodTest.ValidMethods'

# List all tests
./bin/test_runner --gtest_list_tests

# Clean build
make clean && make
```

## 📚 Documentation

| Document | Description |
|----------|-------------|
| [FEATURES_STATUS.md](docs/FEATURES_STATUS.md) | Which features are ready to test, which aren't |
| [INSTALLATION.md](docs/INSTALLATION.md) | Install Google Test 1.8.1 |
| [RUNNING_TESTS.md](docs/RUNNING_TESTS.md) | Run tests with various options |
| [WRITING_TESTS.md](docs/WRITING_TESTS.md) | Write new tests (patterns & examples) |
| [TESTING_STRATEGY.md](docs/TESTING_STRATEGY.md) | Overall testing strategy & architecture |
| [CI_CD.md](docs/CI_CD.md) | GitHub Actions & continuous integration |
| [BEGINNER_GUIDE.md](docs/BEGINNER_GUIDE.md) | Learn testing from scratch |
| [QUICK_FIX.md](docs/QUICK_FIX.md) | Fix common installation errors |

## ⚠️ Important: C++98 Compatibility

This project uses **C++98**, requiring **Google Test 1.8.x** (last version supporting C++98).

| Google Test Version | C++ Standard | Status |
|---------------------|--------------|--------|
| 1.8.x | C++98 | ✅ **Use this!** |
| 1.10+ | C++11+ | ❌ Not compatible |
| Latest (1.14+) | C++14+ | ❌ Not compatible |

**Wrong version installed?** See [docs/QUICK_FIX.md](docs/QUICK_FIX.md)

## ✅ Working Tests

### HttpMethod Tests (All Passing)

```bash
./bin/test_runner --gtest_filter='HttpMethodTest.*'
```

**5 tests covering:**
- Valid HTTP methods (GET, POST, PUT, DELETE, HEAD, OPTIONS)
- Invalid method rejection
- Case-insensitive parsing
- Equality comparison
- Copy constructor

## 🔧 Troubleshooting

### "std::tuple" Error

You installed the wrong Google Test version (too new for C++98).

**Fix:**
```bash
cd tests/
sudo ./install_gtest.sh
```

See [docs/QUICK_FIX.md](docs/QUICK_FIX.md) for details.

### Test Won't Compile

Check [docs/FEATURES_STATUS.md](docs/FEATURES_STATUS.md) - the feature might be excluded due to unimplemented dependencies.

### Linking Errors

```
undefined reference to `SomeClass::someMethod()'
```

The method isn't implemented yet. See [docs/FEATURES_STATUS.md](docs/FEATURES_STATUS.md#-not-yet-implemented).

### More Help

See [docs/RUNNING_TESTS.md](docs/RUNNING_TESTS.md#troubleshooting) for complete troubleshooting guide.

## 🎯 For Developers

### Before Implementing a Feature

1. Check [docs/FEATURES_STATUS.md](docs/FEATURES_STATUS.md) - see what tests expect
2. Read test file (e.g., `unit/test_Path.cpp.disabled`)
3. Understand requirements from tests

### After Implementing a Feature

1. Re-enable test file:
   ```bash
   mv unit/test_Path.cpp.disabled unit/test_Path.cpp
   ```

2. Update `Makefile` to include source files

3. Build and run:
   ```bash
   make clean && make
   ./bin/test_runner --gtest_filter='PathTest.*'
   ```

4. If tests fail: fix implementation OR update tests (document why)

### Writing New Tests

See [docs/WRITING_TESTS.md](docs/WRITING_TESTS.md) for:
- Test naming conventions
- Test structure patterns
- Using mocks
- Best practices
- Example code

### For Beginners

New to testing? Start here:
1. Read [docs/BEGINNER_GUIDE.md](docs/BEGINNER_GUIDE.md)
2. Look at `unit/test_HttpMethod.cpp` (heavily commented)
3. Run tests to see how they work
4. Write simple tests for your features

## 📊 Testing Philosophy

### Test Layers

```
Domain Layer (Value Objects, Entities)
  ↓ Unit Tests: Pure logic, no dependencies
  
Infrastructure Layer (Adapters, IO)
  ↓ Unit Tests: Mock all external dependencies
  ↓ Integration Tests: Real dependencies
  
Presentation Layer (CLI)
  ↓ Unit Tests: Mock everything below
```

### What We Test

✅ **Do test:**
- Business logic
- Validation rules
- Edge cases
- Error handling
- Public interfaces

❌ **Don't test:**
- Private implementation details
- External libraries
- Trivial getters/setters

## 🔄 CI/CD Integration

Return codes:
- `0` = All tests passed ✅
- `non-zero` = Tests failed ❌

Example CI pipeline:
```bash
#!/bin/bash
cd tests/
make clean
make
./bin/test_runner
exit $?
```

See [docs/RUNNING_TESTS.md#continuous-integration](docs/RUNNING_TESTS.md#continuous-integration) for complete examples.

## 📈 Coverage Goals

| Priority | Component | Status |
|----------|-----------|--------|
| High | ErrorCode | 🚧 Implement next |
| High | Route/Routing | 🚧 Blocked by ErrorCode |
| Medium | Path | 🚧 Tests ready |
| Medium | ConfigParser | 🚧 Tests ready |
| Low | Port | ⚠️ Update tests |

## 🤝 Contributing

1. **Follow conventions:** See [docs/WRITING_TESTS.md](docs/WRITING_TESTS.md)
2. **Write tests first:** TDD approach recommended
3. **Keep tests simple:** One concept per test
4. **Run tests often:** `make && ./bin/test_runner`
5. **Update docs:** Keep [docs/FEATURES_STATUS.md](docs/FEATURES_STATUS.md) current

## 📞 Getting Help

- **Installation issues:** [docs/INSTALLATION.md](docs/INSTALLATION.md)
- **Can't run tests:** [docs/RUNNING_TESTS.md](docs/RUNNING_TESTS.md)
- **Writing tests:** [docs/WRITING_TESTS.md](docs/WRITING_TESTS.md)
- **Learning basics:** [docs/BEGINNER_GUIDE.md](docs/BEGINNER_GUIDE.md)
- **Quick fixes:** [docs/QUICK_FIX.md](docs/QUICK_FIX.md)
- **Feature status:** [docs/FEATURES_STATUS.md](docs/FEATURES_STATUS.md)
- **Overall strategy:** [docs/TESTING_STRATEGY.md](docs/TESTING_STRATEGY.md)

## 📜 License

Part of the 42_webserver project. See [../LICENSE](../LICENSE) for details.

---

**Last Updated:** December 26, 2025  
**Google Test Version:** 1.8.1  
**C++ Standard:** C++98  
**Test Coverage:** 1 component (HttpMethod) fully tested, 5 tests passing
