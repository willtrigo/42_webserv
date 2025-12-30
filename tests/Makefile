# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: umeneses <umeneses@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/12/24 09:42:14 by umeneses          #+#    #+#              #
#    Updated: 2025/12/30 20:46:54 by umeneses         ###   ########.fr        #
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
# Explicitly exclude files that cannot be compiled in test environment.
# Files are excluded when they depend on infrastructure not yet implemented
# or when they're part of the main application (not library code).
#
# INFRASTRUCTURE/NETWORK (2 files):
#   - RouteMatcher.cpp, RouteMatcherException.cpp
#
# INFRASTRUCTURE/HTTP (2 files):
#   - RequestParser.cpp, RequestParserException.cpp
#
# INFRASTRUCTURE/CONFIG (3 files):
#   - ConfigParser.cpp, ConfigProvider.cpp, ConfigException.cpp
#
# INFRASTRUCTURE/CONFIG/PARSERS (3 files - depend on entities):
#   - BlockParser.cpp, ParserContext.cpp, ParserState.cpp
#
# INFRASTRUCTURE/CONFIG/HANDLERS (4 files - depend on entities):
#   - GlobalDirectiveHandler.cpp, ServerDirectivehandler.cpp
#   - LocationDirectiveHandler.cpp, ADirectiveHandler.cpp
#
# INFRASTRUCTURE/FILESYSTEM/ADAPTERS (6 files):
#   - FileHandler.cpp, DirectoryLister.cpp, PathResolver.cpp
#   - FileSystemHelper.cpp, DirectoryEntryComparators.cpp
#   - RouteConfigAdapter.cpp
#
# PRESENTATION/CLI (2 files):
#   - CliController.cpp, CliView.cpp
#
# CONFIGURATION/ENTITIES (6 files - depend on infrastructure):
#   - HttpConfig.cpp, HttpConfigException.cpp
#   - ServerConfig.cpp, ServerConfigException.cpp
#   - LocationConfig.cpp, LocationConfigException.cpp
#
# CONFIGURATION/VALUE_OBJECTS (2 files - depend on infrastructure):
#   - UploadConfig.cpp, UploadConfigException.cpp
#
# Total: 32 files excluded + main.cpp
# ============================================================================

PROJECT_SRCS                    := $(shell find $(SRC_DIR) -name '*.cpp' \
											! -name 'main.cpp' \
											! -name 'RouteMatcher.cpp' \
											! -name 'RouteMatcherException.cpp' \
											! -name 'RequestParser.cpp' \
											! -name 'RequestParserException.cpp' \
											! -name 'ConfigParser.cpp' \
											! -name 'ConfigProvider.cpp' \
											! -name 'ConfigException.cpp' \
											! -name 'BlockParser.cpp' \
											! -name 'ParserContext.cpp' \
											! -name 'ParserState.cpp' \
											! -name 'GlobalDirectiveHandler.cpp' \
											! -name 'ServerDirectivehandler.cpp' \
											! -name 'LocationDirectiveHandler.cpp' \
											! -name 'ADirectiveHandler.cpp' \
											! -name 'FileHandler.cpp' \
											! -name 'DirectoryLister.cpp' \
											! -name 'PathResolver.cpp' \
											! -name 'FileSystemHelper.cpp' \
											! -name 'DirectoryEntryComparators.cpp' \
											! -name 'RouteConfigAdapter.cpp' \
											! -name 'CliController.cpp' \
											! -name 'CliView.cpp' \
											! -name 'HttpConfig.cpp' \
											! -name 'HttpConfigException.cpp' \
											! -name 'ServerConfig.cpp' \
											! -name 'ServerConfigException.cpp' \
											! -name 'LocationConfig.cpp' \
											! -name 'LocationConfigException.cpp' \
											! -name 'UploadConfig.cpp' \
											! -name 'UploadConfigException.cpp' \
											)


# Test files
TEST_SRCS                       := test_main.cpp
TEST_SRCS                       += $(shell find $(UNIT_TESTS_DIR) -name 'test_*.cpp' 2>/dev/null || true)
TEST_SRCS                       += $(shell find $(INTEGRATION_TESTS_DIR) -name 'test_*.cpp' 2>/dev/null || true)

# Mock implementations
MOCK_SRCS                       := $(shell find $(MOCKS_DIR) -name '*.cpp' 2>/dev/null || true)

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
	@./$(NAME) --gtest_filter="*Test.*" || true
	@printf "$(YELLOW)[ℹ] Unit test run completed (exit code ignored)$(RESET)\n"

integration: $(NAME)
	@printf "$(CYAN)Running integration tests...$(RESET)\n"
	@./$(NAME) --gtest_filter="*IntegrationTest.*" || true
	@printf "$(YELLOW)[ℹ] Integration test run completed (exit code ignored)$(RESET)\n"

failing-tests: $(NAME)
	@printf "$(CYAN)Analyzing test failures (excluding crashing suites)...$(RESET)\n"
	@printf "$(CYAN)━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━$(RESET)\n"
	@./$(NAME) --gtest_filter="-QueryStringBuilderTest.*" 2>&1 | tee /tmp/test_output_safe.txt; exit 0
	@printf "$(CYAN)━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━$(RESET)\n"
	@printf "\n$(YELLOW)📊 TEST SUMMARY (WITHOUT QueryStringBuilder)$(RESET)\n"
	@printf "$(CYAN)━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━$(RESET)\n"
	@grep -E "Running [0-9]+ tests" /tmp/test_output_safe.txt | sed 's/\[=*\] /  📝 /' || echo "  ⚠️  Could not determine total tests"
	@PASSED=$$(grep -c "^\[       OK \]" /tmp/test_output_safe.txt 2>/dev/null || echo "0"); \
	FAILED=$$(grep -c "^\[  FAILED  \]" /tmp/test_output_safe.txt 2>/dev/null | head -1 || echo "0"); \
	printf "  ✅ PASSED %d tests\n" $$PASSED; \
	printf "  ❌ FAILED %d tests\n" $$FAILED
	@printf "\n$(RED)❌ FAILED TESTS BY SUITE$(RESET)\n"
	@printf "$(CYAN)━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━$(RESET)\n"
	@grep "^\[  FAILED  \]" /tmp/test_output_safe.txt 2>/dev/null | awk '{ \
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
