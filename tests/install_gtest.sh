#!/bin/bash

# Script to install Google Test 1.8.1 (C++98 compatible)
# For 42_webserver project

set -e  # Exit on error

echo "=================================================="
echo "Google Test 1.8.1 Installation Script"
echo "Compatible with C++98 (42 School Projects)"
echo "=================================================="
echo ""

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Step 1: Check if running with sudo (if needed)
if [ "$EUID" -ne 0 ] && [ ! -w /usr/local/lib ]; then 
    echo -e "${RED}ERROR: This script needs sudo privileges to install to /usr/local${NC}"
    echo "Please run: sudo $0"
    exit 1
fi

# Step 2: Remove existing Google Test installations
echo -e "${YELLOW}[1/5] Removing any existing Google Test installations...${NC}"
rm -rf /usr/local/include/gtest
rm -rf /usr/local/lib/libgtest*
rm -rf /usr/local/lib/cmake/GTest
echo -e "${GREEN}✓ Cleanup complete${NC}"
echo ""

# Step 3: Download Google Test 1.8.1
echo -e "${YELLOW}[2/5] Downloading Google Test 1.8.1...${NC}"
cd /tmp
if [ -f "release-1.8.1.tar.gz" ]; then
    rm release-1.8.1.tar.gz
fi
wget -q https://github.com/google/googletest/archive/refs/tags/release-1.8.1.tar.gz
echo -e "${GREEN}✓ Download complete${NC}"
echo ""

# Step 4: Extract and build
echo -e "${YELLOW}[3/5] Extracting archive...${NC}"
tar -xzf release-1.8.1.tar.gz
cd googletest-release-1.8.1

# Patch the source code to fix the uninitialized variable warning
echo "  Patching source code..."
sed -i 's/int dummy;/int dummy = 0;/' googletest/src/gtest-death-test.cc

echo -e "${GREEN}✓ Extraction complete${NC}"
echo ""

echo -e "${YELLOW}[4/5] Building Google Test...${NC}"
mkdir -p build
cd build

# Fix for newer CMake versions and compilers
# Disable warnings-as-errors by turning off strict warnings
echo "  Running CMake..."
cmake .. \
    -DCMAKE_CXX_FLAGS="-std=c++98" \
    -Dgtest_disable_pthreads=OFF \
    -DCMAKE_POLICY_VERSION_MINIMUM=3.5 \
    > cmake_output.log 2>&1

if [ $? -ne 0 ]; then
    echo -e "${RED}✗ CMake configuration failed${NC}"
    cat cmake_output.log
    exit 1
fi

# Disable the -Werror flag that Google Test adds
echo "  Removing -Werror from build files..."
find . -name "flags.make" -exec sed -i 's/-Werror//g' {} \;
find . -name "link.txt" -exec sed -i 's/-Werror//g' {} \;

echo "  Compiling..."
make -j$(nproc) > make_output.log 2>&1

if [ $? -ne 0 ]; then
    echo -e "${RED}✗ Compilation failed${NC}"
    echo "Last 30 lines of output:"
    tail -30 make_output.log
    exit 1
fi

echo -e "${GREEN}✓ Build complete${NC}"
echo ""

# Step 5: Install
echo -e "${YELLOW}[5/5] Installing to /usr/local...${NC}"
make install > /dev/null
echo -e "${GREEN}✓ Installation complete${NC}"
echo ""

# Step 6: Cleanup
echo -e "${YELLOW}Cleaning up temporary files...${NC}"
cd /tmp
rm -rf googletest-release-1.8.1 release-1.8.1.tar.gz
echo -e "${GREEN}✓ Cleanup complete${NC}"
echo ""

# Step 7: Verify installation
echo "=================================================="
echo "Verifying installation..."
echo "=================================================="

if [ -f "/usr/local/lib/libgtest.a" ]; then
    echo -e "${GREEN}✓ libgtest.a found${NC}"
else
    echo -e "${RED}✗ libgtest.a NOT found${NC}"
    exit 1
fi

if [ -f "/usr/local/lib/libgtest_main.a" ]; then
    echo -e "${GREEN}✓ libgtest_main.a found${NC}"
else
    echo -e "${RED}✗ libgtest_main.a NOT found${NC}"
    exit 1
fi

if [ -d "/usr/local/include/gtest" ]; then
    echo -e "${GREEN}✓ gtest headers found${NC}"
else
    echo -e "${RED}✗ gtest headers NOT found${NC}"
    exit 1
fi

# Check for C++11 features (should NOT be present in actual code in 1.8.1)
# Note: Some comments/docs may mention C++11, but the actual API should be C++98
if grep -r "std::tuple" /usr/local/include/gtest/gtest.h > /dev/null 2>&1; then
    echo -e "${RED}✗ ERROR: Found C++11 tuple in main header (wrong version!)${NC}"
    exit 1
else
    echo -e "${GREEN}✓ C++98 compatible (no C++11 tuple in main API)${NC}"
fi

echo ""
echo "=================================================="
echo -e "${GREEN}SUCCESS! Google Test 1.8.1 installed correctly${NC}"
echo "=================================================="
echo ""
echo "You can now build and run tests:"
echo "  cd tests/"
echo "  make"
echo "  make test"
echo ""
