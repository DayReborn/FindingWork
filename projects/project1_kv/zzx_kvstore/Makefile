CC = gcc
TARGET = kvstore
SRCS = kvstore.c kv_reactor.c kv_ntyco.c kv_iouring.c kvs_array.c
INCLUDES = 
LIBS = -L ./NtyCo/ -lntyco -lpthread -ldl -Wl,-Bstatic -luring -Wl,-Bdynamic


all:
	$(CC) -o $(TARGET) $(SRCS) $(INCLUDES) $(LIBS)

clean:
	rm -rf kvstore
	rm -rf test_kvstore
	
test:
	gcc -o test_kvstore test_kvstore.c 
	./test_kvstore 192.168.229.133 2000 0 
	./test_kvstore 192.168.229.133 2000 1
	./test_kvstore 192.168.229.133 2000 2