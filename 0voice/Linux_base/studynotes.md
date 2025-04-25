# Linux环境专栏

## 一、Linux环境编程项目精讲介绍

分为下面几个部分



---



### Linux开发环境

- 项目目的：了解Linux环境搭建，了解LinuxC编程
- 项目知识点：Linux安装，命令使用，shell编程
- 项目：shell脚本实现检测局域网内哪些ip地址机器宕机
- 推荐书籍：鸟哥私房菜
- 课后作业题：Shell脚本实现获取本机ip地址



---



### Linux C编程

#### **1. LinuxC编程统计文件单词数量（文件操作）**

- 项目目的：了解Linux编程环境搭建，了解Linux C编程
- 项目知识点：文件操作，文件指针，
- 推荐书籍：嗨翻c语言
- 课后练习题：**统计每个单词出现的次数**



#### **2. LinuxC编程实现通讯录（结构体）**

- 项目目的：了解结构体定义，数据结构
- 项目知识点：链表操作，添加节点，遍历，查找
- 推荐书籍：大话数据结构
- 课后练习题：**按照姓名首字母存储通讯录，使用数组加链表**



---



### Linux环境编程

#### **3.并发下的计数方案（锁）**

- 项目目的：了解并发，了解计数方案
- 项目知识点：互斥锁，自旋锁，原子操作
- 推荐书籍：Unix高级环境编程
- 课后练习题：了解CAS，并实现CAS



#### **4.CPU与进程的关系（进程与进程通信）**

- 项目目的：了解进程，进程间通信共享内存，管道，sockpair
- 项目知识点：进程操作，进程与CPU粘合，进程间通信
- 推荐书籍：Unix高级环境编程
- 课后练习题：**实现一个进程间通信组件**



#### **5.实现线程池（线程）**

- 项目目的：了解线程，了解线程加锁，互斥锁mutex，条件变量cond
- 项目知识点：线程队列，任务队列，互斥锁，条件变量
- 推荐书籍：Unix高级环境编程
- 课后练习题：**对线程池的数量进行控制**



#### **6.数据库操作**

- 项目目的：了解数据库操作，程序代码操作
- 项目知识点：数据库封装，sql语句封装，网络连接封装
- 推荐书籍：MySQL高效编程
- 课后练习题：**封装一个数据库连接池**



---



### 网络编程

#### **7.DNS请求器器（UDP编程）**

- 项目目的：了解UDP通信，了解DNS协议
- 项目知识点：UDP通信，DNS协议，协议解析
- 推荐书籍：DNS与BIND
- 课后练习题：实现异步DNS



#### **8.实现http请求器（TCP客户端）**

- 项目目的：了解TCP通信，了解HTTP协议
- 项目知识点：TCP编程，HTTP请求协议
- 推荐书籍：图解HTTP
- 课后练习题：实现异步HTTP请求



#### **9.百万级并发服务器（TCP服务器）**

- 项目目的：网络I0，服务器后端编程
- 知识点：tcp，网络io，Linux系统
- 推荐书籍：Linux高性能服务器原理内参
- 课后练习题：测试搭建百万并发服务器



---



### 项目实战

#### **10.实现网络爬虫**

- 项目目的：了解爬虫产品开发，功能分解，产品架构
- 项目功能：Page下载器，Page分析器，调度器
- 依赖库：pcre，libxml2，liburi，libuv，curl
- 课后练习题：实现求职网站上面，职位爬取



---



## 二、Linux系统安装

没什么好说的VMware17安装，以及安装了Ubuntu16.0的系统。

> 用户名：zhenxing
>
> 密码：123456





---



## 三、Linux开发环境ssh与Samba配置

### 1. 安装ssh

按照视频，老版本可能安装好了ssh，但是这边没有装。

所以手动安装如下：

```bash
# 0. 更新
sudo apt-get update

# 1. 安装SSH服务
sudo apt-get install openssh-server

# 2. 启动SSH并开放防火墙
sudo service ssh start
sudo ufw allow 22/tcp

# 3. 检查SSH配置
sudo nano /etc/ssh/sshd_config

# 4. 测试网络连通性（从主机）
ping 192.168.5.128
```



