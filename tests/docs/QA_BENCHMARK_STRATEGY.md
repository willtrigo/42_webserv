# QA Benchmark Strategy (Small Fixes) - Value Objects Edition

**Date:** January 2, 2026  
**Goal:** Fix low-hanging fruit value object bugs without major refactoring  
**Target:** Improve test passing rate from 88.7% to 95%+

---

## 🎯 Priority Order (Easiest to Hardest)

### 1️⃣ **Port (5 bugs) - START HERE** ⏱️ 15-30 minutes
**File:** `src/domain/http/value_objects/Port.cpp`  
**Test Coverage:** 47 tests, 42 passing (89.4%)  
**Why First:** Single validation logic fix, minimal dependencies

**Bugs:**
1. Default constructor returns 0 instead of 80
2. Valid ports (1-65535) rejected
3. Invalid port 0 accepted
4. Port 65536+ not rejected
5. Leading zeros handling

**Fix Strategy:**
```cpp
// Current (BROKEN):
if (value < 1 || value > 65535) {
    throw PortException("Port must be 0"); // WRONG!
}

// Fix to:
if (value < 1 || value > 65535) {
    throw PortException("Port must be between 1 and 65535");
}

// Default constructor:
Port::Port() : m_value(80) {} // Change from 0 to 80
```

**Test Command:**
```bash
cd tests
make test-Port
# Or: ./bin/test_runner --gtest_filter='PortTest.*'
```

**QA Check:**
```bash
make && ./tests/qa_benchmark/check_regression.sh
```

---

### 2️⃣ **Path (4 bugs) - SECOND** ⏱️ 30-45 minutes
**File:** `src/domain/filesystem/value_objects/Path.cpp`  
**Test Coverage:** 62 tests, 58 passing (93.5%)  
**Why Second:** Small edge case fixes

**Bugs from TESTING_STATUS.md:**
1. Trailing slash inconsistencies
2. Path traversal detection gaps
3. Directory extraction issues
4. Edge cases in normalization

**Fix Strategy:**
- Review failed tests: `./bin/test_runner --gtest_filter='PathTest.*' --gtest_also_run_disabled_tests`
- Fix normalization: Consistent handling of `//`, `./`, `../`
- Trailing slash: Preserve or strip based on context
- Directory extraction: Handle edge cases like `/`, `file.txt`

**Test Command:**
```bash
cd tests
make test-Path
```

**QA Check:**
```bash
make && ./tests/qa_benchmark/check_regression.sh
```

---

### 3️⃣ **Permission (4 bugs) - THIRD** ⏱️ 30-45 minutes
**File:** `src/domain/filesystem/value_objects/Permission.cpp`  
**Test Coverage:** 51 tests, 47 passing (92.2%)  
**Why Third:** Isolated logic issues

**Bugs from TESTING_STATUS.md:**
1. `CLASS_ALL` logic incorrect
2. Symbolic validation issues (rwx parsing)

**Fix Strategy:**
- Fix `CLASS_ALL` bit masking: Should represent USER + GROUP + OTHER
- Validate symbolic strings: `r--`, `rw-`, `rwx`, etc.
- Check octal parsing: `0644`, `0755`

**Test Command:**
```bash
cd tests
make test-Permission
```

---

### 4️⃣ **Host (15 bugs) - FOURTH** ⏱️ 1-2 hours
**File:** `src/domain/http/value_objects/Host.cpp`  
**Test Coverage:** 98 tests, 83 passing (84.7%)  
**Why Fourth:** More complex but well-tested

**Bugs from TESTING_STATUS.md:**
1. IPv6 validation failures
2. Hostname parsing issues
3. Trailing dot handling (`.` at end)

**Fix Strategy:**
- IPv6: Use proper bracket detection `[::1]`
- Hostname: Validate DNS label rules (alphanumeric, hyphens)
- Trailing dots: Strip or validate based on RFC

**Test Command:**
```bash
cd tests
make test-Host
```

---

## ❌ SKIP FOR NOW (Requires Design Refactoring)

