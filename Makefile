CC = gcc
CFLAGS = -Wall -Wextra -pthread

# binary isimleri
TARGETS = server client

all: $(TARGETS)

server: server.c opcode.h
	$(CC) $(CFLAGS) -o server server.c

client: client.c opcode.h
	$(CC) $(CFLAGS) -o client client.c

clean:
	rm -f $(TARGETS) *.o DATABASE.txt
