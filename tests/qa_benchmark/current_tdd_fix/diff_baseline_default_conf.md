# 🔍 Regression Test Report

**Test:** default.conf  
**File:** `baseline_default_conf.txt`  
**Date:** 2026-01-05 07:23:50

---

## 🧪 Test Output

```
Test 2/4: Server default.conf
Testing: default.conf
⚠️  DIFF - Changes detected
```

---

## 📊 Summary

- **Added lines:** 2
- **Removed lines:** 71

---

## 🔄 Changes


### ❌ Baseline (Expected)
```diff
```

### ✅ Current (Actual)
```diff
```

#### 📍 Location: `@@ -116,73 +116,4 @@`

```diff
  
  [36m[DEBUG] [0mEntering server block at line 1
  [36m[DEBUG] [0mParsing server block content
-[36m[DEBUG] [0mAdded listen directive '80' at line 2
-[36m[DEBUG] [0mProcessed directive 'listen' with 1 arguments at line 2
-[36m[DEBUG] [0mAdded server name 'localhost' at line 3
-[36m[DEBUG] [0mProcessed directive 'server_name' with 1 arguments at line 3
-[36m[DEBUG] [0mSet root to './var/www/default' at line 5
-[36m[DEBUG] [0mProcessed directive 'root' with 1 arguments at line 5
-[36m[DEBUG] [0mAdded index file 'index.html' at line 6
-[36m[DEBUG] [0mProcessed directive 'index' with 1 arguments at line 6
-[36m[DEBUG] [0mSet error_page 404 -> '/static/errors/404.html' at line 8
-[36m[DEBUG] [0mProcessed directive 'error_page' with 2 arguments at line 8
-[36m[DEBUG] [0mSet error_page 500 -> '/static/errors/500.html' at line 9
-[36m[DEBUG] [0mProcessed directive 'error_page' with 2 arguments at line 9
-[36m[DEBUG] [0mParsing location block content
-[36m[DEBUG] [0mParsing location block for path: /
-[36m[DEBUG] [0mAdded allowed method 'GET' via limit_except at line 12
-[36m[DEBUG] [0mCompleted limit_except block with 1 allowed methods
-[36m[DEBUG] [0mSet autoindex to 'off' at line 14
-[36m[DEBUG] [0mProcessed directive 'autoindex' with 1 arguments at line 14
-[36m[DEBUG] [0mSet try_files with 3 arguments at line 16
-[36m[DEBUG] [0mProcessed directive 'try_files' with 3 arguments at line 16
-[36m[DEBUG] [0mEnd of location block, added location for path: /
-[36m[DEBUG] [0mParsing location block content
-[36m[DEBUG] [0mParsing location block for path: /files
-[36m[DEBUG] [0mAdded allowed method 'POST' via limit_except at line 20
-[36m[DEBUG] [0mCompleted limit_except block with 1 allowed methods
-[36m[DEBUG] [0mSet upload_store to './var/uploads/default' at line 22
-[36m[DEBUG] [0mProcessed directive 'upload_store' with 1 arguments at line 22
-[36m[DEBUG] [0mSet upload_store_permissions to 660 (432 decimal) at line 23
-[36m[DEBUG] [0mProcessed directive 'upload_store_permissions' with 1 arguments at line 23
-[36m[DEBUG] [0mSet upload_store_access component 'user:rw' at line 24
-[36m[DEBUG] [0mSet upload_store_access component 'group:r' at line 24
-[36m[DEBUG] [0mSet upload_store_access component 'all:r' at line 24
-[36m[DEBUG] [0mProcessed directive 'upload_store_access' with 3 arguments at line 24
-[36m[DEBUG] [0mSet autoindex to 'on' at line 26
-[36m[DEBUG] [0mProcessed directive 'autoindex' with 1 arguments at line 26
-[36m[DEBUG] [0mAdded custom header 'Upload-Success: always' at line 28
-[36m[DEBUG] [0mProcessed directive 'add_header' with 2 arguments at line 28
-[36m[DEBUG] [0mSet return content 201 -> 'Upload complete' at line 29
-[36m[DEBUG] [0mProcessed directive 'return' with 2 arguments at line 29
-[36m[DEBUG] [0mEnd of location block, added location for path: /files
-[36m[DEBUG] [0mParsing location block content
-[36m[DEBUG] [0mParsing location block for path: /public
-[36m[DEBUG] [0mSet root to './var/www/public' at line 33
-[36m[DEBUG] [0mProcessed directive 'root' with 1 arguments at line 33
-[36m[DEBUG] [0mAdded allowed method 'GET' via limit_except at line 35
-[36m[DEBUG] [0mAdded allowed method 'POST' via limit_except at line 35
-[36m[DEBUG] [0mAdded allowed method 'DELETE' via limit_except at line 35
-[36m[DEBUG] [0mCompleted limit_except block with 3 allowed methods
-[36m[DEBUG] [0mSet autoindex to 'on' at line 37
-[36m[DEBUG] [0mProcessed directive 'autoindex' with 1 arguments at line 37
-[36m[DEBUG] [0mSet try_files with 3 arguments at line 39
-[36m[DEBUG] [0mProcessed directive 'try_files' with 3 arguments at line 39
-[36m[DEBUG] [0mEnd of location block, added location for path: /public
-[36m[DEBUG] [0mParsing location block content
-[36m[DEBUG] [0mParsing location block for path: /docs
-[36m[DEBUG] [0mSet return 302 -> 'http://localhost/help' at line 43
-[36m[DEBUG] [0mProcessed directive 'return' with 2 arguments at line 43
-[36m[DEBUG] [0mEnd of location block, added location for path: /docs
-[36m[DEBUG] [0mEnd of server block, added server with 1 listen directives
-[32m[INFO] [0mSuccessfully parsed configuration from ./conf/default.conf with 1 servers
-[32m[INFO] [0mConfiguration loaded successfully from ./conf/default.conf with 1 servers
-[32m[INFO] [0mConfiguration loaded successfully:
-  Worker processes: 1
-  Worker connections: 1024
-  Servers: 1
-
-  Server 1:
-    Server names: localhost
-    Locations: 4
-
+[31m[ERROR] [0munexpected error during configuration load: invalid directive: Invalid listen directive format: Invalid listen directive format: 80 - Invalid listen directive format: Invalid listen directive (missing colon separator): 80 at line 2
```

---

## 📝 Legend

- `-` **Red lines** = Removed from baseline (what was expected)
- `+` **Green lines** = Added in current output (what we got now)
- Unchanged lines = Context

---

## 📂 Files

- **Baseline:** `/home/biralavor/Documents/42_Cursus/42_webserver/tests/qa_benchmark/baseline/baseline_default_conf.txt`
- **Current:** `/home/biralavor/Documents/42_Cursus/42_webserver/tests/qa_benchmark/current_tdd_fix/current_default_conf.txt`
- **This report:** `/home/biralavor/Documents/42_Cursus/42_webserver/tests/qa_benchmark/current_tdd_fix/diff_baseline_default_conf.md`

