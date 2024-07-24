/* Testing the client functions */

#include <stdio.h>

#include "cUnit.h"
#include "http-client.h"

struct data {
    struct http_request request;
    struct http_response response;
};

void setup(void *arg) {
    struct data *req = (struct data *)arg;

    clear_http_request(&req->request);
    clear_http_response(&req->response);
}

void teardown(void *arg) {
    (void) arg;
}

/* HTTP request */

bool test_clear_http_request(void *arg) {
    struct data *req = (struct data *)arg;
    struct http_request *request = &req->request;

    clear_http_request(request);
    bool passed = true;
    unsigned int i;
    for (i = 0; i < sizeof(request->action); i++) {
        passed = passed & (request->action[i] == 0);
    }
    for (i = 0; i < sizeof(request->resource); i++) {
        passed = passed & (request->resource[i] == 0);
    }
    for (i = 0; i < sizeof(request->version); i++) {
        passed = passed & (request->version[i] == 0);
    }
    for (i = 0; i < sizeof(request->body); i++) {
        passed = passed & (request->body[i] == 0);
    }

    return passed;
}

bool test_set_request_method(void *arg) {
    struct data *req = (struct data *)arg;
    struct http_request *request = &req->request;
    char *action = "POST\0";
    char *resource = "/some/url/for/testing\0";
    char *version = "HTTP/1.1\0";

    set_request_method(request, action, resource, version);

    char str[200];
    memset(str, 0, 200);
    bool passed = check_condition(true, strncmp(request->action, action, 4) == 0, "Action is copied", str);
    passed = check_condition(true, strncmp(request->resource, resource, strlen(resource)) == 0, "Resource is copied", str);
    passed = check_condition(true, strncmp(request->version, version, strlen(version)) == 0, "Version is copied", str);

    if (!passed) printf("%s\n", str);

    return passed;
}

bool test_set_header(void *arg) {
    struct data *req = (struct data *)arg;
    struct http_request *request = &req->request;

    char *name = "User-Agent\0";
    char *value = "foobar\0";

    set_header(request, name, value);

    char str[200];
    memset(str, 0, 200);
    unsigned long idx = hash((unsigned char *)"user-agent\0");
    bool passed = check_condition(true, strncmp(request->header[idx].key.key, name, strlen(name)) == 0, "Has header name", str);
    passed = check_condition(passed, strncmp(request->header[idx].value.value, value, strlen(value)) == 0, "Has header value", str);

    if (!passed) printf("%s\n", str);
    return passed;
}

bool test_set_header_double(void *arg) {
    struct data *req = (struct data *)arg;
    struct http_request *request = &req->request;

    char *name = "Host\0";
    char *value = "localhost:4221\0";
    char *name2 = "host\0";
    char *value2 = "localhost:4222\0";

    char str[200];
    unsigned long idx = hash((unsigned char *)name2);
    bool passed = check_condition(true, strlen(request->header[idx].key.key) == 0, "The header has length zero", str);
    set_header(request, name, value);
    passed = check_condition(passed, strlen(request->header[idx].key.key) > 0, "The header was added", str);

    set_header(request, name2, value2);

    passed = check_condition(passed, strncmp(request->header[idx].key.key, name2, strlen(name2)) == 0, "Header not modified", str);
    passed = check_condition(passed, strncmp(request->header[idx].value.value, value2, strlen(value2)) == 0, "Value not modified", str);

    if (!passed) printf("%s\n", str);

    return passed;
}

