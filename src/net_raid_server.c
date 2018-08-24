#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h> /* superset of previous */
#include <unistd.h>
#include <stdlib.h>

#define BACKLOG 10

/*
*   this server implementation taken from provided socket connection example
*   from OS_2018_Spring, link : https://classroom.google.com/u/0/c/MTIxNTI5NDEyODZa
*/


void client_handler(int cfd) {
    char buf[1024];
    int data_size;
    while (1) {
        data_size = read (cfd, &buf, 1024);
        if (data_size <= 0)
            break;
        buf [data_size] = 0;
        printf ("recieved data is %s\n with size %d\n", buf, data_size);
        write (cfd, &buf, data_size);
    }
    close(cfd);
}

int main(int argc, char* argv[])
{
    int sfd, cfd;
    struct sockaddr_in addr;
    struct sockaddr_in peer_addr;
    int port = 5000;

    sfd = socket(AF_INET, SOCK_STREAM, 0);
    int optval = 1;
    setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    bind(sfd, (struct sockaddr *) &addr, sizeof(struct sockaddr_in));
    listen(sfd, BACKLOG);

    unsigned int peer_addr_size = sizeof(struct sockaddr_in);
    cfd = accept(sfd, (struct sockaddr *) &peer_addr, &peer_addr_size);

    switch(fork()) {
        case -1:
            exit(100);
        case 0:
            close(sfd);
            client_handler(cfd);
            exit(0);
        default:
            close(cfd);
    }
    close(sfd);
}
