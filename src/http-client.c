#include <string.h>
#include <assert.h>
#include <ctype.h>
#include <stdio.h>

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

void set_header(struct http_request *request, char *name, char *value) {
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

void set_body(struct http_request *request, char *body, int len, char *type)
{
    int n = 0;
    if (len > 0) {
        n = (len < BODY_LEN ? len : BODY_LEN);
    } else {
        n = strlen(body);
        n = n < BODY_LEN ? n : BODY_LEN;
    }
    if (type != NULL && strlen(type) > 0) {
        int m = 0, _n = n;
        // Get the number of digits in the body's length
        while (_n > 0) { m++; _n /= 10; }
        char body_len[m];
        sprintf(body_len, "%d", n);
        set_header(request, "Content-Type", type);
        set_header(request, "Content-Length", body_len);
    }
    strncpy(request->body, body, n);
}
