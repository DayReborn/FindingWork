#include <stdio.h>

#include <stdlib.h>
#include <rte_eal.h>
#include <rte_ethdev.h>
#include <rte_ip.h>

#include <arpa/inet.h>

// 添加函数声明

int global_portid = 0; // Global port ID, can be set to any valid port ID

#define NUM_MBUFS 4096
#define BURST_SIZE 128

#define ENABLE_SEND 1

#define TCP_INIT_WINDOWS 14600

#if ENABLE_SEND

// 这边的s和d的定义相对于发送而言
uint8_t global_smac[RTE_ETHER_ADDR_LEN]; // Source MAC address
uint8_t global_dmac[RTE_ETHER_ADDR_LEN]; // Destination MAC address

uint32_t global_sip; // Source IP address
uint32_t global_dip; // Destination IP address

uint16_t global_sport; // Source port
uint16_t global_dport; // Destination port

#endif

uint8_t global_flags;
uint32_t global_seqnum;
uint32_t global_acknum;

typedef enum __USTACK_TCP_STATUS
{
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

uint8_t tcp_status = USTACK_TCP_STATUS_LISTEN;

int ustack_encode_udp_pkt(uint8_t *msg, uint8_t *data, uint16_t total_len);
int ustack_encode_tcp_pkt(uint8_t *msg, uint16_t total_len);

static const struct rte_eth_conf port_conf_default = {

    .rxmode = {.max_rx_pkt_len = RTE_ETHER_MAX_LEN}

};

static int ustack_init_port(struct rte_mempool *mbuf_pool)
{
    uint16_t nb_sys_ports = rte_eth_dev_count_avail();
    printf("Number of available ports: %u\n", nb_sys_ports);
    if (nb_sys_ports == 0)
    {
        rte_exit(EXIT_FAILURE, "No available port!\n");
    }

    struct rte_eth_dev_info dev_info;
    rte_eth_dev_info_get(global_portid, &dev_info);

    const int num_rx_queues = 1; // Number of RX queues to set up

#if ENABLE_SEND
    const int num_tx_queues = 1; // Number of TX queues to set up
#else
    const int num_tx_queues = 0;
#endif
    // eth0
    rte_eth_dev_configure(global_portid, num_rx_queues, num_tx_queues, &port_conf_default);

    // Setup RX queue
    int ret = rte_eth_rx_queue_setup(global_portid, 0, 128, rte_eth_dev_socket_id(global_portid), NULL, mbuf_pool);
    if (ret < 0)
    {
        rte_exit(EXIT_FAILURE, "Error setting up RX queue for port %u\n", global_portid);
    }

#if ENABLE_SEND
    // Setup TX queue
    struct rte_eth_txconf txq_conf = dev_info.default_txconf;
    txq_conf.offloads = port_conf_default.rxmode.offloads;

    ret = rte_eth_tx_queue_setup(global_portid, 0, 512, rte_eth_dev_socket_id(global_portid), &txq_conf);
    if (ret < 0)
    {
        rte_exit(EXIT_FAILURE, "Error setting up TX queue for port %u\n", global_portid);
    }
#endif

    ret = rte_eth_dev_start(global_portid);
    if (ret < 0)
    {
        rte_exit(EXIT_FAILURE, "Error starting port %u\n", global_portid);
    }

    return 0;
}

int ustack_encode_udp_pkt(uint8_t *msg, uint8_t *data, uint16_t total_len)
{
    // 首先组织以太网头(ether)
    struct rte_ether_hdr *eth = (struct rte_ether_hdr *)msg;
    rte_memcpy(eth->d_addr.addr_bytes, global_dmac, RTE_ETHER_ADDR_LEN);
    rte_memcpy(eth->s_addr.addr_bytes, global_smac, RTE_ETHER_ADDR_LEN);
    eth->ether_type = htons(RTE_ETHER_TYPE_IPV4);

    // 接着组织ip头
    struct rte_ipv4_hdr *ip = (struct rte_ipv4_hdr *)(eth + 1);
    // [equal]struct rte_ipv4_hdr *ip = msg + sizeof(struct rte_ether_hdr);
    ip->version_ihl = 0x45;
    ip->type_of_service = 0;
    ip->total_length = htons(total_len - sizeof(struct rte_ether_hdr));
    ip->packet_id = 0;
    ip->fragment_offset = 0;
    ip->time_to_live = 64;
    ip->next_proto_id = IPPROTO_UDP;
    ip->src_addr = global_sip; // 拷贝过来的不需要修改网络字节序列
    ip->dst_addr = global_dip;

    ip->hdr_checksum = 0;
    ip->hdr_checksum = rte_ipv4_cksum(ip);

    // 组织UDP头
    // 1 udp header

    struct rte_udp_hdr *udp = (struct rte_udp_hdr *)(ip + 1);
    udp->src_port = global_sport;
    udp->dst_port = global_dport;
    uint16_t udplen = total_len - sizeof(struct rte_ether_hdr) - sizeof(struct rte_ipv4_hdr);
    udp->dgram_len = htons(udplen);

    // 将数据拷贝到UDP头后面
    rte_memcpy((uint8_t *)(udp + 1), data, udplen - sizeof(struct rte_udp_hdr));
    udp->dgram_cksum = 0;
    udp->dgram_cksum = rte_ipv4_udptcp_cksum(ip, udp);

    return 0;
}

int ustack_encode_tcp_pkt(uint8_t *msg, uint16_t total_len)
{

    // 1 ether header

    struct rte_ether_hdr *eth = (struct rte_ether_hdr *)msg;
    rte_memcpy(eth->d_addr.addr_bytes, global_dmac, RTE_ETHER_ADDR_LEN);
    rte_memcpy(eth->s_addr.addr_bytes, global_smac, RTE_ETHER_ADDR_LEN);
    eth->ether_type = htons(RTE_ETHER_TYPE_IPV4);

    // 1 ip header
    struct rte_ipv4_hdr *ip = (struct rte_ipv4_hdr *)(eth + 1); // msg + sizeof(struct rte_ether_hdr);
    ip->version_ihl = 0x45;
    ip->type_of_service = 0;
    ip->total_length = htons(total_len - sizeof(struct rte_ether_hdr));
    ip->packet_id = 0;
    ip->fragment_offset = 0;
    ip->time_to_live = 64;
    ip->next_proto_id = IPPROTO_TCP;
    ip->src_addr = global_sip;
    ip->dst_addr = global_dip;

    ip->hdr_checksum = 0;
    ip->hdr_checksum = rte_ipv4_cksum(ip);

    // 1 tcp header

    struct rte_tcp_hdr *tcp = (struct rte_tcp_hdr *)(ip + 1);
    tcp->src_port = global_sport;
    tcp->dst_port = global_dport;
    tcp->sent_seq = htonl(12345); // 这里可以设置为任意值，通常是初始序列号
    tcp->recv_ack = htonl(global_seqnum + 1);
    tcp->data_off = 0x50;
    tcp->tcp_flags = RTE_TCP_SYN_FLAG | RTE_TCP_ACK_FLAG; // 0x1 << 1;

    tcp->rx_win = htons(TCP_INIT_WINDOWS); // htons(4096); // rmem
    tcp->cksum = 0;
    tcp->cksum = rte_ipv4_udptcp_cksum(ip, tcp);

    return 0;
}

int main(int argc, char *argv[])
{
    if (rte_eal_init(argc, argv) < 0)
    {
        rte_exit(EXIT_FAILURE, "Error with EAL init\n");
    }

    struct rte_mempool *mbuf_pool =
        rte_pktmbuf_pool_create("mbuf poll", NUM_MBUFS, 0, 0, RTE_MBUF_DEFAULT_BUF_SIZE, rte_socket_id());
    if (mbuf_pool == NULL)
    {
        rte_exit(EXIT_FAILURE, "Error creating mbuf pool!\n");
    }

    ustack_init_port(mbuf_pool);
    while (1)
    {
        struct rte_mbuf *mbufs[BURST_SIZE] = {0};
        uint16_t num_recved = rte_eth_rx_burst(global_portid, 0, mbufs, BURST_SIZE);
        if (num_recved > BURST_SIZE)
        {
            rte_exit(EXIT_FAILURE, "Error receiving packets: received more than burst size!\n");
        }
        int i = 0;
        for (i = 0; i < num_recved; i++)
        {
            struct rte_ether_hdr *ethhdr = rte_pktmbuf_mtod(mbufs[i], struct rte_ether_hdr *);
            if (ethhdr->ether_type != rte_cpu_to_be_16(RTE_ETHER_TYPE_IPV4))
            {
                continue;
            }
            struct rte_ipv4_hdr *iphdr = rte_pktmbuf_mtod_offset(mbufs[i], struct rte_ipv4_hdr *, sizeof(struct rte_ether_hdr));
            if (iphdr->next_proto_id == IPPROTO_UDP)
            {
                struct rte_udp_hdr *udphdr = (struct rte_udp_hdr *)(iphdr + 1);

                rte_memcpy(global_smac, ethhdr->d_addr.addr_bytes, RTE_ETHER_ADDR_LEN);
                rte_memcpy(global_dmac, ethhdr->s_addr.addr_bytes, RTE_ETHER_ADDR_LEN);

                rte_memcpy(&global_sip, &iphdr->dst_addr, sizeof(uint32_t));
                rte_memcpy(&global_dip, &iphdr->src_addr, sizeof(uint32_t));

                rte_memcpy(&global_sport, &udphdr->dst_port, sizeof(uint16_t));
                rte_memcpy(&global_dport, &udphdr->src_port, sizeof(uint16_t));

                // 小技巧：点分十进制转换
                // inet_ntoa() 函数将网络地址转换为点分十进制
                // 这边的打印是为了验证接收的包是否正确
                struct in_addr src_addr, dst_addr;
                src_addr.s_addr = iphdr->src_addr;
                dst_addr.s_addr = iphdr->dst_addr;
                char src_str[INET_ADDRSTRLEN], dst_str[INET_ADDRSTRLEN];
                inet_ntop(AF_INET, &src_addr, src_str, INET_ADDRSTRLEN);
                inet_ntop(AF_INET, &dst_addr, dst_str, INET_ADDRSTRLEN);
                printf("Received UDP packet from %s:%d (sip) to %s:%d (dip)\n",
                       src_str, ntohs(udphdr->src_port),
                       dst_str, ntohs(udphdr->dst_port));
#if ENABLE_SEND
                uint16_t length = ntohs(udphdr->dgram_len);
                uint16_t total_len = length + sizeof(struct rte_ether_hdr) + sizeof(struct rte_ipv4_hdr);

                // 发送数据包
                struct rte_mbuf *mbuf = rte_pktmbuf_alloc(mbuf_pool);
                if (!mbuf)
                {
                    rte_exit(EXIT_FAILURE, "Failed to allocate mbuf for sending!\n");
                }
                mbuf->pkt_len = total_len;
                mbuf->data_len = total_len;

                uint8_t *msg = rte_pktmbuf_mtod(mbuf, uint8_t *);
                ustack_encode_udp_pkt(msg, (uint8_t *)(udphdr + 1), total_len);

                rte_eth_tx_burst(global_portid, 0, &mbuf, 1);
#endif
                printf("Received UDP: %s\n", (char *)(udphdr + 1));
            }
            else if (iphdr->next_proto_id == IPPROTO_TCP)
            {
                struct rte_tcp_hdr *tcphdr = (struct rte_tcp_hdr *)(iphdr + 1);

                rte_memcpy(global_smac, ethhdr->d_addr.addr_bytes, RTE_ETHER_ADDR_LEN);
                rte_memcpy(global_dmac, ethhdr->s_addr.addr_bytes, RTE_ETHER_ADDR_LEN);

                rte_memcpy(&global_sip, &iphdr->dst_addr, sizeof(uint32_t));
                rte_memcpy(&global_dip, &iphdr->src_addr, sizeof(uint32_t));

                rte_memcpy(&global_sport, &tcphdr->dst_port, sizeof(uint16_t));
                rte_memcpy(&global_dport, &tcphdr->src_port, sizeof(uint16_t));

                global_flags = tcphdr->tcp_flags;
                global_seqnum = ntohl(tcphdr->sent_seq);
                global_acknum = ntohl(tcphdr->recv_ack);
                struct in_addr src_addr, dst_addr;
                src_addr.s_addr = iphdr->src_addr;
                dst_addr.s_addr = iphdr->dst_addr;
                char src_str[INET_ADDRSTRLEN], dst_str[INET_ADDRSTRLEN];
                inet_ntop(AF_INET, &src_addr, src_str, INET_ADDRSTRLEN);
                inet_ntop(AF_INET, &dst_addr, dst_str, INET_ADDRSTRLEN);
                printf("Received TCP packet from %s:%d (sip) to %s:%d (dip) \n[ARGMENT INFO] flags: 0x%x, seqnum: %u, acknum: %u\n",
                       src_str, ntohs(tcphdr->src_port),
                       dst_str, ntohs(tcphdr->dst_port),
                       global_flags, global_seqnum, global_acknum);

                if (global_flags & RTE_TCP_SYN_FLAG)
                {
                    if (tcp_status == USTACK_TCP_STATUS_LISTEN)
                    {
#if ENABLE_SEND
                        uint16_t total_len = sizeof(struct rte_tcp_hdr) + sizeof(struct rte_ipv4_hdr) +
                                             sizeof(struct rte_ether_hdr);
                        // 发送数据包
                        struct rte_mbuf *mbuf = rte_pktmbuf_alloc(mbuf_pool);
                        if (!mbuf)
                        {
                            rte_exit(EXIT_FAILURE, "Failed to allocate mbuf for sending!\n");
                        }
                        mbuf->pkt_len = total_len;
                        mbuf->data_len = total_len;

                        uint8_t *msg = rte_pktmbuf_mtod(mbuf, uint8_t *);
                        ustack_encode_tcp_pkt(msg, total_len);

                        rte_eth_tx_burst(global_portid, 0, &mbuf, 1);
#endif
                        tcp_status = USTACK_TCP_STATUS_SYN_RCVD;
                        printf("[STATUS INFO] Tcp status changed to USTACK_TCP_STATUS_SYN_RCVD\n");
                    }
                }
                if (global_flags & RTE_TCP_ACK_FLAG && tcp_status == USTACK_TCP_STATUS_SYN_RCVD)
                {
                    if (tcp_status == USTACK_TCP_STATUS_SYN_RCVD)
                    {
                        // 这里可以发送ACK包
                        tcp_status = USTACK_TCP_STATUS_ESTABLISHED;
                        printf("[STATUS INFO] Tcp status changed to USTACK_TCP_STATUS_ESTABLISHED\n");
                    }
                }
                if (global_flags & RTE_TCP_PSH_FLAG)
                {
                    printf("[STATUS INFO] Received TCP PSH flag\n");
                    if (tcp_status == USTACK_TCP_STATUS_ESTABLISHED)
                    {
                        // 定位数据存储在tcp包的位置
                        uint8_t hdr_len = (tcphdr->data_off >> 4) * sizeof(uint32_t); // 计算TCP头长度
                        uint8_t *data_start = (uint8_t *)(tcphdr) + hdr_len;          // TCP头后面的数据
                        printf("TCP data:%s\n", data_start);
                    }
                }
                printf("\n");
            }
            else
            {
                // 处理其他协议包
                printf("Received packet with unsupported protocol: %d\n", iphdr->next_proto_id);
            }
        }
    }
    printf("hello dpdk!\n");
}