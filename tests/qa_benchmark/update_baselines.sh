#!/bin/bash

# Update Baseline Outputs Script
# Runs webserver with different configs, waits for ready state, sends SIGINT, captures output

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/../.." && pwd)"
BASELINE_DIR="$SCRIPT_DIR/baseline"

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Create baseline directory
mkdir -p "$BASELINE_DIR"

cd "$PROJECT_ROOT"

echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}📝 Updating Baseline Outputs${NC}"
echo -e "${BLUE}========================================${NC}"
echo ""

# Check if binary exists
if [ ! -f "./bin/webserv" ]; then
    echo -e "${RED}❌ Binary not found. Run 'make' first.${NC}"
    exit 1
fi

# Function to run webserver and capture output
run_and_capture() {
    local config_name="$1"
    local config_arg="$2"
    local baseline_file="$3"
    
    echo -e "${BLUE}Capturing: ${config_name}${NC}"
    
    local tmp_output=$(mktemp)
    local tmp_pid=$(mktemp)
    
    # Start webserver in background, redirect output to file
    if [ -z "$config_arg" ]; then
        ./bin/webserv > "$tmp_output" 2>&1 &
    else
        ./bin/webserv "$config_arg" > "$tmp_output" 2>&1 &
    fi
    
    local webserv_pid=$!
    echo $webserv_pid > "$tmp_pid"
    
    echo -e "   ${YELLOW}Started webserver (PID: $webserv_pid)${NC}"
    
    # Wait for ready message (max 5 seconds)
    local wait_count=0
    local max_wait=50  # 5 seconds (50 * 0.1s)
    local server_ready=false
    
    while [ $wait_count -lt $max_wait ]; do
        if grep -q "Starting event loop" "$tmp_output" 2>/dev/null; then
            echo -e "   ${GREEN}✓ Server ready${NC}"
            server_ready=true
            break
        fi
        
        # Check if process exited (could be due to error)
        if ! kill -0 $webserv_pid 2>/dev/null; then
            # Process exited - check if it's an error or successful exit
            if grep -q "ERROR" "$tmp_output" 2>/dev/null; then
                echo -e "   ${YELLOW}⚠ Server exited with error (captured for baseline)${NC}"
            else
                echo -e "   ${YELLOW}⚠ Server exited early${NC}"
            fi
            # Don't break - we still want to save the output as baseline
            break
        fi
        
        sleep 0.1
        wait_count=$((wait_count + 1))
    done
    
    if [ $wait_count -ge $max_wait ] && [ "$server_ready" = false ]; then
        echo -e "   ${YELLOW}⚠ Timeout waiting for ready message (output captured anyway)${NC}"
    fi
    
    # Send SIGINT to gracefully shutdown (if still running)
    if kill -0 $webserv_pid 2>/dev/null; then
        echo -e "   ${YELLOW}Sending SIGINT...${NC}"
        kill -INT $webserv_pid 2>/dev/null || true
        
        # Wait for process to exit (max 2 seconds)
        local exit_wait=0
        while [ $exit_wait -lt 20 ] && kill -0 $webserv_pid 2>/dev/null; do
            sleep 0.1
            exit_wait=$((exit_wait + 1))
        done
        
        # Force kill if still running
        if kill -0 $webserv_pid 2>/dev/null; then
            echo -e "   ${RED}Force killing...${NC}"
            kill -9 $webserv_pid 2>/dev/null || true
        fi
    fi
    
    # Copy output to baseline
    cp "$tmp_output" "$baseline_file"
    echo -e "   ${GREEN}✓ Baseline saved: $(basename $baseline_file)${NC}"
    
    rm -f "$tmp_output" "$tmp_pid"
    echo ""
    
    return 0
}

# Update baselines
echo "1/4: No config"
run_and_capture "No Config" "" "$BASELINE_DIR/baseline_no_config.txt"

echo "2/4: default.conf"
run_and_capture "default.conf" "./conf/default.conf" "$BASELINE_DIR/baseline_default_conf.txt"

echo "3/4: webserv.conf"
run_and_capture "webserv.conf" "./conf/webserv.conf" "$BASELINE_DIR/baseline_webserv_conf.txt"

echo "4/4: webserv_combined_cgi.conf"
run_and_capture "webserv_combined_cgi.conf" "./conf/webserv_combined_cgi.conf" "$BASELINE_DIR/baseline_webserv_combined_cgi_conf.txt"

# Summary
echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}✅ All Baselines Updated${NC}"
echo -e "${BLUE}========================================${NC}"
echo ""
echo "Updated files:"
echo "  - baseline_no_config.txt"
echo "  - baseline_default_conf.txt"
echo "  - baseline_webserv_conf.txt"
echo "  - baseline_webserv_combined_cgi_conf.txt"
echo ""
echo -e "${GREEN}Done! Baselines are now up to date.${NC}"
