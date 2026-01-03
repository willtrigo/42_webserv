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
    local command="$2"
    local baseline_file="$3"
    local test_number="$4"
    
    echo -e "${BLUE}Testing: ${test_name}${NC}"
    
    # Run command and capture output
    local baseline_name=$(basename $baseline_file)
    local current_name="${baseline_name/baseline_/current_}"
    local tmp_output="$TMP_DIR/$current_name"
    eval "$command > $tmp_output 2>&1 || true"
    
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
    if diff -q "$baseline_file" "$tmp_output" > /dev/null 2>&1; then
        echo -e "${GREEN}✅ PASS - No changes detected${NC}"
        
        # Append PASS status to markdown
        cat >> "$diff_file" << EOF
✅ PASS - No changes detected
\`\`\`

---

## ✅ Result

**No regressions detected.** Server output matches baseline perfectly.

EOF
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
        local additions=$(diff -u "$baseline_file" "$tmp_output" 2>/dev/null | grep -c "^+" || echo "0")
        local deletions=$(diff -u "$baseline_file" "$tmp_output" 2>/dev/null | grep -c "^-" || echo "0")
        
        cat >> "$diff_file" << EOF
- **Added lines:** ${additions}
- **Removed lines:** ${deletions}

---

## 🔄 Changes

EOF
        
        # Generate unified diff and convert to markdown
        diff -u "$baseline_file" "$tmp_output" 2>&1 | while IFS= read -r line; do
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
- **Current:** \`$(realpath "$tmp_output")\`
- **This report:** \`$(realpath "$diff_file")\`

EOF
        
        # Show summary in terminal
        echo ""
        echo -e "${YELLOW}📝 Markdown diff generated:${NC}"
        echo -e "   ${BLUE}$diff_file${NC}"
        echo -e "${YELLOW}   Open in VSCode to view formatted diff${NC}"
        
        return 1
    fi
    echo ""
}

# Run tests
FAILED=0
PASSED=0

echo "Test 1/4: Server without config"
if run_test "No Config" "timeout 2s ./bin/webserv" "$BASELINE_DIR/baseline_no_config.txt" "1"; then
    PASSED=$((PASSED + 1))
else
    FAILED=$((FAILED + 1))
fi
echo ""

echo "Test 2/4: Server with default.conf"
if run_test "default.conf" "timeout 2s ./bin/webserv ./conf/default.conf" "$BASELINE_DIR/baseline_default_conf.txt" "2"; then
    PASSED=$((PASSED + 1))
else
    FAILED=$((FAILED + 1))
fi
echo ""

echo "Test 3/4: Server with webserv.conf"
if run_test "webserv.conf" "timeout 2s ./bin/webserv ./conf/webserv.conf" "$BASELINE_DIR/baseline_webserv_conf.txt" "3"; then
    PASSED=$((PASSED + 1))
else
    FAILED=$((FAILED + 1))
fi
echo ""

echo "Test 4/4: Server with webserv_py_cgi.conf"
if run_test "webserv_py_cgi.conf" "timeout 2s ./bin/webserv ./conf/webserv_py_cgi.conf" "$BASELINE_DIR/baseline_webserv_py_cgi_conf.txt" "4"; then
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
    echo -e "${YELLOW}   timeout 2s ./bin/webserv > tests/qa_benchmark/baseline/baseline_no_config.txt 2>&1 || true${NC}"
    echo -e "${YELLOW}   (Repeat for other configs in baseline/ directory)${NC}"
    exit 1
fi
