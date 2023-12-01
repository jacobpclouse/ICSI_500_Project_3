CC = gcc
CFLAGS = -Wall

# names of files
CHARS = 2_Circular_queue_chars
INTEGER = 1_Circular_queue

all: $(CHARS) $(INTEGER) 

$(INTEGER): $(INTEGER_SRC)
	$(CC) $(CFLAGS) -o $(INTEGER) $(INTEGER).c 

$(CHARS): $(CHARS_SRC)
	$(CC) $(CFLAGS) -o $(CHARS) $(CHARS).c 


.PHONY: clean

clean: 
	rm -f $(INTEGER) $(CHARS)