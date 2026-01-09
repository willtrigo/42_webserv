#!/bin/bash

# QA Regression Check Script
# Compares current server behavior against baseline outputs

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/../.." && pwd)"
BASELINE_DIR="$SCRIPT_DIR/baseline"
TMP_DIR="$SCRIPT_DIR/current_tdd_fix"

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Create temp directory
mkdir -p "$TMP_DIR"

cd "$PROJECT_ROOT"

echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}🧪 QA Regression Check${NC}"
echo -e "${BLUE}========================================${NC}"
echo ""

# Check if binary exists
if [ ! -f "./bin/webserv" ]; then
    echo -e "${RED}❌ Binary not found. Run 'make' first.${NC}"
    exit 1
fi

# Function to run test and compare
run_test() {
    local test_name="$1"
    local config_arg="$2"
    local baseline_file="$3"
    local test_number="$4"
    
    echo -e "${BLUE}Testing: ${test_name}${NC}"
    
    # Create temp files
    local tmp_output=$(mktemp)
    local tmp_pid=$(mktemp)
    
    # Start webserver in background, redirect output to file
    if [ -z "$config_arg" ]; then
        ./bin/webserv > "$tmp_output" 2>&1 &
    else
        ./bin/webserv $config_arg > "$tmp_output" 2>&1 &
    fi
    
    local webserv_pid=$!
    echo $webserv_pid > "$tmp_pid"
    
    # Wait for ready message (max 5 seconds)
    local wait_count=0
    local max_wait=50  # 5 seconds (50 * 0.1s)
    
    while [ $wait_count -lt $max_wait ]; do
        if grep -q "Starting event loop" "$tmp_output" 2>/dev/null; then
            break
        fi
        
        # Check if process is still running
        if ! kill -0 $webserv_pid 2>/dev/null; then
            # Process exited, that's ok for some tests
            break
        fi
        
        sleep 0.1
        wait_count=$((wait_count + 1))
    done
    
    # Send SIGINT to gracefully shutdown (if still running)
    if kill -0 $webserv_pid 2>/dev/null; then
        kill -INT $webserv_pid 2>/dev/null || true
        
        # Wait for process to exit (max 2 seconds)
        local exit_wait=0
        while [ $exit_wait -lt 20 ] && kill -0 $webserv_pid 2>/dev/null; do
            sleep 0.1
            exit_wait=$((exit_wait + 1))
        done
        
        # Force kill if still running
        if kill -0 $webserv_pid 2>/dev/null; then
            kill -9 $webserv_pid 2>/dev/null || true
        fi
    fi
    
    # Copy output for comparison
    local baseline_name=$(basename $baseline_file)
    local current_name="${baseline_name/baseline_/current_}"
    local current_output="$TMP_DIR/$current_name"
    cp "$tmp_output" "$current_output"
    
    # Generate Markdown report (always)
    local diff_file="$TMP_DIR/diff_$(basename ${baseline_file%.txt}).md"
    
    # Create header for markdown
    cat > "$diff_file" << EOF
# 🔍 Regression Test Report

**Test:** ${test_name}  
**File:** \`$(basename $baseline_file)\`  
**Date:** $(date '+%Y-%m-%d %H:%M:%S')

---

## 🧪 Test Output

\`\`\`
Test ${test_number}/4: Server $(echo "$test_name" | tr '[:upper:]' '[:lower:]')
Testing: ${test_name}
EOF
    
    # Compare
    if diff -q "$baseline_file" "$current_output" > /dev/null 2>&1; then
        echo -e "${GREEN}✅ PASS - No changes detected${NC}"
        
        # Append PASS status to markdown
        cat >> "$diff_file" << EOF
✅ PASS - No changes detected
\`\`\`

---

## ✅ Result

**No regressions detected.** Server output matches baseline perfectly.

EOF
        rm -f "$tmp_output" "$tmp_pid"
        return 0
    else
        echo -e "${YELLOW}⚠️  DIFF - Changes detected${NC}"
        
        # Append DIFF status to markdown header
        cat >> "$diff_file" << EOF
⚠️  DIFF - Changes detected
\`\`\`

---

## 📊 Summary

EOF
        
        # Count changes
        local additions=$(diff -u "$baseline_file" "$current_output" 2>/dev/null | grep -c "^+" || echo "0")
        local deletions=$(diff -u "$baseline_file" "$current_output" 2>/dev/null | grep -c "^-" || echo "0")
        
        cat >> "$diff_file" << EOF
- **Added lines:** ${additions}
- **Removed lines:** ${deletions}

---

## 🔄 Changes

EOF
        
        # Generate unified diff and convert to markdown
        diff -u "$baseline_file" "$current_output" 2>&1 | while IFS= read -r line; do
            case "$line" in
                ---*)
                    echo "" >> "$diff_file"
                    echo "### ❌ Baseline (Expected)" >> "$diff_file"
                    echo '```diff' >> "$diff_file"
                    ;;
                +++*)
                    echo '```' >> "$diff_file"
                    echo "" >> "$diff_file"
                    echo "### ✅ Current (Actual)" >> "$diff_file"
                    echo '```diff' >> "$diff_file"
                    ;;
                @@*)
                    echo '```' >> "$diff_file"
                    echo "" >> "$diff_file"
                    echo "#### 📍 Location: \`$line\`" >> "$diff_file"
                    echo "" >> "$diff_file"
                    echo '```diff' >> "$diff_file"
                    ;;
                -*)
                    echo "$line" >> "$diff_file"
                    ;;
                +*)
                    echo "$line" >> "$diff_file"
                    ;;
                *)
                    echo " $line" >> "$diff_file"
                    ;;
            esac
        done || true
        
