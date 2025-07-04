


#include <errno.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <poll.h>
#include <sys/epoll.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/sendfile.h>

#include <openssl/sha.h>
#include <openssl/pem.h>
#include <openssl/bio.h>
#include <openssl/evp.h>


#include "reactor.h"

int http_request(struct conn *c) {

	memset(c->wbuffer, 0, BUFFER_LENGTH);
	c->wlength = 0;

	c->status = 0;

	return 0;
}


int http_response(struct conn *c) {

//Tue Apr 30 06:41:52 AM UTC 2024
#if 0
	c->wlength = sprintf(c->wbuffer, 		
		"HTTP/1.1 200 OK\r\n"		
		"Accept-Ranges: bytes\r\n"		
		"Content-Length: 82\r\n"		
		"Content-Type: text/html\r\n"		
		"Date: Tue, 30 Apr 2024 13:16:46 GMT\r\n\r\n"		
		"<html><head><title>0voice.king</title></head><body><h1>King</h1></body></html>\r\n\r\n");
#elif 0

	int filefd = open("index.html", O_RDONLY);

	struct stat stat_buf;
	fstat(filefd, &stat_buf);

	c->wlength = sprintf(c->wbuffer, 		
		"HTTP/1.1 200 OK\r\n"		
		"Accept-Ranges: bytes\r\n"		
		"Content-Length: %ld\r\n"		
		"Content-Type: text/html\r\n"		
		"Date: Tue, 30 Apr 2024 13:16:46 GMT\r\n\r\n",
		stat_buf.st_size);

	//printf("header: %s\n", c->wbuffer);

	int count = read(filefd, c->wbuffer + c->wlength, BUFFER_LENGTH - c->wlength);
	if (count < 0) {
		printf("read: %d, errno: %d, %s\n", filefd, errno, strerror(errno));
	}
	c->wlength += count;

	close(filefd);
	//printf("body: %s\n", c->wbuffer);


#elif 0

	int filefd = open("index.html", O_RDONLY);

	struct stat stat_buf;
	fstat(filefd, &stat_buf);

	if (c->status == 0) {
		c->wlength = sprintf(c->wbuffer, 		
			"HTTP/1.1 200 OK\r\n"		
			"Accept-Ranges: bytes\r\n"		
			"Content-Length: %ld\r\n"		
			"Content-Type: text/html\r\n"		
			"Date: Tue, 30 Apr 2024 13:16:46 GMT\r\n\r\n",
			stat_buf.st_size);
		c->status = 1;
		printf("header: %s\n", c->wbuffer);
	} else {
		int ret = sendfile(c->fd, filefd, NULL, stat_buf.st_size);
		if (ret == -1) {			
			printf("errno: %d\n", errno);		
		}
		c->status = 0;
		printf("sendfile\n");
		c->wlength = 0;
	}
	
	close(filefd);

#else 

	int filefd = open("c1000k.png", O_RDONLY);

	struct stat stat_buf;
	fstat(filefd, &stat_buf);

	if (c->status == 0) {
		c->wlength = sprintf(c->wbuffer, 		
			"HTTP/1.1 200 OK\r\n"		
			"Accept-Ranges: bytes\r\n"		
			"Content-Length: %ld\r\n"		
			"Content-Type: image/png\r\n"		
			"Date: Tue, 30 Apr 2024 13:16:46 GMT\r\n\r\n",
			stat_buf.st_size);
		c->status = 1;
		printf("header: %s\n", c->wbuffer);
	} else {
		int ret = sendfile(c->fd, filefd, NULL, stat_buf.st_size);
		if (ret == -1) {			
			printf("errno: %d\n", errno);		
		}
		c->status = 0;
		printf("sendfile\n");
		c->wlength = 0;
	}
	
	close(filefd);

#endif

	return c->wlength;

}



// websocket
#define GUID 					"258EAFA5-E914-47DA-95CA-C5AB0DC85B11"


struct _nty_ophdr {

	unsigned char opcode:4,
		 rsv3:1,
		 rsv2:1,
		 rsv1:1,
		 fin:1;
	unsigned char payload_length:7,
		mask:1;

} __attribute__ ((packed));

struct _nty_websocket_head_126 {
	unsigned short payload_length;
	char mask_key[4];
	unsigned char data[8];
} __attribute__ ((packed));

struct _nty_websocket_head_127 {

	unsigned long long payload_length;
	char mask_key[4];

	unsigned char data[8];
	
} __attribute__ ((packed));

typedef struct _nty_websocket_head_127 nty_websocket_head_127;
typedef struct _nty_websocket_head_126 nty_websocket_head_126;
typedef struct _nty_ophdr nty_ophdr;




int base64_encode(char *in_str, int in_len, char *out_str) {    
	BIO *b64, *bio;    
	BUF_MEM *bptr = NULL;    
	size_t size = 0;    

	if (in_str == NULL || out_str == NULL)        
		return -1;    

	b64 = BIO_new(BIO_f_base64());    
	bio = BIO_new(BIO_s_mem());    
	bio = BIO_push(b64, bio);
	
	BIO_write(bio, in_str, in_len);    
	BIO_flush(bio);    

	BIO_get_mem_ptr(bio, &bptr);    
	memcpy(out_str, bptr->data, bptr->length);    
	out_str[bptr->length-1] = '\0';    
	size = bptr->length;    

	BIO_free_all(bio);    
	return size;
}


