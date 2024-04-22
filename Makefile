SERVER_NAME = uchat_server
SERVER_FOLDER = server
CLIENT_NAME = uchat
CLIENT_FOLDER = client

LIBMX_FOLDER = libraries/libmx
LIBMX_NAME = $(LIBMX_FOLDER)/libmx.a
LIBCJSON_FOLDER = libraries/cJSON
LIBCJSON_NAME = $(LIBCJSON_FOLDER)/libcjson.a

.PHONY: all install clean uninstall reinstall $(SERVER_NAME) $(CLIENT_NAME)

all: install

install: $(LIBMX_NAME) $(LIBCJSON_NAME) $(SERVER_NAME) $(CLIENT_NAME)

$(LIBMX_NAME):
	make -sC $(LIBMX_FOLDER)

$(LIBCJSON_NAME):
	make -sC $(LIBCJSON_FOLDER)

$(SERVER_NAME):
	make -sC $(SERVER_FOLDER)

$(CLIENT_NAME):
	make -sC $(CLIENT_FOLDER)

uninstall: clean
	make -sC $(LIBMX_FOLDER) $@
	make -sC $(LIBCJSON_FOLDER) $@
	make -sC $(CLIENT_FOLDER) $@
	make -sC $(SERVER_FOLDER) $@

clean:
	make -sC $(LIBMX_FOLDER) $@
	make -sC $(LIBCJSON_FOLDER) $@
	make -sC $(CLIENT_FOLDER) $@
	make -sC $(SERVER_FOLDER) $@

reinstall: uninstall all