bool test_set_body(void *arg) {
    struct data *req = (struct data *)arg;
    struct http_request *request = &req->request;

    char *body = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nullam rutrum posuere dolor eget feugiat. Morbi rhoncus sollicitudin eleifend. Curabitur enim felis, vulputate sed volutpat a, efficitur non magna. Suspendisse iaculis nunc in eros ullamcorper dictum. Vestibulum vitae auctor est. Lorem ipsum dolor sit amet, consectetur adipiscing elit. Phasellus diam lectus, volutpat ac auctor auctor, luctus vel lectus. Sed in pulvinar massa. Mauris et consectetur erat. Cras dapibus nisl turpis, eget blandit nibh aliquam id. Donec in finibus diam. Quisque eu augue efficitur, condimentum quam vitae, ultricies orci.\0";
    int len = strlen(body);
    set_body(request, body, 0, NULL);

    char str[200];
    bool passed = check_condition(true, strncmp(request->body, body, len) == 0, "Body is correctly copied", str);

    if (!passed) printf("%s\n", str);
    return passed;
}

bool test_set_body_and_add_header(void *arg) {
    struct data *req = (struct data *)arg;
    struct http_request *request = &req->request;

    char *body = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nullam rutrum posuere dolor eget feugiat. Morbi rhoncus sollicitudin eleifend. Curabitur enim felis, vulputate sed volutpat a, efficitur non magna. Suspendisse iaculis nunc in eros ullamcorper dictum. Vestibulum vitae auctor est. Lorem ipsum dolor sit amet, consectetur adipiscing elit. Phasellus diam lectus, volutpat ac auctor auctor, luctus vel lectus. Sed in pulvinar massa. Mauris et consectetur erat. Cras dapibus nisl turpis, eget blandit nibh aliquam id. Donec in finibus diam. Quisque eu augue efficitur, condimentum quam vitae, ultricies orci.\0";
    char *type = "text/plain";
    int len = strlen(body);
    set_body(request, body, len, type);

    char *header1 = "content-type\0";
    char *header2 = "content-length\0";
    long unsigned idx = hash((unsigned char *)header1);
    long unsigned idx2 = hash((unsigned char *)header2);
    char str[200];
    char body_len[10];
    sprintf(body_len, "%d", len);
    bool passed = check_condition(true, strncmp(request->body, body, len) == 0, "Body is correctly copied", str);
    passed = check_condition(passed, strncmp(request->header[idx].key.key, "Content-Type", 12) == 0, "Set Content-Type value", str);
    passed = check_condition(passed, strncmp(request->header[idx].value.value, type, 10) == 0, "Set Content-Type value", str);
    passed = check_condition(passed, strncmp(request->header[idx2].key.key, "Content-Length", 12) == 0, "Set Content-Length header name", str);
    passed = check_condition(passed, strncmp(request->header[idx2].value.value, body_len, strlen(body_len)) == 0, "Set Content-Length value", str);

    if (!passed) printf("%s\n", str);
    return passed;
}

bool test_create_request_str(void *arg) {
    struct data *req = (struct data *) arg;
    struct http_request *request = &req->request;

    set_request_method(request, "GET\0", "/resource/example\0", "HTTP/1.1\0");
    set_header(request, "User-Agent\0", "foobar/1.2.3\0");
    set_header(request, "Host\0", "localhost:4221\0");
    set_header(request, "Accept\0", "text/plain, text/json\0");

    char *body = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nullam rutrum posuere dolor eget feugiat. Morbi rhoncus sollicitudin eleifend. Curabitur enim felis, vulputate sed volutpat a, efficitur non magna. Suspendisse iaculis nunc in eros ullamcorper dictum. Vestibulum vitae auctor est. Lorem ipsum dolor sit amet, consectetur adipiscing elit. Phasellus diam lectus, volutpat ac auctor auctor, luctus vel lectus. Sed in pulvinar massa. Mauris et consectetur erat. Cras dapibus nisl turpis, eget blandit nibh aliquam id. Donec in finibus diam. Quisque eu augue efficitur, condimentum quam vitae, ultricies orci.\0";
    char *type = "text/plain";
    int len = strlen(body);
    set_body(request, body, len, type);

    char str[REQUEST_LEN];
    char *str_p = str;
    memset(str, 0, REQUEST_LEN);
    create_request_str(request, &str_p);

    char *str2 = "GET /resource/example HTTP/1.1\r\nUser-Agent: foobar/1.2.3\r\nHost: localhost:4221\r\nContent-Length: 610\r\nContent-Type: text/plain\r\nAccept: text/plain, text/json\r\n\r\nLorem ipsum dolor sit amet, consectetur adipiscing elit. Nullam rutrum posuere dolor eget feugiat. Morbi rhoncus sollicitudin eleifend. Curabitur enim felis, vulputate sed volutpat a, efficitur non magna. Suspendisse iaculis nunc in eros ullamcorper dictum. Vestibulum vitae auctor est. Lorem ipsum dolor sit amet, consectetur adipiscing elit. Phasellus diam lectus, volutpat ac auctor auctor, luctus vel lectus. Sed in pulvinar massa. Mauris et consectetur erat. Cras dapibus nisl turpis, eget blandit nibh aliquam id. Donec in finibus diam. Quisque eu augue efficitur, condimentum quam vitae, ultricies orci.\0";
    char strm[200];
    memset(strm, 0, 200);
    bool passed = check_condition(true, strncmp(str, str2, strlen(str)) == 0, "Request is properly formed", strm);

    return passed;
}

