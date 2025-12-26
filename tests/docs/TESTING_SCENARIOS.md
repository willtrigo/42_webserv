# Testing Scenarios Reference

This document provides detailed test scenarios for all components of the 42_webserver project, organized by implementation phase and architectural layer.

**Related Documentation:**
- [TESTING_STRATEGY.md](TESTING_STRATEGY.md) - Implementation strategy and priorities
- [GOOGLETEST_GUIDE.md](GOOGLETEST_GUIDE.md) - Google Test framework usage
- [TESTING_STATUS.md](TESTING_STATUS.md) - Current implementation status

---

## 🔴 Phase 1: Domain Layer - Value Objects

### ErrorCode (CRITICAL - Implement First!)

**Priority:** 🔴 Critical (blocks 10+ components)  
**Test Type:** Pure unit tests (no dependencies)  
**Coverage Target:** 90%+

```cpp
// Valid HTTP status codes
TEST(ErrorCodeTest, ValidStatusCodes) {
  EXPECT_NO_THROW(ErrorCode(200));  // OK
  EXPECT_NO_THROW(ErrorCode(301));  // Moved Permanently
  EXPECT_NO_THROW(ErrorCode(404));  // Not Found
  EXPECT_NO_THROW(ErrorCode(500));  // Internal Server Error
}

// fromString() parsing
TEST(ErrorCodeTest, ParseFromString) {
  ErrorCode code = ErrorCode::fromString("404");
  EXPECT_EQ(404, code.getValue());
  
  EXPECT_THROW(ErrorCode::fromString("999"), InvalidStatusCodeException);
}

// isRedirection() for 3xx codes
TEST(ErrorCodeTest, IsRedirection) {
  EXPECT_TRUE(ErrorCode(301).isRedirection());
  EXPECT_TRUE(ErrorCode(302).isRedirection());
  EXPECT_FALSE(ErrorCode(200).isRedirection());
  EXPECT_FALSE(ErrorCode(404).isRedirection());
}

// movedPermanently() returns 301
TEST(ErrorCodeTest, MovedPermanently) {
  ErrorCode code = ErrorCode::movedPermanently();
  EXPECT_EQ(301, code.getValue());
}

// Default error page mapping
TEST(ErrorCodeTest, DefaultErrorPages) {
  ErrorCode code(404);
  EXPECT_EQ("/error_pages/404.html", code.getDefaultErrorPage());
}

// Invalid status codes
TEST(ErrorCodeTest, InvalidStatusCodes) {
  EXPECT_THROW(ErrorCode(0), InvalidStatusCodeException);
  EXPECT_THROW(ErrorCode(99), InvalidStatusCodeException);
  EXPECT_THROW(ErrorCode(600), InvalidStatusCodeException);
}

// Status code categories
TEST(ErrorCodeTest, StatusCategories) {
  EXPECT_TRUE(ErrorCode(200).isSuccess());
  EXPECT_TRUE(ErrorCode(301).isRedirection());
  EXPECT_TRUE(ErrorCode(404).isClientError());
  EXPECT_TRUE(ErrorCode(500).isServerError());
}
```

---

### Path (Important for File Operations)

**Priority:** 🟡 Important  
**Test Type:** Pure unit tests  
**Coverage Target:** 90%+

```cpp
// Valid absolute paths
TEST(PathTest, ValidAbsolutePaths) {
  EXPECT_NO_THROW(Path("/"));
  EXPECT_NO_THROW(Path("/home/user"));
  EXPECT_NO_THROW(Path("/var/www/html/index.html"));
}

// Invalid relative paths when absolute required
TEST(PathTest, RejectRelativePaths) {
  EXPECT_THROW(Path("relative/path"), InvalidPathException);
  EXPECT_THROW(Path("../parent"), InvalidPathException);
  EXPECT_THROW(Path("./current"), InvalidPathException);
}

// Path normalization
TEST(PathTest, PathNormalization) {
  Path path("/foo/../bar");
  EXPECT_EQ("/bar", path.normalize());
  
  Path path2("/a/./b");
  EXPECT_EQ("/a/b", path2.normalize());
  
  Path path3("/x/y/../z");
  EXPECT_EQ("/x/z", path3.normalize());
}

// Security validation (reject path traversal)
TEST(PathTest, RejectPathTraversal) {
  EXPECT_THROW(Path("/var/www/../../etc/passwd"), SecurityException);
  EXPECT_THROW(Path("/upload/../../../root"), SecurityException);
}

// Extract directory, filename, extension
TEST(PathTest, PathComponents) {
  Path path("/var/www/html/index.html");
  
  EXPECT_EQ("/var/www/html", path.getDirectory());
  EXPECT_EQ("index.html", path.getFilename());
  EXPECT_EQ(".html", path.getExtension());
  EXPECT_EQ("index", path.getBasename());
}

// Path joining
TEST(PathTest, JoinPaths) {
  Path base("/var/www");
  Path joined = base.join("html/index.html");
  EXPECT_EQ("/var/www/html/index.html", joined.toString());
}

// Empty and root paths
TEST(PathTest, SpecialPaths) {
  EXPECT_THROW(Path(""), EmptyPathException);
  
  Path root("/");
  EXPECT_EQ("/", root.getDirectory());
  EXPECT_EQ("", root.getFilename());
}
```

