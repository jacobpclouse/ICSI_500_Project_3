#include <stdio.h>
#include <string.h>
#include <pthread.h>

#define MAX_THREADS 4 // You can adjust the number of threads based on your requirements

char str[30];
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// Function to convert characters in a specific range to uppercase
void *convertCase(void *arg) {
    int thread_id = *((int *)arg);

    int start = (strlen(str) / MAX_THREADS) * thread_id;
    int end = (strlen(str) / MAX_THREADS) * (thread_id + 1);

    // Adjust the end for the last thread to cover the remaining characters
    if (thread_id == MAX_THREADS - 1) {
        end = strlen(str);
    }

    for (int i = start; i < end; i++) {
        pthread_mutex_lock(&mutex);

        if (str[i] >= 'a' && str[i] <= 'z') {
            str[i] = str[i] - 32; // Convert to uppercase
        }

        pthread_mutex_unlock(&mutex);
    }

    pthread_exit(NULL);
}

int main() {
    printf("Enter the string: ");
    scanf("%s", str);

    pthread_t threads[MAX_THREADS];
    int thread_ids[MAX_THREADS];

    // Create threads
    for (int i = 0; i < MAX_THREADS; i++) {
        thread_ids[i] = i;
        pthread_create(&threads[i], NULL, convertCase, (void *)&thread_ids[i]);
    }

    // Join threads
    for (int i = 0; i < MAX_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("\nConverted string is: %s\n", str);

    return 0;
}
