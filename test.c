#include <stdio.h>
#include "endpoints.h"

int tests_run = 0;

#define FAIL() printf("\nfailure in %s() line %d\n", __func__, __LINE__)
#define _assert(test) do {if (!(test)) { FAIL(); return 1;}} while (0)
#define _verify(test) do {int r=test(); tests_run++; if (r) return r;} while (0)

int ping_endpoint_test_1() {
	// SET UP A LOCAL SERVER ON PORT 8888 BEFOREHAND!
	struct Endpoint ep;
	ep.host = "127.0.0.1";
	ep.port = 8888;
	ep.active = 1;

	struct EndpointList epl;
	epl.ep = &ep;
	epl.next = NULL;

	_assert(ping_endpoint(&epl) == 1);
	return 0;
}

int ping_endpoint_test_2() {
	struct Endpoint ep;
	ep.host = "127.0.0.1";
	ep.port = 4444;
	ep.active = 1;

	struct EndpointList epl;
	epl.ep = &ep;
	epl.next = NULL;

	_assert(ping_endpoint(&epl) == 0);
	return 0;
}

int verify_tests() {
	_verify(ping_endpoint_test_1);
	_verify(ping_endpoint_test_2);
	return 0;
}

int main() {
	int result = verify_tests();
	if (result == 0)
		printf("PASSED\n");
	printf("Tests run: %d\n", tests_run);

	return result != 0;
}