---

### Port (Update Tests to Match Implementation)

**Priority:** 🔴 Critical  
**Test Type:** Pure unit tests  
**Coverage Target:** 90%+

```cpp
// Valid port ranges (1-65535)
TEST(PortTest, ValidPortRanges) {
  EXPECT_NO_THROW(Port(1));
  EXPECT_NO_THROW(Port(80));
  EXPECT_NO_THROW(Port(443));
  EXPECT_NO_THROW(Port(8080));
  EXPECT_NO_THROW(Port(65535));
}

// Invalid port 0 and > 65535
TEST(PortTest, InvalidPorts) {
  EXPECT_THROW(Port(0), InvalidPortException);
  EXPECT_THROW(Port(65536), InvalidPortException);
  EXPECT_THROW(Port(100000), InvalidPortException);
}

// Default port behavior
TEST(PortTest, DefaultPort) {
  Port port;
  // TODO: Decide if default should be 0 or 80
  // Current implementation returns 0
  // EXPECT_EQ(80, port.getValue());  // Or:
  EXPECT_EQ(0, port.getValue());
}

// Well-known ports
TEST(PortTest, WellKnownPorts) {
  Port http(80);
  EXPECT_TRUE(http.isWellKnown());
  
  Port https(443);
  EXPECT_TRUE(https.isWellKnown());
  
  Port custom(8080);
  EXPECT_FALSE(custom.isWellKnown());
}

// Port comparison
TEST(PortTest, PortComparison) {
  Port port1(80);
  Port port2(80);
  Port port3(443);
  
  EXPECT_EQ(port1, port2);
  EXPECT_NE(port1, port3);
}

// Multiple port parsing for config
TEST(PortTest, ParseMultiplePorts) {
  std::vector<Port> ports = Port::parseList("80,443,8080");
  EXPECT_EQ(3, ports.size());
  EXPECT_EQ(80, ports[0].getValue());
  EXPECT_EQ(443, ports[1].getValue());
  EXPECT_EQ(8080, ports[2].getValue());
}
```

---

### Uri (Complete URL Handling)

**Priority:** 🟡 Important  
**Test Type:** Pure unit tests  
**Coverage Target:** 85%+

```cpp
// Valid URI parsing
TEST(UriTest, ParseValidUri) {
  Uri uri("http://example.com:8080/path?query=value#fragment");
  
  EXPECT_EQ("http", uri.getScheme());
  EXPECT_EQ("example.com", uri.getHost());
  EXPECT_EQ(8080, uri.getPort());
  EXPECT_EQ("/path", uri.getPath());
  EXPECT_EQ("query=value", uri.getQuery());
  EXPECT_EQ("fragment", uri.getFragment());
}

// Authority end detection
TEST(UriTest, FindAuthorityEnd) {
  Uri uri("http://user:pass@host:8080/path");
  EXPECT_EQ("user:pass@host:8080", uri.getAuthority());
}

// Missing components
TEST(UriTest, OptionalComponents) {
  Uri uri("http://example.com/path");
  EXPECT_EQ(80, uri.getPort());  // Default HTTP port
  EXPECT_TRUE(uri.getQuery().empty());
  EXPECT_TRUE(uri.getFragment().empty());
}

// Relative URIs
TEST(UriTest, RelativeUri) {
  Uri uri("/path/to/resource");
  EXPECT_TRUE(uri.isRelative());
  EXPECT_EQ("/path/to/resource", uri.getPath());
}
```

