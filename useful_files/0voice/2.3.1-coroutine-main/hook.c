


#define _GNU_SOURCE

#include <dlfcn.h>

#include <stdio.h>
#include <ucontext.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include <sys/socket.h>
#include <errno.h>
#include <netinet/in.h>


#include <pthread.h>
#include <sys/poll.h>
#include <sys/epoll.h>


#if 1
// hook
typedef ssize_t (*read_t)(int fd, void *buf, size_t count);
read_t read_f = NULL;

typedef ssize_t (*write_t)(int fd, const void *buf, size_t count);
write_t write_f = NULL;



ssize_t read(int fd, void *buf, size_t count) {

	struct pollfd fds[1] = {0};

	fds[0].fd = fd;
	fds[0].events = POLLIN;

	int res = poll(fds, 1, 0);
	if (res <= 0) { //


		// fd --> epoll_ctl();

		swapcontext(); // fd --> ctx
		
	}
	// io

	
	ssize_t ret = read_f(fd, buf, count);
	printf("read: %s\n", (char *)buf);
	return ret;

	
	
}


ssize_t write(int fd, const void *buf, size_t count) {

	printf("write: %s\n", (const char *)buf);

	return write_f(fd, buf, count);
}



void init_hook(void) {

	if (!read_f) {
		read_f = dlsym(RTLD_NEXT, "read");
	}

	
	if (!write_f) {
		write_f = dlsym(RTLD_NEXT, "write");
	}

}

#endif



int main() {

	init_hook();

	int sockfd = socket(AF_INET, SOCK_STREAM, 0);

	struct sockaddr_in serveraddr;
	memset(&serveraddr, 0, sizeof(struct sockaddr_in));

	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(2048);

	if (-1 == bind(sockfd, (struct sockaddr*)&serveraddr, sizeof(struct sockaddr))) {
		perror("bind");
		return -1;
	}

	listen(sockfd, 10);

	struct sockaddr_in clientaddr;
	socklen_t len = sizeof(clientaddr);
	int clientfd = accept(sockfd, (struct sockaddr*)&clientaddr, &len);
	printf("accept\n");

	while (1) {

		char buffer[128] = {0};
		int count = read(clientfd, buffer, 128);
		if (count == 0) {
			break;
		}
		write(clientfd, buffer, count);
		printf("sockfd: %d, clientfd: %d, count: %d, buffer: %s\n", sockfd, clientfd, count, buffer);

	}

	return 0;

}



