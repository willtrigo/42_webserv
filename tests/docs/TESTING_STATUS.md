# Feature Implementation Status

**⏰ DEADLINE: January 8, 2026 (13 days remaining)**

This document tracks implementation status aligned with the 13-day sprint plan. Status updates daily.

**Current Date:** December 26, 2025  
**Project Completion:** ~5% of mandatory requirements  
**Evaluation Ready:** ❌ NO - Need working server

See [TESTING_STRATEGY.md](TESTING_STRATEGY.md) for the complete 13-day implementation plan.

---

## 🚨 Critical Path Components (MUST HAVE)

These components are **absolutely required** for evaluation. Without them, the project will fail.

### Days 1-2: Socket Server (Dec 26-27)

#### I/O Multiplexing
**Status:** 🚧 NOT STARTED  
**Priority:** 🔴 CRITICAL  
**Time Estimate:** 8 hours  
**Blocks:** Everything - no server without this

**Requirements:**
- epoll/poll implementation
- Non-blocking socket handling
- Event loop with multiple file descriptors
- Timeout management

**Success Criteria:**
- [ ] Server accepts connections on port 8080
- [ ] Multiple simultaneous connections
- [ ] Non-blocking I/O working
- [ ] Can telnet without crash

**Implementation Status:**
- No code written yet
- Start with epoll example from man pages
- Copy proven patterns to save time

---

#### Socket Management
**Status:** 🚧 NOT STARTED  
**Priority:** 🔴 CRITICAL  
**Time Estimate:** 6 hours  
**Blocks:** All network communication

**Requirements:**
- socket() + bind() + listen() setup
- Non-blocking flags (fcntl)
- SO_REUSEADDR option
- Multi-port binding (for multi-server)

**Success Criteria:**
- [ ] Bind to port 8080 successfully
- [ ] Handle multiple listen ports
- [ ] Proper socket cleanup
- [ ] No "Address already in use" errors

**Implementation Status:**
- No code written yet
- Reference: UNIX Network Programming examples

---

### Days 3-4: HTTP Protocol (Dec 28-29)

#### Request Parser
**Status:** 🚧 NOT STARTED  
**Priority:** 🔴 CRITICAL  
**Time Estimate:** 8 hours  
**Blocks:** All HTTP functionality

**Requirements:**
- Parse request line (method, URI, version)
- Parse headers (key: value pairs)
- Handle chunked transfer encoding
- Body reading for POST requests
- Malformed request handling

**Success Criteria:**
- [ ] Parse GET requests
- [ ] Parse POST with body
- [ ] Extract headers correctly
- [ ] Handle keep-alive/close
- [ ] Return 400 for malformed requests

**Implementation Status:**
- No code written yet
- Start with minimal GET parser
- Add POST/body parsing on Day 4

---

#### Response Builder
**Status:** 🚧 NOT STARTED  
**Priority:** 🔴 CRITICAL  
**Time Estimate:** 6 hours  
**Blocks:** All HTTP responses

**Requirements:**
- Status line (HTTP/1.1 200 OK)
- Response headers (Content-Type, Content-Length, etc.)
- Body writing
- Chunked encoding support
- Error responses (404, 500, etc.)

**Success Criteria:**
- [ ] Send valid HTTP/1.1 responses
- [ ] Correct Content-Length headers
- [ ] Proper error responses
- [ ] Works with curl -v

**Implementation Status:**
- No code written yet
- Start with hardcoded "Hello World"
- Add dynamic content on Day 4

---

### Days 5-6: Configuration (Dec 30-31)

#### ConfigParser
**Status:** 🚧 NOT STARTED (tests exist but disabled)  
**Priority:** 🔴 CRITICAL  
**Time Estimate:** 10 hours  
**Blocks:** Multi-server, routing, CGI config

**Requirements:**
- Parse nginx-like config syntax
- Server blocks with listen/root/server_name
- Location blocks with routes/methods
- Error page configuration
- CGI configuration

**Success Criteria:**
- [ ] Parse single server block
- [ ] Parse multiple servers
- [ ] Parse location blocks
- [ ] Extract all directives correctly
- [ ] Handle syntax errors gracefully

