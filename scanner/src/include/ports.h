#include <stddef.h>

struct port_list {
  int *ports;
  size_t count;
};

int parse_ports(const char *arg, struct port_list *out);

void free_ports(struct port_list *list);

int sort_port_array(struct port_list *list);
