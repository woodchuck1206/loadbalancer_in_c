struct Endpoint {
	int port;
	char active;
	char *host;
};

struct EndpointList {
	struct Endpoint *ep;
	struct EndpointList *next;
};

char ping_endpoint(struct EndpointList *target); // ret value 0 for invalid endpoints, 1 for valid
void update_endpoint_status(struct EndpointList *epl);
void load_endpoint(struct EndpointList *epl);
