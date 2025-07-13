#include <stdio.h>

#include <stdlib.h>
#include <rte_eal.h>
#include <rte_ethdev.h>

#include <arpa/inet.h>

int global_portid = 0; // Global port ID, can be set to any valid port ID

#define NUM_MBUFS 4096
#define BURST_SIZE 128

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

    // struct rte_eth_dev_info dev_info;
    // rte_eth_dev_info_get(global_portid, &dev_info);

    const int num_rx_queues = 1;
    const int num_tx_queues = 0;
    // eth0
    rte_eth_dev_configure(global_portid, num_rx_queues, num_tx_queues, &port_conf_default);

    int ret = rte_eth_rx_queue_setup(global_portid, 0, 128, rte_eth_dev_socket_id(global_portid), NULL, mbuf_pool);

    if (ret < 0)
    {
        rte_exit(EXIT_FAILURE, "Error setting up RX queue for port %u\n", global_portid);
    }

    ret = rte_eth_dev_start(global_portid);
    if (ret < 0)
    {
        rte_exit(EXIT_FAILURE, "Error starting port %u\n", global_portid);
    }

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
                printf("Received UDP: %s\n", (char *)(udphdr + 1));
            }
        }
    }

    printf("hello dpdk!\n");
}