bool test_create_request_null_body(void *arg) {
    struct data *req = (struct data *) arg;
    struct http_request *request = &req->request;

    set_request_method(request, "GET\0", "/resource/example\0", "HTTP/1.1\0");
    set_header(request, "User-Agent\0", "foobar/1.2.3\0");
    set_header(request, "Host\0", "localhost:4221\0");
    set_header(request, "Accept\0", "text/plain, text/json\0");

    char str[REQUEST_LEN];
    char *str_p = str;
    memset(str, 0, REQUEST_LEN);
    create_request_str(request, &str_p);

    char *str2 = "GET /resource/example HTTP/1.1\r\nUser-Agent: foobar/1.2.3\r\nHost: localhost:4221\r\nAccept: text/plain, text/json\r\n\r\n\0";
    char strm[200];
    memset(strm, 0, 200);
    bool passed = check_condition(true, strncmp(str, str2, strlen(str)) == 0, "Request is properly formed", strm);

    if (!passed) printf("%s\n", strm);

    return passed;
}

/* HTTP response */

bool test_clear_http_response(void *arg) {
    struct data *req = (struct data *)arg;
    struct http_response *response = &req->response;

    clear_http_response(response);

    bool passed = true;
    unsigned int i = 0;
    for (i = 0; i < sizeof(response->version); i++) {
        passed = passed && (response->version[i] == 0);
    }
    for (i = 0; i < sizeof(response->status_code); i++) {
        passed = passed && (response->status_code[i] == 0);
    }
    for (i = 0; i < sizeof(response->status_text); i++) {
        passed = passed && (response->status_text[i] == 0);
    }
    for (i = 0; i < sizeof(response->body); i++) {
        passed = passed && (response->body[i] == 0);
    }

    return passed;
}

