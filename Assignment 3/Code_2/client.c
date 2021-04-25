#include <netdb.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <math.h>
 
#define LEN 10000

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

#define SA struct sockaddr
int local_len = 0;

char* base64convert(char*, int);

void func(int sockfd) 
{ 
	while(1)
	{
        printf("Specify the type of request: Enter 1 for sending type 1 message, Enter 3 for closing connection  ");
        int n;
        scanf("%d", &n);
        if(n == 3){
            char *fix = "3";
            write(sockfd, fix,1); // write in socket
            char *bufsp = (char*) malloc(500*sizeof(char));
            int sp = read(sockfd,bufsp,500); // reading from socket
            bufsp[sp] = '\0';
            printf("%s", bufsp);
            free(bufsp);
            break;
        }
        printf("\nType in your message: ");
        char *buf = (char*) malloc(10000*sizeof(char));
        scanf("%s",buf); 
        
        int lensp = strlen(buf);
        char *buf2 = (char*) malloc(10000*sizeof(char));
        char *buf3 = (char*) malloc(10000*sizeof(char));

        initialise(buf2,buf3);  // initialising buffer

        buf2 = base64convert(buf,lensp); 
        
        for(int i=0;i<strlen(buf2);i++){
        	buf3[i+1] = buf2[i];
        }
        buf3[0] = n + '0';  // message type appending

        printf("\nEncoded Client side string is: %s \n", buf2); 
        buf2[local_len] = '\0';
        local_len = 0;
       
        write(sockfd,buf3,strlen(buf3));  // writing in socket for use by server
        int len2 = read(sockfd,buf,5000);  // reading data from socket in buffer
        
        printf("\nFrom Server: ");

        // printing acknowledgement sent from server
        for(int i=1;i<5000;i++){
        	printf("%c",buf[i]);
        	if(buf[i] == ')') break;
        } 
        printf("\n");

        free(buf);
        free(buf2);
    }    
	close(sockfd); 
} 
 

int main(int argc, char* argv[]) 
{ 
	int sockfd, connfd, PORT; 
	struct sockaddr_in servaddr, cli; 

	// socket create and verification 
	sockfd = socket(AF_INET, SOCK_STREAM, 0); 
	if (sockfd == -1) { 
		printf("socket creation failed...\n"); 
		exit(0); 
	} 
	else {
		printf("Socket successfully created..\n"); 
	}

	bzero(&servaddr, sizeof(servaddr)); 
    PORT = atoi(argv[2]);
	
	// assign IP, PORT 
	servaddr.sin_family = AF_INET; 
	servaddr.sin_addr.s_addr = inet_addr(argv[1]); 
	servaddr.sin_port = htons(PORT); 

	// connect the client socket to server socket 
	if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) { 
		printf("connection with the server failed...\n"); 
		exit(0); 
	} 
	else {
		printf("connected to the server..\n"); 
	}

	// function for chat 
	func(sockfd); 
	// close the socket in func (done inside function)
} 

char* base64convert(char* input,int len_input)
{
   	char* worker = input;
   	char input_str[100];
   	int ii = 0;
   	for(; ii < len_input; ii++) input_str[ii] = worker[ii];

   	int len_str = len_input;
   
	char char_set[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"; 
	
	char *res_str = (char *) malloc(2000 * sizeof(char)); 
	int index, no_of_bits = 0, padding = 0, val = 0, count = 0, temp; 
	int i,j,k = 0; 
	
	for(i = 0; i < len_str; i += 3) 
	{ 
		val = 0, count = 0, no_of_bits = 0; 

		for (j = i; j < len_str && j <= i + 2; j++) 
		{ 
			// binary data of input_str is stored in val 
			val = val << 8; 
				
			// (A + 0 = A) stores character in val 
			val = val | input_str[j]; 
				
			// calculates how many time loop 
			// ran if "MEN" -> 3 otherwise "ON" -> 2 
			count++; 
		} 
		no_of_bits = count * 8; 

		// calculates how many "=" to append after res_str. 
		padding = no_of_bits % 3; 

		// extracts all bits from val (6 at a time) 
		// and find the value of each block 
		while (no_of_bits != 0) 
		{ 
			// retrieve the value of each block 
			if (no_of_bits >= 6) 
			{ 
				temp = no_of_bits - 6; 
					
				// binary of 63 is (111111) f 
				index = (val >> temp) & 63; 
				no_of_bits -= 6;		 
			} 
			else
			{ 
				temp = 6 - no_of_bits; 
					
				// append zeros to right if bits are less than 6 
				index = (val << temp) & 63; 
				no_of_bits = 0; 
			} 
			res_str[k++] = char_set[index]; 
		} 
	} 
   // padding is done here 
	for (i = 1; i <= padding; i++) { 
		res_str[k++] = '='; 
	} 

	res_str[k] = '\0'; 
	local_len=strlen(res_str);   
	return res_str; 
}
