#include "../src/include/ip.h"
#include "../src/ip.c"
#include "../src/util.c"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
  struct host_list target_list;
  parse_ips("127.0.0.1", &target_list);
  printf("\n");
  char *target = (char *)target_list.ips[0];
  enum host_state state = arp_ping(target);
  printf("%s\n", host_state_to_string(state));
  long out = cidr_parsing("127.0.0.1", "24");
  printf("%d\n", out);
  return EXIT_SUCCESS;
}
