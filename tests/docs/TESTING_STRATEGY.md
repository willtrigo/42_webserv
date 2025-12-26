# Testing Strategy for 42_webserver

## Executive Summary

This document defines the **testing strategy** for the 42_webserver project, focusing on:
- Test-driven implementation order based on 42 subject requirements
- Dependency analysis showing which features must be built first
- Layer-by-layer testing approach aligned with Clean Architecture
- Critical path to enable full project evaluation

**⏰ DEADLINE: January 8th, 2026 (13 days from December 26th, 2025)**

**Current Status:** ~5% of mandatory requirements complete. See [TESTING_STATUS.md](TESTING_STATUS.md) for detailed status.

**For HOW-TO guides:** See [GOOGLETEST_INSTALLATION.md](GOOGLETEST_INSTALLATION.md), [GOOGLETEST_REFERENCE.md](GOOGLETEST_REFERENCE.md), [GOOGLETEST_GUIDE.md](GOOGLETEST_GUIDE.md)

---

## ⚡ 13-Day Rapid Implementation Plan

**Reality Check:**
- ⏰ **13 days** until deadline (Jan 8, 2026)
- 📊 **~5% complete** (only HttpMethod done)
- 🎯 **Goal:** Minimum viable server for 42 evaluation
- ⚠️ **Strategy:** Cut all bonuses, focus ONLY on mandatory requirements

**What We Must Build:**
1. [ ] Basic HTTP server (GET, POST, DELETE)
2. [ ] Configuration file parsing
3. [ ] Static file serving
4. [ ] File upload/delete
5. [ ] CGI execution (minimal)
6. [ ] Error handling
7. [ ] Multi-port support

**What We're Cutting:**
- ❌ Extensive testing (use manual tests)
- ❌ Perfect architecture (pragmatic approach)
- ❌ All bonus features
- ❌ Comprehensive error handling
- ❌ Performance optimization

---

## 🎯 Survival Strategy for 13 Days

**Philosophy Shift:**
- ❌ **Not doing:** Full TDD, comprehensive unit tests
- ✅ **Doing instead:** Integration tests + manual verification
- ⚡ **Speed over perfection:** Get it working, then fix issues
- 🎯 **Focus:** Pass 42 evaluation, nothing more

**Why This Approach:**
- **13 days is NOT enough** for full TDD + implementation
- **Must prioritize** working features over perfect tests
- **Can test manually** during evaluation (evaluator runs tests)
- **Time is critical:** Every hour counts

**Testing Strategy:**
1. [ ] **Critical smoke tests** → Does it crash?
2. [ ] **Integration tests** → Does it serve HTTP?
3. [ ] **Manual verification** → Test with curl/browser
4. ❌ **Skip:** Comprehensive unit tests (not enough time)

---

## 📅 Day-by-Day Implementation Plan

### Days 1-2 (Dec 26-27): Foundation + Socket Server

**Goal:** Basic HTTP server accepting connections

**Tasks:**
- [ ] Fix Port value object (1 hour)
- [ ] Implement ErrorCode (3 hours)
- [ ] Basic socket server (non-blocking) (8 hours)
- [ ] I/O multiplexing (poll/epoll) (6 hours)
- [ ] Accept connections, send "Hello World" (2 hours)

**Test:** `curl http://localhost:8080` → Should respond

**Priority:** 🔴 CRITICAL - Nothing works without this

---

### Days 3-4 (Dec 28-29): HTTP Parser + Response

**Goal:** Parse HTTP requests, send proper responses

**Tasks:**
- [ ] Request parser (method, URI, headers) (8 hours)
- [ ] Response builder (status, headers, body) (6 hours)
- [ ] GET method basic implementation (4 hours)
- [ ] Error responses (404, 500) (2 hours)

**Test:** 
```bash
curl http://localhost:8080/test.html  # Should serve file
curl http://localhost:8080/missing   # Should return 404
```

