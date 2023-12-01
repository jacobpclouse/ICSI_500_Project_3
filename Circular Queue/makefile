CC = gcc
CFLAGS = -Wall

# names of files
QUEUE = 4_queue_of_queues_intext
CHARS = 2_Circular_queue_chars
INTEGER = 1_Circular_queue

all: $(CHARS) $(INTEGER) $(QUEUE)

$(QUEUE): $(QUEUE_SRC)
	$(CC) $(CFLAGS) -o $(QUEUE) $(QUEUE).c 

$(INTEGER): $(INTEGER_SRC)
	$(CC) $(CFLAGS) -o $(INTEGER) $(INTEGER).c 

$(CHARS): $(CHARS_SRC)
	$(CC) $(CFLAGS) -o $(CHARS) $(CHARS).c 


.PHONY: clean

clean: 
	rm -f $(INTEGER) $(CHARS) $(QUEUE)