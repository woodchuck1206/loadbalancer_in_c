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
#include <sys/epoll.h>
#include <fcntl.h>

#define COMMON_LIB

#include "endpoints.h"
#include <errno.h>

#define UPDATE_PORT 5555
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
int HEALTH_CHECK_INTERVAL = 15;
int UPDATE_INTERVAL = 1000;

void substring(char *to, char *from, int left, int right);
int find_idx(char *source, int size, char target);
void setnonblocking(int sock);
void update(char *buffer, struct Endpoint *start);
void *update_endpoint(void *arg);
void *timed_update_ep(void *arg);
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
	struct Endpoint load;
	load_endpoint(&load);

	start = &load;
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

	pthread_create(&t_id, NULL, timed_update_ep, (void *)&HEALTH_CHECK_INTERVAL);
	pthread_create(&t_id, NULL, update_endpoint, (void *)&UPDATE_INTERVAL);
	pthread_detach(t_id);

	while (1) {
		if (errno > 0) 
			printf("ERROR NO: %d\n", errno);
		int idx = get_recv_sock_idx(recv_sock_status, &recv_cursor);
		recv_socks[idx].ep = *ep_cur;
		recv_socks[idx].sock_status = recv_sock_status;
		recv_socks[idx].idx = idx;

		if ((recv_socks[idx].recv_sock = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
			error_handling("ACCEPT ERROR");

		pthread_create(&t_id, NULL, handle_conn, (void *)&(recv_socks[idx]));
		pthread_detach(t_id);

		if (!(ep_cur->next))
			ep_cur = start;
		else
			ep_cur = ep_cur->next;
	}
	return 0;
}

void setnonblocking(int sock) {
	int flag = fcntl(sock, F_GETFL, 0);
	fcntl(sock, F_SETFL, flag | O_NONBLOCK);
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

void *timed_update_ep(void *arg) {
	int sec = *((int *)arg);
	printf("Checking on endpoints...\n");
	
	while (1) {
		update_endpoint_status(start);
		sleep(sec);
	}
}

void *update_endpoint(void *arg) {
	int interval = *(int *)arg;

	int update_socket = socket(AF_INET, SOCK_STREAM, 0), conn_sock;
	struct sockaddr_in address;
	int addr_len = sizeof(address);

	char buffer[30] = {0,};
	int epoll_fd;
	int event_count;
	struct epoll_event event, events[30];

	//==============================CONNECTING SOCKET===========================================

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(UPDATE_PORT);

	memset(address.sin_zero, 0, sizeof(address.sin_zero));

	if (bind(update_socket, (struct sockaddr *)&address, addr_len) < 0)
		error_handling("ENDPOINT UPDATE SERVER NOT AVAILABLE");

	if (listen(update_socket, 1000) < 0)
		error_handling("UPDATE SOCKET LISTEN ERROR");

	//==============================CONNECTING SOCKET DOEN======================================

	epoll_fd = epoll_create1(0);
	event.events = EPOLLIN;
	event.data.fd = update_socket;

	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, update_socket, &event)) {
		error_handling("FAILED TO ADD FD TO EPOLL");
		close(epoll_fd);
		close(update_socket);
	}

	printf("Update server initiated...\n");

	struct sockaddr_in recv_addr;
	int recv_addr_len = sizeof(recv_addr);

	while (1) {
		event_count = epoll_wait(epoll_fd, events, 30, interval);
		for (int i = 0; i < event_count; i++) {
			if (events[i].data.fd == update_socket) {
				conn_sock = accept(update_socket, (struct sockaddr *)&recv_addr, (socklen_t *)&recv_addr_len);

				setnonblocking(conn_sock);
				memset(&event, 0, sizeof(event));
				event.events = EPOLLIN | EPOLLET;
				event.data.fd = conn_sock;
				epoll_ctl(epoll_fd, EPOLL_CTL_ADD, conn_sock, &event);
			} else {
				conn_sock = events[i].data.fd;
				memset(buffer, 0, 30);
				int n = read(events[i].data.fd, buffer, 30);
				if (n <= 0) {
					epoll_ctl(epoll_fd, EPOLL_CTL_DEL, conn_sock, &event);
					close(conn_sock);
				} else {
					printf("Received %s\n", buffer);
					update(buffer, start);
					write(conn_sock, "FINISHED\n", 9);
				}
			}
		}
	}

	close(epoll_fd);
	close(update_socket);
}

void update(char *buffer, struct Endpoint *start) {
	char flag = -1; // 0 for add, 1 for del
	char method[4] = {0, };
	char *host, *port_str;
	int port;

	memcpy(method, buffer, 3);
	if (strcmp(method, "ADD") == 0)
		flag = 0;
	else if (strcmp(method, "DEL") == 0)
		flag = 1;

	if (flag == -1) {
		printf("%s is not a valid method. It should be either ADD or DEL\n", method);
		return;
	}

	int second_tab = find_idx(buffer + 4, 25, '\t');
	int semicolon = find_idx(buffer + 4, 25, ';');

	host = (char *)malloc(sizeof(char) * (second_tab - 3));
	substring(host, buffer+4, 0, second_tab-1);
	port_str = (char *)malloc(sizeof(char) * (semicolon - second_tab));
	substring(port_str, buffer+4, second_tab+1, semicolon-1);
	port = atoi(port_str);

	if (flag == 0) {
		while (1) {
		if (start->host == host && start->port == port) {
			break;
		}

		if (!(start->next)) {
			start->next = (struct Endpoint *)malloc(sizeof(struct Endpoint));
			start->next->host = host;
			start->next->port = port;
			start->next->active = 1;
			break;
		}

		start = start->next;
		}
	} else {
		while (1) {
		if (start->host == host && start->port == port) {
			start->active = 0; // to reallocate start to start + 1, a little bit of change of the function structure is needed
		}

		if (!(start->next)) {
			break;
		}

		start = start->next;
		}
	}
		
}

int find_idx(char *source, int size, char target) {
	for (int i = 0; i < size; i++) {
		if (*(source+i) == target) {
			return i;
		}
	}
	return -1;
}

void substring(char *to, char *from, int left, int right) {
	int size = right - left + 1, i = 0;
	while (i < size) {
		*(to+i) = *(from+i+left);
		++i;
	}
	*(to+i) = 0;
}