# Two-Person Sprint Strategy - 13 Days to Evaluation

**Team:** Dande (Foundation Expert) + Bira (TDD Specialist)  
**Deadline:** January 8, 2026  
**Start Date:** December 26, 2025  
**Strategy:** Parallel development with strategic pairing

---

## ⚠️ THE BRUTAL TRUTH

### Without This Strategy

If you build without coordination, just diving in and "figuring it out":

| Metric | Value | Reality Check |
|--------|-------|---------------|
| **Hours needed** | **273 hours** | 21 hours/day for 13 days = IMPOSSIBLE |
| **Success probability** | **40%** | More likely to fail than succeed |
| **Burnout risk** | **100%** | You'll hate each other by Day 5 |
| **Integration time** | **3-4 days** | Big-bang merge = disaster |
| **Wasted effort** | **30-40%** | Duplicate work, incompatible code |

**Translation:** You'll work yourself to death and probably still fail the evaluation.

---

### With This Strategy

If you follow this plan (even imperfectly):

| Metric | Value | Reality Check |
|--------|-------|---------------|
| **Hours needed** | **104 hours total** | 71h per person = 5.5h/day = ACHIEVABLE |
| **Reduction** | **38% less work** | 169 hours saved vs chaos |
| **Success probability** | **85%** | You'll probably pass |
| **Burnout risk** | **Low** | Sustainable pace, built-in breaks |
| **Integration time** | **Continuous** | Small merges every 2 days |
| **Wasted effort** | **< 10%** | Interfaces prevent duplicate work |

**Translation:** You can sleep, stay sane, and actually pass this thing.

---

## 🎯 GUT-LEVEL DETERMINATION

### This Is Not Optional

You have **13 days**. That's **312 hours** of calendar time for two people.

**If you work without strategy:**
- 273 hours needed ÷ 2 people = 136.5 hours each
- 136.5 hours ÷ 13 days = **10.5 hours per day**
- For 13 days straight
- With no plan
- With constant integration conflicts
- With duplicate work
- **Success rate: 40%**

**You will fail.**

**If you follow this strategy:**
- 104 hours needed ÷ 2 people = 52 hours each (base work)
- 54 hours together (pairing/integration) ÷ 2 = 27 hours each
- Total: 52 + 27 = 79 hours each (we budgeted 71, so 8 hours buffer built in)
- 79 hours ÷ 13 days = **6 hours per day**
- With clear tasks every day
- With daily syncs to stay aligned
- With continuous integration (no big-bang)
- **Success rate: 85%**

**You will probably pass.**

---

## 🛡️ SAFETY NETS (Because Plans Never Go Perfectly)

### Reality: You Won't Follow This Perfectly

**We know:**
- Some tasks will take longer than estimated
- You'll have bad days
- Integration might have surprises
- Real life happens

**So we built in safety:**

#### 1. Buffer Hours (Built into every day)
- Days 1-8: Each day has 1 hour unscheduled buffer
- Days 11-13: Integration days have 2-3 hour buffers
- **Total buffer: 18 hours across the project**

#### 2. Escape Hatches (If things go wrong)
- **Day 3 behind?** → Simplify epoll (use poll/select instead), reduce error handling
- **Day 5 behind?** → Use simple config (no regex), reduce location block complexity
- **Day 7 behind?** → Simplify MIME types (just text/html + octet-stream)
- **Day 9 behind?** → Minimal CGI (just PHP echo script, no complex features)

#### 3. Daily Reality Checks
At the end of EVERY day, ask:
- [ ] Did we complete 80% of today's tasks?
- [ ] If not, what do we cut tomorrow?
- [ ] Are we on track for the deadline?

**If "No" two days in a row → Activate escape hatch**

#### 4. Mandatory vs Bonus (According to 42 Subject)

**⚠️ MANDATORY (Cannot drop without getting 0):**
- [ ] **HTTP/1.1 server** - Must parse and respond correctly
- [ ] **GET method** - Serve static files
- [ ] **POST method** - Upload files (handle body)
- [ ] **DELETE method** - Remove resources
- [ ] **Configuration file** - Parse nginx-like config
- [ ] **Multiple ports** - Listen on different ports
- [ ] **Multiple servers** - Different server blocks (host:port)
- [ ] **Routes with rules** - Location blocks, allowed methods
- [ ] **Default error pages** - 404, 403, 500 (can be simple)
- [ ] **Non-blocking I/O** - Must use select/poll/epoll/kqueue
- [ ] **No crashes** - Cannot segfault under any test
- [ ] **No leaks** - Valgrind must show 0 bytes lost
- [ ] **Client body size limit** - Configurable max upload size

**✨ BONUS (Can drop if behind):**
- [ ] CGI execution (usually bonus, but check your subject!)
- [ ] Cookies/Session management
- [ ] Multiple CGI support
- [ ] Custom error pages (beyond basic HTML)
- [ ] File upload chunking/progress
- [ ] Range requests
- [ ] URL rewriting
- [ ] Directory listing auto-generation

**🚨 WHAT YOU CAN SIMPLIFY (not drop):**
- **URI parsing** → Just handle simple paths, no query strings
- **Config parser** → No regex patterns, just exact matches
- **MIME types** → Just 5 types (html, css, js, jpg, default)
- **Error pages** → Hardcoded HTML strings (no file reads)
- **Routing** → Exact match only (no prefix matching complexity)
- **File operations** → No directory indexes, just direct file paths

**❌ WHAT YOU ABSOLUTELY CANNOT DROP:**
1. GET (serving files from configured root)
2. POST (receiving and storing uploaded files)
3. DELETE (removing files)
4. Config file with multiple servers
5. Non-blocking I/O (epoll/poll/select)
6. Multiple ports listening simultaneously
7. HTTP status codes (200, 404, 403, 500, 405)
8. Request parsing (method, URI, headers, body)
9. Response generation (status line, headers, body)
10. Clean shutdown (no leaks, no crashes)

---

## 🚨 COMMITMENT REQUIRED

### Before You Start, Both Agree To:

- [ ] **I will show up every day** - No exceptions unless emergency
- [ ] **I will do my 5-6 hours** - Consistently, not 10h one day, 0h the next
- [ ] **I will attend daily syncs** - 30 min/day is non-negotiable
- [ ] **I will ask for help when stuck** - After 30 min of being blocked
- [ ] **I will code review within 2 hours** - Partner is waiting on me
- [ ] **I will merge small, merge often** - No 3-day branches
- [ ] **I will pair on complex parts** - CGI and integration together
- [ ] **I will trust the strategy** - Even when I want to "just code"
- [ ] **I will cut features if behind** - Pride < Passing
- [ ] **I will prioritize mandatory over bonus** - Grade 0 if mandatory missing
- [ ] **I will read the subject PDF** - Before starting and before evaluation
- [ ] **I will not quit on my partner** - We're in this together

**⚠️ CRITICAL: Before signing, BOTH read the subject PDF together (30 min)**
- Identify every mandatory requirement
- Mark which features are bonus
- Agree on what cannot be dropped
- Write the list here:

**MANDATORY features we MUST implement:**
1. _________________________________
2. _________________________________
3. _________________________________
4. _________________________________
5. _________________________________

**Sign (literally write your names):**

- **Dande:** _____________________ Date: _____
- **Bira:** _____________________ Date: _____

---

## 💪 WHY THIS WILL WORK

### The Math Is On Your Side

**Solo developer (YOU without partner):**
- 104 hours ÷ 1 person = 104 hours
- 104 hours ÷ 13 days = 8 hours/day
- Success rate: 60% (possible but exhausting)

**Chaotic two-person (no strategy):**
- 273 hours ÷ 2 people = 136.5 hours each
- 136.5 hours ÷ 13 days = 10.5 hours/day
- Success rate: 40% (probably fail due to integration hell)

**Strategic two-person (THIS PLAN):**
- 104 hours ÷ 2 people = 52 hours each
- 54 hours together ÷ 2 = 27 hours each
- Total: 79 hours each
- 79 hours ÷ 13 days = 6 hours/day
- Success rate: 85% (coordination bonus > overhead)

**The difference:** Interfaces + daily syncs + small merges = 45% success rate increase

---

## 🎯 Core Principles

1. **Define interfaces first** - No coding until contracts are clear
2. **Daily 30-min syncs** - Non-negotiable, keeps alignment
3. **Small, frequent merges** - No big-bang integration
4. **Mock when blocked** - Never wait idle for other's code
5. **Pair on complex parts** - CGI and integration together
6. **Trust the process** - Strategy beats chaos, even if imperfect
7. **Cut features, not quality** - 80% working > 100% broken
8. **Ask for help after 30 min** - Stuck = waste of time
9. **Reality check daily** - Behind schedule? Adjust immediately
10. **Finish together** - Integration is a team sport

---

## �️ SACRED RULES - Never Break the Build

### Rule 1: Test Before Commit (MANDATORY)

**EVERY time before `git commit`:**

```bash
# Step 1: Build main project
cd /path/to/42_webserver
make clean && make
# ✅ Must compile without errors

# Step 2: Build tests
cd tests
make clean && make
# ✅ Must compile without errors

# Step 3: Run tests
./bin/test_runner
# ✅ Must pass (or at least not crash)

# Step 4: ONLY NOW can you commit
git add .
git commit -m "feat: your changes"
git push
```

**If ANY step fails → FIX IT before committing!**

### Rule 2: Never Commit Directly to Main

**Always use feature branches:**