---



### 2. 安装samba

首先是第一步安装samba

```bash
# 0. 更新
sudo apt-get update

# 1. 安装samba
sudo apt-get install samba

# 2. 安装vim
sudo apt-get install vim
```



第二步配置相关文件

```bash
# 1. 配置共享文件夹
mkdir share 
sudo chmod 777 share/ -R

# 2. 修改配置文件
sudo vim /etc/samba/smb.conf
	#添加
	[share]
        comment = My Samba
        path = /home/zhenxing/share
        browseable =yes
        writeable=yes

# 3. 激活配置文件
sudo smbpasswd -a zhenxing

```



回到windows电脑中，在文件管理器中反斜杠搜索地址即可共享

```bash
\\192.168.5.128
```



---

### 3. 更新gcc环境

```bash
sudo apt-get install build-essential
```



---



## 四、Linux的命令操作

### 1. 处理目录

接下来我们就来看几个常见的处理目录的命令：

- ls：列出目录

- cd：切换目录

- pwd：显示目前的目录

- mkdir：创建一个新的目录

- rmdir：删除一个空的目录

- cp：复制文件或目录

- rm：移除文件或目录

- mv：移动文件与目录，或修改文件与目录的名称


你可以使用man /命令/ 来查看各个命令的使用文档



---



### 2. 处理文件

Linux系统中使用以下命令来查看文件的内容：

- cat：由第一行开始显示文件内容
- tac：从最后一行开始显示，可以看出tac是cat的倒著写！
- nl：显示的时候，顺道输出行号！
- more：一页一页的显示文件内容
- less：与more类似，但是比more更好的是，他可以往前翻页！
- head：只看头几行
- tail：只看尾巴几行

你可以使用man[命令]来查看各个命令的使用文档，如：man cp。



==可以使用vim进行编辑文件，或者查看文件==





---

# Shell脚本编程案例 



简单来说就是编写一个脚本输出hello world

```bash
# !/bin/bash

echo "hello world"

```



之后的话还要设置权限以及使用

```bash
chmod +x first.sh 

ls -l

bash first.sh 
```





---

再补充一点循环的功能，实现相加：

```bash
# !/bin/bash

sum=0
for i in {1..100}; do 
        let sum+=i
done
echo $sum

```



---

小项目，实现检测当前局域网内有那些ip宕机了：

```bash
ping-c 2 -i 0.5 192.168.199.128
```

首先这是常见的ping通指令



```bash
#! /bin/bash

for i in {1..254}; do
    ping -c 2 -i 0.5 192.168.5.$i /dev/null
    if [ $? -eq 0 ]; then
        echo "192.168.5.$i is up"
    else
        echo "192.168.5.$i is down"
    fi
done


```



---



## 课后作业

写一个bash脚本对于ifconfig的输出字段中的eth0的ip地址进行获取

```bash
#! /bin/bash

interface="ens3"

ip_address=$(ifconfig $interface 2>/dev/null | grep -w 'inet' | awk '{print $2}' | cut -d ':' -f2)

if [ -z "$ip_address" ]; then
    echo "错误：未找到接口 $interface 或未分配IP！"
    exit 1
else
    echo "$interface IP地址: $ip_address"
fi
```



详细解析一下这几个部分的用法吧：

#### **分解步骤：**

|       命令/操作       |                          作用                          |               输入示例               |         输出示例          |
| :-------------------: | :----------------------------------------------------: | :----------------------------------: | :-----------------------: |
| `ifconfig $interface` | 输出 `ens3` 接口的网络配置信息（若接口不存在则报错）。 | `ens33: ... inet addr:192.168.1.100` |  接口配置信息的完整文本   |
|     `2>/dev/null`     |         屏蔽错误输出（如接口不存在时的报错）。         |    隐藏 `ens3: error fetching...`    |             -             |
|   `grep -w 'inet'`    |   匹配包含完整单词 `inet` 的行（避免匹配 `inet6`）。   |      `inet addr:192.168.1.100`       | `inet addr:192.168.1.100` |
|  `awk '{print $2}'`   |            提取第二个字段（分隔符为空格）。            |      `inet addr:192.168.1.100`       |   `addr:192.168.1.100`    |
|   `cut -d ':' -f2`    |  以 `:` 为分隔符，取第二个字段（去除 `addr:` 前缀）。  |         `addr:192.168.1.100`         |      `192.168.1.100`      |







