# 📚 Testing Documentation Index

## 🚨 START HERE

### Before Your First Commit
**[COMMIT_CHECKLIST.md](COMMIT_CHECKLIST.md)** - Read this FIRST  
**5 minutes that saves hours.** Step-by-step protocol to never break the build.

---

## 📋 Strategy & Planning

**[TESTING_STRATEGY.md](TESTING_STRATEGY.md)** - Overall testing approach  
**[TWO_PERSON_SPRINT.md](TWO_PERSON_SPRINT.md)** - Day-by-day tasks for 2-person team  
**[TESTING_STATUS.md](TESTING_STATUS.md)** - What's implemented, what's not

---

## 🎓 Learning Resources

**[BEGINNER_GUIDE.md](BEGINNER_GUIDE.md)** - Start here if new to testing  
**[GOOGLETEST_GUIDE.md](GOOGLETEST_GUIDE.md)** - How to use Google Test  
**[GOOGLETEST_REFERENCE.md](GOOGLETEST_REFERENCE.md)** - Quick reference for assertions  
**[MOCK_USAGE.md](MOCK_USAGE.md)** - Mock objects guide with examples  
**[TESTING_SCENARIOS.md](TESTING_SCENARIOS.md)** - Common testing patterns

---

## 🛠️ Setup & Troubleshooting

**[GOOGLETEST_INSTALLATION.md](GOOGLETEST_INSTALLATION.md)** - How to install Google Test 1.8.1  
**[GOOGLETEST_QUICKFIX.md](GOOGLETEST_QUICKFIX.md)** - Fix common Google Test issues  
**[CI_CD.md](CI_CD.md)** - GitHub Actions configuration

---

## 🛡️ Sacred Rules

From [COMMIT_CHECKLIST.md](COMMIT_CHECKLIST.md):

### ❌ NEVER
- Commit without testing first
- Push directly to main branch
- Merge PRs without verification

### ✅ ALWAYS
- Test locally before commit: `make && cd tests && make && ./bin/test_runner`
- Use feature branches
- Test again after merging

**Time investment:** 5 minutes per commit  
**Time saved:** 1-2 hours debugging broken builds

---

## Quick Links

- **Need to fix the build?** → [COMMIT_CHECKLIST.md](COMMIT_CHECKLIST.md#if-you-break-the-build)
- **Writing your first test?** → [BEGINNER_GUIDE.md](BEGINNER_GUIDE.md)
- **Google Test not compiling?** → [GOOGLETEST_QUICKFIX.md](GOOGLETEST_QUICKFIX.md)
- **What should I test next?** → [TWO_PERSON_SPRINT.md](TWO_PERSON_SPRINT.md)
- **CI/CD failing?** → [CI_CD.md](CI_CD.md)

---

## Document Relationships

```
COMMIT_CHECKLIST.md ←─────── Read this FIRST (daily reference)
         │
         ├──→ TWO_PERSON_SPRINT.md (uses checklist in daily tasks)
         └──→ TESTING_STRATEGY.md (implements checklist principles)

BEGINNER_GUIDE.md → GOOGLETEST_GUIDE.md → GOOGLETEST_REFERENCE.md
                                                    │
                                                    └──→ TESTING_SCENARIOS.md

GOOGLETEST_INSTALLATION.md ←──→ GOOGLETEST_QUICKFIX.md (troubleshooting)

CI_CD.md (automated enforcement of COMMIT_CHECKLIST rules)
```

---

## Success Metrics

Track in your team channel:

- **Days without broken builds** 🎯 Goal: 7+ consecutive days
- **Times caught errors before commit** ✅ Each time = 1-2 hours saved
- **Build fix time when broken** ⏱️ Goal: <15 minutes

---

## Updates

- **2024-12-27**: Created COMMIT_CHECKLIST.md after build breakage incident
- **2024-12-27**: Enhanced TWO_PERSON_SPRINT.md with Sacred Rules
- **2025-12-26**: Initial testing strategy documents created
