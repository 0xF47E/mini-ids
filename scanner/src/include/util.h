#include "tcp_connect.h"
#include <time.h>

void print_result_json(time_t ts, const char *target, int port,
                       enum port_state state);
