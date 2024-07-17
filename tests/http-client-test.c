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

void http_client_tests(void)
{
    cUnit_t *tests;
    struct data test_data;

    cunit_init(&tests, &setup, &teardown, (void *)&test_data);

    cunit_add_test(tests, &test_clear_http_request, "init_http_request");
    cunit_add_test(tests, &test_set_request_method, "set_request_method");

    cunit_execute_tests(tests);

    cunit_terminate(&tests);
}