bool test_parse_response_str_w_body(void *arg) {
    struct data *req = (struct data *)arg;
    struct http_response *response = &req->response;

    char response_str[756] = "HTTP/1.1 200 OK\r\n\0";
    strcat(response_str, "User-Agent: foobar/1.2.3\r\n\0");
    strcat(response_str, "Host: localhost:4221\r\n\0");
    strcat(response_str, "Content-Length: 610\r\n\0");
    strcat(response_str, "Content-Type: text/plain\r\n\0");
    strcat(response_str, "Accept: text/plain, text/json\r\n\r\n\0");
    char *body = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nullam rutrum posuere dolor eget feugiat. Morbi rhoncus sollicitudin eleifend. Curabitur enim felis, vulputate sed volutpat a, efficitur non magna. Suspendisse iaculis nunc in eros ullamcorper dictum. Vestibulum vitae auctor est. Lorem ipsum dolor sit amet, consectetur adipiscing elit. Phasellus diam lectus, volutpat ac auctor auctor, luctus vel lectus. Sed in pulvinar massa. Mauris et consectetur erat. Cras dapibus nisl turpis, eget blandit nibh aliquam id. Donec in finibus diam. Quisque eu augue efficitur, condimentum quam vitae, ultricies orci.\0";
    strcat(response_str, body);

    char *ptr = response_str;
    parse_response_str(response, ptr);

    char str[512];
    memset(str, 0, 512);
    bool passed = check_condition(true, strncmp(response->version, "HTTP/1.1", 8) == 0, "Protocol version is 1.1", str);
    passed = check_condition(passed, strncmp(response->status_code, "200", 3) == 0, "Status code is correct", str);
    passed = check_condition(passed, strncmp(response->status_text, "OK", 2) == 0, "Status text is correct", str);
    unsigned long idx = hash((unsigned char *)"user-agent\0");
    passed = check_condition(passed, strncmp(response->headers[idx].key.key, "user-agent\0", 10) == 0, "User-Agent header exists", str);
    passed = check_condition(passed, strncmp(response->headers[idx].value.value, "foobar/1.2.3\0", 12) == 0, "User-Agent header value exists", str);
    idx = hash((unsigned char *)"host");
    passed = check_condition(passed, strncmp(response->headers[idx].key.key, "host", 0) == 0, "Host header exists", str);
    passed = check_condition(passed, strncmp(response->headers[idx].value.value, "localhost:4221", 12) == 0, "Host header value exists", str);
    idx = hash((unsigned char *)"content-length");
    passed = check_condition(passed, strncmp(response->headers[idx].key.key, "content-length", 14) == 0, "Content-Length header exists", str);
    passed = check_condition(passed, strncmp(response->headers[idx].value.value, "610", 3) == 0, "Content-Length header value exists", str);
    idx = hash((unsigned char *)"content-type");
    passed = check_condition(passed, strncmp(response->headers[idx].key.key, "content-type", 12) == 0, "Content-Type header exists", str);
    passed = check_condition(passed, strncmp(response->headers[idx].value.value, "text/plain", 10) == 0, "Content-Type header value exists", str);
    idx = hash((unsigned char *)"accept");
    passed = check_condition(passed, strncmp(response->headers[idx].key.key, "accept", 12) == 0, "Accept header exists", str);
    passed = check_condition(passed, strncmp(response->headers[idx].value.value, "text/plain, text/json", 10) == 0, "Accept header value exists", str);
    passed = check_condition(passed, strncmp(response->body, body, 610) == 0, "Body is present", str);

    if (!passed) printf("%s\n", str);

    return passed;
}

bool test_parser_response_wo_body(void *arg)
{
    struct data *req = (struct data *)arg;
    struct http_response *response = &req->response;

    char response_str[756] = "HTTP/1.1 200 OK\r\n\0";
    strcat(response_str, "User-Agent: foobar/1.2.3\r\n\0");
    strcat(response_str, "Host: localhost:4221\r\n\0");
    strcat(response_str, "Accept: text/plain, text/json\r\n\r\n\0");

    char *ptr = response_str;
    parse_response_str(response, ptr);

    char str[512];
    memset(str, 0, 512);
    bool passed = check_condition(true, strncmp(response->version, "HTTP/1.1", 8) == 0, "Protocol version is 1.1", str);
    passed = check_condition(passed, strncmp(response->status_code, "200", 3) == 0, "Status code is correct", str);
    passed = check_condition(passed, strncmp(response->status_text, "OK", 2) == 0, "Status text is correct", str);
    unsigned long idx = hash((unsigned char *)"user-agent\0");
    passed = check_condition(passed, strncmp(response->headers[idx].key.key, "user-agent\0", 10) == 0, "User-Agent header exists", str);
    passed = check_condition(passed, strncmp(response->headers[idx].value.value, "foobar/1.2.3\0", 12) == 0, "User-Agent header value exists", str);
    idx = hash((unsigned char *)"host");
    passed = check_condition(passed, strncmp(response->headers[idx].key.key, "host", 0) == 0, "Host header exists", str);
    passed = check_condition(passed, strncmp(response->headers[idx].value.value, "localhost:4221", 12) == 0, "Host header value exists", str);
    idx = hash((unsigned char *)"accept");
    passed = check_condition(passed, strncmp(response->headers[idx].key.key, "accept", 12) == 0, "Accept header exists", str);
    passed = check_condition(passed, strncmp(response->headers[idx].value.value, "text/plain, text/json", 10) == 0, "Accept header value exists", str);

    if (!passed) printf("%s\n", str);

    return passed;
}

