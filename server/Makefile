NAME = uchat_server
LOG_FILE = server.log

OBJS_FOLDER = obj
INCS_FOLDER = inc
SRCS_FOLDER = src
API_SRCS_FOLDER = $(SRCS_FOLDER)/api
DATABASE_SRCS_FOLDER = $(SRCS_FOLDER)/database
INCS := $(wildcard $(INCS_FOLDER)/*.h)
SRCS := $(notdir $(wildcard $(SRCS_FOLDER)/*.c $(API_SRCS_FOLDER)/*.c $(DATABASE_SRCS_FOLDER)/*.c))
OBJS := $(patsubst %.c, $(OBJS_FOLDER)/%.o, $(SRCS))

CC = clang
CFLAGS = -std=c11 -Wall -Wextra -Werror -Wpedantic
LIBMX_FOLDER = ../libraries/libmx
LIBMX_NAME = $(LIBMX_FOLDER)/libmx.a
LIBCJSON_FOLDER = ../libraries/cJSON
LIBCJSON_NAME = $(LIBCJSON_FOLDER)/libcjson.a
OPENSSL_FOLDER = ../libraries/openssl
SQLITE3_FOLDER = ../libraries/sqlite3
LIB_FLAGS = -lpthread -lssl -lcrypto -lsqlite3 \
 			-L $(LIBMX_FOLDER) -lmx \
 			-L $(LIBCJSON_FOLDER) -lcjson

.PHONY: all install clean uninstall reinstall

all: install

install: $(NAME)

$(NAME): $(OBJS_FOLDER) $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LIB_FLAGS) -o ../$@

$(OBJS_FOLDER):
	mkdir $(OBJS_FOLDER)

$(OBJS_FOLDER)/%.o:$(SRCS_FOLDER)/%.c $(INCS)
	$(CC) $(CFLAGS) -I $(LIBMX_FOLDER)/$(INCS_FOLDER) \
					-I $(INCS_FOLDER) \
					-I $(LIBCJSON_FOLDER)/$(INCS_FOLDER) \
					-I $(SQLITE3_FOLDER)/$(INCS_FOLDER) \
					-I $(OPENSSL_FOLDER)/$(INCS_FOLDER) \
					-c $< -o $@

$(OBJS_FOLDER)/%.o:$(API_SRCS_FOLDER)/%.c $(INCS)
	$(CC) $(CFLAGS) -I $(LIBMX_FOLDER)/$(INCS_FOLDER) \
					-I $(INCS_FOLDER) \
					-I $(LIBCJSON_FOLDER)/$(INCS_FOLDER) \
					-I $(SQLITE3_FOLDER)/$(INCS_FOLDER) \
					-I $(OPENSSL_FOLDER)/$(INCS_FOLDER) \
					-c $< -o $@

$(OBJS_FOLDER)/%.o:$(DATABASE_SRCS_FOLDER)/%.c $(INCS)
	$(CC) $(CFLAGS) -I $(LIBMX_FOLDER)/$(INCS_FOLDER) \
					-I $(INCS_FOLDER) \
					-I $(LIBCJSON_FOLDER)/$(INCS_FOLDER) \
					-I $(SQLITE3_FOLDER)/$(INCS_FOLDER) \
					-I $(OPENSSL_FOLDER)/$(INCS_FOLDER) \
					-c $< -o $@

uninstall: clean
	rm -f ../$(NAME)

clean:
	rm -rf $(OBJS_FOLDER)
	rm -f $(LOG_FILE)

reinstall: uninstall all

