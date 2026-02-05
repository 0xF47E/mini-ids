#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include "tcp_connect.h"

enum port_state tcp_connect(char addr[], int port) {

  int status, client_fd;
  struct sockaddr_in serv_addr;
  if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    return PORT_ERROR;
  }

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(port);

  if (inet_pton(AF_INET, addr, &serv_addr.sin_addr) <= 0) {
    return PORT_ERROR;
  }

  if ((status = connect(client_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr))) < 0){
    switch (errno) {
      case ECONNREFUSED:
        return PORT_CLOSED;
      case ETIMEDOUT:
        return PORT_FILTERED;
      case EHOSTUNREACH:
        return PORT_FILTERED;
      case ENETUNREACH:
        return PORT_FILTERED;
      default:
        return PORT_ERROR;
    }
  }

  return PORT_OPEN;
}
