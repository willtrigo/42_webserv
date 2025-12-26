#!/bin/bash

# Script to check Google Test installation compatibility with C++98

echo "======================================================"
echo "Google Test C++98 Compatibility Checker"
echo "======================================================"
echo ""

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

# Check if gtest is installed
if [ ! -d "/usr/local/include/gtest" ]; then
    echo -e "${RED}✗ Google Test NOT installed${NC}"
    echo ""
    echo "To install:"
    echo "  sudo ./install_gtest.sh"
    exit 1
fi

echo -e "${GREEN}✓ Google Test is installed${NC}"
echo ""

# Check libraries
echo "Checking libraries..."
if [ -f "/usr/local/lib/libgtest.a" ]; then
    echo -e "${GREEN}  ✓ libgtest.a found${NC}"
else
    echo -e "${RED}  ✗ libgtest.a NOT found${NC}"
fi

if [ -f "/usr/local/lib/libgtest_main.a" ]; then
    echo -e "${GREEN}  ✓ libgtest_main.a found${NC}"
else
    echo -e "${RED}  ✗ libgtest_main.a NOT found${NC}"
fi
echo ""

# Check for C++11 features
echo "Checking C++98 compatibility..."

C11_FEATURES_FOUND=0

# Check for std::tuple
if grep -r "std::tuple" /usr/local/include/gtest/ > /dev/null 2>&1; then
    echo -e "${RED}  ✗ Found std::tuple (C++11 feature)${NC}"
    C11_FEATURES_FOUND=1
fi

# Check for std::move
if grep -r "std::move" /usr/local/include/gtest/ > /dev/null 2>&1; then
    echo -e "${RED}  ✗ Found std::move (C++11 feature)${NC}"
    C11_FEATURES_FOUND=1
fi

# Check for std::shared_ptr
if grep -r "std::shared_ptr" /usr/local/include/gtest/ > /dev/null 2>&1; then
    echo -e "${RED}  ✗ Found std::shared_ptr (C++11 feature)${NC}"
    C11_FEATURES_FOUND=1
fi

# Check for constexpr
if grep -r "constexpr" /usr/local/include/gtest/ > /dev/null 2>&1; then
    echo -e "${RED}  ✗ Found constexpr (C++11 feature)${NC}"
    C11_FEATURES_FOUND=1
fi

echo ""

# Final result
if [ $C11_FEATURES_FOUND -eq 0 ]; then
    echo "======================================================"
    echo -e "${GREEN}SUCCESS: Google Test is C++98 compatible!${NC}"
    echo "======================================================"
    echo ""
    echo "You can now build and run tests:"
    echo "  cd tests/"
    echo "  make"
    echo "  make test"
    echo ""
    exit 0
else
    echo "======================================================"
    echo -e "${RED}ERROR: Google Test is NOT C++98 compatible!${NC}"
    echo "======================================================"
    echo ""
    echo "You have installed a version that requires C++11+."
    echo ""
    echo "To fix this, run:"
    echo "  sudo ./install_gtest.sh"
    echo ""
    echo "This will install Google Test 1.8.1 (C++98 compatible)."
    echo ""
    exit 1
fi