**Priority:** 🔴 CRITICAL - Core HTTP functionality

---

### Days 5-6 (Dec 30-31): Configuration + Routing

**Goal:** Multi-server, location blocks, routing

**Tasks:**
- [ ] ConfigParser (parse nginx-like config) (10 hours)
- [ ] Server configuration (ports, names, roots) (4 hours)
- [ ] Location blocks (routes, methods) (4 hours)
- [ ] Route matching logic (2 hours)

**Test:**
```bash
# Config with multiple servers
server { listen 8080; root /var/www; }
server { listen 8081; root /var/app; }
```

**Priority:** 🔴 CRITICAL - 42 requires multi-server

---

### Days 7-8 (Jan 1-2): File Operations

**Goal:** Static files, uploads, deletion

**Tasks:**
- [ ] Path value object (security checks) (3 hours)
- [ ] Static file serving (GET) (4 hours)
- [ ] Content-Type detection (2 hours)
- [ ] File upload (POST) (6 hours)
- [ ] File deletion (DELETE) (3 hours)
- [ ] Error pages (custom 404/500) (2 hours)

**Test:**
```bash
curl http://localhost:8080/index.html  # GET file
curl -X POST -F "file=@test.txt" http://localhost:8080/upload  # Upload
curl -X DELETE http://localhost:8080/test.txt  # Delete
```

**Priority:** 🔴 CRITICAL - Core functionality

---

### Days 9-10 (Jan 3-4): CGI Execution

**Goal:** Execute PHP-CGI, Python scripts

**Tasks:**
- [ ] CGI configuration (extensions, paths) (3 hours)
- [ ] Fork + execve implementation (6 hours)
- [ ] Environment variables (PATH_INFO, etc.) (3 hours)
- [ ] Pipe communication (stdin/stdout) (4 hours)
- [ ] Timeout handling (2 hours)
- [ ] Error handling (CGI failures) (2 hours)

**Test:**
```bash
# PHP script: <?php echo "Hello CGI"; ?>
curl http://localhost:8080/script.php  # Should execute
```

**Priority:** 🟡 IMPORTANT - Required for full evaluation

---

### Days 11-12 (Jan 5-6): Integration + Debugging

**Goal:** Everything working together, fix bugs

**Tasks:**
- [ ] Integration testing (all features) (4 hours)
- [ ] Memory leak fixes (Valgrind) (4 hours)
- [ ] Edge case handling (4 hours)
- [ ] Concurrent connections testing (3 hours)
- [ ] Config edge cases (2 hours)
- [ ] Error handling polish (3 hours)

**Test:**
```bash
# Stress test
siege -c 10 -r 100 http://localhost:8080/
# Memory check
valgrind --leak-check=full ./webserv
```

**Priority:** 🔴 CRITICAL - Must be stable for evaluation

---

### Day 13 (Jan 7): Final Testing + Documentation

**Goal:** Ready for evaluation (Jan 8)

**Tasks:**
- [ ] Final integration tests (2 hours)
- [ ] Document configuration format (2 hours)
- [ ] Prepare evaluation demo (2 hours)
- [ ] Test with evaluator checklist (3 hours)
- [ ] Fix critical bugs only (3 hours)
- [ ] Buffer time for surprises (4 hours)

**Test:** Full evaluation checklist simulation

**Priority:** 🔴 CRITICAL - Last chance to fix issues

---

## ⚠️ Risk Management

### High-Risk Items (Could fail deadline)

**1. I/O Multiplexing (Day 1-2)**
- **Risk:** Non-blocking I/O is complex
- **Mitigation:** Use proven epoll/poll patterns, copy from examples
- **Fallback:** Blocking I/O (works but slower)

**2. Request Parser (Day 3-4)**
- **Risk:** HTTP parsing has many edge cases
- **Mitigation:** Handle ONLY basic cases (GET/POST/DELETE)
- **Fallback:** Minimal parser, reject complex requests

