#include <stdlib.h>
#include <stdio.h>
#include <argp.h>
#include <time.h>
#include "tcp_connect.h"
#include "util.h"

const char *argp_program_version = "scanner v0.1";
const char *argp_program_bug_address = "...";
static char doc[] = "Minimal CLI scanner";

static struct argp_option options[] = {
  {"target",  't',  "TARGET", 0,  "Target IP or hostname"},
  {"ports",   'p',  "PORTS",  0,  "Comma-separated list of ports"},
  {0}
};

struct arguments
{
  char *target;
  char *ports;
};

static error_t parse_opt(int key, char *arg, struct argp_state *state) {
  struct arguments *arguments = state->input;

  switch(key) {
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

int main(int argc, char **argv)
{
  struct arguments arguments = {0};
  struct argp argp = {options, parse_opt, 0, doc};
  
  argp_parse(&argp, argc, argv, 0, 0, &arguments);

/* printf("Target: %s\n", arguments.target ? arguments.target : "(none)"); */
/* printf("Ports: %s\n", arguments.ports ? arguments.ports : "(none)"); */

  int port = 0;
  time_t currentTime;
  if (arguments.ports) {
    port = atoi(arguments.ports);
  }

  enum port_state state = tcp_connect(arguments.target, port);
  time(&currentTime); // Get current time
  print_result_json(currentTime, arguments.target, port, state);

  exit(0);
}
