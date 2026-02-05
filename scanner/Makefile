CC=gcc
CFLAGS=-Wall -Wextra -O2 -Wno-missing-field-initializers -g -fsanitize=address

scanner: src/scanner.c src/tcp_connect.c src/util.c
	$(CC) $(CFLAGS) -o scanner src/scanner.c src/tcp_connect.c src/util.c

clean:
	rm -f scanner