```bash
# ❌ WRONG
git checkout main
git commit -m "stuff"  # DON'T DO THIS

# ✅ RIGHT
git checkout -b 6-feat/your-feature
git commit -m "feat: implement feature"
git push -u origin 6-feat/your-feature
# Then create PR
```

### Rule 3: Verify Compilation Before PR Merge

**Before merging ANY PR:**

```bash
# 1. Check out the feature branch
git checkout 6-feat/your-feature

# 2. Merge main into your branch (test integration)
git merge main

# 3. Resolve conflicts if any

# 4. Test EVERYTHING
make clean && make  # Main project
cd tests && make clean && make && ./bin/test_runner

# 5. If all passes → NOW merge to main
git checkout main
git merge 6-feat/your-feature
git push
```

### Rule 4: If You Break Main - Fix Immediately

**If someone can't build after your merge:**

```bash
# Option 1: Quick fix (< 15 minutes)
git checkout main
# fix the issue
make clean && make  # verify
cd tests && make && ./bin/test_runner  # verify
git commit -m "fix: broken build"
git push

# Option 2: Revert (if fix takes > 15 minutes)
git checkout main
git revert HEAD  # undo your last commit
git push
# Now fix on your branch, test thoroughly, re-merge later
```

### Rule 5: Communicate Breakage Immediately

**If your code breaks someone else's work:**

```bash
# In your team chat:
"🚨 BROKEN BUILD - I'm fixing it now. 
Don't pull from main for the next 15 minutes.
Issue: [describe what's broken]
ETA: [time when fixed]"
```

### Rule 6: Daily Build Health Check

**Every morning before starting work:**

```bash
git checkout main
git pull
make clean && make
cd tests && make && ./bin/test_runner

# ✅ If passes → start your day
# ❌ If fails → WHO BROKE IT? Fix before proceeding
```

---

## 🎯 Core Principles

**Time:** 2 hours together before splitting

### Morning: Interface Definition Session (1 hour)

**Both together - Define all interfaces:**

```cpp
// IServer.hpp
class IServer {
public:
    virtual ~IServer() {}
    virtual void start() = 0;
    virtual void stop() = 0;
    virtual void addListener(int port) = 0;
};

// IRequestParser.hpp
class IRequestParser {
public:
    virtual ~IRequestParser() {}
    virtual HttpRequest parse(const std::string& raw) = 0;
    virtual bool isComplete(const std::string& raw) = 0;
};

// IResponseBuilder.hpp
class IResponseBuilder {
public:
    virtual ~IResponseBuilder() {}
    virtual std::string build(int status, const std::string& body) = 0;
    virtual void addHeader(const std::string& key, const std::string& value) = 0;
};

// IConfigParser.hpp
class IConfigParser {
public:
    virtual ~IConfigParser() {}
    virtual Config parse(const std::string& filename) = 0;
    virtual std::vector<ServerConfig> getServers() = 0;
};

// IRouter.hpp
class IRouter {
public:
    virtual ~IRouter() {}
    virtual Route match(const std::string& uri, const std::string& method) = 0;
    virtual bool hasRoute(const std::string& uri) = 0;
};

// IFileHandler.hpp
class IFileHandler {
public:
    virtual ~IFileHandler() {}
    virtual std::string read(const std::string& path) = 0;
    virtual bool write(const std::string& path, const std::string& content) = 0;
    virtual bool remove(const std::string& path) = 0;
};
```

**Deliverables:**
- [ ] All interface files created in `src/application/ports/`
- [ ] Both understand each interface
- [ ] Documented in comments

---

### Mid-Morning: Architecture Overview (30 min)

**Dande teaches, Bira takes notes:**

**Draw this diagram together:**
```
┌─────────────────────────────────────┐
│  Presentation (CLI)                  │
│  - main.cpp                          │
│  - Argument parsing                  │
└──────────────┬──────────────────────┘
               │
┌──────────────▼──────────────────────┐
│  Application Layer (Ports)           │
│  - IServer, IParser, IRouter         │
│  - Interfaces only, no logic         │
└──────────────┬──────────────────────┘
        ┌──────┴──────┐
┌───────▼─────┐  ┌────▼──────────────┐
│  Domain      │  │  Infrastructure   │
│  - ErrorCode │  │  - Server         │
│  - Path      │  │  - Parser         │
│  - Port      │  │  - ConfigParser   │
│  - HttpMethod│  │  - FileHandler    │
└──────────────┘  └───────────────────┘
```

**Key points Dande explains:**
- Why Clean Architecture matters
- Where each component lives
- How they interact
- Critical dependencies (ErrorCode blocks routing)

**Deliverables:**
- [ ] Diagram saved as `docs/ARCHITECTURE.md`
- [ ] Bira understands component relationships
- [ ] Questions answered

---

### Late Morning: Branch Strategy & Tools (30 min)

**Set up git workflow:**

```bash
# Both create their working branches
git checkout -b 6-feat/value-objects-dande
git push -u origin 6-feat/value-objects-dande

git checkout -b 7-feat/value-objects-bira
git push -u origin 7-feat/value-objects-bira

# Set up test environment
cd tests/
make clean && make
./bin/test_runner  # Verify current tests pass
```

**GitHub Project Board setup:**
- [ ] Create columns: TODO, In Progress (Dande), In Progress (Bira), Review, Done
- [ ] Add all tasks as issues
- [ ] Assign issues to team members

**Communication tools:**
- [ ] Daily standup time agreed: 7:00 PM (30 min)
- [ ] Emergency contact method: Discord
- [ ] Code review turnaround: < 2 hours

---

## 🚀 Days 1-2: Value Objects (Maximum Parallelism)

**Goal:** Complete all value objects - no dependencies, pure parallel work

**Daily sync:** 7:00 PM (30 min) + 1:00 PM checkpoint (15 min)
**🚨 REALITY CHECK AT END OF DAY:**
- [ ] Did we define all interfaces? (MANDATORY)
- [ ] Did Dande complete ErrorCode? (Can slip to Day 2)
- [ ] Did Bira complete Path? (MANDATORY for Day 2)
- [ ] Are we comfortable with the workflow? (If not, adjust Day 2)


---

### Day 1 - December 26 (Thursday)

#### Dande's Tasks (6 hours)

**Branch:** `6-feat/value-objects-dande`

**Morning (3 hours):**
- [ ] **ErrorCode implementation** (2.5h)
  - [x] Create `src/domain/value_objects/ErrorCode.hpp`
  - [x] Create `src/domain/value_objects/ErrorCode.cpp`
  - [x] Constructor with unsigned int
  - [x] `fromString()` method
  - [x] `movedPermanently()` method
  - [x] `isRedirection()` method
  - [x] `getValue()` method
  - [x] All HTTP status codes (200, 404, 500, 301, 302, etc.)
  - [x] Basic validation
  - [ ] Compile successfully
  - [ ] **NO TESTS** - focus on implementation speed

- [ ] **Documentation** (0.5h)
  - [ ] Write component purpose
  - [ ] Document all public methods
  - [ ] Note what ErrorCode unblocks (routing, error pages)

**Afternoon (3 hours):**
- [ ] **Port fix** (1h)
  - [ ] Review `test_Port.cpp` failures
  - [ ] Decide: Default port 0 or 80?
  - [ ] Update implementation to match tests
  - [ ] Run tests: `./bin/test_runner --gtest_filter='PortTest.*'`
  - [ ] All 10 tests passing

- [ ] **Uri basics** (2h)
  - [ ] Implement `findAuthorityEnd()` method
  - [ ] Basic URI parsing logic
  - [ ] Component extraction (scheme, host, port, path)
  - [ ] Compile successfully

**Evening:**
- [ ] **BUILD HEALTH CHECK** (MANDATORY - 5 min)
  - [ ] `make clean && make` in root (must compile)
  - [ ] `cd tests && make && ./bin/test_runner` (must pass)
  - [ ] **ONLY commit if both pass**
- [ ] Commit and push all changes
- [ ] Create draft PR: "feat: Implement ErrorCode, fix Port, add Uri basics"
- [ ] Tag Bira for review
- [ ] Update project board

**Expected output:** ErrorCode complete, Port tests passing, Uri compiling

---

#### Bira's Tasks (6 hours)

**Branch:** `7-feat/value-objects-bira`

**Morning (3 hours):**
- [ ] **Path implementation with TDD** (3h)
  - [x] Read `tests/unit/test_Path.cpp.disabled`
  - [x] Enable test file: `mv test_Path.cpp.disabled test_Path.cpp`
  - [x] Update Makefile to include Path tests
  - [x] Create `src/domain/value_objects/Path.hpp`
  - [x] Create `src/domain/value_objects/Path.cpp`
  - [x] **TDD Approach:**
    - [x] Run tests (all fail) ✅
    - [x] Implement path validation (reject `../`)
    - [x] Run tests (some pass)
    - [x] Implement path normalization
    - [x] Run tests (more pass)
    - [x] Implement component extraction (directory, filename, extension)
    - [x] Run tests (all pass) ✅
  - [x] Security focus: Directory traversal protection
  - [x] All Path tests passing

**Afternoon (3 hours):**
- [x] **Size implementation with TDD** (1h)
  - [x] Create `tests/unit/test_Size.cpp`
  - [x] Write tests first:
    - [x] `ValidSizeBytes` - "1024" → 1024
    - [x] `ValidSizeKilobytes` - "1K" → 1024
    - [x] `ValidSizeMegabytes` - "1M" → 1048576
    - [x] `InvalidFormat` - "abc" → throws
    - [x] `NegativeSize` - "-100" → throws
  - [x] Implement `src/domain/value_objects/Size.hpp`
  - [x] Implement `src/domain/value_objects/Size.cpp`
  - [x] All Size tests passing

