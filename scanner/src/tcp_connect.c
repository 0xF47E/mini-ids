#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>

int tcp_connect(char addr[], int port) {

  int status, client_fd;
  struct sockaddr_in serv_addr;
  if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    printf("\n Socket creation error\n");
    return -1;
  }

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(port);

  if (inet_pton(AF_INET, addr, &serv_addr.sin_addr) <= 0) {
    printf("\n Invalid address/ Address not supported\n");
    return -1;
  }

  if ((status = connect(client_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr))) < 0){
    printf("\nConnection Failed\n");
    return -1;
  }

  printf("Connection success");
  return 0;
}
