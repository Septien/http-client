#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "http-client.h"
#include "http-client-test.h"

int main(void) {
    printf("Executing tests.\n");
    http_client_tests();
    return 0;
}