**3. CGI Execution (Day 9-10)**
- **Risk:** Fork/pipe/env vars are tricky
- **Mitigation:** Test with simple scripts first
- **Fallback:** Hardcode PHP-CGI path, minimal env vars

### Critical Path Dependencies

```
Socket Server (Day 1-2)
    ↓
HTTP Parser (Day 3-4)
    ↓
Config + Routing (Day 5-6)
    ↓
File Operations (Day 7-8)
    ↓
CGI (Day 9-10)
    ↓
Integration (Day 11-12)
    ↓
Final Testing (Day 13)
```

**If behind schedule:**
- Skip CGI → Lose points but might still pass
- Skip DELETE method → Minor point loss
- Skip custom error pages → Use defaults
- Skip directory listing → Not mandatory

---

## ✅ Minimum Viable Product (MVP)

**Absolutely Required (Will Fail Without):**
1. [ ] HTTP server accepting connections
2. [ ] GET method (serve static files)
3. [ ] POST method (file upload)
4. [ ] DELETE method (file deletion)
5. [ ] Configuration file parsing
6. [ ] Multi-server support (different ports)
7. [ ] Error codes (404, 500, etc.)
8. [ ] CGI execution (at least one script working)
9. [ ] No memory leaks (Valgrind clean)
10. [ ] No crashes under load

**Nice to Have (Can Skip if Desperate):**
- ⚠️ Custom error pages (use defaults)
- ⚠️ Directory listing (return 404 instead)
- ⚠️ Cookie/session handling (bonus feature)
- ⚠️ Multiple CGI types (focus on PHP-CGI only)
- ⚠️ Perfect HTTP compliance (handle basics only)

---

## 🧪 Testing Approach (Simplified)

### Smoke Tests (5 minutes each)

**Purpose:** Does it crash?

```bash
# Day 1-2: Socket test
./webserv config.conf
curl http://localhost:8080  # Should respond

# Day 3-4: HTTP test  
curl -v http://localhost:8080/test.html  # Check headers
curl http://localhost:8080/404  # Should return 404

# Day 5-6: Config test
./webserv multi_server.conf  # Should start 2+ servers
curl http://localhost:8080  # Server 1
curl http://localhost:8081  # Server 2

# Day 7-8: File operations
curl http://localhost:8080/file.txt  # GET
curl -X POST -F "file=@test.txt" http://localhost:8080/  # POST
curl -X DELETE http://localhost:8080/file.txt  # DELETE

# Day 9-10: CGI test
curl http://localhost:8080/test.php  # Should execute PHP

# Day 11-12: Stress test
siege -c 50 -r 100 http://localhost:8080/
valgrind --leak-check=full ./webserv config.conf
```

### Integration Tests (30 minutes per day)

**Purpose:** Do features work together?

```bash
# Create test script
#!/bin/bash
set -e  # Exit on error

# Test 1: Basic GET
curl -f http://localhost:8080/index.html

# Test 2: 404 handling
curl -f -X GET http://localhost:8080/missing || echo "OK: 404 works"

# Test 3: File upload
curl -f -X POST -F "file=@test.txt" http://localhost:8080/upload

# Test 4: File delete
curl -f -X DELETE http://localhost:8080/test.txt

# Test 5: CGI
curl -f http://localhost:8080/test.php | grep "expected output"

# Test 6: Multi-server
curl -f http://localhost:8080/
curl -f http://localhost:8081/

echo "✅ All tests passed"
```

### Manual Testing (During development)

**Purpose:** Quick verification while coding

- Use browser for GET requests (visual feedback)
- Use curl for POST/DELETE (command line)
- Use Postman/Insomnia for complex requests
- Check server logs for errors

**No extensive unit tests - TIME IS CRITICAL**

---

## 📋 Component Status & Dependencies

**✅ Complete (1 component):**
- HttpMethod: All functionality working

