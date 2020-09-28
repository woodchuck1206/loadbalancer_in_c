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
#include <errno.h>

#define PORT 8888
#define LOCALHOST "127.0.0.1"
#define MAX_CONN 3000

struct ThreadArgs {
	int recv_sock;
	char *sock_status;
	int idx;
	struct Endpoint ep;
};

// pthread_mutex_t mutx; // recv_sock_status does what semaphore would have done
struct Endpoint *start, *ep_cur;

void *handle_conn(void *arg);
int get_recv_sock_idx(char *recv_sock_status, int *recv_cursor);

int main() {
	int server_fd;
	struct sockaddr_in address;
	int addrlen = sizeof(address);

	struct ThreadArgs recv_socks[MAX_CONN];
	char recv_sock_status[MAX_CONN] = {1,};
	int recv_cursor = 0;

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

	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
		error_handling("SOCKET ERROR");

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);

	memset(address.sin_zero, 0, sizeof(address.sin_zero));
	
	if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
		error_handling("BIND ERROR");

	if (listen(server_fd, 1000) < 0)
		error_handling("LISTEN ERROR");

	printf("LISTENING TO PORT: %d\n\n", PORT);

	while (1) {
		if (errno > 0) 
			printf("ERROR NO: %d\n", errno);
		int idx = get_recv_sock_idx(recv_sock_status, &recv_cursor);
		recv_socks[idx].ep = *ep_cur;
		recv_socks[idx].sock_status = recv_sock_status;
		recv_socks[idx].idx = idx;

		if ((recv_socks[idx].recv_sock = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
			error_handling("ACCEPT ERROR");
		printf("RECEIVED REQUEST FROM %s:%d, trying to pass onto %s:%d\n", inet_ntoa(address.sin_addr), address.sin_port, recv_socks[idx].ep.host, recv_socks[idx].ep.port);

		pthread_create(&t_id, NULL, handle_conn, (void *)&(recv_socks[idx]));
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
	*(ta.sock_status+ta.idx) = 1;
}

int get_recv_sock_idx(char *recv_sock_status, int *recv_cursor) {
	while (1) {
		(*recv_cursor) += 1;
		if (*(recv_cursor) >= MAX_CONN)
			(*recv_cursor) = 0;
		if (*(recv_sock_status+*recv_cursor)) {
			*(recv_sock_status+*recv_cursor) = 0;
			break;
		}
	}
	return *recv_cursor;
}
