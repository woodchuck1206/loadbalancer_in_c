#ifndef COMMON_LIB

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#endif

#include "endpoints.h"

#define ENDPOINT "./endpoints"
#define URI_LENGTH 256

char pass_request(int recv_socket, struct Endpoint *cur, struct Endpoint *start) {
	char buffer[1024];
	struct sockaddr_in target;
	int proxy_socket = socket(AF_INET, SOCK_STREAM, 0);

	// Search if alive ep exists
	while (cur && !(cur->active)) {
		cur = cur->next;
	}
	// If reached the end (NULL), go back to start
	if (!cur)
		cur = start;
	// Search once more
	while (cur && !(cur->active)) {
		cur = cur->next;
	}
	// If no ep is available, return 1
	if (!cur)
		return 1;

	target.sin_family = AF_INET;
	target.sin_addr.s_addr = inet_addr(cur->host);
	target.sin_port = htons(cur->port);
	memset(&target.sin_zero, 0,sizeof(target.sin_zero));

	if (connect(proxy_socket, (struct sockaddr *)&target, sizeof(target)) == -1)
		return 1;
	
	exchange_data(recv_socket, proxy_socket, buffer, sizeof(buffer));
	close(proxy_socket);
	return 0;
}

// FUNCTION UNDERNEATH HAS YET TO IMPLEMENT ERROR CATCHING LOGIC
void exchange_data(int recv_fd, int send_fd, char *buffer, int buffer_size) {
	// READ FROM RECV FD
	memset(buffer, 0, buffer_size);
	read(recv_fd, buffer, buffer_size);
	// WRITE RECEIVED PAYLOAD TO SEND FD
	write(send_fd, buffer, buffer_size);
	memset(buffer, 0, buffer_size);
	// PASS RESPONSE FROM SEND FD ONTO RECV FD 
	read(send_fd, buffer, buffer_size);
	write(recv_fd, buffer, buffer_size);
}

/*
	In case payload is in HTTP/1 format
	target->uri = URI except scheme, domain, port
	target->size = target->uri's size

	In case payload is not in HTTP/1 format
	target->uri = NULL;
	target->size = 0;
*/
void parse_uri(struct Uri *target, char *payload) {
	target->uri = NULL;
	target->size = 0;
	int first_space = -1;
	int i = 0;
	char ch;

	while ((ch = *(payload + i)) != 0) {
		if (ch == ' ') {
			if (first_space == -1) {
				first_space = i;
			} else {
				target->uri = payload+first_space+1;
				target->size = i - first_space - 1;
				break;
			}
		}
		++i;
	}
}

void load_endpoint(struct Endpoint *ep) {
	FILE *fp = fopen(ENDPOINT, "r");
	char buffer[25] = {0};
	
	while (fgets(buffer, 25, fp) != NULL) {
		// ADDR PARSER NOT IMPLEMENTED YET
		memset(buffer, 0, 25);
	}
	fclose(fp);
}

void update_endpoint_status(struct Endpoint *ep) {
	struct Endpoint *cur = ep;
	char ping_result;

	while (cur) {
		if ((ping_result = ping_endpoint(cur)) & 1) {
			ep->active = 1;
		} else {
			ep->active = 0;
		}
		cur = cur->next;
	}
}

char ping_endpoint(struct Endpoint *ep) {
	struct sockaddr_in ep_addr;
	memset(&ep_addr, 0, sizeof(ep_addr));
	
	int addr_size = sizeof(ep_addr);
	int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	
	ep_addr.sin_family = AF_INET;
	ep_addr.sin_addr.s_addr = inet_addr(ep->host);
	ep_addr.sin_port = htons(ep->port);
		
	if (connect(sock_fd, (struct sockaddr *)&ep_addr, (socklen_t)addr_size) == -1) {
		close(sock_fd);
		return 0;
	}
	close(sock_fd);
	return 1;
}

void error_handling(char *msg) {
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(1);
}