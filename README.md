# ICSI_500_Project_3
Develop a practical understanding of task collaboration using socket programming, semaphores, multithreading.

## TO DO: 
- [ ] Write documentation & report for Project 3
- * [ ] Create Sequence Diagram for helper/server/client
- [ ] Need to use CRC encoding for sharing data back and forth from helper and server
- [ ] At end of uppercase, The writer thread will share the data received with the serverEncoder
component (encoding for going back to server)
- [ ] Impliment queue of strings for incoming data strings
- * [ ] This structure will be an array of pointers to strings with integers (pointers) to indicate
the head and tail of the list.
- * [ ] The maximum size of the buffer array will be 5.
- * [ ] Threads should terminate when end of input data is reached. 
- [x] figure out how to do a three way (ie: client -> server -> helper)

## OBJECTIVES
Develop a client/server application using Linux TCP sockets and the C programming language.
Your solution will be based on the code you developed for project-2. You are to add to the Chat
system you developed for project-2 an additional service provided by the server to all client
communication.

## PROBLEM
You are to develop a data processing system to process strings of characters. The solution must
replace all instances of lowercase vowels with uppercase ones.
You are to modify your Chat application such that the server will collaborate with another node
in order to successfully respond to requests from the different clients. The server you developed
for project-2 will share, through a socket, the data component of the frames received with this
new helper node. Your project-2 server will forward the text information from the frames to the
helper node for processing. The helper node will create six threads, process the data received,
and use a socket to share the generated results to the server. All communications between all
nodes will be encoded according to the format defined in project-2


## Resources Used: 
- Convert Uppercase to Lowercase in C: https://www.javatpoint.com/convert-uppercase-to-lowercase-in-c
- Working and creating threads in C but am running into a problem with the argument type: https://stackoverflow.com/questions/71230931/working-and-creating-threads-in-c-but-am-running-into-a-problem-with-the-argumen
- Java Multithreading Uppercase String: https://stackoverflow.com/questions/53089643/java-multithreading-uppercase-string
- Switch Statements: https://www.tutorialspoint.com/cprogramming/switch_statement_in_c.htm
- Mutex lock for Linux Thread Synchronization: https://www.geeksforgeeks.org/mutex-lock-for-linux-thread-synchronization/
- Introduction and Array Implementation of Queue: https://www.geeksforgeeks.org/introduction-and-array-implementation-of-queue/
- Creating a Queue in C: https://www.digitalocean.com/community/tutorials/queue-in-c
- [IMPORTANT] Implementing a queue in C: https://www.youtube.com/watch?v=Ra6p-Bmajlw
