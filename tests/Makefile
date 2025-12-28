# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: umeneses <umeneses@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/12/24 09:42:14 by umeneses          #+#    #+#              #
#    Updated: 2025/12/27 22:43:53 by umeneses         ###   ########.fr        #
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

# Source files from main project (exclude main.cpp and unimplemented features)
# Exclude files with unimplemented dependencies:
# - Uri.cpp has unimplemented functions
# - Route.cpp, RouteMatcher.cpp, RouteConfigAdapter.cpp depend on unimplemented features
# - ConfigParser.cpp, ConfigProvider.cpp have interface mismatches (parsePath/parseFile)
# - ServerConfig.cpp, LocationConfig.cpp, ErrorPage.cpp, UploadConfig.cpp, HttpConfig.cpp depend on ConfigParser
# - CliController.cpp, ListenDirective.cpp depend on config classes
# - CgiConfig.cpp uses RegexPattern which has empty pattern initialization issues
# - DirectoryLister.cpp has static RegexPattern members that initialize with empty patterns
# - DirectoryEntryComparators.cpp has member access issues (m_size vs size)
# - RequestParser.cpp temporarily excluded while refactoring
# - FileSystemHelper.cpp and PathResolver.cpp have namespace issues from refactoring
PROJECT_SRCS                    := $(shell find $(SRC_DIR) -name '*.cpp' \
                                      ! -name 'main.cpp' \
                                      ! -name 'Uri.cpp' \
                                      ! -name 'Route.cpp' \
                                      ! -name 'RouteMatcher.cpp' \
                                      ! -name 'RouteConfigAdapter.cpp' \
                                      ! -name 'ConfigParser.cpp' \
                                      ! -name 'ConfigProvider.cpp' \
                                      ! -name 'ServerConfig.cpp' \
                                      ! -name 'LocationConfig.cpp' \
                                      ! -name 'ErrorPage.cpp' \
                                      ! -name 'UploadConfig.cpp' \
                                      ! -name 'HttpConfig.cpp' \
                                      ! -name 'CliController.cpp' \
                                      ! -name 'ListenDirective.cpp' \
                                      ! -name 'CgiConfig.cpp' \
                                      ! -name 'DirectoryLister.cpp' \
                                      ! -name 'DirectoryEntryComparators.cpp' \
                                      ! -name 'RequestParser.cpp' \
                                      ! -name 'FileSystemHelper.cpp' \
                                      ! -name 'PathResolver.cpp' \
                                      ! -name 'FileHandler.cpp')

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
	@./$(NAME)

unit: $(NAME)
	@printf "$(CYAN)Running unit tests...$(RESET)\n"
	@./$(NAME) --gtest_filter="*Test.*"

integration: $(NAME)
	@printf "$(CYAN)Running integration tests...$(RESET)\n"
	@./$(NAME) --gtest_filter="*IntegrationTest.*"

valgrind: $(NAME)
	@printf "$(CYAN)Running tests with valgrind...$(RESET)\n"
	@valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose --log-file=valgrind-output.txt ./$(NAME)
	@printf "$(GREEN)[✓] Valgrind report saved to valgrind-output.txt$(RESET)\n"

valgrind-brief: $(NAME)
	@printf "$(CYAN)Running tests with valgrind (brief)...$(RESET)\n"
	@valgrind --leak-check=full --show-leak-kinds=all ./$(NAME)

clean:
	@rm -rf $(BUILD_DIR)
	@printf "$(RED)[✓] Test objects cleaned$(RESET)\n"

fclean: clean
	@rm -rf $(BIN_DIR)
	@printf "$(RED)[✓] Test binary cleaned$(RESET)\n"

re: fclean all

.PHONY: all test unit integration valgrind valgrind-brief clean fclean re

-include $(DEPS)
