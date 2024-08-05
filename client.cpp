#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/ip.h>

// Prints an error message with the error number and aborts the program
static void die(const char *msg) {
    int err = errno;
    fprintf(stderr, "[%d] %s\n", err, msg);
    abort();
}

int main() {
    // Create a socket for IPv4 and TCP communication
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        die("socket()");
    }

    // Define the server address and port
    struct sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_port = ntohs(1234); // Set port to 1234
    addr.sin_addr.s_addr = ntohl(INADDR_LOOPBACK); // Connect to localhost (127.0.0.1)

    // Connect to the server
    int rv = connect(fd, (const struct sockaddr *)&addr, sizeof(addr));
    if (rv) {
        die("connect");
    }

    // Send a message to the server
    char msg[] = "hello";
    write(fd, msg, strlen(msg));

    // Receive a message from the server
    char rbuf[64] = {};
    ssize_t n = read(fd, rbuf, sizeof(rbuf) - 1);
    if (n < 0) {
        die("read");
    }
    printf("server says: %s\n", rbuf); // Print the server's response

    // Close the socket
    close(fd);
    return 0;
}
