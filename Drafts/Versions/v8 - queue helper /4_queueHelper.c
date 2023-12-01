// # =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// 1_helper.c
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
#include <pthread.h> // threading for the aieou stuff
#include <stdbool.h> // for queue

char uppercasedBuffer[BUFFER_SIZE]; // to store thread data
pthread_mutex_t mutex;

// queue struct and variables
#define QUEUE_EMPTY '\0'

typedef struct
{
    char *values;
    int head, tail, num_entries, size; // pointers to keep track of start and back of queue, also number of allowed entries (5 for us), and size of queue
} queue;

// # =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// # Functions
// # =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

// -- Function to create threads to break up and uppercase the input string
// Source: https://www.tutorialspoint.com/cprogramming/switch_statement_in_c.htm
// Source 2: https://www.geeksforgeeks.org/mutex-lock-for-linux-thread-synchronization/
void *serverDecoder(void *arg)
{
    TheadsForAIEOU *inputDataStreamToUppercase = (TheadsForAIEOU *)arg;

    pthread_mutex_lock(&mutex); // lock the mutex so we can access the shared buffer

    // we uppercase based on thread id, pass data between them
    // Source: https://www.tutorialspoint.com/cprogramming/switch_statement_in_c.htm
    switch (inputDataStreamToUppercase->thread_id)
    {
    case 1:
        for (int i = 0; i < BUFFER_SIZE && uppercasedBuffer[i] != '\0'; ++i)
        {
            if (uppercasedBuffer[i] == 'a')
            {
                uppercasedBuffer[i] = 'A';
            }
        }
        // printf("charA thread output string: %s\n", uppercasedBuffer);

        break;
    case 2:
        for (int i = 0; i < BUFFER_SIZE && uppercasedBuffer[i] != '\0'; ++i)
        {
            if (uppercasedBuffer[i] == 'e')
            {
                uppercasedBuffer[i] = 'E';
            }
        }

        // printf("charE thread output string: %s\n", uppercasedBuffer);
        break;
    case 3:
        for (int i = 0; i < BUFFER_SIZE && uppercasedBuffer[i] != '\0'; ++i)
        {
            if (uppercasedBuffer[i] == 'o')
            {
                uppercasedBuffer[i] = 'O';
            }
        }

        // printf("charO thread output string: %s\n", uppercasedBuffer);
        break;
    case 4:
        for (int i = 0; i < BUFFER_SIZE && uppercasedBuffer[i] != '\0'; ++i)
        {
            if (uppercasedBuffer[i] == 'i')
            {
                uppercasedBuffer[i] = 'I';
            }
        }

        // printf("charI thread output string: %s\n", uppercasedBuffer);
        break;
    case 5:
        for (int i = 0; i < BUFFER_SIZE && uppercasedBuffer[i] != '\0'; ++i)
        {
            if (uppercasedBuffer[i] == 'u')
            {
                uppercasedBuffer[i] = 'U';
            }
        }

        // printf("charU thread output string: %s\n", uppercasedBuffer);
        break;
    default:
        break;
    }

    pthread_mutex_unlock(&mutex); // unlock
    printf("Thread %d: %s\n", inputDataStreamToUppercase->thread_id, uppercasedBuffer);

    // iterate through and pass data from string to string
    if (inputDataStreamToUppercase->thread_id < 5) // queue here
    {
        TheadsForAIEOU nextThreadData = {inputDataStreamToUppercase->thread_id + 1};
        pthread_t nextThread;
        pthread_create(&nextThread, NULL, serverDecoder, &nextThreadData);
        pthread_join(nextThread, NULL);

        // NEED TO HAVE ServerEncoder HERE!
    }

    pthread_exit(NULL);
}

// queue functions below:
// source: https://youtu.be/oyX30WVuEos
// initalize queue, pass in a pointer to the queue struct and tell it the size we want to allocate
void init_queue(queue *q, int max_size)
{
    q->size = max_size;
    q->values = malloc(sizeof(char) * q->size);
    q->num_entries = 0; // start empty
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

// # =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// # MAIN
// # =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
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

    // main communication loop
    while (1)
    {

        char datastreamFromMainServer[BUFFER_SIZE];
        int dataBytesIncoming = recv(server_socket, datastreamFromMainServer, BUFFER_SIZE, 0);
        if (dataBytesIncoming <= 0)
        {
            connectionTerminated();
            break;
        }
        else
        {
            // printf("\n=> DATA REQUEST RECIEVED!\n");
            printf("\n=> DATA REQUEST RECIEVED! Received data size: %d bytes\n", dataBytesIncoming);
        }

        // first enqueue all the data we have
        // buffer is 100, so we need 20 queues

        int num_queues = 20; // Number of queues you want
        int queue_size = 5;  // Size of each queue

        // Create an array of queues dynamically
        queue *queues = malloc(sizeof(queue) * num_queues);

        // Initialize each queue in the array
        for (int i = 0; i < num_queues; i++)
        {
            init_queue(&queues[i], queue_size);
        }
        // Enqueue received data into the queues
        int str_index = 0;
        for (int i = 0; i < num_queues; i++)
        {
            for (int j = 0; j < queue_size; j++)
            {
                if (str_index < dataBytesIncoming)
                {
                    enqueue(&queues[i], datastreamFromMainServer[str_index]);
                    str_index++;
                }
                else
                {
                    // If we reach the end of the received data, fill up with spaces
                    enqueue(&queues[i], ' ');
                }
            }
        }

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

        /*
                // -----
                // USE THREADING TO UPPERCASE DATA - Start!
                // -----

                pthread_mutex_init(&mutex, NULL); // initialize

                // Lock the mutex to access the shared buffer
                pthread_mutex_lock(&mutex);

                // Copy the server data to the shared buffer
                strncpy(uppercasedBuffer, datastreamFromMainServer, BUFFER_SIZE - 1);
                uppercasedBuffer[BUFFER_SIZE - 1] = '\0';

                // Unlock the mutex
                pthread_mutex_unlock(&mutex);

                // Create the first thread
                TheadsForAIEOU firstThreadData = {1};
                pthread_t firstThread;
                pthread_create(&firstThread, NULL, serverDecoder, &firstThreadData);
                pthread_join(firstThread, NULL);

                // Destroy mutex
                pthread_mutex_destroy(&mutex);

                // -----
                // USE THREADING TO UPPERCASE DATA - End!
                // -----
        */

        // Send the uppercase datastreamFromMainServer back to the main server
        // send(server_socket, uppercasedBuffer, dataBytesIncoming, 0);
    }

    // close sockets for cleanup
    close(server_socket);
    close(helperSocket);

    return 0;
}
