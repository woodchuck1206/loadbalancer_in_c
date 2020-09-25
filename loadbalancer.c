#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string.h> //memset

#define COMMON_LIB

#include "endpoint.h"

#define PORT 8888

int main() {
	int server_fd;
	struct sockaddr_in address;
	int addrlen = sizeof(address);
	int new_socket;
	int valread;

	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
		error_handling("SOCKET ERROR");

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);

	memset(address.sin_zero, 0, sizeof(address.sin_zero));
	
	if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
		error_handling("BIND ERROR");

	if (listen(server_fd, 100) < 0)
		error_handling("LISTEN ERROR");

	while (1) {
		if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
			error_handling("ACCEPT ERROR");

		char buffer[1024];
		memset(buffer, 0, 1024);
		valread = read(new_socket, buffer, 1024);
		printf("%s\n", buffer);
		close(new_socket);
	}
	return 0;
}

