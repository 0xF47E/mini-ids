#include "arp_ping.h"
#include "tcp_connect.h"
#include <stdio.h>
#include <time.h>

const char *port_state_to_string(enum port_state state) {
  switch (state) {
  case PORT_OPEN:
    return "open";
  case PORT_CLOSED:
    return "closed";
  case PORT_FILTERED:
    return "filtered";
  case PORT_ERROR:
    return "error";
  default:
    return "unknown";
  }
}

const char *host_state_to_string(enum host_state state) {
  switch (state) {
  case HOST_UP:
    return "up";
  case HOST_DOWN:
    return "down";
  case HOST_ERROR:
    return "error";
  default:
    return "unknown";
  }
}

void print_result_json(time_t ts, const char *target, int port,
                       enum port_state state) {
  printf("{\"ts\":%ld,\"target\":\"%s\",\"port\":%d,\"state\":\"%s\"}\n", ts,
         target, port, port_state_to_string(state));
}
