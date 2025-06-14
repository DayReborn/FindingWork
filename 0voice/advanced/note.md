# 0_前言

> ### **课上能不能直接写到简历**
>
> 1. 项目做到上线
>
> 2. 如何写？
>    	a. 名字自行命名。
>    	b. 用课上的技术，解决现实的具体。
>
>    
>
>    按照这个说法就是：
>
>    > 比如的训练集有很多数据图片，我就可以做图床
>    > 我要存一些我的pdf，论文什么的就可以做kv存储
>
>  ==**简单来说就是高一点使用价值出来**==





---





# 2.1 网络编程



## 2.1.1 网络IO与IO多路复用

### 一、引入

> 1. 使用微信的时候，发送文字，发送视频，发送语音，与网络io什么关系
> 2. 抖音的视频资源，如何到达你的ApP。
> 3. github/gitlab，git clone,为什么能够到达本地
> 4. 共享电动车能够开锁。
> 5. 通过手机操作你家的空调，

![image-20250610235430780](note/image-20250610235430780.png)



```cpp
#include <iostream>
int main()
{
    return 0;
}
```

首先思考这个代码是怎么被执行的（编译）

```
gcc -o test main.c
```



==**客户端与服务端进行通信**==





---





### 二、初始化写一个TCP连接

> Linux与windows映射：samba
> 编写代码：代码编辑器（vscode)
> 编译：gcc/g++
> 运行：ubuntu
> 终端工具ssh:xshell，putty，crt



1.  **代码实现**

```c
#include <sys/socket.h>
#include <errno.h>
#include <netinet/in.h>

#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main()
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY); // 监听所有接口——0.0.0.0
    servaddr.sin_port = htons(4646);              // 监听端口4646，0-1023端口为系统保留端口
    if (-1 == bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)))
    {
        printf("bind error: %s\n", strerror(errno));
        return -1;
    }

    listen(sockfd, 10); // 监听队列长度为10
    getchar(); // 等待用户输入以便查看结果
    
    return 0;
}
```



---



2. **性能查看**

- **正常启动**

![image-20250611003400065](note/image-20250611003400065.png)

```bash
zhenxing@ubuntu:~/share/advanced/2.1.1网络IO与IO多路复用$ netstat -anop | grep 4646
(Not all processes could be identified, non-owned process info
 will not be shown, you would have to be root to see it all.)
tcp        0      0 0.0.0.0:4646            0.0.0.0:*               LISTEN      1699/tcp         off (0.00/0/0)

```



- **同时查看两个的时候：**

![image-20250611003631040](note/image-20250611003631040.png)

会有端口被占用的情况发生。



- **网络调试助手创建连接**

  ![image-20250611004122858](note/image-20250611004122858.png)

![image-20250611004135511](note/image-20250611004135511.png)

连接成功



---



3. **具体的代码解析如下：**

> **1.`socket()` - 创建通信端点**
>
> ```c
> int sockfd = socket(AF_INET, SOCK_STREAM, 0);
> ```
>
> **作用**：创建一个用于网络通信的套接字（类似文件描述符），是后续所有操作的基础。
>
> **参数详解**
>
> - **`AF_INET`**：指定使用IPv4协议族（Address Family）。其他常见值如 `AF_INET6`（IPv6）、`AF_UNIX`（本地通信）。
> - **`SOCK_STREAM`**：指定使用面向连接的TCP协议。特点是有序、可靠、双向字节流。若使用UDP则为 `SOCK_DGRAM`。
> - **`0`**：自动选择协议类型。对于TCP，等价于 `IPPROTO_TCP`；UDP则为 `IPPROTO_UDP`。
>
> **返回值**
>
> - **成功**：返回非负整数，即套接字描述符（类似文件句柄）。
> - **失败**：返回-1，错误码存储在 `errno`中（可通过 `perror`打印）。
>
> **类比**：就像安装了一部电话机，但尚未插线（未绑定地址）或开机（未监听）。
>
> **2. `memset()` - 内存初始化**
>
> ```c
> memset(&serveraddr, 0, sizeof(struct sockaddr_in));
> ```
>
> **作用**：将 `serveraddr`结构体的所有字节置为0，避免残留数据干扰后续操作。
>
> **关键性**：网络编程中结构体可能存在填充字节，未清零可能导致 `bind()`失败。
>
> **参数**
>
> - **目标地址**：`&serveraddr`。
> - **填充值**：`0`（全部字节置0）。
> - **长度**：`sizeof(struct sockaddr_in)`（确保覆盖整个结构体）。
>
> **3. `bind()` - 绑定套接字与地址**
>
> ```c
> bind(sockfd, (struct sockaddr *)&serveraddr, sizeof(struct sockaddr));
> ```
>
> **作用**：将套接字关联到特定的IP地址和端口，使客户端能通过该地址访问服务。
>
> **参数解析**
>
> - **`sockfd`**：由 `socket()`返回的描述符。
> - **`(struct sockaddr *)&serveraddr`**：将 `sockaddr_in`强制转换为通用地址结构体 `sockaddr`（历史原因，需统一接口）。
> - **`sizeof(struct sockaddr)`**：地址结构体的大小。此处有潜在问题（应使用 `sizeof(serveraddr)`）。
>
> **地址结构体字段**
>
> - **`sin_family`**：必须与 `socket()`的地址族一致（此处为 `AF_INET`）。
> - **`sin_addr.s_addr`**：IP地址（`INADDR_ANY`表示监听所有本地网卡）。
> - **`sin_port`**：端口号（需用 `htons()`转换为网络字节序）。
>
> **常见错误**
>
> - **`EADDRINUSE`**：端口被占用。
> - **`EACCES`**：绑定到特权端口（<1024）无root权限。
>
> **类比**：为电话机分配电话号码（IP+端口），允许他人拨打。
>
> **4. `listen()` - 开启监听模式**
>
> ```c
> listen(sockfd, 10);
> ```
>
> **作用**：将套接字设置为被动模式，准备接受客户端的连接请求。
>
> **参数详解**
>
> - **`sockfd`**：已绑定地址的套接字。
> - **`10`**：内核维护的未完成连接队列的最大长度（实际值可能被系统调整，如Linux默认为128）。
>
> **队列类型**
>
> - **未完成队列（SYN队列）**：客户端发送SYN后处于 `SYN_RCVD`状态。
> - **已完成队列（ACCEPT队列）**：已完成三次握手，等待 `accept()`取出。
>
> **返回值**：成功返回0，失败返回-1（需检查错误）。
>
> **类比**：电话机开机并设置为响铃模式，等待来电。
>
> **5. `getchar()` - 阻塞程序退出**
>
> ```c
> getchar(); // 等待用户输入
> ```
>
> **作用**：防止程序立即结束，保持服务器运行以便测试。
>
> **实际应用**：真实服务器应使用事件循环（如 `while(1)` + `accept()`）持续处理请求。
>
> **潜在问题**：此处仅为演示，无实际连接处理逻辑。