---

### Size (Max Body Size Validation)

**Priority:** 🟡 Important  
**Test Type:** Pure unit tests  
**Coverage Target:** 85%+

```cpp
// Valid size parsing
TEST(SizeTest, ParseValidSizes) {
  EXPECT_EQ(1024, Size::parse("1K").getBytes());
  EXPECT_EQ(1048576, Size::parse("1M").getBytes());
  EXPECT_EQ(1073741824, Size::parse("1G").getBytes());
}

// Body size limit validation
TEST(SizeTest, BodySizeLimit) {
  Size limit = Size::parse("10M");
  
  EXPECT_TRUE(limit.isWithinLimit(Size::parse("5M")));
  EXPECT_FALSE(limit.isWithinLimit(Size::parse("15M")));
}

// Invalid size formats
TEST(SizeTest, InvalidFormats) {
  EXPECT_THROW(Size::parse("ABC"), InvalidSizeException);
  EXPECT_THROW(Size::parse("-10M"), InvalidSizeException);
}
```

---

## 🔴 Phase 2: Infrastructure Layer - Core Server

### I/O Multiplexing (poll/select/epoll)

**Priority:** 🔴 Critical (blocks everything)  
**Test Type:** Integration tests  
**Coverage Target:** 75%+

```cpp
// Single socket monitoring
TEST(IOMultiplexerTest, SingleSocket) {
  IOMultiplexer mux;
  int sock = createTestSocket();
  
  mux.addSocket(sock, POLLIN);
  
  // Simulate data available
  writeToSocket(sock, "test");
  
  std::vector<int> ready = mux.poll(1000);  // 1 second timeout
  EXPECT_EQ(1, ready.size());
  EXPECT_EQ(sock, ready[0]);
}

// Multiple sockets simultaneously
TEST(IOMultiplexerTest, MultipleSockets) {
  IOMultiplexer mux;
  int sock1 = createTestSocket();
  int sock2 = createTestSocket();
  int sock3 = createTestSocket();
  
  mux.addSocket(sock1, POLLIN);
  mux.addSocket(sock2, POLLIN);
  mux.addSocket(sock3, POLLIN);
  
  writeToSocket(sock1, "data");
  writeToSocket(sock3, "data");
  
  std::vector<int> ready = mux.poll(1000);
  EXPECT_EQ(2, ready.size());
  EXPECT_TRUE(contains(ready, sock1));
  EXPECT_TRUE(contains(ready, sock3));
}

// Timeout handling
TEST(IOMultiplexerTest, TimeoutBehavior) {
  IOMultiplexer mux;
  int sock = createTestSocket();
  mux.addSocket(sock, POLLIN);
  
  // No data, should timeout
  std::vector<int> ready = mux.poll(100);  // 100ms
  EXPECT_TRUE(ready.empty());
}

// Add/remove sockets dynamically
TEST(IOMultiplexerTest, DynamicSocketManagement) {
  IOMultiplexer mux;
  int sock = createTestSocket();
  
  mux.addSocket(sock, POLLIN);
  EXPECT_EQ(1, mux.getSocketCount());
  
  mux.removeSocket(sock);
  EXPECT_EQ(0, mux.getSocketCount());
}

// Non-blocking behavior
TEST(IOMultiplexerTest, NonBlocking) {
  IOMultiplexer mux;
  int sock = createTestSocket();
  
  EXPECT_TRUE(isNonBlocking(sock));
  
  mux.addSocket(sock, POLLIN);
  std::vector<int> ready = mux.poll(0);  // Should return immediately
  // Test completes without blocking
}
```

---

### Request Parser

**Priority:** 🔴 Critical  
**Test Type:** Unit + Integration  
**Coverage Target:** 85%+

