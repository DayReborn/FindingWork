#include <stdio.h>
#include <stdlib.h>
#include <rte_eal.h>       // DPDK环境抽象层
#include <rte_ethdev.h>    // DPDK以太网设备接口
#include <rte_ip.h>        // IP协议相关定义
#include <arpa/inet.h>     // 网络地址转换函数

// 全局配置变量
int global_portid = 0; // 全局端口ID，可设置为任意有效端口编号

// 网络缓冲区配置
#define NUM_MBUFS 4096    // 内存池中mbuf数量
#define BURST_SIZE 128     // 单次收发包的最大数量
#define ENABLE_SEND 1      // 使能发送功能开关
#define TCP_INIT_WINDOWS 14600 // TCP初始窗口大小

#if ENABLE_SEND
// 源/目的MAC地址（相对于发送方向）
uint8_t global_smac[RTE_ETHER_ADDR_LEN]; // 源MAC地址
uint8_t global_dmac[RTE_ETHER_ADDR_LEN]; // 目的MAC地址

// IP地址和端口号
uint32_t global_sip;  // 源IP地址（网络字节序）
uint32_t global_dip;  // 目的IP地址（网络字节序）
uint16_t global_sport; // 源端口号（主机字节序）
uint16_t global_dport; // 目的端口号（主机字节序）
#endif

// TCP控制字段
uint8_t global_flags;   // 接收到的TCP标志位
uint32_t global_seqnum; // 接收到的TCP序列号
uint32_t global_acknum; // 接收到的TCP确认号

// TCP状态机定义
typedef enum __USTACK_TCP_STATUS {
    USTACK_TCP_STATUS_CLOSED = 0,
    USTACK_TCP_STATUS_LISTEN,
    USTACK_TCP_STATUS_SYN_RCVD,
    USTACK_TCP_STATUS_SYN_SENT,
    USTACK_TCP_STATUS_ESTABLISHED,
    USTACK_TCP_STATUS_FIN_WAIT_1,
    USTACK_TCP_STATUS_FIN_WAIT_2,
    USTACK_TCP_STATUS_CLOSING,
    USTACK_TCP_STATUS_TIMEWAIT,
    USTACK_TCP_STATUS_CLOSE_WAIT,
    USTACK_TCP_STATUS_LAST_ACK
} USTACK_TCP_STATUS;

uint8_t tcp_status = USTACK_TCP_STATUS_LISTEN; // 初始TCP状态为监听

// 函数前置声明
int ustack_encode_udp_pkt(uint8_t *msg, uint8_t *data, uint16_t total_len);
int ustack_encode_tcp_pkt(uint8_t *msg, uint16_t total_len);

// 默认端口配置（只设置最大接收包长度）
static const struct rte_eth_conf port_conf_default = {
    .rxmode = {.max_rx_pkt_len = RTE_ETHER_MAX_LEN}
};

/**
 * @brief 初始化指定网卡端口
 * @param mbuf_pool 预分配的内存池
 * @return 成功返回0，失败退出程序
 */
