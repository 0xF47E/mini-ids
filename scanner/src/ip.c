#include "arp_ping.h"
#include "ports.h"
#include "tcp_connect.h"
#include "util.h"
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>

uint32_t cidr_to_mask(int prefix) {
  if (prefix <= 0 || prefix >= 32)
    return 0;
  return (0xFFFFFFFF << (32 - prefix));
}

void ip_to_buf(uint32_t ip, char *buf, size_t buflen) {
  struct in_addr addr = {.s_addr = ip};
  inet_ntop(AF_INET, &addr, buf, buflen);
}

void handle_ip(const char *ip, struct port_list *ports, time_t scan_time) {
  if (arp_ping(ip) == HOST_UP) {
    int port;
    for (size_t i = 0; i < ports->count; i++) {
      port = ports->ports[i];
      enum port_state state = tcp_connect(ip, port);
      time(&scan_time);
      print_result_json(scan_time, ip, port, state);
    }
  }
}

int iterate_single(const char *ip, struct port_list *ports, time_t scan_time) {
  handle_ip(ip, ports, scan_time);
  return 0;
}

int iterate_cidr(const char *ip, const char *str_mask, struct port_list *ports,
                 time_t scan_time) {
  uint32_t network;
  uint32_t mask;
  uint32_t broadcast;

  inet_pton(AF_INET, ip, &network);
  network = ntohl(network);
  int prefix = atoi(str_mask);
  mask = cidr_to_mask(prefix);
  broadcast = network | ~mask;

  char buf[INET_ADDRSTRLEN];

  for (uint32_t addr = network + 1; addr < broadcast; addr++) {
    uint32_t net_addr = htonl(addr);
    ip_to_buf(net_addr, buf, sizeof(buf));
    handle_ip(buf, ports, scan_time);
  }
  return 0;
}

int parse_ips(const char *arg, struct port_list *ports, time_t scan_time) {
  if (arg == NULL)
    return -1;
  char *ip_string = strdup(arg);
  if (ip_string == NULL) {
    free(ip_string);
    return -1;
  }

  // If IP is CIDR notation cidr parsing and handling else single ip handling

  char *slash = strchr(ip_string, '/');
  if (slash) {
    char *saveptr = NULL;
    char *token = strtok_r(ip_string, "/", &saveptr);
    char *ip = strdup(token);
    token = strtok_r(NULL, "/", &saveptr);
    char *subnetmask = strdup(token);

    // Iterate IPS
    iterate_cidr(ip, subnetmask, ports, scan_time);
    free(ip_string);
    free(ip);
    free(subnetmask);
    return 0;

  } else {
    // Handle single IP
    iterate_single(ip_string, ports, scan_time);

    free(ip_string);
    return 0;
  }
}