```cpp
// Parse valid GET request
TEST(RequestParserTest, ParseGetRequest) {
  std::string raw = "GET /index.html HTTP/1.1\r\n"
                    "Host: example.com\r\n"
                    "\r\n";
  
  HttpRequest request = RequestParser::parse(raw);
  
  EXPECT_EQ("GET", request.getMethod());
  EXPECT_EQ("/index.html", request.getPath());
  EXPECT_EQ("HTTP/1.1", request.getVersion());
  EXPECT_EQ("example.com", request.getHeader("Host"));
}

// Parse POST with body
TEST(RequestParserTest, ParsePostRequest) {
  std::string raw = "POST /upload HTTP/1.1\r\n"
                    "Host: example.com\r\n"
                    "Content-Length: 11\r\n"
                    "\r\n"
                    "Hello World";
  
  HttpRequest request = RequestParser::parse(raw);
  
  EXPECT_EQ("POST", request.getMethod());
  EXPECT_EQ("/upload", request.getPath());
  EXPECT_EQ("Hello World", request.getBody());
}

// Handle chunked encoding
TEST(RequestParserTest, ChunkedEncoding) {
  std::string raw = "POST /api HTTP/1.1\r\n"
                    "Host: example.com\r\n"
                    "Transfer-Encoding: chunked\r\n"
                    "\r\n"
                    "5\r\n"
                    "Hello\r\n"
                    "6\r\n"
                    " World\r\n"
                    "0\r\n"
                    "\r\n";
  
  HttpRequest request = RequestParser::parse(raw);
  EXPECT_EQ("Hello World", request.getBody());
}

// Detect malformed requests
TEST(RequestParserTest, MalformedRequests) {
  EXPECT_THROW(RequestParser::parse("INVALID"), MalformedRequestException);
  EXPECT_THROW(RequestParser::parse("GET"), MalformedRequestException);
  EXPECT_THROW(RequestParser::parse("GET /path"), MalformedRequestException);
}

// Header parsing
TEST(RequestParserTest, HeaderParsing) {
  std::string raw = "GET / HTTP/1.1\r\n"
                    "Host: example.com\r\n"
                    "Content-Length: 100\r\n"
                    "Content-Type: application/json\r\n"
                    "\r\n";
  
  HttpRequest request = RequestParser::parse(raw);
  
  EXPECT_EQ("example.com", request.getHeader("Host"));
  EXPECT_EQ("100", request.getHeader("Content-Length"));
  EXPECT_EQ("application/json", request.getHeader("Content-Type"));
}
```

---

### Response Builder

**Priority:** 🔴 Critical  
**Test Type:** Unit tests  
**Coverage Target:** 85%+

```cpp
// Build 200 OK response
TEST(ResponseBuilderTest, OkResponse) {
  HttpResponse response = ResponseBuilder()
    .setStatus(200)
    .setBody("Hello World")
    .build();
  
  std::string raw = response.toString();
  
  EXPECT_TRUE(raw.find("HTTP/1.1 200 OK") != std::string::npos);
  EXPECT_TRUE(raw.find("Content-Length: 11") != std::string::npos);
  EXPECT_TRUE(raw.find("Hello World") != std::string::npos);
}

// Build 404 Not Found with error page
TEST(ResponseBuilderTest, NotFoundResponse) {
  std::string errorPage = "<html><body>404 Not Found</body></html>";
  
  HttpResponse response = ResponseBuilder()
    .setStatus(404)
    .setBody(errorPage)
    .build();
  
  std::string raw = response.toString();
  EXPECT_TRUE(raw.find("HTTP/1.1 404 Not Found") != std::string::npos);
}

// Add headers
TEST(ResponseBuilderTest, ResponseHeaders) {
  HttpResponse response = ResponseBuilder()
    .setStatus(200)
    .setHeader("Content-Type", "text/html")
    .setHeader("Server", "webserv/1.0")
    .build();
  
  EXPECT_EQ("text/html", response.getHeader("Content-Type"));
  EXPECT_EQ("webserv/1.0", response.getHeader("Server"));
}

// Handle chunked responses
TEST(ResponseBuilderTest, ChunkedResponse) {
  HttpResponse response = ResponseBuilder()
    .setStatus(200)
    .setHeader("Transfer-Encoding", "chunked")
    .setBody("Hello World")
    .build();
  
  std::string raw = response.toString();
  EXPECT_TRUE(raw.find("Transfer-Encoding: chunked") != std::string::npos);
}
```

---

## 🟡 Phase 3: Infrastructure Layer - Configuration

### ConfigParser

**Priority:** 🟡 Important  
**Test Type:** Unit + Integration  
**Coverage Target:** 80%+

