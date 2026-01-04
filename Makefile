# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: umeneses <umeneses@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/12/12 17:14:28 by dande-je          #+#    #+#              #
#    Updated: 2026/01/04 01:16:02 by dande-je         ###   ########.fr        #
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

SRCS_DOMAIN_DIR                              := $(SRCS_DIR)domain/
SRCS_DOMAIN_CONFIGURATION_DIR                := $(SRCS_DOMAIN_DIR)configuration/
SRCS_DOMAIN_CONFIGURATION_ENTITIES_DIR       := $(SRCS_DOMAIN_CONFIGURATION_DIR)entities/
SRCS_DOMAIN_CONFIGURATION_EXCEPTIONS_DIR     := $(SRCS_DOMAIN_CONFIGURATION_DIR)exceptions/
SRCS_DOMAIN_CONFIGURATION_VALUE_OBJECTS_DIR  := $(SRCS_DOMAIN_CONFIGURATION_DIR)value_objects/
SRCS_DOMAIN_FILESYSTEM_DIR                   := $(SRCS_DOMAIN_DIR)filesystem/
SRCS_DOMAIN_FILESYSTEM_EXCEPTIONS_DIR        := $(SRCS_DOMAIN_FILESYSTEM_DIR)exceptions/
SRCS_DOMAIN_FILESYSTEM_SERVICES_DIR          := $(SRCS_DOMAIN_FILESYSTEM_DIR)services/
SRCS_DOMAIN_FILESYSTEM_VALUE_OBJECTS_DIR     := $(SRCS_DOMAIN_FILESYSTEM_DIR)value_objects/
SRCS_DOMAIN_HTTP_DIR                         := $(SRCS_DOMAIN_DIR)http/
SRCS_DOMAIN_HTTP_VALUE_OBJECTS_DIR           := $(SRCS_DOMAIN_HTTP_DIR)value_objects/
SRCS_DOMAIN_HTTP_EXCEPTIONS_DIR              := $(SRCS_DOMAIN_HTTP_DIR)exceptions/
SRCS_DOMAIN_SHARED_DIR                       := $(SRCS_DOMAIN_DIR)shared/
SRCS_DOMAIN_SHARED_EXCEPTION_DIR             := $(SRCS_DOMAIN_SHARED_DIR)exceptions/
SRCS_DOMAIN_SHARED_UTILS_DIR                 := $(SRCS_DOMAIN_SHARED_DIR)utils/
SRCS_DOMAIN_SHARED_VALUE_OBJECTS_DIR         := $(SRCS_DOMAIN_SHARED_DIR)value_objects/

SRCS_INFRASTRUCTURE_DIR                      := $(SRCS_DIR)infrastructure/
SRCS_INFRASTRUCTURE_CONFIG_DIR               := $(SRCS_INFRASTRUCTURE_DIR)config/
SRCS_INFRASTRUCTURE_CONFIG_ADAPTERS_DIR      := $(SRCS_INFRASTRUCTURE_CONFIG_DIR)adapters/
SRCS_INFRASTRUCTURE_CONFIG_EXCEPTIONS_DIR    := $(SRCS_INFRASTRUCTURE_CONFIG_DIR)exceptions/
SRCS_INFRASTRUCTURE_CONFIG_HANDLERS_DIR      := $(SRCS_INFRASTRUCTURE_CONFIG_DIR)handlers/
SRCS_INFRASTRUCTURE_CONFIG_LEXER_DIR         := $(SRCS_INFRASTRUCTURE_CONFIG_DIR)lexer/
SRCS_INFRASTRUCTURE_CONFIG_PARSERS_DIR       := $(SRCS_INFRASTRUCTURE_CONFIG_DIR)parsers/
SRCS_HTTP_DIR                   := $(SRCS_INFRASTRUCTURE_DIR)http/
SRCS_IO_DIR                     := $(SRCS_INFRASTRUCTURE_DIR)io/

