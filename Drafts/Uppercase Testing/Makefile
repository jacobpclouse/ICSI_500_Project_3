CC = gcc
CFLAGS = -pthread

all: case_converter aeiouThreadUppercase inputStringThreads

case_converter: case_converter.c
	$(CC) $(CFLAGS) -o case_converter case_converter.c

aeiouThreadUppercase: aeiouThreadUppercase.c
	$(CC) $(CFLAGS) -o aeiouThreadUppercase aeiouThreadUppercase.c

inputStringThreads: inputStringThreads.c
	$(CC) $(CFLAGS) -o inputStringThreads inputStringThreads.c

clean:
	rm -f case_converter aeiouThreadUppercase inputStringThreads
