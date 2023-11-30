#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUEUE_SIZE 100

// Structure to represent a node in the queue
struct QueueNode {
    char* data;
    struct QueueNode* next;
};

// Structure to represent the queue
struct Queue {
    struct QueueNode* head;
    struct QueueNode* tail;
};

// Function to initialize an empty queue
void initializeQueue(struct Queue* queue) {
    queue->head = NULL;
    queue->tail = NULL;
}

// Function to check if the queue is empty
int isQueueEmpty(struct Queue* queue) {
    return (queue->head == NULL);
}

// Function to enqueue a string into the queue
void enqueue(struct Queue* queue, const char* data) {
    struct QueueNode* newNode = (struct QueueNode*)malloc(sizeof(struct QueueNode));
    if (newNode == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }

    newNode->data = strdup(data);  // Copy the string to a new memory location
    newNode->next = NULL;

    if (isQueueEmpty(queue)) {
        // If the queue is empty, set both head and tail to the new node
        queue->head = newNode;
        queue->tail = newNode;
    } else {
        // Otherwise, add the new node to the end of the queue
        queue->tail->next = newNode;
        queue->tail = newNode;
    }
}

// Function to dequeue a string from the queue
char* dequeue(struct Queue* queue) {
    if (isQueueEmpty(queue)) {
        fprintf(stderr, "Queue is empty. Cannot dequeue.\n");
        exit(EXIT_FAILURE);
    }

    // Get the data from the head of the queue
    char* data = queue->head->data;

    // Remove the node from the queue
    struct QueueNode* temp = queue->head;
    queue->head = queue->head->next;
    free(temp);

    // If the queue becomes empty, update the tail as well
    if (queue->head == NULL) {
        queue->tail = NULL;
    }

    return data;
}

// Function to free the memory allocated for the queue
void freeQueue(struct Queue* queue) {
    while (!isQueueEmpty(queue)) {
        char* data = dequeue(queue);
        free(data);
    }
}

int main() {
    struct Queue myQueue;
    initializeQueue(&myQueue);

    // Enqueue some strings
    enqueue(&myQueue, "First");
    enqueue(&myQueue, "Second");
    enqueue(&myQueue, "Third");

    // Dequeue and print strings
    while (!isQueueEmpty(&myQueue)) {
        char* data = dequeue(&myQueue);
        printf("Dequeued: %s\n", data);
        free(data);  // Free the memory allocated for the dequeued string
    }

    // Free the memory allocated for the queue
    freeQueue(&myQueue);

    return 0;
}
