struct Buffer {
	char *buf;
	int size;
};

struct Uri {
	char *uri;
	int size;
};

struct Endpoint {
	char *host;
	int port;
	char active;
};

struct EndpointList {
	struct Endpoint *ep;
	struct EndpointList *next;
};

void parse_uri(struct Uri *target, char *payload);
char ping_endpoint(struct EndpointList *target); // ret value 0 for invalid endpoints, 1 for valid
void update_endpoint_status(struct EndpointList *epl);
void load_endpoint(struct EndpointList *epl);
void error_handling(char *msg);