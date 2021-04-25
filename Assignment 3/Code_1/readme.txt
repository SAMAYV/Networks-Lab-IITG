Client: <executableCode> <ServerIP> <ServerPort>
Server: <executableCode> <ServerPort>

No need to compile encode.c and decode.c as they only have functions which are finally imported in server and client files.

Compiling the code :

- To compile server.c use the command: gcc server.c -o server
- To compile client.c use the command: gcc client.c -o client

Running the Program :

- Start the server which will take one command line argument as port number on which it would work.
- Start the client which will take 2 command line arguments of which the first is the server's IP address and the second is the server's port number.

You can run both client and server on the same computer.

Example:
server : ./server 3000
client1 : ./client 127.0.0.1 3000
client2 : ./client 127.0.0.1 3000

Properties of the presented code :

- Sequence of messages is shown in either of the two formats: 
  1) "TYPE x MESSAGE <SENT TO/RECEIVE FROM> <CLIENT> <client IP>:<client Port>"
  2) "TYPE x MESSAGE <SENT TO/RECEIVE FROM> <SERVER> <server IP>:<server Port>".
  
  <client IP> is the IP of client from which server is receiving messages or to which server is sending messages.
  <server IP> is the IP of server from which client is receiving messages or to which client is sending messages.
  <client Port> is the Port of client from which server is receiving messages or to which server is sending messages.
  <server Port> is the Port of server from which client is receiving messages or to which client is sending messages.
  x can be 1 or 2 or 3.

- Server can handle multiple clients simultaneously. 
- Server uses Base64 encoding and both the client and the server will use TCP sockets.
- Initially the server will be waiting for TCP connection from client.
- Client will then connect to the server using server's TCP port that is already known to the client.
- When the connection sets up successfully, the client will receive input message from the terminal typed by the user and encodes it using Base64 encoding.
- When the encoded message is computed, the client sends the message (Type 1) to the server by writing to the socket on which the server can read.
- When the message is received by the server, it prints the original message and the decoded message and then sends the ACK (Type 2 message) to the client by 
  writing to the socket which that client can read.
- Client and server remain in the loop to send any number of messages which the client wishes.
- When the client wants to close the connection, it sends Type 3 message to the server and the TCP connections on both are closed gracefully by releasing the 
  socket resources.
- Maximum message length can be 4999 (5000 including '\0').
- Type-1 message: Normal Client Message sent to Server
- Type-1 message is completed at the first occurence of '\n'
- Type-2 message: Server Acknowledgement from Server to Client
- Type-3 message: Client Close Connection Message to Server