bool test_get_response_line(void *arg)
{
    struct data *req = (struct data *)arg;
    struct http_response *response = &req->response;

    char response_str[756] = "HTTP/1.1 404 Not Found\r\n\0";
    strcat(response_str, "User-Agent: foobar/1.2.3\r\n\0");
    strcat(response_str, "Host: localhost:4221\r\n\0");
    strcat(response_str, "Accept: text/plain, text/json\r\n\r\n\0");

    char *ptr = response_str;
    parse_response_str(response, ptr);

    char version[VERSION_LEN];
    char status_code[STATUS_LEN];
    char status_text[ST_TXT_LEN];
    char *vptr = version, *scptr = status_code, *stptr = status_text;

    get_response_line(response, vptr, scptr, stptr);

    char str[256];
    bool passed = check_condition(true, strncmp(version, "HTTP/1.1", VERSION_LEN) == 0, "Correct version", str);
    passed = check_condition(passed, strncmp(status_code, "404", STATUS_LEN) == 0, "Correct status code", str);
    passed = check_condition(passed, strncmp(status_text, "Not Found", ST_TXT_LEN) == 0, "Correct status text", str);

    if (!passed) printf("%s\n", str);

    return passed;
}

bool test_get_header(void *arg)
{
    struct data *req = (struct data *)arg;
    struct http_response *response = &req->response;

    char response_str[756] = "HTTP/1.1 404 Not Found\r\n\0";
    strcat(response_str, "User-Agent: foobar/1.2.3\r\n\0");
    strcat(response_str, "Host: localhost:4221\r\n\0");
    strcat(response_str, "Accept: text/plain, text/json\r\n\r\n\0");
    char *ptr = response_str;
    parse_response_str(response, ptr);

    char *header = "User-Agent\0";
    char value[HEADERS_LEN];
    memset(value, 0, HEADERS_LEN);
    int ret = get_header(response, header, value);
    char str[256];
    memset(str, 0, 256);
    bool passed = check_condition(true, ret == 0, "Return value is zero", str);
    passed = check_condition(passed, strncmp(value, "foobar/1.2.3", HEADERS_LEN) == 0, "User-Agent value is correct", str);
    memset(value, 0, HEADERS_LEN);
    ret = get_header(response, "Host", value);
    passed = check_condition(passed, ret == 0, "Return value is zero for host", str);
    passed = check_condition(passed, strncmp(value, "localhost:4221", HEADERS_LEN) == 0, "Host value is valid", str);
    memset(value, 0, HEADERS_LEN);
    ret = get_header(response, "Accept", value);
    passed = check_condition(passed, ret == 0, "Return value is zero for accept", str);
    passed = check_condition(passed, strncmp(value, "text/plain, text/json", HEADERS_LEN) == 0, "Accept value is valid", str);
    // Get a header not received in the response
    memset(value, 0, HEADERS_LEN);
    ret = get_header(response, "Content-Type", value);
    passed = check_condition(passed, ret == -1, "Return value is -1 for header not present in response", str);
    passed = check_condition(passed, strlen(value) == 0, "value array should be empty", str);

    if (!passed) printf("%s\n", str);

    return passed;
}

