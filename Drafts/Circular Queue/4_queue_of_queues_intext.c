// CAN PROVIDE SPECIAL INPUT YESS!!!

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#define QUEUE_EMPTY '\0'

typedef struct
{
    char *values;
    int head, tail, num_entries, size;
} queue;

void init_queue(queue *q, int max_size)
{
    q->size = max_size;
    q->values = malloc(sizeof(char) * q->size);
    q->num_entries = 0;
    q->head = 0;
    q->tail = 0;
}

bool queue_empty(queue *q)
{
    return (q->num_entries == 0);
}

bool queue_full(queue *q)
{
    return (q->num_entries == q->size);
}

void queue_destroy(queue *q)
{
    free(q->values);
}

bool enqueue(queue *q, char value)
{
    if (queue_full(q))
    {
        return false;
    }
    q->values[q->tail] = value;
    q->num_entries++;
    q->tail = (q->tail + 1) % q->size;
    return true;
}

char dequeue(queue *q)
{
    char result;
    if (queue_empty(q))
    {
        return QUEUE_EMPTY;
    }
    result = q->values[q->head];
    q->head = (q->head + 1) % q->size;
    q->num_entries--;
    return result;
}

int main()
{
    int num_queues = 20; // Number of queues you want
    int queue_size = 5;  // Size of each queue

    // Create an array of queues dynamically
    queue *queues = malloc(sizeof(queue) * num_queues);

    // Initialize each queue in the array
    for (int i = 0; i < num_queues; i++)
    {
        init_queue(&queues[i], queue_size);
    }

// custom input below ----------
    char input_string[] = "HelloWorld";  // Input string to be distributed among queues
    int str_index = 0;

    // Example usage of queues with characters from the string
    for (int i = 0; i < num_queues; i++) {
        for (int j = 0; j < queue_size; j++) {
            if (input_string[str_index] != '\0') {
                enqueue(&queues[i], input_string[str_index]);
                str_index++;
            } else {
                // If we reach the end of the string, fill up with spaces
                enqueue(&queues[i], ' ');
            }
        }
    }



// -- 
    // Print and dequeue elements from each queue
    for (int i = 0; i < num_queues; i++)
    {
        printf("Queue %d:\n", i + 1);
        char t;
        while ((t = dequeue(&queues[i])) != QUEUE_EMPTY)
        {
            printf("t = %c\n", t);
        }
    }

    // Free memory for each queue
    for (int i = 0; i < num_queues; i++)
    {
        queue_destroy(&queues[i]);
    }

    // Free memory for the array of queues
    free(queues);

    return 0;
}