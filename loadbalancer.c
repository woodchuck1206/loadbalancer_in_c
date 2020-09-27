/*
DEMO LOADBALANCER,
BUFFER SIZE LIMITED TO 1024 BYTES FOR RECV AND SEND PURPOSES
DISTRIBUTES REQUEST TO PREDEFINED ENDPOINTS ON ROUND ROBIN TERMS
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <pthread.h>

#define COMMON_LIB

#include "endpoints.h"

#define PORT 8888
#define LOCALHOST "127.0.0.1"
#define SEND_SOCK_LENGTH 1000

struct ThreadArgs {
	int recv_sock;
	struct Endpoint ep;
};

pthread_mutex_t mutx;
struct Endpoint *start, *ep_cur;

int send_sockets[SEND_SOCK_LENGTH];
char send_status[SEND_SOCK_LENGTH];

void *handle_conn(void *arg);
void init_send_sock();
int get_send_sock();

int main() {
	int server_fd;
	struct sockaddr_in address;
	int addrlen = sizeof(address);
	pthread_t t_id;

	// PRESET ENDPOINTS FOR TEST PURPOSES
	struct Endpoint ep1, ep2;

	ep2.host = LOCALHOST;
	ep2.port = 7777;
	ep2.active = 1;
	ep2.next = NULL;

	ep1.host = LOCALHOST;
	ep1.port = 6666;
	ep1.active = 1;
	ep1.next = &ep2;

	start = &ep1;
	ep_cur = start;

	// TEST ENDPOINT SET UP
	init_send_sock();

	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
		error_handling("SOCKET ERROR");

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);

	memset(address.sin_zero, 0, sizeof(address.sin_zero));
	
	if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
		error_handling("BIND ERROR");

	if (listen(server_fd, 10) < 0)
		error_handling("LISTEN ERROR");

	printf("LISTENING TO PORT: %d\n\n", PORT);

	while (1) {
		struct ThreadArgs ta;
		ta.ep = *ep_cur;

		if ((ta.recv_sock = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
			error_handling("ACCEPT ERROR");
		printf("RECEIVED REQUEST FROM %s:%d, trying to pass onto %s:%d\n", inet_ntoa(address.sin_addr), address.sin_port, ta.ep.host, ta.ep.port);

		pthread_mutex_lock(&mutx);
		int send_sock_idx = get_send_sock_idx();		
		pthread_mutex_unlock(&mutx);

		pthread_create(&t_id, NULL, handle_conn, (void *)&ta);
		pthread_detach(t_id);

		if (!(ep_cur->next))
			ep_cur = start;
		else
			ep_cur = ep_cur->next;
	}
	return 0;
}

void *handle_conn(void *arg) {
	struct ThreadArgs ta = *((struct ThreadArgs *)arg);
	
	if (pass_request(ta.recv_sock, &(ta.ep), start)) {
		printf("FAILED TO PASS REQUEST TO %s:%d\n", ta.ep.host, ta.ep.port);
	}
	
	close(ta.recv_sock);
}

void init_send_sock() {
	for (int i = 0; i < SEND_SOCK_LENGTH; i++) {
		send_sockets[i] = socket(AF_INET, SOCK_STREAM, 0);
		send_status[i] = 1;
	}
}

int get_send_sock_idx() {
	for (int i = 0; i < SEND_SOCK_LENGTH; i++) {
		// to be implemented
	}
}