**Implementation Status:**
- Test file exists: `unit/test_ConfigParser.cpp.disabled`
- No implementation code
- Need recursive descent parser or simple line-by-line

**Test file:** `unit/test_ConfigParser.cpp.disabled` - Enable when ready

---

#### Port (domain/value_objects)
**Status:** ⚠️ PARTIAL (1/10 tests passing)  
**Priority:** 🔴 CRITICAL  
**Time Estimate:** 1 hour fix  
**Blocks:** ConfigParser, multi-server

**Issues:**
- Default constructor returns 0 instead of 80
- Test expectations don't match implementation
- 9/10 tests failing

**Success Criteria:**
- [ ] Decide: Default port 0 or 80?
- [ ] Update tests OR implementation
- [ ] All 10 tests passing

**Implementation Status:**
- Class exists but needs alignment
- **Quick fix:** 30 minutes to align tests with implementation

**How to test:**
```bash
./bin/test_runner --gtest_filter='PortTest.*'
```

---

### Days 7-8: File Operations (Jan 1-2)

#### Path (domain/value_objects)
**Status:** 🚧 NOT STARTED (tests disabled)  
**Priority:** 🔴 CRITICAL  
**Time Estimate:** 3 hours  
**Blocks:** File operations, security

**Requirements:**
- Path validation (no ../, absolute paths)
- Path normalization
- Component extraction (directory, filename, extension)
- Security checks (prevent directory traversal)

**Success Criteria:**
- [ ] Validate safe paths
- [ ] Reject ../ attacks
- [ ] Extract filename/extension
- [ ] Normalize paths correctly

**Implementation Status:**
- Test file exists: `unit/test_Path.cpp.disabled`
- No implementation code
- Critical for file serving security

**Test file:** `unit/test_Path.cpp.disabled` - Enable when ready

---

#### ErrorCode (domain/value_objects)
**Status:** 🚧 NOT STARTED  
**Priority:** 🔴 CRITICAL  
**Time Estimate:** 3 hours  
**Blocks:** 10+ components (Route, error pages, redirections)

**Missing implementations:**
- Constructor with unsigned int
- `fromString()` method
- `movedPermanently()` method
- `isRedirection()` method
- `getValue()` method
- Destructor

**Success Criteria:**
- [ ] All HTTP status codes (200, 404, 500, etc.)
- [ ] Redirection detection (301, 302, 307, 308)
- [ ] Error page mapping
- [ ] Compiles without errors

**Implementation Status:**
- **HIGHEST PRIORITY:** Unblocks routing, error handling
- Start on Day 1-2 in parallel with socket work
- Use enum or simple class

---

#### Static File Serving (GET)
**Status:** 🚧 NOT STARTED  
**Priority:** 🔴 CRITICAL  
**Time Estimate:** 6 hours  
**Blocks:** GET method evaluation

**Requirements:**
- Open file with fopen()
- Read file contents
- Detect Content-Type (MIME types)
- Send file in HTTP response
- Handle missing files (404)

**Success Criteria:**
- [ ] Serve HTML files
- [ ] Serve CSS/JS files
- [ ] Serve images
- [ ] Correct Content-Type headers
- [ ] 404 for missing files

**Implementation Status:**
- No code written yet
- Simple fread() + response builder
- Hardcode common MIME types

---

#### File Upload (POST)
**Status:** 🚧 NOT STARTED  
**Priority:** 🔴 CRITICAL  
**Time Estimate:** 8 hours  
**Blocks:** POST method evaluation

**Requirements:**
- Parse multipart/form-data
- Extract file boundaries
- Write uploaded file to disk
- Handle large files
- Return success/error response

**Success Criteria:**
- [ ] Upload text files
- [ ] Upload binary files (images)
- [ ] Handle multiple files
- [ ] Proper error responses
- [ ] No corruption

**Implementation Status:**
- No code written yet
- Focus on simple single-file upload first
- Multipart parsing is complex (use simple approach)

---

#### File Deletion (DELETE)
**Status:** 🚧 NOT STARTED  
**Priority:** 🟡 IMPORTANT (can cut if desperate)  
**Time Estimate:** 2 hours  
**Blocks:** DELETE method evaluation

