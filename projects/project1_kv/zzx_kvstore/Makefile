CC = gcc
TARGET = kvstore
SRCS = kvstore.c kv_reactor.c kv_ntyco.c kv_iouring.c kvs_array.c kvs_rbtree.c kvs_hash.c
INCLUDES = 
LIBS = -L ./NtyCo/ -lntyco -lpthread -ldl -Wl,-Bstatic -luring -Wl,-Bdynamic
SUBDIR = ./NtyCo/
TESTCASE_SRCS = test_kvstore.c
TESTCASE = test_kvstore

all:
	$(CC) -o $(TARGET) $(SRCS) $(INCLUDES) $(LIBS)

clean:
	rm -rf kvstore
	rm -rf $(TESTCASE)
	
array_testcase:
	$(CC) -o ${TESTCASE} $(TESTCASE_SRCS)
	./${TESTCASE} 192.168.229.133 2000 0
array_testcase_1w:
	$(CC) -o ${TESTCASE} $(TESTCASE_SRCS)
	./${TESTCASE} 192.168.229.133 2000 1
rbtree_testcase:
	$(CC) -o ${TESTCASE} $(TESTCASE_SRCS)
	./${TESTCASE} 192.168.229.133 2000 2
rbtree_testcase_1w:
	$(CC) -o ${TESTCASE} $(TESTCASE_SRCS)
	./${TESTCASE} 192.168.229.133 2000 3
rbtree_testcase_3w:
	$(CC) -o ${TESTCASE} $(TESTCASE_SRCS)
	./${TESTCASE} 192.168.229.133 2000 4
hash_testcase:
	$(CC) -o ${TESTCASE} $(TESTCASE_SRCS)
	./${TESTCASE} 192.168.229.133 2000 5
hash_testcase_1w:
	$(CC) -o ${TESTCASE} $(TESTCASE_SRCS)
	./${TESTCASE} 192.168.229.133 2000 6
hash_testcase_3w:
	$(CC) -o ${TESTCASE} $(TESTCASE_SRCS)
	./${TESTCASE} 192.168.229.133 2000 7
