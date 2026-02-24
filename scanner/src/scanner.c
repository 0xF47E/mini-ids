#include "arp_ping.h"
#include "ip.h"
#include "log.h"
#include "ports.h"
#include "util.h"
#include <argp.h>
#include <stdlib.h>
#include <time.h>

const char *argp_program_version = "scanner v0.1";
const char *argp_program_bug_address = "...";
static char doc[] = "Minimal CLI scanner";

static struct argp_option options[] = {
    {"target", 't', "TARGET", 0, "Target IP or hostname"},
    {"ports", 'p', "PORTS", 0, "Comma-separated list of ports"},
    {"debug", 'd', "DEBUG", 0, "Non-zero for debug prints stderr"},
    {0}};

struct arguments {
  char *target;
  char *ports;
  int *debug;
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
  case 'd':
    arguments->debug = (int *)arg;
  default:
    return ARGP_ERR_UNKNOWN;
  }
  return 0;
}

int debug = 0;

int main(int argc, char **argv) {
  struct arguments arguments = {0};
  struct argp argp = {options, parse_opt, 0, doc};

  argp_parse(&argp, argc, argv, 0, 0, &arguments);

  // Checks if debug flag is set.
  if (arguments.debug) {
    debug = 1;
  }

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
  DEBUG_PRINT("Starting parsing ports.");
  int return_code = parse_ports(arguments.ports, &portList);
  if (return_code != 0) {
    ERROR_PRINT("Parsing ports failed.");
    exit(1);
  }

  // Sort port array
  sort_port_array(&portList);

  // ----- Scanning -----

  // Parse targets
  return_code = parse_ips(arguments.target, &portList, currentTime);
  if (return_code) {
    ERROR_PRINT("Parsing IP addresses failed.");
    exit(1);
  }

  free_ports(&portList);
  exit(0);
}
