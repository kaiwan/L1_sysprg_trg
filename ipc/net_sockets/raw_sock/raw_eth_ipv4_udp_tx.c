#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/if_ether.h>
#include <netpacket/packet.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netinet/udp.h>

#define PAYLOAD "Hello, UDP!"
#define ETH_HDR_LEN 14
#define IP_HDR_LEN 20
#define UDP_HDR_LEN 8

// Pseudo-header for UDP checksum
struct pseudo_header {
    uint32_t src_addr;
    uint32_t dst_addr;
    uint8_t zero;
    uint8_t protocol;
    uint16_t udp_length;
};

unsigned short checksum(unsigned short *buf, int len) {
    unsigned long sum = 0;
    while (len > 1) {
        sum += *buf++;
        len -= 2;
    }
    if (len) sum += *(unsigned char *)buf;
    sum = (sum >> 16) + (sum & 0xffff);
    sum += (sum >> 16);
    return (unsigned short)(~sum);
}

int main() {
    int sockfd;
    char ifname[] = "wlo1"; // Replace with your interface
    struct ifreq if_idx, if_mac;
    struct sockaddr_ll socket_address;
    char sendbuf[ETH_FRAME_LEN];
    int tx_len = 0;

    // Payload
    char *data = PAYLOAD;
    int data_len = strlen(data);

    // Create raw socket
    if ((sockfd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL))) == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // Get interface index
    memset(&if_idx, 0, sizeof(struct ifreq));
    strncpy(if_idx.ifr_name, ifname, IFNAMSIZ - 1);
    if (ioctl(sockfd, SIOCGIFINDEX, &if_idx) < 0) {
        perror("SIOCGIFINDEX");
        exit(EXIT_FAILURE);
    }

    // Get MAC address
    memset(&if_mac, 0, sizeof(struct ifreq));
    strncpy(if_mac.ifr_name, ifname, IFNAMSIZ - 1);
    if (ioctl(sockfd, SIOCGIFHWADDR, &if_mac) < 0) {
        perror("SIOCGIFHWADDR");
        exit(EXIT_FAILURE);
    }

    // Ethernet header
    unsigned char dest_mac[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff}; // Broadcast
    memcpy(sendbuf, dest_mac, 6);
    tx_len += 6;
    memcpy(sendbuf + tx_len, if_mac.ifr_hwaddr.sa_data, 6);
    tx_len += 6;
    sendbuf[tx_len++] = 0x08; // EtherType = IPv4
    sendbuf[tx_len++] = 0x00;

    // IP header
    struct iphdr *iph = (struct iphdr *)(sendbuf + ETH_HDR_LEN);
    iph->ihl = 5;
    iph->version = 4;
    iph->tos = 0;
    iph->tot_len = htons(IP_HDR_LEN + UDP_HDR_LEN + data_len);
    iph->id = htons(12345);
    iph->frag_off = 0;
    iph->ttl = 64;
    iph->protocol = IPPROTO_UDP;
    iph->check = 0;
    iph->saddr = inet_addr("192.168.0.100"); // Change to real source IP
    iph->daddr = inet_addr("192.168.0.255"); // Change to real dest IP (broadcast)

    iph->check = checksum((unsigned short *)iph, IP_HDR_LEN);

    // UDP header
    struct udphdr *udph = (struct udphdr *)(sendbuf + ETH_HDR_LEN + IP_HDR_LEN);
    udph->source = htons(12345);
    udph->dest = htons(54321);
    udph->len = htons(UDP_HDR_LEN + data_len);
    udph->check = 0; // Calculated below

    // Copy payload
    memcpy(sendbuf + ETH_HDR_LEN + IP_HDR_LEN + UDP_HDR_LEN, data, data_len);

    // Pseudo-header + UDP + payload for checksum
    struct pseudo_header psh;
    psh.src_addr = iph->saddr;
    psh.dst_addr = iph->daddr;
    psh.zero = 0;
    psh.protocol = IPPROTO_UDP;
    psh.udp_length = htons(UDP_HDR_LEN + data_len);

    int psize = sizeof(struct pseudo_header) + UDP_HDR_LEN + data_len;
    char *pseudogram = malloc(psize);
    memcpy(pseudogram, &psh, sizeof(struct pseudo_header));
    memcpy(pseudogram + sizeof(struct pseudo_header), udph, UDP_HDR_LEN + data_len);
    udph->check = checksum((unsigned short *)pseudogram, psize);
    free(pseudogram);

    // Destination socket address
    memset(&socket_address, 0, sizeof(struct sockaddr_ll));
    socket_address.sll_ifindex = if_idx.ifr_ifindex;
    socket_address.sll_halen = ETH_ALEN;
    memcpy(socket_address.sll_addr, dest_mac, 6);

    tx_len = ETH_HDR_LEN + IP_HDR_LEN + UDP_HDR_LEN + data_len;

    // Send packet
    if (sendto(sockfd, sendbuf, tx_len, 0,
               (struct sockaddr *)&socket_address, sizeof(struct sockaddr_ll)) < 0) {
        perror("sendto");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Ethernet + IP + UDP packet sent successfully\n");
    close(sockfd);
    return 0;
}
