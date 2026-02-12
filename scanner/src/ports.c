#include <stdlib.h>
#include <string.h>
#include "ports.h"

int parse_ports(const char *arg, struct port_list *out) {

  char *port_string = strdup(arg);
  if (port_string == NULL) {
    free(port_string);
    return -1;
  }

  size_t capacity = 4;
  int *arr = malloc(capacity * sizeof(int));
  if (arr == NULL) {
    free(port_string);
    return -1;
  }
  size_t count = 0;
  char *token = strtok(port_string, ",");
  while (token) {
    if (count >= capacity) {
      capacity *= 2;
      int *new_arr = realloc(arr, capacity * sizeof(int));
      if (new_arr == NULL) {
        free(arr);
        free(port_string);
        return -1;
      }
      arr = new_arr;
    }
    arr[count++] = atoi(token);
    token = strtok(NULL, ",");
  }

  free(port_string);
  out->ports = arr;
  out->count = count;
  return 0;
}

void free_ports(struct port_list *list) {
  free(list->ports);
  list->ports = NULL;
  list->count = 0;
}
