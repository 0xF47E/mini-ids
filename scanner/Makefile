CC=gcc
CFLAGS=-Wall -Wextra -O2 -Wno-missing-field-initializers -g -fsanitize=address -Isrc/include

scanner: src/scanner.c src/tcp_connect.c src/util.c src/ports.c
	$(CC) $(CFLAGS) -o scanner src/scanner.c src/tcp_connect.c src/util.c src/ports.c

clean:
	rm -f scanner
