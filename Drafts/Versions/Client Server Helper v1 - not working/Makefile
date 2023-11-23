CC = gcc
CFLAGS = -Wall
LIBS = -lpthread
FUNC = encDec.c

# Server and Client executables
SERVER = server
CLIENT = client
HELPER = helper

all: $(SERVER) $(CLIENT) $(HELPER)

$(SERVER): $(SERVER_SRC)
	$(CC) $(CFLAGS) -o $(SERVER) $(SERVER).c

$(CLIENT): $(CLIENT_SRC)
	$(CC) $(CFLAGS) -o $(CLIENT) $(CLIENT).c

$(HELPER): $(HELPER).c
	$(CC) $(CFLAGS) -o $(HELPER) $(HELPER).c

.PHONY: clean

clean:
	rm -f $(SERVER) $(CLIENT) $(HELPER)