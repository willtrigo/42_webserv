# CI/CD Setup Guide

This document explains the Continuous Integration/Continuous Deployment (CI/CD) setup for the 42_webserver project.

## Overview

The project uses **GitHub Actions** to automatically:
- ✅ Build the project on every push
- ✅ Run tests on every push and pull request
- ✅ Verify Google Test 1.8.1 installation
- ✅ Upload test results as artifacts
- ✅ Display status badges in README

## Workflows

### 1. Tests Workflow (`.github/workflows/tests.yml`)

**Triggers:**
- Push to: `main`, `master`, `develop`, or any `feature` branch
- Pull requests to: `main`, `master`, `develop`

**Steps:**
1. **Checkout code** - Gets repository code with submodules
2. **Install dependencies** - Installs build-essential, cmake, wget
3. **Install Google Test 1.8.1** - Uses `tests/install_gtest.sh` script
4. **Verify installation** - Checks libraries and headers exist
5. **Build tests** - Compiles test suite
6. **Run tests** - Executes tests with XML output
7. **Upload results** - Saves test results as artifacts
8. **Test summary** - Adds summary to GitHub Actions UI

**View results:**
```
GitHub Repository → Actions tab → Tests workflow
```

**Download test results:**
```
Workflow run → Artifacts → test-results
```

### 2. Build Workflow (`.github/workflows/build.yml`)

**Triggers:**
- Push to: `main`, `master`, `develop`, or any `feature` branch
- Pull requests to: `main`, `master`, `develop`

**Steps:**
1. **Checkout code** - Gets repository code with submodules
2. **Install dependencies** - Installs build-essential, cmake
3. **Build project** - Compiles main project
4. **Check executable** - Verifies `bin/webserv` exists

**View results:**
```
GitHub Repository → Actions tab → Build workflow
```

## Status Badges

Badges show the current state of tests and builds:

**In README.md:**
```markdown
[![Tests](https://github.com/biralavor/42_webserver/actions/workflows/tests.yml/badge.svg)](https://github.com/biralavor/42_webserver/actions/workflows/tests.yml)
[![Build](https://github.com/biralavor/42_webserver/actions/workflows/build.yml/badge.svg)](https://github.com/biralavor/42_webserver/actions/workflows/build.yml)
```

**Badge states:**
- 🟢 **Passing** - All tests/builds successful
- 🔴 **Failing** - One or more tests/builds failed
- 🟡 **Running** - Workflow currently executing
- ⚪ **No runs** - Workflow hasn't run yet

## Enabling GitHub Actions

### First Time Setup

1. **Push workflows to GitHub:**
   ```bash
   git add .github/workflows/
   git commit -m "Add GitHub Actions for tests and build"
   git push
   ```

2. **Enable Actions** (if not already enabled):
   - Go to repository Settings
   - Click "Actions" in left sidebar
   - Select "Allow all actions and reusable workflows"
   - Click "Save"

3. **Verify workflows:**
   - Go to "Actions" tab
   - You should see "Tests" and "Build" workflows
   - They will run automatically on next push

### Permissions

GitHub Actions needs these permissions (enabled by default):
- ✅ Read repository contents
- ✅ Write check runs (for status checks)
- ✅ Upload artifacts

## Workflow Files Explained

### tests.yml Structure

```yaml
name: Tests                          # Workflow name

on:                                  # When to run
  push:
    branches: [ main, develop, ... ] # On push to these branches
  pull_request:
    branches: [ main, develop ]      # On PRs to these branches

jobs:                                # What to do
  test:                              # Job name
    runs-on: ubuntu-latest           # Ubuntu runner
    
    steps:
      - name: Checkout code          # Step 1: Get code
        uses: actions/checkout@v3
        with:
          submodules: recursive      # Include submodules
      
      - name: Install dependencies   # Step 2: System packages
        run: |
          sudo apt-get update
          sudo apt-get install -y build-essential cmake wget
      
      - name: Install Google Test    # Step 3: Test framework
        run: |
          cd tests/
          sudo ./install_gtest.sh    # Uses our script
      
      - name: Build tests            # Step 4: Compile tests
        run: |
          cd tests/
          make clean
          make
      
      - name: Run tests              # Step 5: Execute tests
        run: |
          cd tests/
          ./bin/test_runner --gtest_output=xml:test-results.xml
      
      - name: Upload test results    # Step 6: Save results
        if: always()                 # Run even if tests fail
        uses: actions/upload-artifact@v3
        with:
          name: test-results
          path: tests/test-results.xml
```

## Test Results

### Viewing Test Results

**In GitHub UI:**
1. Go to "Actions" tab
2. Click on workflow run
3. See summary of passed/failed tests
4. Download artifacts for detailed XML results

**XML Output Format:**
```xml
<?xml version="1.0" encoding="UTF-8"?>
<testsuites tests="5" failures="0" errors="0" time="0.001">
  <testsuite name="HttpMethodTest" tests="5" failures="0" errors="0">
    <testcase name="ValidMethods" status="run" time="0.000"/>
    <testcase name="InvalidMethod" status="run" time="0.000"/>
    ...
  </testsuite>
</testsuites>
```

### Test Artifacts

After each test run, XML results are saved as artifacts:

**Download:**
```
Workflow run → Artifacts section → test-results.zip
```

