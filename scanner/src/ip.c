#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "arp_ping.h"

int parse_ips(const char *arg) {
  if (arg == NULL) return -1;
  char *ip_string = strdup(arg);
  if (ip_string == NULL) return -1;

  char *slash = strchr(ip_string, '/');
  if (slash) {
    char *saveptr = NULL;
    char *token = strtok_r(ip_string, "/", &saveptr);
    char *ip = strdup(token);
    token = strtok_r(NULL, "/", &saveptr);
    char *subnetmask = strdup(token);

    printf("%s = %s + / + %s", ip_string, ip, subnetmask);
    return 0;
  }
}

long cidr_parsing(const char *ip, const char *mask) {
  if (ip == NULL) return -1;
  char *ip_string = strdup(ip);
  if (ip_string == NULL) return -1;

  if (mask == NULL) return -1;
  char *subnetmask = strdup(mask);
  if (subnetmask == NULL) return -1;

  char *p = NULL;
  long prefix = strtol(subnetmask, &p, 10);
  long host_bits = 32 - prefix;
  long host_count = 1<<host_bits;
  return host_count;
}