```cpp
// Parse single server block
TEST(ConfigParserTest, SingleServer) {
  std::string config = 
    "server {\n"
    "  listen 80;\n"
    "  server_name example.com;\n"
    "}\n";
  
  ConfigParser parser;
  ServerConfig server = parser.parse(config);
  
  EXPECT_EQ(80, server.getPort());
  EXPECT_EQ("example.com", server.getServerName());
}

// Parse multiple servers on different ports
TEST(ConfigParserTest, MultipleServers) {
  std::string config = 
    "server {\n"
    "  listen 80;\n"
    "}\n"
    "server {\n"
    "  listen 443;\n"
    "}\n";
  
  ConfigParser parser;
  std::vector<ServerConfig> servers = parser.parseAll(config);
  
  EXPECT_EQ(2, servers.size());
  EXPECT_EQ(80, servers[0].getPort());
  EXPECT_EQ(443, servers[1].getPort());
}

// Parse location blocks with routes
TEST(ConfigParserTest, LocationBlocks) {
  std::string config = 
    "server {\n"
    "  location /api {\n"
    "    allowed_methods GET POST;\n"
    "  }\n"
    "}\n";
  
  ConfigParser parser;
  ServerConfig server = parser.parse(config);
  Route route = server.getRoute("/api");
  
  EXPECT_TRUE(route.isMethodAllowed("GET"));
  EXPECT_TRUE(route.isMethodAllowed("POST"));
  EXPECT_FALSE(route.isMethodAllowed("DELETE"));
}

// Handle syntax errors gracefully
TEST(ConfigParserTest, SyntaxErrors) {
  EXPECT_THROW(parser.parse("server {"), SyntaxErrorException);
  EXPECT_THROW(parser.parse("server { listen; }"), SyntaxErrorException);
}

// Load default error pages
TEST(ConfigParserTest, ErrorPages) {
  std::string config = 
    "server {\n"
    "  error_page 404 /errors/404.html;\n"
    "}\n";
  
  ConfigParser parser;
  ServerConfig server = parser.parse(config);
  
  EXPECT_EQ("/errors/404.html", server.getErrorPage(404));
}

// Parse CGI configuration
TEST(ConfigParserTest, CgiConfiguration) {
  std::string config = 
    "server {\n"
    "  location /*.php {\n"
    "    cgi_pass /usr/bin/php-cgi;\n"
    "  }\n"
    "}\n";
  
  ConfigParser parser;
  ServerConfig server = parser.parse(config);
  
  EXPECT_TRUE(server.hasCgi(".php"));
  EXPECT_EQ("/usr/bin/php-cgi", server.getCgiExecutor(".php"));
}

// Validate max body size
TEST(ConfigParserTest, MaxBodySize) {
  std::string config = 
    "server {\n"
    "  client_max_body_size 10M;\n"
    "}\n";
  
  ConfigParser parser;
  ServerConfig server = parser.parse(config);
  
  EXPECT_EQ(10485760, server.getMaxBodySize());  // 10MB in bytes
}
```

---

### Route Entity

**Priority:** 🟡 Important (depends on ErrorCode!)  
**Test Type:** Unit tests  
**Coverage Target:** 85%+

```cpp
// Match exact path
TEST(RouteTest, ExactPathMatch) {
  Route route("/api/users", Route::EXACT);
  
  EXPECT_TRUE(route.matches("/api/users"));
  EXPECT_FALSE(route.matches("/api/users/123"));
  EXPECT_FALSE(route.matches("/api"));
}

// Match prefix path
TEST(RouteTest, PrefixPathMatch) {
  Route route("/static", Route::PREFIX);
  
  EXPECT_TRUE(route.matches("/static"));
  EXPECT_TRUE(route.matches("/static/"));
  EXPECT_TRUE(route.matches("/static/css/style.css"));
  EXPECT_FALSE(route.matches("/api"));
}

// Allow/deny HTTP methods per route
TEST(RouteTest, MethodFiltering) {
  Route route("/api");
  route.allowMethod("GET");
  route.allowMethod("POST");
  
  EXPECT_TRUE(route.isMethodAllowed("GET"));
  EXPECT_TRUE(route.isMethodAllowed("POST"));
  EXPECT_FALSE(route.isMethodAllowed("DELETE"));
}

// Apply default error pages
TEST(RouteTest, CustomErrorPages) {
  Route route("/");
  route.setErrorPage(ErrorCode(404), "/custom_404.html");
  
  EXPECT_EQ("/custom_404.html", route.getErrorPage(ErrorCode(404)));
}

// HTTP redirections
TEST(RouteTest, Redirections) {
  Route route("/old-path");
  route.setRedirect(ErrorCode(301), "/new-path");
  
  EXPECT_TRUE(route.hasRedirect());
  EXPECT_EQ("/new-path", route.getRedirectTarget());
  EXPECT_EQ(301, route.getRedirectStatus().getValue());
}
```

