#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <ctype.h>

// Structure to pass data to the threads
typedef struct {
    char *str;
    char targetChar;
} ThreadData;

// Function to uppercase specific characters in the string
void *uppercaseChars(void *arg) {
    ThreadData *data = (ThreadData *)arg;
    char *str = data->str;
    char targetChar = data->targetChar;

    for (int i = 0; str[i] != '\0'; ++i) {
        if (str[i] == targetChar) {
            str[i] = toupper(targetChar);
        }
    }

    return NULL;
}

int main() {
    // Example string
    char myString[] = "abcdeabcdeabcde";

    // Create threads to uppercase specific characters
    pthread_t threadA, threadC, threadZ;

    ThreadData dataA = {myString, 'a'};
    ThreadData dataC = {myString, 'c'};
    ThreadData dataZ = {myString, 'z'};

    pthread_create(&threadA, NULL, uppercaseChars, &dataA);
    pthread_create(&threadC, NULL, uppercaseChars, &dataC);
    pthread_create(&threadZ, NULL, uppercaseChars, &dataZ);

    // Wait for threads to finish
    pthread_join(threadA, NULL);
    pthread_join(threadC, NULL);
    pthread_join(threadZ, NULL);

    // Print the modified string
    printf("Modified String: %s\n", myString);

    return 0;
}
