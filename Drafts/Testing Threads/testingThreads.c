#include <stdio.h>
#include <pthread.h>

#define MAX_THREADS 5

// Structure to pass data to the thread function
typedef struct {
    int number;
} ThreadData;

// Function to increment the number and create another thread if necessary
void *incrementNumber(void *arg) {
    ThreadData *data = (ThreadData *)arg;

    // Increment the number
    data->number++;

    printf("Thread %lu: Incremented number to %d\n", pthread_self(), data->number);

    // Create another thread if the number is less than 5
    if (data->number < MAX_THREADS) {
        pthread_t newThread;
        ThreadData newData = {data->number};
        pthread_create(&newThread, NULL, incrementNumber, (void *)&newData);
        pthread_join(newThread, NULL);
    }

    pthread_exit(NULL);
}

int main() {
    int initialNumber = 0;
    pthread_t threads[MAX_THREADS];
    ThreadData data = {initialNumber};

    // Create 5 threads
    for (int i = 0; i < MAX_THREADS; i++) {
        pthread_create(&threads[i], NULL, incrementNumber, (void *)&data);
    }

    // Wait for all threads to finish
    for (int i = 0; i < MAX_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("Final number: %d\n", data.number);

    return 0;
}
