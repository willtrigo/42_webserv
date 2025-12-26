# Installation Guide - Google Test 1.8.1

This guide will help you install the correct version of Google Test for this C++98 project.

## ⚠️ Important: Version Requirements

**This project uses C++98**, which means you need **Google Test 1.8.x** (the last version supporting C++98).

| Google Test Version | C++ Standard | Compatible? |
|---------------------|--------------|-------------|
| 1.8.x | C++98 | ✅ YES - Use this! |
| 1.10+ | C++11+ | ❌ NO |
| Latest (1.14+) | C++14+ | ❌ NO |

## Prerequisites

- C++ compiler (g++ or clang++)
- CMake 3.5 or later
- wget or curl (for downloading)
- sudo privileges (for installation)

---

## Quick Installation (Recommended)

Use our automated installation script that handles everything for you:

```bash
cd tests/
sudo ./install_gtest.sh
```

### What the script does:
1. ✅ Removes any existing Google Test installation
2. ✅ Downloads Google Test 1.8.1 from official repository
3. ✅ Patches source code for modern compiler compatibility
4. ✅ Builds with CMake compatibility flags
5. ✅ Removes -Werror flags to allow compilation on modern GCC
6. ✅ Installs to /usr/local/
7. ✅ Verifies installation success

**Expected output:**
```
==> Removing existing Google Test installation...
==> Downloading Google Test 1.8.1...
==> Extracting archive...
==> Patching source for modern GCC...
==> Building Google Test...
==> Removing -Werror flags...
==> Installing Google Test...
==> Verifying installation...
✓ Libraries found: /usr/local/lib/libgtest.a
✓ Headers found: /usr/local/include/gtest/
==> Google Test 1.8.1 installed successfully!
```

---

## Manual Installation

If you prefer to install manually or the script doesn't work:

### Step 1: Remove Existing Installation

First, clean up any previous Google Test installation:

```bash
sudo rm -rf /usr/local/include/gtest
sudo rm -rf /usr/local/include/gmock
sudo rm -rf /usr/local/lib/libgtest*
sudo rm -rf /usr/local/lib/libgmock*
sudo rm -rf /usr/local/lib/cmake/GTest
sudo rm -rf /usr/local/share/gmock
sudo rm -rf /usr/local/share/googletest
```

### Step 2: Download Google Test 1.8.1

```bash
cd /tmp
wget https://github.com/google/googletest/archive/refs/tags/release-1.8.1.tar.gz
tar -xzf release-1.8.1.tar.gz
cd googletest-release-1.8.1
```

### Step 3: Patch Source (for modern GCC)

Modern GCC compilers are stricter. Fix uninitialized variable warning:

```bash
# Fix uninitialized variable in death test
sed -i 's/int dummy;/int dummy = 0;/' googletest/src/gtest-death-test.cc
```

### Step 4: Build with CMake

```bash
mkdir build && cd build

# Use compatibility flag for older CMake version
cmake -DCMAKE_POLICY_VERSION_MINIMUM=3.5 ..

make
```

### Step 5: Remove -Werror Flags

Modern GCC generates warnings in old code. Remove -Werror to allow compilation:

```bash
# Remove -Werror from all build files
find . -name "flags.make" -type f -exec sed -i 's/-Werror[^ ]*//g' {} \;
```

### Step 6: Rebuild and Install

```bash
make clean
make
sudo make install
```

### Step 7: Verify Installation

```bash
# Check libraries
ls -la /usr/local/lib/libgtest*
# Should show: libgtest.a and libgtest_main.a

# Check headers
ls -la /usr/local/include/gtest/
# Should show: gtest.h and other headers

# Verify C++98 compatibility
grep -r "std::tuple" /usr/local/include/gtest/ && echo "❌ Wrong version!" || echo "✅ Correct version!"
```

### Step 8: Clean Up

```bash
cd /tmp
rm -rf googletest-release-1.8.1 release-1.8.1.tar.gz
```

---

## Platform-Specific Instructions

### macOS

If you previously installed via Homebrew:

```bash
# Uninstall Homebrew version (it's too new)
brew uninstall googletest

# Then follow the manual installation above
```

### Ubuntu/Debian

```bash
# DO NOT use apt package (might be wrong version)
# sudo apt install libgtest-dev  # ❌ DON'T DO THIS

# Instead, use manual installation above ✅
```

### Arch Linux

```bash
# DO NOT use pacman package (will be too new)
# sudo pacman -S gtest  # ❌ DON'T DO THIS

# Instead, use manual installation above ✅
```

---

## Verification

After installation, verify everything is correct:

### Check Files

```bash
# Libraries should exist
ls -la /usr/local/lib/libgtest.a
ls -la /usr/local/lib/libgtest_main.a

# Headers should exist
ls -la /usr/local/include/gtest/gtest.h
```

### Check Version Compatibility

```bash
# This should find NOTHING (old version doesn't have C++11 features)
grep -r "std::tuple" /usr/local/include/gtest/

# If grep finds nothing, you're good! ✅
# If grep finds matches, wrong version installed ❌
```

### Test Compilation

```bash
cd /home/biralavor/Documents/42_Cursus/42_webserver/tests
make clean
make

# Should compile without errors
```

---

## Troubleshooting

### Error: "CMake version too old"

```bash
# Ubuntu/Debian
sudo apt update
sudo apt install cmake

# macOS
brew install cmake
```

### Error: "wget: command not found"

```bash
# Ubuntu/Debian
sudo apt install wget

# macOS
brew install wget

# Or use curl instead:
curl -L -O https://github.com/google/googletest/archive/refs/tags/release-1.8.1.tar.gz
```

### Error: "Permission denied"

You need sudo privileges. Make sure to use `sudo` for installation commands:

```bash
sudo make install
```

### Build Still Fails After Installation

```bash
# Make sure old build artifacts are removed
cd tests/
make clean

# Rebuild everything
make

# If it still fails, check your paths
echo $LD_LIBRARY_PATH
```

### Headers Not Found During Compilation

The Makefile should already include the correct paths, but if needed:

```bash
# Add to your Makefile INCLUDES:
INCLUDES = -I/usr/local/include -I../src
```

---

## Uninstalling Google Test

If you need to completely remove Google Test:

```bash
sudo rm -rf /usr/local/include/gtest
sudo rm -rf /usr/local/include/gmock
sudo rm -rf /usr/local/lib/libgtest*
sudo rm -rf /usr/local/lib/libgmock*
sudo rm -rf /usr/local/lib/cmake/GTest
sudo rm -rf /usr/local/share/gmock
sudo rm -rf /usr/local/share/googletest
```

---

## Next Steps

After successful installation:

1. [ ] Build the tests: `cd tests && make`
2. [ ] Run the tests: `./bin/test_runner`
3. [ ] Read [GOOGLETEST_REFERENCE.md](GOOGLETEST_REFERENCE.md) for command-line options
4. [ ] Read [TESTING_SCENARIOS.md](TESTING_SCENARIOS.md) for test scenarios
5. [ ] Read [BEGINNER_GUIDE.md](BEGINNER_GUIDE.md) to learn testing basics

---

## Additional Resources

- [Google Test 1.8.1 Release](https://github.com/google/googletest/tree/release-1.8.1)
- [Official Documentation](https://github.com/google/googletest/tree/release-1.8.1/googletest/docs)
- [Quick Fix Guide](QUICK_FIX.md) for common errors

**Last Updated:** December 26, 2025