**Requirements:**
- Parse DELETE request
- Validate file path
- Delete file with unlink()
- Return 200/204 on success
- Return 404 if file not found

**Success Criteria:**
- [ ] Delete existing files
- [ ] 404 for missing files
- [ ] Proper response codes
- [ ] Security checks (no ../)

**Implementation Status:**
- No code written yet
- Simple unlink() call
- **Can skip if behind schedule**

---

### Days 9-10: CGI (Jan 3-4)

#### CGI Executor
**Status:** 🚧 NOT STARTED  
**Priority:** 🟡 IMPORTANT (can cut if desperate)  
**Time Estimate:** 12 hours  
**Blocks:** CGI evaluation points

**Requirements:**
- Fork + execve implementation
- Environment variables (PATH_INFO, SCRIPT_FILENAME, etc.)
- Pipe communication (stdin/stdout)
- Timeout handling
- Error handling (script not found, execution failed)

**Success Criteria:**
- [ ] Execute PHP-CGI scripts
- [ ] Pass environment variables correctly
- [ ] Capture script output
- [ ] Send output in HTTP response
- [ ] Handle script errors

**Implementation Status:**
- No code written yet
- Complex: fork(), pipe(), dup2(), execve()
- **Skip if behind on Day 9**

---

#### CgiConfig
**Status:** 🚧 NOT STARTED (blocked by RegexPattern)  
**Priority:** 🟡 IMPORTANT  
**Time Estimate:** 3 hours  
**Blocks:** CGI extension matching

**Blockers:**
- RegexPattern throws exception with empty pattern
- Static initialization crash

**Success Criteria:**
- [ ] Match .php files
- [ ] Configure PHP-CGI path
- [ ] Set CGI timeout
- [ ] Fix RegexPattern issue

**Implementation Status:**
- Fix RegexPattern constructor first (1 hour)
- Then implement CgiConfig

---

## ✅ Completed Components

### HttpMethod (domain/value_objects)
**Status:** ✅ COMPLETE (5/5 tests passing)  
**Priority:** ✅ Done  
**Completion Date:** Before Dec 26

**Tests Passing:**
- ✅ ValidMethods - GET, POST, PUT, DELETE, HEAD, OPTIONS
- ✅ InvalidMethod - Rejects invalid methods
- ✅ CaseInsensitive - Case handling
- ✅ Equality - Comparison operators
- ✅ CopyConstructor - Copy semantics

**How to test:**
```bash
./bin/test_runner --gtest_filter='HttpMethodTest.*'
```

---

## 🟢 Optional Components (Skip if Behind)

These are NOT required for basic evaluation. Cut these if running out of time.

### DirectoryLister
**Status:** 🚧 NOT STARTED (blocked by RegexPattern)  
**Priority:** 🟢 BONUS  
**Cut if behind:** YES

**Why skip:**
- Not mandatory for evaluation
- Auto-index is bonus feature
- Can return 404 instead

---

### Custom Error Pages
**Status:** 🚧 NOT STARTED  
**Priority:** 🟢 BONUS  
**Cut if behind:** YES

**Why skip:**
- Can use default error messages
- Not critical for evaluation
- Takes time to implement

---

### Size (domain/value_objects)
**Status:** 🚧 NOT STARTED  
**Priority:** 🟢 BONUS  
**Cut if behind:** YES

**Why skip:**
- Body size limits are bonus
- Can hardcode limit or skip validation
- Not blocking other features

---

### Uri (domain/value_objects)
**Status:** 🚧 NOT STARTED  
**Priority:** 🟡 IMPORTANT (but can simplify)  
**Cut if behind:** Simplify, don't skip

**Missing:**
- `findAuthorityEnd()` method

**Simplification:**
- Use simple string parsing instead of full URI parsing
- Handle only basic URIs
- Skip complex authority detection

---

## 📊 Overall Progress Dashboard

### By Priority

### By Priority

