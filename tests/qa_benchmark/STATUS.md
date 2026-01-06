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
**Status:** Running (ListenDirective -> Uri -> Route)

**Completed:**
- ✅ Port (47 tests, 100%)
- ✅ Path (62 tests, 100%)  
- ✅ Permission (51 tests, 100%)
- ✅ Host (98 tests, 100%)
- ✅ ErrorCode (77 tests, 100%)
- ✅ ErrorPage (56 tests, 100%)
- ✅ HttpMethod (39 tests, 100%)
- ✅ Size (44 tests, 100%)
- ✅ QueryStringBuilder (36 tests, 100%)
- ✅ UploadAccess (52 tests, 100%)
- ✅ RegexPattern (50 tests, 100%)
- ✅ CgiConfig (63 tests, 100%)
- ✅ ListenDirective (59 tests, 100%)

**Pending:**
- ⚠️ Uri (115 tests, 47 bugs)
- ⚠️ Route (62 tests, 25 bugs)
- ❌ RouteMatchInfo (0 tests)

---

See [SMALL_FIXES_STRATEGY.md](SMALL_FIXES_STRATEGY.md) for detailed fix guidance.
