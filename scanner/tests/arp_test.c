#include "../src/arp_ping.c"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
  enum host_state state = arp_ping("127.0.0.1");
  switch (state) {
  case (HOST_UP):
    printf("up");
  case (HOST_DOWN):
    printf("down");
  case (HOST_ERROR):
    printf("error");
  }
  return EXIT_SUCCESS;
}
