# 🚨 EVALUATION BLOCKERS - UPDATED (Jan 8, 2026)

**CRITICAL FINDING:** ubuntu_tester CRASHED - HEAD method not implemented!

**Status:** 8/15 features working, 5 CRITICAL bugs

---

## 🔴 SHOWSTOPPER (Fix First - Ubuntu Tester Failed)

### 0. ❌ HEAD METHOD CAUSES SEGMENTATION FAULT
**Test:** `curl -I http://127.0.0.1:8080/` (twice)  
**Expected:** `200 OK` with headers, NO body (both times)  
**Actual:** **FATAL ERROR - Server crashes on 2nd HEAD request**  

**Ubuntu Tester Output:**
```
Test HEAD http://127.0.0.1:8080/
FATAL ERROR ON LAST TEST: bad status code
```

**GoogleTest Integration Results:**
```bash
[ RUN      ] HttpServerIntegrationTest.HeadRequestReturns200
[       OK ] HttpServerIntegrationTest.HeadRequestReturns200 (89437 ms)  # First HEAD: OK

[ RUN      ] HttpServerIntegrationTest.HeadRequestHasNoBody  
[1]  + 993462 segmentation fault (core dumped)  # Second HEAD: CRASH
[  FAILED  ] HttpServerIntegrationTest.HeadRequestHasNoBody (91590 ms)
```

**Root Cause:** HEAD method implementation has memory corruption bug  
**Likely Issues:**
- Buffer overflow/underflow in HEAD response handler
- Double-free of response buffer
- Null pointer dereference on 2nd HEAD
- Keep-alive connection not properly cleaned up

**Fix Location:** Response builder + HEAD handler  
**Fix Priority:** 🔥🔥🔥 **HIGHEST** - Server completely unusable  
**Time to Fix:** 2-4 hours (requires GDB debugging)

**How to Debug:**
```bash
# Terminal 1: Server with GDB
cd /home/biralavor/Documents/42_Cursus/42_webserver
gdb --args ./bin/webserv conf/default.conf
(gdb) run

# Terminal 2: Trigger crash
curl -I http://127.0.0.1:8080/  # OK
curl -I http://127.0.0.1:8080/  # CRASH - check Terminal 1 backtrace
```

**Why Critical:**
- Evaluator tests HEAD first (42 evaluation requirement)
- Server crashes = instant failure
- Cannot test ANY other features after crash
- 22/24 GoogleTest integration tests failed due to server crash

---

## CRITICAL (Will Fail Evaluation)

### 1. ❌ Client Body Size Limit (413 Response)
**Test:** `curl -X POST -F "file=@/tmp/large_file.bin" http://127.0.0.1:8080/files`  
**Expected:** `413 Payload Too Large`  
**Actual:** `500 Internal Server Error`  

**Root Cause:** Body size check missing or throwing exception  
**Fix Location:** `src/application/handlers/PostHandler.cpp` or request parser  
**Fix Priority:** 🔥 HIGHEST - Mandatory requirement  
**Time to Fix:** 30-60 minutes

**How to Fix:**
```cpp
// In RequestParser or PostHandler
if (contentLength > config.getMaxBodySize()) {
    return Response::error(413, "Payload Too Large");
}
```

---

### 2. ❌ DELETE Method (500 Error)
**Test:** `curl -X DELETE http://127.0.0.1:8080/public/test.txt`  
**Expected:** `200 OK` or `204 No Content`  
**Actual:** `500 Internal Server Error`  

**Root Cause:** File path resolution or permission check crashing  
**Fix Location:** `src/application/handlers/DeleteHandler.cpp`  
**Fix Priority:** 🔥 CRITICAL - Mandatory requirement  
**Time to Fix:** 30-60 minutes

**Check Server Logs For:**
- Exception stack trace
- File path being accessed
- Permission error details

---

