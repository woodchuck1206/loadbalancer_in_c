#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/epoll.h>

int main() {
    int cli_sock;
    struct sockaddr_in addr = {0, };
    int addrlen = sizeof(addr);
    char buff[1024] = {0,};

    cli_sock = socket(AF_INET, SOCK_DGRAM, 0);
    
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_family = AF_INET;
    addr.sin_port = htons(7777);

    fgets(buff, 1024, stdin);
    if (buff[0] == '!') {
        sleep(30);
    }
    sendto(cli_sock, buff, 1024, 0, (struct sockaddr *)&addr, (socklen_t)addrlen);
    close(cli_sock);
}