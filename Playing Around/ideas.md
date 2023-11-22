1) idea - when a client sends a message to the server, have the server then do something (like print out the messgage)
    then have the server send a message to another service
    then have the server communicate to another server on a specific port (simple call and response)
    then have the second server (helper) recieve data and print it
    then have the second server recieve data, alter it and send it back
    then have the first server take that recieved data, and print it when it gets it back
    then have the first server send that data back to the client instead of the original data (in the buffer)