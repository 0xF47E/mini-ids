#include "../src/tcp_connect.c"
#include "../src/util.c"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <ip> <port>\n", argv[0]);
        return EXIT_FAILURE;
    }

    char *ip = argv[1];
    int port = atoi(argv[2]);

    enum port_state state = tcp_connect(ip, port);
    printf("%s\n", port_state_to_string(state));

    return EXIT_SUCCESS;
}
