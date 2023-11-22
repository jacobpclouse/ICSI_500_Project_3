#include <stdio.h>
#include <pthread.h>

// Function to be executed by the threads
void *printMessage(void *threadId) {
    long tid = (long)threadId;
    printf("Hello from Thread %ld!\n", tid);
    pthread_exit(NULL);
}

int main() {
    // Number of threads to create
    const int numThreads = 2;

    // Thread IDs and the thread array
    pthread_t threads[numThreads];

    // Create threads
    for (long i = 0; i < numThreads; i++) {
        int result = pthread_create(&threads[i], NULL, printMessage, (void *)i);
        if (result) {
            fprintf(stderr, "Error creating thread %ld: %d\n", i, result);
            return -1;
        }
    }

    // Wait for threads to finish
    for (long i = 0; i < numThreads; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("All threads have completed.\n");

    return 0;
}
