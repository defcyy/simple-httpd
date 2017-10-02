#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include "server.h"
#include "http.h"

request *request_new(void) {
	request *req = malloc(sizeof(request));
	req->headers = dict_new();
	req->_current_field = NULL;
	req->url = NULL;
	req->method = NULL;
	req->body = NULL;

	return req;
}

void request_release(request *req) {
	dict_release(req->headers);
	if (req->_current_field != NULL) {
		free(req->_current_field);
	}
	if (req->url !=NULL) {
		free(req->url);
	}
	if (req->method != NULL) {
		free(req->method);
	}
	if (req->body != NULL) {
		free(req->body);
	}
	free(req);
}

response *response_new(void) {
	response *resp = malloc(sizeof(response));

	resp->status = 0;
	resp->headers = dict_new();
	resp->content = NULL;
	resp->length = 0;

	return resp;
}

void response_release(response *resp) {
	dict_release(resp->headers);
	if (resp->content != NULL) {
		free(resp->content);
	}
	free(resp);
}

void set_response_status(response *resp, unsigned int status) {
	resp->status = status;
}

void set_response_header(response *resp, char *key, void *value) {
	dict_set(resp->headers, key, value);
}

void set_response_content(response *resp, void *content, size_t length) {
	resp->content = malloc(length);
	memcpy(resp->content, content, length);
	resp->length = length;

	char *len = malloc(sizeof(length) + 1);
	sprintf(len, "%u", (unsigned int)length);
	set_response_header(resp, "Content-Length", len);
}

const char *get_status_description(unsigned int status) {
    switch (status) {
    case 100:
        return "Continue";
    case 101:
        return "Switching Protocols";
    case 102:
        return "Processing";
    case 200:
        return "OK";
    case 201:
        return "Created";
    case 202:
        return "Accepted";
    case 203:
        return "Non-Authoritative Information";
    case 204:
        return "No Content";
    case 205:
        return "Reset Content";
    case 206:
        return "Partial Content";
    case 207:
        return "Multi-Status";
    case 208:
        return "Already Reported";
    case 226:
        return "IM Used";
    case 300:
        return "Multiple Choices";
    case 301:
        return "Moved Permanently";
    case 302:
        return "Found";
    case 303:
        return "See Other";
    case 304:
        return "Not Modified";
    case 305:
        return "Use Proxy";
    case 307:
        return "Temporary Redirect";
    case 308:
        return "Permanent Redirect";
    case 400:
        return "Bad Request";
    case 401:
        return "Unauthorized";
    case 402:
        return "Payment Required";
    case 403:
        return "Forbidden";
    case 404:
        return "Not Found";
    case 405:
        return "Method Not Allowed";
    case 406:
        return "Not Acceptable";
    case 407:
        return "Proxy Authentication Required";
    case 408:
        return "Request Timeout";
    case 409:
        return "Conflict";
    case 410:
        return "Gone";
    case 411:
        return "Length Required";
    case 412:
        return "Precondition Failed";
    case 413:
        return "Payload Too Large";
    case 414:
        return "URI Too Long";
    case 415:
        return "Unsupported Media Type";
    case 416:
        return "Range Not Satisfiable";
    case 417:
        return "Expectation Failed";
    case 421:
        return "Misdirected Request";
    case 422:
        return "Unprocessable Entity";
    case 423:
        return "Locked";
    case 424:
        return "Failed Dependency";
    case 426:
        return "Upgrade Required";
    case 428:
        return "Precondition Required";
    case 429:
        return "Too Many Requests";
    case 431:
        return "Request Header Fields Too Large";
    case 451:
        return "Unavailable For Legal Reasons";
    case 500:
        return "Internal Server Error";
    case 501:
        return "Not Implemented";
    case 502:
        return "Bad Gateway";
    case 503:
        return "Service Unavailable";
    case 504:
        return "Gateway Timeout";
    case 505:
        return "HTTP Version Not Supported";
    case 506:
        return "Variant Also Negotiates";
    case 507:
        return "Insufficient Storage";
    case 508:
        return "Loop Detected";
    case 510:
        return "Not Extended";
    case 511:
        return "Network Authentication Required";
    }

    return "Unknown";
}

void send_response(int conn, response *resp) {
	char *data;
	unsigned int header_length;
	int ret, total_length, line_length = 0;

	const char *status_desc = get_status_description(resp->status);
	line_length = strlen(HTTP_PROTOCOL) + 5 + strlen(status_desc) + strlen(HTTP_RESPONSE_SEPERATOR);
	data = malloc(line_length + 1);
	sprintf(data, "%s %u %s%s", HTTP_PROTOCOL, resp->status, status_desc, HTTP_RESPONSE_SEPERATOR);

	total_length = line_length;

	dict_entry *header = resp->headers->data;
	while (header != NULL) {
		line_length = strlen(header->key) + strlen((char *)header->value) + 2 + strlen(HTTP_RESPONSE_SEPERATOR);
		data = (char *) realloc(data,  total_length + line_length + 1);
		sprintf(data + total_length, "%s: %s%s", (char *)header->key, (char *)header->value, HTTP_RESPONSE_SEPERATOR);
		total_length += line_length;
		header = header->next;
	}

	data = (char *) realloc(data, total_length + strlen(HTTP_RESPONSE_SEPERATOR));
	sprintf(data + total_length, "%s", HTTP_RESPONSE_SEPERATOR);
	total_length += strlen(HTTP_RESPONSE_SEPERATOR);
	ret = send(conn, data, strlen(data), 0);
	DEBUG_LOG("send headers ret: %d\n", ret);
	DEBUG_LOG("response header: \n%s", data);
	ret = send(conn, resp->content, resp->length, 0);
	DEBUG_LOG("send body ret: %d\n", ret);
	DEBUG_LOG("response content: \n%s\n", (char *) resp->content);

	free(data);
}