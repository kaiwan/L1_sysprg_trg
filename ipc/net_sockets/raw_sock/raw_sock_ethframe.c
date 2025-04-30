#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/if_ether.h> // For ETH_P_ALL
#include <netpacket/packet.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>

int main() {
    int sockfd;
    char ifname[] = "wlo1"; // Change as needed
    struct ifreq if_idx;
    struct ifreq if_mac;
    struct sockaddr_ll socket_address;
    char sendbuf[ETH_FRAME_LEN];
    int tx_len = 0;

    // Create raw socket
    if ((sockfd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL))) == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // Get the index of the interface
    memset(&if_idx, 0, sizeof(struct ifreq));
    strncpy(if_idx.ifr_name, ifname, IFNAMSIZ - 1);
    if (ioctl(sockfd, SIOCGIFINDEX, &if_idx) < 0) {
        perror("SIOCGIFINDEX");
        exit(EXIT_FAILURE);
    }

    // Get the MAC address of the interface
    memset(&if_mac, 0, sizeof(struct ifreq));
    strncpy(if_mac.ifr_name, ifname, IFNAMSIZ - 1);
    if (ioctl(sockfd, SIOCGIFHWADDR, &if_mac) < 0) {
        perror("SIOCGIFHWADDR");
        exit(EXIT_FAILURE);
    }

    // Construct the Ethernet header
    // Destination MAC
    unsigned char dest_mac[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff}; // Broadcast

    memcpy(sendbuf, dest_mac, 6);
    tx_len += 6;

    // Source MAC
    memcpy(sendbuf + tx_len, if_mac.ifr_hwaddr.sa_data, 6);
    tx_len += 6;

    // EtherType field (custom value)
    sendbuf[tx_len++] = 0x08;
    sendbuf[tx_len++] = 0x00; // IPv4

    // Payload data
    const char *payload = "Hello, Ethernet!";
    int payload_len = strlen(payload);
    memcpy(sendbuf + tx_len, payload, payload_len);
    tx_len += payload_len;

    // Prepare sockaddr_ll
    memset(&socket_address, 0, sizeof(struct sockaddr_ll));
    socket_address.sll_ifindex = if_idx.ifr_ifindex;
    socket_address.sll_halen = ETH_ALEN;
    memcpy(socket_address.sll_addr, dest_mac, 6);

    // Send packet
    if (sendto(sockfd, sendbuf, tx_len, 0,
               (struct sockaddr *)&socket_address, sizeof(struct sockaddr_ll)) < 0) {
        perror("sendto");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Packet sent successfully\n");
    close(sockfd);
    return 0;
}