**🔴 CRITICAL - Start Immediately:**
1. **Port** (1 hour) → Blocks ConfigParser
2. **ErrorCode** (3 hours) → Blocks routing, error handling
3. **Socket Server** (8 hours) → Blocks everything

**🔴 CRITICAL - Days 3-8:**
4. **Request Parser** (8 hours) → HTTP functionality
5. **Response Builder** (6 hours) → HTTP responses
6. **ConfigParser** (10 hours) → Multi-server
7. **Path** (3 hours) → File security
8. **File Operations** (15 hours) → GET/POST/DELETE

**🟡 IMPORTANT - Days 9-10:**
9. **CGI Execution** (18 hours) → Dynamic content

**🟢 SKIP IF DESPERATE:**
- Custom error pages
- Directory listing
- Perfect HTTP compliance
- Multiple CGI types
- Cookies/sessions

---

## 📚 Documentation Reference

**Strategy (this document):**
- 13-day rapid implementation plan
- Day-by-day tasks and tests
- Risk management
- MVP definition

**Test Scenarios:**
- **[TESTING_SCENARIOS.md](TESTING_SCENARIOS.md)** - Detailed test scenarios (use for manual testing)

**How-To Guides:**
- **[GOOGLETEST_INSTALLATION.md](GOOGLETEST_INSTALLATION.md)** - Install Google Test (skip if no time)
- **[GOOGLETEST_REFERENCE.md](GOOGLETEST_REFERENCE.md)** - Command-line options (skip if no time)
- **[GOOGLETEST_GUIDE.md](GOOGLETEST_GUIDE.md)** - Framework usage (skip if no time)
- **[BEGINNER_GUIDE.md](BEGINNER_GUIDE.md)** - Testing fundamentals (skip if no time)
- **[GOOGLETEST_QUICKFIX.md](GOOGLETEST_QUICKFIX.md)** - Installation errors (skip if no time)

**Status & Reference:**
- **[TESTING_STATUS.md](TESTING_STATUS.md)** - What's implemented, what's blocked
- **[CI_CD.md](CI_CD.md)** - GitHub Actions workflows (ignore for now)

---

## 📊 Realistic Timeline Assessment

### What 13 Days Really Means

**Total Available Hours:**
- 13 days × 12 hours/day = **156 hours maximum**
- Assume 10 hours/day realistic = **130 hours**
- Subtract debugging/breaks = **~100 effective hours**

**What We Need:**
| Component | Time Estimate | Priority |
|-----------|--------------|----------|
| Socket Server | 20 hours | 🔴 CRITICAL |
| HTTP Parser/Response | 18 hours | 🔴 CRITICAL |
| Config Parser | 12 hours | 🔴 CRITICAL |
| File Operations | 16 hours | 🔴 CRITICAL |
| CGI | 18 hours | 🟡 IMPORTANT |
| Integration/Debug | 20 hours | 🔴 CRITICAL |
| **TOTAL** | **104 hours** | **Barely possible** |

**Verdict:** ⚠️ **EXTREMELY TIGHT** - No room for major problems

---

## 🚀 What to Do RIGHT NOW

### Hour 1 (Today, Dec 26)

```bash
# 1. Fix Port (30 minutes)
cd tests/unit/
# Update test_Port.cpp OR Port implementation

# 2. Start ErrorCode (30 minutes)
cd src/domain/value_objects/
# Create ErrorCode.hpp, ErrorCode.cpp
# See TESTING_SCENARIOS.md for requirements
```

### Hours 2-20 (Dec 26-27)

**Focus:** Basic socket server