static int ustack_init_port(struct rte_mempool *mbuf_pool)
{
    // 查询可用端口数量
    uint16_t nb_sys_ports = rte_eth_dev_count_avail();
    printf("可用端口数量: %u\n", nb_sys_ports);
    if (nb_sys_ports == 0) {
        rte_exit(EXIT_FAILURE, "没有可用端口!\n");
    }

    // 获取网卡设备信息
    struct rte_eth_dev_info dev_info;
    rte_eth_dev_info_get(global_portid, &dev_info);

    // 配置队列数量（根据发送功能开关决定TX队列）
    const int num_rx_queues = 1; // RX队列数量
#if ENABLE_SEND
    const int num_tx_queues = 1; // TX队列数量
#else
    const int num_tx_queues = 0;
#endif

    // 配置网卡端口
    rte_eth_dev_configure(global_portid, num_rx_queues, num_tx_queues, &port_conf_default);

    // 设置RX队列
    int ret = rte_eth_rx_queue_setup(global_portid, 0, 128, 
                                    rte_eth_dev_socket_id(global_portid), 
                                    NULL, mbuf_pool);
    if (ret < 0) {
        rte_exit(EXIT_FAILURE, "端口%u的RX队列设置失败\n", global_portid);
    }

#if ENABLE_SEND
    // 设置TX队列
    struct rte_eth_txconf txq_conf = dev_info.default_txconf;
    txq_conf.offloads = port_conf_default.rxmode.offloads;
    ret = rte_eth_tx_queue_setup(global_portid, 0, 512, 
                                rte_eth_dev_socket_id(global_portid), 
                                &txq_conf);
    if (ret < 0) {
        rte_exit(EXIT_FAILURE, "端口%u的TX队列设置失败\n", global_portid);
    }
#endif

    // 启动网卡端口
    ret = rte_eth_dev_start(global_portid);
    if (ret < 0) {
        rte_exit(EXIT_FAILURE, "启动端口%u失败\n", global_portid);
    }

    return 0;
}

/**
 * @brief 构造UDP数据包
 * @param msg 缓冲区指针
 * @param data UDP载荷数据
 * @param total_len 数据包总长度（包括各层头部）
 * @return 成功返回0
 */
int ustack_encode_udp_pkt(uint8_t *msg, uint8_t *data, uint16_t total_len)
{
    // 1. 组装以太网头部
    struct rte_ether_hdr *eth = (struct rte_ether_hdr *)msg;
    rte_memcpy(eth->d_addr.addr_bytes, global_dmac, RTE_ETHER_ADDR_LEN); // 目的MAC
    rte_memcpy(eth->s_addr.addr_bytes, global_smac, RTE_ETHER_ADDR_LEN); // 源MAC
    eth->ether_type = htons(RTE_ETHER_TYPE_IPV4);  // 设置以太网类型为IPv4

    // 2. 组装IPv4头部
    struct rte_ipv4_hdr *ip = (struct rte_ipv4_hdr *)(eth + 1);
    ip->version_ihl = 0x45;  // IPv4，头部长度20字节
    ip->type_of_service = 0; // 服务类型（默认）
    ip->total_length = htons(total_len - sizeof(struct rte_ether_hdr)); // IP包总长
    ip->packet_id = 0;       // 包标识（未分片）
    ip->fragment_offset = 0; // 分片偏移（未分片）
    ip->time_to_live = 64;   // TTL值
    ip->next_proto_id = IPPROTO_UDP; // 上层协议为UDP
    ip->src_addr = global_sip; // 源IP（已为网络字节序）
    ip->dst_addr = global_dip; // 目的IP（已为网络字节序）
    
    // 计算IP头部校验和
    ip->hdr_checksum = 0;
    ip->hdr_checksum = rte_ipv4_cksum(ip);

    // 3. 组装UDP头部
    struct rte_udp_hdr *udp = (struct rte_udp_hdr *)(ip + 1);
    udp->src_port = global_sport;   // 源端口（主机字节序）
    udp->dst_port = global_dport;   // 目的端口（主机字节序）
    uint16_t udplen = total_len - sizeof(struct rte_ether_hdr) - sizeof(struct rte_ipv4_hdr);
    udp->dgram_len = htons(udplen); // UDP数据报总长

    // 4. 填充UDP载荷数据
    rte_memcpy((uint8_t *)(udp + 1), data, udplen - sizeof(struct rte_udp_hdr));
    
    // 计算UDP校验和（包括伪头部）
    udp->dgram_cksum = 0;
    udp->dgram_cksum = rte_ipv4_udptcp_cksum(ip, udp);

    return 0;
}

/**
 * @brief 构造TCP数据包（仅SYN+ACK响应）
 * @param msg 缓冲区指针
 * @param total_len 数据包总长度
 * @return 成功返回0
 */
