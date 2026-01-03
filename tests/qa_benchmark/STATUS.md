# QA Benchmark Complete ✅

**Date:** January 2, 2026  
**Purpose:** Establish regression testing baseline before small fixes

---

## 📦 Baseline Files Created

All files saved in `tests/qa_benchmark/baseline/`:

| File | Size | Description |
|------|------|-------------|
| `baseline_no_config.txt` | 7.8K | Server without args (uses default.conf) |
| `baseline_default_conf.txt` | 7.9K | Server with `./conf/default.conf` |
| `baseline_webserv_conf.txt` | 5.6K | Server with `./conf/webserv.conf` |
| `baseline_webserv_py_cgi_conf.txt` | 11K | Server with CGI config |

**Current test outputs** stored in `tests/qa_benchmark/current_tdd_fix/` (gitignored)

---

## ✅ How to Use

### After Each Fix
```bash
# 1. Make your fix
vim src/domain/http/value_objects/Port.cpp

# 2. Rebuild
make

# 3. Run unit tests
cd tests && make test-Port

# 4. Check for regressions
./qa_benchmark/check_regression.sh
```

---

## 🎯 Next: Start with Port Fixes

According to [SMALL_FIXES_STRATEGY.md](SMALL_FIXES_STRATEGY.md):

**Port has 5 bugs** (easiest fix, 15-30 minutes):

1. ✅ **DefaultConstructor** - Returns 0, should return 80
2. ✅ **InvalidPortZero** - Port 0 accepted, should throw
3. ✅ **ParsePortStringShouldHandleLeadingZeros** - "08080" throws, should accept as 8080
4. ✅ **ParsePortStringShouldHandleMultipleLeadingZeros** - "00080" throws, should accept as 80
5. ✅ **ParsePortStringShouldRejectZero** - "0" accepted, should throw

**Fix file:** `src/domain/http/value_objects/Port.cpp`

**Test command:**
```bash
cd tests
./bin/test_runner --gtest_filter='PortTest.*'
```

**Expected result:** 47/47 tests passing (100%)

---

## 📊 Current Test Status

- **Total:** 1005 tests
- **Passing:** 892 (88.7%)
- **Failing:** 113
- **Target after fixes:** 920+ (91.5%+)

**Fix order:**
1. Port (5 bugs) ← ✅ All Done
2. Path (4 bugs) ← **NEXT**
3. Permission (4 bugs)
4. Host (15 bugs)

---

## 🚀 Ready to Start!

Open Port.cpp and let's fix those 5 bugs:
```bash
code src/domain/http/value_objects/Port.cpp
```

See [SMALL_FIXES_STRATEGY.md](SMALL_FIXES_STRATEGY.md) for detailed fix guidance.
