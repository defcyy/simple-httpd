#include <stddef.h>
#include "dict.h"

#define HTTP_PROTOCOL "HTTP/1.1"
#define HTTP_RESPONSE_SEPERATOR "\r\n"
#define DEFAULT_INDEX_FILE "index.html"

static dict_t *mime_types;

typedef struct request {

	//private
	char *_current_field;

	char *url;
	char *method;
	char *body;
	dict_t *headers;

} request;

typedef struct response {

	unsigned int status;
	dict_t *headers;
	void *content;
	size_t length;

} response;

request *request_new(void);
void set_request_url(request *req, const char *url);
void set_request_method(request *req, const char *method);
void request_release(request *req);

response *response_new(void);
void response_release(response *resp);
void set_response_status(response *resp, unsigned int status);
void set_response_header(response *resp, char *key, void *value);
void set_response_content(response *resp, void *content, size_t length);
void send_response(int conn, response *resp);

void mime_types_init(void);
void mime_types_release(void);