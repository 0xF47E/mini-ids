#include "ports.h"
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int append_port(int **arrp, size_t *countp, size_t *capacityp,
                       int port) {
  if (*countp >= *capacityp) {
    size_t newcap = (*capacityp) * 2;
    int *tmp = realloc(*arrp, newcap * sizeof(int));
    if (!tmp)
      return -1;
    *arrp = tmp;
    *capacityp = newcap;
  }
  (*arrp)[(*countp)++] = port;
  return 0;
}

void free_ports(struct port_list *list) {
  free(list->ports);
  list->ports = NULL;
  list->count = 0;
}

int parse_ports(const char *arg, struct port_list *out) {
  if (arg == NULL)
    return -1;
  char *port_string = strdup(arg);
  if (port_string == NULL)
    return -1;

  size_t capacity = 8;
  int *arr = malloc(capacity * sizeof(int));
  if (arr == NULL) {
    free(port_string);
    return -1;
  }
  size_t count = 0;

  char *saveptr = NULL;
  char *token = strtok_r(port_string, ",", &saveptr);
  while (token) {
    while (*token && isspace((unsigned char)*token))
      token++;
    char *end = token + strlen(token) - 1;
    while (end >= token && isspace((unsigned char)*end))
      *end-- = '\0';

    if (*token == '\0') {
      token = strtok_r(NULL, ",", &saveptr);
      continue;
    }

    char *dash = strchr(token, '-');
    if (dash) {
      /* port ranges */
      *dash = '\0';
      char *a_str = token;
      char *b_str = dash + 1;

      while (*a_str && isspace((unsigned char)*a_str))
        a_str++;
      char *aend = a_str + strlen(a_str) - 1;
      while (aend >= a_str && isspace((unsigned char)*aend))
        *aend-- = '\0';
      while (*b_str && isspace((unsigned char)*b_str))
        b_str++;
      char *bend = b_str + strlen(b_str) - 1;
      while (bend >= b_str && isspace((unsigned char)*bend))
        *bend-- = '\0';

      errno = 0;
      char *p = NULL;
      long aval = strtol(a_str, &p, 10);
      if (p == a_str || *p != '\0' || errno != 0) {
        free(arr);
        free(port_string);
        return -1;
      }
      errno = 0;
      long bval = strtol(b_str, &p, 10);
      if (p == b_str || *p != '\0' || errno != 0) {
        free(arr);
        free(port_string);
        return -1;
      }

      int a = (int)aval;
      int b = (int)bval;

      if (a > b) {
        int tmp = a;
        a = b;
        b = tmp;
      }

      for (int v = a; v <= b; ++v) {
        if (append_port(&arr, &count, &capacity, v) != 0) {
          free(arr);
          free(port_string);
          return -1;
        }
      }
    } else {
      errno = 0;
      char *p = NULL;
      long val = strtol(token, &p, 10);
      if (p == token || *p != '\0' || errno != 0) {
        free(arr);
        free(port_string);
        return -1;
      }
      if (append_port(&arr, &count, &capacity, (int)val) != 0) {
        free(arr);
        free(port_string);
        return -1;
      }
    }

    token = strtok_r(NULL, ",", &saveptr);
  }

  free(port_string);
  out->ports = arr;
  out->count = count;
  return 0;
}

int cmp(const void *a, const void *b) { return (*(int *)a - *(int *)b); }

int sort_port_array(struct port_list *list) {
  int *arr = list->ports;
  int cnt = list->count;

  qsort(arr, cnt, sizeof(arr[0]), cmp);
  return 0;
}