**Contains:**
- `test-results.xml` - JUnit-style test report
- Can be parsed by CI tools
- Shows test names, times, pass/fail status

## Troubleshooting

### Workflow Not Running

**Check:**
1. Is workflow file in `.github/workflows/`?
2. Is file named with `.yml` or `.yaml` extension?
3. Are Actions enabled in repository settings?
4. Does branch match trigger conditions?

**Fix:**
```bash
# Verify file exists
ls -la .github/workflows/

# Check YAML syntax
cat .github/workflows/tests.yml

# Push again to trigger
git push
```

### Build Failing

**Check workflow logs:**
1. Go to Actions tab
2. Click failing workflow
3. Click failed job
4. Expand failed step
5. Read error message

**Common issues:**

**Google Test installation fails:**
```
Error: Could not download Google Test 1.8.1
```
Solution: Check network, GitHub rate limits, or use cached version

**Tests fail to compile:**
```
Error: undefined reference to...
```
Solution: Check [FEATURES_STATUS.md](FEATURES_STATUS.md) - feature might not be implemented yet

**Tests fail to run:**
```
Error: RegexPatternException
```
Solution: Feature has unimplemented dependencies (see [FEATURES_STATUS.md](FEATURES_STATUS.md))

### Slow Workflow

**Current times:**
- Google Test installation: ~2-3 minutes
- Build tests: ~10-30 seconds
- Run tests: < 1 second
- **Total: ~3-4 minutes**

**Optimization options:**

**1. Cache Google Test:**
```yaml
- name: Cache Google Test
  uses: actions/cache@v3
  with:
    path: /usr/local/lib/libgtest*
    key: gtest-1.8.1-${{ runner.os }}

- name: Install Google Test (if not cached)
  if: steps.cache.outputs.cache-hit != 'true'
  run: |
    cd tests/
    sudo ./install_gtest.sh
```

**2. Cache build artifacts:**
```yaml
- name: Cache build
  uses: actions/cache@v3
  with:
    path: tests/build
    key: test-build-${{ hashFiles('tests/**/*.cpp') }}
```

## Advanced Configuration

### Running on Multiple OS

```yaml
jobs:
  test:
    strategy:
      matrix:
        os: [ubuntu-latest, macos-latest]
    runs-on: ${{ matrix.os }}
    # ... rest of steps
```

### Different C++ Compilers

```yaml
jobs:
  test:
    strategy:
      matrix:
        compiler: [g++, clang++]
    steps:
      - name: Set compiler
        run: echo "CXX=${{ matrix.compiler }}" >> $GITHUB_ENV
      # ... rest of steps
```

### Scheduled Runs

```yaml
on:
  push:
    branches: [ main ]
  schedule:
    - cron: '0 0 * * *'  # Daily at midnight
```

### Manual Trigger

```yaml
on:
  push:
    branches: [ main ]
  workflow_dispatch:  # Adds "Run workflow" button
```

## Pull Request Integration

### Status Checks

GitHub Actions automatically adds status checks to PRs:

**In PR:**
- ✅ **Tests passing** - Merge allowed
- ❌ **Tests failing** - Merge blocked (if required)
- 🟡 **Tests running** - Waiting for completion

### Require Checks

**Make tests required before merge:**
1. Go to Settings → Branches
2. Add branch protection rule for `main`
3. Enable "Require status checks to pass"
4. Select "Tests" and "Build" workflows
5. Save changes

Now PRs can't be merged until tests pass!

## Local Testing Before Push

**Always test locally first:**

```bash
# Run tests locally
cd tests/
make clean && make
./bin/test_runner

# Only push if tests pass
git add .
git commit -m "Your changes"
git push
```

## Cost & Limits

**GitHub Actions is free for:**
- ✅ Public repositories (unlimited)
- ✅ Private repositories (2,000 minutes/month)

**Current usage:**
- ~4 minutes per workflow run
- ~2 workflows per push
- ~8 minutes per push
- **Can run ~250 pushes/month on free tier**

**Optimize to save minutes:**
- Use caching
- Run tests only on relevant changes
- Skip workflows with `[skip ci]` in commit message

## Monitoring

### Workflow Status

**Check overall health:**
```
Repository → Insights → Actions
```

Shows:
- Success/failure rates
- Average run times
- Most active workflows

### Email Notifications

GitHub emails you when:
- ❌ Your push fails workflows
- ✅ Failing workflows are fixed
- 🔔 Someone mentions you in workflow run

**Configure:**
Settings → Notifications → Actions

## Resources

- **GitHub Actions Docs:** https://docs.github.com/en/actions
- **Workflow Syntax:** https://docs.github.com/en/actions/reference/workflow-syntax-for-github-actions
- **Google Test CI:** https://github.com/google/googletest/blob/main/docs/pkgconfig.md

## Summary

✅ **GitHub Actions configured** for automated testing  
✅ **Two workflows** - Tests and Build  
✅ **Runs automatically** on every push and PR  
✅ **Status badges** show current state  
✅ **Test results** saved as artifacts  
✅ **Ready to use** - just push to GitHub!

**Next steps:**
1. Push workflows to GitHub
2. Check Actions tab
3. Watch tests run automatically
4. Add status badges to README (already done)

**Last Updated:** December 26, 2025
