CC = gcc
CFLAGS = -Wall
LIBS = -lpthread
FUNC = encDec.c

# Server and Client executables
HELPER = uppercase_threads

all: $(HELPER) 

$(SAMPLE): $(SAMPLE).c
	$(CC) $(CFLAGS) -o $(SAMPLE) $(SAMPLE).c $(LIBS)


.PHONY: clean

clean:
	rm -f $(HELPER) 