```cpp
// Minimal socket server skeleton
int main() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    // Set non-blocking
    fcntl(server_fd, F_SETFL, O_NONBLOCK);
    
    // Bind to port 8080
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);
    addr.sin_addr.s_addr = INADDR_ANY;
    bind(server_fd, (struct sockaddr*)&addr, sizeof(addr));
    
    listen(server_fd, 128);
    
    // epoll loop
    int epoll_fd = epoll_create1(0);
    struct epoll_event event;
    event.events = EPOLLIN;
    event.data.fd = server_fd;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &event);
    
    while (1) {
        struct epoll_event events[10];
        int n = epoll_wait(epoll_fd, events, 10, -1);
        // Handle events...
    }
}
```

**Test:** `curl http://localhost:8080` should connect

### Hours 21-40 (Dec 28-29)

**Focus:** HTTP parsing and responses

```cpp
// Parse minimal HTTP request
std::string request = read_from_socket(client_fd);
// Extract: "GET /path HTTP/1.1"
std::string method = /* first word */;
std::string path = /* second word */;

// Send minimal response
std::string response = 
    "HTTP/1.1 200 OK\r\n"
    "Content-Type: text/html\r\n"
    "Content-Length: 13\r\n"
    "\r\n"
    "Hello, World!";
write(client_fd, response.c_str(), response.length());
```

**Test:** `curl -v http://localhost:8080` → Should see HTTP response

### Hours 41-65 (Dec 30-31 + Jan 1)

**Focus:** Configuration + file serving

1. Parse config file (10 hours)
2. Implement Path validation (3 hours)
3. Serve static files with fread() (4 hours)
4. Multi-server support (4 hours)

**Test:**
```bash
curl http://localhost:8080/index.html  # Should serve file
curl http://localhost:8081/  # Different server
```

### Hours 66-85 (Jan 2-3)

**Focus:** POST/DELETE + file operations

1. Parse multipart/form-data (8 hours)
2. Write uploaded files (4 hours)
3. Delete files (2 hours)
4. Error handling (6 hours)

**Test:**
```bash
curl -X POST -F "file=@test.txt" http://localhost:8080/upload
curl -X DELETE http://localhost:8080/test.txt
```

### Hours 86-100 (Jan 4-5)

**Focus:** CGI execution

1. Basic CGI setup (4 hours)
2. Fork + execve (6 hours)
3. Environment variables (3 hours)
4. Pipe I/O (5 hours)

**Test:**
```bash
echo '<?php echo "CGI works!"; ?>' > test.php
curl http://localhost:8080/test.php  # Should execute
```

### Hours 101-130 (Jan 6-7)

**Focus:** Integration + bug fixes

1. Run Valgrind, fix leaks (10 hours)
2. Test all features together (6 hours)
3. Fix critical bugs (8 hours)
4. Stress testing (4 hours)
5. Documentation (2 hours)

**Test:**
```bash
valgrind --leak-check=full ./webserv
siege -c 50 -r 100 http://localhost:8080/
```

---

## ⚠️ Emergency Fallback Plan

**If behind schedule on Jan 4:**

### Scenario 1: No CGI by Jan 4
- **Decision:** Skip CGI, focus on GET/POST/DELETE perfection
- **Impact:** Will lose CGI points, but might pass
- **Extra time:** +18 hours for polish

### Scenario 2: Server not stable by Jan 3  
- **Decision:** CUT DELETE method, minimal POST
- **Focus:** GET working perfectly + config
- **Impact:** Partial implementation, risky

### Scenario 3: Major blocker (parser issues, architecture problems)
- **Decision:** SIMPLIFY EVERYTHING
  - Single server only (skip multi-server)
  - Fixed config (no parsing)
  - Basic GET only
- **Impact:** Will likely fail evaluation
- **Last resort:** Better than nothing

---

## 🎯 Daily Success Criteria

### Day 1 (Dec 26): Socket Server
[ ] Server accepts connections on port 8080  
[ ] Can telnet to server without crash  
[ ] epoll loop working

### Day 2 (Dec 27): HTTP Basics
[ ] Parse "GET /path HTTP/1.1"  
[ ] Send "HTTP/1.1 200 OK" response  
[ ] curl shows valid HTTP response

