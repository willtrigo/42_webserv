# Commit Checklist - NEVER SKIP THIS

## 🚨 Before EVERY `git commit`:

### Step 1: Build Main Project
```bash
cd /home/biralavor/Documents/42_Cursus/42_webserver
make clean && make
```
**✅ MUST compile without errors**  
**❌ If fails → Fix before committing**

---

### Step 2: Build Tests
```bash
cd /home/biralavor/Documents/42_Cursus/42_webserver/tests
make clean && make
```
**✅ MUST compile without errors**  
**❌ If fails → Fix before committing**

---

### Step 3: Run Tests
```bash
./bin/test_runner
```
**✅ Tests should pass (or at least not crash)**  
**⚠️ New failing tests are OK if documented**  
**❌ Crashes are NOT OK**

---

### Step 4: Commit
```bash
git add .
git commit -m "feat: your descriptive message"
git push
```

---

## 🛡️ Sacred Rules

### ❌ NEVER:
- Commit directly to `main`
- Push without testing
- Merge PRs without verifying both branches compile
- Leave broken code overnight
- Skip tests "just this once"

### ✅ ALWAYS:
- Work on feature branches
- Test before commit
- Test before merge
- Communicate if you break something
- Fix broken builds within 15 minutes or revert

---

## 🚨 If You Break the Build:

### Option 1: Quick Fix (< 15 minutes)
```bash
git checkout main
# Fix the issue
make clean && make  # Verify works
cd tests && make && ./bin/test_runner  # Verify works
git commit -m "fix: broken build - [what was wrong]"
git push
```

### Option 2: Revert (> 15 minutes to fix)
```bash
git checkout main
git revert HEAD  # Undo your commit
git push

# Now fix properly on your branch
git checkout your-feature-branch
# Fix thoroughly
# Test extensively
# Merge again when ready
```

---

## 📋 Daily Morning Checklist:

**Every morning BEFORE starting work:**

```bash
cd /home/biralavor/Documents/42_Cursus/42_webserver
git checkout main
git pull
make clean && make
cd tests
make clean && make
./bin/test_runner
```

**✅ If passes → Start your day**  
**❌ If fails → Find who broke it, fix before proceeding**

---

## 🤝 Merge Checklist:

### Before Merging ANY PR:

```bash
# 1. Test feature branch
git checkout feature-branch
make clean && make
cd tests && make && ./bin/test_runner

# 2. Update with main
git merge main
# Resolve conflicts

# 3. Test again
make clean && make
cd tests && make && ./bin/test_runner

# 4. Merge to main
git checkout main
git merge feature-branch

# 5. Final test
make clean && make
cd tests && make && ./bin/test_runner

# 6. Push
git push
```

---

## 💬 Communication Template:

### If You Break Something:

**Post in team chat immediately:**

```
🚨 BROKEN BUILD ALERT 🚨

What broke: [e.g., "ConfigParser missing parsePath()"]
Who: [your name]
Status: Fixing now / Reverting
ETA: [15 min / 30 min / etc.]
Action: Don't pull from main until fixed

Sorry! Will have build health checks in place.
```

---

## ⏰ Time Investment:

**Per commit:** 5 minutes  
**Per day:** 10 minutes (2 commits)  
**Total over 13 days:** 130 minutes

**Broken build cost:**  
- Your time: 30-60 minutes  
- Teammate's time: 30-60 minutes  
- **Total cost: 1-2 hours**

**5 minutes of checking saves 1-2 hours of pain** ✅

---

## 🎯 Success Metrics:

Track on your daily log:

- [ ] Days with 0 broken builds: ____/13
- [ ] Times you caught errors before commit: ____
- [ ] Times you broke main: ____ (goal: 0)
- [ ] Average time to fix broken builds: ____ min (goal: < 15 min)

---

**Remember:** We have 13 days. One broken build = 2 hours lost.  
5 broken builds = 10 hours = 1 full day wasted.

**Test before commit. Always.**
