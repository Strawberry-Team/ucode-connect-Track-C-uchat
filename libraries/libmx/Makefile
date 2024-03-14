OBJS_FOLDER = obj
INCS_FOLDER = inc
SRCS_FOLDER = src
NAME = libmx.a
SRCS := $(wildcard $(SRCS_FOLDER)/*.c)
INCS := $(wildcard $(INCS_FOLDER)/*.h)
OBJS := $(patsubst $(SRCS_FOLDER)/%.c, $(OBJS_FOLDER)/%.o, $(SRCS))
CC = clang
CFLAGS = -std=c11 -Wall -Wextra -Werror -Wpedantic

.PHONY: all install clean uninstall reinstall

all: install

install: $(NAME)

$(NAME): $(OBJS_FOLDER) $(OBJS)
	ar rsc $(NAME) $(OBJS)

$(OBJS_FOLDER):
	mkdir $(OBJS_FOLDER)

$(OBJS_FOLDER)/%.o:$(SRCS_FOLDER)/%.c $(INCS)
	$(CC) $(CFLAGS) -I $(INCS_FOLDER) -c $< -o $@

uninstall: clean
	rm -f $(NAME)

clean:
	rm -rf $(OBJS_FOLDER)

reinstall: uninstall all
