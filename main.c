#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define PORT 3334

// #define LOG(...) printf(__VA_ARGS__)
#define LOG(...)

void throw() {
    perror(NULL);
    exit(errno);
}

void recv_mode() {
    LOG("recv\n");
    int fd = socket(PF_INET, SOCK_STREAM, 0);
    if (fd == -1)
        throw();
    if (errno)
        throw();

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(struct sockaddr_in));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(3334);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    int result =
        bind(fd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr));
    if (result == -1)
        throw();

    result = listen(fd, 0);
    if (result == -1)
        throw();

    int client_fd = accept(fd, NULL, NULL);
    if (errno)
        throw();

    ssize_t BUF_SIZE = 1024;
    char buf[BUF_SIZE];
    ssize_t bytes_read = read(client_fd, buf, BUF_SIZE);
    if (errno)
        throw();
    if (bytes_read == -1) {
        printf("err\n");
        exit(1);
    }
    if (bytes_read > BUF_SIZE) {
        printf("buffer overflowed.");
        exit(1);
    }
    printf("%s\n", buf);

    close(fd);
    LOG("done\n");
}

void send_mode(const char *addr, const char *msg) {
    LOG("send\n");
    int fd = socket(PF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(3334);
    inet_pton(AF_INET, addr, &(server_addr.sin_addr));
    int tmp_result;
    tmp_result =
        connect(fd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr));
    if (errno)
        throw();

    write(fd, msg, strlen(msg) + 1);
    LOG("done\n");
}

int main(int argc, char *argv[]) {
    if (argc == 1) {
        recv_mode();
    } else if (argc == 3) {
        char *addr = argv[1];
        char *msg = argv[2];
        send_mode(addr, msg);
    }
}

