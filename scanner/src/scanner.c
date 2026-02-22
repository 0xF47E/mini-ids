#include "arp_ping.h"
#include "ip.h"
#include "ports.h"
#include "tcp_connect.h"
#include "util.h"
#include <argp.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

const char *argp_program_version = "scanner v0.1";
const char *argp_program_bug_address = "...";
static char doc[] = "Minimal CLI scanner";

static struct argp_option options[] = {
    {"target", 't', "TARGET", 0, "Target IP or hostname"},
    {"ports", 'p', "PORTS", 0, "Comma-separated list of ports"},
    {0}};

struct arguments {
  char *target;
  char *ports;
};

static error_t parse_opt(int key, char *arg, struct argp_state *state) {
  struct arguments *arguments = state->input;

  switch (key) {
  case 't':
    arguments->target = arg;
    break;
  case 'p':
    arguments->ports = arg;
    break;
  default:
    return ARGP_ERR_UNKNOWN;
  }
  return 0;
}

int main(int argc, char **argv) {
  struct arguments arguments = {0};
  struct argp argp = {options, parse_opt, 0, doc};

  argp_parse(&argp, argc, argv, 0, 0, &arguments);

  // Scan time
  time_t currentTime;

  // Checks if the target argument is given.
  if (!arguments.target) {
    exit(1);
  }

  // Checks if the port argument is given.
  if (!arguments.ports) {
    exit(1);
  }

  // Parse ports
  struct port_list portList;
  int return_code = parse_ports(arguments.ports, &portList);
  if (return_code != 0) {
    perror("Parsing ports failed.\n");
    exit(1);
  }

  // Sort port array
  sort_port_array(&portList);

  // ----- Scanning -----

  // Parse targets
  return_code = parse_ips(arguments.target, &portList, currentTime);
  if (return_code) {
    exit(1);
  }

  exit(0);

  /* Scans the ports*/

  /* for (int i = 0; i < portList.count; i++) {
    port = portList.ports[i];
    enum port_state state = tcp_connect(arguments.target, port);
    time(&currentTime);
    print_result_json(currentTime, arguments.target, port, state);
  } */
  free_ports(&portList);

  exit(0);
}