---





### 三、添加接收功能

> **前面有一些注意事项**
>
> 1. 端口被绑定以后，不能再次被绑定。
> 2. 执行listen，可以通过netstat看到io的状态。
> 3. 进入listen可以被连接，并且会产生新连接状态。
> 4. io与tcp连接



```c
struct sockaddr_in clientaddr;
    socklen_t len = sizeof(clientaddr);

    // 当我们运行代码的时候首先会阻塞在这个地方，而不是阻塞在下面的getchar();
    printf("accepting...\n");
    int clientfd = accept(sockfd, (struct sockaddr *)&clientaddr, &len);

    printf("Accept finished!\n");
    printf("[Sockfd:%d, Clientfd:%d]\n", sockfd, clientfd);

    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));

    // 接下来会阻塞在recv函数上，等待客户端发送数据
    printf("Recv data...\n");
    int count = recv(clientfd, buffer, 1024, 0);
    printf("-recv count: %d\n", count);
    printf("-recv data: %s\n", buffer);

    count = send(clientfd, "Hello, client!", 15, 0);
    printf("-send count: %d\n", count);
    printf("-send data: Hello, client!\n");

```



---



这部分就是最简单的tcp的连接和发送信息的流程：

> 简单解析一下这部分的话
>
>  **1.`struct sockaddr_in clientaddr`**
>
> - **作用**：定义一个IPv4地址结构体，用于存储**客户端**的连接信息（IP和端口）。
> - 关键字段：
>   - `sin_family`：地址族（自动填充为 `AF_INET`）。
>   - `sin_port`：客户端的端口号（网络字节序）。
>   - `sin_addr`：客户端的IP地址（可通过 `inet_ntoa()`转换为字符串）。
>
> ---
>
> **2. `socklen_t len = sizeof(clientaddr)`**
>
> - **作用**：声明一个变量表示地址结构体的大小，供 `accept()`函数使用。
> - **传递方式**：必须传递指针（`&len`），因为 `accept()`可能修改此值以反映实际写入的地址大小。
>
> ---
>
> **3. `accept()` - 接受客户端连接**
>
> ```c
> int clientfd = accept(sockfd, (struct sockaddr *)&clientaddr, &len);
> ```
>
> - **作用**：从监听队列中取出一个已完成的连接，并返回一个新的套接字描述符（`clientfd`）。
> - 参数详解：
>   - **`sockfd`**：监听套接字（由 `socket()`创建并通过 `bind()`+`listen()`启用）。
>   - **`(struct sockaddr *)&clientaddr`**：存储客户端地址信息的结构体（需强制类型转换）。
>   - **`&len`**：输入时为地址结构体大小，输出时为实际写入的地址大小。
>
> - 返回值：
>   - **成功**：返回一个新的套接字描述符（`clientfd`），用于与客户端通信。
>   - **失败**：返回-1（需检查 `errno`）。
> - **阻塞机制**：若监听队列中没有已完成的连接，`accept()`会阻塞直到有客户端连接。
> - **类比**：电话接线员从等待队列中接听一个来电，并为该通话分配专用线路（`clientfd`）。
>
> ---
>
> **4. `recv` 函数深度解析**
>
> ```c
> int count = recv(clientfd, buffer, 128, 0);
> ```
>
> **函数作用**
>
> - 从已连接的套接字（`clientfd`）接收数据，将数据存入 `buffer`。
> - **类比**：类似于从水管（连接）中读取水流（数据）。
>
> ---
>
> **参数详解**
>
> |    参数    |                             说明                             |
> | :--------: | :----------------------------------------------------------: |
> | `clientfd` |           已连接的套接字描述符（由 `accept` 返回）           |
> |  `buffer`  |                     存储接收数据的缓冲区                     |
> |   `128`    |                  缓冲区最大容量（避免溢出）                  |
> |    `0`     | 标志位（常见选项：`MSG_WAITALL`阻塞收满数据，`MSG_DONTWAIT`非阻塞） |
>
> ---
>
> **返回值意义**
>
> |  返回值  |                   含义                    |
> | :------: | :---------------------------------------: |
> | **`>0`** | 实际接收的字节数（可能小于请求的128字节） |
> | **`0`**  |    对方已关闭连接（需关闭 `clientfd`）    |
> | **`-1`** |          出错（需检查 `errno`）           |