---

## 🟡 Phase 4: Infrastructure Layer - File Operations

### FileHandler Integration

**Priority:** 🟡 Important (needs architecture fix!)  
**Test Type:** Integration tests  
**Coverage Target:** 75%+

```cpp
// Create and write file
TEST(FileHandlerTest, CreateAndWrite) {
  FileHandler handler;
  
  EXPECT_NO_THROW(handler.write("/tmp/test.txt", "Hello World"));
  EXPECT_TRUE(handler.exists("/tmp/test.txt"));
}

// Read existing file
TEST(FileHandlerTest, ReadFile) {
  FileHandler handler;
  handler.write("/tmp/test.txt", "Test content");
  
  std::string content = handler.read("/tmp/test.txt");
  EXPECT_EQ("Test content", content);
}

// Append to file
TEST(FileHandlerTest, AppendToFile) {
  FileHandler handler;
  handler.write("/tmp/test.txt", "Line 1\n");
  handler.append("/tmp/test.txt", "Line 2\n");
  
  std::string content = handler.read("/tmp/test.txt");
  EXPECT_EQ("Line 1\nLine 2\n", content);
}

// Copy file
TEST(FileHandlerTest, CopyFile) {
  FileHandler handler;
  handler.write("/tmp/source.txt", "Original");
  handler.copy("/tmp/source.txt", "/tmp/dest.txt");
  
  EXPECT_EQ("Original", handler.read("/tmp/dest.txt"));
}

// Delete file
TEST(FileHandlerTest, DeleteFile) {
  FileHandler handler;
  handler.write("/tmp/test.txt", "Delete me");
  
  EXPECT_TRUE(handler.exists("/tmp/test.txt"));
  handler.remove("/tmp/test.txt");
  EXPECT_FALSE(handler.exists("/tmp/test.txt"));
}

// Get file metadata
TEST(FileHandlerTest, FileMetadata) {
  FileHandler handler;
  handler.write("/tmp/test.txt", "Test");
  
  FileInfo info = handler.getInfo("/tmp/test.txt");
  EXPECT_EQ(4, info.size);  // "Test" = 4 bytes
  EXPECT_TRUE(info.isRegularFile);
}

// Handle permission denied
TEST(FileHandlerTest, PermissionDenied) {
  FileHandler handler;
  
  EXPECT_THROW(handler.read("/root/secret.txt"), PermissionDeniedException);
}

// Handle file not found
TEST(FileHandlerTest, FileNotFound) {
  FileHandler handler;
  
  EXPECT_THROW(handler.read("/nonexistent.txt"), FileNotFoundException);
}
```

---

### Static File Serving

**Priority:** 🟡 Important (needs server + FileHandler)  
**Test Type:** Integration tests  
**Coverage Target:** 75%+

```cpp
// GET /index.html returns file content
TEST(StaticFilesTest, ServeIndexHtml) {
  HttpRequest request("GET", "/index.html");
  HttpResponse response = server.handle(request);
  
  EXPECT_EQ(200, response.getStatus());
  EXPECT_TRUE(response.getBody().find("<html>") != std::string::npos);
  EXPECT_EQ("text/html", response.getHeader("Content-Type"));
}

// GET /missing.html returns 404
TEST(StaticFilesTest, MissingFile) {
  HttpRequest request("GET", "/missing.html");
  HttpResponse response = server.handle(request);
  
  EXPECT_EQ(404, response.getStatus());
}

// GET directory with index.html
TEST(StaticFilesTest, DirectoryWithIndex) {
  HttpRequest request("GET", "/docs/");
  HttpResponse response = server.handle(request);
  
  EXPECT_EQ(200, response.getStatus());
  // Should serve /docs/index.html
}

// GET directory without index.html
TEST(StaticFilesTest, DirectoryWithoutIndex) {
  HttpRequest request("GET", "/empty/");
  HttpResponse response = server.handle(request);
  
  // Either 404 or directory listing
  EXPECT_TRUE(response.getStatus() == 404 || response.getStatus() == 200);
}

// Handle large files
TEST(StaticFilesTest, LargeFile) {
  HttpRequest request("GET", "/large.bin");
  HttpResponse response = server.handle(request);
  
  EXPECT_EQ(200, response.getStatus());
  EXPECT_GT(response.getBody().size(), 1000000);  // > 1MB
}

// Content-Type headers
TEST(StaticFilesTest, ContentTypeHeaders) {
  HttpRequest html("GET", "/page.html");
  EXPECT_EQ("text/html", server.handle(html).getHeader("Content-Type"));
  
  HttpRequest css("GET", "/style.css");
  EXPECT_EQ("text/css", server.handle(css).getHeader("Content-Type"));
  
  HttpRequest js("GET", "/script.js");
  EXPECT_EQ("application/javascript", server.handle(js).getHeader("Content-Type"));
}
```

