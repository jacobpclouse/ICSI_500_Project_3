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
#include <math.h>    // ceil function
#include <pthread.h>


// global variables
char uppercasedBuffer[BUFFER_SIZE]; // to store thread data back from queue


// # =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// # Functions
// # =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

// queue functions below:
// source: https://youtu.be/oyX30WVuEos
// -- Function to initalize queue, pass in a pointer to the queue struct and tell it the size we want to allocate
void init_queue(structForQueueBoi *q, int max_size)
{
    q->size = max_size;
    q->values = malloc(sizeof(char) * q->size);
    q->num_entries = 0; // start empty
    q->head = 0;
    q->tail = 0;
}

// -- Function to check to see if our queue is empty --
bool queue_empty(structForQueueBoi *q)
{
    return (q->num_entries == 0);
}

// // -- Function to check to see if we have filled up our queue -- 
// bool queue_full(structForQueueBoi *q)
// {
//     return (q->num_entries == q->size);
// }


// add element to the back of the queue (queue is the queue we are adding to, value is what we want to add to the queue)
bool enqueue(structForQueueBoi *q, char value)
{

    // check to make sure queue is not full
    // if (queue_full(q))
    if (q->num_entries == q->size)
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
char dequeue(structForQueueBoi *q)
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

// -- Function to create threads to break up and uppercase the input string
// Source: https://www.tutorialspoint.com/cprogramming/switch_statement_in_c.htm
// Source 2: https://www.geeksforgeeks.org/mutex-lock-for-linux-thread-synchronization/
// dequeue -> uppercase -> requeue (for loop size of 5)
// caseThread to determine which case we want
void *serverDecoder(void *arg)
{
    struct TheadsForAIEOU *threadData = (struct TheadsForAIEOU *)arg;

    structForQueueBoi *q = threadData->q;
    int currentThreadIDCASE = threadData->threadNumber;

    printf("Starting server decoder for Thread %d!\n", currentThreadIDCASE);

    structForQueueBoi tempQueue;
    init_queue(&tempQueue, q->size);

    // check to see if this causes an infinite loop -- remove while
    while (!queue_empty(q))
    {
        char element = dequeue(q);

        switch (currentThreadIDCASE)
        {
        case 1: // A
            if (element == 'a')
            {
                element = 'A';
            }
            break;

        case 2: // E
            if (element == 'e')
            {
                element = 'E';
            }
            break;

        case 3: // I
            if (element == 'i')
            {
                element = 'I';
            }
            break;

        case 4: // O
            if (element == 'o')
            {
                element = 'O';
            }
            break;

        case 5: // U
            if (element == 'u')
            {
                element = 'U';
            }
            break;

        case 6: // serverencode
            printf(">server encoding!\n");
            break;

        default:
            break;
        }

        enqueue(&tempQueue, element);
    }

    while (!queue_empty(&tempQueue))
    {
        char element = dequeue(&tempQueue);
        enqueue(q, element);
    }

    free(tempQueue.values);

    pthread_exit(NULL);
}

// gets data ready to send back to server
void *serverEncoder(int server_socket, int dataBytesIncoming)
{
    printf("sending back to server.c...\n");
    send(server_socket, uppercasedBuffer, dataBytesIncoming, 0);
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
    printf("1_helper node listening on IP: %d on Port %d...\n", INADDR_ANY, HELPER_SERVER_PORT);

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
            printf("\n=> DATA REQUEST RECIEVED!\n");
        }

        // first enqueue all the data we have
        // buffer is 100, so we need 20 queues
        int queue_size = 5; // Size of each queue
        int num_queues = (int)ceil((double)dataBytesIncoming / queue_size);
        // int num_queues = 20;
        printf("*Data bytes: %d, Queue size: %d, Number of queues: %d\n", dataBytesIncoming, queue_size, num_queues);

        // Create an array of queues dynamically
        structForQueueBoi *queues = malloc(sizeof(structForQueueBoi) * num_queues); // store incoming data from buffer here

        // Initialize each queue in the array
        for (int i = 0; i < num_queues; i++)
        {
            init_queue(&queues[i], queue_size); // input queues initialize
        }

        // INPUT QUEUES: Enqueue received data into the queues
        int indexfortheinputdata = 0;
        for (int i = 0; i < num_queues; i++)
        {
            for (int j = 0; j < queue_size; j++)
            {
                if (indexfortheinputdata < (dataBytesIncoming - 1)) // if not shorter, will screw everything up with carrage return
                {
                    enqueue(&queues[i], datastreamFromMainServer[indexfortheinputdata]);
                    indexfortheinputdata++;
                }
                else
                {
                    // If we reach the end of the received data, fill up with spaces
                    enqueue(&queues[i], '_');
                }
            }
        }
        // ----
        // ENQUEING IS GREAT, NOW WE NEED TO PASS QUEUES INTO THE UPPERCASE THREADING FUNCTION
        // THEN HAVE IT THREAD AND UPPERCASE ACCROSS SUBSEQUENT QUEUES

        for (int i = 0; i < num_queues; i++)
        {
            // int threadCaseSwitcher = 1;
            printf("THREADING...\n");

            // Create an array of thread data structures
            struct TheadsForAIEOU threadData[MAX_THREADS];

            // Initialize each thread data structure with the appropriate values
            for (int j = 0; j < MAX_THREADS; j++)
            {
                threadData[j].q = &queues[i];
                threadData[j].threadNumber = j + 1;
                // threadData[j].threadNumber = threadCaseSwitcher;
            }

            // Create and join threads
            pthread_t threads[MAX_THREADS];
            for (int j = 0; j < MAX_THREADS; j++)
            {
                // first 5 threads - MAKE SURE THIS WORKS
                if (j < MAX_THREADS)
                {
                    pthread_create(&threads[j], NULL, serverDecoder, (void *)&threadData[j]);
                }

            }

            for (int j = 0; j < MAX_THREADS; j++)
            {
                pthread_join(threads[j], NULL);
            }
        }

        // --------- FREE QUEUE

        // print & dequeue 
        int counter = 0; // count / keep track of where we are

        for (int i = 0; i < num_queues; i++)
        {
            printf("\nQueue %d:\n", i + 1);
            for (int j = 0; j < queue_size; j++)
            {
                char t = dequeue(&queues[i]);
                uppercasedBuffer[counter] = t;
                counter++;
                printf("Char #%d --> %c , ", counter, t);
            }
        }

        uppercasedBuffer[counter-1] = '\n';
        uppercasedBuffer[counter] = '\0';

        printf("\nIMPORTANT: Uppercased data: %s\n", uppercasedBuffer);
        // Send the uppercase datastreamFromMainServer back to the main server
        serverEncoder(server_socket, dataBytesIncoming);
        // send(server_socket, uppercasedBuffer, dataBytesIncoming, 0);

        // destroy queues
        for (int i = 0; i < num_queues; i++)
        {
            free(queues[i].values);
        }

        // free memory from queues
        free(queues);

        // Send the uppercase datastreamFromMainServer back to the main server
        // send(server_socket, uppercasedBuffer, dataBytesIncoming, 0);
    }

    // close sockets for cleanup
    close(server_socket);
    close(helperSocket);

    return 0;
}
