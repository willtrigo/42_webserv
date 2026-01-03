# QA Benchmark Baselines

Baseline outputs captured on: **January 2, 2026**

## Purpose
These files serve as regression test baselines. After making fixes, compare new outputs with these baselines to ensure no regression.

## Files

**Baseline Directory** (`baseline/`):
- `baseline_no_config.txt` - Server run without config (uses default.conf fallback)
- `baseline_default_conf.txt` - Server with `./conf/default.conf`
- `baseline_webserv_conf.txt` - Server with `./conf/webserv.conf`
- `baseline_webserv_py_cgi_conf.txt` - Server with `./conf/webserv_py_cgi.conf`

**Current Test Outputs** (`current_tdd_fix/` - gitignored):
- Generated on each test run
- Compared against baselines to detect regressions

## Usage

### Quick Check (after making changes)
```bash
# Run all configs and compare
./tests/qa_benchmark/check_regression.sh
```

### Manual Comparison
```bash
# Rebuild and test
make && make test

# Compare specific config
timeout 2s ./bin/webserv > /tmp/no_config_new.txt 2>&1
diff tests/qa_benchmark/baseline/baseline_no_config.txt /tmp/no_config_new.txt

# Or compare all
timeout 2s ./bin/webserv ./conf/default.conf > /tmp/default_new.txt 2>&1
diff tests/qa_benchmark/baseline/baseline_default_conf.txt /tmp/default_new.txt
```

### Expected Differences
Some differences are OK (timestamps, PIDs, etc.). Look for:
- ❌ New error messages
- ❌ Missing configuration parsing
- ❌ Segfaults or crashes
- ❌ Different server counts
- ✅ Same structure and flow
- ✅ Same DEBUG/INFO patterns

## Update Baselines
After intentional changes that modify output:
```bash
timeout 2s ./bin/webserv > tests/qa_benchmark/baseline/baseline_no_config.txt 2>&1 || true
timeout 2s ./bin/webserv ./conf/default.conf > tests/qa_benchmark/baseline/baseline_default_conf.txt 2>&1 || true
timeout 2s ./bin/webserv ./conf/webserv.conf > tests/qa_benchmark/baseline/baseline_webserv_conf.txt 2>&1 || true
timeout 2s ./bin/webserv ./conf/webserv_py_cgi.conf > tests/qa_benchmark/baseline/baseline_webserv_py_cgi_conf.txt 2>&1 || true
```
