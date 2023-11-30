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

#define NUM_THREADS 5

char uppercasedBuffer[BUFFER_SIZE]; // to store thread data
pthread_mutex_t mutex;

// Define a structure for the queue
typedef struct
{
    int indices[BUFFER_SIZE];
    int front, rear;
} IndexQueue;

IndexQueue indexQueue;      // Global queue for indices
pthread_mutex_t queueMutex; // Mutex for the index queue

// # =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// # Functions
// # =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

// Initialize the index queue
void initializeQueue()
{
    indexQueue.front = -1;
    indexQueue.rear = -1;
}

// Enqueue an index to the queue
void enqueueIndex(int index)
{
    pthread_mutex_lock(&queueMutex);
    if (indexQueue.front == -1)
        indexQueue.front = 0;
    indexQueue.rear++;
    indexQueue.indices[indexQueue.rear] = index;
    pthread_mutex_unlock(&queueMutex);
}

// Dequeue an index from the queue
int dequeueIndex()
{
    int index;
    pthread_mutex_lock(&queueMutex);
    index = indexQueue.indices[indexQueue.front];
    indexQueue.front++;
    if (indexQueue.front > indexQueue.rear)
    {
        indexQueue.front = -1;
        indexQueue.rear = -1;
    }
    pthread_mutex_unlock(&queueMutex);
    return index;
}

// -- Function to create threads to break up and uppercase the input string
// Source: https://www.tutorialspoint.com/cprogramming/switch_statement_in_c.htm
// Source 2: https://www.geeksforgeeks.org/mutex-lock-for-linux-thread-synchronization/
void *serverDecoder(void *arg) {
    printf("-- Inside ServerDecoder! --");
    TheadsForAIEOU *inputDataStreamToUppercase = (TheadsForAIEOU *)arg;

    int currentIndex = inputDataStreamToUppercase->thread_id;

    while (currentIndex < BUFFER_SIZE && uppercasedBuffer[currentIndex] != '\0') {
        char currentChar = uppercasedBuffer[currentIndex];

        pthread_mutex_lock(&mutex);
        switch (currentChar) {
            case 'a':
                uppercasedBuffer[currentIndex] = 'A';
                break;
            case 'e':
                uppercasedBuffer[currentIndex] = 'E';
                break;
            case 'i':
                uppercasedBuffer[currentIndex] = 'I';
                break;
            case 'o':
                uppercasedBuffer[currentIndex] = 'O';
                break;
            case 'u':
                uppercasedBuffer[currentIndex] = 'U';
                break;
            // Add more cases for other characters if needed
        }
        pthread_mutex_unlock(&mutex);

        currentIndex = dequeueIndex();

        if (currentIndex != -1) {
            pthread_mutex_lock(&mutex);
            printf("Thread %d: %s\n", inputDataStreamToUppercase->thread_id, uppercasedBuffer);
            pthread_mutex_unlock(&mutex);
        }
    }

    // Free the dynamically allocated memory before exiting the thread
    free(inputDataStreamToUppercase);

    pthread_exit(NULL);
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

    initializeQueue(); // Initialize the index queue

    serverConnected(); // shows main 2_server has connected

    // main communication loop
    while (1)
    {
        char datastreamFromMainServer[BUFFER_SIZE];
        // int dataBytesIncoming = recv(server_socket, datastreamFromMainServer, BUFFER_SIZE, 0);

        // char datastreamFromMainServer[BUFFER_SIZE];
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

        // -----
        // USE THREADING TO UPPERCASE DATA - Start!
        // -----

        pthread_mutex_lock(&queueMutex);

        // Enqueue indices to the queue
        for (int i = 0; i < BUFFER_SIZE; ++i)
        {
            enqueueIndex(i);
        }

        pthread_mutex_unlock(&queueMutex);

        // Create threads for each character
        pthread_t threads[NUM_THREADS];
        for (int i = 0; i < NUM_THREADS; ++i)
        {
            TheadsForAIEOU *threadData = malloc(sizeof(TheadsForAIEOU));
            threadData->thread_id = i + 1;
            pthread_create(&threads[i], NULL, serverDecoder, threadData);
        }

        // Wait for all threads to finish
        for (int i = 0; i < NUM_THREADS; ++i)
        {
            pthread_join(threads[i], NULL);
        }

        // Send the uppercase datastreamFromMainServer back to the main server
        send(server_socket, uppercasedBuffer, dataBytesIncoming, 0);
    }
    // close sockets for cleanup
    close(server_socket);
    close(helperSocket);

    return 0;
}