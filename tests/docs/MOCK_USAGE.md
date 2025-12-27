# Mock Usage Guide

## 📚 What Are Mocks?

**Mocks** are test doubles that simulate the behavior of real objects. They allow you to:
1. Test components in **isolation** (without dependencies)
2. Control **exact behavior** (force specific inputs/outputs)
3. Test **error cases** (simulate failures)
4. Run tests **fast** (no I/O, no network, no disk)

---

## 🎯 When to Use Mocks

### ✅ Use Mocks When:
- Testing a component that depends on unimplemented code
- You need to control exact input/output
- You want to test error handling
- Real dependencies are slow (network, disk, database)
- You want isolated unit tests

### ❌ Don't Use Mocks When:
- Testing the actual implementation (integration tests)
- The real object is simple (e.g., `std::string`)
- You're testing the interaction between components

---

## 🔧 Available Mocks

### 1. MockServer

**Purpose:** Simulate an HTTP server without real sockets.

**Use Cases:**
- Test server lifecycle (start/stop)
- Test port binding
- Test request routing
- Test connection management

**Example:**
```cpp
#include "mocks/MockServer.hpp"

TEST(ServerTest, StartsOnCorrectPort) {
  mocks::MockServer server;
  server.start(8080);
  
  EXPECT_TRUE(server.isRunning());
  EXPECT_TRUE(server.isListeningOn(8080));
}

TEST(ServerTest, HandlesMultipleRequests) {
  mocks::MockServer server;
  server.handleRequest("GET / HTTP/1.1\\r\\n\\r\\n");
  server.handleRequest("GET /about HTTP/1.1\\r\\n\\r\\n");
  
  EXPECT_EQ(2u, server.getRequestCount());
}
```

---

### 2. MockRequestParser

**Purpose:** Simulate HTTP request parsing without implementing HTTP/1.1.

**Use Cases:**
- Test routing logic with different HTTP methods
- Test header handling
- Test request validation
- Test error handling for malformed requests

**Example:**
```cpp
#include "mocks/MockRequestParser.hpp"

TEST(RouterTest, RoutesGetRequest) {
  mocks::MockRequestParser parser;
  parser.setupGetRequest("/api/users");
  
  Router router(&parser);
  Route* route = router.findRoute();
  
  EXPECT_EQ("/api/users", route->getPath());
}

TEST(RouterTest, RejectsInvalidMethod) {
  mocks::MockRequestParser parser;
  parser.setMethod("INVALID");
  parser.setUri("/");
  
  Router router(&parser);
  
  EXPECT_THROW(router.handleRequest(), InvalidMethodException);
}
```

---

### 3. MockResponseBuilder

**Purpose:** Simulate HTTP response building without formatting HTTP.

**Use Cases:**
- Test correct status codes are set
- Test headers are added correctly
- Test response body content
- Test error page generation

**Example:**
```cpp
#include "mocks/MockResponseBuilder.hpp"

TEST(FileHandlerTest, Returns404ForMissingFile) {
  mocks::MockResponseBuilder builder;
  FileHandler handler(&builder);
  
  handler.serveFile("/nonexistent.html");
  
  EXPECT_EQ(404, builder.getStatusCode());
  EXPECT_TRUE(builder.hasHeader("Content-Type"));
  EXPECT_TRUE(builder.getBody().find("Not Found") != std::string::npos);
}

TEST(FileHandlerTest, SetsCorrectContentType) {
  mocks::MockResponseBuilder builder;
  FileHandler handler(&builder);
  
  handler.serveFile("/index.html");
  
  EXPECT_EQ(200, builder.getStatusCode());
  EXPECT_EQ("text/html", builder.getHeader("Content-Type"));
}
```

---

## 🎓 Mock Patterns

### Pattern 1: Setup-Exercise-Verify

```cpp
TEST(ExampleTest, Pattern) {
  // SETUP: Create mock and configure it
  mocks::MockRequestParser parser;
  parser.setMethod("POST");
  parser.setUri("/upload");
  parser.setBody("file data");
  
  // EXERCISE: Use the mock in your code
  UploadHandler handler(&parser);
  handler.processUpload();
  
  // VERIFY: Check the results
  EXPECT_TRUE(handler.uploadWasSuccessful());
}
```

### Pattern 2: Error Simulation

