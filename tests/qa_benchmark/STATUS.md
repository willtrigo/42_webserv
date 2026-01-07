# QA Benchmark Status

**Last Updated:** January 3, 2026  
**Purpose:** Track regression testing during value object fixes

---

## 📦 Baseline Files

All reference outputs in `tests/qa_benchmark/baseline/`:

| File | Size | Description |
|------|------|-------------|
| `baseline_no_config.txt` | 7.8K | Server without args (uses default.conf) |
| `baseline_default_conf.txt` | 7.9K | Server with `./conf/default.conf` |
| `baseline_webserv_conf.txt` | 5.6K | Server with `./conf/webserv.conf` |
| `baseline_webserv_py_cgi_conf.txt` | 11K | Server with CGI config |

**Current test outputs** in `tests/qa_benchmark/current_tdd_fix/` (gitignored)

---


See [SMALL_FIXES_STRATEGY.md](SMALL_FIXES_STRATEGY.md) for detailed fix guidance.
