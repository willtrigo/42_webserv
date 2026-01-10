#!/bin/bash

# Quick Fix Workflow Helper
# Automates the test → fix → verify cycle
# Supports both unit tests and integration tests

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
    echo -e "${YELLOW}Usage: $0 <test-suite-name> [config-file]${NC}"
    echo ""
    echo "Unit test suites:"
    echo "  - Port"
    echo "  - Path"
    echo "  - Permission"
    echo "  - Host"
    echo "  - ErrorCode"
    echo "  - Size"
    echo "  - Uri"
    echo "  - HttpMethod"
    echo ""
    echo "Integration test suites:"
    echo "  - HttpServerIntegration"
    echo "  - FileHandlerIntegration"
    echo "  - AdvancedIntegration"
    echo "  - MultiPortIntegration"
    echo "  - CgiIntegration"
    echo ""
    echo "Examples:"
    echo "  $0 Port                          # Run unit test"
    echo "  $0 HttpServerIntegration         # Run integration test with default.conf"
    echo "  $0 HttpServerIntegration webserv.conf  # Run with custom config"
    exit 1
fi

TEST_SUITE="$1"
CONFIG_FILE="${2:-default.conf}"

# Detect if this is an integration test
IS_INTEGRATION=false
if [[ "$TEST_SUITE" =~ Integration$ ]]; then
    IS_INTEGRATION=true
fi

cd "$PROJECT_ROOT"

# Function to start webserver
start_webserver() {
    local config="$1"
    echo -e "${BLUE}Starting webserver with $config...${NC}"
    # Kill any existing webserver instances
    pkill -9 webserv 2>/dev/null || true
    sleep 1
    
    # Start webserver in background
    ./bin/webserv "conf/$config" > /tmp/webserv_test.log 2>&1 &
    WEBSERV_PID=$!
    
    # Wait for server to be ready
    sleep 2
    if ! kill -0 $WEBSERV_PID 2>/dev/null; then
        echo -e "${RED}❌ Failed to start webserver${NC}"
        cat /tmp/webserv_test.log
        exit 1
    fi
    echo -e "${GREEN}✅ Webserver started (PID: $WEBSERV_PID)${NC}"
}

# Function to stop webserver
stop_webserver() {
    if [ -n "$WEBSERV_PID" ] && kill -0 $WEBSERV_PID 2>/dev/null; then
        echo -e "${BLUE}Stopping webserver...${NC}"
        kill -9 $WEBSERV_PID 2>/dev/null || true
        sleep 1
        echo -e "${GREEN}✅ Webserver stopped${NC}"
    fi
}

# Trap to ensure webserver is stopped on exit
trap stop_webserver EXIT

echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}🔧 Fix Workflow: ${TEST_SUITE}${NC}"
if [ "$IS_INTEGRATION" = true ]; then
    echo -e "${BLUE}(Integration Test Mode with $CONFIG_FILE)${NC}"
fi
echo -e "${BLUE}========================================${NC}"
echo ""

# For integration tests, start webserver
if [ "$IS_INTEGRATION" = true ]; then
    start_webserver "$CONFIG_FILE"
    echo ""
fi

# Step 1: Show current failures
echo -e "${BLUE}Step 1/4: Current test status${NC}"
cd "$PROJECT_ROOT/tests"
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

# For integration tests, restart webserver after rebuild
if [ "$IS_INTEGRATION" = true ]; then
    stop_webserver
    sleep 1
    start_webserver "$CONFIG_FILE"
    echo ""
fi

# Step 3: Run tests again
echo -e "${BLUE}Step 3/4: Running ${TEST_SUITE} tests${NC}"
cd "$PROJECT_ROOT/tests"
if ./bin/test_runner --gtest_filter="${TEST_SUITE}Test.*"; then
    echo -e "${GREEN}✅ All ${TEST_SUITE} tests passing!${NC}"
else
    echo -e "${YELLOW}⚠️  Some tests still failing${NC}"
fi
echo ""

# Step 4: Regression check
echo -e "${BLUE}Step 4/4: Checking for regressions${NC}"
cd "$PROJECT_ROOT"
if "$SCRIPT_DIR/check_regression.sh"; then
    echo -e "${GREEN}✅ No regressions detected${NC}"
else
    echo -e "${YELLOW}⚠️  Output changed - review diffs${NC}"
fi
echo ""

# Clean up webserver for integration tests
if [ "$IS_INTEGRATION" = true ]; then
    stop_webserver
fi

echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}✅ Workflow complete!${NC}"
echo -e "${BLUE}========================================${NC}"
echo ""
echo "Next steps:"
echo "  - Review any remaining failures"
echo "  - Commit your changes: git add -p && git commit"
echo "  - Move to next test suite"
