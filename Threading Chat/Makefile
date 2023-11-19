CC = gcc
CFLAGS = -Wall
LIBS = -lpthread
FUNC = encDec.c

# Server and Client executables
SERVER = server
CLIENT = client

# Source files for server and client
SERVER_SRC = 1_server.c
CLIENT_SRC = 2_client.c

all: $(SERVER) $(CLIENT)

$(SERVER): $(SERVER_SRC)
	$(CC) $(CFLAGS) -o $(SERVER) $(SERVER_SRC) $(FUNC) $(LIBS)

$(CLIENT): $(CLIENT_SRC)
	$(CC) $(CFLAGS) -o $(CLIENT) $(CLIENT_SRC) $(FUNC)

.PHONY: clean

clean:
	rm -f $(SERVER) $(CLIENT) *.txt
