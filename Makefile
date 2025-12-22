# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/12/12 17:14:28 by dande-je          #+#    #+#              #
#    Updated: 2025/12/22 20:11:33 by dande-je         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

#******************************************************************************#
#                                   COLOR                                      #
#******************************************************************************#

RED                             := \033[0;31m
GREEN                           := \033[0;32m
YELLOW                          := \033[0;33m
PURPLE                          := \033[0;35m
CYAN                            := \033[0;36m
RESET                           := \033[0m

#******************************************************************************#
#                                   PATH                                       #
#******************************************************************************#

SRCS_DIR                        := src/

SRCS_APPLICATION_DIR            := $(SRCS_DIR)application/
SRCS_USE_CASES_DIR              := $(SRCS_APPLICATION_DIR)use_cases/
SRCS_SERVICES_DIR               := $(SRCS_APPLICATION_DIR)services/

SRCS_DOMAIN_DIR                 := $(SRCS_DIR)domain/
SRCS_ENTITIES_DIR               := $(SRCS_DOMAIN_DIR)entities/
SRCS_VALUE_OBJECTS_DIR          := $(SRCS_DOMAIN_DIR)value_objects/

SRCS_INFRASTRUCTURE_DIR         := $(SRCS_DIR)infrastructure/
SRCS_ADAPTER_DIR                := $(SRCS_INFRASTRUCTURE_DIR)adapters/
SRCS_CONFIG_DIR                 := $(SRCS_INFRASTRUCTURE_DIR)config/
SRCS_IO_DIR                     := $(SRCS_INFRASTRUCTURE_DIR)io/
SRCS_FILESYSTEM_DIR             := $(SRCS_INFRASTRUCTURE_DIR)filesystem/
SRCS_LOGGING_DIR                := $(SRCS_INFRASTRUCTURE_DIR)logging/

SRCS_PRESENTATION_DIR           := $(SRCS_DIR)presentation/
SRCS_CLI_DIR                    := $(SRCS_PRESENTATION_DIR)cli/

SRCS_SHARED_DIR                 := $(SRCS_DIR)shared/
SRCS_EXCEPTIONS_DIR             := $(SRCS_SHARED_DIR)exceptions/
SRCS_UTILS_DIR                  := $(SRCS_SHARED_DIR)utils/

INCS                            := src/
BIN_DIR                         := bin/
BUILD_DIR                       := build/

#******************************************************************************#
#                                  COMMANDS                                    #
#******************************************************************************#
arser                          := rm -rf
MKDIR                           := mkdir -p
MAKEFLAGS                       += --no-print-directory
SLEEP                           := sleep 0.01

#******************************************************************************#
#                                   FILES                                      #
#******************************************************************************#

NAME_OUTPUT                     = webserv
NAME                            = $(BIN_DIR)$(NAME_OUTPUT)

SRCS_FILES                      += $(addprefix $(SRCS_ENTITIES_DIR), CgiConfig.cpp)
SRCS_FILES                      += $(addprefix $(SRCS_VALUE_OBJECTS_DIR), ErrorCode.cpp \
																	 HttpMethod.cpp \
																	 Path.cpp \
																	 Permission.cpp \
																	 Port.cpp \
																	 RegexPattern.cpp \
																	 Size.cpp \
																	 Uri.cpp)

SRCS_FILES                      += $(addprefix $(SRCS_ADAPTER_DIR), ConfigProvider.cpp)
SRCS_FILES                      += $(addprefix $(SRCS_CONFIG_DIR), ConfigParser.cpp)
SRCS_FILES                      += $(addprefix $(SRCS_FILESYSTEM_DIR), FileSystemHelper.cpp \
																	 PathResolver.cpp)
SRCS_FILES                      += $(addprefix $(SRCS_IO_DIR), FileWriter.cpp \
																	 StreamWriter.cpp)
SRCS_FILES                      += $(addprefix $(SRCS_LOGGING_DIR), Logger.cpp)

SRCS_FILES                      += $(addprefix $(SRCS_CLI_DIR), CliController.cpp \
																	 CliView.cpp)

SRCS_FILES                      += $(addprefix $(SRCS_EXCEPTIONS_DIR), BaseException.cpp \
																	 CgiConfigException.cpp \
																	 ConfigException.cpp \
																	 ErrorCodeException.cpp \
																	 FileSystemHelperException.cpp \
																	 HttpMethodException.cpp \
																	 PathException.cpp \
																	 PathResolverException.cpp \
																	 PermissionException.cpp \
																	 PortException.cpp \
																	 RegexPatternException.cpp \
																	 SizeException.cpp \
																	 UriException.cpp)
SRCS_FILES                      += $(addprefix $(SRCS_UTILS_DIR), TerminalColor.cpp)

SRCS_FILES                      += $(addprefix $(SRCS_DIR), main.cpp)

OBJS                            += $(SRCS_FILES:%.cpp=$(BUILD_DIR)%.o)

DEPS                            += $(OBJS:.o=.d)

#******************************************************************************#
#                               OUTPUTS MESSAGES                               #
#******************************************************************************#

COUNT                           = 0
OBJS_COUNT                      = 0
MATH                            = 0
CLEAN_MESSAGE                   := $(NAME_OUTPUT) objects deleted
FCLEAN_MESSAGE                  := $(NAME_OUTPUT) deleted
EXE_MESSAGE                     = $(RESET)[100%%] $(GREEN)Built target $(NAME_OUTPUT)
COMP_MESSAGE                    = Building CPP object

#******************************************************************************#
#                               COMPILATION                                    #
#******************************************************************************#

CC                             := c++
CFLAGS                         = -Wall -Wextra -Werror -Wshadow -O3 -ffast-math -Wunreachable-code -Wpedantic -std=c++98
CPPFLAGS                       := $(addprefix -I,$(INCS)) -MMD -MP
DFLAGS                         := -Wall -Wextra -Werror -g3 -std=c++98
LFLAGS                         := -march=native
COMPILE_OBJS                   = $(CC) $(CFLAGS) $(LFLAGS) $(CPPFLAGS) -c $< -o $@
COMPILE_EXE                    = $(CC) $(CFLAGS) $(OBJS) -o $(NAME)

#******************************************************************************#
#                                   DEFINE                                     #
#******************************************************************************#

ifdef WITH_DEBUG
	CFLAGS                     = $(DFLAGS)
endif

#******************************************************************************#
#                                  FUNCTION                                    #
#******************************************************************************#

define create_dir
	$(MKDIR) $(dir $@)
	$(MKDIR) log
endef

define comp_objs
	$(eval COUNT=$(shell expr $(COUNT) + 1))
	$(COMPILE_OBJS)
	$(eval MATH=$(shell expr "$(COUNT)" \* 100 \/ "$(OBJS_COUNT)"))
	$(eval MATH=$(shell if [ $(COUNT) -lt 1 ]; then echo $(shell expr "$(MATH)" + 100) ; else echo $(MATH) ; fi))
	printf "[%3d%%] $(YELLOW)$(COMP_MESSAGE) $@ \r$(RESET)\n" $$(echo $(MATH))
endef

define comp_exe
	$(MKDIR) $(BIN_DIR)
	$(COMPILE_EXE)
	$(SLEEP)
	printf "$(EXE_MESSAGE)\n$(RESET)"
endef

define clean
	$(RM) $(BUILD_DIR)
	$(SLEEP)
	printf "$(RED)$(CLEAN_MESSAGE)$(RESET)\n"
endef

define fclean
	$(RM) $(BIN_DIR)
	$(SLEEP)
	printf "$(RED)$(FCLEAN_MESSAGE)$(RESET)\n"
endef

define debug
	$(call clean)
	$(call fclean)
	$(MAKE) WITH_DEBUG=TRUE
endef

define reset_count
	$(eval COUNT=$(1))
	$(eval OBJS_COUNT=$(words $(SRCS_FILES)))
endef

#******************************************************************************#
#                                   TARGETS                                    #
#******************************************************************************#

all: $(NAME)

$(BUILD_DIR)%.o: %.cpp
	$(call create_dir)
	$(call comp_objs)

$(NAME): $(call reset_count, -$(words $(OBJS))) $(OBJS)
	$(call comp_exe)

clean:
	$(call clean)

fclean: clean
	$(call fclean)

re: fclean all

debug:
	$(call debug)

.PHONY: all clean fclean re debug
.DEFAULT_GOAL := all
.SILENT:

