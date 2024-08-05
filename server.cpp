#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/ip.h>

// Prints an error message to standard error
static void msg(const char *msg) {
    fprintf(stderr, "%s\n", msg);
}

// Prints an error message with the error number and aborts the program
static void die(const char *msg) {
    int err = errno;
    fprintf(stderr, "[%d] %s\n", err, msg);
    abort();
}

// Handles communication with a connected client
static void do_something(int connfd) {
    char rbuf[64] = {}; // Buffer to store received data
    ssize_t n = read(connfd, rbuf, sizeof(rbuf) - 1); // Read data from client
    if (n < 0) {
        msg("read() error");
        return;
    }
    printf("client says: %s\n", rbuf); // Print the message received from the client

    char wbuf[] = "world";
    write(connfd, wbuf, strlen(wbuf)); // Send "world" message back to the client
}

int main() {
    // Create a socket for IPv4 and TCP communication
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        die("socket()");
    }

    // Allow the socket to be reused immediately after the program terminates
    int val = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));

    // Bind the socket to the address and port
    struct sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_port = ntohs(1234); // Set port to 1234
    addr.sin_addr.s_addr = ntohl(0); // Use the wildcard address 0.0.0.0
    int rv = bind(fd, (const sockaddr *)&addr, sizeof(addr));
    if (rv) {
        die("bind()");
    }

    // Listen for incoming connections with a maximum backlog of SOMAXCONN
    rv = listen(fd, SOMAXCONN);
    if (rv) {
        die("listen()");
    }

    while (true) {
        // Accept an incoming connection
        struct sockaddr_in client_addr = {};
        socklen_t socklen = sizeof(client_addr);
        int connfd = accept(fd, (struct sockaddr *)&client_addr, &socklen);
        if (connfd < 0) {
            continue; // Ignore errors and continue accepting other connections
        }

        // Handle the connection
        do_something(connfd);
        close(connfd); // Close the connection after handling
    }

    return 0;
}
