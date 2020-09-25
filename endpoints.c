#ifndef COMMON_LIB

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

#endif

#include "endpoint.h"

#define ENDPOINT "./endpoints"

void update_endpoint(struct EndpointList *epl) {
	FILE *fp = fopen(ENDPOINT, "r");
	char buffer[25] = {0};
	

	close(fp);
}

int ping_endpoint(struct EndpointList *epl) {
	struct sockaddr_in ep_addr;
	memset(&ep_addr, 0, sizeof(ep_addr));
	ep_addr.sin_family = AF_INET;
	int addr_size = sizeof(ep_addr);
	int socket = socket(AF_INET, SOCK_STREAM, 0);

	for (int i = 0; i < epl->size; i++) {
		ep_addr.sin_addr.s_addr = inet_addr((*((epl->ep)+i)).host);
		ep_addr.sin_port = htons((*((epl->ep)+i)).port);
		
		if (connect(socket, (struct sockaddr *)&ep_addr, (socklen_t)addr_size) == -1) {
			//NODE would be so much better for this kind of logic
		}
	}
}

void error_handling(char *msg) {
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(1);
}
