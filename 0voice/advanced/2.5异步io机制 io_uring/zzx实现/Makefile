CC = gcc
LDFLAGS = -luring -static

all: epoll_server uring_server test_client

epoll_server: 
	$(CC)  epoll_tcp_server.c -o epoll_tcp_server

uring_server: 
	$(CC) uring_tcp_server.c -o uring_tcp_server $(LDFLAGS)

test_epoll_qps: clean
	$(CC) test_qps_client.c -o test_qps_client -lpthread
	./test_qps_client -s 127.0.0.1 -p 2048 -t 50 -c 100 -n 1000000

test_uring_qps: clean
	$(CC) test_qps_client.c -o test_qps_client -lpthread
	./test_qps_client -s 127.0.0.1 -p 9999 -t 50 -c 100 -n 1000000

clean:
	rm -rf epoll_tcp_server uring_tcp_server test_qps_client

.PHONY: all clean