### ErrorPage (3 bugs - DESIGN ISSUE)
**Status:** 56 tests, 53 passing (94.6%)  
**Problem:** Default constructor creates invalid state  
**Why Skip:** Requires architectural discussion about value object design philosophy

### Uri (47 bugs - MAJOR REFACTOR)
**Status:** 115 tests, 68 passing (59.1%)  
**Problem:** Authority parsing, port handling, scheme validation all broken  
**Why Skip:** Too many interconnected bugs, needs full rewrite session

### QueryStringBuilder (SEGFAULT - CRITICAL)
**Status:** 36 tests, BLOCKED  
**Problem:** Crashes immediately  
**Why Skip:** Requires debugging session with valgrind/gdb

### ListenDirective (16 bugs)
**Status:** 59 tests, 43 passing (72.9%)  
**Problem:** Parsing, validation, state management  
**Why Skip:** Mid-level complexity, save for later

### Route (25 bugs)
**Status:** 62 tests, 37 passing (59.7%)  
**Problem:** Constructor validation too strict, handler configuration  
**Why Skip:** Needs design review of validation logic

### RegexPattern (10 bugs)
**Status:** 50 tests, 40 passing (80%)  
**Problem:** Empty pattern handling, length validation  
**Why Skip:** Affects CgiConfig (6 dependent bugs)

### CgiConfig (12 bugs, 6 from RegexPattern)
**Status:** 63 tests, 51 passing (81%)  
**Problem:** Dependency on RegexPattern + exception handling  
**Why Skip:** Wait for RegexPattern fixes first

---

## 📊 Expected Impact

**Before Fixes:**
- Total: 1005 tests
- Passing: 892 (88.7%)
- Failing: 113

**After Port + Path + Permission + Host (28 bugs fixed):**
- Total: 1005 tests
- Passing: ~920 (91.5%)
- Failing: ~85

**Time Investment:** 3-5 hours for 3% improvement
**Risk Level:** LOW - All fixes are isolated value object logic

---

## 🔄 Workflow

1. **Fix Port** (15-30 min)
   ```bash
   # Edit src/domain/http/value_objects/Port.cpp
   make test-Port
   make && ./tests/qa_benchmark/check_regression.sh
   git add -p && git commit -m "fix(Port): correct validation range 1-65535"
   ```

2. **Fix Path** (30-45 min)
   ```bash
   # Edit src/domain/filesystem/value_objects/Path.cpp
   make test-Path
   make && ./tests/qa_benchmark/check_regression.sh
   git add -p && git commit -m "fix(Path): normalize trailing slashes and traversal"
   ```

3. **Fix Permission** (30-45 min)
   ```bash
   # Edit src/domain/filesystem/value_objects/Permission.cpp
   make test-Permission
   make && ./tests/qa_benchmark/check_regression.sh
   git add -p && git commit -m "fix(Permission): CLASS_ALL logic and symbolic validation"
   ```

4. **Fix Host** (1-2 hours)
   ```bash
   # Edit src/domain/http/value_objects/Host.cpp
   make test-Host
   make && ./tests/qa_benchmark/check_regression.sh
   git add -p && git commit -m "fix(Host): IPv6 brackets and hostname validation"
   ```

5. **Run Full Suite**
   ```bash
   cd tests
   make test
   make test-report
   ```

---

## ✅ Success Criteria

- [ ] Port: 47/47 tests passing (100%)
- [ ] Path: 62/62 tests passing (100%)
- [ ] Permission: 51/51 tests passing (100%)
- [ ] Host: 98/98 tests passing (100%)
- [ ] **Overall: 920/1005 tests passing (91.5%+)**
- [ ] No regressions in server startup (QA benchmark)
- [ ] All changes committed with clear messages

---

## 🚀 Ready to Start?

**Recommendation: Start with Port** ✅

It's the easiest win - single validation logic fix, 5 bugs squashed, 15-30 minutes of work.

```bash
# Open the file
code src/domain/http/value_objects/Port.cpp

# Run tests in watch mode (optional)
cd tests
make test-Port

# After fix, verify
make && ./qa_benchmark/check_regression.sh
```
