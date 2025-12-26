# Quick Fix Guide - Google Test Installation Error

## 🔴 Problem
You installed the latest Google Test (v1.14+) which requires **C++11**, but your project uses **C++98**.

## ✅ Solution

Follow these steps to fix it:

### Step 1: Uninstall Current Version

```bash
sudo rm -rf /usr/local/include/gtest
sudo rm -rf /usr/local/lib/libgtest*
sudo rm -rf /usr/local/lib/cmake/GTest
```

### Step 2: Install Google Test 1.8.1 (Two Options)

**Option A: Automated Script (Easiest)**
```bash
cd tests/
sudo ./install_gtest.sh
```

**Option B: Manual Installation**
```bash
cd /tmp
wget https://github.com/google/googletest/archive/refs/tags/release-1.8.1.tar.gz
tar -xzf release-1.8.1.tar.gz
cd googletest-release-1.8.1
mkdir build && cd build
cmake ..
make
sudo make install
```

### Step 3: Verify Installation

```bash
# Check libraries
ls -la /usr/local/lib/libgtest*

# Verify it's C++98 compatible (should find nothing)
grep -r "std::tuple" /usr/local/include/gtest/ || echo "✓ Correct version!"
```

### Step 4: Build and Run Tests

```bash
cd tests/
make clean
make
make test
```

## 🎯 Why This Happens

| Google Test Version | C++ Standard | Compatible? |
|---------------------|--------------|-------------|
| 1.8.x | C++98 | ✅ YES |
| 1.10+ | C++11+ | ❌ NO (for this project) |
| Latest (1.14+) | C++14+ | ❌ NO (for this project) |

42 School projects **must use C++98**, so you need the older Google Test version.

## 🚨 Common Mistakes

### ❌ Don't do this:
```bash
brew install googletest          # Installs latest version
apt install libgtest-dev         # Might install wrong version
git clone googletest (main branch)  # Gets latest version
```

### ✅ Do this instead:
```bash
sudo ./install_gtest.sh          # Uses correct version
# OR manually install release-1.8.1
```

## 📚 Need More Help?

- See [README.md](../README.md) for overview
- See [BEGINNER_GUIDE.md](BEGINNER_GUIDE.md) for testing tutorials
- See [INSTALLATION.md](INSTALLATION.md) for complete installation guide
- Check [RUNNING_TESTS.md](RUNNING_TESTS.md) for troubleshooting

## ⚡ Quick Commands Reference

```bash
# Clean everything
make clean

# Build tests
make

# Run all tests
make test

# Run specific test
./bin/test_runner --gtest_filter="HttpMethodTest.*"

# Verbose output
./bin/test_runner --gtest_verbose
```

---

**After following these steps, your tests should compile and run successfully!** 🎉