- [x] **Mock infrastructure setup** (2h)
  - [x] Create `tests/mocks/MockServer.hpp`
  - [x] Create `tests/mocks/MockRequestParser.hpp`
  - [x] Create `tests/mocks/MockResponseBuilder.hpp`
  - [x] Document mock usage patterns
  - [x] Add to Makefile

**Evening:**
- [x] **BUILD HEALTH CHECK** (MANDATORY - 5 min)
  - [x] `make clean && make` in root (must compile)
  - [x] `cd tests && make && ./bin/test_runner` (must pass)
  - [x] **ONLY commit if both pass**
- [x] Commit and push all changes
- [x] Create draft PR: "feat: Implement Path and Size with TDD, add mock infrastructure"
- [x] Tag Dande for review
- [x] Update project board

**Expected output:** Path tests passing, Size tests passing, Mocks ready for use

---

### Day 2 - December 27 (Friday)

#### Dande's Tasks (5 hours + 1h merge)

**Morning (3 hours):**
- [ ] **Code review Bira's PR** (1h)
  - [ ] Review Path implementation
  - [ ] Learn TDD patterns from Bira's code
  - [ ] Suggest improvements (if any)
  - [ ] Approve when satisfied

- [ ] **Complete Uri implementation** (2h)
  - [ ] Full URI parsing
  - [ ] Query string extraction
  - [ ] Fragment handling
  - [ ] Validation logic
  - [ ] Edge cases (empty, malformed)
  - [ ] Compile and basic manual testing

**Afternoon (2 hours):**
- [ ] **Documentation updates** (1h)
  - [ ] Document ErrorCode usage
  - [ ] Document Uri API
  - [ ] Update TESTING_STATUS.md
  - [ ] Note what's complete vs in-progress

- [ ] **Helper utilities** (1h)
  - [ ] String utilities (trim, split, toLower)
  - [ ] File utilities (exists, isDirectory)
  - [ ] Put in `src/shared/utils/`

**Evening (1 hour):**
- [ ] **Integration session with Bira** (1h together)
  - [ ] Merge both PRs into `develop` branch
  - [ ] Resolve any merge conflicts
  - [ ] Verify all tests still pass
  - [ ] Update Makefile with new files

---

#### Bira's Tasks (5 hours + 1h merge)

**Morning (3 hours):**
- [ ] **Code review Dande's PR** (1h)
  - [x] Review ErrorCode implementation
  - [x] Check if tests are needed (suggest adding)
  - [ ] Review Port fix
  - [ ] Learn architecture patterns
  - [ ] Approve when satisfied

- [x] **Write tests for ErrorCode** (2h)
  - [x] Create `tests/unit/test_ErrorCode.cpp`
  - [x] Test all status codes
  - [x] Test `isRedirection()`
  - [x] Test `fromString()` parsing
  - [x] Run tests, verify all pass
  - [x] Commit to Dande's branch (or new PR)

**Afternoon (2 hours):**
- [ ] **Research HTTP parsing** (1h)
  - [ ] Read HTTP/1.1 RFC 2616 (sections 4-5)
  - [ ] Study request format:
    ```
    GET /path HTTP/1.1\r\n
    Host: localhost\r\n
    Content-Length: 0\r\n
    \r\n
    ```
  - [ ] Document parsing strategy
  - [ ] Plan state machine approach

- [ ] **Prepare for HTTP parser** (1h)
  - [ ] Create `tests/unit/test_RequestParser.cpp` skeleton
  - [ ] Write first failing tests:
    - [ ] `ParseSimpleGET`
    - [ ] `ParseWithHeaders`
    - [ ] `ParsePOSTWithBody`
  - [ ] Don't implement yet - save for Days 3-4

**Evening (1 hour):**
- [ ] **Integration session with Dande** (1h together)
  - [ ] **CRITICAL: Test before merge**
    ```bash
    # Dande's branch
    git checkout 6-feat/value-objects-dande
    make clean && make  # Must pass
    cd tests && make && ./bin/test_runner  # Must pass
    
    # Bira's branch  
    git checkout 7-feat/value-objects-bira
    make clean && make  # Must pass
    cd tests && make && ./bin/test_runner  # Must pass
    ```
  - [ ] Merge both PRs into `develop` branch
  - [ ] Resolve any merge conflicts
  - [ ] **TEST AGAIN after merge:**
    ```bash
    git checkout develop
    make clean && make  # Must still pass
    cd tests && make && ./bin/test_runner  # Must still pass
    ```
  - [ ] If all passes → merge develop to main
  - [ ] Update Makefile with new files

---

### Day 2 Evening: Checkpoint

**Both together (15 minutes):**

**Verify completion:**
- [ ] ✅ ErrorCode complete and tested
- [ ] ✅ Port 10/10 tests passing
- [ ] ✅ Path complete with tests
- 🚨 REALITY CHECK:**
- [ ] Did we hit 80% of Days 1-2 goals?
- [ ] **If YES:** Proceed to Days 3-4 as planned
- [ ] **If NO:** What can we drop?
  - Drop Uri completely? Use hardcoded paths?
  - Skip Size? Just use strings?
  - Reduce Port tests? Just validate basic cases?

**Decision:** __________________ (write your plan)

**[ ] ✅ Size complete with tests
- [ ] ✅ Uri basic implementation
- [ ] ✅ Mock infrastructure ready
- [ ] ✅ All tests passing
- [ ] ✅ Clean git history

**Plan Days 3-4:**
- [ ] Dande: Socket server + epoll
- [ ] Bira: Request parser + Response builder
- [ ] Interface already defined, can work parallel
- [ ] Daily syncs to verify compatibility

---

## 🌐 Days 3-4: Core Server + HTTP Protocol

**Goal:** Socket server accepting connections + HTTP request/response handling

**Strategy:** Coordinated parallelism - both build against agreed interfaces

**Daily sync:** 7:00 PM (30 min) + 1:00 PM checkpoint (15 min)

---

### Day 3 - December 28 (Saturday)

#### Dande's Tasks (6 hours)

**Branch:** `8-feat/core-server-dande`

**Morning (3 hours):**
- [ ] **Socket management** (3h)
  - [ ] Create `src/infrastructure/network/SocketManager.hpp`
  - [ ] Create `src/infrastructure/network/SocketManager.cpp`
  - [ ] `socket()` + `bind()` + `listen()` setup
  - [ ] Set non-blocking: `fcntl(fd, F_SETFL, O_NONBLOCK)`
  - [ ] Set SO_REUSEADDR option
  - [ ] Multi-port support (bind to multiple ports)
  - [ ] Error handling (already in use, permission denied)
  - [ ] Basic test: bind to 8080, verify with `netstat`

**Afternoon (3 hours):**
- [ ] **I/O multiplexing (epoll) start** (3h)
  - [ ] Create `src/infrastructure/network/EventLoop.hpp`
  - [ ] Create `src/infrastructure/network/EventLoop.cpp`
  - [ ] `epoll_create1()` setup
  - [ ] Add server socket to epoll: `epoll_ctl()`
  - [ ] `epoll_wait()` loop skeleton
  - [ ] Handle EPOLLIN events
  - [ ] Accept new connections
  - [ ] Basic logging (connection received)
  - [ ] Test: `telnet localhost 8080` should connect

**Evening:**
- [ ] Test milestone: Server accepts connection without crashing
- [ ] Commit and push
- [ ] Update Bira on interface compliance

**Blockers to communicate:**
- If epoll is too complex, consider `poll()` fallback
- Document any interface changes needed

---

#### Bira's Tasks (6 hours)

**Branch:** `9-feat/http-protocol-bira`

**Morning (3 hours):**
- [ ] **Request Parser - Part 1** (3h)
  - [ ] Create `src/infrastructure/http/RequestParser.hpp`
  - [ ] Create `src/infrastructure/http/RequestParser.cpp`
  - [ ] Implements `IRequestParser` interface
  - [ ] **Parse request line:**
    ```cpp
    "GET /path HTTP/1.1\r\n" → 
      method="GET", uri="/path", version="HTTP/1.1"
    ```
  - [ ] State machine: READING_METHOD, READING_URI, READING_VERSION
  - [ ] **Unit tests:**
    - [ ] `ParseSimpleGET` ✅
    - [ ] `ParsePOST` ✅
    - [ ] `ParseDELETE` ✅
    - [ ] `InvalidMethod` (should throw/return error)
    - [ ] `MissingHTTPVersion` (should handle gracefully)
  - [ ] All tests passing

**Afternoon (3 hours):**
- [ ] **Request Parser - Part 2** (3h)
  - [ ] **Parse headers:**
    ```cpp
    "Host: localhost\r\n"
    "Content-Length: 100\r\n"
    "\r\n" → headers map
    ```
  - [ ] State machine: READING_HEADERS
  - [ ] Split on `: ` (colon + space)
  - [ ] Store in `std::map<string, string>`
  - [ ] Detect end of headers (`\r\n\r\n`)
  - [ ] **Unit tests:**
    - [ ] `ParseHeaders` ✅
    - [ ] `ParseMultipleHeaders` ✅
    - [ ] `MissingHost` (should handle)
    - [ ] `MalformedHeader` (no colon)
  - [ ] All tests passing

**Evening:**
- [ ] Commit and push
- [ ] Share progress with Dande
- [ ] Test parser with mock socket data

**Note:** Don't implement body parsing yet - save for Day 4

