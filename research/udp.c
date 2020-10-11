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
    int serv_sock;
    struct sockaddr_in serv_addr = {0,};
    int serv_addrlen = sizeof(serv_addr);

    serv_sock = socket(AF_INET, SOCK_DGRAM);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(7777);

    bind(serv_sock, (struct sockaddr *)&serv_addr, (socklen_t)serv_addrlen);
    listen(serv_sock, 100);
    
    return 0;
}