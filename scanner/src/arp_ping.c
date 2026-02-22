#include "arp_ping.h"
#include <arpa/inet.h>
#include <asm-generic/errno-base.h>
#include <asm-generic/errno.h>
#include <asm-generic/socket.h>
#include <errno.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>

struct arp_hdr {
  __be16 hw_type;
  __be16 proto_type;
  unsigned char hw_len;
  unsigned char proto_len;
  __be16 op;
  unsigned char sender_mac[6];
  __be32 sender_ip;
  unsigned char target_mac[6];
  __be32 target_ip;
} __attribute__((packed));

struct arp_packet {
  struct ethhdr eth;
  struct arp_hdr arp;
} __attribute__((packed));

enum host_state arp_ping(char addr[]) {

  int status, socket_fd;
  if ((socket_fd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ARP))) < 0) {
    perror("socket problem");
    return HOST_ERROR;
  }

  // Get interface details (MAC, IP, index)
  struct ifreq ifr;
  const char *iface = "enp5s0";
  strncpy(ifr.ifr_name, iface, IFNAMSIZ - 1);

  // Get interface index
  if (ioctl(socket_fd, SIOCGIFINDEX, &ifr) < 0) {
    perror("ioctl (SIOCGIFINDEX) failed");
    return HOST_ERROR;
  }

  int ifindex = ifr.ifr_ifindex;

  // Get MAC address
  if (ioctl(socket_fd, SIOCGIFHWADDR, &ifr) < 0) {
    perror("ioctl (SIOCGIFHWADDR) failed");
    return HOST_ERROR;
  }

  unsigned char *sender_mac = (unsigned char *)ifr.ifr_hwaddr.sa_data;

  // Get IP address
  if (ioctl(socket_fd, SIOCGIFADDR, &ifr) < 0) {
    perror("ioctl (SIOCGIFADDR) failed");
    return HOST_ERROR;
  }

  struct sockaddr_in *sender_ip_addr = (struct sockaddr_in *)&ifr.ifr_addr;
  __be32 sender_ip = sender_ip_addr->sin_addr.s_addr;

  // Configure destination (broadcast)
  struct sockaddr_ll socket_addr;
  memset(&socket_addr, 0, sizeof(socket_addr));
  socket_addr.sll_family = ARPHRD_ETHER;
  socket_addr.sll_ifindex = ifindex;     // Interface index
  socket_addr.sll_halen = ETH_ALEN;      // MAC length (6)
  memset(socket_addr.sll_addr, 0xff, 6); // Broadcast MAC (ff:ff:ff:ff:ff:ff)

  // Populate ARP packet
  struct arp_packet arp_req;
  memset(&arp_req, 0, sizeof(arp_req));

  // Ethernet header
  memset(arp_req.eth.h_dest, 0xff, 6);         // Broadcast MAC
  memcpy(arp_req.eth.h_source, sender_mac, 6); // Source MAC
  arp_req.eth.h_proto = htons(ETH_P_ARP);      // EtherType: ARP (0x0806)

  // ARP header
  arp_req.arp.hw_type = htons(ARPHRD_ETHER);     // Hardware type: Ethernet
  arp_req.arp.proto_type = htons(ETH_P_IP);      // Protocol type: IPv4 (0x0800)
  arp_req.arp.hw_len = ETH_ALEN;                 // MAC length (6)
  arp_req.arp.proto_len = 4;                     // IP length (4)
  arp_req.arp.op = htons(ARPOP_REQUEST);         // Operation: Request (1)
  memcpy(arp_req.arp.sender_mac, sender_mac, 6); // Sender MAC
  arp_req.arp.sender_ip = sender_ip;             // Sender IP
  memset(arp_req.arp.target_mac, 0, 6);          // Target MAC (0s for request)
  arp_req.arp.target_ip = inet_addr(addr);       // Target IP

  // ----- SENDING AND RECEIVING -----

  // Send packet
  ssize_t bytes_sent =
      sendto(socket_fd, &arp_req, sizeof(arp_req), 0,
             (struct sockaddr *)&socket_addr, sizeof(socket_addr));
  if (bytes_sent < 0) {
    perror("sendto failed");
    return HOST_ERROR;
  }

  struct timeval timeout;
  timeout.tv_sec = 0;
  timeout.tv_usec = 5;

  errno = 0;
  setsockopt(socket_fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

  // Receiving packet
  unsigned char buffer[4096]; // Buffer for incoming packets
  while (1) {
    ssize_t bytes_recv =
        recvfrom(socket_fd, buffer, sizeof(buffer), 0, NULL, NULL);

    if (errno == EAGAIN || errno == EWOULDBLOCK) {
      close(socket_fd);
      return HOST_DOWN;
    }

    if (bytes_recv < 0) {
      perror("recvfrom failed");
      close(socket_fd);
      return HOST_ERROR;
    }

    struct arp_packet *arp_reply = (struct arp_packet *)buffer;

    // Check if ARP packet
    if (ntohs(arp_reply->eth.h_proto) != ETH_P_ARP) {
      continue; // Skip if non-ARP packet
    }

    // Check if ARP reply
    if (ntohs(arp_reply->arp.op) != ARPOP_REPLY) {
      continue; // Skip requests
    }

    // Check if reply is for our target IP
    if (arp_reply->arp.target_ip != sender_ip) {
      continue; // Skip if target is not our IP
    }
    close(socket_fd);
    return HOST_UP;
  }
}