---

### Day 4 - December 29 (Sunday)

#### Dande's Tasks (6 hours)

**Morning (3 hours):**
- [ ] **Complete epoll event loop** (3h)
  - [ ] Handle client read events
  - [ ] Buffer management (read into buffer)
  - [ ] Detect partial reads (need more data)
  - [ ] Detect complete requests (see `\r\n\r\n`)
  - [ ] Call Bira's parser (using mock for now)
  - [ ] Handle connection close (EPOLLHUP, EPOLLERR)
  - [ ] Test: Send raw HTTP request via telnet, see it received

**Afternoon (3 hours):**
- [ ] **Client management** (3h)
  - [ ] Create `src/infrastructure/network/Client.hpp`
  - [ ] Create `src/infrastructure/network/Client.cpp`
  - [ ] Track client state (READING, PROCESSING, WRITING)
  - [ ] Read buffer (accumulate data)
  - [ ] Write buffer (response queue)
  - [ ] Connection timeout handling
  - [ ] Test: Multiple concurrent connections

**Evening:**
- [ ] **Integration prep** (1h with Bira)
  - [ ] Server can receive data ✅
  - [ ] Parser can parse data ✅
  - [ ] Integrate: Server → Parser → mock response
  - [ ] Test: `curl localhost:8080` should get response

---

#### Bira's Tasks (6 hours)

**Morning (3 hours):**
- [ ] **Request Parser - Part 3** (2h)
  - [ ] **Parse body (for POST):**
    - [ ] Check `Content-Length` header
    - [ ] Read exactly N bytes
    - [ ] State machine: READING_BODY
    - [ ] Store body as string
  - [ ] **Unit tests:**
    - [ ] `ParsePOSTWithBody` ✅
    - [ ] `ParseBodyPartial` (incomplete)
    - [ ] `ParseBodyComplete` ✅
  - [ ] All tests passing

- [ ] **Request validation** (1h)
  - [ ] Validate HTTP version (only 1.1)
  - [ ] Validate method (GET, POST, DELETE only)
  - [ ] Validate URI (not empty)
  - [ ] Return error codes for invalid requests

**Afternoon (3 hours):**
- [ ] **Response Builder** (3h)
  - [ ] Create `src/infrastructure/http/ResponseBuilder.hpp`
  - [ ] Create `src/infrastructure/http/ResponseBuilder.cpp`
  - [ ] Implements `IResponseBuilder` interface
  - [ ] **Build status line:**
    ```cpp
    build(200, "OK") → "HTTP/1.1 200 OK\r\n"
    ```
  - [ ] **Add headers:**
    ```cpp
    addHeader("Content-Type", "text/html")
    addHeader("Content-Length", "13")
    ```
  - [ ] **Build complete response:**
    ```
    HTTP/1.1 200 OK\r\n
    Content-Type: text/html\r\n
    Content-Length: 13\r\n
    \r\n
    Hello, World!
    ```
  - [ ] **Unit tests:**
    - [ ] `Build200Response` ✅
    - [ ] `Build404Response` ✅
    - [ ] `BuildWithHeaders` ✅
    - [ ] `BuildWithBody` ✅
  - [ ] All tests passing

**Evening:**
- [ ] **Integration with Dande** (1h together)
  - [ ] Swap mocks with real implementations
  - [ ] Server → Parser → ResponseBuilder → Client
  - [ ] Test end-to-end: `curl -v localhost:8080`
  - [ ] Verify valid HTTP response received

---

### Day 4 Evening: Integration Checkpoint

**Both together (1 hour):**

**Integration test:**
```bash
# Start server
./webserv test_config.conf

# Test in another terminal
curl -v http://localhost:8080/
# Should see:
# HTTP/1.1 200 OK
# Content-Type: text/html
# Content-Length: 13
#
# 🚨 REALITY CHECK:**
- [ ] Can we connect and get a response?
- [ ] **If YES:** You're 40% done! Keep going!
- [ ] **If NO:** EMERGENCY MODE
  - Pair program tomorrow (both on server)
  - Skip routing temporarily
  - Get ONE working endpoint first
  - Sacrifice features, not core functionality

**Hello, World!
```

**Verify:**
- [ ] ✅ Server accepts connections
- [ ] ✅ Parser extracts method, URI, headers
- [ ] ✅ Response builder creates valid HTTP response
- [ ] ✅ `curl` shows proper HTTP response
- [ ] ✅ No crashes, no leaks (quick valgrind check)

**Merge:**
- [ ] Merge both branches into `develop`
- [ ] Tag: `v0.2-http-server-working`

**Plan Days 5-6:**
- [ ] Dande: ConfigParser (complex, needs focus)
- [ ] Bira: Routing + error handling
- [ ] Both need config format agreement

---

## ⚙️ Days 5-6: Configuration + Routing

**Goal:** Parse nginx-like config files, route requests to handlers

**Strategy:** Dande defines config format, Bira consumes it

---

### Day 5 - December 30 (Monday)

#### Morning Session Together (30 minutes)

**Define config file format:**

```nginx
# Example config for reference
server {
    listen 8080;
    server_name localhost;
    root /var/www/html;
    
    error_page 404 /404.html;
    error_page 500 502 503 /50x.html;
    
    client_max_body_size 10M;
    
    location / {
        allow_methods GET POST;
        index index.html;
    }
    
    location /upload {
        allow_methods POST DELETE;
        upload_path /var/www/uploads;
    }
    
    location ~ \.php$ {
        cgi_pass /usr/bin/php-cgi;
        cgi_ext .php;
    }
}

server {
    listen 8081;
    server_name app.local;
    root /var/www/app;
}
```

**Agreement:**
- [ ] Which directives are mandatory
- [ ] Default values
- [ ] Data structure representation
- [ ] How Bira's router will consume it

---

#### Dande's Tasks (6 hours)

**Branch:** `10-feat/config-parser-dande`

**Morning (3 hours after sync):**
- [ ] **Config parser skeleton** (3h)
  - [ ] Create `src/infrastructure/config/ConfigParser.hpp`
  - [ ] Create `src/infrastructure/config/ConfigParser.cpp`
  - [ ] Create `src/infrastructure/config/Config.hpp` (data structures)
  - [ ] Tokenizer: split file into tokens
  - [ ] State machine: OUTSIDE_BLOCK, IN_SERVER, IN_LOCATION
  - [ ] Parse `server {` and `}` blocks
  - [ ] Parse `listen` directive
  - [ ] Parse `server_name` directive
  - [ ] Parse `root` directive
  - [ ] Basic validation (closing braces)
  - [ ] Test with simple config file

**Afternoon (3 hours):**
- [ ] **Config parser advanced** (3h)
  - [ ] Parse `location` blocks
  - [ ] Parse `allow_methods` directive
  - [ ] Parse `error_page` directives
  - [ ] Parse `client_max_body_size`
  - [ ] Parse CGI directives (`cgi_pass`, `cgi_ext`)
  - [ ] Handle multiple servers
  - [ ] Handle nested locations
  - [ ] Error reporting (line numbers)
  - [ ] Test with complex config file

**Evening:**
- [ ] Enable ConfigParser tests: `mv test_ConfigParser.cpp.disabled test_ConfigParser.cpp`
- [ ] Run existing tests, fix failures
- [ ] Commit and push

---

#### Bira's Tasks (5 hours)

**Branch:** `11-feat/routing-bira`

**Morning (3 hours after sync):**
- [ ] **Route entity** (2h)
  - [ ] Create `src/domain/entities/Route.hpp`
  - [ ] Create `src/domain/entities/Route.cpp`
  - [ ] Properties: path, methods, root, errorPages, cgi
  - [ ] Match logic: exact match, prefix match
  - [ ] **Unit tests:**
    - [ ] `ExactMatch` - "/api" matches "/api" ✅
    - [ ] `PrefixMatch` - "/api" matches "/api/users" ✅
    - [ ] `MethodFilter` - GET allowed, POST denied ✅
    - [ ] `NotFound` - no match returns nullptr

- [ ] **Documentation** (1h)
  - [ ] Document routing algorithm
  - [ ] Document match priorities
  - [ ] Examples for Dande to understand

**Afternoon (2 hours):**
- [ ] **Error handling** (2h)
  - [ ] Create `src/infrastructure/http/ErrorPageHandler.hpp`
  - [ ] Create `src/infrastructure/http/ErrorPageHandler.cpp`
  - [ ] Generate default error pages (404, 500, 403)
  - [ ] Load custom error pages from config
  - [ ] **Unit tests:**
    - [ ] `DefaultErrorPage` ✅
    - [ ] `CustomErrorPage` ✅
    - [ ] `ErrorPageNotFound` (fallback to default)

**Evening:**
- [ ] Research Path security patterns
- [ ] Prepare for file operations (Days 7-8)

---

### Day 6 - December 31 (Tuesday)

#### Dande's Tasks (5 hours + 1h merge)

**Morning (3 hours):**
- [ ] **Complete ConfigParser** (2h)
  - [ ] Final edge cases (comments, whitespace)
  - [ ] Validation (duplicate server names, invalid ports)
  - [ ] Default values for optional directives
  - [ ] All ConfigParser tests passing

- [ ] **Config to Route adapter** (1h)
  - [ ] Create `src/infrastructure/adapters/RouteConfigAdapter.hpp`
  - [ ] Convert Config → List of Routes
  - [ ] For each location block → create Route object
  - [ ] Apply server-level defaults
  - [ ] Test with mock config

