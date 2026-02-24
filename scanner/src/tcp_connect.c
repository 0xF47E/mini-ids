#include "tcp_connect.h"
<<<<<<< HEAD
=======
#include "log.h"
>>>>>>> b37305b (scanner: arp ping and cidr parsing implementation)
#include <arpa/inet.h>
#include <asm-generic/errno.h>
#include <asm-generic/socket.h>
#include <bits/types/struct_timeval.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>

int connect_with_timeout(int sock, const struct sockaddr *addr,
                         socklen_t addr_len, int timeout) {

  struct timeval tv;
  tv.tv_sec = timeout;
  tv.tv_usec = 0;

  int ret;

  // set socket to non-blocking
  int flags = fcntl(sock, F_GETFL, 0);
  fcntl(sock, F_SETFL, flags | O_NONBLOCK);

  if ((ret = connect(sock, addr, addr_len)) < 0) {
    if (errno == EINPROGRESS) {

      fd_set wfds;
      FD_ZERO(&wfds);
      FD_SET(sock, &wfds);

      ret = select(sock + 1, NULL, &wfds, NULL, &tv);
      fprintf(stderr, "DEBUG: select returned %d, errno=%d\n", ret, errno);
    }
  } else {
    ret = 1;
  }

  if (fcntl(sock, F_SETFL, flags) < 0) {
    return -1;
  }

  if (ret < 0) {
    // connection failed immediately, not in progress
    return -1;
  } else if (ret == 0) {
    errno = ETIMEDOUT;
    return -1;
  } else {
    socklen_t len = sizeof(flags);
    if (getsockopt(sock, SOL_SOCKET, SO_ERROR, &flags, &len) < 0) {
      return -1;
    }

    if (flags) {
      errno = flags;
      return -1;
    }
  }

  return 0;
}
enum port_state tcp_connect(char addr[], int port) {

  int status, client_fd;
  struct sockaddr_in serv_addr;

<<<<<<< HEAD
  fprintf(stderr, "DEBUG: tcp_connect called with addr=%s port=%d\n", addr,
          port);
=======
  DEBUG_PRINT("tcp_connect called with addr=%s port=%d", addr, port);
>>>>>>> b37305b (scanner: arp ping and cidr parsing implementation)

  if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    ERROR_PRINT("Socket creation failed.");
    return PORT_ERROR;
  }

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(port);
  if (inet_pton(AF_INET, addr, &serv_addr.sin_addr) <= 0) {
    ERROR_PRINT("Parsing IP to network byte order failed.");
    return PORT_ERROR;
  }

  if ((status = connect_with_timeout(client_fd, (struct sockaddr *)&serv_addr,
<<<<<<< HEAD
                                     sizeof(serv_addr), 5)) < 0) {
    fprintf(stderr, "DEBUG: connect_with_timeout returned -1, errno=%d (%s)\n",
            errno, strerror(errno));
=======
                                     sizeof(serv_addr), 1)) < 0) {

    ERROR_PRINT("connect_with_timeout returned -1, errno=%d (%s)", errno,
                strerror(errno));
>>>>>>> b37305b (scanner: arp ping and cidr parsing implementation)
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
