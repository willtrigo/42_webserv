# HTTP Parsing Research

**Date:** December 27, 2025  
**Goal:** Research HTTP/1.1 request parsing for webserver implementation  
**Time Allocated:** 1 hour

---

## RFC 2616 Summary (Sections 4-5)

### Section 4: HTTP Message Format

**Basic Structure:**
```
generic-message = start-line
                  *(message-header CRLF)
                  CRLF
                  [ message-body ]
```

**Request Structure:**
```
Request = Request-Line              ; Section 5.1
          *(( general-header        ; Section 4.5
           | request-header         ; Section 5.3
           | entity-header ) CRLF)  ; Section 7.1
          CRLF
          [ message-body ]          ; Section 4.3
```

**Key Points:**
- CRLF = `\r\n` (carriage return + line feed)
- Headers end with double CRLF: `\r\n\r\n`
- Message body is optional (depends on method and headers)
- Line length: No limit in spec, but practical implementations use 8KB

### Section 5: Request

**5.1 Request-Line:**
```
Request-Line = Method SP Request-URI SP HTTP-Version CRLF
```

Example:
```
GET /index.html HTTP/1.1\r\n
```

Components:
- **Method:** GET, POST, PUT, DELETE, HEAD, OPTIONS, TRACE, CONNECT
- **SP:** Single space character
- **Request-URI:** Absolute or relative path
- **HTTP-Version:** "HTTP/1.1" or "HTTP/1.0"

**5.2 Request-URI:**
```
Request-URI = "*" | absoluteURI | abs_path | authority
```

Most common form: `abs_path [ "?" query ]`

Example: `/path/to/resource?key1=value1&key2=value2`

**5.3 Request Header Fields:**

Common headers:
- `Host:` - **Required in HTTP/1.1**
- `Content-Length:` - Body size in bytes
- `Content-Type:` - MIME type of body
- `Transfer-Encoding:` - chunked, gzip, etc.
- `Connection:` - keep-alive, close
- `User-Agent:` - Client identification

---

## Example HTTP Requests

### Simple GET Request
```http
GET /path HTTP/1.1\r\n
Host: localhost\r\n
Content-Length: 0\r\n
\r\n
```

### GET with Query Parameters
```http
GET /search?q=test&limit=10 HTTP/1.1\r\n
Host: example.com\r\n
User-Agent: Mozilla/5.0\r\n
\r\n
```

### POST with Body
```http
POST /api/users HTTP/1.1\r\n
Host: localhost:8080\r\n
Content-Type: application/json\r\n
Content-Length: 27\r\n
\r\n
{"name":"John","age":30}
```

### PUT Request
```http
PUT /resource/123 HTTP/1.1\r\n
Host: api.example.com\r\n
Content-Type: text/plain\r\n
Content-Length: 13\r\n
\r\n
Updated text
```

---

## Parsing Strategy

### State Machine Approach

The parser should use a **finite state machine** with these states:

```
1. PARSING_REQUEST_LINE
   ├─> Parse method (GET, POST, etc.)
   ├─> Parse URI (path + query)
   └─> Parse HTTP version

2. PARSING_HEADERS
   ├─> Parse header name
   ├─> Parse header value
   └─> Detect double CRLF (end of headers)

3. PARSING_BODY (if Content-Length > 0 or Transfer-Encoding: chunked)
   ├─> Read Content-Length bytes
   └─> Or parse chunked encoding

4. COMPLETE
   └─> Request fully parsed
```

### Parsing Phases

**Phase 1: Request Line**
1. Read until first `\r\n`
2. Split by spaces (SP)
3. Validate: exactly 3 parts (Method, URI, Version)
4. Extract and validate each component

**Phase 2: Headers**
1. Read line by line until `\r\n\r\n`
2. Split each line by `:` (colon)
3. Trim whitespace from header name and value
4. Store in key-value map (case-insensitive keys)
5. Special handling for:
   - `Content-Length`: Parse as integer
   - `Transfer-Encoding`: Check for "chunked"
   - `Host`: Required in HTTP/1.1

**Phase 3: Body (if present)**
1. Check `Content-Length` header
   - If present: Read exact number of bytes
   - If absent and method is POST/PUT: Error (411 Length Required)
2. Check `Transfer-Encoding: chunked`
   - Parse chunk size in hex
   - Read chunk data
   - Repeat until zero-sized chunk

### Error Handling

