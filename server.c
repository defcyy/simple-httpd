#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <error.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "server.h"
#include "http_parser.h"
#include "http.h"
#include "common.h"
#include "file.h"

void error_exit(const char *message) {
	perror(message);
	exit(EXIT_FAILURE);
}

static int on_message_begin(http_parser *parser) {
	DEBUG_LOG("on_message_begain called\n");
	return 0;
}

static int on_url(http_parser *parser, const char *at, size_t length) {
	DEBUG_LOG("on_url called\n");
	request *req = (request *)parser->data;

	char *url = malloc(length + 1);
	memset(url, 0, length + 1);
	strncpy(url, at, length);
	set_request_url(req, url);
	
	free(url);
	return 0;
}

static int on_status(http_parser *parser, const char *at, size_t length) {
	DEBUG_LOG("on_status called\n");
	return 0;
}

static int on_header_field(http_parser *parser, const char *at, size_t length) {
	request *req = (request *)parser->data;
	req->_current_field = malloc(length + 1);
	memset(req->_current_field, 0, length + 1);
	memcpy(req->_current_field, at, length);

	return 0;
}

static int on_header_value(http_parser *parser, const char *at, size_t length) {
	request *req = (request *)parser->data;

	if (req->_current_field == NULL) {
		DEBUG_LOG("request header value without field");
		return 0;
	}

	char *value = malloc(length + 1);
	memset(value, 0, length + 1);
	memcpy(value, at, length);
	dict_set(req->headers, req->_current_field, value);

	free(req->_current_field);
	free(value);
	req->_current_field = NULL;

	return 0;
}

static int on_body(http_parser *parser, const char *at, size_t length) {
	DEBUG_LOG("on_body called\n");
	return 0;
}

static int on_headers_complete(http_parser *parser) {
	DEBUG_LOG("on_headers_complete called\n");
	return 0;
}

static int on_message_complete(http_parser *parser) {
	DEBUG_LOG("on_message_complete called\n");
	return 0;
}

static int on_chunk_header(http_parser *parser) {
	DEBUG_LOG("on_chunk_header called\n");
	return 0;
}

static int on_chunk_complete(http_parser *parser) {
	DEBUG_LOG("on_chunk_complete called\n");
	return 0;
}

int main(int argc, char const *argv[])
{

	int sock, conn;
    int addr_reuse = 1;
	struct sockaddr_in serv_addr, client_addr;

	for (int k = 1; k < argc; k++) {
		if (strcmp(argv[k], "-p") == 0 || strcmp(argv[k], "--port") == 0) {
			k++;
			PORT = atoi(argv[k]);
			DEBUG_LOG("get port from arg: %d\n", PORT);
		}

		if (strcmp(argv[k], "-r") == 0 || strcmp(argv[k], "--root") == 0) {
			k++;
			strcpy(ROOT, argv[k]);
			DEBUG_LOG("get web root from arg: %s\n", ROOT);
		} 
	}

	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1 ) {
		error_exit("socket init error");
	}

    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (const void*)&addr_reuse, sizeof(addr_reuse)) == -1) {
        error_exit("socket setsockopt error");
    }

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);
	serv_addr.sin_addr.s_addr = INADDR_ANY;

	if (bind(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1) {
		error_exit("socket bind error");
	}
	if (listen(sock, 10) == -1) {
		error_exit("socket listen error");
	}
	
	DEBUG_LOG("socket init successful\n");

	socklen_t addr_len = sizeof(struct sockaddr_in);

	char buffer[1024] = { 0 };
	size_t ret, nparsed;
	http_parser *parser;
	http_parser_settings parser_settings = { 0 };

	parser_settings.on_message_begin = on_message_begin;
	parser_settings.on_url = on_url;
	parser_settings.on_status = on_status;
	parser_settings.on_header_field = on_header_field;
	parser_settings.on_header_value = on_header_value;
	parser_settings.on_headers_complete = on_headers_complete;
	parser_settings.on_body = on_body;
	parser_settings.on_message_complete = on_message_complete;
	parser_settings.on_chunk_header = on_chunk_header;
	parser_settings.on_chunk_complete = on_chunk_complete;

	dict_t *headers;
	dict_entry *header;
	request *req;
	for (;;) {

		if ((conn = accept(sock, (struct sockaddr *)&client_addr, &addr_len)) == -1) {
			error_exit("socket accept error");
		}

		parser = malloc(sizeof(http_parser));
		ret = read(conn, buffer, sizeof(buffer));

		req = request_new();
		http_parser_init(parser, HTTP_REQUEST);
		parser->data = req;

		nparsed = http_parser_execute(parser, &parser_settings, buffer, ret);
		set_request_method(req, (const char *) http_method_str((enum http_method) parser->method));

		DEBUG_LOG("method: %s, url: %s\n", req->method, req->url);

		header = req->headers->data;
		while (header != NULL) {
			DEBUG_LOG("fied: %s, value: %s\n", (char *) header->key, (char *) header->value);
			header = header->next;
		}	

		file_serv(conn, req->url);

		request_release(req);
		free(parser);
	}

	free(parser);

	return EXIT_SUCCESS;
}