---



这边的连接运行没有问题：

![image-20250611011607533](note/image-20250611011607533.png)



但是就是不支持多客户端的连接





---



### 四、添加循环调用多个客户端

这边用最简单的方式来做

```c
#elif 1
    while(1)
    {
        printf("accepting...\n");
        clientfd = accept(sockfd, (struct sockaddr *)&clientaddr, &len);
        printf("Accept finished!\n");
        printf("[Sockfd:%d, Clientfd:%d]\n", sockfd, clientfd);

        char buffer[1024];
        memset(buffer, 0, sizeof(buffer));

        // 接下来会阻塞在recv函数上，等待客户端发送数据
        printf("Recv data...\n");
        int count = recv(clientfd, buffer, 1024, 0);
        if (count <= 0)
        {
            printf("Client disconnected or error occurred.\n");
            break;
        }
        printf("-recv count: %d\n", count);
        printf("-recv data: %s\n", buffer);

        count = send(clientfd, "Hello, client!", 15, 0);
        printf("-send count: %d\n", count);
        printf("-send data: Hello, client!\n");
    }

#endif
```



> 在TCP连接模型中，客户端连接请求（connect）和服务器的`accept()`是两个独立的过程，你的代码中循环处理连接的方式是关键。让我详细解释流程和概念：
>
> ### 核心概念
>
> 1. **连接队列**：
>    - 当客户端发起连接时（`connect()`），服务器内核会完成三次握手
>    - 完成握手的连接会进入**已完成连接队列**（代码中`listen(sockfd, 10)`设置队列长度为10）
>    - 此时连接已建立，但尚未被应用程序处理
> 2. **`accept()`的作用**：
>    - 从已完成队列中取出一个连接
>    - 返回新的套接字(`clientfd`)用于与客户端通信
>    - **不是**创建新连接，而是"取出"已建立的连接
>
> ### 你的代码流程
>
> ```
> while(1) {
>     // 阻塞点1：等待新连接到来
>     clientfd = accept(...);  // 步骤A：从队列取连接
>     
>     // 阻塞点2：等待该连接发数据
>     recv(clientfd, ...);     // 步骤B：处理具体连接
>     send(clientfd, ...);
> }
> ```
>
> ### 工作场景模拟
>
> 假设有两个客户端（ClientA, ClientB）同时连接：
>
> 1. **时序场景**：
>
>    ```
>    时刻0: ClientA连接 -> 进入队列
>    时刻1: accept()取出ClientA
>    时刻2: 执行到recv()阻塞（等待ClientA发数据）
>    
>    时刻3: ClientB连接 -> 进入队列
>    （此时recv仍在阻塞，但ClientB的连接已在队列中）
>    
>    时刻4: ClientA发送数据 -> recv()返回
>    时刻5: send()响应后循环回到accept()
>    时刻6: accept()立即取出ClientB
>    ```
>
> 2. **关键观察**：
>
>    - **连接成功与accept分离**：ClientB在时刻3已完成连接，即使此时服务器卡在`recv`
>    - **队列的作用**：ClientB的连接被暂存在内核队列中（最长存放10个连接）
>    - **非阻塞处理**：服务器代码在`recv`阻塞时，仍能接收新连接（只是暂不处理）
>
> ### 为什么不会丢失连接
>
> ```mermaid
> graph TD
>     subgraph 内核空间
>         Queue[已完成连接队列 max=10]
>     end
>     
>     subgraph 用户空间
>         accept -->|取出连接| Queue
>         recv -->|操作连接| clientfd
>     end
>     
>     Client1 -->|connect| Queue
>     Client2 -->|connect| Queue
>     Client3 -->|connect| Queue
> ```
>
> - 只要队列未满（<10个连接），新连接都能立即成功
> - `accept()`只是从队列取连接，不参与建立连接的过程
> - 当阻塞在`recv`时，内核仍在独立维护连接队列
>
> ### 关键结论
>
> 1. **连接建立不依赖`accept()`**：客户端`connect()`成功时，连接已在内核中建立
> 2. **`accept()`本质是出队操作**：从内核管理的队列取出已建立连接
> 3. **阻塞位置的影响：**
>    - 阻塞在`accept()`：说明队列空（无新连接）
>    - 阻塞在`recv()`：连接已建立但等待数据（此时队列可继续收新连接）
> 4. **队列满时的行为**：当队列满时（已有10个完成连接），新连接会被拒绝







