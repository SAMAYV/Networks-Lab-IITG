#include "encode.c"
#include "decode.c"
#define LEN 5000

/*  General sockaddr_in struct
	struct sockaddr_in{
	  short   sin_family; 
	  u_short sin_port;
	  struct  in_addr sin_addr;
	  char    sin_zero[8]; 
	};
*/

// function for clearing old buffer in message
void initialise(char *p,char *q)
{
	int i;
	for(i = 0; i < LEN; i++){
		p[i] = '\0';
		q[i] = '\0';
	}
	q[i] = '\0';
}

int main(int argc, char *argv[])
{
	// structure containing an internet address
	struct sockaddr_in server; 
	
	// stores response from user
	// 'y' : send more messages whereas 'n' : close the established connection
	char response[2]; 

	char* status = "Do you want to send message? If Yes Enter 'y' else Enter 'n' and then press Enter ";
 	
 	// sock variable stores the value returned by the socket system call. The socket() system call creates a new socket. It takes three arguments. 
	// The first is the address domain of the socket. AF_INET is used for Internet domain (for any two hosts on the Internet).
	// The second argument is the type of socket. SOCK_STREAM in which characters are read in a continuous stream.
	// The third argument is the protocol. '0' means (OS will choose the most appropriate protocol). It will choose TCP for stream sockets. 
	// If the socket call fails, it returns -1
	int sock = socket(AF_INET,SOCK_STREAM,0);

	// for message_type + message
	char buffer[LEN+1];  

	// shows customised error when socket system call fails by catching error directly from stderr
	if(sock == -1){
		perror("SOCKET FAILURE\n");
		exit(-1);
	} 
	else {
		printf("SOCKET SUCCESSFULLY CREATED\n");  
	}

	//code for the address family
	server.sin_family = AF_INET; 

	// gets ip address of server from the terminal arguments and convert it into network bytes
	if(inet_aton(argv[1],&server.sin_addr) == 0){
		write(1,"ERROR",strlen("ERROR"));
	} 

	int pt = atoi(argv[2]); 					 	// gets port number of server from the terminal.
	server.sin_port = htons(pt); 					// converts port number integer to network byte order 
	socklen_t len = sizeof(struct sockaddr_in);  	// len is the length of the socket address.

	// Establish a connection to the server. 
	// It takes three arguments, the socket file descriptor, the address of the host to which it wants to connect
	// This function returns 0 on success and -1 if it fails. 
	if(connect(sock,(struct sockaddr *) &server, len) == -1){
		perror("CONNECT ERROR");
		exit(-1);
	} 
	else {
		printf("CONNECTION ESTABLISHED WITH SERVER HAVING SOCKET ADDRESS");
		printf(" %s:%d  \n",argv[1],pt);
		fflush(stdout);
	}

	// This is used to store the message that the user would want to send to the server.
	char message[LEN]; 

	// endless loop for communicating any number of messages closes when the client wants
	while(1)
	{	
		// For getting y or n from the user as defined earlier.
		write(1,status,strlen(status)); 

		// getting user response of 'n' or 'y'
		read(0,response,2);

		// user wants to close the connection.
		if(response[0] == 'n'){ 
			char* encoded_message = asciiToBase64("Close Connection"); 		// Encode the closing message
			snprintf(buffer,sizeof(buffer),"%c%s",'3',encoded_message); // prepending the type of message to encoded message. '3' is used as it is a Type 3 message
			write(sock,buffer,strlen(buffer)); 							// writing close message to socket for server to receive
			write(1,"TYPE 3 MESSAGE SENT TO SERVER",strlen("TYPE 3 MESSAGE SENT TO SERVER"));
			printf(" %s:%d  \n",argv[1],pt);
			fflush(stdout);
			// Connection close message printed on client console
			write(1,"CONNECTION CLOSED WITH SERVER HAVING SOCKET ADDRESS",strlen("CONNECTION CLOSED WITH SERVER HAVING SOCKET ADDRESS")); 		
			printf(" %s:%d  \n",argv[1],pt);
			fflush(stdout);
			break; 																	// stop communication
		}

		write(1,"\nENTER YOUR MESSAGE: ",strlen("\nENTER YOUR MESSAGE: ")); 		// ask message from client
		initialise(message,buffer); 	// this initialisation is necessary so that message string doesn't contain characters from previous messages
		
		int i = 0;

		// reading client message character by character
		while(1)
		{ 
			read(0,response,1); 					// reading character from console             
			if(response[0] == '\n' || i == LEN-1) 	// stop reading on newline character or max message size
				break;
			message[i++] = response[0]; 			// storing characters into message
		}

		char* encoded_message = asciiToBase64(message); 			// encoding the client message to Base64 using encode.c file
		snprintf(buffer,sizeof(buffer),"%c%s",'1',encoded_message); // prepending type of message. '1' is used as it is a Type 1 Message
		write(sock,buffer,strlen(buffer)); 							// writing full message to socket for server to receive
		write(1,"TYPE 1 MESSAGE SENT TO SERVER",strlen("TYPE 1 MESSAGE SENT TO SERVER"));
		printf(" %s:%d  \n",argv[1],pt);
		fflush(stdout);

		initialise(message,buffer);
		int received = read(sock,message,100); 		// read message from server after sending server Type 1 message.
		strcpy(message+1,base64ToASCII(message+1)); // decoding the received message into message array using decode.c file. 

		// if received message from server is not an acknowledgement
		if(message[0] != '2'){ 
			write(1,"ACKNOWLEDGEMENT NOT RECEIVED!!\nRESEND THE MESSAGE!!\n", strlen("ACKNOWLEDGEMENT NOT RECEIVED!!\nRESEND THE MESSAGE!!\n")); 
		}
		else {
			write(1,"TYPE 2 MESSAGE (ACKNOWLEDGEMENT) RECEIVED FROM SERVER",strlen("TYPE 2 MESSAGE (ACKNOWLEDGEMENT) RECEIVED FROM SERVER"));
			printf(" %s:%d  \n",argv[1],pt); 					// Prints IP, Port number of the server onto the terminal.
			fflush(stdout); 									// It is used to clear the buffer. 
			write(1,message+1,strlen(message)-1); 				// Print ACK message from server
			write(1,"\n\n",2);
		}
	}
	close(sock); // release socket resources
	return 0;
}
