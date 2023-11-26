CC = gcc
CFLAGS = -pthread

all: case_converter

case_converter: case_converter.c
	$(CC) $(CFLAGS) -o case_converter case_converter.c

clean:
	rm -f case_converter
