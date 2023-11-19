# ICSI_500_Project_3
Develop a practical understanding of task collaboration using socket programming, semaphores, multithreading.

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
- 