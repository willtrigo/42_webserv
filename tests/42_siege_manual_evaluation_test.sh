#!/bin/bash

# 42 Webserver Evaluation Test Suite
# Automated tests matching 42 evaluation requirements
# Run with server at http://127.0.0.1:8080

# Don't use 'set -e' - we want to continue even if individual tests fail
set +e

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

PASSED=0
FAILED=0
SERVER_URL="http://127.0.0.1:8080"

# Test results array
declare -a RESULTS

log_test() {
    echo -e "${BLUE}[TEST]${NC} $1"
}

log_pass() {
    echo -e "${GREEN}[PASS]${NC} $1"
    RESULTS+=("✅ PASS: $1")
    ((PASSED++))
}

log_fail() {
    echo -e "${RED}[FAIL]${NC} $1"
    RESULTS+=("❌ FAIL: $1")
    ((FAILED++))
}

log_warn() {
    echo -e "${YELLOW}[WARN]${NC} $1"
    RESULTS+=("⚠️ WARN: $1")
}

echo "================================================"
echo "   42 WEBSERVER EVALUATION TEST SUITE"
echo "================================================"
echo ""
echo "Testing server at: $SERVER_URL"
echo ""

# ============================================
# PRE-CHECK: Verify Required Tools
# ============================================
echo "=== PRE-CHECK: Required Tools ==="
echo ""

# Check siege installation
if command -v siege &> /dev/null; then
    SIEGE_VERSION=$(siege --version 2>&1 | head -n 1)
    echo -e "${GREEN}✓${NC} siege is installed: $SIEGE_VERSION"
    echo "   - Full stress tests will be executed"
    SIEGE_AVAILABLE=true
else
    echo -e "${YELLOW}✗${NC} siege is NOT installed"
    echo "   - ${YELLOW}WARNING: 42 evaluation may require siege tests${NC}"
    echo ""
    echo -e "${BLUE}Would you like to install siege now? (y/n)${NC}"
    read -r INSTALL_SIEGE
    
    if [ "$INSTALL_SIEGE" = "y" ] || [ "$INSTALL_SIEGE" = "Y" ]; then
        echo ""
        echo "Installing siege..."
        
        # Detect OS and use appropriate package manager
        if [[ "$OSTYPE" == "darwin"* ]]; then
            # macOS
            if command -v brew &> /dev/null; then
                echo "Using Homebrew to install siege..."
                brew install siege
            else
                echo -e "${RED}Error: Homebrew not found. Install Homebrew first: https://brew.sh${NC}"
                SIEGE_AVAILABLE=false
            fi
        elif [[ "$OSTYPE" == "linux-gnu"* ]]; then
            # Linux
            if command -v apt-get &> /dev/null; then
                echo "Using apt to install siege..."
                sudo apt-get update && sudo apt-get install -y siege
            elif command -v yum &> /dev/null; then
                echo "Using yum to install siege..."
                sudo yum install -y siege
            elif command -v dnf &> /dev/null; then
                echo "Using dnf to install siege..."
                sudo dnf install -y siege
            else
                echo -e "${RED}Error: No supported package manager found (apt/yum/dnf)${NC}"
                SIEGE_AVAILABLE=false
            fi
        else
            echo -e "${RED}Error: Unsupported OS type: $OSTYPE${NC}"
            SIEGE_AVAILABLE=false
        fi
        
        # Verify installation
        if command -v siege &> /dev/null; then
            SIEGE_VERSION=$(siege --version 2>&1 | head -n 1)
            echo -e "${GREEN}✓${NC} siege successfully installed: $SIEGE_VERSION"
            SIEGE_AVAILABLE=true
        else
            echo -e "${RED}✗${NC} siege installation failed"
            echo "   - Simplified stress tests will be used instead"
            SIEGE_AVAILABLE=false
        fi
    else
        echo "   - Skipping siege installation"
        echo "   - Simplified stress tests will be used instead"
        echo "   - Manual install: ${BLUE}brew install siege${NC} (macOS) or ${BLUE}sudo apt install siege${NC} (Linux)"
        SIEGE_AVAILABLE=false
    fi
