// Implementing a Circular Queue in C
// source: https://youtu.be/oyX30WVuEos

// NEED THIS TO STORE STRING CHARS

#include <stdio.h>
#include <limits.h>
#include <stdbool.h>
#include <stdlib.h>

#define QUEUE_EMPTY INT_MIN

typedef struct 
{
	int *values;
	int head, tail, num_entries, size; // pointers to keep track of start and back of queue, also number of allowed entries (5 for us), and size of queue
} queue;


// initalize queue, pass in a pointer to the queue struct and tell it the size we want to allocate
void init_queue(queue *q, int max_size){
	q->size = max_size;
	q->values = malloc(sizeof(int) * q->size);
	q->num_entries = 0; // make it empty at the start
	q->head = 0;
	q->tail = 0;
}


// check to see if our queue is empty
bool queue_empty(queue* q){
	return (q->num_entries == 0);
}

// check to see if we have filled up our queue
bool queue_full(queue* q){
	return (q->num_entries == q->size);
}


// destroy queue to clean up space + prevent memory leaks
void queue_destroy(queue *q){
	free(q->values);
}


// add element to the back of the queue (queue is the queue we are adding to, value is what we want to add to the queue)
bool enqueue(queue *q, int value){

	// check to make sure queue is not full
	if (queue_full(q)){
		return false;
	}

	// if not full, add to the back and increase num of entries and move tail
	q->values[q->tail] = value;
	q->num_entries++;
	q->tail = (q->tail +1) % q->size; // will clock around if reach end of the queue

	return true;
}


// remove item from our queue
int dequeue(queue *q){

	int result;

	// if queue empty, break and say that the queue is empty
	if (queue_empty(q)){
		return QUEUE_EMPTY;
	}

	// if not empty, save result that is the queue's head positions and return it 
	result = q->values[q->head];
	q->head = (q->head + 1) % q->size; // keep track of size
	q->num_entries--;


	return result;
}



// main function
int main() {

	queue q1;

	init_queue(&q1, 5);// create queue and give it a size of 5

	enqueue(&q1, 56);
	enqueue(&q1, 78);
	enqueue(&q1, 23);
	enqueue(&q1, 988);
	enqueue(&q1, 13);
	enqueue(&q1, 8); // should fail, should have full queue


	// run through until nothing left
	int t;
	while ((t = dequeue(&q1)) != QUEUE_EMPTY){
		printf("t = %d\n",t);
	}


    // free memory
    queue_destroy(&q1);
}