
CC = gcc

SOURCES = \
	server.c \
	http.c \
	common.c \
	http_parser.c \
	dict.c \
	file.c

server:
	$(CC) -std=c99 -g -o server $(SOURCES)

.PHONY: server