#!/bin/bash

# Quick Fix Workflow Helper
# Automates the test → fix → verify cycle

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/../.." && pwd)"

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

# Check argument
if [ $# -lt 1 ]; then
    echo -e "${YELLOW}Usage: $0 <test-suite-name>${NC}"
    echo ""
    echo "Available test suites:"
    echo "  - Port"
    echo "  - Path"
    echo "  - Permission"
    echo "  - Host"
    echo "  - ErrorCode"
    echo "  - Size"
    echo "  - Uri"
    echo "  - HttpMethod"
    echo ""
    echo "Example: $0 Port"
    exit 1
fi

TEST_SUITE="$1"

cd "$PROJECT_ROOT"

echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}🔧 Fix Workflow: ${TEST_SUITE}${NC}"
echo -e "${BLUE}========================================${NC}"
echo ""

# Step 1: Show current failures
echo -e "${BLUE}Step 1/4: Current test status${NC}"
cd tests
./bin/test_runner --gtest_filter="${TEST_SUITE}Test.*" 2>&1 | tail -30
echo ""

# Ask if user wants to continue
read -p "Press Enter to rebuild and retest, or Ctrl+C to exit..."

# Step 2: Rebuild
echo ""
echo -e "${BLUE}Step 2/4: Rebuilding project${NC}"
cd "$PROJECT_ROOT"
if make; then
    echo -e "${GREEN}✅ Build successful${NC}"
else
    echo -e "${RED}❌ Build failed${NC}"
    exit 1
fi
echo ""

# Step 3: Run tests again
echo -e "${BLUE}Step 3/4: Running ${TEST_SUITE} tests${NC}"
cd tests
if ./bin/test_runner --gtest_filter="${TEST_SUITE}Test.*"; then
    echo -e "${GREEN}✅ All ${TEST_SUITE} tests passing!${NC}"
else
    echo -e "${YELLOW}⚠️  Some tests still failing${NC}"
fi
echo ""

# Step 4: Regression check
echo -e "${BLUE}Step 4/4: Checking for regressions${NC}"
if "$SCRIPT_DIR/check_regression.sh"; then
    echo -e "${GREEN}✅ No regressions detected${NC}"
else
    echo -e "${YELLOW}⚠️  Output changed - review diffs${NC}"
fi
echo ""

echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}✅ Workflow complete!${NC}"
echo -e "${BLUE}========================================${NC}"
echo ""
echo "Next steps:"
echo "  - Review any remaining failures"
echo "  - Commit your changes: git add -p && git commit"
echo "  - Move to next test suite"