### 五、多线程方式解决上述难点

```cc
#elif 1
    while(1)
    {
        printf("accepting...\n");
        clientfd = accept(sockfd, (struct sockaddr *)&clientaddr, &len);
        printf("Accept finished!\n");
        printf("[Sockfd:%d, Clientfd:%d]\n", sockfd, clientfd);

        pthread_t thread_id;
        if (pthread_create(&thread_id, NULL, client_thread, &clientfd) != 0)
        {
            printf("Failed to create thread: %s\n", strerror(errno));
            close(clientfd);
            continue; // 如果线程创建失败，继续等待下一个连接
        }
        
    }
```

多线程部分代码

```c
void *client_thread(void *arg)
{
    int clientfd = *(int *)arg; // 从参数中获取客户端文件描述符

    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));

    while (1)
    {
        // 接下来会阻塞在recv函数上，等待客户端发送数据
        printf("[Clientfd:%d]-Recving data...\n", clientfd);
        int count = recv(clientfd, buffer, 1024, 0);
        if (count <= 0)
        {
            printf("Client disconnected or error occurred.\n");
            close(clientfd); // 关闭客户端连接
            printf("[Clientfd:%d]-Connection closed.\n", clientfd);
            pthread_exit(NULL); // 退出线程
        }
        printf("[Clientfd:%d]-recv count: %d\n", clientfd, count);
        printf("[Clientfd:%d]-recv data: %s\n", clientfd, buffer);

        count = send(clientfd, "Hello, client!", 15, 0);
        printf("[Clientfd:%d]-send count: %d\n",clientfd, count);
        printf("[Clientfd:%d]-send data: Hello, client!\n",clientfd);
    }
}
```

![image-20250611220134535](note/image-20250611220134535.png)





---





## 2.1.2 事件驱动reactor的原理与实现

### 一、不使用多线程

首先明确一下  我们的fd的话，一开始的时候是被占用的：

**linux中的很多操作都是通过==fd==实现的**



![image-20250611222701232](note/image-20250611222701232.png)





> **需要实现的目标：**
>
> 1. select/poll/epoll
> 2. reactor.c

![image-20250611234713228](note/image-20250611234713228.png)



首先我们要分析为什么不使用多线程的方法。

> 创建多线程的话，假设一个线程消耗8M内存，有1w个客户端同时进行连接，我们消耗的缓冲区就已经是80G了，这显然是不可以接受的!

那我们可以使用一个线程的方式来解决问题吗？

==可以！使用select、poll、epoll！==





---

### 二、使用select方法

> 网络IO
>
> 1. accept --> listenfd
> 2. recv/send --> clientfd、
>
> 我们的代码也就是要按照这样的方法来写



代码实现

```c
#elif 1 // select方法
    // @param maxfd: 设置内部循环的最大fd编号
    // @param rset: fd_set 本质是 1024 位的位图，用来标志这个io是否可读
    // @param wset: fd_set，用来标志这个io是否可写
    // @param eset: fd_set，用来标志这个io是否可读
    // @param timeout: 超时控制
    // *int nready = select(maxfd, rset, wset,eset,timeout)这个函数需要五个参数

    fd_set rfds, rset;
    FD_ZERO(&rfds);
    FD_SET(sockfd, &rfds);

    int maxfd = sockfd; // 最大文件描述符
    while (1)
    {
        rset = rfds;                                             // rset是rfds的副本，select会修改rset，所以每次循环都需要重新赋值
        // nready表示有多少个文件描述符就绪, +1是因为select的第一个参数是最大文件描述符加1
        // select会阻塞，直到有文件描述符就绪或者超时
        // timeout参数为NULL表示无限等待，直到有文件描述符就绪
        printf("Waiting for select...\n");
        int nready = select(maxfd + 1, &rset, NULL, NULL, NULL);
        if (FD_ISSET(sockfd, &rset))
        {
            struct sockaddr_in clientaddr;
            socklen_t len = sizeof(clientaddr);
            memset(&clientaddr, 0, len); // 清空客户端地址结构体

            int clientfd = accept(sockfd, (struct sockaddr *)&clientaddr, &len);

            FD_SET(clientfd, &rfds);
            printf("New connection accepted: [Sockfd:%d, Clientfd:%d]\n", sockfd, clientfd);

            if (maxfd < clientfd)
                maxfd = clientfd;
        }

        int i = 0;
        for (i = sockfd + 1; i <= maxfd; ++i)
        {
            if (FD_ISSET(i, &rset))
            {
                char buffer[1024] = {0};
                int count = recv(i, buffer, 1024, 0);
                if (count == 0)
                {
                    // 客户端断开连接
                    printf("Clientfd:%d disconnected\n", i);
                    // !不关闭事件会一直循环循环
                    // close(i);
                    FD_CLR(i, &rfds);
                    close(i);

                    continue;
                }
                send(i, buffer, count, 0);
                printf("Clientfd %d\ncount: %d\nbuffer: %s\n", i, count, buffer);
            }
        }
    }
```