| Priority | Component | Status | Time | Start Date |
|----------|-----------|--------|------|------------|
| 🔴 CRITICAL | Socket Server (I/O + Sockets) | 🚧 NOT STARTED | 14h | Dec 26-27 |
| 🔴 CRITICAL | HTTP Parser + Response | 🚧 NOT STARTED | 14h | Dec 28-29 |
| 🔴 CRITICAL | ConfigParser | 🚧 NOT STARTED | 10h | Dec 30-31 |
| 🔴 CRITICAL | Port (fix) | ⚠️ PARTIAL | 1h | Dec 26 |
| 🔴 CRITICAL | ErrorCode | 🚧 NOT STARTED | 3h | Dec 26-27 |
| 🔴 CRITICAL | Path | 🚧 NOT STARTED | 3h | Jan 1 |
| 🔴 CRITICAL | Static Files (GET) | 🚧 NOT STARTED | 6h | Jan 1-2 |
| 🔴 CRITICAL | File Upload (POST) | 🚧 NOT STARTED | 8h | Jan 2 |
| 🔴 CRITICAL | Integration Testing | 🚧 NOT STARTED | 20h | Jan 5-6 |
| 🟡 IMPORTANT | File Delete (DELETE) | 🚧 NOT STARTED | 2h | Jan 3 |
| 🟡 IMPORTANT | CGI Executor | 🚧 NOT STARTED | 12h | Jan 3-4 |
| 🟡 IMPORTANT | CgiConfig | 🚧 NOT STARTED | 3h | Jan 3 |
| 🟢 BONUS | DirectoryLister | 🚧 NOT STARTED | - | SKIP |
| 🟢 BONUS | Custom Error Pages | 🚧 NOT STARTED | - | SKIP |
| 🟢 BONUS | Size validation | 🚧 NOT STARTED | - | SKIP |
| ✅ DONE | HttpMethod | ✅ COMPLETE | - | Complete |

### By Day (13-Day Sprint)

| Day | Date | Focus | Components | Hours |
|-----|------|-------|------------|-------|
| 1-2 | Dec 26-27 | Socket Server | I/O multiplexing, sockets, ErrorCode, Port fix | 20h |
| 3-4 | Dec 28-29 | HTTP Protocol | Request parser, response builder | 20h |
| 5-6 | Dec 30-31 | Configuration | ConfigParser, routing | 20h |
| 7-8 | Jan 1-2 | File Operations | Path, GET, POST | 20h |
| 9-10 | Jan 3-4 | CGI + DELETE | CGI executor, CgiConfig, DELETE | 20h |
| 11-12 | Jan 5-6 | Integration | Testing, debugging, Valgrind, Siege | 20h |
| 13 | Jan 7 | Final Polish | Bug fixes, evaluation prep | 10h |

**Total Time Required:** ~130 hours over 13 days = 10 hours/day

### Completion Percentages

| Category | Complete | In Progress | Not Started | % Complete |
|----------|----------|-------------|-------------|------------|
| **Value Objects** | 1 (HttpMethod) | 1 (Port) | 3 (ErrorCode, Path, Size) | **20%** |
| **Core Server** | 0 | 0 | 5 (I/O, sockets, parser, response, client handler) | **0%** |
| **Configuration** | 0 | 0 | 4 (ConfigParser, routing) | **0%** |
| **File Operations** | 0 | 0 | 3 (GET, POST, DELETE) | **0%** |
| **CGI** | 0 | 0 | 2 (executor, config) | **0%** |
| **Overall Mandatory** | - | - | - | **~5%** |

---

## ⚠️ Critical Blockers

### 1. No HTTP Server (Blocks Everything)
**Impact:** Cannot test ANY HTTP functionality  
**Components Blocked:** All HTTP methods, file operations, CGI, evaluation  
**Time to Fix:** 20 hours (Days 1-4)  
**Priority:** 🔴 START NOW

**Without server:**
- ❌ Cannot serve files
- ❌ Cannot handle requests
- ❌ Cannot run CGI
- ❌ Cannot evaluate project
- ❌ 95% of project non-functional

**With server:**
- [ ] Can test all HTTP features
- [ ] Can serve static files
- [ ] Can upload/delete files
- [ ] Can run CGI scripts
- [ ] Can pass evaluation

---

