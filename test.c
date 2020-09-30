#include <stdio.h>
#include <string.h>
#include "endpoints.h"

int tests_run = 0;

#define FAIL() printf("\nfailure in %s() line %d\n", __func__, __LINE__)
#define _assert(test) do {if (!(test)) { FAIL(); return 1;}} while (0)
#define _assert_not(test) do {if ((test)) { FAIL(); return 1;}} while (0)
#define _verify(test) do {int r=test(); tests_run++; if (r) return r;} while (0)

int ping_endpoint_test_1() {
	// SET UP A LOCAL SERVER ON PORT 8888 BEFOREHAND!
	struct Endpoint ep;
	ep.host = "127.0.0.1";
	ep.port = 9999;
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
	ep.port = 9999;
	ep.active = 1;

	struct EndpointList epl;
	epl.ep = &ep;
	epl.next = NULL;

	_assert(ping_endpoint(&epl) == 0);
	return 0;
}

int parse_uri_test_1() {
	char *test_request = "GET /test HTTP/1.1\nHost: localhost:8888\nUser-Agent: curl/7.68.0\nAccept: */*\n";
	struct Uri parsed_uri;
	parse_uri(&parsed_uri, test_request);

	char goal[] = "/test";
	int len_goal = sizeof(goal) - 1; // Length of NULL Character subtracted
	if (len_goal != parsed_uri.size)
		_assert(0);

	for (int i = 0; i < len_goal; i++) 
		_assert(*((parsed_uri.uri)+i) == goal[i]);
	return 0;
}

int parse_uri_test_2() {
	char *test_request = "someGobbledeyGook";
	struct Uri parsed_uri;
	parse_uri(&parsed_uri, test_request);

	_assert(parsed_uri.uri == NULL);
	_assert(parsed_uri.size == 0);
	return 0;
}

int parse_uri_test_3() {
	char *test_request = "GET /test/somethingaldkfjas;fdkj";
	struct Uri parsed_uri;
	parse_uri(&parsed_uri, test_request);

	_assert(parsed_uri.uri == NULL);
	_assert(parsed_uri.size == 0);
	return 0;
}

int pass_request_test_1() {
	// LEARNT WHEN NETWORK IO OR SOME OTHER MODULE IS INVOLVED, IT PROBABLY IS NOT THE BEST WAY TO TEST LIKE THIS
	// SOME PYTHON SCRIPT OR TOOL WOULD BE HANDY HERE
	return 0;
}

int verify_tests() {
	_verify(ping_endpoint_test_1);
	_verify(ping_endpoint_test_2);
	_verify(parse_uri_test_1);
	_verify(parse_uri_test_2);
	return 0;
}

int main() {
	int result = verify_tests();
	if (result == 0)
		printf("PASSED\n");
	printf("Tests run: %d\n", tests_run);

	return result != 0;
}