int ustack_encode_tcp_pkt(uint8_t *msg, uint16_t total_len)
{
    // 1. 组装以太网头部
    struct rte_ether_hdr *eth = (struct rte_ether_hdr *)msg;
    rte_memcpy(eth->d_addr.addr_bytes, global_dmac, RTE_ETHER_ADDR_LEN); // 目的MAC
    rte_memcpy(eth->s_addr.addr_bytes, global_smac, RTE_ETHER_ADDR_LEN); // 源MAC
    eth->ether_type = htons(RTE_ETHER_TYPE_IPV4);  // 设置以太网类型为IPv4

    // 2. 组装IPv4头部
    struct rte_ipv4_hdr *ip = (struct rte_ipv4_hdr *)(eth + 1);
    ip->version_ihl = 0x45;  // IPv4，头部长度20字节
    ip->type_of_service = 0; // 服务类型（默认）
    ip->total_length = htons(total_len - sizeof(struct rte_ether_hdr)); // IP包总长
    ip->packet_id = 0;       // 包标识（未分片）
    ip->fragment_offset = 0; // 分片偏移（未分片）
    ip->time_to_live = 64;   // TTL值
    ip->next_proto_id = IPPROTO_TCP; // 上层协议为TCP
    ip->src_addr = global_sip;      // 源IP
    ip->dst_addr = global_dip;      // 目的IP
    
    // 计算IP头部校验和
    ip->hdr_checksum = 0;
    ip->hdr_checksum = rte_ipv4_cksum(ip);

    // 3. 组装TCP头部（SYN+ACK响应）
    struct rte_tcp_hdr *tcp = (struct rte_tcp_hdr *)(ip + 1);
    tcp->src_port = global_sport;       // 源端口
    tcp->dst_port = global_dport;       // 目的端口
    tcp->sent_seq = htonl(12345);        // 发送序列号（固定值）
    tcp->recv_ack = htonl(global_seqnum + 1); // 确认号 = 接收序列号+1
    tcp->data_off = 0x50;                // 数据偏移（5 * 4=20字节）
    tcp->tcp_flags = RTE_TCP_SYN_FLAG | RTE_TCP_ACK_FLAG; // SYN+ACK标志位
    tcp->rx_win = htons(TCP_INIT_WINDOWS); // 接收窗口大小
    
    // 计算TCP校验和（包括伪头部）
    tcp->cksum = 0;
    tcp->cksum = rte_ipv4_udptcp_cksum(ip, tcp);

    return 0;
}

/**
 * @brief 主函数（DPDK程序入口）
 * @param argc 参数个数
 * @param argv 参数列表
 * @return 程序退出状态
 */
