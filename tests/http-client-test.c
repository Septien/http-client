/* Testing the client functions */

#include <stdio.h>

#include "cUnit.h"
#include "http-client.h"

struct data {
    struct http_request request;
};

void setup(void *arg) {
    struct data *req = (struct data *)arg;

    clear_http_request(&req->request);
}

void teardown(void *arg) {
    (void) arg;
}

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

    cunit_execute_tests(tests);

    cunit_terminate(&tests);
}
