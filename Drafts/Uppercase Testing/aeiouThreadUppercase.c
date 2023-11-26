#include <stdio.h>
#include <string.h>
#include <pthread.h>

#define NUM_THREADS 5

char str[30];
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// Function to convert a specific vowel to uppercase
void *convertVowelToUppercase(void *arg) {
    char vowel = *((char *)arg);

    for (int i = 0; i < strlen(str); i++) {
        pthread_mutex_lock(&mutex);

        if (str[i] == vowel || str[i] == (vowel - 32)) {
            str[i] = str[i] - 32; // Convert to uppercase
        }

        pthread_mutex_unlock(&mutex);
    }

    pthread_exit(NULL);
}

int main() {
    printf("Enter the string: ");
    scanf("%s", str);

    pthread_t threads[NUM_THREADS];
    char vowels[NUM_THREADS] = {'a', 'e', 'i', 'o', 'u'};

    // Create threads for each vowel
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, convertVowelToUppercase, (void *)&vowels[i]);
    }

    // Join threads
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("\nConverted string is: %s\n", str);

    return 0;
}
