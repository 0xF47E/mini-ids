#ifndef ARP_PING_H_
#define ARP_PING_H_

enum host_state {
  HOST_UP,
  HOST_DOWN,
  HOST_ERROR
};

enum host_state arp_ping(char addr[]);

#endif
