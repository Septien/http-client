#include "http-client.h"
#include <string.h>
#include <assert.h>

unsigned long hash(unsigned char *str) {
    unsigned long hash = 5381;
    int c;

    while ((c = *str++))
        hash = c + (hash << 6) + (hash << 16) - hash;

    return hash;
}

void clear_http_request(struct http_request *request) {
    memset(request, 0, sizeof(struct http_request));
}

void set_request_method(struct http_request *request, char *action, char *resource, char *version) {
    assert(request != NULL);

    strncpy(request->action, action, strlen(action));
    strncpy(request->resource, resource, strlen(resource));
    strncpy(request->version, version, strlen(version));
}