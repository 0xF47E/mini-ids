#ifndef TCP_CONNECT_H_
#define TCP_CONNECT_H_

enum port_state {
  PORT_OPEN,
  PORT_CLOSED,
  PORT_FILTERED,
  PORT_ERROR
};

enum port_state tcp_connect(char addr[], int port);

#endif