==**注意select方法仅适用于兼容性要求高的场景。**==

> select():
>
> 1. connect, select返回

> select()的缺点是什么：
>
> 1. 参数量五个很多
> 2. 每次都需要把待检测的io集合rset完全拷贝进内核
> 3. 对io的数量是有限制的。

> 性能缺陷分析：
>
> 1. io集合rset完全拷贝
> 2. io需要从头到尾遍历



---



### 三、使用poll方法

* 其实简单讲讲就是poll和select函数并没有区别，只是两者的接口不一样。


**==其实写法跟select几乎一样！！！！==**



```c
#elif 1
    // poll方法
    struct pollfd fds[1024] = {0};

    fds[sockfd].fd = sockfd;
    fds[sockfd].events = POLLIN;
    int maxfd = sockfd;
    while (1)
    {
        int nready = poll(fds, maxfd + 1, -1);
        if (fds[sockfd].revents & POLLIN)
        {
            struct sockaddr_in clientaddr;
            socklen_t len = sizeof(clientaddr);
            int clientfd = accept(sockfd, (struct sockaddr *)&clientaddr, &len);

            printf("New connection accepted: [Sockfd:%d, Clientfd:%d]\n", sockfd, clientfd);
            fds[clientfd].fd = clientfd;
            fds[clientfd].events = POLLIN;

            maxfd = clientfd;
        }
        int i = 0;
        for (i = sockfd + 1; i <= maxfd; ++i)
        {
            if (fds[i].revents & POLLIN)
            {
                char buffer[1024] = {0};
                int count = recv(i, buffer, 1024, 0);
                if (count == 0)
                {
                    printf("Clientfd:%d disconnected\n", i);
                    // !不关闭事件会一直循环循环
                    fds[i].fd = -1;
                    fds[i].events = 0;
                    close(i);

                    continue;
                }
                send(i, buffer, count, 0);
                printf("clientfd: %d\ncount: %d\nbuffer: %s\n", i, count, buffer);
            }
        }
    }
```

c

> ### poll 系统调用详解
>
> poll 是 Linux/Unix 系统中实现 I/O 多路复用的一种机制，用于同时监控多个文件描述符的状态变化，尤其适合网络编程中处理多连接场景。
>
> #### poll 的核心数据结构
>
> #### `struct pollfd`
>
> ```c
> #include <poll.h>
> 
> struct pollfd {
>     int   fd;         // 要监控的文件描述符
>     short events;     // 请求监控的事件（位掩码）
>     short revents;    // 实际发生的事件（位掩码）
> };
> ```
>
> 
>
> #### 核心函数
>
> #### `poll()` 函数
>
> ```c
> int poll(struct pollfd *fds, nfds_t nfds, int timeout);
> ```
>
> 参数说明：
>
> - `cfds`: `struct pollfd` 数组指针
> - `nfds`: 数组中文件描述符的数量
> - `timeout`: 超时时间（毫秒）
>   - `-1`：永久阻塞直到事件发生
>   - `0`：立即返回（非阻塞）
>   - `>0`：等待指定毫秒数
>
> 返回值：
>
> - `>0`：就绪的文件描述符数量
> - `0`：超时
> - `-1`：出错（errno 被设置）
>
> #### 事件标志位
>
> | 事件标志  |             描述              |
> | :-------: | :---------------------------: |
> |  POLLIN   | 有数据可读（包括新连接到来）  |
> |  POLLPRI  |  紧急数据可读（如带外数据）   |
> |  POLLOUT  |         可以写入数据          |
> | POLLRDHUP | 对端关闭连接（Linux 2.6.17+） |
> |  POLLERR  |           发生错误            |
> |  POLLHUP  |           对端挂断            |
> | POLLNVAL  |       文件描述符未打开        |





---





### 四、使用epoll方法

![image-20250612021056340](note/image-20250612021056340.png)

**简单来说epoll使得linux开始被用为服务器！！！！**

