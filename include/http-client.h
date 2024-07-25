#ifndef HTTP_CLIENT_HEADER
#define HTTP_CLIENT_HEADER
/**
 * 
 */

#include <stdlib.h>

/* For the request, configurable */
#ifndef REQUEST_LEN
#define REQUEST_LEN 4096
#endif

/* For the request line */
#define ACTION_LEN 7
#define RESOURCE_LEN 50
#define VERSION_LEN 9

/* For the response line */
#define STATUS_LEN 3    // It contains only 3 numbers
#define ST_TXT_LEN 100  // Len of status text

/* For the map structure */
#define HEADERS_LEN 50
#define HEADERS 50

/* For the body, configurable */
#ifndef BODY_LEN
#define BODY_LEN 1024
#endif

struct Key {
    char key[HEADERS_LEN];
};

struct Value {
    char value[HEADERS_LEN];
};

/* A data structure for simulating a hash map */
struct Map {
    struct Key key;
    struct Value value;
};

/**
 * @brief Holds the data associated with a http request.
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
 * @brief Holds the data associated with a http response. 
 */
struct http_response {
    /* Response line */
    char version[VERSION_LEN];
    char status_code[STATUS_LEN];
    char status_text[ST_TXT_LEN];
    /* The headers */
    struct Map headers[HEADERS];
    /* Body */
    char body[BODY_LEN];
};

/**
 * The djb2 hash function for strings.
 * 
 * Reference: http://www.cse.yorku.ca/~oz/hash.html
 */
unsigned long hash(unsigned char *str);

void clear_http_request(struct http_request *request);

void clear_http_response(struct http_response *response);

/**
 * @brief Set the values for creating the request line.
 *      Example: GET /resource HTTP/1.1
 * 
 * @param request The structure for holding the request.
 * @param action The verb or noun associated with the request (GET, POST, OPTIONS, etc.)
 * @param resource The resource of the request.
 * @param version The protocol version to use in the request.
 */
void set_request_method(struct http_request *request, char *action, char *resource, char *version);

/**
 * @brief Set the headers' name and value for the request. As the
 * headers in the request are case-insensitive, you can pass the header
 * name either in upper case or lower case, and will always store the
 * header in the same position, avoiding duplicates. If you pass
 * an already existing header, it will overwrite its previous value.
 * 
 * @param request The structure holding the request data.
 * @param name The header's name.
 * @param value The header's value.
 */
void set_header(struct http_request *request, char *name, char *value);

/**
 * @brief Copy the body to the request. You can pass the body's length and the
 * body's type (text/plain, text/html...) and the function will add the headers
 * 'Content-Type' and 'Content-Length' with their corresponding values for you.
 * 
 * @param request The structure holding the request data.
 * @param body The request's body.
 * @param len The body's len, used for the header. May be zero.
 * @param type The body's type, used for the header. May NULL or "".
 */
void set_body(struct http_request *request, char *body, int len, char *type);

/**
 * @brief Create a request string from the request structure, ready to be
 *        to the server. The headers do not follow any particular order.
 *
 * @param request The structure holding the request's data.
 * @param str String for storing the request.
 */
void create_request_str(struct http_request *request, char *str);

/**
 * @brief Parse the response string returned form the header and store all
 *        the values in the response structure.
 * 
 * @param reponse The structure to hold the response returned form server.
 * @param str The string containing the response.
 */
void parse_response_str(struct http_response *response, char *str);

/**
 * @brief Returns the protocol version, status code, and status text
 *        recieved form the server.
 *
 * @param response The structure containing the response.
 * @param version Array holding the protocol's version.
 * @param code Array containing the status code.
 * @param status Array containing the status text.
 */
void get_response_line(struct http_response *response, char *version, char *code, char *status);

/**
 * @brief Return the value of a given header.
 *
 * @param response The structure holding the response data.
 * @param header The header's name.
 * @param value Array to hold the header's value.
 *
 * @return 0 when header is found, -1 in other case.
 */
int get_header(struct http_response *response, char *header, char *value);

/**
 * @brief Returns the response's body. Internally, it searches for
 *        the 'Content-Length' header to determine whether the
 *        body exists and its length.
 *
 * @param response The structure holding the response.
 * @param body Array to store the body. It is assumed that
 *             the variable already has allocated memory.
 *
 * @return -1 when there is no body, and the body length's
 *  when it is present.
 */
int get_body(struct http_response *response, char *body);

#endif // HTTP_CLIENT_HEADER
