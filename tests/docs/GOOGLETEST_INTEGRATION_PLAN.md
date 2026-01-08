# Test Scripts Comparison & GoogleTest Integration

## Current Implementation Status

### ✅ **Implemented Integration Tests**

1. **test_HttpServer_Integration.cpp** ✅ 
   - HTTP method tests (GET, POST, DELETE, HEAD)
   - Status code validation (200, 404, 405, etc.)
   - Request/response handling
   - 511 lines, 50+ test cases

2. **test_FileHandler_Integration.cpp** ✅ NEW!
   - File upload via POST
   - Large file handling (1MB test)
   - File download via GET
   - File deletion via DELETE
   - Non-existent file error handling
   - 130 lines, 6 test cases

3. **test_CGI_Integration.cpp** ✅
   - CGI script execution
   - Environment variable passing
   - Output handling

4. **test_MultiPort_Integration.cpp** ✅
   - Multiple port listening
   - Port-specific content delivery

5. **test_Advanced_Integration.cpp** ✅
   - Chunked transfer encoding
   - Request timeout
   - Large file operations
   - Connection handling

6. **HttpTestClient.hpp** ✅
   - C++98 compatible HTTP client
   - Raw socket implementation
   - GET, POST, DELETE, HEAD support
   - 288 lines of helper code

### 🔄 **CI/CD Integration**
- GitHub Actions workflow (integration_tests.yml) ✅
- Automated server startup/shutdown ✅
- Valgrind memory leak detection ✅
- Test result reporting ✅

---

## 1. About 42_test_evaluation.sh

---

### **42_test_evaluation.sh** (351 lines)
- **Type:** Automated test suite with logic
- **Structure:** Proper test functions with assertions
- **Output:** Clean colored pass/fail messages
- **Pass/Fail:** Automatic detection with counters
- **Status codes:** Captured and verified automatically
- **Summary:** Pass/fail counts at end
- **User interaction:** "Press Enter to start"
- **Created:** Just now, based on 42 evaluation requirements

**Example:**
```bash
log_test "1.1 GET request"
RESPONSE=$(curl -s -o /dev/null -w "%{http_code}" $SERVER_URL/)
if [ "$RESPONSE" = "200" ]; then
    log_pass "GET / returns 200"
else
    log_fail "GET / returns $RESPONSE (expected 200)"
fi
```

---



## 2. Can We Move Shell Tests to GoogleTest Integration Tests?

### ✅ **YES** - Most Tests Can Be Moved

**What CAN be tested with GoogleTest:**

```cpp
// tests/integration/test_HttpServer_Integration.cpp

class HttpServerIntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Start server in background thread
        server_thread = std::thread([]() {
            // Run server on port 8080
        });
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    
    void TearDown() override {
        // Stop server
        server_thread.join();
    }
    
    std::thread server_thread;
};

TEST_F(HttpServerIntegrationTest, GetRequestReturns200) {
    // Make HTTP request using C++ HTTP client
    HttpClient client("127.0.0.1", 8080);
    Response response = client.get("/");
    
    EXPECT_EQ(response.getStatusCode(), 200);
    EXPECT_TRUE(response.hasHeader("Content-Type"));
}

TEST_F(HttpServerIntegrationTest, PostToRootReturns405) {
    HttpClient client("127.0.0.1", 8080);
    Response response = client.post("/", "test data");
    
    EXPECT_EQ(response.getStatusCode(), 405); // Method Not Allowed
}

TEST_F(HttpServerIntegrationTest, HeadRequestReturns200NoBody) {
    HttpClient client("127.0.0.1", 8080);
    Response response = client.head("/");
    
    EXPECT_EQ(response.getStatusCode(), 200);
    EXPECT_EQ(response.getBody().size(), 0); // No body in HEAD
}

TEST_F(HttpServerIntegrationTest, LargeBodyReturns413) {
    HttpClient client("127.0.0.1", 8080);
    std::string large_body(2 * 1024 * 1024, 'X'); // 2MB
    Response response = client.post("/files", large_body);
    
    EXPECT_EQ(response.getStatusCode(), 413); // Payload Too Large
}

TEST_F(HttpServerIntegrationTest, DeleteFileReturns200) {
    HttpClient client("127.0.0.1", 8080);
    
    // First upload a file
    client.post("/files", "test content");
    
    // Then delete it
    Response response = client.delete_("/files/test.txt");
    
    EXPECT_EQ(response.getStatusCode(), 200);
}

TEST_F(HttpServerIntegrationTest, ConcurrentRequests) {
    std::vector<std::thread> threads;
    std::atomic<int> success_count(0);
    
    for (int i = 0; i < 10; ++i) {
        threads.emplace_back([&success_count]() {
            HttpClient client("127.0.0.1", 8080);
            Response response = client.get("/");
            if (response.getStatusCode() == 200) {
                success_count++;
            }
        });
    }
    
    for (auto& t : threads) {
        t.join();
    }
    
    EXPECT_EQ(success_count, 10); // All 10 should succeed
}
```

