# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: umeneses <umeneses@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/12/24 09:42:14 by umeneses          #+#    #+#              #
#    Updated: 2026/01/10 15:40:45 by umeneses         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

#******************************************************************************#
#                                   COLOR                                      #
#******************************************************************************#

RED                             := \033[0;31m
GREEN                           := \033[0;32m
YELLOW                          := \033[0;33m
CYAN                            := \033[0;36m
RESET                           := \033[0m

#******************************************************************************#
#                                   PATH                                       #
#******************************************************************************#

ROOT_DIR                        := ..
SRC_DIR                         := $(ROOT_DIR)/src
TESTS_DIR                       := .
BUILD_DIR                       := build
BIN_DIR                         := bin

UNIT_TESTS_DIR                  := unit
INTEGRATION_TESTS_DIR           := integration
MOCKS_DIR                       := mocks

#******************************************************************************#
#                                   FILES                                      #
#******************************************************************************#

NAME                            := $(BIN_DIR)/test_runner

# ============================================================================
# SOURCE FILE EXCLUSIONS
# ============================================================================
# Files excluded:
#  1. main.cpp - tests have test_main.cpp
# ============================================================================

# Get all project source files
PROJECT_SRCS_ALL                := $(shell find $(SRC_DIR) -name '*.cpp')

# Exclude files that conflict with test environment
EXCLUDED_FILES                  := $(SRC_DIR)/main.cpp

# Filter out excluded files
PROJECT_SRCS                    := $(filter-out $(EXCLUDED_FILES),$(PROJECT_SRCS_ALL))


# Test files
TEST_SRCS                       := test_main.cpp
TEST_SRCS                       += $(shell find $(UNIT_TESTS_DIR) -name 'test_*.cpp' 2>/dev/null || true)
TEST_SRCS                       += $(shell find $(INTEGRATION_TESTS_DIR) -name 'test_*.cpp' 2>/dev/null || true)

# Mock implementations - exclude MockIncludeProcessor since real implementation is used
MOCK_SRCS_ALL                   := $(shell find $(MOCKS_DIR) -name '*.cpp' 2>/dev/null || true)
MOCK_SRCS                       := $(filter-out $(MOCKS_DIR)/MockIncludeProcessor.cpp,$(MOCK_SRCS_ALL))

ALL_SRCS                        := $(PROJECT_SRCS) $(TEST_SRCS) $(MOCK_SRCS)
OBJS                            := $(ALL_SRCS:%.cpp=$(BUILD_DIR)/%.o)
DEPS                            := $(OBJS:.o=.d)

#******************************************************************************#
#                               COMPILATION                                    #
#******************************************************************************#

CXX                             := c++
CXXFLAGS                        := -Wall -Wextra -Werror -std=c++98 -g3
CPPFLAGS                        := -I$(ROOT_DIR) -I$(SRC_DIR) -I$(TESTS_DIR) -MMD -MP
LDFLAGS                         := -lgtest -lgtest_main -lpthread

#******************************************************************************#
#                                  TARGETS                                     #
#******************************************************************************#

all: $(NAME)

$(NAME): $(OBJS)
	@mkdir -p $(BIN_DIR)
	@$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME) $(LDFLAGS)
	@printf "$(GREEN)[✓] Tests compiled successfully$(RESET)\n"

$(BUILD_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	@$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@
	@printf "$(YELLOW)[•] Compiling: $<$(RESET)\n"

$(BUILD_DIR)/$(SRC_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	@$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@
	@printf "$(YELLOW)[•] Compiling: $<$(RESET)\n"

test: $(NAME)
	@printf "$(CYAN)Running tests...$(RESET)\n"
	@./$(NAME) || true
	@printf "$(YELLOW)[ℹ] Test run completed (exit code ignored)$(RESET)\n"

unit: $(NAME)
	@printf "$(CYAN)Running unit tests...$(RESET)\n"
	@./$(NAME) --gtest_filter="*Test.*:-*IntegrationTest.*" || true
	@printf "$(YELLOW)[ℹ] Unit test run completed (exit code ignored)$(RESET)\n"

integration: $(NAME)
	@printf "$(CYAN)Running integration tests...$(RESET)\n"
	@./$(NAME) --gtest_filter="*IntegrationTest.*" 2>&1 | tee /tmp/integration_test_output.txt || true
	@TOTAL=$$(grep -oP 'Running \K[0-9]+' /tmp/integration_test_output.txt | head -1 2>/dev/null || echo "0"); \
	FAILED=$$(grep '^\[  FAILED  \]' /tmp/integration_test_output.txt | grep -oP '\K[0-9]+(?= tests)' | head -1 2>/dev/null || echo "0"); \
	if [ -z "$$FAILED" ]; then FAILED=0; fi; \
	if [ "$$TOTAL" -gt 0 ] && [ "$$TOTAL" -eq "$$FAILED" ]; then \
		printf "$(RED)━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━$(RESET)\n"; \
		printf "$(RED)100%% of the Integration Tests FAILED! Are you running the webserver binary?$(RESET)\n"; \
		printf "$(RED)━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━$(RESET)\n"; \
	else \
		printf "$(YELLOW)[ℹ] Integration test run completed (exit code ignored)$(RESET)\n"; \
	fi; \
	rm -f /tmp/integration_test_output.txt

failing-tests: $(NAME)
	@printf "$(CYAN)Analyzing test failures (excluding crashing suites)...$(RESET)\n"
	@printf "$(CYAN)━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━$(RESET)\n"
	@./$(NAME) --gtest_filter="-QueryStringBuilderTest.*" 2>&1 | tee /tmp/test_output_safe.txt; exit 0
	@printf "$(CYAN)━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━$(RESET)\n"
	@printf "\n$(YELLOW)📊 TEST SUMMARY (WITHOUT QueryStringBuilder)$(RESET)\n"
	@printf "$(CYAN)━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━$(RESET)\n"
	@grep -E "Running [0-9]+ tests" /tmp/test_output_safe.txt | sed 's/\[=*\] /  📝 /' || echo "  ⚠️  Could not determine total tests"
	@PASSED=$$(grep -c "^\[       OK \]" /tmp/test_output_safe.txt 2>/dev/null || echo "0"); \
	FAILED=$$(grep "^\[  FAILED  \]" /tmp/test_output_safe.txt | grep -c "([0-9]* ms)" 2>/dev/null || echo "0"); \
	printf "  ✅ PASSED %d tests\n" $$PASSED; \
	printf "  ❌ FAILED %d tests\n" $$FAILED
	@printf "\n$(RED)❌ FAILED TESTS BY SUITE$(RESET)\n"
	@printf "$(CYAN)━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━$(RESET)\n"
	@grep "^\[  FAILED  \]" /tmp/test_output_safe.txt | grep "([0-9]* ms)" 2>/dev/null | awk '{ \
		gsub(/\[  FAILED  \] /, ""); \
		gsub(/\(.*\)$$/, ""); \
		gsub(/^ +| +$$/, ""); \
		split($$0, parts, "."); \
		suite = parts[1]; \
		test = parts[2]; \
		if (suite != "" && test != "") { \
			if (!(suite in counts)) { \
				suites[++suite_count] = suite; \
			} \
			counts[suite]++; \
			tests[suite, counts[suite]] = test; \
		} \
	} END { \
		if (suite_count == 0) { \
			print "  ✅ No test failures detected"; \
		} else { \
			for (i = 1; i <= suite_count; i++) { \
				suite = suites[i]; \
				printf "\n  • %s: %d failed\n", suite, counts[suite]; \
				for (j = 1; j <= counts[suite] && j <= 5; j++) { \
					printf "    - %s\n", tests[suite, j]; \
				} \
				if (counts[suite] > 5) { \
					printf "    ... and %d more\n", counts[suite] - 5; \
				} \
			} \
		} \
	}' || echo "  ✅ No test failures detected"
	@printf "$(CYAN)━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━$(RESET)\n"
	@printf "\n$(YELLOW)⚠️  Note: QueryStringBuilder tests excluded due to segfault$(RESET)\n"
	@rm -f /tmp/test_output_safe.txt

val-verb: $(NAME)
	@printf "$(CYAN)Running tests with valgrind...$(RESET)\n"
	@valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose --log-file=valgrind-output.txt ./$(NAME)
	@printf "$(GREEN)[✓] Valgrind report saved to valgrind-output.txt$(RESET)\n"

val: $(NAME)
	@printf "$(CYAN)Running tests with valgrind (brief)...$(RESET)\n"
	@valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./$(NAME)

clean:
	@rm -rf $(BUILD_DIR)
	@printf "$(RED)[✓] Test objects cleaned$(RESET)\n"

fclean: clean
	@rm -rf $(BIN_DIR)
	@printf "$(RED)[✓] Test binary cleaned$(RESET)\n"

re: fclean all

.PHONY: all test unit integration failing-tests val val-verb clean fclean re

-include $(DEPS)