---

## 🟢 Phase 5: Infrastructure Layer - CGI

### CGI Execution

**Priority:** 🟢 Enhancement  
**Test Type:** Integration tests  
**Coverage Target:** 70%+

```cpp
// Execute PHP-CGI script
TEST(CgiTest, ExecutePhpScript) {
  HttpRequest request("GET", "/test.php");
  HttpResponse response = server.handle(request);
  
  EXPECT_EQ(200, response.getStatus());
  EXPECT_TRUE(response.getBody().find("<?php") == std::string::npos);  // Executed, not raw
}

// Pass environment variables correctly
TEST(CgiTest, EnvironmentVariables) {
  HttpRequest request("GET", "/env.php?key=value");
  request.setHeader("User-Agent", "TestClient");
  
  HttpResponse response = server.handle(request);
  
  // Script should have access to QUERY_STRING, HTTP_USER_AGENT, etc.
  EXPECT_TRUE(response.getBody().find("key=value") != std::string::npos);
}

// Handle CGI output (headers + body)
TEST(CgiTest, ParseCgiOutput) {
  // CGI returns: "Content-Type: text/html\r\n\r\n<html>...</html>"
  HttpResponse response = executeCgi("/test.php");
  
  EXPECT_EQ("text/html", response.getHeader("Content-Type"));
  EXPECT_TRUE(response.getBody().find("<html>") != std::string::npos);
}

// Detect EOF from CGI
TEST(CgiTest, DetectEof) {
  // CGI closes output pipe
  HttpResponse response = executeCgi("/eof_test.php");
  
  EXPECT_FALSE(response.getBody().empty());
  // No timeout, proper EOF detection
}

// Execute in correct directory
TEST(CgiTest, WorkingDirectory) {
  HttpRequest request("GET", "/cgi-bin/pwd.sh");
  HttpResponse response = server.handle(request);
  
  EXPECT_TRUE(response.getBody().find("/cgi-bin") != std::string::npos);
}

// Handle chunked request before CGI
TEST(CgiTest, ChunkedRequestToCgi) {
  HttpRequest request("POST", "/upload.php");
  request.setHeader("Transfer-Encoding", "chunked");
  request.setBody("5\r\nHello\r\n0\r\n\r\n");
  
  HttpResponse response = server.handle(request);
  
  // CGI should receive "Hello", not chunked format
  EXPECT_EQ(200, response.getStatus());
}

// Timeout after 30 seconds
TEST(CgiTest, TimeoutHandling) {
  HttpRequest request("GET", "/infinite_loop.php");
  
  auto start = std::time(NULL);
  HttpResponse response = server.handle(request);
  auto duration = std::time(NULL) - start;
  
  EXPECT_EQ(500, response.getStatus());  // Gateway Timeout
  EXPECT_LE(duration, 35);  // Should timeout around 30 seconds
}

// Handle CGI error (500)
TEST(CgiTest, CgiError) {
  HttpRequest request("GET", "/error.php");
  HttpResponse response = server.handle(request);
  
  EXPECT_EQ(500, response.getStatus());
}
```

---

## 🟢 Phase 6: Quality Assurance & Testing

### Valgrind - Memory Leak Detection

**Priority:** 🟡 Important (42 evaluation requirement)  
**Test Type:** Manual + CI integration  
**Target:** Zero leaks