**Afternoon (2 hours):**
- [ ] **Code review Bira's routing** (1h)
  - [ ] Review Route implementation
  - [ ] Test route matching logic
  - [ ] Suggest improvements

- [ ] **Documentation** (1h)
  - [ ] Document config file format
  - [ ] Add example configs
  - [ ] Update TESTING_STATUS.md

**Evening (1 hour):**
- [ ] **Integration with Bira**
  - [ ] ConfigParser → RouteAdapter → Router
  - [ ] Test: Load config, match routes
  - [ ] Verify multi-server support

---

#### Bira's Tasks (5 hours + 1h merge)

**Morning (3 hours):**
- [ ] **Route Matcher** (3h)
  - [ ] Create `src/domain/entities/RouteMatcher.hpp`
  - [ ] Create `src/domain/entities/RouteMatcher.cpp`
  - [ ] Algorithm:
    1. Find all matching routes (prefix match)
    2. Sort by specificity (longest prefix first)
    3. Filter by HTTP method
    4. Return best match
  - [ ] **Unit tests:**
    - [ ] `MatchSingleRoute` ✅
    - [ ] `MatchMultipleRoutes` (returns most specific) ✅
    - [ ] `MethodNotAllowed` (route exists but method wrong) ✅
    - [ ] `NoMatchFound` ✅
  - [ ] All tests passing

**Afternoon (2 hours):**
- [ ] **Path security hardening** (2h)
  - [ ] Revisit Path implementation
  - [ ] Add more security checks:
    - [ ] Reject `../` in any form (`..%2F`, `..%5C`)
    - [ ] Reject absolute paths outside root
    - [ ] Reject URL-encoded attacks
    - [ ] Canonicalize paths (resolve `.` and `..`)
  - [ ] **Security tests:**
    - [ ] `RejectParentDirectory` ✅
    - [ ] `RejectURLEncoded` ✅
    - [ ] `RejectAbsolutePath` ✅
    - [ ] `AllowValidPaths` ✅
  - [ ] Document security considerations

**Evening (1 hour):**
- [ ] **Integration with Dande**
  - [ ] Verify config → routes → matcher works
  - [ ] Test with example config
  - [ ] Merge both branches

---

### Day 6 Evening: Checkpoint
🚨 REALITY CHECK:**
- [ ] Does config parsing work at all?
- [ ] **If YES:** You're 60% done! File ops next!
- [ ] **If NO:** ESCAPE HATCH
  - Tomorrow: Both work on config together
  - Simplify: No regex, no nested locations
  - Hardcode routes if needed
  - Cut multiple servers if desperate
  - ONE working server > broken fancy system

**
**Both together (30 minutes):**

**Verify:**
- [ ] ✅ Config file parsing works
- [ ] ✅ Multiple servers supported
- [ ] ✅ Location blocks parsed
- [ ] ✅ Routes match correctly
- [ ] ✅ Error pages configured
- [ ] ✅ Path security solid

**Test:**
```bash
./webserv multi_server.conf
curl http://localhost:8080/api/users  # Should route correctly
curl http://localhost:8081/           # Different server
curl http://localhost:8080/../etc/passwd  # Should reject
```

**Merge:**
- [ ] Merge to `develop`
- [ ] Tag: `v0.3-config-routing-working`

**Plan Days 7-8:**
- [ ] Dande: GET (static file serving)
- [ ] Bira: POST/DELETE (file upload/deletion)
- [ ] Can work in parallel

---

## 📁 Days 7-8: File Operations

**Goal:** Serve static files (GET), upload files (POST), delete files (DELETE)

**Strategy:** Split by HTTP method - minimal overlap

---

### Day 7 - January 1 (Wednesday)

#### Dande's Tasks (6 hours)

**Branch:** `12-feat/file-get-dande`

**Morning (3 hours):**
- [ ] **FileHandler integration** (1h)
  - [ ] Fix FileSystemHelper constructor (make public)
  - [ ] Enable FileHandler tests: `mv test_FileHandler_Integration.cpp.disabled ...`
  - [ ] Update Makefile to include FileHandler
  - [ ] Run tests, fix immediate failures

- [ ] **Static file serving** (2h)
  - [ ] Create `src/infrastructure/http/StaticFileHandler.hpp`
  - [ ] Create `src/infrastructure/http/StaticFileHandler.cpp`
  - [ ] Read file with `fopen()` + `fread()`
  - [ ] Return file contents as string
  - [ ] Handle file not found → 404
  - [ ] Handle permission denied → 403
  - [ ] Basic test: Serve test.html

**Afternoon (3 hours):**
- [ ] **Content-Type detection** (2h)
  - [ ] Create `src/infrastructure/http/MimeTypeDetector.hpp`
  - [ ] Hardcode common types:
    ```cpp
    if (ext == ".html") return "text/html";
    if (ext == ".css") return "text/css";
    if (ext == ".js") return "text/javascript";
    if (ext == ".jpg" || ext == ".jpeg") return "image/jpeg";
    if (ext == ".png") return "image/png";
    if (ext == ".gif") return "image/gif";
    return "application/octet-stream";  // Default
    ```
  - [ ] Extract extension from path
  - [ ] Test with various file types

- [ ] **Directory index** (1h)
  - [ ] Check if path is directory
  - [ ] Look for index.html
  - [ ] Serve index.html if exists
  - [ ] Return 404 if no index

**Evening:**
- [ ] Test GET requests:
  ```bash
  curl http://localhost:8080/index.html  # Should return HTML
  curl http://localhost:8080/style.css   # Should return CSS
  curl http://localhost:8080/missing     # Should return 404
  ```
- [ ] Commit and push

---

#### Bira's Tasks (6 hours)

**Branch:** `13-feat/file-post-delete-bira`

**Morning (3 hours):**
- [ ] **Multipart parser research** (1h)
  - [ ] Study multipart/form-data format:
    ```
    POST /upload HTTP/1.1
    Content-Type: multipart/form-data; boundary=----WebKitFormBoundary
    
    ------WebKitFormBoundary
    Content-Disposition: form-data; name="file"; filename="test.txt"
    Content-Type: text/plain
    
    [file contents here]
    ------WebKitFormBoundary--
    ```
  - [ ] Plan parsing strategy
  - [ ] Document boundary detection

- [ ] **Multipart parser implementation** (2h)
  - [ ] Create `src/infrastructure/http/MultipartParser.hpp`
  - [ ] Create `src/infrastructure/http/MultipartParser.cpp`
  - [ ] Extract boundary from Content-Type header
  - [ ] Split body by boundary
  - [ ] Parse Content-Disposition header
  - [ ] Extract filename
  - [ ] Extract file contents
  - [ ] **Unit tests:**
    - [ ] `ParseSingleFile` ✅
    - [ ] `ParseFilename` ✅
    - [ ] `ExtractContents` ✅

**Afternoon (3 hours):**
- [ ] **File upload handler** (3h)
  - [ ] Create `src/infrastructure/http/FileUploadHandler.hpp`
  - [ ] Create `src/infrastructure/http/FileUploadHandler.cpp`
  - [ ] Receive multipart body from parser
  - [ ] Validate filename (security: no `../`, no absolute paths)
  - [ ] Write to upload directory (from config)
  - [ ] Write file with `fopen()` + `fwrite()`
  - [ ] Return 201 Created on success
  - [ ] Return 400 on validation error
  - [ ] Return 500 on write error
  - [ ] **Integration test:**
    ```bash
    curl -X POST -F "file=@test.txt" http://localhost:8080/upload
    # Should see file in upload directory
    ```

**Evening:**
- [ ] Test edge cases:
  - [ ] Large file (10MB)
  - [ ] Binary file (image)
  - [ ] Invalid filename
- [ ] Commit and push

---

### Day 8 - January 2 (Thursday)

#### Dande's Tasks (4 hours)

**Morning (2 hours):**
- [ ] **GET polish** (2h)
  - [ ] Range requests (optional, bonus feature)
  - [ ] Cache headers (Last-Modified, ETag)
  - [ ] Error page integration (use configured error pages)
  - [ ] Performance: Buffered reads for large files
  - [ ] Test with large file (100MB)

**Afternoon (2 hours):**
- [ ] **Code review Bira's POST/DELETE** (1h)
  - [ ] Review upload handler security
  - [ ] Test upload with malicious filenames
  - [ ] Suggest improvements

- [ ] **Documentation** (1h)
  - [ ] Document file serving flow
  - [ ] Document MIME type mapping
  - [ ] Update TESTING_STATUS.md

---

#### Bira's Tasks (4 hours)

**Morning (2 hours):**
- [ ] **DELETE method** (2h)
  - [ ] Create `src/infrastructure/http/FileDeleteHandler.hpp`
  - [ ] Create `src/infrastructure/http/FileDeleteHandler.cpp`
  - [ ] Parse DELETE request URI
  - [ ] Validate path (security!)
  - [ ] Check file exists
  - [ ] Delete with `unlink()`
  - [ ] Return 204 No Content on success
  - [ ] Return 404 if file not found
  - [ ] Return 403 if permission denied
  - [ ] **Integration test:**
    ```bash
    curl -X DELETE http://localhost:8080/test.txt
    # File should be deleted
    ```

**Afternoon (2 hours):**
- [ ] **Error handling integration** (1h)
  - [ ] Wire error pages into GET/POST/DELETE handlers
  - [ ] Return custom 404 page
  - [ ] Return custom 500 page
  - [ ] Test error responses

- [ ] **Security review** (1h)
  - [ ] Review all path validations
  - [ ] Test directory traversal attacks
  - [ ] Test upload attacks
  - [ ] Test delete attacks
  - [ ] Document security measures