SRCS_FILESYSTEM_DIR                         := $(SRCS_INFRASTRUCTURE_DIR)filesystem/
SRCS_FILESYSTEM_ADAPTERS_DIR                := $(SRCS_FILESYSTEM_DIR)adapters/
SRCS_FILESYSTEM_EXCEPTION_DIR               := $(SRCS_FILESYSTEM_DIR)exceptions/

SRCS_LOGGING_DIR                := $(SRCS_INFRASTRUCTURE_DIR)logging/
SRCS_NETWORK_DIR                := $(SRCS_INFRASTRUCTURE_DIR)network/

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
RM                              := rm -rf
MKDIR                           := mkdir -p
MAKEFLAGS                       += --no-print-directory
SLEEP                           := sleep 0.01

#******************************************************************************#
#                                   FILES                                      #
#******************************************************************************#

NAME_OUTPUT                     = webserv
NAME                            = $(BIN_DIR)$(NAME_OUTPUT)

# DOMAIN
SRCS_FILES                      += $(addprefix $(SRCS_DOMAIN_CONFIGURATION_ENTITIES_DIR), HttpConfig.cpp \
																	 LocationConfig.cpp \
																	 ServerConfig.cpp)
SRCS_FILES                      += $(addprefix $(SRCS_DOMAIN_CONFIGURATION_EXCEPTIONS_DIR),  CgiConfigException.cpp \
																	 ErrorPageException.cpp \
																	 HttpConfigException.cpp \
																	 ListenDirectiveException.cpp \
																	 LocationConfigException.cpp \
																	 RouteException.cpp \
																	 ServerConfigException.cpp \
																	 UploadConfigException.cpp)
SRCS_FILES                      += $(addprefix $(SRCS_DOMAIN_CONFIGURATION_VALUE_OBJECTS_DIR), CgiConfig.cpp \
																	 ErrorPage.cpp \
																	 ListenDirective.cpp \
																	 Route.cpp \
																	 UploadConfig.cpp)

SRCS_FILES                      += $(addprefix $(SRCS_DOMAIN_FILESYSTEM_EXCEPTIONS_DIR), PathException.cpp \
																	 PermissionException.cpp \
																	 SizeException.cpp \
																	 UploadAccessException.cpp)
SRCS_FILES                      += $(addprefix $(SRCS_DOMAIN_FILESYSTEM_VALUE_OBJECTS_DIR), Path.cpp \
																	 Permission.cpp \
																	 Size.cpp \
																	 UploadAccess.cpp)

SRCS_FILES                      += $(addprefix $(SRCS_DOMAIN_HTTP_EXCEPTIONS_DIR), HostException.cpp \
																	 HttpMethodException.cpp \
																	 PortException.cpp \
																	 QueryStringBuilderException.cpp \
																	 UriException.cpp)
SRCS_FILES                      += $(addprefix $(SRCS_DOMAIN_HTTP_VALUE_OBJECTS_DIR), Host.cpp \
																	 HttpMethod.cpp \
																	 Port.cpp \
																	 QueryStringBuilder.cpp \
																	 Uri.cpp)

SRCS_FILES                      += $(addprefix $(SRCS_DOMAIN_SHARED_EXCEPTION_DIR), ErrorCodeException.cpp \
																	 RegexPatternException.cpp)
SRCS_FILES                      += $(addprefix $(SRCS_DOMAIN_SHARED_UTILS_DIR), StringUtils.cpp)
SRCS_FILES                      += $(addprefix $(SRCS_DOMAIN_SHARED_VALUE_OBJECTS_DIR), ErrorCode.cpp \
																	 RegexPattern.cpp)

# INFRASTRUCTURE
SRCS_FILES                      += $(addprefix $(SRCS_INFRASTRUCTURE_CONFIG_ADAPTERS_DIR), ConfigProvider.cpp)
SRCS_FILES                      += $(addprefix $(SRCS_INFRASTRUCTURE_CONFIG_EXCEPTIONS_DIR), ConfigException.cpp \
																	 ParserException.cpp \
																	 SyntaxException.cpp \
																	 ValidationException.cpp)