```cpp
TEST(ExampleTest, HandlesParseError) {
  // SETUP: Force an error
  mocks::MockRequestParser parser;
  parser.setParseError(true);
  
  // EXERCISE
  RequestHandler handler(&parser);
  
  // VERIFY: Check error handling
  EXPECT_THROW(handler.process(), ParseException);
}
```

### Pattern 3: State Verification

```cpp
TEST(ExampleTest, BuildsCorrectResponse) {
  // SETUP
  mocks::MockResponseBuilder builder;
  
  // EXERCISE
  MyHandler handler(&builder);
  handler.handleRequest();
  
  // VERIFY: Check state was modified correctly
  EXPECT_EQ(200, builder.getStatusCode());
  EXPECT_TRUE(builder.hasBody());
  EXPECT_EQ("application/json", builder.getHeader("Content-Type"));
}
```

---

## 🚨 Common Pitfalls

### ❌ Pitfall 1: Testing the Mock Instead of Your Code

```cpp
// WRONG: This tests the mock, not your code
TEST(BadTest, Example) {
  mocks::MockServer server;
  server.start(8080);
  EXPECT_TRUE(server.isRunning());  // Just testing the mock!
}

// RIGHT: This tests YOUR code using the mock
TEST(GoodTest, Example) {
  mocks::MockServer server;
  ServerManager manager(&server);
  
  manager.startServer(8080);  // Testing YOUR code
  EXPECT_TRUE(server.isRunning());  // Verifying it used the server correctly
}
```

### ❌ Pitfall 2: Over-Mocking

```cpp
// WRONG: Mocking everything
TEST(BadTest, Example) {
  MockA a;
  MockB b;
  MockC c;
  MockD d;
  // Too many mocks = not testing real interactions
}

// RIGHT: Mock only external dependencies
TEST(GoodTest, Example) {
  mocks::MockServer server;  // External dependency
  Router router;  // Real implementation
  RouteTable table;  // Real implementation
  
  // Test real interactions between Router and RouteTable
  // Only mock the Server
}
```

### ❌ Pitfall 3: Not Resetting Mocks

```cpp
// WRONG: State carries between tests
MockServer server;  // Class member

TEST(BadTest, First) {
  server.addClient(1);
  EXPECT_EQ(1u, server.getClientCount());
}

TEST(BadTest, Second) {
  EXPECT_EQ(0u, server.getClientCount());  // FAILS! Still has client from First
}

// RIGHT: Reset in SetUp or use fresh mocks
TEST(GoodTest, Example) {
  mocks::MockServer server;  // Fresh mock each test
  // OR
  void SetUp() {
    server.reset();  // Reset shared mock
  }
}
```

---

## 📖 For Your Evaluation

**Question:** "Why did you use mocks in your tests?"

**Answer:** 
"I used mocks to test components in isolation. For example, when testing the Router, I don't need a real HTTP parser - I just need to control what 'request' data is passed in. The MockRequestParser lets me simulate any HTTP request without implementing HTTP parsing first. This makes tests faster, more predictable, and lets me test error cases easily."

**Question:** "What's the difference between a mock and a real object?"

**Answer:**
"A real RequestParser actually parses HTTP strings using complex state machines and validation. A MockRequestParser just stores the data I give it. The real parser is what we use in production. The mock is what we use in tests to isolate the component being tested from its dependencies."

**Question:** "How do mocks help with TDD?"

**Answer:**
"Mocks let me write tests before implementing dependencies. I can define interfaces, create mocks from those interfaces, write tests using the mocks, and implement the real code later. This is the 'London School' of TDD - test behavior, not implementation."

---

## 🎯 Best Practices

1. **Name mocks clearly:** `MockRequestParser`, not `FakeParser` or `TestParser`
2. **Keep mocks simple:** Only implement what you need for tests
3. **Don't add business logic to mocks:** They should be dumb
4. **Use mocks for isolation:** Test one component at a time
5. **Write integration tests too:** Mocks don't test real interactions
6. **Document mock behavior:** Comments explaining what it simulates
7. **Reset state between tests:** Use `reset()` methods or fresh instances

---

## 📚 Further Reading

- Test Doubles: https://martinfowler.com/bliki/TestDouble.html
- Mocking in C++: GoogleTest + GoogleMock
- London vs. Chicago TDD: Different approaches to mocking
