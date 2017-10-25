#include <stdio.h>

#define DEBUG 1
#define DEBUG_LOG(fmt, ...) do { if (DEBUG) fprintf(stdout, fmt, ##__VA_ARGS__); } while (0)
#define ERROR_LOG(fmt, ...) do { fprintf(stdout, fmt, ##__VA_ARGS__); } while (0)

#define SERVER_NAME "Simple Http Server"

#define HTTP_HEADER_DATE_FORMAT "%a, %d %b %Y %X %Z"

static int PORT = 8000;
static char ROOT[256] = "/root/tmp";
