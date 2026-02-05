#include <time.h>
#include <stdio.h>
#include "tcp_connect.h"

const char *state_to_string(enum port_state state) {
  switch(state) {
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

void print_result_json(
  time_t ts,
  const char *target,
  int port,
  enum port_state state
) {
  printf("{\"ts\":%ld,\"target\":\"%s\",\"port\":%d,\"state\":\"%s\"}", ts, target, port, state_to_string(state));
}
