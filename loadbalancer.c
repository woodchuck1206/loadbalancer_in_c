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

#define COMMON_LIB

#include "endpoints.h"

#define PORT 8888
#define LOCALHOST "127.0.0.1"

int main() {
	int server_fd;
	struct sockaddr_in address;
	int addrlen = sizeof(address);
	int new_socket;
	char valread;
	int cur = 0;

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

	struct Endpoint *start = &ep1;
	struct Endpoint *ep_cur = start;

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
		if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
			error_handling("ACCEPT ERROR");
		printf("RECEIVED REQUEST FROM %s:%d, trying to pass onto %s:%d\n", inet_ntoa(address.sin_addr), address.sin_port, ep_cur->host, ep_cur->port);
		if (pass_request(new_socket, ep_cur, start)) {
			printf("FAILED TO PASS REQUEST TO %s:%d\n", ep_cur->host, ep_cur->port);
			// FALLBACK TO BE IMPLEMENTED
		}
		if (!(ep_cur->next))
			ep_cur = start;
		else
			ep_cur = ep_cur->next;
		close(new_socket);
	}
	return 0;
}