echo '```' >> "$diff_file"

        cat >> "$diff_file" << EOF

---

## 📝 Legend

- \`-\` **Red lines** = Removed from baseline (what was expected)
- \`+\` **Green lines** = Added in current output (what we got now)
- Unchanged lines = Context

---

## 📂 Files

- **Baseline:** \`$(realpath "$baseline_file")\`
- **Current:** \`$(realpath "$current_output")\`
- **This report:** \`$(realpath "$diff_file")\`

EOF
        
        # Show summary in terminal
        echo ""
        echo -e "${YELLOW}📝 Markdown diff generated:${NC}"
        echo -e "   ${BLUE}$diff_file${NC}"
        echo -e "${YELLOW}   Open in VSCode to view formatted diff${NC}"
        
        rm -f "$tmp_output" "$tmp_pid"
        return 1
    fi
    echo ""
}

# Run tests
FAILED=0
PASSED=0

echo "Test 1/4: Server without config"
if run_test "No Config" "" "$BASELINE_DIR/baseline_no_config.txt" "1"; then
    PASSED=$((PASSED + 1))
else
    FAILED=$((FAILED + 1))
fi
echo ""

echo "Test 2/4: Server with default.conf"
if run_test "default.conf" "./conf/default.conf" "$BASELINE_DIR/baseline_default_conf.txt" "2"; then
    PASSED=$((PASSED + 1))
else
    FAILED=$((FAILED + 1))
fi
echo ""

echo "Test 3/4: Server with webserv.conf"
if run_test "webserv.conf" "./conf/webserv.conf" "$BASELINE_DIR/baseline_webserv_conf.txt" "3"; then
    PASSED=$((PASSED + 1))
else
    FAILED=$((FAILED + 1))
fi
echo ""

echo "Test 4/4: Server with webserv_py_cgi.conf"
if run_test "webserv_py_cgi.conf" "./conf/webserv_py_cgi.conf" "$BASELINE_DIR/baseline_webserv_py_cgi_conf.txt" "4"; then
    PASSED=$((PASSED + 1))
else
    FAILED=$((FAILED + 1))
fi
echo ""

# Summary
echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}📊 Summary${NC}"
echo -e "${BLUE}========================================${NC}"
echo -e "Passed: ${GREEN}${PASSED}/4${NC}"
echo -e "Changed: ${YELLOW}${FAILED}/4${NC}"
echo ""

if [ $FAILED -eq 0 ]; then
    echo -e "${GREEN}✅ All tests match baseline - No regression!${NC}"
    exit 0
else
    echo -e "${YELLOW}⚠️  Some outputs changed - Review diffs in $TMP_DIR/${NC}"
    echo -e "${YELLOW}   If changes are intentional, update baselines with:${NC}"
    echo -e "${YELLOW}   ./tests/qa_benchmark/update_baselines.sh${NC}"
    exit 1
fi
