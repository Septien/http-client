/* Testing the client functions */

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

void http_client_tests(void)
{
    cUnit_t *tests;
    struct data test_data;

    cunit_init(&tests, &setup, &teardown, (void *)&test_data);

    cunit_add_test(tests, &test_clear_http_request, "init_http_request");

    cunit_execute_tests(tests);

    cunit_terminate(&tests);
}