int main(int argc, char *argv[])
{
    // 1. 初始化DPDK环境
    if (rte_eal_init(argc, argv) < 0) {
        rte_exit(EXIT_FAILURE, "EAL初始化失败\n");
    }

    // 2. 创建内存池
    struct rte_mempool *mbuf_pool = rte_pktmbuf_pool_create(
        "mbuf poll", NUM_MBUFS, 0, 0, RTE_MBUF_DEFAULT_BUF_SIZE, rte_socket_id());
    if (mbuf_pool == NULL) {
        rte_exit(EXIT_FAILURE, "内存池创建失败!\n");
    }

    // 3. 初始化网卡端口
    ustack_init_port(mbuf_pool);

    // 4. 主处理循环
    while (1) {
        struct rte_mbuf *mbufs[BURST_SIZE] = {0}; // 接收缓冲区
        
        // 接收数据包（一次最多BURST_SIZE个）
        uint16_t num_recved = rte_eth_rx_burst(global_portid, 0, mbufs, BURST_SIZE);
        if (num_recved > BURST_SIZE) {
            rte_exit(EXIT_FAILURE, "接收错误: 超出突发大小!\n");
        }

        // 处理每个收到的数据包
        for (int i = 0; i < num_recved; i++) {
            struct rte_ether_hdr *ethhdr = rte_pktmbuf_mtod(mbufs[i], struct rte_ether_hdr *);
            
            // 仅处理IPv4数据包
            if (ethhdr->ether_type != rte_cpu_to_be_16(RTE_ETHER_TYPE_IPV4)) {
                continue; // 跳过非IPv4包
            }
            
            // 解析IP头部
            struct rte_ipv4_hdr *iphdr = rte_pktmbuf_mtod_offset(
                mbufs[i], struct rte_ipv4_hdr *, sizeof(struct rte_ether_hdr));
            
            // 处理UDP协议
            if (iphdr->next_proto_id == IPPROTO_UDP) {
                struct rte_udp_hdr *udphdr = (struct rte_udp_hdr *)(iphdr + 1);
                
                // 记录反转发包需要的五元组信息（源变目的，目的变源）
#if ENABLE_SEND
                rte_memcpy(global_smac, ethhdr->d_addr.addr_bytes, RTE_ETHER_ADDR_LEN);
                rte_memcpy(global_dmac, ethhdr->s_addr.addr_bytes, RTE_ETHER_ADDR_LEN);
                rte_memcpy(&global_sip, &iphdr->dst_addr, sizeof(uint32_t));
                rte_memcpy(&global_dip, &iphdr->src_addr, sizeof(uint32_t));
                rte_memcpy(&global_sport, &udphdr->dst_port, sizeof(uint16_t));
                rte_memcpy(&global_dport, &udphdr->src_port, sizeof(uint16_t));
#endif
                
                // 打印收到的UDP包信息（源IP:PORT -> 目的IP:PORT）
                struct in_addr src_addr = {.s_addr = iphdr->src_addr};
                struct in_addr dst_addr = {.s_addr = iphdr->dst_addr};
                char src_str[INET_ADDRSTRLEN], dst_str[INET_ADDRSTRLEN];
                inet_ntop(AF_INET, &src_addr, src_str, INET_ADDRSTRLEN);
                inet_ntop(AF_INET, &dst_addr, dst_str, INET_ADDRSTRLEN);
                printf("接收UDP包: %s:%d -> %s:%d\n",
                       src_str, ntohs(udphdr->src_port),
                       dst_str, ntohs(udphdr->dst_port));
                
                // 启用发送时进行UDP回显
#if ENABLE_SEND
                uint16_t length = ntohs(udphdr->dgram_len);
                uint16_t total_len = length + sizeof(struct rte_ether_hdr) + sizeof(struct rte_ipv4_hdr);
                
                // 分配发送缓冲区
                struct rte_mbuf *mbuf = rte_pktmbuf_alloc(mbuf_pool);
                if (!mbuf) {
                    rte_exit(EXIT_FAILURE, "发送mbuf分配失败!\n");
                }
                mbuf->pkt_len = total_len;
                mbuf->data_len = total_len;
                
                // 构造并发送回显包
                uint8_t *msg = rte_pktmbuf_mtod(mbuf, uint8_t *);
                ustack_encode_udp_pkt(msg, (uint8_t *)(udphdr + 1), total_len);
                rte_eth_tx_burst(global_portid, 0, &mbuf, 1);
#endif
                // 打印UDP载荷内容
                printf("UDP载荷: %s\n\n", (char *)(udphdr + 1));
            }
            // 处理TCP协议
            else if (iphdr->next_proto_id == IPPROTO_TCP) {
                struct rte_tcp_hdr *tcphdr = (struct rte_tcp_hdr *)(iphdr + 1);
                
                // 保存反转发包需要的五元组信息
#if ENABLE_SEND
                rte_memcpy(global_smac, ethhdr->d_addr.addr_bytes, RTE_ETHER_ADDR_LEN);
                rte_memcpy(global_dmac, ethhdr->s_addr.addr_bytes, RTE_ETHER_ADDR_LEN);
                rte_memcpy(&global_sip, &iphdr->dst_addr, sizeof(uint32_t));
                rte_memcpy(&global_dip, &iphdr->src_addr, sizeof(uint32_t));
                rte_memcpy(&global_sport, &tcphdr->dst_port, sizeof(uint16_t));
                rte_memcpy(&global_dport, &tcphdr->src_port, sizeof(uint16_t));
#endif
                
                // 保存关键TCP控制字段
                global_flags = tcphdr->tcp_flags;
                global_seqnum = ntohl(tcphdr->sent_seq);
                global_acknum = ntohl(tcphdr->recv_ack);
                
                // 打印收到的TCP包信息
                struct in_addr src_addr = {.s_addr = iphdr->src_addr};
                struct in_addr dst_addr = {.s_addr = iphdr->dst_addr};
                char src_str[INET_ADDRSTRLEN], dst_str[INET_ADDRSTRLEN];
                inet_ntop(AF_INET, &src_addr, src_str, INET_ADDRSTRLEN);
                inet_ntop(AF_INET, &dst_addr, dst_str, INET_ADDRSTRLEN);
                printf("接收TCP包: %s:%d -> %s:%d \n参数: flags=0x%x, seqnum=%u, acknum=%u\n",
                       src_str, ntohs(tcphdr->src_port),
                       dst_str, ntohs(tcphdr->dst_port),
                       global_flags, global_seqnum, global_acknum);
                
                // TCP状态机处理
                // SYN包处理（监听状态）
                if (global_flags & RTE_TCP_SYN_FLAG) {
                    if (tcp_status == USTACK_TCP_STATUS_LISTEN) {
#if ENABLE_SEND
                        // 构造SYN+ACK响应包
                        uint16_t total_len = sizeof(struct rte_tcp_hdr) + 
                                            sizeof(struct rte_ipv4_hdr) +
                                            sizeof(struct rte_ether_hdr);
                        struct rte_mbuf *mbuf = rte_pktmbuf_alloc(mbuf_pool);
                        if (!mbuf) {
                            rte_exit(EXIT_FAILURE, "发送mbuf分配失败!\n");
                        }
                        mbuf->pkt_len = total_len;
                        mbuf->data_len = total_len;
                        
                        // 构造并发送SYN+ACK包
                        uint8_t *msg = rte_pktmbuf_mtod(mbuf, uint8_t *);
                        ustack_encode_tcp_pkt(msg, total_len);
                        rte_eth_tx_burst(global_portid, 0, &mbuf, 1);
#endif
                        tcp_status = USTACK_TCP_STATUS_SYN_RCVD;
                        printf("[状态变更] TCP进入SYN_RCVD状态\n");
                    }
                }
                
                // ACK包处理（SYN_RCVD状态）
                if ((global_flags & RTE_TCP_ACK_FLAG) && (tcp_status == USTACK_TCP_STATUS_SYN_RCVD)) {
                    tcp_status = USTACK_TCP_STATUS_ESTABLISHED;
                    printf("[状态变更] TCP进入ESTABLISHED状态\n");
                }
                
                // PSH包处理（已建立连接）
                if (global_flags & RTE_TCP_PSH_FLAG) {
                    printf("收到TCP PSH标志\n");
                    if (tcp_status == USTACK_TCP_STATUS_ESTABLISHED) {
                        // 计算TCP头部长度并定位载荷数据
                        uint8_t hdr_len = (tcphdr->data_off >> 4) * sizeof(uint32_t);
                        uint8_t *data_start = (uint8_t *)(tcphdr) + hdr_len;
                        printf("TCP载荷数据: %s\n", data_start);
                    }
                }
                printf("\n");
            }
            // 处理其他协议
            else {
                printf("收到不支持协议: %d\n\n", iphdr->next_proto_id);
            }
            
            // 释放接收缓冲区（实际应用需添加）
            rte_pktmbuf_free(mbufs[i]); 
        }
    }
    
    // 程序不会执行到此（实际应用需添加退出清理逻辑）
    printf("hello dpdk!\n");
    return 0;
}