### 3. ❌ Method Restrictions Not Enforced
**Test:** `curl -X GET http://127.0.0.1:8080/files`  
**Config:** `limit_except POST { deny all; }` (only POST allowed)  
**Expected:** `405 Method Not Allowed`  
**Actual:** `201 Created "Upload complete"`  

**Root Cause:** Router not checking allowed methods before executing handler  
**Fix Location:** `src/application/Router.cpp` or `LocationConfig`  
**Fix Priority:** 🔥 HIGH - Security issue  
**Time to Fix:** 30 minutes

**How to Fix:**
```cpp
// In Router before calling handler
if (!location.isMethodAllowed(request.getMethod())) {
    return Response::error(405, "Method Not Allowed");
}
```

---

### 4. ❌ POST Upload Not Saving Files
**Test:** `curl -X POST -F "file=@README.md" http://127.0.0.1:8080/files`  
**Expected:** File saved to `./var/uploads/default/README.md`  
**Actual:** Returns 201 but file not created  

**Root Cause:** 
- Upload directory doesn't exist
- PostHandler not writing file (just returning hardcoded 201)

**Fix Priority:** 🔥 CRITICAL  
**Time to Fix:** 1-2 hours

**How to Fix:**
1. Create missing directories:
```bash
mkdir -p ./var/uploads/default
mkdir -p ./var/www/public
chmod 755 ./var/uploads/default
```

2. Implement file saving in PostHandler:
```cpp
// PostHandler.cpp
std::string uploadPath = config.getUploadStore() + "/" + filename;
FileWriter::write(uploadPath, fileContent);
return Response::success(201, "Upload complete");
```

---

## MEDIUM Priority (Should Fix)

### 5. ⚠️ Autoindex Not Working
**Test:** `curl http://127.0.0.1:8080/files/`  
**Config:** `autoindex on;`  
**Expected:** HTML directory listing  
**Actual:** `201 "Upload complete"` (wrong handler executed)  

**Root Cause:** GET request to directory not triggering autoindex logic  
**Fix Location:** `src/application/handlers/GetHandler.cpp`  
**Time to Fix:** 1 hour

---

### 6. ⚠️ CGI Not Implemented
**Test:** `curl http://127.0.0.1:8080/test.php`  
**Expected:** PHP script output  
**Actual:** `404 Not Found`  

**Root Cause:** 
- CGI handler not implemented OR
- No CGI files created for testing

**Fix Priority:** ⚠️ MEDIUM (check if bonus in subject)  
**Time to Fix:** 2-4 hours (if required)

---

## Quick Wins (Easy Fixes)

### 7. ✅ Create Missing Directories
```bash
cd /home/biralavor/Documents/42_Cursus/42_webserver
mkdir -p var/www/public
mkdir -p var/uploads/default
mkdir -p var/www/default/static/errors
chmod -R 755 var/
echo "Test file" > var/www/public/test.txt
```
**Time:** 2 minutes

---

## Testing Checklist Before Evaluation

Run these commands and verify EXACT responses:

```bash
# 1. POST small file (must return 201 AND save file)
curl -v -X POST -F "file=@README.md" http://127.0.0.1:8080/files
ls -la ./var/uploads/default/README.md  # ← File MUST exist

# 2. POST large file (must return 413, not 500)
dd if=/dev/zero of=/tmp/large.bin bs=1M count=2
curl -v -X POST -F "file=@/tmp/large.bin" http://127.0.0.1:8080/files
# Expected: HTTP/1.1 413 Payload Too Large

# 3. GET /files (must return 405, not 201)
curl -v -X GET http://127.0.0.1:8080/files
# Expected: HTTP/1.1 405 Method Not Allowed

# 4. DELETE existing file (must return 200/204, not 500)
echo "test" > ./var/www/public/deleteme.txt
curl -v -X DELETE http://127.0.0.1:8080/public/deleteme.txt
# Expected: HTTP/1.1 200 OK or 204 No Content
ls ./var/www/public/deleteme.txt  # ← File MUST be gone

# 5. POST on "/" (must return 405)
curl -v -X POST http://127.0.0.1:8080/
# Expected: HTTP/1.1 405 Method Not Allowed

# 6. GET 404 (must return custom error page)
curl -v http://127.0.0.1:8080/nonexistent
# Expected: HTTP/1.1 404 with HTML error page

# 7. Redirect (must return 302)
curl -v http://127.0.0.1:8080/docs
# Expected: HTTP/1.1 302 Found, Location: http://localhost:8080/help
```

