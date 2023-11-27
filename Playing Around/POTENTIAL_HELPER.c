#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

#define BUFFER_SIZE 100

// Shared variables
char sharedBuffer[BUFFER_SIZE];
pthread_mutex_t mutex;

// Thread data structure
typedef struct {
    int thread_id;
} ThreadData;

// Thread function
void *threadFunction(void *arg) {
    ThreadData *data = (ThreadData *)arg;

    // Lock the mutex to access the shared buffer
    pthread_mutex_lock(&mutex);

    // Perform transformations based on the thread ID
    switch (data->thread_id) {
        case 1:
            for (int i = 0; i < BUFFER_SIZE && sharedBuffer[i] != '\0'; ++i) {
                if (sharedBuffer[i] == 'a') {
                    sharedBuffer[i] = 'A';
                }
            }
            break;
        case 2:
            for (int i = 0; i < BUFFER_SIZE && sharedBuffer[i] != '\0'; ++i) {
                if (sharedBuffer[i] == 'e') {
                    sharedBuffer[i] = 'E';
                }
            }
            break;
        case 3:
            for (int i = 0; i < BUFFER_SIZE && sharedBuffer[i] != '\0'; ++i) {
                if (sharedBuffer[i] == 'o') {
                    sharedBuffer[i] = 'O';
                }
            }
            break;
        case 4:
            for (int i = 0; i < BUFFER_SIZE && sharedBuffer[i] != '\0'; ++i) {
                if (sharedBuffer[i] == 'i') {
                    sharedBuffer[i] = 'I';
                }
            }
            break;
        case 5:
            for (int i = 0; i < BUFFER_SIZE && sharedBuffer[i] != '\0'; ++i) {
                if (sharedBuffer[i] == 'u') {
                    sharedBuffer[i] = 'U';
                }
            }
            break;
        default:
            break;
    }

    // Unlock the mutex
    pthread_mutex_unlock(&mutex);

    // Print the result
    printf("Thread %d: Transformed string: %s\n", data->thread_id, sharedBuffer);

    // Pass the transformed string to the next thread
    if (data->thread_id < 5) {
        ThreadData nextThreadData = {data->thread_id + 1};
        pthread_t nextThread;
        pthread_create(&nextThread, NULL, threadFunction, &nextThreadData);
        pthread_join(nextThread, NULL);
    }

    pthread_exit(NULL);
}

int main() {
    // Initialize mutex
    pthread_mutex_init(&mutex, NULL);

    // Simulate receiving data from a server
    const char *serverData = "a simple example string";

    // Lock the mutex to access the shared buffer
    pthread_mutex_lock(&mutex);

    // Copy the server data to the shared buffer
    strncpy(sharedBuffer, serverData, BUFFER_SIZE - 1);
    sharedBuffer[BUFFER_SIZE - 1] = '\0';

    // Unlock the mutex
    pthread_mutex_unlock(&mutex);

    // Create the first thread
    ThreadData firstThreadData = {1};
    pthread_t firstThread;
    pthread_create(&firstThread, NULL, threadFunction, &firstThreadData);
    pthread_join(firstThread, NULL);

    // Destroy mutex
    pthread_mutex_destroy(&mutex);

    return 0;
}
