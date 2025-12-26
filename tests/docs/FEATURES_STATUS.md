# Feature Testing Status

This document tracks which features are ready for testing and which are still being implemented.

## ✅ Fully Implemented & Tested

### HttpMethod (domain/value_objects)
**Status:** All tests passing ✅

Tests validating HTTP method operations:
- ✅ ValidMethods - Tests standard HTTP methods (GET, POST, PUT, DELETE, HEAD, OPTIONS)
- ✅ InvalidMethod - Verifies rejection of invalid methods
- ✅ CaseInsensitive - Confirms case-insensitive method parsing
- ✅ Equality - Tests method comparison operators
- ✅ CopyConstructor - Validates copy semantics

**How to test:**
```bash
cd tests/
make
./bin/test_runner --gtest_filter='HttpMethodTest.*'
```

---

## ⚠️ Partially Implemented (Tests Need Updates)

### Port (domain/value_objects)
**Status:** Implementation differs from test expectations ⚠️

**Issues:**
- Default constructor returns 0 instead of 80
- Port validation logic stricter than tests expect
- 9 out of 10 tests failing

**Next Steps:**
- [ ] Review actual Port class implementation
- [ ] Update tests to match implementation OR
- [ ] Update implementation to match test expectations
- [ ] Decide on proper default port value

**Current test status:**
```bash
# Run to see which tests fail
./bin/test_runner --gtest_filter='PortTest.*'
```

---

## 🚧 Not Yet Implemented

### Path (domain/value_objects)
**Status:** Tests disabled (unimplemented features)

**What's missing:**
- Core path validation logic
- Path component extraction (directory, filename, extension)
- Path manipulation methods

**Test file:** `unit/test_Path.cpp.disabled`

---

### ConfigParser (infrastructure/config)
**Status:** Tests disabled (implementation incomplete)

**What's missing:**
- Configuration file parsing
- Error handling for invalid configs
- Configuration validation

**Test file:** `unit/test_ConfigParser.cpp.disabled`

---

### FileHandler Integration
**Status:** Tests disabled (design issues)

**Blockers:**
- `FileSystemHelper` has private constructor/destructor
- Cannot instantiate for integration testing
- Needs architectural review

**Test file:** `integration/test_FileHandler_Integration.cpp.disabled`

---

## 🔒 Excluded from Test Builds

The following components are excluded from test compilation due to unimplemented methods or initialization issues:

### ErrorCode (domain/value_objects)
**Reason:** Multiple unimplemented methods

**Missing implementations:**
- Constructor with unsigned int
- `fromString()` method
- `movedPermanently()` method
- `isRedirection()` method
- `getValue()` method
- Destructor

**Impact:** All routing-related features blocked

---

### Uri (domain/value_objects)
**Reason:** Missing implementation

**Missing implementations:**
- `findAuthorityEnd()` method

---

### Routing Components
**Excluded files:**
- `Route.cpp` - Depends on ErrorCode
- `RouteMatcher.cpp` - Depends on ErrorCode
- `RouteConfigAdapter.cpp` - Depends on ErrorCode

**Reason:** These depend on ErrorCode which has unimplemented methods

---

### CGI & Directory Listing
**Excluded files:**
- `CgiConfig.cpp` - Uses RegexPattern with empty pattern issues
- `DirectoryLister.cpp` - Has static RegexPattern initialization problems

**Reason:** RegexPattern throws exception when initialized with empty pattern during static initialization

**Technical details:**
- RegexPattern validates pattern in constructor
- Empty patterns throw `RegexPatternException`
- Static initialization triggers validation before main()
- Causes crash at program startup

---

## 📊 Test Coverage Summary

| Component | Status | Tests Passing | Notes |
|-----------|--------|--------------|-------|
| HttpMethod | ✅ Ready | 5/5 (100%) | Fully working |
| Port | ⚠️ Partial | 1/10 (10%) | Implementation mismatch |
| Path | 🚧 Not Ready | 0/0 (disabled) | Tests disabled |
| ConfigParser | 🚧 Not Ready | 0/0 (disabled) | Tests disabled |
| FileHandler | 🚧 Not Ready | 0/0 (disabled) | Design blocker |
| ErrorCode | 🚧 Not Ready | - | Unimplemented |
| Uri | 🚧 Not Ready | - | Unimplemented |
| Route | 🚧 Not Ready | - | Blocked by ErrorCode |
| RouteMatcher | 🚧 Not Ready | - | Blocked by ErrorCode |
| RouteConfigAdapter | 🚧 Not Ready | - | Blocked by ErrorCode |
| CgiConfig | 🚧 Not Ready | - | RegexPattern issues |
| DirectoryLister | 🚧 Not Ready | - | RegexPattern issues |

---

## 🎯 Recommended Implementation Order

To maximize testability, implement features in this order:

1. **ErrorCode** (Highest Priority)
   - Unblocks Route, RouteMatcher, RouteConfigAdapter
   - Critical for HTTP response handling
   - Start here to enable routing tests

2. **RegexPattern Initialization**
   - Fix empty pattern validation in default constructor
   - Enables CgiConfig and DirectoryLister
   - Medium impact, multiple dependencies

3. **Uri**
   - Implement `findAuthorityEnd()`
   - Enables URI parsing tests
   - Medium priority

4. **Port Validation**
   - Clarify default port behavior
   - Update tests or implementation
   - Low priority, minor impact

5. **Path**
   - Implement core path operations
   - Enable path validation tests
   - Medium priority

6. **ConfigParser**
   - Complete configuration parsing
   - Enable config-driven testing
   - Medium priority

7. **FileSystemHelper Refactoring**
   - Make constructor public or provide factory
   - Enable integration tests
   - Lower priority, design decision needed

---

## 🔄 Enabling Disabled Tests

Once features are implemented, re-enable tests:

### Re-enable Test File
```bash
cd tests/
mv unit/test_Path.cpp.disabled unit/test_Path.cpp
```

### Re-enable Source Files in Makefile
Edit `tests/Makefile` and remove exclusions from `PROJECT_SRCS`:
```makefile
# Remove these exclusion lines as features are implemented:
! -name 'ErrorCode.cpp' \
! -name 'Uri.cpp' \
! -name 'Route.cpp' \
```

### Rebuild and Test
```bash
make clean
make
./bin/test_runner
```

---

## 📝 Notes for Developers

- **Before implementing:** Read the corresponding test file to understand requirements
- **After implementing:** Re-enable tests and verify they pass
- **If tests fail:** Either fix implementation or update tests (document why)
- **Add new tests:** When adding features, write tests first (TDD)

---

## 📞 Getting Help

- **Installation issues:** See [docs/INSTALLATION.md](INSTALLATION.md)
- **Quick fixes:** See [docs/QUICK_FIX.md](QUICK_FIX.md)
- **Learning to test:** See [docs/BEGINNER_GUIDE.md](BEGINNER_GUIDE.md)
- **Running tests:** See [docs/RUNNING_TESTS.md](RUNNING_TESTS.md)

**Last Updated:** December 26, 2025