---

### Day 8 Evening: Integration (2 hours together)

**Both together:**

**Full HTTP test:**
**🚨 CRITICAL DECISION POINT:**
- [ ] Do GET + POST + DELETE work?
- [ ] **If YES:** You have ALL mandatory methods! CGI decision time!
- [ ] **If NO:** EMERGENCY - Days 9-13 = Fix mandatory features
  - Must have GET working (serve static files)
  - Must have POST working (upload files)
  - Must have DELETE working (remove files)
  - **Cannot pass without all three methods**

**CGI Decision (Check your subject PDF!):**
- [ ] **Option 1 (CGI is BONUS):** Skip CGI, focus on polish
  - Days 9-13: Stability, tests, valgrind, siege
  - 100% of mandatory = guaranteed pass
- [ ] **Option 2 (CGI is MANDATORY):** Minimal CGI implementation
  - Days 9-10: Basic PHP-CGI execution only
  - Just fork + exec + read output
  - No advanced features, just working
- [ ] **Option 3 (Ahead of schedule):** Full CGI (follow plan)

**⚠️ CHECK YOUR SUBJECT PDF RIGHT NOW:**
- Is CGI in the mandatory part? ________________
- If yes → MUST implement (minimal is okay)
- If bonus → Can skip for stability

**Decision:** __________________ (write your choice)

**Merge:**
- [ ] Merge both branches to `develop`
- [ ] Tag: `v0.4-file-operations-working`
- [ ] Run valgrind, fix leaks
- [ ] Commit

**Plan Days 9-10:**
- [ ] PAIR PROGRAMMING on CGI (or fixing issues if behind)

---

## 🔧 Days 9-10: CGI (PAIR PROGRAMMING)

**Goal:** Execute PHP-CGI scripts, return output as HTTP response

**Strategy:** STOP PARALLELISM - Work together

**Why together:**
- CGI involves fork(), exec(), pipes, env vars (complex)
- High integration risk
- Immediate feedback loop faster
- Reduces debugging time later

**Branch:** `14-feat/cgi-integration` (both work on same branch)

---

### Day 9 - January 3 (Friday)

**Both together (8 hours total, 4h each)**

**Morning (4 hours together):**

- [ ] **CGI configuration** (1h)
  - [ ] Fix RegexPattern empty constructor bug:
    ```cpp
    RegexPattern::RegexPattern(const std::string& pattern) {
        if (pattern.empty()) {
            // Don't throw - allow empty pattern
            return;
        }
        // ... rest of validation
    }
    ```
  - [ ] Create `src/infrastructure/cgi/CgiConfig.hpp`
  - [ ] Create `src/infrastructure/cgi/CgiConfig.cpp`
  - [ ] Map file extensions to CGI executables:
    ```cpp
    .php → /usr/bin/php-cgi
    .py  → /usr/bin/python3
    .pl  → /usr/bin/perl
    ```
  - [ ] Test: Config parses CGI directives

- [ ] **CGI executor skeleton** (3h)
  - [ ] Create `src/infrastructure/cgi/CgiExecutor.hpp`
  - [ ] Create `src/infrastructure/cgi/CgiExecutor.cpp`
  - [ ] Basic structure:
    ```cpp
    std::string execute(const std::string& scriptPath,
                        const HttpRequest& request) {
        // 1. Fork process
        // 2. Setup pipes (stdin, stdout)
        // 3. Setup environment variables
        // 4. Execute CGI script
        // 5. Read output
        // 6. Wait for child
        // 7. Return output
    }
    ```
  - [ ] Implement fork():
    ```cpp
    pid_t pid = fork();
    if (pid == 0) {
        // Child process
        execve(cgi_path, args, env);
    } else {
        // Parent process
        wait(&status);
    }
    ```

**Afternoon (4 hours together):**

- [ ] **Pipe communication** (2h)
  - [ ] Create pipes for stdin/stdout:
    ```cpp
    int stdin_pipe[2];
    int stdout_pipe[2];
    pipe(stdin_pipe);
    pipe(stdout_pipe);
    ```
  - [ ] In child:
    ```cpp
    dup2(stdin_pipe[0], STDIN_FILENO);
    dup2(stdout_pipe[1], STDOUT_FILENO);
    close(stdin_pipe[1]);
    close(stdout_pipe[0]);
    ```
  - [ ] In parent:
    ```cpp
    close(stdin_pipe[0]);
    close(stdout_pipe[1]);
    write(stdin_pipe[1], request_body);
    read(stdout_pipe[0], output_buffer);
    ```
  - [ ] Test with simple script: `echo "Hello CGI"`

- [ ] **Environment variables** (2h)
  - [ ] Create env array:
    ```cpp
    std::vector<char*> env_vars;
    env_vars.push_back("REQUEST_METHOD=GET");
    env_vars.push_back("SCRIPT_FILENAME=/path/to/script.php");
    env_vars.push_back("PATH_INFO=/api/users");
    env_vars.push_back("QUERY_STRING=id=123");
    env_vars.push_back("CONTENT_LENGTH=100");
    env_vars.push_back("CONTENT_TYPE=application/json");
    env_vars.push_back(NULL);
    ```
  - [ ] Pass to execve():
    ```cpp
    execve(cgi_path, args, env_vars.data());
    ```
  - [ ] Test: PHP script can access `$_SERVER` variables

**Evening:**
- [ ] Test with real PHP script:
  ```php
  <?php
  echo "Hello from PHP!";
  echo "Method: " . $_SERVER['REQUEST_METHOD'];
  ?>
  ```
- [ ] Verify output received
- [ ] Commit progress

---

### Day 10 - January 4 (Saturday)

**Both together (8 hours total, 4h each)**

**Morning (4 hours together):**

- [ ] **CGI output parsing** (2h)
  - [ ] CGI returns:
    ```
    Content-Type: text/html
    
    <html>Hello</html>
    ```
  - [ ] Parse headers from output
  - [ ] Split on first `\r\n\r\n`
  - [ ] Headers before, body after
  - [ ] Build HTTP response:
    ```
    HTTP/1.1 200 OK
    Content-Type: text/html
    [headers from CGI]
    
    [body from CGI]
    ```

- [ ] **Error handling** (2h)
  - [ ] CGI script not found → 404
  - [ ] CGI script not executable → 500
  - [ ] CGI script timeout (5 seconds) → 504
  - [ ] CGI script segfault → 500
  - [ ] Use `waitpid()` with timeout:
    ```cpp
    int status;
    pid_t result = waitpid(pid, &status, WNOHANG);
    if (result == 0) {
        // Still running, check timeout
        if (elapsed > 5.0) {
            kill(pid, SIGKILL);
            return "504 Gateway Timeout";
        }
    }
    ```

**Afternoon (4 hours together):**

- [ ] **POST request to CGI** (2h)
  - [ ] Write request body to CGI stdin
  - [ ] Set CONTENT_LENGTH env var
  - [ ] Set CONTENT_TYPE env var
  - [ ] Test form submission:
    ```bash
    curl -X POST -d "name=John&age=30" http://localhost:8080/form.php
    ```

- [ ] **CGI integration testing** (2h)
  - [ ] Test various scripts:
    - [ ] Simple output
    - [ ] Form processing
    - [ ] File upload via CGI
    - [ ] Database query (if available)
  - [ ] Test error cases:
    - [ ] Script doesn't exist
    - [ ] Script syntax error
    - [ ] Script infinite loop (timeout)
  - [ ] Fix bugs found

**Evening:**
- [ ] Full CGI test suite:
  ```bash
  curl http://localhost:8080/hello.php  # ✅
  curl -X POST -d "data=test" http://localhost:8080/form.php  # ✅
  curl http://localhost:8080/missing.php  # ✅ 404
  ```
- [ ] Commit and push
- [ ] Merge to `develop`
- [ ] Tag: `v0.5-cgi-working`

---

### Day 10 Evening: Major Milestone

**Both together (1 hour):**

**Celebrate:** 🎉 All major features complete!

**Verify:**
- [ ] ✅ HTTP server running
- [ ] ✅ Configuration parsed
- [ ] ✅ Routes matching
- [ ] ✅ GET serving files
- [ ] ✅ POST uploading files
- [ ] ✅ DELETE removing files
- [ ] ✅ CGI executing scripts (or skipped if behind)

**Test full flow:**
```bash
# 1. Server starts
./webserv webserv.conf

# 2. GET request
curl http://localhost:8080/index.html  # ✅

# 3. POST upload
curl -X POST -F "file=@test.txt" http://localhost:8080/upload  # ✅

# 4. DELETE file
curl -X DELETE http://localhost:8080/upload/test.txt  # ✅

# 5. CGI script (if implemented)
curl http://localhost:8080/test.php  # ✅
ls upload_dir/upload.txt  # ✅ File exists

# DELETE
curl -X DELETE http://localhost:8080/upload/upload.txt  # ✅
ls upload_dir/upload.txt  # ✅ File gone

# Security
curl http://localhost:8080/../etc/passwd  # ✅ 403
curl -X POST -F "file=@test.txt" http://localhost:8080/../etc/  # ✅ 400
```

**Merge:**
- [ ] Merge both branches to `develop`
- [ ] Tag: `v0.4-file-operations-working`
- [ ] Run valgrind, fix leaks
- [ ] Commit

**Plan Days 9-10:**
- [ ] PAIR PROGRAMMING on CGI
- [ ] Complex, high-risk - better together

---

