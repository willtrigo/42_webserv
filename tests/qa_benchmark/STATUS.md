# QA Benchmark Status

**Last Updated:** January 3, 2026  
**Purpose:** Track regression testing during value object fixes

---

## 📦 Baseline Files

All reference outputs in `tests/qa_benchmark/baseline/`:

| File | Size | Description |
|------|------|-------------|
| `baseline_no_config.txt` | 7.8K | Server without args (uses default.conf) |
| `baseline_default_conf.txt` | 7.9K | Server with `./conf/default.conf` |
| `baseline_webserv_conf.txt` | 5.6K | Server with `./conf/webserv.conf` |
| `baseline_webserv_py_cgi_conf.txt` | 11K | Server with CGI config |

**Current test outputs** in `tests/qa_benchmark/current_tdd_fix/` (gitignored)

---

## ✅ Completed Fixes

### 1. Port (5 bugs) - ✅ 47/47 (100%)
**Status:** All tests passing  
**File:** `src/domain/http/value_objects/Port.cpp`

**Fixes:**
- Default constructor returns 80 (not 0)
- Port(0) now throws exception
- Leading zeros accepted ("08080" → 8080)
- Port("0") rejected

---

### 2. Path (4+ bugs) - ✅ 62/62 (100%)
**Status:** All tests passing  
**File:** `src/domain/filesystem/value_objects/Path.cpp`

**Fixes:**
- Normalize removes trailing slashes
- hasDirectoryTraversal() rejects all ".." components with URL decoding
- getDirectory()/getFilename() strip trailing slashes before extraction

---

### 3. Permission (4+ bugs) - ✅ 51/51 (100%)
**Status:** All tests passing  
**File:** `src/domain/filesystem/value_objects/Permission.cpp`

**Fixes:**
- Symbolic string validation (character checking)
- Special bits format: "SST rwxr-xr-x" (13 chars with space)
- parseRegularPermissions() refactored (extract→validate→build)
- **Test bugs fixed:** 0644/0755 incorrect expectations

---

### 4. Host (15 bugs) - ✅ 98/98 (100%)
**Status:** All tests passing  
**File:** `src/domain/http/value_objects/Host.cpp`

**Fixes:**
- IPv6 group length validation (max 4 hex digits per group)
- IPv6 group count validation (max 7 explicit groups with ::)
- IPv4/Hostname leading/trailing dot rejection
- **Test bugs fixed:** 
  - "2001:0db8:85a3::8a2e:0370:7334:1234" is valid (7+1=8 groups)
  - "invalid-host" is valid hostname
  - ComparisonWithDifferentTypes wrong expectations

---

## 📊 Current Test Status

**Total Tests:** 1041  
**Status:** Running (QueryStringBuilder segfault blocks full suite)

**Completed:**
- ✅ Port: 47/47 (100%)
- ✅ Path: 62/62 (100%)  
- ✅ Permission: 51/51 (100%)
- ✅ Host: 98/98 (100%)

---

## 🎯 Next Priority Fixes

### Option A: ErrorPage (3 bugs) - Quick Win
**Failing Tests:** 53/56 passing (3 failures)
- `ErrorPageTest.DefaultConstructor`
- `ErrorPageTest.ConstructWithEmptyContent`
- `ErrorPageTest.ValidateThroughConstructorNoContent`

**Estimated time:** 15-30 minutes  
**Risk:** Low (likely validation logic)

---

### Option B: QueryStringBuilder Segfault - Critical
**Issue:** Segmentation fault in `CopyConstructor` test  
**Blocks:** Full test suite execution  
**Estimated time:** 30-60 minutes  
**Risk:** Medium (memory management issue)

---

### Option C: Run Regression Check
**Purpose:** Verify all fixes don't break server startup
```bash
cd tests/qa_benchmark
./check_regression.sh
```

**Expected:** All 4 configs should show PASS (no diffs from baseline)

---

## 🚀 Ready to Start!

Open Port.cpp and let's fix those 5 bugs:
```bash
code src/domain/http/value_objects/Port.cpp
```

See [SMALL_FIXES_STRATEGY.md](SMALL_FIXES_STRATEGY.md) for detailed fix guidance.