---

# 统计文件单词数量（文件操作）

## 1. 状态机实现文件单词统计

![image-20250425004618964](studynotes/image-20250425004618964.png)

简单来说用状态机分析一下如上图。

但是感觉画的不对，应该正确的是这样的：

![image-20250425013343494](studynotes/image-20250425013343494.png)

视频中给出的代码如下：

```c


#include <stdio.h>


#define OUT		0
#define IN		1

#define INIT	OUT

int splite(char c) {
	if ((' ' == c) || ('\n' == c) || ('\t' == c) ||
			('\"' == c) || ('\'' == c) || ('+' == c) ||
			(',' == c) || ';' == c || '.' == c)
		return 1;
	else
		return 0;
	
}

int count_word(char *filename) {

	int status = INIT;
	int word = 0;

	FILE *fp = fopen(filename, "r");
	if (fp == NULL) return -1;

	char c;
	while ((c = fgetc(fp)) != EOF) {

		if (splite(c)) { //
			status = OUT;
		} else if (OUT == status) {
			status = IN;
			word ++;
		}

	}

	return word;

}


int main(int argc, char *argv[]) {

	if (argc < 2) return -1;

	printf("word: %d\n", count_word(argv[1]));

}



```

感觉写的不太对，这样写下去，所有的分隔符都得包含在里面，不然就会报错。



```c
#include <stdio.h>

#define OUT 0
#define IN 1

#define INIT OUT

int count_word(char *filename)
{
    int status = INIT;
    FILE *fp = fopen(filename, "r");
    if (fp == NULL)
        return -1;

    int word_num = 0;
    int c;
    while ((c = fgetc(fp)) != EOF)
    {
        if (('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z'))
        {
            if (status == OUT)
            {
                status = IN;
                word_num++;
            }
        }
        else
        {
            if (status == IN)
            {
                status = OUT;
            }
        }
    }
    fclose(fp);
    return word_num;
}

int main(int argc, char *argv[])
{
    if (argc < 2)
        return -1;
    printf("nums: %d", count_word(argv[1]));
}
```

==感觉我这样判断字母才是对的。==



---



## 2. 解决连字符的问题

> 连字符问题我觉得分为两种：
>
> 1. 段内的连字符：JNJ-code这种
>
> 2. 换行的连字符：
>
>    acon-
>
>    da
>
> 这两种情况都需要解决



```c
#include <stdio.h>

#define OUT 0   // 空闲状态
#define IN 1    // 正在读单词
#define TEMP 2  // 刚读到 '-'，待判定
#define TEMP1 3 // 在 '-' 之后遇到 '\n'，待判定

int is_letter(int c)
{
    return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z');
}

int count_word(const char *filename)
{
    int state = OUT;
    FILE *fp = fopen(filename, "r");
    if (!fp)
        return -1;

    int word_num = 0;
    int c;
    while ((c = fgetc(fp)) != EOF)
    {
        switch (state)
        {
        case OUT:
            if (is_letter(c))
            {
                state = IN;
                word_num++;
            }
            // 其他情况都留在 OUT
            break;

        case IN:
            if (is_letter(c))
            {
                // 继续在词中
            }
            else if (c == '-')
            {
                state = TEMP; // 可能的连字符
            }
            else
            {
                state = OUT; // 遇到非字母非'-'，词结束
            }
            break;

        case TEMP:
            if (is_letter(c))
            {
                state = IN; // 段内连字符，如 JNJ-code
            }
            else if (c == '\n')
            {
                state = TEMP1; // 换行连字符，如 acon-\n 后面待判
            }
            else
            {
                state = OUT; // 既不是字母也不是换行，回到 OUT
            }
            break;

        case TEMP1:
            if (is_letter(c))
            {
                state = IN; // 连字符换行后字母，继续 IN
            }
            else
            {
                state = OUT; // 不是字母，视为词已结束
            }
            break;
        }
    }

    fclose(fp);
    return word_num;
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s filename\n", argv[0]);
        return 1;
    }
    int n = count_word(argv[1]);
    if (n < 0)
    {
        perror("fopen");
        return 1;
    }
    printf("nums: %d\n", n);
    return 0;
}

```

