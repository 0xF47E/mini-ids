#include "../src/arp_ping.c"
#include "../src/util.c"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
  enum host_state state = arp_ping("192.168.50.240");
  printf("%s\n", host_state_to_string(state));
  state = arp_ping("192.168.50.250");
  printf("%s\n", host_state_to_string(state));
  return EXIT_SUCCESS;
}