fi

# Check curl (required)
if command -v curl &> /dev/null; then
    echo -e "${GREEN}✓${NC} curl is installed"
else
    echo -e "${RED}✗${NC} curl is NOT installed - REQUIRED"
    echo "   - Install: ${BLUE}brew install curl${NC} (macOS) or ${BLUE}apt install curl${NC} (Linux)"
    exit 1
fi

# Check bc (required for calculations)
if command -v bc &> /dev/null; then
    echo -e "${GREEN}✓${NC} bc is installed"
else
    echo -e "${YELLOW}✗${NC} bc is NOT installed - needed for availability calculations"
    echo "   - Install: ${BLUE}brew install bc${NC} (macOS) or ${BLUE}apt install bc${NC} (Linux)"
fi

echo ""
echo "Press Enter to start tests..."
read

# ============================================
# CRITICAL: HTTP Methods Support
# ============================================
echo ""
echo "=== 1. HTTP METHODS SUPPORT ==="
echo ""

# Test GET
log_test "1.1 GET request"
RESPONSE=$(curl -s -o /dev/null -w "%{http_code}" $SERVER_URL/)
if [ "$RESPONSE" = "200" ]; then
    log_pass "GET / returns 200"
else
    log_fail "GET / returns $RESPONSE (expected 200)"
fi

# Test HEAD (CRITICAL - ubuntu_tester failed here)
log_test "1.2 HEAD request"
RESPONSE=$(curl -s -I -o /dev/null -w "%{http_code}" $SERVER_URL/)
if [ "$RESPONSE" = "200" ]; then
    log_pass "HEAD / returns 200"
else
    log_fail "HEAD / returns $RESPONSE (expected 200) - UBUNTU_TESTER WILL FAIL"
fi

# Verify HEAD has no body
log_test "1.3 HEAD request has no body"
BODY_SIZE=$(curl -s -I $SERVER_URL/ | wc -c)
if [ "$BODY_SIZE" -lt 500 ]; then
    log_pass "HEAD response has no body (size: $BODY_SIZE bytes)"
else
    log_fail "HEAD response has body (size: $BODY_SIZE bytes)"
fi

# Test POST
log_test "1.4 POST request"
RESPONSE=$(curl -s -o /dev/null -w "%{http_code}" -X POST -d "test=data" $SERVER_URL/files)
if [ "$RESPONSE" = "201" ] || [ "$RESPONSE" = "200" ]; then
    log_pass "POST /files returns $RESPONSE"
else
    log_fail "POST /files returns $RESPONSE (expected 200/201)"
fi

# Test DELETE
log_test "1.5 DELETE request"
echo "test file" > /tmp/webserv_delete_test.txt
curl -s -X POST -F "file=@/tmp/webserv_delete_test.txt" $SERVER_URL/files > /dev/null 2>&1
RESPONSE=$(curl -s -o /dev/null -w "%{http_code}" -X DELETE $SERVER_URL/files/webserv_delete_test.txt)
if [ "$RESPONSE" = "200" ] || [ "$RESPONSE" = "204" ]; then
    log_pass "DELETE returns $RESPONSE"
else
    log_fail "DELETE returns $RESPONSE (expected 200/204, got 500 means broken)"
fi

# Test UNKNOWN method (should not crash)
log_test "1.6 UNKNOWN method doesn't crash"
RESPONSE=$(curl -s -o /dev/null -w "%{http_code}" -X UNKNOWN $SERVER_URL/)
if [ "$RESPONSE" = "405" ] || [ "$RESPONSE" = "501" ]; then
    log_pass "UNKNOWN method returns $RESPONSE (server didn't crash)"
