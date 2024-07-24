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

/*---------------------- HTTP request ----------------------*/

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

void create_request_str(struct http_request *request, char **str)
{
    assert(str != NULL);
    // Print the request line
    sprintf(*str, "%s %s %s\r\n", request->action, request->resource, request->version);
    int i = 0;
    while (i < HEADERS) {
        if (strlen(request->header[i].key.key) > 0) {
            int len = strlen(request->header[i].key.key) + strlen(request->header[i].value.value);
            char header[len];
            memset(header, 0, len);
            sprintf(header, "%s: %s\r\n", request->header[i].key.key, request->header[i].value.value);
            strcat(*str, header);
        }
        i++;
    }
    strcat(*str, "\r\n");
    strcat(*str, request->body);
}

/*---------------------- HTTP response ----------------------*/

void clear_http_response(struct http_response *response)
{
    memset(response, 0, sizeof(struct http_response));
}

/*
* Parse a string in search of del delimiter, beginning at start. 
* Returns the substring's length.
*/
unsigned long parse_string(char *str, char del, unsigned long start)
{
    if (start > strlen(str)) {
        return -1;
    }
    unsigned long i = start;
    while (str[i++] != del) {}

    return i;
}

/*
* Verify whether the following four characters are \r\n\r\n
* indicating the headers section's end.
*/
int headers_end(char *str, unsigned long i)
{
    return (str[i] == '\r') && (str[i + 1] == '\n') && (str[i + 2] == '\r') && (str[i + 3] == '\n');
}

unsigned long copy_string(char *dest, char *src, char del, unsigned long start)
{
    unsigned long len = 0;
    len = parse_string(src, del, start);
    strncpy(dest, &src[start], len - start - 1);
    dest[len] = '\0';
    return len;
}

void parse_response_str(struct http_response *response, char *str)
{
    // Get the status line's information
    unsigned long len = 0;
    len = copy_string(response->version, str, ' ', len);
    len = copy_string(response->status_code, str, ' ', len);
    len = copy_string(response->status_text, str, '\r', len);

    unsigned long body_len = 0;
    // Parse the headers
    while (!headers_end(str, len - 1)) {
        /* The headers follow the format:
        *               HEADER_NAME: HEADER_VALUE\r\n
        *  We parse the string until the character ':' is found to obtain
        *  the header name. Then skip the whitespace and obtain the 
        *  header's value. We manually skip \r\n for not storing them
        *  in the header's value.
        */

        len++;     // Advance to characters to remove \r\n
        // Search for the header name
        int start = len;
        len = parse_string(str, ':', len);
        char header[HEADERS_LEN];
        memset(header, 0, HEADERS_LEN);
        int n = len - start - 1;
        // Store header in lower case, for ease of search
        for (int i = 0; i < n; i++) {
            header[i] = tolower(str[start + i]);
        }
        header[n] = '\0';
        unsigned long idx = hash((unsigned char *)header);
        strncpy(response->headers[idx].key.key, header, len - start - 1);

        // Search for header's value
        start = len + 1; // Remove space
        len = parse_string(str, '\r', start);
        memset(header, 0, HEADERS_LEN);
        strncpy(header, &str[start], len - start - 1);
        strncpy(response->headers[idx].value.value, header, len - start - 1);

        // Check for body presence
        if (strcmp(response->headers[idx].key.key, "content-length") == 0) {
            sscanf(response->headers[idx].value.value, "%lu", &body_len);
        }
    }

    if (body_len > 0) {
        int body_start = strlen(str) - body_len;
        strncpy(response->body, &str[body_start], body_len);
    }
}

void get_response_line(struct http_response *response, char *version, char *code, char *status)
{
    strncpy(version, response->version, VERSION_LEN);
    strncpy(code, response->status_code, STATUS_LEN);
    strncpy(status, response->status_text, strlen(response->status_text));
}