```c
#elif 1
    // 使用epoll方法
    // create只要参数不为0和负即可
    int epfd = epoll_create(1);

    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = sockfd;
    epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &ev);  // 将sockfd添加到epoll中
    printf("Epoll created and sockfd added!\n");
    printf("Epollfd: %d\n", epfd);
    printf("Sockfd: %d\n", sockfd);
    printf("Waiting for connections...\n");

    struct epoll_event events[1024] = {0};
    while (1)
    {
        int nready = epoll_wait(epfd, events, 1024, -1);
        int i = 0;
        for (i = 0; i < nready; ++i)
        {
            int connfd = events[i].data.fd;
            if (sockfd == connfd)
            {
                struct sockaddr_in clientaddr;
                socklen_t len = sizeof(clientaddr);

                int clientfd = accept(sockfd, (struct sockaddr *)&clientaddr, &len);
                printf("New connection accepted: [Sockfd:%d, Clientfd:%d]\n", sockfd, clientfd);

                ev.events = EPOLLIN;
                ev.data.fd = clientfd;
                epoll_ctl(epfd, EPOLL_CTL_ADD, clientfd, &ev);

                printf("sockfd: %d\n", clientfd);
            }
            else if (events[i].events & EPOLLIN)
            {
                char buffer[128] = {0};
                int count = recv(connfd, buffer, 128, 0);
                if (count == 0)
                {
                    printf("Clientfd:%d disconnected\n", connfd);
                    // !不关闭事件会一直循环循环

                    epoll_ctl(epfd, EPOLL_CTL_DEL, connfd, NULL);// 从epoll中删除事件
                    close(connfd);

                    continue;
                }
                send(connfd, buffer, count, 0);
                printf("[Clientfd: %d, Count: %d]\nbuffer: %s\n", connfd, count, buffer);
            }
        }
    }
```



> ==**相比较select而言，对于大并发优势在哪里？**==
>
> 1. 100w
> 2. 积累起来的，就绪就ok了
> 3. 就绪是我们需要事件.



> 下面是一些关键函数的注解：
>
> 1. `epoll_create`
>
> ```c
> int epfd = epoll_create(1);
> ```
>
> * **作用** ：创建epoll实例
> * **参数** ：
>   * `size`：历史遗留参数，现代内核忽略该值（>0即可）
>
> * **返回值** ：epoll文件描述符
>
> 2. `epoll_ctl`
>
> ```c
> epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &ev);
> ```
>
> * **作用** ：管理epoll监控列表
> * **参数** ：
>   * `op`：操作类型（ADD/MOD/DEL）
>   * `fd`：待监控的文件描述符
>   * `event`：关联的事件结构体
>
> * **事件类型** ：
>   * `EPOLLIN`：数据可读
>   * `EPOLLOUT`：数据可写
>   * `EPOLLET`：边缘触发模式
>
>
> 3. `epoll_wait`
>
> ```c
> int nready = epoll_wait(epfd, events, 1024, -1);
> ```
>
> * **作用** ：等待事件发生
> * **参数** ：
> * `events`：输出参数，存储就绪事件
> * `maxevents`：最大接收事件数（需<=数组长度）
> * `timeout`：-1表示阻塞等待，0立即返回，>0超时时间(ms)
>





> ==**接下来是个重点！！！**==
>
> **水平触发(LT)**以及**边沿触发(ET)**的区别：
>
> - 水平触发LT：只要IO之中还有数据，他就会一直接收，一直触发，然后分条直到全部输出为止
>
>   - > 输入：01234567890123456789
>     >
>     > 输出：count: 15
>     >
>     > ​	    buffer: 012345678901234
>     >
>     > ​	    count: 5
>     >
>     > ​	    buffer: 56789
>
> - 边沿触发ET：即使IO中还有数据，他也只会读一次，剩下的数据存着，直到下一次有数据输入进来（==理论上可能越积越多==）
>
>   - > 输入：01234567890123456789
>     >
>     > 输出：count: 15
>     >
>     > ​	    buffer: 012345678901234
>     >
>     > 输入：111012345
>     >
>     > 输出：count: 14
>     >
>     > ​	    buffer: 56789111012345
>
> - 具体应用场景来说：
>
>   - **LT触发的话**：在tcp的情况下可以设置接受的长度；甚至可以==分包==——截成不同的段落。
>
>   ![image-20250306002224480](note/image-20250306002224480.png)
>
>   -  **ET触发的用处**：
>
>   ![image-20250306002811957](note/image-20250306002811957.png)
>
>   - 如果要将一个大小为2g的文件一口度进来，就这样用while  全都进来。





### 五、思考

==思考！！！==

#### 0. 整集和就绪集合用什么数据结构存储



![image-20250612023218299](note/image-20250612023218299.png)