### 2. ErrorCode Not Implemented
**Impact:** Blocks 10+ components  
**Components Blocked:** Route, RouteMatcher, RouteConfigAdapter, error pages, redirections  
**Time to Fix:** 3 hours  
**Priority:** 🔴 HIGH - Start on Day 1

---

### 3. ConfigParser Not Implemented
**Impact:** Cannot use configuration files  
**Components Blocked:** Multi-server, routing, CGI config  
**Time to Fix:** 10 hours  
**Priority:** 🔴 HIGH - Days 5-6

---

### 4. Path Not Implemented
**Impact:** Security risk, cannot serve files safely  
**Components Blocked:** File operations, directory traversal protection  
**Time to Fix:** 3 hours  
**Priority:** 🔴 HIGH - Day 7

---

### 5. RegexPattern Empty Constructor Bug
**Impact:** Program crashes on startup  
**Components Blocked:** CgiConfig, DirectoryLister  
**Time to Fix:** 1 hour  
**Priority:** 🟡 MEDIUM - Day 3 or skip

**Fix:**
```cpp
// In RegexPattern constructor
RegexPattern::RegexPattern(const std::string& pattern) {
    if (pattern.empty()) {
        // Don't throw - allow empty pattern
        return;
    }
    // ... rest of validation
}
```

---

## 🎯 Daily Checklist

### Day 1 (Dec 26) - TODAY
- [ ] Fix Port value object (1 hour)
- [ ] Start ErrorCode implementation (3 hours)
- [ ] Start socket server skeleton (8 hours)
- [ ] Test: `telnet localhost 8080` connects

### Day 2 (Dec 27)
- [ ] Complete socket server with epoll (8 hours)
- [ ] Accept connections, send "Hello" (2 hours)
- [ ] Test: `curl http://localhost:8080` responds

### Day 3 (Dec 28)
- [ ] Implement request parser (8 hours)
- [ ] Parse GET method, URI, headers (6 hours)
- [ ] Test: Parse real HTTP request

### Day 4 (Dec 29)
- [ ] Implement response builder (6 hours)
- [ ] Send proper HTTP responses (4 hours)
- [ ] Test: `curl -v` shows valid HTTP

### Day 5 (Dec 30)
- [ ] Start ConfigParser (10 hours)
- [ ] Parse server blocks
- [ ] Test: Load config file

### Day 6 (Dec 31)
- [ ] Complete ConfigParser (6 hours)
- [ ] Parse location blocks (4 hours)
- [ ] Test: Multi-server config

### Day 7 (Jan 1)
- [ ] Implement Path validation (3 hours)
- [ ] Start static file serving (4 hours)
- [ ] Test: Serve HTML file

### Day 8 (Jan 2)
- [ ] Complete GET method (4 hours)
- [ ] Implement POST upload (8 hours)
- [ ] Test: Upload file via curl

### Day 9 (Jan 3)
- [ ] Implement DELETE method (2 hours)
- [ ] Start CGI executor (8 hours)
- [ ] Test: Delete file via curl

### Day 10 (Jan 4)
- [ ] Complete CGI executor (8 hours)
- [ ] Test PHP-CGI script (2 hours)
- [ ] Test: CGI output in response

### Day 11 (Jan 5)
- [ ] Full integration testing (10 hours)
- [ ] Fix critical bugs
- [ ] Test: All features together

### Day 12 (Jan 6)
- [ ] Run Valgrind, fix leaks (10 hours)
- [ ] Run Siege stress test (2 hours)
- [ ] Test: No crashes, no leaks

### Day 13 (Jan 7)
- [ ] Final bug fixes (6 hours)
- [ ] Prepare evaluation demo (2 hours)
- [ ] Document configuration (2 hours)

---

## 🚨 Evaluation Readiness

### Mandatory Requirements Status

