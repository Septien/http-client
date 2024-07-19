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
    char *action = "POST";
    char *resource = "/some/url/for/testing";
    char *version = "HTTP/1.1";

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

    char *name = "host";
    char *value = "localhost:4221";

    set_header(request, name, value);

    char str[200];
    memset(str, 0, 200);
    unsigned long idx = hash((unsigned char *)name);
    bool passed = check_condition(true, strncmp(request->header[idx].key.key, name, strlen(name)) == 0, "Has header name", str);
    passed = check_condition(passed, strncmp(request->header[idx].value.value, value, strlen(value)) == 0, "Has header value", str);

    if (!passed) printf("%s\n", str);
    return passed;
}

bool test_set_header_double(void *arg) {
    struct data *req = (struct data *)arg;
    struct http_request *request = &req->request;

    char *name = "Host";
    char *value = "localhost:4221";
    char *name2 = "host";
    char *value2 = "localhost:4222";

    set_header(request, name, value);

    set_header(request, name2, value2);

    char str[200];
    unsigned long idx = hash((unsigned char *)name2);
    bool passed = check_condition(true, strncmp(request->header[idx].key.key, name2, strlen(name2)) == 0, "Header not modified", str);
    passed = check_condition(passed, strncmp(request->header[idx].value.value, value2, strlen(value2)) == 0, "Value not modified", str);

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

    cunit_execute_tests(tests);

    cunit_terminate(&tests);
}
