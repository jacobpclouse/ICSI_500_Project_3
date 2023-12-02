// # =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// # encDec.h Importing Libraries / Modules / Headers 
// # =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// for client/server/helper:
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <stdint.h>


// # =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// # Constants
// # =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
#ifndef SETUPHEADER_H
#define SETUPHEADER_H

#define BUFFER_SIZE 100 // buffers for helper, server and client
#define HELPER_SERVER_PORT 1994// the port that the helper server will run on - used in both helper and server
#define HELPER_SERVER_IP "127.0.0.1" // IP that the helper server will run on, used primarily in server, might use in helper later
#define MAXIMUM_CONNECTED_CLIENTS 6 // server max number of allowed clients!
#define MAX_THREADS 6 // max queue threads - helper
#define QUEUE_EMPTY '\0' // return when helper at end of queue -helper

// # =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// # Structs
// # =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

// -- Structs for queues for helper to store data --
// pointers to keep track of start and back of queue, also number of allowed entries (5 for us), and size of queue
typedef struct
{
    char *values;
    int head, tail, countOfEntries, size; 
} structForQueueBoi;



// -- Struct to pass data to each thread for uppercasing -- 1_helper.c
// Referenced code here: https://www.geeksforgeeks.org/structures-c/#
struct TheadsForAIEOU
{
    structForQueueBoi *q;
    int threadNumber;
};


// Structure to organize client data - 2_server.c
// Referenced code here: https://www.geeksforgeeks.org/structures-c/#
struct organizedClientData
{
    int socket;
    char name[BUFFER_SIZE];
};



// # =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// # Functions
// # =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

// print out functions:
void myLogo();

void serverConnected();

void connectionTerminated();

void serverConnectedToHelper();
// end of print out functions

#endif