| Requirement | Status | Blocker | ETA |
|-------------|--------|---------|-----|
| Accept connections | ❌ | No server | Day 2 |
| GET static files | ❌ | No server + file handling | Day 8 |
| POST upload files | ❌ | No server + file handling | Day 8 |
| DELETE files | ❌ | No server + file handling | Day 9 |
| Multiple servers (ports) | ❌ | No config parser | Day 6 |
| Configuration file | ❌ | No config parser | Day 6 |
| Error pages (404, 500) | ❌ | No ErrorCode + server | Day 4 |
| Non-blocking I/O | ❌ | No I/O multiplexing | Day 2 |
| CGI execution (PHP) | ❌ | No CGI executor | Day 10 |
| No memory leaks | ❌ | No Valgrind testing | Day 12 |
| Stress test (Siege) | ❌ | No server | Day 12 |
| **Ready for Evaluation** | **❌ NO** | Multiple blockers | **Jan 8** |

### Risk Assessment

**Current Risk Level:** 🔴 **EXTREME**

**Why Extreme:**
- 13 days is VERY tight
- ~95% of project not implemented
- No room for major problems
- Behind schedule = failure

**Mitigation Strategy:**
1. Work 10+ hours/day consistently
2. Cut bonuses immediately if behind
3. Focus on MVP only
4. Manual testing > unit tests
5. Copy proven code patterns

**Fallback Plan (If Behind by Jan 4):**
- [ ] Keep: Socket, HTTP, Config, GET, POST
- ❌ Cut: DELETE, CGI (lose points but might pass)
- ❌ Cut: Custom errors, directory listing (use defaults)

---

## 📋 Quick Status Summary

**What's Working:**
- ✅ HttpMethod value object (5/5 tests passing)
- ✅ Test infrastructure (Google Test, Makefile, CI/CD)

**What's Broken:**
- ❌ Everything else (~95% of project)

**What's Blocking:**
1. **No HTTP server** - Cannot test anything HTTP-related
2. **ErrorCode missing** - Blocks routing and error handling
3. **ConfigParser missing** - Cannot load config files
4. **Path missing** - Security risk for file operations

**What to Do RIGHT NOW:**
```bash
# Hour 1 (Today):
cd src/domain/value_objects/
# 1. Fix Port (30 min)
# 2. Start ErrorCode (30 min)

# Hours 2-20:
cd src/infrastructure/network/
# Build socket server with epoll
```

---

---

## 🔄 How to Enable Components

### Re-enable Disabled Test Files

Once features are implemented:

```bash
cd tests/

# Enable Path tests
mv unit/test_Path.cpp.disabled unit/test_Path.cpp

# Enable ConfigParser tests
mv unit/test_ConfigParser.cpp.disabled unit/test_ConfigParser.cpp

# Enable FileHandler integration tests
mv integration/test_FileHandler_Integration.cpp.disabled integration/test_FileHandler_Integration.cpp
```

### Re-enable Source Files in Makefile

Edit `tests/Makefile` to remove exclusions:

```makefile
# Remove these lines as components are implemented:
! -name 'ErrorCode.cpp' \
! -name 'Uri.cpp' \
! -name 'Route.cpp' \
! -name 'RouteMatcher.cpp' \
! -name 'RouteConfigAdapter.cpp' \
! -name 'CgiConfig.cpp' \
! -name 'DirectoryLister.cpp' \
```

### Rebuild and Verify

```bash
make clean
make
./bin/test_runner
```

---

## 📚 Related Documentation

**Implementation Plan:**
- **[TESTING_STRATEGY.md](TESTING_STRATEGY.md)** - Complete 13-day sprint plan with day-by-day tasks

**Test Scenarios:**
- **[TESTING_SCENARIOS.md](TESTING_SCENARIOS.md)** - Detailed test code examples (for manual testing)

**How-To Guides (Skip if no time):**
- **[GOOGLETEST_INSTALLATION.md](GOOGLETEST_INSTALLATION.md)** - Install Google Test
- **[GOOGLETEST_REFERENCE.md](GOOGLETEST_REFERENCE.md)** - Command-line options
- **[GOOGLETEST_GUIDE.md](GOOGLETEST_GUIDE.md)** - Framework usage
- **[BEGINNER_GUIDE.md](BEGINNER_GUIDE.md)** - Quick start guide
- **[GOOGLETEST_QUICKFIX.md](GOOGLETEST_QUICKFIX.md)** - Common errors

**Status & CI:**
- **[CI_CD.md](CI_CD.md)** - GitHub Actions (ignore for now)
- **[tests/README.md](../README.md)** - Quick overview

