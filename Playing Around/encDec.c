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
    printf("¡¡Dedicated to Peter Zlomek and Harely Alderson III (hi mom)!!\n\n");
}

// -- Function to print out when the server connects to the helper --
void serverConnected(){
    // printf("SERVER CONNECTED");
    printf(" _____                        _____                     _         _  \n");
    printf("|   __|___ ___ _ _ ___ ___   |     |___ ___ ___ ___ ___| |_ ___ _| | \n");
    printf("|__   | -_|  _| | | -_|  _|  |   --| . |   |   | -_|  _|  _| -_| . | \n");
    printf("|_____|___|_|  \\_/|___|_|    |_____|___|_|_|_|_|___|___|_| |___|___|\n");
}

// -- Function to print out when the server disconnects from the helper --
void connectionTerminated() {
    // printf("CONNECTION TERMINATED...");
    printf(" _____                     _   _            _____               _         _         _ \n");
    printf("|     |___ ___ ___ ___ ___| |_|_|___ ___   |_   _|___ ___ _____|_|___ ___| |_ ___ _| |\n");
    printf("|   --| . |   |   | -_|  _|  _| | . |   |    | | | -_|  _|     | |   | .'|  _| -_| . |\n");
    printf("|_____|___|_|_|_|_|___|___|_| |_|___|_|_|    |_| |___|_| |_|_|_|_|_|_|__,|_| |___|___|\n");
}


