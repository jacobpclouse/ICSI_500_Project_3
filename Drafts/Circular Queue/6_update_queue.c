// Implementing a Circular Queue in C
// source: https://youtu.be/oyX30WVuEos

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#define QUEUE_EMPTY '\0'

// #define QUEUE_EMPTY INT_MIN

typedef struct
{
	char *values;
	int head, tail, num_entries, size; // pointers to keep track of start and back of queue, also number of allowed entries (5 for us), and size of queue
} queue;

// initalize queue, pass in a pointer to the queue struct and tell it the size we want to allocate
void init_queue(queue *q, int max_size)
{
	q->size = max_size;
	q->values = malloc(sizeof(int) * q->size);
	q->num_entries = 0; // make it empty at the start
	q->head = 0;
	q->tail = 0;
}

// check to see if our queue is empty
bool queue_empty(queue *q)
{
	return (q->num_entries == 0);
}

// check to see if we have filled up our queue
bool queue_full(queue *q)
{
	return (q->num_entries == q->size);
}

// destroy queue to clean up space + prevent memory leaks
void queue_destroy(queue *q)
{
	free(q->values);
}

// add element to the back of the queue (queue is the queue we are adding to, value is what we want to add to the queue)
bool enqueue(queue *q, char value)
{

	// check to make sure queue is not full
	if (queue_full(q))
	{
		return false;
	}

	// if not full, add to the back and increase num of entries and move tail
	q->values[q->tail] = value;
	q->num_entries++;
	q->tail = (q->tail + 1) % q->size; // will clock around if reach end of the queue

	return true;
}

// remove item from our queue
char dequeue(queue *q)
{

	char result;

	// if queue empty, break and say that the queue is empty
	if (queue_empty(q))
	{
		return QUEUE_EMPTY;
	}

	// if not empty, save result that is the queue's head positions and return it
	result = q->values[q->head];
	q->head = (q->head + 1) % q->size; // keep track of size
	q->num_entries--;

	return result;
}

// Function to update the queue as specified
void updateQueue(queue *q)
{
    queue tempQueue;
    init_queue(&tempQueue, q->size);

    while (!queue_empty(q))
    {
        char element = dequeue(q);
        
        // Check if the dequeued character is 'a'
        if (element == 'a')
        {
            // Uppercase 'a' and enqueue it back into the original queue
            element = 'A';
        }

        // Enqueue the modified or unmodified element into the temporary queue
        enqueue(&tempQueue, element);
    }

    // Transfer elements back to the original queue
    while (!queue_empty(&tempQueue))
    {
        char element = dequeue(&tempQueue);
        enqueue(q, element);
    }

    // Destroy the temporary queue
    queue_destroy(&tempQueue);
}



// main function
int main()
{

	queue q1;

	init_queue(&q1, 5); // create queue and give it a size of 5

	// only one char per slot
	enqueue(&q1, 'a');
	enqueue(&q1, 'b');
	enqueue(&q1, 'c');
	enqueue(&q1, 'd');
	enqueue(&q1, 'e');
	enqueue(&q1, 'f'); // should fail, should have full queue


	// Update the queue as per the specified function
    updateQueue(&q1);

	// run through until nothing left
	char t;
	while ((t = dequeue(&q1)) != QUEUE_EMPTY)
	{
		printf("t = %c\n", t);
	}

	// free memory
	queue_destroy(&q1);
}