SRCS_FILES                      += $(addprefix $(SRCS_INFRASTRUCTURE_CONFIG_HANDLERS_DIR), ADirectiveHandler.cpp \
																	 GlobalDirectiveHandler.cpp \
																	 LocationDirectiveHandler.cpp \
																	 ServerDirectiveHandler.cpp)
SRCS_FILES                      += $(addprefix $(SRCS_INFRASTRUCTURE_CONFIG_LEXER_DIR), ConfigLexer.cpp \
																	 Token.cpp)
SRCS_FILES                      += $(addprefix $(SRCS_INFRASTRUCTURE_CONFIG_PARSERS_DIR), BlockParser.cpp \
																	 ConfigParser.cpp \
																	 IncludeProcessor.cpp \
																	 ParserContext.cpp \
																	 ParserState.cpp)

SRCS_FILES                      += $(addprefix $(SRCS_FILESYSTEM_EXCEPTION_DIR), DirectoryListerException.cpp \
																	 FileHandlerException.cpp \
																	 FileSystemHelperException.cpp \
																	 PathResolverException.cpp)
SRCS_FILES                      += $(addprefix $(SRCS_FILESYSTEM_ADAPTERS_DIR), DirectoryEntryComparators.cpp \
																	 DirectoryLister.cpp \
																	 FileHandler.cpp \
																	 FileSystemHelper.cpp \
																	 PathResolver.cpp)

SRCS_FILES                      += $(addprefix $(SRCS_IO_DIR), FileWriter.cpp \
																	 StreamWriter.cpp)
SRCS_FILES                      += $(addprefix $(SRCS_LOGGING_DIR), Logger.cpp)

# PRESENTATION
SRCS_FILES                      += $(addprefix $(SRCS_CLI_DIR), CliController.cpp \
																	 CliView.cpp)

SRCS_FILES                      += $(addprefix $(SRCS_EXCEPTIONS_DIR), BaseException.cpp)
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
#                              DOCKER FUNCTIONS                                #
#******************************************************************************#

define get_container_name
	grep 'container_name:' docker-compose.yaml | head -n 1 | awk '{print $$2}'
endef

define get_image_name
	cat docker-compose.yaml | grep 'image:' | awk '{printf $$2}'
endef

#******************************************************************************#
#                              DOCKER PATHS                                    #
#******************************************************************************#

IMAGE = $(shell $(get_image_name))
CONTAINER = $(shell $(get_container_name))
COLORED_USER_ID = $(CYAN)$(shell id -u)$(YELLOW)
COLORED_GROUP_ID = $(CYAN)$(shell id -g)$(YELLOW)

#******************************************************************************#
#                              DOCKER TARGETS                                  #
#******************************************************************************#

.PHONY: docker-build
docker-build:
	docker compose up -d --build
	@printf "$(GREEN)Docker image $(IMAGE) built successfully!$(RESET)"

.PHONY: docker-go
docker-go: docker-build
	@printf "$(GREEN)Docker container $(CONTAINER) is running!$(RESET)"
	$(call prepare_googletest)
	$(call build_googletest)
	docker compose exec app /bin/bash

.PHONY: docker-stop
docker-stop:
	docker compose stop $(CONTAINER)
	@printf "$(YELLOW)Docker container $(CONTAINER) stopped!$(RESET)"

.PHONY: docker-down
docker-down:
	docker compose down
	@printf "$(YELLOW)Docker container $(CONTAINER) stopped and removed!$(RESET)"

.PHONY: docker-reload
docker-reload: docker-stop docker-build docker-go

.PHONY: docker-ps
docker-ps:
	docker ps -a
	@printf "$(YELLOW)Docker containers listed!$(RESET)"

.PHONY: docker-deep-clean
docker-deep-clean:
	docker builder prune -f
	@printf "$(RED)Docker builder cache cleaned!$(RESET)\n"

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