```bash
# Run server with Valgrind
valgrind --leak-check=full \
         --show-leak-kinds=all \
         --track-origins=yes \
         --log-file=valgrind.log \
         ./bin/webserv test.conf

# Expected output:
# LEAK SUMMARY:
#    definitely lost: 0 bytes in 0 blocks
#    indirectly lost: 0 bytes in 0 blocks
#      possibly lost: 0 bytes in 0 blocks
#    still reachable: 0 bytes in 0 blocks

# Test with client connections
valgrind --leak-check=full ./bin/webserv test.conf &
SERVER_PID=$!

curl http://localhost:8080/
curl -X POST -d "data" http://localhost:8080/upload
curl -X DELETE http://localhost:8080/file.txt

kill $SERVER_PID

# Check valgrind.log for leaks
```

---

### Siege - Load Testing

**Priority:** 🟡 Important (99.5% availability test)  
**Test Type:** Integration  
**Target:** 99.5%+ availability

```bash
# Start server
./bin/webserv test.conf &
SERVER_PID=$!

# Run Siege stress test
siege -b -t 30S -c 10 http://localhost:8080/

# Expected output:
# Transactions:                   2500 hits
# Availability:                  99.96 %      <- Must be >= 99.5%
# Elapsed time:                  29.99 secs
# Data transferred:               5.23 MB
# Response time:                  0.12 secs
# Transaction rate:              83.36 trans/sec
# Throughput:                     0.17 MB/sec
# Concurrency:                    9.92
# Successful transactions:        2500
# Failed transactions:               1

# Advanced test scenarios
siege -b -t 60S -c 50 http://localhost:8080/             # High concurrency
siege -f urls.txt -c 20 -r 100                           # Multiple URLs
siege -b -t 30S -c 10 http://localhost:8080/upload.php   # CGI stress test

kill $SERVER_PID
```

---

### Code Coverage

**Priority:** 🟢 Enhancement  
**Test Type:** Automated  
**Target:** 80%+ overall

```bash
# Build with coverage flags
make clean
make CXXFLAGS="-coverage -fprofile-arcs -ftest-coverage"

# Run all tests
cd tests/
make
./bin/test_runner

# Generate coverage report
lcov --capture --directory . --output-file coverage.info
lcov --remove coverage.info '/usr/*' --output-file coverage.info  # Remove system headers
genhtml coverage.info --output-directory coverage_report

# View report
open coverage_report/index.html

# Expected coverage:
# - Domain Layer: 90%+
# - Infrastructure Layer: 75%+
# - Presentation Layer: 70%+
# - Overall: 80%+
```

---

### Performance Benchmarks

**Priority:** 🟢 Enhancement  
**Test Type:** Automated  
**Target:** < 50ms average response time

```bash
# Apache Bench (ab)
ab -n 1000 -c 10 http://localhost:8080/

# Expected output:
# Requests per second:    200.00 [#/sec]
# Time per request:       50.000 [ms] (mean)
# Time per request:       5.000 [ms] (mean, across all concurrent requests)

# Detailed timing
ab -n 1000 -c 10 -g results.tsv http://localhost:8080/
gnuplot plot.gnu  # Generate response time graph

# wrk (modern alternative)
wrk -t4 -c100 -d30s http://localhost:8080/

# Expected output:
# Latency     50.00ms   100.00ms   500.00ms
# Req/Sec     200.00    250.00     300.00
```

---

## 📚 Additional Resources

**Related Documentation:**
- [TESTING_STRATEGY.md](TESTING_STRATEGY.md) - Overall strategy and priorities
- [GOOGLETEST_GUIDE.md](GOOGLETEST_GUIDE.md) - Google Test framework usage
- [TESTING_STATUS.md](TESTING_STATUS.md) - Current status
- [GOOGLETEST_REFERENCE.md](GOOGLETEST_REFERENCE.md) - Command-line options

**External Resources:**
- [Google Test Documentation](https://github.com/google/googletest/tree/release-1.8.1/googletest/docs)
- [HTTP/1.1 RFC 2616](https://www.rfc-editor.org/rfc/rfc2616)
- [CGI RFC 3875](https://www.rfc-editor.org/rfc/rfc3875)

---

**Last Updated:** December 26, 2025  
**Version:** 1.0  
**Maintainer:** 42_webserver Team