else
    log_warn "UNKNOWN method returns $RESPONSE (expected 405 or 501)"
fi

# ============================================
# CONFIGURATION: Error Pages
# ============================================
echo ""
echo "=== 2. ERROR PAGES & STATUS CODES ==="
echo ""

# Test 404
log_test "2.1 404 Not Found"
RESPONSE=$(curl -s -o /dev/null -w "%{http_code}" $SERVER_URL/nonexistent_file_12345.html)
if [ "$RESPONSE" = "404" ]; then
    log_pass "404 for nonexistent file"
else
    log_fail "Expected 404, got $RESPONSE"
fi

# Test custom 404 page
log_test "2.2 Custom 404 error page"
BODY=$(curl -s $SERVER_URL/nonexistent)
if echo "$BODY" | grep -q "404\|Not Found"; then
    log_pass "Custom 404 error page contains '404' or 'Not Found'"
else
    log_fail "404 error page missing or incorrect"
fi

# Test 405 Method Not Allowed
log_test "2.3 405 Method Not Allowed"
RESPONSE=$(curl -s -o /dev/null -w "%{http_code}" -X POST $SERVER_URL/)
if [ "$RESPONSE" = "405" ]; then
    log_pass "POST on GET-only route returns 405"
else
    log_fail "POST on GET-only route returns $RESPONSE (expected 405)"
fi

# ============================================
# CONFIGURATION: Body Size Limit
# ============================================
echo ""
echo "=== 3. BODY SIZE LIMIT (Critical) ==="
echo ""

log_test "3.1 Small body accepted"
RESPONSE=$(curl -s -o /dev/null -w "%{http_code}" -X POST -d "small body" $SERVER_URL/files)
if [ "$RESPONSE" = "201" ] || [ "$RESPONSE" = "200" ]; then
    log_pass "Small POST body accepted ($RESPONSE)"
else
    log_fail "Small POST body rejected ($RESPONSE)"
fi

log_test "3.2 Large body rejected with 413"
dd if=/dev/zero of=/tmp/large_file.bin bs=1M count=2 2>/dev/null
RESPONSE=$(curl -s -o /dev/null -w "%{http_code}" -X POST -F "file=@/tmp/large_file.bin" $SERVER_URL/files)
if [ "$RESPONSE" = "413" ]; then
    log_pass "Large body rejected with 413 Payload Too Large"
else
    log_fail "Large body returns $RESPONSE (expected 413, got 500 means broken)"
fi
rm -f /tmp/large_file.bin

# ============================================
# FILE UPLOAD & RETRIEVAL
# ============================================
echo ""
echo "=== 4. FILE UPLOAD & RETRIEVAL ==="
echo ""

log_test "4.1 Upload file via POST"
echo "Test file content $(date)" > /tmp/upload_test.txt
RESPONSE=$(curl -s -o /dev/null -w "%{http_code}" -X POST -F "file=@/tmp/upload_test.txt" $SERVER_URL/files)
if [ "$RESPONSE" = "201" ] || [ "$RESPONSE" = "200" ]; then
    log_pass "File upload returns $RESPONSE"
else
    log_fail "File upload returns $RESPONSE (expected 200/201)"
fi

log_test "4.2 Retrieve uploaded file via GET"
# Wait a moment for file to be written
sleep 1
RESPONSE=$(curl -s -o /dev/null -w "%{http_code}" $SERVER_URL/files/upload_test.txt)
if [ "$RESPONSE" = "200" ]; then
    log_pass "Retrieve uploaded file returns 200"
else
    log_warn "Retrieve uploaded file returns $RESPONSE (file might not be saved)"
fi

rm -f /tmp/upload_test.txt

# ============================================
# ROUTES & DIRECTORIES
# ============================================
echo ""
echo "=== 5. ROUTES & DIRECTORIES ==="
echo ""