### Day 3 (Dec 28): Request/Response
[ ] Parse headers  
[ ] Send headers + body  
[ ] 404 error handling

### Day 4 (Dec 29): Polish HTTP
[ ] Handle multiple requests  
[ ] Connection: close/keep-alive  
[ ] No crashes with malformed requests

### Day 5 (Dec 30): Config Parser
[ ] Parse server block  
[ ] Parse listen directive  
[ ] Parse root directive

### Day 6 (Dec 31): Config Complete
[ ] Parse location blocks  
[ ] Multiple servers  
[ ] Route matching

### Day 7 (Jan 1): File Operations Start
[ ] Path validation working  
[ ] Serve static HTML file  
[ ] Content-Type headers

### Day 8 (Jan 2): GET Complete
[ ] Serve any file type  
[ ] Handle missing files (404)  
[ ] Handle permissions errors

### Day 9 (Jan 3): POST Upload
[ ] Parse multipart/form-data  
[ ] Write uploaded file  
[ ] Success/error responses

### Day 10 (Jan 4): DELETE + CGI Start
[ ] DELETE method working  
[ ] CGI fork() + execve() skeleton  
[ ] Basic PHP execution

### Day 11 (Jan 5): CGI Complete
[ ] Environment variables correct  
[ ] Pipe I/O working  
[ ] CGI output in HTTP response

### Day 12 (Jan 6): Integration
[ ] All features working together  
[ ] Valgrind clean (no leaks)  
[ ] No crashes under siege

### Day 13 (Jan 7): Final Polish
[ ] All evaluation tests pass  
[ ] Config file documented  
[ ] Demo prepared

---

## 📋 Quick Reference Card

**Print this and tape to monitor:**

```
═══════════════════════════════════════════════
        42_WEBSERVER - 13 DAY SPRINT
═══════════════════════════════════════════════

DEADLINE: January 8, 2026 (13 days)
START: December 26, 2025

DAILY GOAL: 10 hours focused work

PHASE 1 (Day 1-2): Socket + epoll
  → Test: curl connects

PHASE 2 (Day 3-4): HTTP parser
  → Test: curl gets response

PHASE 3 (Day 5-6): Config parser
  → Test: multi-server works

PHASE 4 (Day 7-8): GET method
  → Test: serve static files

PHASE 5 (Day 9): POST/DELETE
  → Test: upload + delete files

PHASE 6 (Day 10-11): CGI
  → Test: PHP script executes

PHASE 7 (Day 12): Valgrind + Siege
  → Test: no leaks, no crashes

PHASE 8 (Day 13): Final check
  → Test: evaluation ready

CRITICAL: If behind, CUT:
  ❌ CGI (lose points)
  ❌ DELETE (minor loss)
  ❌ Custom errors (use defaults)

MUST HAVE:
  [ ] Socket server
  [ ] HTTP GET
  [ ] Config parser
  [ ] Multi-server
  [ ] POST upload
  [ ] No leaks

═══════════════════════════════════════════════
```

---

## 💡 Development Tips for Speed

### 1. Copy Working Code
```bash
# Don't reinvent the wheel
# Copy epoll examples from:
man epoll
# HTTP parser examples:
# https://github.com/nodejs/http-parser
```

### 2. Use Minimal Error Handling
```cpp
// DON'T do this (too slow):
if (!validate_every_edge_case()) {
    log_detailed_error();
    throw CustomException();
}

// DO this instead:
if (fd < 0) return -1;  // Fail fast
```

### 3. Hardcode When Possible
```cpp
// DON'T:
MimeTypeDetector detector;
detector.loadFromFile("/etc/mime.types");
std::string type = detector.detect(filename);

// DO:
std::string getContentType(std::string ext) {
    if (ext == ".html") return "text/html";
    if (ext == ".css") return "text/css";
    if (ext == ".js") return "text/javascript";
    return "application/octet-stream";
}
```

