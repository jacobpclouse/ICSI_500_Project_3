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