bool test_get_body(void *arg)
{
    struct data *req = (struct data *)arg;
    struct http_response *response = &req->response;

    char response_str[756] = "HTTP/1.1 200 OK\r\n\0";
    strcat(response_str, "User-Agent: foobar/1.2.3\r\n\0");
    strcat(response_str, "Host: localhost:4221\r\n\0");
    strcat(response_str, "Content-Length: 610\r\n\0");
    strcat(response_str, "Content-Type: text/plain\r\n\0");
    strcat(response_str, "Accept: text/plain, text/json\r\n\r\n\0");
    char *body = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nullam rutrum posuere dolor eget feugiat. Morbi rhoncus sollicitudin eleifend. Curabitur enim felis, vulputate sed volutpat a, efficitur non magna. Suspendisse iaculis nunc in eros ullamcorper dictum. Vestibulum vitae auctor est. Lorem ipsum dolor sit amet, consectetur adipiscing elit. Phasellus diam lectus, volutpat ac auctor auctor, luctus vel lectus. Sed in pulvinar massa. Mauris et consectetur erat. Cras dapibus nisl turpis, eget blandit nibh aliquam id. Donec in finibus diam. Quisque eu augue efficitur, condimentum quam vitae, ultricies orci.\0";
    strcat(response_str, body);

    char *ptr = response_str;
    parse_response_str(response, ptr);

    // Get the body length
    char value[HEADERS_LEN];
    memset(value, 0, HEADERS_LEN);
    int ret = get_header(response, "Content-Length", value);
    unsigned long n = 0;
    sscanf(value, "%lu", &n);
    char str[256];
    memset(str, 0, 256);
    bool passed = check_condition(true, n == 610, "Size of body is correct", str);
    char return_body[n];
    ret = get_body(response, return_body);
    passed = check_condition(passed, (unsigned long)ret == n, "Ret is n", str);
    passed = check_condition(passed, strncmp(return_body, body, n) == 0, "Body is correctly returned", str);

    if (!passed) printf("%s\n", str);

    return passed;
}

bool test_get_body_empty(void *arg)
{
    struct data *req = (struct data *)arg;
    struct http_response *response = &req->response;

    char response_str[756] = "HTTP/1.1 200 OK\r\n\0";
    strcat(response_str, "User-Agent: foobar/1.2.3\r\n\0");
    strcat(response_str, "Host: localhost:4221\r\n\0");
    strcat(response_str, "Accept: text/plain, text/json\r\n\r\n\0");

    char *ptr = response_str;
    parse_response_str(response, ptr);

    char return_body[BODY_LEN];
    memset(return_body, 0, BODY_LEN);
    int ret = get_body(response, return_body);
    char str[256];
    memset(str, 0, 256);
    bool passed = check_condition(true, ret == -1, "Ret is -1", str);
    passed = check_condition(passed, strlen(return_body) == 0, "Body is correctly returned", str);

    if (!passed) printf("%s\n", str);

    return passed;
}

void http_client_tests(void)
{
    cUnit_t *tests;
    struct data test_data;

    cunit_init(&tests, &setup, &teardown, (void *)&test_data);

    cunit_add_test(tests, &test_clear_http_request, "init_http_request");
    cunit_add_test(tests, &test_set_request_method, "set_request_method");
    cunit_add_test(tests, &test_set_header, "set_header");
    cunit_add_test(tests, &test_set_header_double, "set_header, double header");
    cunit_add_test(tests, &test_set_body, "set_body");
    cunit_add_test(tests, &test_set_body_and_add_header, "set_body_and_add_header");
    cunit_add_test(tests, &test_create_request_str, "create_request_str");
    cunit_add_test(tests, &test_create_request_null_body, "create_request_str w/null body");

    /* Http response */
    cunit_add_test(tests, &test_clear_http_response, "clear_http_response");
    cunit_add_test(tests, &test_parse_response_str_w_body, "paser_response_str w/body");
    cunit_add_test(tests, &test_parser_response_wo_body, "parse_response wo/body");
    cunit_add_test(tests, &test_get_response_line, "get_response_line");
    cunit_add_test(tests, &test_get_header, "get_header");
    cunit_add_test(tests, &test_get_body, "get_body");
    cunit_add_test(tests, &test_get_body_empty, "get_body with empty body");

    cunit_execute_tests(tests);

    cunit_terminate(&tests);
}
