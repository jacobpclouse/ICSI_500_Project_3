CC = gcc
CFLAGS = -Wall
LIBS = -lpthread
FUNC = encDec.c
MATH = -lm

# Server and Client executables
SAMPLE = 1_helper
SERVER = 2_server
CLIENT = 3_client

all: $(SERVER) $(CLIENT) $(SAMPLE)

$(SERVER): $(SERVER_SRC)
	$(CC) $(CFLAGS) -o $(SERVER) $(SERVER).c $(LIBS) $(FUNC)

$(CLIENT): $(CLIENT_SRC)
	$(CC) $(CFLAGS) -o $(CLIENT) $(CLIENT).c $(FUNC)


$(SAMPLE): $(SAMPLE).c
	$(CC) $(CFLAGS) -o $(SAMPLE) $(SAMPLE).c $(LIBS) $(FUNC) $(MATH)


.PHONY: clean

clean: 
	rm -f $(SERVER) $(CLIENT) *.txt $(SAMPLE)