int readline(char* allbuf,int level,char* linebuf){    
	int len = strlen(allbuf);    

	for (;level < len; ++level)    {        
		if(allbuf[level]=='\r' && allbuf[level+1]=='\n')            
			return level+2;        
		else            
			*(linebuf++) = allbuf[level];    
	}    

	return -1;
}

void demask(char *data,int len,char *mask){    
	int i;    
	for (i = 0;i < len;i ++)        
		*(data+i) ^= *(mask+(i%4));
}



char* decode_packet(unsigned char *stream, char *mask, int length, int *ret) {

	nty_ophdr *hdr =  (nty_ophdr*)stream;
	unsigned char *data = stream + sizeof(nty_ophdr);
	int size = 0;
	int start = 0;
	//char mask[4] = {0};
	int i = 0;

	if ((hdr->mask & 0x7F) == 126) {

		nty_websocket_head_126 *hdr126 = (nty_websocket_head_126*)data;
		size = hdr126->payload_length;
		
		for (i = 0;i < 4;i ++) {
			mask[i] = hdr126->mask_key[i];
		}
		
		start = 8;
		
	} else if ((hdr->mask & 0x7F) == 127) {

		nty_websocket_head_127 *hdr127 = (nty_websocket_head_127*)data;
		size = hdr127->payload_length;
		
		for (i = 0;i < 4;i ++) {
			mask[i] = hdr127->mask_key[i];
		}
		
		start = 14;

	} else {
		size = hdr->payload_length;

		memcpy(mask, data, 4);
		start = 6;
	}

	*ret = size;
	demask(stream+start, size, mask);

	return stream + start;
	
}

int encode_packet(char *buffer,char *mask, char *stream, int length) {

	nty_ophdr head = {0};
	head.fin = 1;
	head.opcode = 1;
	int size = 0;

	if (length < 126) {
		head.payload_length = length;
		memcpy(buffer, &head, sizeof(nty_ophdr));
		size = 2;
	} else if (length < 0xffff) {
		nty_websocket_head_126 hdr = {0};
		hdr.payload_length = length;
		memcpy(hdr.mask_key, mask, 4);

		memcpy(buffer, &head, sizeof(nty_ophdr));
		memcpy(buffer+sizeof(nty_ophdr), &hdr, sizeof(nty_websocket_head_126));
		size = sizeof(nty_websocket_head_126);
		
	} else {
		
		nty_websocket_head_127 hdr = {0};
		hdr.payload_length = length;
		memcpy(hdr.mask_key, mask, 4);
		
		memcpy(buffer, &head, sizeof(nty_ophdr));
		memcpy(buffer+sizeof(nty_ophdr), &hdr, sizeof(nty_websocket_head_127));

		size = sizeof(nty_websocket_head_127);
		
	}

	memcpy(buffer+2, stream, length);

	return length + 2;
}

#define WEBSOCK_KEY_LENGTH	19

int handshark(struct conn *c) {

	//ev->buffer , ev->length

	char linebuf[1024] = {0};
	int idx = 0;
	char sec_data[128] = {0};
	char sec_accept[32] = {0};

	do {

		memset(linebuf, 0, 1024);
		idx = readline(c->rbuffer, idx, linebuf);

		if (strstr(linebuf, "Sec-WebSocket-Key")) {

			//linebuf: Sec-WebSocket-Key: QWz1vB/77j8J8JcT/qtiLQ==
			strcat(linebuf, GUID);

			//linebuf: 
			//Sec-WebSocket-Key: QWz1vB/77j8J8JcT/qtiLQ==258EAFA5-E914-47DA-95CA-C5AB0DC85B11

			
			SHA1(linebuf + WEBSOCK_KEY_LENGTH, strlen(linebuf + WEBSOCK_KEY_LENGTH), sec_data); // openssl

			base64_encode(sec_data, strlen(sec_data), sec_accept);

			memset(c->wbuffer, 0, BUFFER_LENGTH); 

			c->wlength = sprintf(c->wbuffer, "HTTP/1.1 101 Switching Protocols\r\n"
					"Upgrade: websocket\r\n"
					"Connection: Upgrade\r\n"
					"Sec-WebSocket-Accept: %s\r\n\r\n", sec_accept);

			printf("ws response : %s\n", c->wbuffer);

			break;
			
		}

	} while((c->rbuffer[idx] != '\r' || c->rbuffer[idx+1] != '\n') && idx != -1 );

	return 0;
}



int ws_request(struct conn *c) {

	if (c->status == 0) {
		
		handshark(c);
		c->status = 1;
		
	} else if (c->status == 1) {
		char mask[4] = {0};
		int ret = 0;

		char *data = decode_packet(c->rbuffer, mask, c->rlength, &ret);

		printf("data : %s , length : %d\n", data, ret);

		ret = encode_packet(c->wbuffer, mask, data, ret);
		c->wlength = ret;
	}

	return 0;
}

int ws_response(struct conn *c) {

	return 0;
}


