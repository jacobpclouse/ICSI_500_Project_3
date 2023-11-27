#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NUM_THREADS 5

// Shared variables
int sharedVariable = 0;
pthread_mutex_t mutex;

// Thread data structure
typedef struct {
    int thread_id;
} ThreadData;

// Thread function
void *threadFunction(void *arg) {
    ThreadData *data = (ThreadData *)arg;

    // Increment the shared variable
    pthread_mutex_lock(&mutex);
    sharedVariable++;
    int updatedValue = sharedVariable;
    pthread_mutex_unlock(&mutex);

    // Print the result
    printf("Thread %d: Incremented value: %d\n", data->thread_id, updatedValue);

    // Pass the updated value to the next thread
    if (data->thread_id < NUM_THREADS - 1) {
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

    // Create the first thread
    ThreadData firstThreadData = {1}; // start with the first thread
    pthread_t firstThread;
    pthread_create(&firstThread, NULL, threadFunction, &firstThreadData);
    pthread_join(firstThread, NULL);

    // Destroy mutex
    pthread_mutex_destroy(&mutex);

    return 0;
}