---

## Time Estimate (Updated Jan 8)

| Task | Priority | Time | Status |
|------|----------|------|--------|
| **Implement HEAD method** | 🔥🔥🔥 | 1-2 hours | ⬜ **BLOCKING** |
| Create directories | 🔥 | 2 min | ✅ DONE |
| Fix 413 body size | 🔥 | 30-60 min | ⬜ TODO |
| Fix DELETE 500 | 🔥 | 30-60 min | ⬜ TODO |
| Fix method restrictions | 🔥 | 30 min | ⬜ TODO |
| Fix POST file saving | 🔥 | 1-2 hours | ⬜ TODO |
| Fix autoindex | ⚠️ | 1 hour | ⬜ TODO |
| Implement CGI | ⚠️ | 2-4 hours | ⬜ TODO (if mandatory) |

**Total Critical Fixes:** 4-7 hours  
**Total with Medium:** 7-12 hours  

**You have:** Today (Jan 8) - Evaluation likely soon  
**Recommendation:** 
1. **FIX HEAD METHOD FIRST** (ubuntu_tester will crash without it)
2. Fix 413, DELETE, method restrictions
3. Fix POST file saving
4. Test everything with automated script

---

## Next Steps

1. **Check server terminal** for exception stack traces during tests
2. **Create missing directories** (2 minutes)
3. **Fix 413 body size check** (highest priority)
4. **Fix DELETE handler** (check logs for exception)
5. **Fix method restriction enforcement** (router logic)
6. **Implement POST file saving** (most complex)
7. **Test everything again** (Updated)

- [ ] **HEAD method works** ❌ **SHOWSTOPPER**
- [ ] GET requests work ✅
- [ ] POST small file → 201 + file saved ❌ (returns 201 but doesn't save)
- [ ] POST large file → 413 (not 500) ❌ (returns 500)
- [ ] GET /files → 405 (not 201) ❌ (returns 201 - wrong)
- [ ] DELETE file → 200/204 + file removed ❌ (returns 500)
- [ ] POST on "/" → 405 ✅
- [ ] GET nonexistent → 404 with custom page ✅
- [ ] Redirect → 302 ✅
- [ ] No crashes under load ❓ (need siege test)
- [ ] Valgrind clean (0 leaks) ❓ (need test)
- [ ] CGI working (if mandatory) ❌ (not implemented)

**Current Score:** 4/12 mandatory features (33%)  
**Target Score:** 12/12 features (100%)  
**Gap:** 8 features to fix

**CRITICAL:** Ubuntu tester will FAIL immediately without HEAD method

---

## 🚀 NEW TESTING TOOLS

### Automated Test Script
```bash
cd /home/biralavor/Documents/42_Cursus/42_webserver/tests
./test_42_evaluation.sh
```

This script tests:
- All HTTP methods (GET, POST, DELETE, HEAD)
- Status codes (200, 404, 405, 413, 301/302)
- File upload/download
- Body size limits
- Concurrent requests
- Availability (99.5% requirement)

### Manual Test Checklist
See `tests/42_MANUAL_TESTS.md` for:
- Code review questions (evaluator will ask)
- Browser-based tests
- Port configuration tests
- Siege stress tests
- Memory leak verification
- Bonus features (if time permits)aks) ✅
- [ ] CGI working (if mandatory) ❓

**Current Score:** 8/15 features (53%)  
**Target Score:** 15/15 features (100%)  
**Gap:** 7 features to fix