---

### **What You Need to Add:**

1. **HTTP Client Library** (for making requests from C++)
   ```cpp
   // tests/integration/HttpClient.hpp
   class HttpClient {
   public:
       HttpClient(const std::string& host, int port);
       Response get(const std::string& path);
       Response post(const std::string& path, const std::string& body);
       Response delete_(const std::string& path);
       Response head(const std::string& path);
   };
   ```

2. **Server Test Fixture** (starts/stops server)
   ```cpp
   class ServerFixture {
       // Starts server in background
       // Waits for it to be ready
       // Provides cleanup
   };
   ```

---

### **Advantages of GoogleTest Integration Tests:**

✅ **Automated** - Run with `./bin/test_runner`  
✅ **CI/CD ready** - GitHub Actions can run them  
✅ **Fast feedback** - Instant pass/fail  
✅ **Assertions** - Better than manual inspection  
✅ **Debuggable** - Can attach gdb to test process  
✅ **Repeatable** - Same results every time  
✅ **Granular** - One test = one failure  

---

### **Disadvantages (SOLVED):**

✅ ~~**Complex setup**~~ - HttpTestClient implemented (288 lines)  
✅ ~~**C++98 limitations**~~ - Raw socket implementation works  
✅ ~~**Threading**~~ - Not needed; server runs separately  
✅ ~~**Sockets**~~ - Raw socket code written and tested  
✅ ~~**Time consuming**~~ - Already implemented!  

---

## 3. Can We Test Siege in GoogleTest?

### ❌ **NO** - Siege Cannot Be in GoogleTest

**Reasons:**

### 1. **Siege is External Tool**
```bash
# Siege is a separate executable, not a C++ library
siege -b -c 10 -r 100 http://127.0.0.1:8080/
```

GoogleTest can only run C++ code, not external executables.

---

### 2. **Siege Tests System-Level Behavior**

Siege tests:
- **Real network load** (1000s of connections)
- **System resource limits** (file descriptors, memory)
- **Operating system scheduler** (process scheduling)
- **Real-world timing** (latency, throughput)

GoogleTest tests:
- **Unit behavior** (single function)
- **Integration behavior** (component interaction)
- **Deterministic results** (same input → same output)

---

### 3. **Siege Requires Running Server**

```cpp
// This doesn't make sense:
TEST(SiegeTest, ServerHandles1000Requests) {
    // Start server
    // Call siege???  ← Can't do this from C++
    // Check results??? ← Siege outputs to terminal
}
```

---

### **What You CAN Do: Stress Test in GoogleTest**

Instead of siege, write your own stress test:

```cpp
TEST(StressTest, Handle1000ConcurrentRequests) {
    const int NUM_CLIENTS = 10;
    const int REQUESTS_PER_CLIENT = 100;
    
    std::vector<std::thread> threads;
    std::atomic<int> success(0);
    std::atomic<int> failed(0);
    
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < NUM_CLIENTS; ++i) {
        threads.emplace_back([&]() {
            for (int j = 0; j < REQUESTS_PER_CLIENT; ++j) {
                HttpClient client("127.0.0.1", 8080);
                Response response = client.get("/");
                
                if (response.getStatusCode() == 200) {
                    success++;
                } else {
                    failed++;
                }
            }
        });
    }
    
    for (auto& t : threads) {
        t.join();
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(end - start);
    
    int total = success + failed;
    double availability = (static_cast<double>(success) / total) * 100.0;
    
    std::cout << "Total requests: " << total << std::endl;
    std::cout << "Success: " << success << std::endl;
    std::cout << "Failed: " << failed << std::endl;
    std::cout << "Availability: " << availability << "%" << std::endl;
    std::cout << "Duration: " << duration.count() << "s" << std::endl;
    
    EXPECT_GE(availability, 99.5); // 99.5% availability required
    EXPECT_EQ(failed, 0); // Ideally zero failures
}
```

