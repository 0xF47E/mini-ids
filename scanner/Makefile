CC=gcc
CFLAGS=-Wall -Wextra -O2 -Wno-missing-field-initializers

scanner: src/scanner.c src/tcp_connect.c
	$(CC) $(CFLAGS) -o scanner src/scanner.c src/tcp_connect.c

clean:
	rm -f scanner
