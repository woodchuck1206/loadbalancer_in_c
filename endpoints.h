struct Endpoint {
	char host[];
	int port;
	int active;
};

struct EndpointList {
	struct Endpoint ep;
	struct Endpoint *next;
}

void ping_endpoint(struct EndpointList *epl);
void update_endpoint(struct EndpointList *epl);