Parser should detect and report:
- **400 Bad Request:** Malformed request line, invalid syntax
- **411 Length Required:** POST/PUT without Content-Length
- **413 Payload Too Large:** Body exceeds server limits
- **414 URI Too Long:** Request-URI exceeds limits
- **431 Request Header Fields Too Large:** Headers exceed limits
- **505 HTTP Version Not Supported:** Not HTTP/1.0 or HTTP/1.1

---

## Implementation Plan (TDD)

### Test 1: ParseSimpleGET
```cpp
TEST(RequestParserTest, ParseSimpleGET)
{
    std::string raw = "GET /path HTTP/1.1\r\nHost: localhost\r\n\r\n";
    Request request = RequestParser::parse(raw);
    
    EXPECT_EQ(request.getMethod(), "GET");
    EXPECT_EQ(request.getUri(), "/path");
    EXPECT_EQ(request.getVersion(), "HTTP/1.1");
    EXPECT_EQ(request.getHeader("Host"), "localhost");
}
```

### Test 2: ParseWithHeaders
```cpp
TEST(RequestParserTest, ParseWithHeaders)
{
    std::string raw = 
        "GET /index.html HTTP/1.1\r\n"
        "Host: example.com\r\n"
        "User-Agent: TestClient/1.0\r\n"
        "Accept: text/html\r\n"
        "\r\n";
    
    Request request = RequestParser::parse(raw);
    
    EXPECT_EQ(request.getMethod(), "GET");
    EXPECT_EQ(request.getUri(), "/index.html");
    EXPECT_EQ(request.getHeader("Host"), "example.com");
    EXPECT_EQ(request.getHeader("User-Agent"), "TestClient/1.0");
    EXPECT_EQ(request.getHeader("Accept"), "text/html");
}
```

### Test 3: ParsePOSTWithBody
```cpp
TEST(RequestParserTest, ParsePOSTWithBody)
{
    std::string raw = 
        "POST /api/data HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "Content-Type: application/json\r\n"
        "Content-Length: 18\r\n"
        "\r\n"
        "{\"key\":\"value\"}";
    
    Request request = RequestParser::parse(raw);
    
    EXPECT_EQ(request.getMethod(), "POST");
    EXPECT_EQ(request.getUri(), "/api/data");
    EXPECT_EQ(request.getHeader("Content-Length"), "18");
    EXPECT_EQ(request.getBody(), "{\"key\":\"value\"}");
}
```

### Additional Test Cases (Future)
- Parse HEAD request (no body allowed)
- Parse PUT request with body
- Parse DELETE request
- Parse request with query parameters
- Handle invalid method (400 error)
- Handle missing Host header (400 error)
- Handle malformed request line (400 error)
- Handle Content-Length mismatch (400 error)
- Handle chunked transfer encoding
- Handle multiple headers with same name
- Handle header continuation (deprecated in HTTP/1.1)

---

## Architecture Integration

### Domain Layer
- **Entity:** `Request` (represents parsed HTTP request)
- **Value Objects:** 
  - `HttpMethod` (already implemented)
  - `Uri` (path + query parsing)
  - `HttpVersion` (validation)
  - `HttpHeaders` (case-insensitive map)

### Infrastructure Layer
- **Parser:** `RequestParser` (stateful parsing logic)
- **Buffer:** `RequestBuffer` (manages raw socket data)
- **State Machine:** `ParserState` (enum for parser states)

### Application Layer
- **Use Case:** `ParseHttpRequest` (orchestrates parsing)
- **Port:** `IRequestParser` (interface for testing)

---

## Next Steps (Day 3-4)

1. **Create test skeleton** (`tests/unit/test_RequestParser.cpp`)
2. **Write 3 failing tests:** ParseSimpleGET, ParseWithHeaders, ParsePOSTWithBody
3. **Implement Request entity** (domain layer)
4. **Implement RequestParser** (infrastructure layer)
5. **Make tests pass** (TDD red-green-refactor)
6. **Add edge cases and error handling tests**
7. **Refactor for Clean Architecture**

---

## References

- RFC 2616: HTTP/1.1 - https://www.ietf.org/rfc/rfc2616.txt
- RFC 7230: HTTP/1.1 Message Syntax (updated) - https://tools.ietf.org/html/rfc7230
- MDN HTTP Messages: https://developer.mozilla.org/en-US/docs/Web/HTTP/Messages

---

**Research Complete:** ✅  
**Time Spent:** 1h (estimated)  
**Ready for Implementation:** Days 3-4