## 🔧 Days 9-10: CGI (PAIR PROGRAMMING)

**Goal:** Execute PHP-CGI scripts, return output as HTTP response

**Strategy:** STOP PARALLELISM - Work together

**Why together:**
- CGI involves fork(), exec(), pipes, env vars (complex)
- High integration risk
- Immediate feedback loop faster
- Reduces debugging time later

**Branch:** `14-feat/cgi-integration` (both work on same branch)

---

### Day 9 - January 3 (Friday)

**Both together (8 hours total, 4h each)**

**Morning (4 hours together):**

- [ ] **CGI configuration** (1h)
  - [ ] Fix RegexPattern empty constructor bug:
    ```cpp
    RegexPattern::RegexPattern(const std::string& pattern) {
        if (pattern.empty()) {
            // Don't throw - allow empty pattern
            return;
        }
        // ... rest of validation
    }
    ```
  - [ ] Create `src/infrastructure/cgi/CgiConfig.hpp`
  - [ ] Create `src/infrastructure/cgi/CgiConfig.cpp`
  - [ ] Map file extensions to CGI executables:
    ```cpp
    .php → /usr/bin/php-cgi
    .py  → /usr/bin/python3
    .pl  → /usr/bin/perl
    ```
  - [ ] Test: Config parses CGI directives

- [ ] **CGI executor skeleton** (3h)
  - [ ] Create `src/infrastructure/cgi/CgiExecutor.hpp`
  - [ ] Create `src/infrastructure/cgi/CgiExecutor.cpp`
  - [ ] Basic structure:
    ```cpp
    std::string execute(const std::string& scriptPath,
                        const HttpRequest& request) {
        // 1. Fork process
        // 2. Setup pipes (stdin, stdout)
        // 3. Setup environment variables
        // 4. Execute CGI script
        // 5. Read output
        // 6. Wait for child
        // 7. Return output
    }
    ```
  - [ ] Implement fork():
    ```cpp
    pid_t pid = fork();
    if (pid == 0) {
        // Child process
        execve(cgi_path, args, env);
    } else {
        // Parent process
        wait(&status);
    }
    ```

**Afternoon (4 hours together):**

- [ ] **Pipe communication** (2h)
  - [ ] Create pipes for stdin/stdout:
    ```cpp
    int stdin_pipe[2];
    int stdout_pipe[2];
    pipe(stdin_pipe);
    pipe(stdout_pipe);
    ```
  - [ ] In child:
    ```cpp
    dup2(stdin_pipe[0], STDIN_FILENO);
    dup2(stdout_pipe[1], STDOUT_FILENO);
    close(stdin_pipe[1]);
    close(stdout_pipe[0]);
    ```
  - [ ] In parent:
    ```cpp
    close(stdin_pipe[0]);
    close(stdout_pipe[1]);
    write(stdin_pipe[1], request_body);
    read(stdout_pipe[0], output_buffer);
    ```
  - [ ] Test with simple script: `echo "Hello CGI"`

- [ ] **Environment variables** (2h)
  - [ ] Create env array:
    ```cpp
    std::vector<char*> env_vars;
    env_vars.push_back("REQUEST_METHOD=GET");
    env_vars.push_back("SCRIPT_FILENAME=/path/to/script.php");
    env_vars.push_back("PATH_INFO=/api/users");
    env_vars.push_back("QUERY_STRING=id=123");
    env_vars.push_back("CONTENT_LENGTH=100");
    env_vars.push_back("CONTENT_TYPE=application/json");
    env_vars.push_back(NULL);
    ```
  - [ ] Pass to execve():
    ```cpp
    execve(cgi_path, args, env_vars.data());
    ```
  - [ ] Test: PHP script can access `$_SERVER` variables

**Evening:**
- [ ] Test with real PHP script:
  ```php
  <?php
  echo "Hello from PHP!";
  echo "Method: " . $_SERVER['REQUEST_METHOD'];
  ?>
  ```
- [ ] Verify output received
- [ ] Commit progress

---

### Day 10 - January 4 (Saturday)

**Both together (8 hours total, 4h each)**

**Morning (4 hours together):**

- [ ] **CGI output parsing** (2h)
  - [ ] CGI returns:
    ```
    Content-Type: text/html
    
    <html>Hello</html>
    ```
  - [ ] Parse headers from output
  - [ ] Split on first `\r\n\r\n`
  - [ ] Headers before, body after
  - [ ] Build HTTP response:
    ```
    HTTP/1.1 200 OK
    Content-Type: text/html
    [headers from CGI]
    
    [body from CGI]
    ```

- [ ] **Error handling** (2h)
  - [ ] CGI script not found → 404
  - [ ] CGI script not executable → 500
  - [ ] CGI script timeout (5 seconds) → 504
  - [ ] CGI script segfault → 500
  - [ ] Use `waitpid()` with timeout:
    ```cpp
    int status;
    pid_t result = waitpid(pid, &status, WNOHANG);
    if (result == 0) {
        // Still running, check timeout
        if (elapsed > 5.0) {
            kill(pid, SIGKILL);
            return "504 Gateway Timeout";
        }
    }
    ```

**Afternoon (4 hours together):**

- [ ] **POST request to CGI** (2h)
  - [ ] Write request body to CGI stdin
  - [ ] Set CONTENT_LENGTH env var
  - [ ] Set CONTENT_TYPE env var
  - [ ] Test form submission:
    ```bash
    curl -X POST -d "name=John&age=30" http://localhost:8080/form.php
    ```

- [ ] **CGI integration testing** (2h)
  - [ ] Test various scripts:
    - [ ] Simple output
    - [ ] Form processing
    - [ ] File upload via CGI
    - [ ] Database query (if available)
  - [ ] Test error cases:
    - [ ] Script doesn't exist
    - [ ] Script syntax error
    - [ ] Script infinite loop (timeout)
  - [ ] Fix bugs found

**Evening:** (or skipped if behind)

**Test full flow:**
```bash
# 1. Server starts
./webserv webserv.conf

# 2. GET request
curl http://localhost:8080/index.html  # ✅

# 3. POST upload
curl -X POST -F "file=@test.txt" http://localhost:8080/upload  # ✅

# 4. DELETE file
curl -X DELETE http://localhost:8080/upload/test.txt  # ✅

# 5. CGI script (if implemented)
curl http://localhost:8080/test.php  # ✅
```

**🚨 FINAL REALITY CHECK (Use subject PDF checklist):**
- [ ] ✅ Does server compile without errors/warnings?
- [ ] ✅ Does server start and listen on multiple ports?
- [ ] ✅ Does GET serve static files correctly?
- [ ] ✅ Does POST upload files and save them?
- [ ] ✅ Does DELETE remove files correctly?
- [ ] ✅ Does config file parse multiple servers?
- [ ] ✅ Do routes match (location blocks)?
- [ ] ✅ Do default error pages work (404, 403, 500)?
- [ ] ✅ Does it handle wrong method (405)?
- [ ] ✅ Is I/O non-blocking (epoll/poll/select)?

**Count your checkmarks:**
- **10/10** → Perfect! You'll get bonus points!
- **8-9/10** → Solid pass, just polish the rest
- **6-7/10** → Borderline, Days 11-13 = fix missing items
- **< 6/10** → CRISIS MODE

**If < 6/10 → Days 11-13 Emergency Plan:**
1. List what's broken vs what's missing
2. Fix broken things first (crashes, leaks)
3. Implement missing mandatory features
4. Forget about bonus features entirely
5. Focus: Stable + Complete mandatory > Unstable + Bonus

**Remember:** Evaluator prefers "works perfectly with no bonus" over "crashes with all features"

**State of project:** ____/10 complete
**Verify:**
- [ ] ✅ HTTP server running
- [ ] ✅ Configuration parsed
- [ ] ✅ Routes matching
- [ ] ✅ GET serving files
- [ ] ✅ POST uploading files
- [ ] ✅ DELETE removing files
- [ ] ✅ CGI executing scripts

**Test full flow:**
```bash
# 1. Server starts
./webserv webserv.conf

# 2. GET request
curl http://localhost:8080/index.html  # ✅

# 3. POST upload
curl -X POST -F "file=@test.txt" http://localhost:8080/upload  # ✅

# 4. DELETE file
curl -X DELETE http://localhost:8080/upload/test.txt  # ✅

# 5. CGI script
curl http://localhost:8080/test.php  # ✅
```

**Plan Days 11-13:**
- [ ] Integration testing
- [ ] Bug hunting
- [ ] Valgrind
- [ ] Siege
- [ ] Final polish

---

## 🧪 Days 11-13: Integration & Polish (TOGETHER)

**Goal:** Stable, leak-free, crash-free server ready for evaluation

**Strategy:** MUST be together - integration requires both perspectives

---

### Day 11 - January 5 (Sunday)

**Both together (10 hours total, 5h each)**

**Morning (3 hours):**

- [ ] **Full integration testing** (3h)
  - [ ] Test every feature in sequence
  - [ ] Test every feature combination:
    - [ ] GET from server 1, server 2
    - [ ] POST to different locations
    - [ ] DELETE with error pages
    - [ ] CGI on different routes
  - [ ] Test concurrent requests:
    ```bash
    # Run 10 curl commands in parallel
    for i in {1..10}; do
        curl http://localhost:8080/ &
    done
    wait
    ```
  - [ ] Document all bugs found

**Afternoon (2 hours):**

- [ ] **Bug fixes - Critical** (2h)
  - [ ] Fix any crashes
  - [ ] Fix any segfaults
  - [ ] Fix any hangs/deadlocks
  - [ ] Fix any data corruption
  - [ ] Priority: Stability over features

