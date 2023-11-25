// # =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// # encDec.h Importing Libraries / Modules / Headers 
// # =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// for client/server:
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>


// - orig below
#include <stdint.h>

#ifndef SETUPHEADER_H
#define SETUPHEADER_H

#define MAX_MESSAGE_LENGTH 1024

// # =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// # Dataframe Setup
// # =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

// // Structure for the encDecFrame to send back and forth
// // Referenced code here: https://www.geeksforgeeks.org/structures-c/#
// struct Send_data_pack {
//     int client_id;
//     char message[MAX_MESSAGE_LENGTH];
// };



// struct encDecFrame
// {
//     char syn1;       // first SYN character (ASCII 22)
//     char syn2;       // second SYN character (ASCII 22)
//     uint8_t control; // control char to show length of data block (1 to 512)
//     char parity;     // parity bit
//     char data[512];  // max 64 data characters - 512 binary characters
// };


// Function declarations
void myLogo();

void serverConnected();

void connectionTerminated();

char *readSimpleCharactersFunc(const char *fileToRead);

void outputStringToFile(const char *stringToSave, const char *nameOfOutputFile);


#endif