功能验证没有问题。



---



## 3. 统计单词出现的频率

我个人的第一想法是，使用一个树来做统计。每个节点对应一个27个子节点的树==（26个字母+一个连字符）==，以此来存储英文单词。

（主要换行的连字符-我们可以丢弃，但是单词间有的时候也有连字符：jnj-box这种）



> 但是我的做法里面有个小问题我还不知道怎么去解决，就是对于每句话的首字母大写，我的做法是把**每个单词中的所有大写字母转化为小写**，但是这种转化是否合适呢？或者说本质上我们就应该定义52个节点来作为大小写的分开判断？
>
> **这边简化流程，我决定将所有的字母变为小写来保存单词，同时例如 Jnj-box 则保存为jnj-box。毕竟拓展起来也不难**



最终代码：

```c
#include <stdio.h>
#include <stdlib.h>

#define OUT 0   // 空闲状态
#define IN 1    // 正在读单词
#define TEMP 2  // 刚读到 '-'，待判定
#define TEMP1 3 // 在 '-' 之后遇到 '\n'，待判定

typedef struct letter_node
{
    int count;
    struct letter_node *children[27];
} letter_node;

letter_node *create_node(void)
{
    letter_node *temp_node = malloc(sizeof(letter_node));
    temp_node->count = 0;
    for (int i = 0; i < 27; ++i)
    {
        temp_node->children[i] = NULL;
    }
    return temp_node;
}

int is_letter(int c)
{
    return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z');
}

int count_word(const char *filename, letter_node *Dummy_node)
{
    int state = OUT;
    FILE *fp = fopen(filename, "r");
    if (!fp)
        return -1;

    int word_num = 0;
    int c;

    letter_node *temp_node;

    while ((c = fgetc(fp)) != EOF)
    {
        switch (state)
        {
        case OUT:
            if (is_letter(c))
            {
                state = IN;
                word_num++;
                c = c >= 'A' && c <= 'Z' ? c - 'A' + 'a' : c;
                if (Dummy_node->children[c - 'a'] == NULL)
                {
                    Dummy_node->children[c - 'a'] = create_node();
                }
                temp_node = Dummy_node->children[c - 'a'];
            }
            // 其他情况都留在 OUT
            break;

        case IN:
            if (is_letter(c))
            {
                c = c >= 'A' && c <= 'Z' ? c - 'A' + 'a' : c;
                if (temp_node->children[c - 'a'] == NULL)
                {
                    temp_node->children[c - 'a'] = create_node();
                }
                temp_node = temp_node->children[c - 'a'];
                // 继续在词中
            }
            else if (c == '-')
            {
                state = TEMP; // 可能的连字符
            }
            else
            {
                temp_node->count++;
                temp_node = NULL;
                state = OUT; // 遇到非字母非'-'，词结束
            }
            break;

        case TEMP:
            if (is_letter(c))
            {
                if (temp_node->children[26] == NULL)
                {
                    temp_node->children[26] = create_node();
                }
                temp_node = temp_node->children[26];

                c = c >= 'A' && c <= 'Z' ? c - 'A' + 'a' : c;

                if (temp_node->children[c - 'a'] == NULL)
                {
                    temp_node->children[c - 'a'] = create_node();
                }

                temp_node = temp_node->children[c - 'a'];

                state = IN; // 段内连字符，如 JNJ-code
            }
            else if (c == '\n')
            {
                state = TEMP1; // 换行连字符，如 acon-\n 后面待判
            }
            else
            {
                temp_node->count++;
                temp_node = NULL;
                state = OUT; // 既不是字母也不是换行，回到 OUT
            }
            break;

        case TEMP1:
            if (is_letter(c))
            {
                c = c >= 'A' && c <= 'Z' ? c - 'A' + 'a' : c;

                if (temp_node->children[c - 'a'] == NULL)
                {
                    temp_node->children[c - 'a'] = create_node();
                }

                temp_node = temp_node->children[c - 'a'];

                state = IN; // 连字符换行后字母，继续 IN
            }
            else
            {
                temp_node->count++;
                temp_node = NULL;
                state = OUT; // 不是字母，视为词已结束
            }
            break;
        }
    }

    if (temp_node != NULL)
        temp_node->count++;
    fclose(fp);
    return word_num;
}

// 前向声明辅助函数
void traverse(letter_node *node, char *word, int depth);

void print_result(letter_node *Dummy_node)
{
    char buffer[1024]; // 假设单词最大长度为1023字符
    for (int i = 0; i < 26; ++i)
    { // 遍历所有可能的首字母（a-z）
        letter_node *child = Dummy_node->children[i];
        if (child != NULL)
        {
            buffer[0] = 'a' + i;        // 设置首字母
            traverse(child, buffer, 1); // 深度从1开始
        }
    }
}

void traverse(letter_node *node, char *word, int depth)
{
    if (node == NULL)
        return;

    // 如果当前节点有计数，输出单词
    if (node->count > 0)
    {
        word[depth] = '\0'; // 终止字符串
        printf("%s: %d\n", word, node->count);
    }

    // 遍历所有子节点（a-z和连字符）
    for (int i = 0; i < 27; ++i)
    {
        if (node->children[i] != NULL)
        {
            char c;
            if (i < 26)
            {
                c = 'a' + i;
            }
            else
            {
                c = '-';
            }
            word[depth] = c; // 添加当前字符到单词中
            traverse(node->children[i], word, depth + 1);
        }
    }
}

int main(int argc, char *argv[])
{
    letter_node *Dummy_node = create_node();
    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s filename\n", argv[0]);
        return 1;
    }
    int n = count_word(argv[1], Dummy_node);
    if (n < 0)
    {
        perror("fopen");
        return 1;
    }
    printf("nums: %d\n", n);
    printf("==========================\n");
    printf("单词统计如下：\n");
    print_result(Dummy_node);
    printf("==========================\n");
    return 0;
}

```



