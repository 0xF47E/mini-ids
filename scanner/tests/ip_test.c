#include "../src/ip.c"
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
  parse_ips("127.0.0.1/24");
  printf("\n");
  long out = cidr_parsing("127.0.0.1", "24");
  printf("%d\n", out);
  return EXIT_SUCCESS;
}
