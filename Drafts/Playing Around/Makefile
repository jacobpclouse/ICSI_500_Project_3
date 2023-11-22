CC = gcc
CFLAGS = -Wall
LIBS = -lpthread
FUNC = encDec.c

# Server and Client executables
SERVER = 1_server
CLIENT = 2_client
HELPER = helper
SAMPLE = sampleThreads

all: $(SERVER) $(CLIENT)

$(SERVER): $(SERVER_SRC)
	$(CC) $(CFLAGS) -o $(SERVER) $(SERVER).c $(LIBS)

$(CLIENT): $(CLIENT_SRC)
	$(CC) $(CFLAGS) -o $(CLIENT) $(CLIENT).c $(LIBS)

$(HELPER): $(HELPER).c
	$(CC) $(CFLAGS) -o $(HELPER) $(HELPER).c

$(SAMPLE): $(SAMPLE).c
	$(CC) $(CFLAGS) -o $(SAMPLE) $(SAMPLE).c $(LIBS)


.PHONY: clean

clean:
	rm -f $(SERVER) $(CLIENT) $(HELPER)