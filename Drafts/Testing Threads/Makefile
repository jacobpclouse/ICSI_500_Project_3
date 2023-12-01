CC = gcc
CFLAGS = -pthread

INPUT1 = passingThreads
INPUT2 = testingThreads

all: $(INPUT1) $(INPUT2)

$(INPUT1): $(INPUT1).c
	$(CC) $(CFLAGS) -o $(INPUT1) $(INPUT1).c

$(INPUT2): $(INPUT2).c
	$(CC) $(CFLAGS) -o $(INPUT2) $(INPUT2).c

clean:
	rm -f $(INPUT1) $(INPUT2)
