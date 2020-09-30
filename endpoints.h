struct Uri {
	char *uri;
	int size;
};

struct Endpoint {
	char *host;
	int port;
	char active;
	struct Endpoint *next;
};

void exchange_data(int recv_fd, int send_fd, char *buffer, int buffer_size); // copy recv_fd & write to send_fd and vice versa
char pass_request(int recv_socket, struct Endpoint *cur, struct Endpoint *start); // pass request onto endpoint and send back response
void parse_uri(struct Uri *target, char *payload); // extract uri path from request payload
char ping_endpoint(struct Endpoint *target); // ret value 0 for invalid endpoints, 1 for valid
void update_endpoint_status(struct Endpoint *epl); // update endpoint's active field by ping
void load_endpoint(struct Endpoint *ep); // load endpoint details from file
void error_handling(char *msg); // handle error