**Evening (5 hours - may extend):**

- [ ] **Valgrind leak hunting** (5h)
  - [ ] Run full test suite under valgrind:
    ```bash
    valgrind --leak-check=full \
             --show-leak-kinds=all \
             --track-origins=yes \
             ./webserv test.conf
    ```
  - [ ] Fix every memory leak:
    - [ ] Unreachable blocks
    - [ ] Still reachable blocks
    - [ ] Lost blocks
  - [ ] Common sources:
    - [ ] Missing `delete` for `new`
    - [ ] Missing `close()` for file descriptors
    - [ ] Missing `free()` for C allocations
  - [ ] Goal: **0 bytes leaked**
  - [ ] Commit after each fix

---

### Day 12 - January 6 (Monday)

**Both together (10 hours total, 5h each)**

**Morning (3 hours):**

- [ ] **Siege stress testing** (3h)
  - [ ] Install siege: `sudo apt install siege`
  - [ ] Run stress test:
    ```bash
    # 50 concurrent users, 100 requests each = 5000 total
    siege -c 50 -r 100 http://localhost:8080/
    ```
  - [ ] Monitor with top/htop:
    - [ ] CPU usage
    - [ ] Memory usage
    - [ ] Connection count
  - [ ] Check results:
    ```
    Transactions: 5000 hits
    Availability: 99.50 %  ← MUST BE > 99.5%
    Elapsed time: X.XX secs
    Response time: X.XX secs
    Transaction rate: XXX trans/sec
    Failed transactions: 25  ← MUST BE < 50
    ```
  - [ ] If availability < 99.5%:
    - [ ] Check for race conditions
    - [ ] Check for deadlocks
    - [ ] Increase timeout values
    - [ ] Fix and retest

**Afternoon (3 hours):**

- [ ] **Bug fixes - Non-critical** (3h)
  - [ ] Fix edge cases
  - [ ] Fix error messages
  - [ ] Fix logging output
  - [ ] Improve error handling
  - [ ] Code cleanup

**Evening (4 hours):**

- [ ] **Final validation** (4h)
  - [ ] Run all tests again:
    ```bash
    cd tests/
    make clean && make
    ./bin/test_runner  # All pass?
    ```
  - [ ] Run valgrind again (verify 0 leaks)
  - [ ] Run siege again (verify 99.5%+)
  - [ ] Test with evaluator checklist:
    - [ ] Multiple servers work
    - [ ] All HTTP methods work
    - [ ] Error pages work
    - [ ] CGI works
    - [ ] No crashes
  - [ ] Fix any remaining issues

---

### Day 13 - January 7 (Tuesday) - FINAL DAY

**Both together (10 hours total, 5h each)**

**Morning (3 hours):**

- [ ] **Documentation** (2h)
  - [ ] Update README.md:
    - [ ] How to compile
    - [ ] How to run
    - [ ] Example configs
    - [ ] Supported features
  - [ ] Create CONFIG_GUIDE.md:
    - [ ] All directives explained
    - [ ] Examples
    - [ ] Default values
  - [ ] Update TESTING_STATUS.md:
    - [ ] Mark everything complete
    - [ ] Document known limitations

- [ ] **Code review** (1h)
  - [ ] Both review full codebase
  - [ ] Check for TODOs
  - [ ] Check for debug prints
  - [ ] Clean up dead code

**Afternoon (3 hours):**

- [ ] **Evaluation preparation** (2h)
  - [ ] Create demo config files:
    - [ ] `basic.conf` - Simple server
    - [ ] `multi.conf` - Multiple servers
    - [ ] `cgi.conf` - CGI enabled
  - [ ] Create test files:
    - [ ] index.html
    - [ ] 404.html
    - [ ] test.php
  - [ ] Practice evaluation demo:
    - [ ] Start server
    - [ ] Show GET
    - [ ] Show POST
    - [ ] Show DELETE
    - [ ] Show CGI
    - [ ] Show multiple ser (or at least GET + POST)
- [ ] CGI works with PHP-CGI (if implemented)
- [ ] Multiple servers work (if implemented)
- [ ] Configuration file valid
- [ ] No memory leaks (valgrind clean)
- [ ] No crashes under load (siege passed)
- [ ] Documentation complete

**🎯 ABSOLUTE MINIMUM FOR PASSING (0 if missing ANY):**
- [ ] Server compiles without errors ✅ (MANDATORY)
- [ ] Server starts without crashes ✅ (MANDATORY)
- [ ] GET serves static files ✅ (MANDATORY)
- [ ] POST receives and stores files ✅ (MANDATORY)
- [ ] DELETE removes files ✅ (MANDATORY)
- [ ] Config file parsed (multiple servers) ✅ (MANDATORY)
- [ ] Multiple ports listening ✅ (MANDATORY)
- [ ] Routes/location blocks work ✅ (MANDATORY)
- [ ] Non-blocking I/O (select/poll/epoll) ✅ (MANDATORY)
- [ ] Error pages (404, 403, 500) ✅ (MANDATORY)
- [ ] No segfaults/crashes ✅ (MANDATORY)
- [ ] Valgrind shows 0 bytes lost ✅ (MANDATORY)

**BONUS POINTS (check your subject PDF):**
- [ ] CGI execution (might be mandatory - CHECK PDF!)
- [ ] Cookies/Session management
- [ ] Multiple CGI support
- [ ] Custom error page files (beyond hardcoded HTML)
- [ ] Siege > 99.5% availability
- [ ] Directory listing
- [ ] URL rewriting

**⚠️ BEFORE EVALUATION - VERIFY AGAINST SUBJECT PDF:**
- [ ] Read mandatory section line by line
- [ ] Test every mandatory feature listed
- [ ] Don't assume anything is "good enough"

**Evaluation structure:**
1. **Compile and run** (evaluator watches)
2. **Basic tests** (curl commands)
3. **Siege stress test** (evaluator runs)
4. **Valgrind** (evaluator checks)
5. **Code review** (evaluator asks questions)

**Both answer questions:**
- Dande: Architecture, server implementation
- Bira: Testing, security, validation

**Remember:** 
- Calm confidence > nervous perfection
- Explain trade-offs you made
- "We prioritized stability over features"
- If something doesn't work: "We identified this risk and focused on core functionality"

**Good luck!** 🚀

---

## 🏆 POST-EVALUATION DEBRIEF

**After evaluation (regardless of result):**

**What worked:**
- What parts of the strategy helped most?
- What communication patterns worked?
- What technical decisions were right?

**What didn't work:**
- Where did the plan break down?
- What took longer than expected?
- What would you do differently?

**For future projects:**
- Would you use this strategy again?
- What would you change?
- What did you learn about teamwork?

**Write your notes here after Jan 8:**

___________________________________________
___________________________________________
___________________________________________
___________________________________________
___________________________________________
## ✅ Evaluation Day - January 8 (Wednesday)

**Both present for evaluation:**

**Pre-evaluation checklist:**
- [ ] Server compiles with no warnings
- [ ] Server starts with no errors
- [ ] All HTTP methods work
- [ ] CGI works with PHP-CGI
- [ ] Multiple servers work
- [ ] Configuration file valid
- [ ] No memory leaks (valgrind clean)
- [ ] No crashes under load (siege passed)
- [ ] Documentation complete

**Evaluation structure:**
1. **Compile and run** (evaluator watches)
2. **Basic tests** (curl commands)
3. **Siege stress test** (evaluator runs)
4. **Valgrind** (evaluator checks)
5. **Code review** (evaluator asks questions)

**Both answer questions:**
- Dande: Architecture, server implementation
- Bira: Testing, security, validation

**Good luck!** 🚀

---

## 📊 Summary Statistics

### Workload Distribution

| Phase | Dande Hours | Bira Hours | Together | Total |
|-------|-------------|------------|----------|-------|
| Days 1-2: Value Objects | 11h | 11h | 2h | 24h |
| Days 3-4: Server/HTTP | 12h | 12h | 2h | 26h |
| Days 5-6: Config/Routes | 11h | 10h | 2h | 23h |
| Days 7-8: File Ops | 10h | 10h | 2h | 22h |
| Days 9-10: CGI | - | - | 16h | 16h |
| Days 11-13: Integration | - | - | 30h | 30h |
| **Total** | **44h** | **43h** | **54h** | **141h** |

**Per person:** 44h + 27h (half of together) = **71 hours**  
**Per day:** 71h ÷ 13 days = **5.5 hours/day**

**vs Solo:** 104 hours ÷ 13 days = 8 hours/day  
**Savings:** 2.5 hours/day per person = **31% less workload**

---

## 🎯 Success Factors

### Communication
- [ ] Daily standups (15 min)
- [ ] Daily check-ins (10 min)
- [ ] Code reviews (< 2 hour turnaround)
- [ ] Shared task board updated daily

### Integration
- [ ] Interfaces defined Day 0
- [ ] Small frequent merges (Days 2, 4, 6, 8)
- [ ] No big-bang integration
- [ ] Continuous testing

### Knowledge Sharing
- [ ] Dande documents while building
- [ ] Bira shares TDD practices
- [ ] Code reviews = learning
- [ ] Pair programming on complex parts

### Risk Mitigation
- [ ] Mock when blocked
- [ ] Backup tasks ready
- [ ] Stop parallelism for high-risk parts
- [ ] Buffer time built in

---

**Good luck to both Dande and Bira!** 🎉

You've got this! 💪
