CC=gcc
CFLAGS=-pthread -I.

loadtest: loadbalancer.c endpoints.c endpoints.h
	$(CC) -o loadbalancer.out loadbalancer.c endpoints.c $(CFLAGS)
