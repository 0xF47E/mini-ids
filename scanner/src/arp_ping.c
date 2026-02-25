#include "arp_ping.h"
#include "log.h"
#include <arpa/inet.h>
#include <asm-generic/errno-base.h>
#include <asm-generic/errno.h>
#include <asm-generic/socket.h>
#include <errno.h>
#include <linux/if_ether.h>
#include <linux/if_packet.h>
#include <linux/sockios.h>
#include <net/ethernet.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <netinet/in.h>
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

enum host_state arp_ping(char addr[]) {

  int socket_fd;
  if ((socket_fd = socket(AF_PACKET, SOCK_DGRAM, 0)) < 0) {
    ERROR_PRINT("Socket creation failed.");
    return HOST_ERROR;
  }

  // Get interface details (MAC, IP, index)
  struct ifreq ifr;
  const char *iface = "enp5s0"; // TODO dynamic setting of interface.
  strncpy(ifr.ifr_name, iface, IFNAMSIZ - 1);

  // Get interface index
  if (ioctl(socket_fd, SIOCGIFINDEX, &ifr) < 0) {
    perror("ioctl (SIOCGIFINDEX) failed");
    ERROR_PRINT("ioctl (SIOCGIFINDEX) failed.");
    return HOST_ERROR;
  }

  int ifindex = ifr.ifr_ifindex;

  // Get MAC address
  if (ioctl(socket_fd, SIOCGIFHWADDR, &ifr) < 0) {
    perror("ioctl (SIOCGIFHWADDR) failed");
    ERROR_PRINT("ioctl (SIOCGIFHWADDR) failed.");
    return HOST_ERROR;
  }

  unsigned char *sender_mac = (unsigned char *)ifr.ifr_hwaddr.sa_data;

  // Get IP address
  if (ioctl(socket_fd, SIOCGIFADDR, &ifr) < 0) {
    ERROR_PRINT("ioctl (SIOCGIFADDR) failed.");
    return HOST_ERROR;
  }

  struct sockaddr_in *sender_ip_addr = (struct sockaddr_in *)&ifr.ifr_addr;
  __be32 sender_ip = sender_ip_addr->sin_addr.s_addr;

  // Configure destination (broadcast)
  struct sockaddr_ll dest_addr;
  memset(&dest_addr, 0, sizeof(dest_addr));
  dest_addr.sll_family = AF_PACKET;
  dest_addr.sll_ifindex = ifindex; // Interface index
  dest_addr.sll_protocol = htons(ETH_P_ARP);
  dest_addr.sll_halen = ETH_ALEN;      // MAC length (6)
  memset(dest_addr.sll_addr, 0xff, 6); // Broadcast MAC (ff:ff:ff:ff:ff:ff)

  // Configure source
  struct sockaddr_ll src_addr;
  memset(&src_addr, 0, sizeof(src_addr));
  src_addr.sll_family = AF_PACKET;
  src_addr.sll_ifindex = ifindex;
  src_addr.sll_protocol = htons(ETH_P_ARP);
  src_addr.sll_halen = ETH_ALEN;

  // Bind socket to interface, otherwise ARP replies wont be accepted through
  // the NIC
  if (bind(socket_fd, (const struct sockaddr *)&src_addr, sizeof(src_addr)) <
      0) {
    ERROR_PRINT("Binding socket failed.");
    return HOST_ERROR;
  };

  // FIX: cast ifr into sockaddr_ll to extract sll_addr when getting SIOCGIFADDR
  socklen_t addr_len = sizeof(src_addr);
  getsockname(socket_fd, (const struct sockaddr *)&src_addr, &addr_len);

  // Populate ARP packet
  struct arp_hdr arp_req;
  memset(&arp_req, 0, sizeof(arp_req));

  // ARP header
  arp_req.hw_type = htons(ARPHRD_ETHER); // Hardware type: Ethernet
  arp_req.proto_type = htons(ETH_P_IP);  // Protocol type: IPv4 (0x0800)
  arp_req.hw_len = ETH_ALEN;             // MAC length (6)
  arp_req.proto_len = 4;                 // IP length (4)
  arp_req.op = htons(ARPOP_REQUEST);     // Operation: Request (1)
  memcpy(arp_req.sender_mac, src_addr.sll_addr, 6); // FIX:
  /* memcpy(arp_req.sender_mac, sender_mac, 6);   // Sender MAC */
  arp_req.sender_ip = sender_ip;       // Sender IP
  memset(arp_req.target_mac, 0, 6);    // Target MAC (0s for request)
  arp_req.target_ip = inet_addr(addr); // Target IP

  // =================================
  // ----- SENDING AND RECEIVING -----
  // =================================

  // Send packet
  ssize_t bytes_sent = sendto(socket_fd, &arp_req, sizeof(arp_req), 0,
                              (struct sockaddr *)&dest_addr, sizeof(dest_addr));
  if (bytes_sent < 0) {
    ERROR_PRINT("Sendto failed.");
    close(socket_fd);
    return HOST_ERROR;
  }

  struct timeval timeout;
  timeout.tv_sec = 0;
  timeout.tv_usec = 200;

  errno = 0;
  setsockopt(socket_fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

  struct sockaddr_ll from_addr;
  socklen_t from_len = sizeof(from_addr);

  // Receiving packet
  unsigned char buffer[4096]; // Buffer for incoming packets
  while (1) {
    ssize_t bytes_recv = recvfrom(socket_fd, buffer, sizeof(buffer), 0,
                                  (struct sockaddr *)&from_addr, &from_len);
    DEBUG_PRINT("Receiving %zd bytes.", bytes_recv);

    if (bytes_recv < 0) {
      if (errno == EAGAIN || errno == EWOULDBLOCK) {
        close(socket_fd);
        ERROR_PRINT("Host down.");
        return HOST_DOWN;
      } else {
        ERROR_PRINT("Recvfrom failed.");
        close(socket_fd);
        return HOST_ERROR;
      }
    }

    struct arp_hdr *arp_reply = (struct arp_hdr *)buffer;
    // Check if packet is unicast
    if (from_addr.sll_pkttype != PACKET_HOST) {
      DEBUG_PRINT("Packet not unicast.");
      continue;
    }

    // Check if ARP packet
    if (ntohs(arp_reply->proto_type) != ETH_P_IP) {
      DEBUG_PRINT("Not an ARP packet, Protocol flag: %d",
                  arp_reply->proto_type);
      continue; // Skip if non-ARP packet
    }

    // Check if ARP reply
    if (ntohs(arp_reply->op) != ARPOP_REPLY) {
      DEBUG_PRINT("Not an ARP reply, op flag: %d", arp_reply->op);
      continue; // Skip requests
    }

    // Check if reply is for our target IP
    if ((arp_reply->target_ip != sender_ip) ||
        (arp_reply->sender_ip != inet_addr(addr))) {
      DEBUG_PRINT("ARP reply was not for our IP.");
      continue; // Skip if target is not our IP
    }
    close(socket_fd);
    return HOST_UP;
  }
}
