# Two-Person Sprint Strategy - 10 Days to Evaluation

**Team:** Dande (Foundation Expert) + Bira (TDD Specialist)  
**Deadline:** January 8, 2026  
**Current Date:** December 29, 2025 (**10 DAYS REMAINING**)  
**Strategy:** Foundation-first testing, then build server on solid ground

## 🚨 CRITICAL UPDATE - December 29, 2025

**What Changed:** After 3 days of work, Dande has built ~24,600 lines of excellent infrastructure code BUT we discovered:
1. ❌ **NO ACTUAL SERVER EXISTS** - Zero network I/O code
2. ✅ **185/187 tests passing** - Domain layer well-tested
3. ⚠️ **9 value objects untested** - Foundation incomplete

**New Reality:** We have 10 days (was 13), but we have better architecture than expected. **REVISED STRATEGY:** Test all value objects first (Days 1-5), then build server quickly (Days 6-10).

**Why this works:** Solid foundation = faster server development with fewer bugs.

---

## ⚠️ THE BRUTAL TRUTH - UPDATED DEC 29

### Current Situation

**What We Have:**
- ✅ 24,600 lines of code (excellent architecture)
- ✅ 185/187 tests passing (98.93%)
- ✅ Complete domain layer (value objects, entities, exceptions)
- ✅ Infrastructure adapters (ConfigParser, FileHandler, RequestParser)
- ❌ **NO NETWORK SERVER** - Cannot bind to port, accept connections

**What We're Missing:**
- ❌ Socket server (0 lines of code)
- ❌ Epoll/poll event loop (0 lines of code)
- ❌ HTTP server runtime (0 lines of code)
- ❌ 9 untested value objects (foundation gaps)

### Without Foundation-First Strategy

If you build the server on untested value objects:

| Metric | Value | Reality Check |
|--------|-------|---------------|
| **Debugging time** | **40+ hours** | "Is it my code or the value object?" |
| **Success probability** | **60%** | Unstable foundation = unstable building |
| **Integration bugs** | **HIGH** | Foundation bugs cascade upward |
| **Confidence** | **LOW** | Never sure if basics work |

**Translation:** Fast start, slow finish. Debugging hell at the end.

---

### With Foundation-First Strategy (THIS PLAN)

If you test all value objects first, then build server:

| Metric | Value | Reality Check |
|--------|-------|---------------|
| **Testing time** | **40 hours** | Days 1-5: Solid foundation |
| **Server build time** | **60 hours** | Days 6-10: Fast development on solid base |
| **Debugging time** | **10 hours** | Know exactly where bugs are |
| **Success probability** | **85%** | Confidence in every layer |
| **Integration bugs** | **LOW** | Foundation proven before building |
| **Confidence** | **HIGH** | Tests prove basics work |

**Translation:** Slow start, fast finish. Smooth development from proven foundation.

---

## 🎯 THE FOUNDATION-FIRST PRINCIPLE

### Why Test Value Objects First?

**Dependency Chain:**
```
Value Objects (MUST TEST FIRST)
    ↓
Entities (depend on value objects)
    ↓
Infrastructure Adapters (depend on entities)
    ↓
Application Layer (depends on adapters)
    ↓
Server Runtime (depends on everything)
```

**Example of cascading failure:**
1. Uri is broken (crashes on empty string)
2. RequestParser uses Uri → crashes
3. HttpServer uses RequestParser → crashes
4. **You waste 8 hours debugging server when problem was Uri**

**With foundation-first:**
1. Test Uri thoroughly (2 hours)
2. Know Uri works perfectly
3. RequestParser failures must be in RequestParser code
4. **Debug in 30 minutes, not 8 hours**

---

## 📊 WHAT WE NEED TO TEST (9 Value Objects)

### Currently Tested (5/14) ✅
- ✅ **ErrorCode** (50/50 tests) - HTTP status codes
- ✅ **HttpMethod** (5/5 tests) - GET/POST/DELETE  
- ✅ **Path** (25/25 tests) - Filesystem paths
- ✅ **Size** (44/44 tests) - File sizes
- ⚠️ **Port** (8/10 tests) - Needs 2 test fixes

### MUST TEST BEFORE SERVER (9 untested)

**Priority 1 - HTTP Layer (3 value objects):**
1. **Uri** - Used by: RequestParser, RouteMatcher, ALL request handling
2. **Host** - Used by: ServerConfig, ListenDirective, multi-server routing
3. **QueryStringBuilder** - Used by: RequestParser, CGI parameter passing

**Priority 2 - Configuration Layer (4 value objects):**
4. **ListenDirective** - Used by: ServerConfig, port binding
5. **ErrorPage** - Used by: ServerConfig, error responses
6. **Route** - Used by: RouteMatcher, routing logic
7. **RegexPattern** - Used by: Route, LocationConfig (currently crashes!)