### 4. Test with Simple Tools
```bash
# Faster than writing test files:
curl -v http://localhost:8080/test.html
echo "test" | nc localhost 8080
telnet localhost 8080
```

### 5. Use printf Debugging
```cpp
// Faster than setting up gdb every time:
printf("DEBUG: fd=%d, bytes=%ld\n", fd, bytes);
fflush(stdout);
```

---

## 🚨 Critical Warnings

### 1. Scope Creep Killers
❌ "Let's make perfect error messages" → NO  
❌ "Let's support HTTP/2" → NO  
❌ "Let's add WebSocket support" → NO  
❌ "Let's make beautiful logs" → NO  

✅ "Does it work?" → YES, move on

### 2. Perfectionism Traps
❌ 100% test coverage → Skip tests  
❌ Beautiful architecture → Pragmatic only  
❌ Comprehensive documentation → Minimal  
❌ All edge cases → Handle basics

### 3. Time Wasters
❌ Debugging tiny memory leaks → Fix big ones first  
❌ Optimizing performance → Get it working first  
❌ Refactoring code → Only if blocking progress  
❌ Adding features → Stick to MVP

---

## 📚 Updated Documentation Structure

**For Implementation (Use These):**
1. **TESTING_STRATEGY.md** (this file) → Day-by-day plan
2. **TESTING_SCENARIOS.md** → Copy test code for manual verification
3. **TESTING_STATUS.md** → Track what's done

**For Later (Skip Now):**
- GOOGLETEST_* files → No time for unit tests
- BEGINNER_GUIDE.md → Already know basics
- CI_CD.md → GitHub Actions not priority

---

## 🎯 Final Survival Checklist

**Before Starting (Today):**
- [ ] Read this entire document
- [ ] Print quick reference card
- [ ] Set up work environment (editor, terminal, curl, Valgrind)
- [ ] Clone/review HTTP server examples
- [ ] Review 42 evaluation sheet

**Every Day:**
- [ ] Check daily success criteria
- [ ] Manual smoke test (5 min)
- [ ] Commit working code
- [ ] Document blockers
- [ ] Sleep at least 6 hours (avoid burnout)

**Every 3 Days:**
- [ ] Full integration test (30 min)
- [ ] Valgrind check
- [ ] Review time remaining
- [ ] Adjust plan if behind

**Jan 7 (Day Before):**
- [ ] Full evaluation simulation
- [ ] Fix critical bugs only
- [ ] Prepare demo
- [ ] Get good sleep

---

**Last Updated:** December 26, 2025  
**Deadline:** January 8, 2026 (13 days)  
**Status:** URGENT - START IMMEDIATELY  
**Version:** 3.0 (Emergency 13-day sprint plan)

---

**REMEMBER:** Perfect is the enemy of done. Get it working, then fix critical issues. 13 days is VERY tight - every hour counts!

**Week 2-3:** Core Server (biggest effort)
- I/O multiplexing (poll/select/epoll)
- Socket management (non-blocking)
- Request parser (HTTP)
- Response builder (HTTP)
- Client handler (lifecycle)

**Week 4:** Configuration & Files
- ConfigParser → multi-server configs
- Route/RouteMatcher → request routing
- FileHandler → GET/POST/DELETE
- Static file serving

**Week 5:** CGI & Testing
- CGI executor (fork/execve)
- PHP-CGI integration
- Valgrind (memory leaks)
- Siege (stress testing)

**Result:** Minimum viable server for evaluation ✅

---

## 📚 Documentation Reference

**Strategy (this document):**
- Test-driven implementation order
- Dependency analysis
- 42 requirements mapping

**Test Scenarios:**
- **[TESTING_SCENARIOS.md](TESTING_SCENARIOS.md)** - Detailed test scenarios for all components

