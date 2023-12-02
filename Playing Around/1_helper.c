// # =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// 1_helper.c
// # =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// # Importing Libraries / Modules / Headers
// # =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
#include "encDec.h" // all imports in header


// # =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// # Global Variables
// # =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
char uppercasedBuffer[BUFFER_SIZE]; // to store thread data back from queue

// # =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// # Functions
// # =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

// -- Function to remove item from our queue --
// source: https://youtu.be/oyX30WVuEos
char removeFromTheQueue(structForQueueBoi *queueToDecriment)
{

    char charDataToReturn;

    // if queue empty, break and say that the queue is empty
    if (queueToDecriment->countOfEntries == 0)
    {
        return '\0'; // exit out
    }

    // if not empty, save result that is the queue's head positions and return it
    charDataToReturn = queueToDecriment->values[queueToDecriment->head];
    queueToDecriment->head = (queueToDecriment->head + 1) % queueToDecriment->size; // keep track of size
    queueToDecriment->countOfEntries--;

    return charDataToReturn;
}

// -- Function to initalize queue, pass in a pointer to the queue struct and tell it the size we want to allocate
// source: https://youtu.be/oyX30WVuEos
void setupTheQueue(structForQueueBoi *inputQueue, int maximumSizeBoi)
{
    inputQueue->size = maximumSizeBoi;
    inputQueue->values = malloc(sizeof(char) * inputQueue->size);
    inputQueue->countOfEntries = 0;
    inputQueue->head = 0;
    inputQueue->tail = 0;
}

// -- Function to add element to the back of the queue --
// (queueStructureToUse is the queue we are adding to, dataToAppend is what we want to add to the queue)
// source: https://youtu.be/oyX30WVuEos
bool addToTheQueue(structForQueueBoi *queueStructureToUse, char dataToAppend)
{

    // check to make sure queue is not full
    if (queueStructureToUse->countOfEntries == queueStructureToUse->size)
    {
        return false;
    }

    // if not full, add to the back and increase num of entries and move tail
    queueStructureToUse->values[queueStructureToUse->tail] = dataToAppend;
    queueStructureToUse->countOfEntries++;
    queueStructureToUse->tail = (queueStructureToUse->tail + 1) % queueStructureToUse->size; // will clock around if reach end of the queue

    return true;
}

// -- Function to create threads to break up and uppercase the input string
// Source: https://www.tutorialspoint.com/cprogramming/switch_statement_in_c.htm
// Source 2: https://www.geeksforgeeks.org/mutex-lock-for-linux-thread-synchronization/
// dequeue -> uppercase -> requeue (for loop size of 5)
void *serverDecoder(void *arg)
{
    // creating struct for args data
    struct TheadsForAIEOU *myDataFromThread = (struct TheadsForAIEOU *)arg;

    structForQueueBoi *insideQueue = myDataFromThread->q;
    int currentThreadIDCASE = myDataFromThread->threadNumber;

    printf("Starting server decoder for Thread %d!\n", currentThreadIDCASE);

    // temp queue to hold data taken from orig queue
    structForQueueBoi tempQueue;
    setupTheQueue(&tempQueue, insideQueue->size);

    // uppercasing part - check id of the thread to determine the switch
    while (!(insideQueue->countOfEntries == 0))
    {
        char indexToUppercase = removeFromTheQueue(insideQueue);

        switch (currentThreadIDCASE)
        {
        case 1: // A
            if (indexToUppercase == 'a')
            {
                indexToUppercase = 'A';
            }
            break;

        case 2: // E
            if (indexToUppercase == 'e')
            {
                indexToUppercase = 'E';
            }
            break;

        case 3: // I
            if (indexToUppercase == 'i')
            {
                indexToUppercase = 'I';
            }
            break;

        case 4: // O
            if (indexToUppercase == 'o')
            {
                indexToUppercase = 'O';
            }
            break;

        case 5: // U
            if (indexToUppercase == 'u')
            {
                indexToUppercase = 'U';
            }
            break;

        case 6: // serverencode
            printf(">server encoding!\n");
            break;

        default:
            break;
        }

        addToTheQueue(&tempQueue, indexToUppercase);
    }

    while (!(tempQueue.countOfEntries == 0))
    {
        char indexToUppercase = removeFromTheQueue(&tempQueue);
        addToTheQueue(insideQueue, indexToUppercase);
    }

    free(tempQueue.values);

    pthread_exit(NULL);
}