**Pros:**
- ✅ Runs in GoogleTest
- ✅ Automated pass/fail
- ✅ CI/CD compatible

**Cons:**
- ❌ Not as comprehensive as siege
- ❌ Doesn't test system limits
- ❌ Doesn't measure real-world performance

---

## 📊 Recommendation Summary

### **Keep Shell Scripts For:**
1. ✅ **Quick manual testing** during development
2. ✅ **Demonstration** during evaluation
3. ✅ **Siege stress tests** (can't be automated in GoogleTest)
4. ✅ **Valgrind checks** (external tool)
5. ✅ **Browser testing** (requires human)

**Use:** `test_42_evaluation.sh` (delete `test_evaluation.sh`)

---

### **✅ COMPLETED - GoogleTest Integration Tests:**
1. ✅ **HTTP method tests** (GET, POST, DELETE, HEAD) - `test_HttpServer_Integration.cpp`
2. ✅ **Status code validation** (200, 404, 405, 413) - `test_HttpServer_Integration.cpp`
3. ✅ **Request/response parsing** - `test_HttpServer_Integration.cpp`
4. ✅ **File upload/download** - `test_FileHandler_Integration.cpp` 🆕
5. ✅ **File deletion** - `test_FileHandler_Integration.cpp` 🆕
6. ✅ **Large file handling** - `test_FileHandler_Integration.cpp` 🆕
7. ✅ **CGI execution** - `test_CGI_Integration.cpp`
8. ✅ **Multi-port support** - `test_MultiPort_Integration.cpp`
9. ✅ **Advanced features** - `test_Advanced_Integration.cpp`
10. ✅ **Error handling** - All test files

**Status:** 5 integration test suites fully implemented!

---

### **✅ Timeline - COMPLETED:**

**What was implemented:**

| Task | Status | Notes |
|------|--------|-------|
| Write HttpTestClient class | ✅ DONE | 288 lines, C++98 compatible |
| Write test fixtures | ✅ DONE | 5 test suites implemented |
| Port tests to GoogleTest | ✅ DONE | 50+ integration tests |
| File upload/download tests | ✅ DONE | test_FileHandler_Integration.cpp |
| CI/CD integration | ✅ DONE | GitHub Actions workflow |
| Valgrind integration | ✅ DONE | Memory leak detection |

**Current Status (Jan 8, 2026):**

✅ **GoogleTest integration tests are DONE and WORKING!**  
✅ **CI/CD pipeline is running on GitHub Actions**  
✅ **All 5 integration test suites passing**  
✅ **File handler tests newly added today**  
✅ **Valgrind memory leak checks integrated**  

**Recommendation NOW:** 
1. ✅ **Keep using GoogleTest** - Already implemented and working
2. ✅ **Use shell script for demos** (`42_test_evaluation.sh`) - Good for evaluation
3. ✅ **Run both** - GoogleTest for CI/CD, shell for manual testing
4. ✅ **Monitor CI/CD** - Automatic testing on every push

**Both approaches are now available!**

---

## 🎯 Current Status & Next Steps

**✅ COMPLETED (Jan 8):**
1. ✅ GoogleTest integration tests fully implemented
2. ✅ HttpTestClient (C++98 HTTP client library)
3. ✅ test_FileHandler_Integration.cpp created
4. ✅ CI/CD pipeline with GitHub Actions
5. ✅ Valgrind integration in workflow

**📊 Test Coverage:**
- 5 integration test suites
- 50+ test cases
- HTTP methods (GET, POST, DELETE, HEAD)
- File operations (upload, download, delete)
- Error handling (404, 405, 413)
- CGI execution
- Multi-port listening
- Advanced features (chunked, timeout)

**🚀 Running Tests:**

```bash
# Run all integration tests
cd tests
./bin/test_runner --gtest_filter="*Integration*"

# Run only file handler tests
./bin/test_runner --gtest_filter="FileHandlerIntegrationTest.*"

# Run with specific server
# Start server: ./bin/webserv conf/default.conf
# Then run tests in another terminal
```

**🎓 For Evaluation:**
- Use shell script for manual demonstration
- GoogleTest runs automatically in CI/CD
- Show both approaches to evaluator
- Mention comprehensive test coverage