**How-To Guides:**
- **[GOOGLETEST_INSTALLATION.md](GOOGLETEST_INSTALLATION.md)** - Install Google Test 1.8.1
- **[GOOGLETEST_REFERENCE.md](GOOGLETEST_REFERENCE.md)** - Command-line options and output
- **[GOOGLETEST_GUIDE.md](GOOGLETEST_GUIDE.md)** - Framework usage (fixtures, mocks, assertions)
- **[BEGINNER_GUIDE.md](BEGINNER_GUIDE.md)** - Testing fundamentals
- **[GOOGLETEST_QUICKFIX.md](GOOGLETEST_QUICKFIX.md)** - Common installation errors

**Status & Reference:**
- **[TESTING_STATUS.md](TESTING_STATUS.md)** - What's implemented, what's blocked
- **[CI_CD.md](CI_CD.md)** - GitHub Actions workflows
- **[tests/README.md](../README.md)** - Quick overview with badges

---

## 📊 Success Metrics

### Coverage Targets
- **Domain Layer**: 90%+ (pure logic, no dependencies)
- **Infrastructure Layer**: 75%+ (some OS-dependent code)
- **Presentation Layer**: 70%+ (UI interactions)
- **Overall**: 80%+ before evaluation

### Test Execution Time
- **Unit tests**: < 1 second (value objects)
- **Integration tests**: < 10 seconds (file I/O, config)
- **Full suite**: < 15 seconds (including mocks)

### Quality Gates
- [ ] All tests passing in CI/CD
- [ ] Zero memory leaks (Valgrind)
- [ ] 99.5% availability (Siege stress test)
- [ ] No crashes under load
- [ ] Concurrent connections handled

---

## 💡 Testing Best Practices

**Write tests that:**
- [ ] **Fail first** → Implement → Pass (TDD)
- [ ] **Are independent** → No shared state
- [ ] **Run fast** → < 1ms per unit test
- [ ] **Are repeatable** → Same result every time
- [ ] **Are self-validating** → Pass/fail, no manual checks

**Remember:**
- Tests are **documentation** of expected behavior
- Tests enable **confident refactoring**
- **Don't test implementation details** → Test behavior
- **Mock external dependencies** → Keep tests fast
- **Integration tests verify** → Components work together

**For Google Test patterns, see [GOOGLETEST_GUIDE.md](GOOGLETEST_GUIDE.md) (AAA pattern, fixtures, assertions, mocks, etc.)**

---

## 🚀 Next Steps

**Current State:**
- ✅ Test infrastructure complete (Google Test 1.8.1, CI/CD)
- ✅ HttpMethod fully tested (5/5 passing)
- ⚠️ Port partially tested (1/10 passing)
- 🚧 ~95% of features not implemented yet

**Your Next Action:**

```bash
# 1. Run existing tests
cd tests/
make
./bin/test_runner

# 2. Implement ErrorCode (highest impact!)
# Create: src/domain/value_objects/ErrorCode.cpp
# Write tests: tests/unit/test_ErrorCode.cpp

# 3. See tests guide what to implement next
# Tests reveal dependencies and implementation order
```

**Implementation Priority:**
1. 🔴 **ErrorCode** → Unblocks 10+ components (1-2 days)
2. 🔴 **Core Server** → Enables ALL HTTP testing (2-3 weeks)
3. 🟡 **ConfigParser** → Multi-server configs (tests ready!)
4. 🟡 **File Operations** → GET/POST/DELETE (1 week)
5. 🟢 **CGI** → Dynamic content (3-5 days)

**Timeline to Evaluation:**
- Minimum: 3-4 weeks (focused implementation)
- Realistic: 5-6 weeks (with testing/debugging)
- Comfortable: 7-8 weeks (with polish/bonuses)

**Remember:** Tests show you what to build next! Follow the dependencies in this document.

---

**Last Updated:** December 26, 2025  
**Document Owner:** 42_webserver Team  
**Version:** 2.0 (Merged strategy with implementation priority)