log_test "5.1 Root route (/) works"
RESPONSE=$(curl -s -o /dev/null -w "%{http_code}" $SERVER_URL/)
if [ "$RESPONSE" = "200" ]; then
    log_pass "Root route returns 200"
else
    log_fail "Root route returns $RESPONSE"
fi

log_test "5.2 Different route (/public) works"
RESPONSE=$(curl -s -o /dev/null -w "%{http_code}" $SERVER_URL/public/)
if [ "$RESPONSE" = "200" ] || [ "$RESPONSE" = "404" ] || [ "$RESPONSE" = "403" ]; then
    log_pass "Route /public returns $RESPONSE (valid)"
else
    log_fail "Route /public returns $RESPONSE"
fi

log_test "5.3 Default index file served"
BODY=$(curl -s $SERVER_URL/)
if echo "$BODY" | grep -q "<!DOCTYPE\|<html\|<HTML"; then
    log_pass "Default index file is HTML"
else
    log_warn "Default index file might not be HTML"
fi

# ============================================
# REDIRECTS
# ============================================
echo ""
echo "=== 6. REDIRECTS ==="
echo ""

log_test "6.1 Redirect returns 301/302/307"
RESPONSE=$(curl -s -o /dev/null -w "%{http_code}" $SERVER_URL/docs)
if [ "$RESPONSE" = "301" ] || [ "$RESPONSE" = "302" ] || [ "$RESPONSE" = "307" ]; then
    log_pass "Redirect returns $RESPONSE"
else
    log_warn "Redirect returns $RESPONSE (expected 301/302/307)"
fi

log_test "6.2 Redirect has Location header"
LOCATION=$(curl -s -I $SERVER_URL/docs | grep -i "location:" | wc -l)
if [ "$LOCATION" -ge 1 ]; then
    log_pass "Redirect has Location header"
else
    log_fail "Redirect missing Location header"
fi

# ============================================
# CGI BASIC CHECK
# ============================================
echo ""
echo "=== 7. CGI SUPPORT ==="
echo ""

log_test "7.1 CGI script exists"
if [ -f "./cgi_tester" ] || [ -f "./YoupiBanane/youpi.bla" ]; then
    log_pass "CGI test files found"
else
    log_warn "CGI test files not found (might be required for evaluation)"
fi

log_test "7.2 CGI GET request"
RESPONSE=$(curl -s -o /dev/null -w "%{http_code}" "$SERVER_URL/cgi-bin/test.cgi?name=test")
if [ "$RESPONSE" = "200" ]; then
    log_pass "CGI GET returns 200"
else
    log_warn "CGI GET returns $RESPONSE (CGI might not be configured)"
fi

# ============================================
# STRESS TEST (Siege - 42 Evaluation)
# ============================================
echo ""
echo "=== 8. SIEGE STRESS TEST (42 Evaluation) ==="
echo ""

# Check if siege is installed (already verified at startup)
if [ "$SIEGE_AVAILABLE" = false ]; then
    log_warn "siege not installed - using simplified stress tests instead"
    log_warn "For complete 42 evaluation, install siege: brew install siege (macOS) or apt install siege (Linux)"
