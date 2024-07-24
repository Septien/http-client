A simple http client written in C, aimed to be executed in embedded devices.

The intention of this small library is to provide a simple and light-weight
http client, that can construct a request and parse a response string. This
library does not handle the connection to the server, so you can use any
TCP implementation you have at hand. It supports HTTP v1.1 only.

It provides two data structures:
- http_request
- http_response

and some associated functions to interact with them.

The `http_request` structure allows you to build the request, the request
line, the headers associated with the request, and attach a body. Then you can
obtain the string using the `create_request_str` function, which is then
ready to be sent to the server.

With the `http_response`, you can parse a response string returned from the
server with the function `parse_response_str` and then obtain the data from
it using the appropiate functions.

We use static memory to store all the data associated with the messages. You
can configure its size according to your needs, by changing the appropiate
macros in the `http-client.h` file.

To include it in your project, just copy the `http-client.h` and
`http-client.c` files into your project.

Running the command `make test && bin/main` will execute the unit tests.