**Priority 3 - Filesystem Layer (2 value objects):**
8. **Permission** - Used by: FileHandler, upload validation
9. **UploadAccess** - Used by: LocationConfig, security

**Total Effort:** ~40 hours (20h each = 4h/day for 5 days)

---

## 🎯 GUT-LEVEL DETERMINATION - UPDATED

### The New Math

You have **10 days**. That's **240 hours** of calendar time for two people.

**Option 1: Skip foundation testing (DON'T DO THIS):**
- Build server immediately: 60 hours
- Debug foundation bugs in server: 40 hours  
- Total: 100 hours ÷ 2 = 50h each = 5h/day
- **BUT:** 60% success rate (foundation bugs kill you)

**Option 2: Foundation-first (THIS PLAN):**
- Test 9 value objects: 40 hours (Days 1-5)
- Build server on solid base: 60 hours (Days 6-10)
- Total: 100 hours ÷ 2 = 50h each = 5h/day
- **AND:** 85% success rate (no foundation surprises)

**The difference:** Same time, but 25% higher success rate with foundation-first.

---

## 🚀 REVISED 10-DAY PLAN

### Phase 1: Foundation Testing (Days 1-5)

**Goal:** All value objects 100% tested before writing server

| Day | Bira Tasks | Dande Tasks | Tests Added |
|-----|------------|-------------|-------------|
| **Day 1** | Uri (20 tests, 4h) | Host (15 tests, 3h) | 35 tests |
| | QueryStringBuilder (12 tests, 3h) | Fix Port (2 tests, 1h) | +14 tests |
| **Day 2** | Permission (18 tests, 4h) | ListenDirective (15 tests, 3h) | 33 tests |
| | UploadAccess (10 tests, 2h) | RegexPattern fix (2h) | +10 tests |
| **Day 3** | ErrorPage (12 tests, 3h) | Route (25 tests, 5h) | 37 tests |
| | Document value objects (2h) | Review Bira's tests (1h) | - |
| **Day 4** | Re-enable FileHandler tests | Re-enable ConfigParser tests | Integration |
| | Test infrastructure adapters | Fix adapter bugs | tests |
| **Day 5** | Review all tests | Fix remaining test issues | Buffer day |

**Milestone:** ~240 tests passing, 100% value object coverage

### Phase 2: Server Development (Days 6-10)

**Goal:** Build HTTP server on proven foundation

| Day | Bira Tasks | Dande Tasks | Milestone |
|-----|------------|-------------|-----------|
| **Day 6** | SocketServer (8h) | Response builder (6h) | Binds to port |
| **Day 7** | Epoll event loop (8h) | Static file GET (6h) | Accepts connections |
| **Day 8** | Connection manager (8h) | POST upload (6h) | Serves files |
| **Day 9** | Request handler (6h) | DELETE + Config (7h) | All methods work |
| **Day 10** | Integration testing | Bug fixes + valgrind | Ready for eval |

**Milestone:** Functional HTTP server, all features working

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

## 🚀 Days 1-5: Foundation Testing (VALUE OBJECTS FIRST)

**Philosophy:** Test every building block before building the house.

**Goal:** 240+ tests passing, all value objects proven to work.

**Strategy:** Both focus on testing. Dande learns TDD from Bira. Parallel work on different value objects.

---

### Day 1 - December 29 (Monday) - AGGRESSIVE DAY: All HTTP + Filesystem Value Objects

**Goal:** Test Uri, Host, QueryStringBuilder, Permission, UploadAccess + fix Port (COMBINED PRIORITIES 1 & 2)

**🔥 INTENSIVE WORK DAY:** This is an ambitious plan combining 2 days of work. Take breaks, stay hydrated!

#### Bira's Tasks (10-12 hours) - MARATHON SESSION

**Morning Block 1 (9:00 AM - 1:00 PM): HTTP Value Objects - 4h**

**Uri Testing (4h):**
- [x] Create `tests/unit/test_Uri.cpp`
- [x] **Test Categories:**
  - [x] Construction tests (5 tests)
    ```cpp
    TEST(UriTest, ConstructFromString)
    TEST(UriTest, ConstructFromComponents)
    TEST(UriTest, CopyConstructor)
    TEST(UriTest, AssignmentOperator)
    TEST(UriTest, EmptyUri)
    ```
  - [x] Parsing tests (8 tests)
    ```cpp
    TEST(UriTest, ParseSimpleUri)
    TEST(UriTest, ParseWithQuery)
    TEST(UriTest, ParseWithFragment)
    TEST(UriTest, ParseWithPort)
    TEST(UriTest, ParseWithScheme)
    TEST(UriTest, ParseAbsolutePath)
    TEST(UriTest, ParseRelativePath)
    TEST(UriTest, MalformedUri)
    ```
  - [x] Component extraction (7 tests)
    ```cpp
    TEST(UriTest, GetScheme)
    TEST(UriTest, GetHost)
    TEST(UriTest, GetPort)
    TEST(UriTest, GetPath)
    TEST(UriTest, GetQuery)
    TEST(UriTest, GetFragment)
    TEST(UriTest, ToString)
    ```
- [x] **Total: 55 tests**
- [x] Run tests immediately
- [x] **Commit checkpoint:** "test: Add Uri tests (55 tests)"

**🍽️ LUNCH BREAK (1:00 PM - 2:00 PM) - MANDATORY!**

**Afternoon Block 1 (2:00 PM - 5:00 PM): QueryStringBuilder - 3h**

- [x] Create `tests/unit/test_QueryStringBuilder.cpp`
- [x] **Test Categories:**
  - [x] Construction (3 tests)
    ```cpp
    TEST(QueryStringBuilderTest, EmptyBuilder)
    TEST(QueryStringBuilderTest, ConstructFromString)
    TEST(QueryStringBuilderTest, CopyConstructor)
    ```
  - [x] Building queries (5 tests)
    ```cpp
    TEST(QueryStringBuilderTest, AddParam)
    TEST(QueryStringBuilderTest, AddMultipleParams)
    TEST(QueryStringBuilderTest, BuildQuery)
    TEST(QueryStringBuilderTest, UrlEncoding)
    TEST(QueryStringBuilderTest, SpecialCharacters)
    ```
  - [x] Parsing queries (4 tests)
    ```cpp
    TEST(QueryStringBuilderTest, ParseSimple)
    TEST(QueryStringBuilderTest, ParseMultiple)
    TEST(QueryStringBuilderTest, ParseEncoded)
    TEST(QueryStringBuilderTest, ParseEmpty)
    ```
- [x] **Total: 36 tests**
- [x] Run and verify all pass
- [x] **Commit checkpoint:** "test: Add QueryStringBuilder tests (36 tests)"

**☕ BREAK (5:00 PM - 5:30 PM) - REST YOUR BRAIN!**

**Evening Block 1 (5:30 PM - 9:30 PM): Filesystem Value Objects - 4h**

**Permission Testing (2.5h):**
- [x] Create `tests/unit/test_Permission.cpp`
- [x] **Test Categories:**
  - [x] Construction (4 tests)
  - [x] Permission bits (6 tests) - rwx for owner/group/other
  - [x] Validation (4 tests)
  - [x] Operations (4 tests) - has, add, remove permissions
- [x] **Total: 51 tests**
- [x] **Commit checkpoint:** "test: Add Permission tests (51 tests)"

**UploadAccess Testing (1.5h):**
- [x] Create `tests/unit/test_UploadAccess.cpp`
- [x] **Test Categories:**
  - [x] Construction (3 tests)
  - [x] Validation (4 tests)
  - [x] Access control (3 tests)
- [x] **Total: 52 tests**
- [x] **Commit checkpoint:** "test: Add UploadAccess tests (52 tests)"

**Final Push:**
- [x] Run ALL tests: `./bin/test_runner`
- [x] **Expected: 185 + 194 = 379 tests passing!**
- [x] Push all commits
- [x] Create PR: "test: Add 194 tests for HTTP and Filesystem value objects"
- [x] Update test status documentation

**Expected Output:** 194 new tests passing (Uri: 58, QueryStringBuilder: 36, Permission: 51, UploadAccess: 52)

**🎯 Milestone:** HTTP value objects + Filesystem value objects 100% tested!

---

#### Bira's Tasks (7 hours) - Support & Preparation

**Morning Block (10:00 AM - 1:00 PM): Host Testing - 3h**

- [x] Create `tests/unit/test_Host.cpp`
- [x] **Test Categories:**
  - [x] Construction (13 tests)
  - [x] Assignment (9 tests)
  - [x] Validation (25 tests)
  - [x] Operations (4 tests)
  - [x] Getters (5 tests)
  - [x] Setters (6 tests)
  - [x] Comparison (5 tests)
  - [x] Internal Validation via public API (31 tests)
- [x] **Total: 98 tests** (83 passing, 15 bugs found!)
- [x] **Commit checkpoint:** "feat: tests for Host: 98 written, 15 bugs found"
- [x] **GitHub Actions:** Created unit_Host.yml and updated tests.yml

**Actual Results:** 
- ✅ 98 Host tests created (exceeded 15-test goal by 653%!)
- ✅ Comprehensive coverage of all private methods through public API
- ✅ Found 15 implementation bugs (IPv6 validation, hostname parsing, trailing dots)
- ✅ CI/CD integration complete

**Afternoon Block (2:00 PM - 3:00 PM): Fix Port Tests - 1h**

- [ ] Open `src/domain/http/value_objects/Port.cpp`
- [ ] Fix failing tests:
  - [ ] **DefaultConstructor:** Decide default (0 or 80?)
    ```cpp
    // Current: returns 0
    // Test expects: 80
    // Decision: Change default to 80 or update test?
    ```
  - [ ] **InvalidPortZero:** Make Port(0) throw exception
    ```cpp
    Port::Port(unsigned int value) {
      if (value == 0 || value > 65535)
        throw PortException("Invalid port");
      m_value = value;
    }
    ```
- [ ] Run tests: `./bin/test_runner --gtest_filter='PortTest.*'`
- [ ] Verify: **10/10 tests passing** ✅
- [ ] **Commit:** "fix: Port implementation - now 10/10 tests passing"

**Evening Block (3:00 PM - 6:00 PM): Study & Prepare - 3h**

- [ ] Study TDD from Bira's test code:
  - [ ] Read `tests/unit/test_Path.cpp` - Test structure patterns
  - [ ] Read `tests/unit/test_Size.cpp` - Test organization
  - [ ] Read `tests/unit/test_ErrorCode.cpp` - Assertion patterns
- [ ] Document learnings: What makes a good test?
- [ ] Prepare for Day 2: RegexPattern bug fix strategy
- [ ] Study RegexPattern implementation to understand crash bug
- [ ] Review Bira's PRs as they come in
- [ ] Help Bira with any test failures if needed

**Expected Output:** 98 new tests (Host: 98)

**🎯 Milestone:** HTTP value objects 100% tested! Host implementation bugs documented!

**📊 Day 1 ACTUAL RESULTS (December 29, 2025):**
- ✅ **Uri:** 58 tests created (55 passing, 3 bugs found)
- ✅ **QueryStringBuilder:** 36 tests created (1 segfault + 5 bugs found)
- ✅ **Permission:** 51 tests created (47 passing, 4 bugs found)
- ✅ **UploadAccess:** 52 tests created (52 passing with 5 TODO workarounds)
- ✅ **Host:** 98 tests created (83 passing, 15 bugs found)
- ✅ **Total:** 295 tests created in ONE day!
- ✅ **Test count:** 185 → 482 tests (160% increase!)
- ✅ **Bug hunting:** 28 bugs documented for Day 2
- ✅ **GitHub Actions:** 5 new workflows created
- 🎯 **Strategy success:** Day 1 focused on test creation, not bug fixing

---

### Day 2 - December 30 (Tuesday) - RegexPattern Fix & Configuration Foundation

**Goal:** Fix RegexPattern crash bug, prepare for configuration testing

**⚠️ NOTE:** Day 1 was intensive, so Day 2 is lighter - focus on quality over quantity

#### Dande's Tasks (5-6 hours) - RegexPattern Bug Fix

**Morning (9:00 AM - 12:00 PM): RegexPattern Crash Debug - 3h**

- [ ] **Understand the bug:**
  - [ ] Empty pattern crashes in constructor
  - [ ] Currently excluded from compilation: `! -name 'RegexPattern.cpp'`
  - [ ] Location: `src/shared/utils/RegexPattern.cpp`
- [ ] **Debug strategy:**
  - [ ] Read RegexPattern implementation
  - [ ] Identify crash point (likely missing null check)
  - [ ] Write minimal test to reproduce crash
- [ ] **Fix implementation:**
  - [ ] Add validation for empty patterns
  - [ ] Add proper error handling
  - [ ] Test fix locally

**Afternoon (1:00 PM - 3:00 PM): RegexPattern Testing - 2h**

- [ ] Remove RegexPattern from Makefile exclusions
- [ ] Create `tests/unit/test_RegexPattern.cpp`
- [ ] **Test Categories:**
  - [ ] Construction (4 tests) - including empty pattern
  - [ ] Pattern matching (6 tests)
  - [ ] Edge cases (3 tests)
- [ ] **Total: 13 tests**
- [ ] Run and verify all pass
- [ ] **Commit:** "fix: RegexPattern empty pattern crash + tests (13 tests)"

**Evening (3:00 PM - 4:00 PM): Configuration Directory Prep - 1h**

- [ ] Analyze `tests/Makefile` exclusions
- [ ] Identify what's blocking `domain/configuration/*`
- [ ] Document findings for Day 3
- [ ] Prepare plan to re-enable configuration directory

**Expected Output:** RegexPattern fixed, 13 new tests, path to configuration testing cleared

#### Bira's Tasks (4-5 hours) - Documentation & Review

**Morning (9:00 AM - 11:00 AM): Update Documentation - 2h**

- [ ] Update `TESTING_STATUS.md` with Day 1 results:
  - [ ] 245/247 tests passing (98.8%)
  - [ ] 11/15 value objects tested
  - [ ] HTTP value objects: Uri, Host, QueryStringBuilder, Port ✅
  - [ ] Filesystem value objects: Permission, UploadAccess ✅
- [ ] Update test count in all README files
- [ ] Create summary of Day 1 achievements
- [ ] Document any bugs found during testing

**Afternoon (1:00 PM - 4:00 PM): Code Review & Cleanup - 3h**

- [ ] Review all Day 1 test code for consistency
- [ ] Refactor any duplicated test patterns
- [ ] Improve test readability and documentation
- [ ] Help Dande with RegexPattern if needed
- [ ] Prepare for Day 3: Configuration testing strategy

**Expected Output:** Documentation updated, test code polished, ready for Day 3

---

### Day 2 Evening Checkpoint (Both, 30 min)

**5:00 PM:** Review progress
- [ ] Verify RegexPattern fixed and tested
- [ ] Review documentation updates
- [ ] Merge Day 2 PRs
- [ ] **Test Count:** 245 + 13 = 258 tests passing
- [ ] Plan Day 3: Configuration value objects (ListenDirective, ErrorPage, Route, CgiConfig, UploadConfig)

---

### Day 3 - December 31 (Wensday) - Configuration Value Objects (BLOCKED → UNBLOCKED)

**Expected Output:** 28 new tests passing

---

#### Dande's Tasks (7 hours)

**Morning (3h):** ListenDirective Testing
- [ ] Create `tests/unit/test_ListenDirective.cpp`
- [ ] **Test Categories:**
  - [ ] Construction (4 tests)
  - [ ] Parsing "host:port" (5 tests)
  - [ ] Validation (3 tests)
  - [ ] Operations (3 tests)
- [ ] **Total: 15 tests**

**Afternoon (4h):** Fix RegexPattern (CRITICAL)
- [ ] **Current Issue:** Crashes on empty pattern
- [ ] Open `src/domain/shared/value_objects/RegexPattern.cpp`
- [ ] Find crash: `RegexPatternException("pattern cannot be empty")`
- [ ] **Fix Strategy:**
  ```cpp
  RegexPattern::RegexPattern(const std::string& pattern) {
    if (pattern.empty()) {
      // DON'T throw immediately, set to invalid state
      m_isValid = false;
      m_pattern = "";
      return;
    }
    // ... rest of logic
  }
  ```
- [ ] Update all usages to check `isValid()` before using
- [ ] **Files to update:**
  - `src/domain/configuration/value_objects/Route.cpp`
  - `src/domain/configuration/entities/LocationConfig.cpp`
  - `src/domain/configuration/value_objects/CgiConfig.cpp`
- [ ] Re-enable in tests/Makefile (remove ! -name exclusions)
- [ ] Run all tests
- [ ] **Verify:** ConfigParser no longer crashes

**Expected Output:** 15 tests + RegexPattern fixed (unblocks 10+ files)

---

**Goal:** Test ErrorPage, Route, comprehensive review

#### Bira's Tasks (6 hours)

**Morning (3h):** ErrorPage Testing
- [x] Create `tests/unit/test_ErrorPage.cpp`  
- [x] **Total: 56 tests** (construction, validation, operations)
- [x] **Result: 53/56 passing - DESIGN ISSUE FOUND** ⚠️

**🚨 DESIGN ISSUE DISCOVERED - ErrorPage Default Constructor:**

**Problem:** Default constructor creates invalid state that violates class invariants

```cpp
// ❌ CURRENT BROKEN IMPLEMENTATION:
ErrorPage::ErrorPage() 
    : m_errorCode(500), m_hasContent(false) 
{ 
  // No content generation, no validate() call
  // Creates invalid state: empty page that would fail validate()
}

// ✅ CORRECT IMPLEMENTATION (like ErrorCode constructor):
ErrorPage::ErrorPage() 
    : m_errorCode(500) 
{
  m_content = generateDefaultContent(m_errorCode);
  m_hasContent = true;
  m_contentType = "text/html";
  validate();  // Ensures consistency
}
```

**Why this matters:**
- ErrorCode constructor: Generates content + validates ✅
- Default constructor: No content + no validation ❌
- validate() method: Requires content or file (throws if neither)
- **Result:** Default construction creates invalid object state

**Fix required:** Update `src/domain/configuration/value_objects/ErrorPage.cpp`

**Tests affected:**
- DefaultConstructor (expects content, gets empty)
- ConstructWithEmptyContent (expects throw, gets empty page)
- ValidateThroughConstructorNoContent (expects throw, gets empty page)

**Action:** Dande to fix implementation on Day 3 (5 minutes)

**Afternoon (3h):** Test Documentation
- [ ] Update `tests/README.md` with new test counts
- [ ] Update `tests/docs/TESTING_STATUS.md`
- [ ] Document all tested value objects
- [ ] Create test coverage report

---

#### Dande's Tasks (6 hours)

**Morning (5h):** Route Testing (Complex!)
- [ ] Create `tests/unit/test_Route.cpp`
- [ ] **Test Categories:**
  - [ ] Construction (5 tests)
  - [ ] Pattern matching (8 tests)
  - [ ] HTTP method validation (5 tests)
  - [ ] Parameter extraction (4 tests)
  - [ ] Operations (3 tests)
- [ ] **Total: 25 tests**

**Afternoon (1h):** Review Bira's PRs
- [ ] Code review all value object tests
- [ ] Verify test quality
- [ ] Approve and merge

---

### Days 4-5 - Jan 1-2 - Infrastructure Adapter Testing

**Goal:** Re-enable all infrastructure tests, fix bugs

#### Day 4: Re-enable Infrastructure Tests
- [ ] Remove directory exclusions from tests/Makefile
- [ ] Fix include paths in infrastructure files
- [ ] Test FileHandler, PathResolver, DirectoryLister
- [ ] Fix compilation errors
- [ ] Gradually add files back to build

#### Day 5: Buffer & Integration
- [ ] Fix remaining test failures
- [ ] Ensure all 240+ tests passing
- [ ] Memory leak check with valgrind
- [ ] Documentation complete
- [ ] Ready for server development

**Milestone:** ~250 tests passing, 100% domain layer coverage

---

## 🌐 Days 6-10: Server Development (BUILD ON SOLID FOUNDATION)

**Philosophy:** Now that foundation is proven, build server with confidence.

**Goal:** Functional HTTP server serving GET/POST/DELETE with multi-server config.

**Advantage:** When server bugs appear, you KNOW it's not the value objects!

---

### Day 6 - January 3 (Friday) - Socket Foundation

**Goal:** Server binds to port, accepts connections

#### Bira's Tasks (8 hours) - SocketServer

**Create:** `src/infrastructure/network/SocketServer.hpp/cpp`

**Morning (4h):**
- [ ] **SocketServer class:**
  ```cpp
  class SocketServer {
    int createSocket();
    void bindSocket(int fd, const Host& host, const Port& port);
    void listenSocket(int fd, int backlog);
    int acceptConnection(int listenFd);
    void setNonBlocking(int fd);
    void setReuseAddr(int fd);
    void closeSocket(int fd);
  };
  ```
- [ ] Implement each method
- [ ] Error handling (EADDRINUSE, EACCES)
- [ ] Test: Bind to port 8080

**Afternoon (4h):**
- [ ] **Unit tests:** `tests/unit/test_SocketServer.cpp`
  ```cpp
  TEST(SocketServerTest, CreateSocketReturnsValidFd)
  TEST(SocketServerTest, BindSocketSucceeds)
  TEST(SocketServerTest, ListenSocketSucceeds)
  TEST(SocketServerTest, SetNonBlockingSucceeds)
  TEST(SocketServerTest, SetReuseAddrSucceeds)
  ```
- [ ] Manual test:
  ```bash
  ./webserv test.conf &
  netstat -tuln | grep 8080  # Should show LISTEN
  telnet localhost 8080       # Should connect
  ```

**Expected Output:** Can bind and accept 1 connection

---

#### Dande's Tasks (6 hours) - Response Builder

**Create:** `src/infrastructure/http/ResponseBuilder.hpp/cpp`

**Morning (3h):**
- [ ] **ResponseBuilder class:**
  ```cpp
  class ResponseBuilder {
    void setStatus(const ErrorCode& code);
    void addHeader(const std::string& key, const std::string& value);
    void setBody(const std::string& body);
    std::string build();
    std::string buildStatusLine();
  };
  ```
- [ ] Implement HTTP/1.1 response format
- [ ] Auto-add Content-Length header

**Afternoon (3h):**
- [ ] **Unit tests:** `tests/unit/test_ResponseBuilder.cpp`
  ```cpp
  TEST(ResponseBuilderTest, BuildSimple200Response)
  TEST(ResponseBuilderTest, BuildWithHeaders)
  TEST(ResponseBuilderTest, BuildWithBody)
  TEST(ResponseBuilderTest, Build404Response)
  TEST(ResponseBuilderTest, ContentLengthAutomatic)
  ```
- [ ] Test with curl:
  ```bash
  # Hardcode response in server
  curl -v http://localhost:8080/
  # Should see valid HTTP/1.1 200 OK
  ```

**Expected Output:** Can generate valid HTTP responses

---

### Day 7 - January 4 (Saturday) - Event Loop + GET

**Goal:** Non-blocking server serving static files

#### Bira's Tasks (8 hours) - Epoll Event Loop

**Create:** `src/infrastructure/network/EpollManager.hpp/cpp`

**Morning (4h):**
- [ ] **EpollManager class:**
  ```cpp
  class EpollManager {
    int createEpoll();
    void addFd(int fd, uint32_t events);
    void modifyFd(int fd, uint32_t events);
    void removeFd(int fd);
    std::vector<epoll_event> wait(int timeout);
  };
  ```
- [ ] Implement epoll_create1, epoll_ctl, epoll_wait
- [ ] Handle EPOLLIN, EPOLLOUT, EPOLLERR, EPOLLHUP

**Afternoon (4h):**
- [ ] **HttpServer class:** `src/infrastructure/network/HttpServer.hpp/cpp`
  ```cpp
  class HttpServer {
    void start(const std::vector<ServerConfig>& configs);
    void eventLoop();
    void handleAccept(int listenFd);
    void handleRead(int clientFd);
    void handleWrite(int clientFd);
    void stop();
  };
  ```
- [ ] Integrate SocketServer + EpollManager
- [ ] Basic read/write loop

**Expected Output:** Event loop processing connections

---

#### Dande's Tasks (6 hours) - Static File GET

**Create:** `src/application/handlers/GetHandler.hpp/cpp`

**Morning (3h):**
- [ ] **GetHandler class:**
  ```cpp
  class GetHandler {
    Response handle(const ParsedRequest& req, const ServerConfig& config);
  private:
    std::string readFile(const Path& path);
    std::string detectContentType(const Path& path);
    Response buildFileResponse(const std::string& content, const std::string& type);
  };
  ```
- [ ] Use Dande's FileHandler (already exists!)
- [ ] Content-Type detection (html, css, js, jpg, default)

**Afternoon (3h):**
- [ ] **Integration with server:**
  - [ ] RequestParser parses request
  - [ ] GetHandler serves file
  - [ ] ResponseBuilder formats response
  - [ ] Server sends to client
- [ ] Test:
  ```bash
  echo "Hello World" > var/www/index.html
  curl http://localhost:8080/index.html
  # Should return: Hello World
  ```

**Expected Output:** GET requests serve static files

---

### Day 8 - January 5 (Sunday) - Connection Management + POST/DELETE

**Goal:** All HTTP methods working

#### Bira's Tasks (8 hours) - Connection Manager

**Create:** `src/infrastructure/network/ConnectionManager.hpp/cpp`

**Morning (4h):**
- [ ] **Connection class:**
  ```cpp
  class Connection {
    enum State { READING_REQUEST, PROCESSING, WRITING_RESPONSE, CLOSED };
    State state;
    std::string readBuffer;
    std::string writeBuffer;
    time_t lastActivity;
  };
  
  class ConnectionManager {
    void addConnection(int fd);
    void removeConnection(int fd);
    Connection* getConnection(int fd);
    void processConnection(int fd);
    void cleanupTimeouts();
  };
  ```
- [ ] Track connection state
- [ ] Buffer management
- [ ] Timeout handling (30 seconds)

**Afternoon (4h):**
- [ ] Integrate with HttpServer
- [ ] Handle partial reads/writes
- [ ] Test concurrent connections:
  ```bash
  for i in {1..10}; do curl http://localhost:8080/ & done
  wait
  # All should succeed
  ```

**Expected Output:** Handles 10+ concurrent connections

---

#### Dande's Tasks (6 hours) - POST/DELETE Handlers

**Morning (3h):** POST Upload Handler
- [ ] **PostHandler class:**
  ```cpp
  class PostHandler {
    Response handle(const ParsedRequest& req, const ServerConfig& config);
  private:
    Response saveUpload(const std::vector<char>& body, const Path& uploadPath);
  };
  ```
- [ ] Parse multipart/form-data (simple version)
- [ ] Save file using FileWriter
- [ ] Return 201 Created
- [ ] Test:
  ```bash
  curl -F "file=@test.txt" http://localhost:8080/upload
  ls var/www/uploads/test.txt  # Should exist
  ```

**Afternoon (3h):** DELETE Handler
- [ ] **DeleteHandler class:**
  ```cpp
  class DeleteHandler {
    Response handle(const ParsedRequest& req, const ServerConfig& config);
  private:
    bool validatePath(const Path& path);
  };
  ```
- [ ] Validate file exists
- [ ] Check permissions
- [ ] Remove file using FileHandler
- [ ] Return 204 No Content
- [ ] Test:
  ```bash
  curl -X DELETE http://localhost:8080/test.txt
  # File should be gone
  ```

**Expected Output:** POST uploads, DELETE removes files

---

### Day 9 - January 6 (Monday) - Config Integration + CGI

**Goal:** Multi-server config, basic CGI

#### Dande's Tasks (7 hours) - Config Integration

**Morning (4h):**
- [ ] Integrate ConfigParser with HttpServer
- [ ] Multi-server setup (different ports)
- [ ] Location block routing
- [ ] Error page configuration
- [ ] Test multi-server:
  ```bash
  # Config:
  server { listen 8080; root /var/www1; }
  server { listen 8081; root /var/www2; }
  
  curl http://localhost:8080/  # Server 1
  curl http://localhost:8081/  # Server 2
  ```

**Afternoon (3h):**
- [ ] Error handling polish
- [ ] Proper 404/403/500 responses
- [ ] Client body size limits
- [ ] Timeout enforcement

---

#### Bira's Tasks (6 hours) - Basic CGI

**Create:** `src/infrastructure/cgi/CgiExecutor.hpp/cpp`

**Morning (3h):**
- [ ] **CgiExecutor class:**
  ```cpp
  class CgiExecutor {
    std::string execute(const Path& scriptPath, const ParsedRequest& req);
  private:
    void setupEnvironment(const ParsedRequest& req);
    std::string readFromPipe(int fd);
  };
  ```
- [ ] fork() + execve() for PHP-CGI
- [ ] Environment variables (PATH_INFO, QUERY_STRING)
- [ ] Pipe for stdin/stdout

**Afternoon (3h):**
- [ ] CGI response parsing (headers + body)
- [ ] Timeout (5 seconds using alarm())
- [ ] Test:
  ```bash
  echo '<?php echo "CGI Works"; ?>' > var/www/test.php
  curl http://localhost:8080/test.php
  # Should return: CGI Works
  ```

**Expected Output:** Basic PHP-CGI execution works

---

### Day 10 - January 7 (Tuesday) - FINAL TESTING

**Goal:** Bug-free, leak-free, evaluation-ready

#### Both Together (16 hours total - 8h each)

**Morning (4h each):** Integration Testing
- [ ] Test every feature end-to-end
- [ ] GET files from all configured roots
- [ ] POST uploads to different servers
- [ ] DELETE with permission checks
- [ ] CGI scripts with parameters
- [ ] Error pages (404, 403, 500)
- [ ] Multiple concurrent requests
- [ ] Config file edge cases

**Afternoon (4h each):** Bug Fixes + Valgrind
- [ ] Fix all discovered bugs
- [ ] Valgrind check:
  ```bash
  valgrind --leak-check=full --show-leak-kinds=all ./webserv webserv.conf
  # MUST show: 0 bytes lost
  ```
- [ ] Stress test:
  ```bash
  siege -c 50 -r 100 http://localhost:8080/
  # MUST NOT crash
  ```
- [ ] Memory fixes
- [ ] Edge case handling

**Evening (both):** Documentation & Checklist
- [ ] Update README.md
- [ ] Create evaluation checklist
- [ ] Test with subject requirements PDF
- [ ] Verify every mandatory feature works
- [ ] Commit final version
- [ ] Tag release: `v1.0-evaluation-ready`

**Expected Output:** ✅ READY FOR EVALUATION

---

## ✅ SUCCESS CHECKLIST (January 7 Evening)

### Mandatory Requirements (MUST ALL BE ✅)

**Server Basics:**
- [ ] Binds to multiple ports
- [ ] Accepts connections
- [ ] Non-blocking I/O (epoll/poll)
- [ ] No crashes under normal load
- [ ] No memory leaks (valgrind clean)

**HTTP Methods:**
- [ ] GET serves static files
- [ ] POST accepts file uploads
- [ ] DELETE removes files
- [ ] Proper HTTP/1.1 responses

**Configuration:**
- [ ] Parses nginx-like config
- [ ] Multiple server blocks work
- [ ] Location blocks route correctly
- [ ] Error pages configurable
- [ ] Client body size limit enforced

**CGI:**
- [ ] Executes PHP-CGI scripts
- [ ] Environment variables set correctly
- [ ] Handles CGI output (headers + body)
- [ ] Timeout protection (no infinite scripts)

**Error Handling:**
- [ ] 404 Not Found
- [ ] 403 Forbidden  
- [ ] 500 Internal Server Error
- [ ] 405 Method Not Allowed
- [ ] 413 Payload Too Large

### Testing Verification (Run ALL Before Evaluation)

```bash
# 1. Compilation test
make clean && make
# ✅ Must compile without errors or warnings

# 2. Test suite
cd tests && make && ./bin/test_runner
# ✅ Must show ~250 tests passing

# 3. Basic server test
./webserv webserv.conf &
curl http://localhost:8080/
# ✅ Must return valid HTTP response

# 4. Memory leak test
valgrind --leak-check=full ./webserv webserv.conf
# ✅ Must show "0 bytes lost"

# 5. Concurrent connection test
siege -c 50 -r 100 http://localhost:8080/
# ✅ Must complete without crashes

# 6. CGI test
curl http://localhost:8080/test.php
# ✅ Must execute PHP script

# 7. File operations test
curl -F "file=@test.txt" http://localhost:8080/upload  # Upload
curl http://localhost:8080/test.txt                    # Download
curl -X DELETE http://localhost:8080/test.txt          # Delete
# ✅ All must work correctly
```

**If ANY test fails → FIX BEFORE EVALUATION**

---

## 🎉 YOU'RE READY!

**What you built in 10 days:**
- ✅ ~250 tests (100% value object coverage)
- ✅ Complete HTTP/1.1 server
- ✅ Multi-server configuration
- ✅ GET/POST/DELETE methods
- ✅ Basic CGI execution
- ✅ Non-blocking I/O
- ✅ No memory leaks
- ✅ Evaluation-ready

**Success rate with this plan:** 85%

**Why it worked:** Foundation-first = solid building, fewer bugs, faster development.

**Good luck! You've got this! 🚀**
