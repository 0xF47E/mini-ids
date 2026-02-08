#include <stdlib.h>
#include <stdio.h>
#include <argp.h>
#include <time.h>
#include <string.h>
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

  int port_array[100];
  char port_argument[100];
  time_t currentTime;
  
  /* Checks if the port argument is given. */
  if (arguments.ports) {
    strcpy(port_argument, arguments.ports);
  }

  /* Splits the port arguments by "," */
  char* token = strtok(port_argument, ",");
  int iterator = 0;

  while(token != NULL){
    port_array[iterator] = atoi(token);
    iterator++;
    token = strtok(NULL, " , ");
  }

  /* Scans the ports*/
  int port;

  for (int i = 0;port_array[i]>0;i++) {
    port = port_array[i];
    enum port_state state = tcp_connect(arguments.target, port);
    time(&currentTime);
    print_result_json(currentTime, arguments.target, port, state);
  }

  exit(0);
}