---

## 💡 Development Tips

### Speed Over Perfection

**DON'T:**
- ❌ Write comprehensive unit tests (no time)
- ❌ Achieve perfect test coverage (no time)
- ❌ Implement all edge cases (focus basics)
- ❌ Build beautiful architecture (pragmatic only)
- ❌ Add bonus features (mandatory only)

**DO:**
- [ ] Copy proven code examples
- [ ] Hardcode when possible
- [ ] Test manually with curl
- [ ] Fix critical bugs only
- [ ] Get it working first

### Manual Testing Commands

```bash
# Socket test
telnet localhost 8080

# GET test
curl http://localhost:8080/index.html
curl -v http://localhost:8080/test.html

# POST test
curl -X POST -F "file=@test.txt" http://localhost:8080/upload

# DELETE test
curl -X DELETE http://localhost:8080/test.txt

# CGI test
curl http://localhost:8080/test.php

# Multi-server test
curl http://localhost:8080/  # Server 1
curl http://localhost:8081/  # Server 2

# Stress test
siege -c 50 -r 100 http://localhost:8080/

# Memory leak test
valgrind --leak-check=full ./webserv config.conf
```

### Time-Saving Tricks

**1. Use printf debugging (faster than gdb):**
```cpp
printf("DEBUG: fd=%d, bytes=%ld\n", fd, bytes);
fflush(stdout);
```

**2. Hardcode MIME types (don't parse /etc/mime.types):**
```cpp
if (ext == ".html") return "text/html";
if (ext == ".css") return "text/css";
if (ext == ".js") return "text/javascript";
if (ext == ".jpg") return "image/jpeg";
return "application/octet-stream";
```

**3. Copy epoll examples:**
```bash
man epoll
# Copy the example code at the bottom
```

**4. Simplify HTTP parsing:**
```cpp
// Just split on spaces and \r\n
// Don't build perfect parser
std::istringstream iss(request);
std::string method, uri, version;
iss >> method >> uri >> version;
```

---

## 🎯 Success Metrics

### Daily Goals

Each day has clear success criteria:

- **Day 1-2:** `curl localhost:8080` connects and responds
- **Day 3-4:** `curl -v localhost:8080` shows valid HTTP
- **Day 5-6:** Config file loads multiple servers
- **Day 7-8:** Files served via GET, uploaded via POST
- **Day 9-10:** CGI scripts execute, DELETE works
- **Day 11-12:** Valgrind clean, Siege passes
- **Day 13:** Ready for evaluation

### Evaluation Checklist

Print this and check off before Jan 8:

```
MANDATORY FEATURES:
[ ] Server accepts connections (telnet works)
[ ] Serves static files (GET)
[ ] Uploads files (POST)
[ ] Deletes files (DELETE)
[ ] Multiple servers on different ports
[ ] Configuration file parsed correctly
[ ] Error responses (404, 500)
[ ] Non-blocking I/O (epoll/poll)
[ ] CGI execution (at least PHP-CGI)
[ ] No memory leaks (Valgrind clean)
[ ] No crashes under load (Siege test)
[ ] Can handle simultaneous connections

DOCUMENTATION:
[ ] Config file format documented
[ ] README explains how to run
[ ] Example config file works

READY FOR EVALUATION: [ ]
```

---

## 🚨 Emergency Contact

**If you're behind schedule:**

1. **Review TESTING_STRATEGY.md** - Check fallback plans
2. **Cut bonuses immediately** - DELETE, custom errors, directory listing
3. **Simplify implementation** - Hardcode, copy examples
4. **Focus on GET only** - Skip POST/DELETE if desperate
5. **Manual testing only** - No time for unit tests

**If major blocker encountered:**

1. **Check online examples** - Don't solve from scratch
2. **Ask for help** - 42 peers, Stack Overflow
3. **Simplify approach** - Make it work, not perfect
4. **Document workaround** - Explain limitations

---

**Last Updated:** December 26, 2025 (Updated for 13-day sprint)  
**Next Update:** Daily (track progress)  
**Deadline:** January 8, 2026 (13 days remaining)  
**Status:** 🔴 URGENT - START IMMEDIATELY
