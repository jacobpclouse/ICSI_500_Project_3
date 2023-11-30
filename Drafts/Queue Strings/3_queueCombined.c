#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// edit this code, add in the functions and split up how we queue and uppercase the data

#define MAX_SIZE 5

// Define the structure for the queue node
typedef struct QueueNode {
    char* data;
    struct QueueNode* next;
} QueueNode;

// Define the structure for the queue
typedef struct {
    QueueNode* head;
    QueueNode* tail;
} Queue;

// Function to initialize an empty queue
void initializeQueue(Queue* queue) {
    queue->head = NULL;
    queue->tail = NULL;
}

// Function to enqueue a string
void enqueue(Queue* queue, const char* data) {
    // Check if the queue is full
    if (queue->head && queue->tail && queue->tail->next == queue->head) {
        printf("Queue is full. Cannot enqueue.\n");
        return;
    }

    // Create a new node
    QueueNode* newNode = (QueueNode*)malloc(sizeof(QueueNode));
    if (!newNode) {
        printf("Memory allocation error.\n");
        return;
    }

    // Allocate memory for the string and copy data
    newNode->data = strdup(data);
    newNode->next = NULL;

    // Check if the queue is empty
    if (queue->head == NULL) {
        queue->head = newNode;
        queue->tail = newNode;
    } else {
        // Enqueue the new node
        queue->tail->next = newNode;
        queue->tail = newNode;
    }
}

// Function to dequeue a string
char* dequeue(Queue* queue) {
    // Check if the queue is empty
    if (queue->head == NULL) {
        printf("Queue is empty. Cannot dequeue.\n");
        return NULL;
    }

    // Dequeue the front node
    QueueNode* front = queue->head;
    queue->head = front->next;

    // Retrieve the data
    char* data = front->data;

    // Free the memory for the node
    free(front);

    return data;
}

// Function to free the memory occupied by the queue
void freeQueue(Queue* queue) {
    while (queue->head != NULL) {
        QueueNode* temp = queue->head;
        queue->head = temp->next;
        free(temp->data);
        free(temp);
    }
    queue->tail = NULL;
}

int main() {
    Queue myQueue;
    initializeQueue(&myQueue);

    // Example usage
    enqueue(&myQueue, "First");
    enqueue(&myQueue, "Second");
    enqueue(&myQueue, "Third");

    while (myQueue.head != NULL) {
        char* data = dequeue(&myQueue);
        printf("Dequeued: %s\n", data);
        free(data);
    }

    freeQueue(&myQueue);

    return 0;
}
