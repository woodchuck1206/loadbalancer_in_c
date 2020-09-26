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

void parse_uri(struct Uri *target, char *payload) {
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
	// IF URI NOT COPIED
	// UNEXPECTED BEHAVIOUR WHEN URI_BUFFER'S SIZE IS EQUAL TO URI_LENGTH
	if (target->size == 0) {
		target->uri = NULL;
	}
}

void load_endpoint(struct EndpointList *epl) {
	FILE *fp = fopen(ENDPOINT, "r");
	char buffer[25] = {0};
	
	while (fgets(buffer, 25, fp) != NULL) {
		// ADDR PARSER NOT IMPLEMENTED YET
		memset(buffer, 0, 25);
	}
	fclose(fp);
}

void update_endpoint_status(struct EndpointList *epl) {
	struct EndpointList *cur = epl;
	char ping_result;

	while (cur) {
		if ((ping_result = ping_endpoint(cur)) & 1) {
			epl->ep->active = 1;
		} else {
			epl->ep->active = 0;
		}
		cur = cur->next;
	}
}

char ping_endpoint(struct EndpointList *target) {
	struct sockaddr_in ep_addr;
	memset(&ep_addr, 0, sizeof(ep_addr));
	
	int addr_size = sizeof(ep_addr);
	int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	
	ep_addr.sin_family = AF_INET;
	ep_addr.sin_addr.s_addr = inet_addr(target->ep->host);
	ep_addr.sin_port = htons(target->ep->port);
		
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
