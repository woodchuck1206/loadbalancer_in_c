#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/epoll.h>
// #include <sys/epoll.h>

int main() {
    int serv_sock;
    struct sockaddr_in serv_addr = {0,};
    int serv_addrlen = sizeof(serv_addr);
    char buff[1024] = {0, };

    serv_sock = socket(AF_INET, SOCK_DGRAM, 0);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(7777);

    bind(serv_sock, (struct sockaddr *)&serv_addr, (socklen_t)serv_addrlen);

    int cli_sock;
    struct sockaddr_in cli_addr = {0,};
    int cli_addrlen = sizeof(cli_addr);
    int val;

	while (1) {
        printf("receiving...\n");
		recv(serv_sock, buff, 1024, 0);
        printf("\n%s\n", buff);
    }
    close(serv_sock);
    
    return 0;
}
