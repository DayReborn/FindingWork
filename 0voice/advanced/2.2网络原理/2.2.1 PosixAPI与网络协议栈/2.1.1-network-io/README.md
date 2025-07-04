# 2.1.1 Network Io


```
$ gcc -o networkio networkio.c -lpthread
```

```
$ gcc -o websocket websocket.c reactor.c webserver.c -lssl -lcrypto
```


#### ERROR
```
info: relocation truncated to fit: R_X86_64_PC32 against symbol 'begin' defined in COMMON section in /tmp/

solution: compile add flag '-mcmodel=medium'

$ gcc -o reactor reactor.c -mcmodel=medium

```



![image](https://disk.0voice.com/p/nV)