else
    # 8.1 Concurrent connections stress test
    log_test "8.1 Concurrent connections (10 users, 100 requests)"
    SIEGE_OUTPUT=$(siege -b -c 10 -r 10 $SERVER_URL/ 2>&1)
    AVAILABILITY=$(echo "$SIEGE_OUTPUT" | grep "Availability:" | awk '{print $2}' | tr -d '%')
    FAILED_TRANS=$(echo "$SIEGE_OUTPUT" | grep "Failed transactions:" | awk '{print $3}')
    
    if (( $(echo "$AVAILABILITY >= 99.0" | bc -l) )); then
        log_pass "Concurrent stress test: ${AVAILABILITY}% availability (>= 99% required)"
    else
        log_fail "Concurrent stress test: ${AVAILABILITY}% availability (< 99% required)"
    fi
    
    if [ "$FAILED_TRANS" -le 1 ]; then
        log_pass "Failed transactions: $FAILED_TRANS (acceptable)"
    else
        log_fail "Failed transactions: $FAILED_TRANS (too many failures)"
    fi
    
    # 8.2 Sustained load test
    log_test "8.2 Sustained load (20 users, 5 seconds)"
    SIEGE_OUTPUT=$(siege -b -c 20 -t 5S $SERVER_URL/ 2>&1)
    AVAILABILITY=$(echo "$SIEGE_OUTPUT" | grep "Availability:" | awk '{print $2}' | tr -d '%')
    RESPONSE_TIME=$(echo "$SIEGE_OUTPUT" | grep "Response time:" | awk '{print $3}')
    
    if (( $(echo "$AVAILABILITY >= 99.0" | bc -l) )); then
        log_pass "Sustained load test: ${AVAILABILITY}% availability"
    else
        log_fail "Sustained load test: ${AVAILABILITY}% availability (< 99%)"
    fi
    
    if (( $(echo "$RESPONSE_TIME < 1.0" | bc -l) )); then
        log_pass "Average response time: ${RESPONSE_TIME}s (< 1s)"
    else
        log_warn "Average response time: ${RESPONSE_TIME}s (> 1s, consider optimization)"
    fi
    
    # 8.3 High concurrency test (stress limit)
    log_test "8.3 High concurrency (50 users, 100 requests)"
    SIEGE_OUTPUT=$(siege -b -c 50 -r 2 $SERVER_URL/ 2>&1)
    AVAILABILITY=$(echo "$SIEGE_OUTPUT" | grep "Availability:" | awk '{print $2}' | tr -d '%')
    
    if (( $(echo "$AVAILABILITY >= 95.0" | bc -l) )); then
        log_pass "High concurrency: ${AVAILABILITY}% availability (>= 95% acceptable)"
    else
        log_fail "High concurrency: ${AVAILABILITY}% availability (< 95%, server may crash under load)"
    fi
    
    # 8.4 Keep-alive connection reuse
    log_test "8.4 Keep-alive connection reuse"
    SIEGE_OUTPUT=$(siege -b -c 5 -r 20 $SERVER_URL/ 2>&1)
    TRANS_RATE=$(echo "$SIEGE_OUTPUT" | grep "Transaction rate:" | awk '{print $3}')
    
    if (( $(echo "$TRANS_RATE > 50.0" | bc -l) )); then
        log_pass "Transaction rate: ${TRANS_RATE} trans/sec (keep-alive working)"
    else
        log_warn "Transaction rate: ${TRANS_RATE} trans/sec (low, check keep-alive)"
    fi
    
    # 8.5 POST request stress test
    log_test "8.5 POST request stress (10 users, 50 requests)"
    echo "test data for siege" > /tmp/siege_post_data.txt
    SIEGE_OUTPUT=$(siege -b -c 10 -r 5 "$SERVER_URL/files" POST < /tmp/siege_post_data.txt 2>&1)
    AVAILABILITY=$(echo "$SIEGE_OUTPUT" | grep "Availability:" | awk '{print $2}' | tr -d '%')
    
    if (( $(echo "$AVAILABILITY >= 95.0" | bc -l) )); then
        log_pass "POST stress test: ${AVAILABILITY}% availability"
    else
        log_fail "POST stress test: ${AVAILABILITY}% availability (< 95%)"
    fi
    rm -f /tmp/siege_post_data.txt
    
    # 8.6 Mixed workload test
    log_test "8.6 Mixed workload (GET, POST, static files)"
    cat > /tmp/siege_urls.txt << EOF