> # epoll 数据结构详解
>
> ## 1. epoll 的整集存储数据结构
>
> epoll 使用高效的内核数据结构来管理监控的文件描述符集合：
>
> ### **红黑树 (Red-Black Tree)**
>
> - **存储位置**：内核空间
>
> - **数据结构类型**：自平衡二叉搜索树
>
> - **存储内容：**
>
>   ```
>   struct epitem {
>       struct rb_node rbn;       // 红黑树节点
>       struct list_head rdllink; // 就绪队列链表节点
>       struct epoll_filefd ffd; // 包含 fd + file* 指针
>       struct eventpoll *ep;     // 所属的epoll实例
>       struct epoll_event event; // 监控的事件设置
>       // ...
>   };
>   ```
>
> - 特性：
>
>   - O(log n) 时间复杂度的插入、删除和查找操作
>   - 通过 `epoll_ctl(EPOLL_CTL_ADD)` 添加的新描述符都会插入红黑树
>   - 每个 fd 对应一个 epitem 节点
>
> ### **设计优势**
>
> 1. **高效查询**：对数时间复杂度确保万级连接下仍保持高效
> 2. **内核管理**：所有操作在内核完成，避免用户-内核空间频繁拷贝
> 3. **动态扩展**：连接数不受限制，只受系统文件描述符上限约束
>
> 
>
> ## 2. epoll 的就绪集合数据结构
>
> ### **就绪队列 (Ready List)**
>
> - **数据结构**：双向链表 (linked list)
>
> - 内核实现：
>
>   ```
>   struct eventpoll {
>       // 红黑树根节点（存储所有监控项）
>       struct rb_root_cached rbr;
>       
>       // 就绪队列（双向链表）
>       struct list_head rdllist;
>       
>       // 等待队列
>       wait_queue_head_t wq;
>       // ...
>   };
>   ```
>
> - 工作流程：
>
>   1. 当事件发生时，内核将对应的 epitem 加入 `rdllist`
>   2. `epoll_wait()` 调用时，内核拷贝就绪事件的 `epoll_event` 结构到用户空间
>   3. 就绪链表被清空，等待新事件
>
> ### **用户空间获取接口**
>
> ```
> int epoll_wait(int epfd, struct epoll_event *events,
>                int maxevents, int timeout);
> ```
>
> - `events`：用户预分配的缓冲区
> - `maxevents`：每次调用最多返回的事件数量
> - 关键特点：
>   - 仅拷贝就绪事件（**O(1) 事件复杂度**）
>   - 返回实际就绪事件数 `n`，用户只需遍历 `0` 到 `n-1`
>   - 避免全量扫描（select/poll 的 O(n) 问题）
>
> ## 三种 I/O 多路复用对比
>
> | 特性                 | select           | poll             | epoll            |
> | -------------------- | ---------------- | ---------------- | ---------------- |
> | **整集数据结构**     | 固定大小位图     | 结构体数组       | 内核红黑树       |
> | **整集最大数量**     | FD_SETSIZE(1024) | 无硬限制         | 系统文件限制     |
> | **就绪集合存储**     | 覆写传入的位图   | 修改结构体数组   | 内核维护链表     |
> | **就绪集合获取方式** | 遍历检查位图     | 遍历检查结构体   | 内核填充用户数组 |
> | **时间复杂度**       | O(n)             | O(n)             | O(1)事件通知     |
> | **内存拷贝**         | 每次调用完整拷贝 | 每次调用完整拷贝 | 仅就绪事件拷贝   |
> | **事件注册**         | 每次调用重新设置 | 每次调用重新设置 | 一次注册永久有效 |
>
> ## epoll 底层原理深度解析
>
> ### 1. 高效的就绪通知机制
>
> - **回调函数注册**：通过 `epoll_ctl()` 向内核注册回调
> - 事件触发流程：
>   1. 网卡收到数据 → 触发硬件中断
>   2. 内核网络栈处理数据包
>   3. 将对应 socket 标记为就绪
>   4. 调用 `ep_poll_callback()` 将 epitem 加入就绪队列
>   5. 唤醒等待的进程
>
> ### 2. 水平触发 (LT) vs 边沿触发 (ET)
>
> ```
> // ET模式设置
> ev.events = EPOLLIN | EPOLLET;
> ```
>
> - **水平触发 (默认)**：
>   - 只要缓冲区有数据，每次 `epoll_wait()` 都返回
>   - 类似 select/poll 行为
> - **边沿触发**：
>   - 仅在状态变化时通知一次
>   - 需要一次性读取所有数据（循环读取直到 EAGAIN）
>   - 减少系统调用次数，提高性能
>
> ### 3. epoll 系统调用分析
>
> | 系统调用     | 操作                 | 内核操作                        |
> | ------------ | -------------------- | ------------------------------- |
> | epoll_create | 创建 epoll 实例      | 分配 eventpoll 结构体           |
> | epoll_ctl    | 添加/修改/删除监控项 | 操作红黑树 (插入/删除/更新节点) |
> | epoll_wait   | 等待事件             | 检查就绪队列，无事件则休眠      |
>
> epoll 的这种设计使其能够轻松应对 C10K 甚至 C100K 问题，是现代高性能网络服务器的基石。





#### 1. **`epoll` 是否使用 `mmap`？**

> **答案：是的，`epoll` 内部使用 `mmap` 优化性能。**
>
> 具体机制：
>
> - **内核与用户空间的数据传递**：
>   `epoll` 通过 `mmap` 在内核和用户空间之间**共享内存**，避免数据从内核态到用户态的拷贝（零拷贝技术）。当调用 `epoll_wait` 时，事件数据直接通过 `mmap` 映射的内存区域返回，无需传统 `read`/`write` 的多次拷贝。
> - **性能优势**：
>   这种设计使得 `epoll` 在处理大量文件描述符时效率远高于 `select`/`poll`，尤其是高并发场景（如百万级连接）。
>
> 验证方法：
>
> - 通过
>
>   ```
>   strace
>   ```
>
>   跟踪系统调用可以发现，
>
>   ```
>   epoll_create
>   ```
>
>   时会调用
>
>   ```
>   mmap
>   ```
>
>   ```bash
>   strace -e trace=mmap,epoll_create your_program
>   ```
>





