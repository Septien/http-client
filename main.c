#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <errno.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "http-client.h"
#include "http-client-test.h"

int main(void) {
#ifdef TEST
    printf("Executing tests.\n");
    http_client_tests();
#else
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd == -1) {
        printf("Cannot create socket: %s...\n", strerror(errno));
        return 1;
    }
    /* Connect to server */
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_addr.sin_port = htons(5000);
    memset(serv_addr.sin_zero, 0, 8);

    int rc = connect(socket_fd, (struct sockaddr *) &serv_addr, sizeof serv_addr);
    if (rc == -1) {
        printf("Cannot connect to server %s\n", strerror(errno));
    }

    struct http_request request, *req_ptr;
    req_ptr = &request;
    clear_http_request(req_ptr);
    set_request_method(req_ptr, "GET", "/", "HTTP/1.1");
    set_header(req_ptr, "Host", "127.0.0.1:5000");
    set_header(req_ptr, "User-Agent", "C client");
    set_header(req_ptr, "Accept", "*/*");
    char request_str[REQUEST_LEN];
    memset(request_str, 0, REQUEST_LEN);
    create_request_str(req_ptr, request_str);

    printf("%s\n", request_str);
    int bytes_sent = send(socket_fd, request_str, strlen(request_str), 0);
    printf("Total bytes sent: %d\n", bytes_sent);

    memset(request_str, 0, REQUEST_LEN);
    int bytes_recv = recv(socket_fd, request_str, REQUEST_LEN, 0);
    printf("Byte recv: %d\n", bytes_recv);
    printf("%s\n", request_str);

    struct http_response response, *rep_ptr;
    rep_ptr = &response;
    clear_http_response(rep_ptr);
    parse_response_str(rep_ptr, request_str);
    
    char version[VERSION_LEN];
    char code[STATUS_LEN];
    char status[ST_TXT_LEN];
    memset(version, 0, VERSION_LEN);
    memset(code, 0, STATUS_LEN);
    memset(status, 0, ST_TXT_LEN);
    get_response_line(rep_ptr, version, code, status);
    printf("%s %s %s\n", version, code, status);

    char value[HEADERS_LEN];
    memset(value, 0, HEADERS_LEN);
    get_header(rep_ptr, "Server", value);
    printf("%s\n", value);
    memset(value, 0, HEADERS_LEN);
    get_header(rep_ptr, "Date", value);
    printf("%s\n", value);
    memset(value, 0, HEADERS_LEN);
    get_header(rep_ptr, "Content-Type", value);
    printf("%s\n", value);
    memset(value, 0, HEADERS_LEN);
    get_header(rep_ptr, "Content-Length", value);
    printf("%s\n", value);
    memset(value, 0, HEADERS_LEN);
    get_header(rep_ptr, "Connection", value);
    printf("%s\n", value);

    char body[BODY_LEN];
    memset(body, 0, BODY_LEN);
    get_body(rep_ptr, body);
    printf("%s\n", body);

    close(socket_fd);
#endif
    return 0;
}