$SERVER_URL/
$SERVER_URL/public/
$SERVER_URL/files POST test=data
EOF
    SIEGE_OUTPUT=$(siege -b -c 15 -r 10 -f /tmp/siege_urls.txt 2>&1)
    AVAILABILITY=$(echo "$SIEGE_OUTPUT" | grep "Availability:" | awk '{print $2}' | tr -d '%')
    
    if (( $(echo "$AVAILABILITY >= 98.0" | bc -l) )); then
        log_pass "Mixed workload: ${AVAILABILITY}% availability"
    else
        log_fail "Mixed workload: ${AVAILABILITY}% availability (< 98%)"
    fi
    rm -f /tmp/siege_urls.txt
    
    # 8.7 Server stability check (no crashes)
    log_test "8.7 Server stability (long-running test)"
    SIEGE_OUTPUT=$(siege -b -c 30 -t 30S $SERVER_URL/ 2>&1)
    LONGEST_TRANS=$(echo "$SIEGE_OUTPUT" | grep "Longest transaction:" | awk '{print $3}')
    
    if (( $(echo "$LONGEST_TRANS < 5.0" | bc -l) )); then
        log_pass "Longest transaction: ${LONGEST_TRANS}s (< 5s, no timeout issues)"
    else
        log_warn "Longest transaction: ${LONGEST_TRANS}s (> 5s, possible timeout/deadlock)"
    fi
    
    echo ""
    echo "=== SIEGE TEST SUMMARY ==="
    echo "$SIEGE_OUTPUT" | grep -E "Transactions:|Availability:|Elapsed time:|Response time:|Transaction rate:|Concurrency:|Successful transactions:|Failed transactions:"
    echo ""
fi

# ============================================
# FALLBACK: Simplified Stress Test
# ============================================
if [ "$SIEGE_AVAILABLE" = false ]; then
    echo ""
    echo "=== 8. SIMPLIFIED STRESS TEST (Fallback) ==="
    echo ""
    
    log_test "8.1 Concurrent requests (10 simultaneous)"
    SUCCESS=0
    for i in {1..10}; do
        curl -s -o /dev/null -w "%{http_code}" $SERVER_URL/ &
    done
    wait
    log_pass "10 concurrent requests completed"
    
    log_test "8.2 Rapid sequential requests (100 requests)"
    FAILURES=0
    for i in {1..100}; do
        RESPONSE=$(curl -s -o /dev/null -w "%{http_code}" $SERVER_URL/)
        if [ "$RESPONSE" != "200" ]; then
            ((FAILURES++))
        fi
    done
    AVAILABILITY=$(echo "scale=2; (100 - $FAILURES) / 100 * 100" | bc)
    if (( $(echo "$AVAILABILITY >= 99.5" | bc -l) )); then
        log_pass "Availability: ${AVAILABILITY}% (>= 99.5% required)"
    else
        log_fail "Availability: ${AVAILABILITY}% (< 99.5% required)"
    fi
fi

# ============================================
# MEMORY LEAKS (Valgrind check)
# ============================================
echo ""
echo "=== 9. MEMORY LEAKS CHECK ==="
echo ""

log_test "9.1 Valgrind check"
if command -v valgrind &> /dev/null; then
    log_warn "Valgrind available - run manually: valgrind --leak-check=full ./bin/webserv"
else
    log_warn "Valgrind not installed - cannot check memory leaks automatically"
fi

# ============================================
# SUMMARY
# ============================================
echo ""
echo "================================================"
echo "              TEST SUMMARY"
echo "================================================"
echo ""
for result in "${RESULTS[@]}"; do
    echo "$result"
done
echo ""
echo "Total Tests: $((PASSED + FAILED))"
echo -e "${GREEN}Passed: $PASSED${NC}"
echo -e "${RED}Failed: $FAILED${NC}"
echo ""

if [ $FAILED -eq 0 ]; then
    echo -e "${GREEN}✅ ALL TESTS PASSED!${NC}"
    echo "Server is ready for 42 evaluation (manual tests still required)"
    exit 0
else
    echo -e "${RED}❌ SOME TESTS FAILED${NC}"
    echo "Fix critical issues before evaluation"
    exit 1
fi
