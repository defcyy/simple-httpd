#include <stdio.h>

#define DEBUG 1
#define DEBUG_LOG(fmt, ...) do { if (DEBUG) fprintf(stdout, fmt, ##__VA_ARGS__); } while (0)
#define ERROR_LOG(fmt, ...) do { fprintf(stdout, fmt, ##__VA_ARGS__); } while (0)

#define PORT 8000