经过检查是对的，最终结果测试案例

```
Foreword.
 After several ' " ; , . ! { -+  } ye-ars of cont date-!
emplation and of thought upon the matter her-
ein contained-
  overflowed
```

对于上述这段文字的话，

最终输出结果如下：

```bash
zhenxing@ubuntu:~/share/01_count$ ./homework_count_per a.txt 
nums: 17
==========================
单词统计如下：
after: 1
and: 1
cont: 1
contained: 1
date: 1
emplation: 1
foreword: 1
herein: 1
matter: 1
of: 2
overflowed: 1
several: 1
the: 1
thought: 1
upon: 1
ye-ars: 1
==========================
```

实现我的功能设想：

1. 实测对大文件b.txt也可以实现，但是单词太多了，输出结果都显示不全。可能需要**保存到 log.txt 中去比较好。**（TODO）

2. **实测b.txt文件中也含有连字符单词的情况，所以这个的连字符处理方式是没有问题的。**

3. 部分重复代码实际上可以抽象成函数，这样更简洁。

   ```c
   c = c >= 'A' && c <= 'Z' ? c - 'A' + 'a' : c;
   if (temp_node->children[c - 'a'] == NULL)
   {
       temp_node->children[c - 'a'] = create_node();
   }
   temp_node = temp_node->children[c - 'a'];
   ```

   

---





# 实现通讯录（结构体）

## 1. 通讯录实现的架构设计与需求分析





