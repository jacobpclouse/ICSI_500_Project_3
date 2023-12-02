CC = gcc
CFLAGS = -Wall
LIBS = -lpthread
FUNC = encDec.c
MATH = -lm

# Server and Client executables
HELPER = 1_helper
SERVER = 2_server
CLIENT = 3_client
SAMPLE = 4_queueHelper

all: $(SERVER) $(CLIENT) $(SAMPLE)

$(SERVER): $(SERVER_SRC)
	$(CC) $(CFLAGS) -o $(SERVER) $(SERVER).c $(LIBS) $(FUNC)

$(CLIENT): $(CLIENT_SRC)
	$(CC) $(CFLAGS) -o $(CLIENT) $(CLIENT).c $(FUNC)

# $(HELPER): $(HELPER).c
# 	$(CC) $(CFLAGS) -o $(HELPER) $(HELPER).c $(LIBS) $(FUNC)

$(SAMPLE): $(SAMPLE).c
	$(CC) $(CFLAGS) -o $(SAMPLE) $(SAMPLE).c $(LIBS) $(FUNC) $(MATH)


.PHONY: clean

clean: 
	rm -f $(SERVER) $(CLIENT) *.txt $(SAMPLE)