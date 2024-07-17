/**
 * 
 */

#include <stdlib.h>

/* For the request, configurable */
#ifndef REQUEST_LEN
#define REQUEST_LEN 2000
#endif

/* For the request line */
#define ACTION_LEN 7
#define RESOURCE_LEN 50
#define VERSION_LEN 8

/* For the map structure */
#define HEADERS_LEN 50
#define HEADERS 100

/* For the body, configurable */
#ifndef BODY_LEN
#define BODY_LEN 1000
#endif

struct Key {
    char keys[HEADERS_LEN];
};

struct Value {
    char values[HEADERS_LEN];
};

/* A data structure for simulating a hash map */
struct Map {
    struct Key key;
    struct Value value;
};

/**
 * It holds all the information associated with a
 * http request.
 */
struct http_request {
    /* Related to the request line */
    char action[ACTION_LEN];
    char resource[RESOURCE_LEN];
    char version[VERSION_LEN];
    /* For holding the headers */
    struct Map header[HEADERS];
    /* Body */
    char body[BODY_LEN];
};

/**
 * The djb2 hash function for strings.
 * 
 * Reference: http://www.cse.yorku.ca/~oz/hash.html
 */
unsigned long hash(unsigned char *str);