> ------
>





#### 2. **`epoll` 是否线程安全？**

> **答案：`epoll` 本身是线程安全的，但需要合理使用。**
>
> 详细说明：
>
> - **线程安全的操作**：
>
>   - **`epoll_wait`**：
>     多个线程可以同时调用 `epoll_wait` 监听同一个 `epoll` 实例，内核会保证事件分发的原子性。
>   - **`epoll_ctl`**：
>     对同一个 `epoll` 实例的修改操作（如 `EPOLL_CTL_ADD`/`EPOLL_CTL_MOD`/`EPOLL_CTL_DEL`）需要**加锁保护**，否则可能引发竞争条件。
>
> - **非线程安全的场景**：
>   如果多个线程同时对同一个文件描述符调用 `epoll_ctl`，可能导致不可预知的行为。例如：
>
>   ```c
>   // 线程1：添加事件
>   epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev);
>   
>   // 线程2：删除事件
>   epoll_ctl(epfd, EPOLL_CTL_DEL, fd, NULL);
>   ```
>
>   若不加锁，这两个操作可能在内核中交错执行，导致状态不一致。
>
> 正确用法：
>
> - 使用互斥锁保护 `epoll_ctl`：
>
>   ```c
>   pthread_mutex_lock(&epoll_mutex);
>   epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev);
>   pthread_mutex_unlock(&epoll_mutex);
>   ```
>
> - **每个线程使用独立的 `epoll` 实例**：
>   如果不需要共享事件监听，可以为每个线程创建独立的 `epoll` 实例，避免竞争。
>
> ------
>
> 代码示例：线程安全的 `epoll` 使用
>
> ```c
> #include <pthread.h>
> #include <sys/epoll.h>
> 
> #define MAX_EVENTS 10
> 
> int epfd;
> pthread_mutex_t epoll_mutex = PTHREAD_MUTEX_INITIALIZER;
> 
> void* thread_func(void* arg) {
>     struct epoll_event events[MAX_EVENTS];
>     while (1) {
>         // 多个线程可以安全调用 epoll_wait
>         int nready = epoll_wait(epfd, events, MAX_EVENTS, -1);
>         for (int i = 0; i < nready; i++) {
>             // 处理事件
>         }
>     }
> }
> 
> void add_fd_to_epoll(int fd) {
>     struct epoll_event ev;
>     ev.events = EPOLLIN;
>     ev.data.fd = fd;
>     
>     // 加锁保护 epoll_ctl
>     pthread_mutex_lock(&epoll_mutex);
>     epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev);
>     pthread_mutex_unlock(&epoll_mutex);
> }
> ```
>





> ------
>





#### 3. **补充：ET 模式下的线程安全问题**

> - **边沿触发（ET）模式**：
>   在 ET 模式下，必须确保一次性读取所有数据（循环调用 `recv` 直到返回 `EAGAIN`）。如果多个线程处理同一个 socket 的 `EPOLLIN` 事件，可能导致数据被多个线程重复处理。此时需要设计合理的任务分发机制（如 Leader-Follower 模式）。
> - **非阻塞 IO 的必要性**：
>   ET 模式下必须将 socket 设为非阻塞模式，否则 `recv` 可能阻塞整个线程，影响其他事件处理。
>
> ------
>
> 4. `epoll`实现方式划分
>
> ![image-20250306010104190](note/image-20250306010104190.png)
>
> 分析：
>
> - 左边明显是**面向IO**的写法
>   - 会越来越累赘，越来越复杂
> - 右边是**面向事件**的写法
>   - 这样的写法本质上就是==reactor==
>   - 核心的作用相较于面向IO的写法





#### 4. 我们目前的做法是否有什么不妥：

![image-20250612024141545](note/image-20250612024141545.png)

==**简单来说我们还是io事件触发**==

> io的生命周期，无数多个事件组成
>
> 而作为一个server端，7*24工作
>
> ​	**我们应该是针对事件 --> 执行不同的回调函数**
>
> 这样更符合我们的业务开发 --> 这就是==**reactor模式！！！**==
>
> ![image-20250612024612638](note/image-20250612024612638.png)
>
> 当有事情发生的时候！！！
>
> 我们就调用某个回调函数！！！
>
> 由**IO管理**变为**事件管理**







---







## 2.1.3服务器百万并发实现

### 一、引入

> **我们今天的主要任务：**
>
> 1. 实现reactor模式
> 2. 实现百万并发



> 简单来说就是：
>
> **不同的io事件，对应不同的回调**
>
> 1. register
> 2. callback
>
> ![image-20250612121944251](note/image-20250612121944251.png)





![image-20250612122457027](note/image-20250612122457027.png)



### 二、reactor代码实现