// -- Function to recieve data and send it back to server --
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
    printf("1_helper node listening on IP: %d on Port: %d...\n", INADDR_ANY, HELPER_SERVER_PORT);

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
        int sizeOfQueue = 5; // Size of each queue
        int numberOfQueues = (int)ceil((double)dataBytesIncoming / sizeOfQueue);
        printf("*Data bytes: %d, Queue size: %d, Number of queues: %d\n", dataBytesIncoming, sizeOfQueue, numberOfQueues);

        // array queues setup
        structForQueueBoi *queues = malloc(sizeof(structForQueueBoi) * numberOfQueues); // store incoming data from buffer here

        for (int i = 0; i < numberOfQueues; i++)
        {
            setupTheQueue(&queues[i], sizeOfQueue); // input queues initialize
        }

        // INPUT QUEUES: Enqueue received data into the queues
        int indexfortheinputdata = 0;
        for (int i = 0; i < numberOfQueues; i++)
        {
            for (int j = 0; j < sizeOfQueue; j++)
            {
                if (indexfortheinputdata < (dataBytesIncoming - 1)) // if not shorter, will screw everything up with carrage return
                {
                    // run threw each data index, add to the queue, incriment
                    addToTheQueue(&queues[i], datastreamFromMainServer[indexfortheinputdata]);
                    indexfortheinputdata++;
                }
                else
                {
                    // If we reach the end of the received data, fill up with spaces
                    addToTheQueue(&queues[i], ' '); // orig was underscores _
                }
            }
        }
        // ----
        // ENQUEING IS GREAT, NOW WE NEED TO PASS QUEUES INTO THE UPPERCASE THREADING FUNCTION
        // THEN HAVE IT THREAD AND UPPERCASE ACCROSS SUBSEQUENT QUEUES

        for (int i = 0; i < numberOfQueues; i++)
        {
            // int threadCaseSwitcher = 1;
            printf("THREADING...\n");

            // array of threads
            struct TheadsForAIEOU myDataFromThread[MAX_THREADS];

            // initalize threads
            for (int j = 0; j < MAX_THREADS; j++)
            {
                myDataFromThread[j].q = &queues[i];
                myDataFromThread[j].threadNumber = j + 1;
                // myDataFromThread[j].threadNumber = threadCaseSwitcher;
            }

            // create threads
            pthread_t threads[MAX_THREADS];
            for (int j = 0; j < MAX_THREADS; j++)
            {
                if (j < MAX_THREADS)
                {
                    pthread_create(&threads[j], NULL, serverDecoder, (void *)&myDataFromThread[j]);
                }
            }

            // join threads
            for (int j = 0; j < MAX_THREADS; j++)
            {
                pthread_join(threads[j], NULL);
            }
        }

        // --------- DONE with the threading and joining, need to cleanup

        // print & dequeue
        int counter = 0; // count / keep track of where we are

        for (int i = 0; i < numberOfQueues; i++)
        {
            printf("\nQueue %d:\n", i + 1);
            for (int j = 0; j < sizeOfQueue; j++)
            {
                char storeDequeuedData = removeFromTheQueue(&queues[i]);
                uppercasedBuffer[counter] = storeDequeuedData;
                counter++;
                printf("Char #%d --> %c , ", counter, storeDequeuedData);
            }
        }

        uppercasedBuffer[counter - 1] = '\n';
        uppercasedBuffer[counter] = '\0';

        printf("\n>>IMPORTANT: Uppercased data: %s\n", uppercasedBuffer);
        // send the uppercased data back to the main server

        serverEncoder(server_socket, dataBytesIncoming);
        // send(server_socket, uppercasedBuffer, dataBytesIncoming, 0);

        // destroy queues
        for (int i = 0; i < numberOfQueues; i++)
        {
            free(queues[i].values);
        }

        // free memory from queues
        free(queues);
    }

    // close sockets for cleanup
    close(server_socket);
    close(helperSocket);

    return 0;
}
