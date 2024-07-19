#include <string.h>
#include <assert.h>
#include <ctype.h>

#include "http-client.h"

unsigned long hash(unsigned char *str) {
    unsigned long hash = 5381;
    int c;

    while ((c = *str++))
        hash = c + (hash << 6) + (hash << 16) - hash;

    return hash % HEADERS;
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

void set_header(struct http_request *request, char *name, char* value) {
    assert(request != NULL);
    /* Convert to lower case. */
    int n = strlen(name);
    char lower[n+1];
    memset(lower, 0, n);
    for (int i = 0; i < n; i++) {
        lower[i] = tolower(name[i]);
    }
    lower[n] = '\0';
    unsigned long idx = hash((unsigned char *)lower);
    /* Copy the key name */
    strncpy(request->header[idx].key.key, name, strlen(name));
    /* Copy the value name */
    strncpy(request->header[idx].value.value, value, strlen(value));
}
