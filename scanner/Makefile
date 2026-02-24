CC=gcc
CFLAGS=-Wall -Wextra -O2 -Wno-missing-field-initializers -g -fsanitize=address -Isrc/include

scanner: src/scanner.c src/tcp_connect.c src/util.c src/ports.c src/ip.c src/arp_ping.c
	$(CC) $(CFLAGS) -o scanner src/scanner.c src/tcp_connect.c src/util.c src/ports.c src/ip.c src/arp_ping.c

clean:
	rm -f scanner

tcp_test: tests/tcp_test.c src/tcp_connect.c src/util.c
	$(CC) $(CFLAGS) -o tests/tcp_test tests/tcp_test.c
