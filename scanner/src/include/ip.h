#include "ports.h"
#include <stddef.h>
#include <stdint.h>
#include <time.h>

uint32_t cidr_to_mask(const char *cidr);

void ip_to_buf(uint32_t ip, char *buf);

void handle_ip(const char *ip, struct port_list ports, time_t scan_time);

int iterate_single(const char *ip, struct port_list ports, time_t scan_time);

int iterate_cidr(const char *ip, struct port_list ports, time_t scan_time);

int parse_ips(const char *arg, struct port_list *ports, time_t scan_time);
