// # =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// 4_queueHelper.c
// # =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// # Importing Libraries / Modules / Headers
// # =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
#include "encDec.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
#include <ctype.h> // Include for toupper function

#define QUEUE_SIZE 100
#define NUM_THREADS 5

struct Queue
{
    char *data[QUEUE_SIZE];
    int head;
    int tail;
};

void initializeQueue(struct Queue *queue)
{
    queue->head = -1;
    queue->tail = -1;
}

int isQueueEmpty(struct Queue *queue)
{
    return (queue->head == -1 && queue->tail == -1);
}

int isQueueFull(struct Queue *queue)
{
    return ((queue->tail + 1) % QUEUE_SIZE == queue->head);
}

void enqueue(struct Queue *queue, const char *str)
{
    if (isQueueFull(queue))
    {
        printf("Queue is full. Cannot enqueue.\n");
        return;
    }

    if (isQueueEmpty(queue))
    {
        queue->head = 0;
        queue->tail = 0;
    }
    else
    {
        queue->tail = (queue->tail + 1) % QUEUE_SIZE;
    }

    queue->data[queue->tail] = strdup(str);
}

char *dequeue(struct Queue *queue)
{
    if (isQueueEmpty(queue))
    {
        printf("Queue is empty. Cannot dequeue.\n");
        return NULL;
    }

    char *dequeuedString = queue->data[queue->head];

    if (queue->head == queue->tail)
    {
        queue->head = -1;
        queue->tail = -1;
    }
    else
    {
        queue->head = (queue->head + 1) % QUEUE_SIZE;
    }

    return dequeuedString;
}

void *serverDecoder(void *arg)
{
    struct Queue *queue = (struct Queue *)arg;

    while (!isQueueEmpty(queue))
    {
        char *datastream = dequeue(queue);

        // Process the data as needed (uppercase specific characters)
        for (int i = 0; i < BUFFER_SIZE && datastream[i] != '\0'; ++i)
        {
            // Uppercase logic
            datastream[i] = toupper(datastream[i]);
        }

        // Print the processed data
        printf("Thread: %s\n", datastream);

        // Free the memory allocated for the dequeued string
        free(datastream);
    }

    pthread_exit(NULL);
}

int main()
{
    int helperSocket;
    struct sockaddr_in helper_addr, server_addr;
    socklen_t addr_size = sizeof(server_addr);

    // create socket, configure connection type
    helperSocket = socket(AF_INET, SOCK_STREAM, 0);
    helper_addr.sin_family = AF_INET;
    helper_addr.sin_port = htons(HELPER_SERVER_PORT);
    // helper_addr.sin_addr.s_addr = HELPER_SERVER_IP;
    helper_addr.sin_addr.s_addr = INADDR_ANY;

    // bind socket
    if (bind(helperSocket, (struct sockaddr *)&helper_addr, sizeof(helper_addr)) == -1)
    {
        perror("ERROR: Issue with binding!");
        exit(1);
    }

    // listen for connections
    if (listen(helperSocket, 1) == -1)
    {
        perror("ERROR: Issue with listening!");
        exit(1);
    }

    myLogo(); // startup print outs
    printf("1_helper node listening on IP: %s on Port %d...\n", INADDR_ANY, HELPER_SERVER_PORT);

    // accept incoming connections
    int server_socket = accept(helperSocket, (struct sockaddr *)&server_addr, &addr_size);
    if (server_socket == -1)
    {
        perror("ERROR: Experienced issue accepting connection!");
        exit(1);
    }

    serverConnected(); // shows main 2_server has connected

    // Create and initialize the queue
    struct Queue dataQueue;
    initializeQueue(&dataQueue);

    // Receive data from the main server and enqueue it
    while (1)
    {
        pthread_t threads[NUM_THREADS];
        for (int i = 0; i < NUM_THREADS; ++i)
        {
            pthread_create(&threads[i], NULL, serverDecoder, &dataQueue);
        }

        int receivingData = 1;

        while (receivingData)
        {
            char datastreamFromMainServer[BUFFER_SIZE];
            int dataBytesIncoming = recv(server_socket, datastreamFromMainServer, BUFFER_SIZE, 0);

            if (dataBytesIncoming <= 0)
            {
                connectionTerminated();
                receivingData = 0;
            }
            else
            {
                printf("\n=> DATA REQUEST RECEIVED!\n");
            }

            enqueue(&dataQueue, datastreamFromMainServer);
        }

        for (int i = 0; i < NUM_THREADS; ++i)
        {
            enqueue(&dataQueue, NULL);
        }

        for (int i = 0; i < NUM_THREADS; ++i)
        {
            pthread_join(threads[i], NULL);
        }

        // Send processed data back to the main server
        while (!isQueueEmpty(&dataQueue))
        {
            char *processedData = dequeue(&dataQueue);

            if (processedData == NULL)
            {
                break;
            }

            size_t remainingBytes = strlen(processedData);
            ssize_t sentBytes;
            char *currentPointer = processedData; // Keep track of the current position in the data

            while (remainingBytes > 0)
            {
                sentBytes = send(server_socket, currentPointer, remainingBytes, 0);

                if (sentBytes <= 0)
                {
                    // Handle error or connection closure
                    break;
                }

                remainingBytes -= sentBytes;
                currentPointer += sentBytes;
            }

            // Free the memory allocated for the processed string
            free(processedData);
        }
    }

    close(server_socket);
    close(helperSocket);

    return 0;
}