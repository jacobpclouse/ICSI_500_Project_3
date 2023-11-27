// # =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// # encDec.c - Importing Libraries / Modules / Headers 
// # =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <dirent.h>
#include <time.h>   // To get use the time function
#include <stdint.h> // encDecFrame
#include <ctype.h>  // used for initial uppercase example
#include <math.h> // for control rounding up with ceiling

// header
#include "encDec.h" 



// # =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// # Functions
// # =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// -- Function to print out my logo --
void myLogo()
{
    printf("Created and Tested by: \n");
    printf("   __                  _         ___ _                       \n");
    printf("   \\ \\  __ _  ___ ___ | |__     / __\\ | ___  _   _ ___  ___  \n");
    printf("    \\ \\/ _` |/ __/ _ \\| '_ \\   / /  | |/ _ \\| | | / __|/ _ \\ \n");
    printf(" /\\_/ / (_| | (_| (_) | |_) | / /___| | (_) | |_| \\__ \\  __/ \n");
    printf(" \\___/ \\__,_|\\___\\___/|_.__/  \\____/|_|\\___/ \\__,_|___/\\___| \n");
    printf("¡¡Dedicated to Peter Zlomek and Harely Alderson III!!\n\n");
}

// -- Function to print out when the server connects to the helper --
void serverConnected(){
    printf("┏┓            ┏┓             ┓\n");
    printf("┗┓┏┓┏┓┓┏┏┓┏┓  ┃ ┏┓┏┓┏┓┏┓┏╋┏┓┏┫\n");
    printf("┗┛┗ ┛ ┗┛┗ ┛   ┗┛┗┛┛┗┛┗┗ ┗┗┗ ┗┻\n");
}

// -- Function to print out when the server disconnects from the helper --
void connectionTerminated() {
    printf("┏┓          •      ┏┳┓       •        ┓\n");
    printf("┃ ┏┓┏┓┏┓┏┓┏╋┓┏┓┏┓   ┃ ┏┓┏┓┏┳┓┓┏┓┏┓╋┏┓┏┫\n");
    printf("┗┛┗┛┛┗┛┗┗ ┗┗┗┗┛┛┗   ┻ ┗ ┛ ┛┗┗┗┛┗┗┻┗┗ ┗┻\n");
}


// -- Function to read simple characters - no binary --
// help from https://stackoverflow.com/questions/3501338/c-read-file-line-by-line
char *readSimpleCharactersFunc(const char *fileToRead)
{
    FILE *targetFile = fopen(fileToRead, "r");
    if (targetFile == NULL)
    {
        printf("ERROR: %s failed to open!\n", fileToRead);
        return NULL;
    }

    char *characterString = NULL;
    size_t buffLen = 0;
    size_t currentLen = 0;

    int character;
    while ((character = fgetc(targetFile)) != EOF)
    {
        size_t charLen = 1; // Length of a character is always 1

        if (currentLen + charLen + 1 > buffLen)
        {
            buffLen += charLen + 1;
            characterString = (char *)realloc(characterString, buffLen);
            if (characterString == NULL)
            {
                perror("Memory allocation error");
                exit(EXIT_FAILURE);
            }
        }

        characterString[currentLen++] = character;
        characterString[currentLen] = '\0';
    }

    fclose(targetFile);
    return characterString;
}


// -- Function to save string to output file --
// referenced code here: https://www.opentechguides.com/how-to/article/c/75/c-write-to-file.html
void outputStringToFile(const char *stringToSave, const char *nameOfOutputFile)
{
    // Initial errors
    if (stringToSave == NULL)
    {
        printf("Error: No Data provided to write!\n"); // input string null
        return;
    }
    if (nameOfOutputFile == NULL)
    {
        printf("Error: Empty Filename...\n"); // filename null
        return;
    }

    FILE *createdFile = fopen(nameOfOutputFile, "a"); // open to write
    if (createdFile == NULL)
    {
        printf("Error: Issue opening file to output text\n"); // unable to open out file
        return;
    }

    fputs(stringToSave, createdFile); // writing
    fclose(createdFile);              // close
    printf("String saved to %s\n", nameOfOutputFile);
}


