# 🔍 Regression Test Report

**Test:** webserv_py_cgi.conf  
**File:** `baseline_webserv_py_cgi_conf.txt`  
**Date:** 2026-01-05 07:23:50

---

## 🧪 Test Output

```
Test 4/4: Server webserv_py_cgi.conf
Testing: webserv_py_cgi.conf
⚠️  DIFF - Changes detected
```

---

## 📊 Summary

- **Added lines:** 2
- **Removed lines:** 6

---

## 🔄 Changes


### ❌ Baseline (Expected)
```diff
```

### ✅ Current (Actual)
```diff
```

#### 📍 Location: `@@ -306,8 +306,4 @@`

```diff
  [32m[INFO] [0mSuccessfully processed include: 'sites-available/*.conf'
  [36m[DEBUG] [0mProcessed directive 'include' with 1 arguments at line 10
  [36m[DEBUG] [0mParsing server block content
-[36m[DEBUG] [0mAdded listen directive '80' at line 13
-[36m[DEBUG] [0mProcessed directive 'listen' with 1 arguments at line 13
-[36m[DEBUG] [0mAdded listen directive '[::]:80' at line 14
-[36m[DEBUG] [0mProcessed directive 'listen' with 1 arguments at line 14
-[31m[ERROR] [0munexpected error during configuration load: invalid directive: Invalid listen directive format: Invalid listen directive format: 80 - Duplicate listen directive: 80 at line 15
+[31m[ERROR] [0munexpected error during configuration load: invalid directive: Invalid listen directive format: Invalid listen directive format: 80 - Invalid listen directive format: Invalid listen directive (missing colon separator): 80 at line 13
```

---

## 📝 Legend

- `-` **Red lines** = Removed from baseline (what was expected)
- `+` **Green lines** = Added in current output (what we got now)
- Unchanged lines = Context

---

## 📂 Files

- **Baseline:** `/home/biralavor/Documents/42_Cursus/42_webserver/tests/qa_benchmark/baseline/baseline_webserv_py_cgi_conf.txt`
- **Current:** `/home/biralavor/Documents/42_Cursus/42_webserver/tests/qa_benchmark/current_tdd_fix/current_webserv_py_cgi_conf.txt`
- **This report:** `/home/biralavor/Documents/42_Cursus/42_webserver/tests/qa_benchmark/current_tdd_fix/diff_baseline_webserv_py_cgi_conf.md`

