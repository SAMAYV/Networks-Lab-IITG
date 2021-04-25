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

#define MAX 80 
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

#define maxreq 100
#define SA struct sockaddr 
int local_len = 0;
int client = 0;

char* base64convert(char*, int);

// Function designed for chat between client and server. 
void func(int sockfd, int k) 
{      
   int forkret = fork();
   client = client+1;
   if(forkret == 0){
	    int rcb = client;     
		while(1)
		{
			char *buf2 = (char*) malloc(10000*sizeof(char));
			char *buf3 = (char*) malloc(10000*sizeof(char));

			initialise(buf2,buf3);

			int tot = read(sockfd, buf2, 10000);  // read in socket the data from client
			buf2[tot] = '\0';
			
			printf("\nServing Client: %d\n", rcb);

			// copying received data in buf3 with no message type
			int i = 0;
			for(;i < 9999;i++){ 
				buf3[i] = buf2[i+1];
			}

			if(buf2[0] == '3'){ 
				char* bufsp = "Type 3 message received, the_end,_we_are_closing_on_request_of_client\n"; 
				printf("%s\n",bufsp); 

				bufsp = "Type 3 message sent, closing request\n"; 
				int lensp = strlen(bufsp); 
				write(sockfd,bufsp,lensp);  // writing in socket to receive by client
				close(sockfd); 
				break;
			}
			else {
				char* return_make = base64convert(buf3, tot);
				int hh = 0;
				for(;hh < 5000; hh++){
					buf3[5000 + hh] = return_make[hh];	
				} 
				
				buf3[5000 + local_len] = '\0';
				printf("\nDecoded message from client %s\n", buf3 + 5000);
				printf("Message sent by client in encoded form: %s\n", buf3);

				char* buf4 = (char*) malloc(10001*sizeof(char));
				buf4 = "2Type 2 message (Acknowledgement received)\n";
				write(sockfd,buf4,strlen(buf4));   // write in socket to send to client

				local_len = 0;
			}
			free(buf3); 
			free(buf2);
		}
   		exit(0);
   }
} 

// Driver function 
int main(int argc, char* argv[]) 
{ 
	int sockfd, len, PORT; 
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
    PORT = atoi(argv[1]);
	
	// assign IP, PORT 
	servaddr.sin_family = AF_INET; 
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY); 
	servaddr.sin_port = htons(PORT); 

	// Binding newly created socket to given IP and verification 
	if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) { 
		printf("socket bind failed...\n"); 
		exit(0); 
	} 
	else {
		printf("Socket successfully binded..\n"); 
	}

	// Now server is ready to listen and verification 
	if ((listen(sockfd, 5)) != 0) { 
		printf("Listen failed...\n"); 
		exit(0); 
	} 
	else {
		printf("Server listening..\n"); 
	}

	int k = 0;
    while(1){  
    	k = k + 1;
    	int connfd;
		len = sizeof(cli); 

		// Accept the data packet from client and verification 
		connfd = accept(sockfd, (SA*)&cli, &len); 
		if (connfd < 0) { 
			printf("server acccept failed...\n"); 
			exit(0); 
		} 
		else
			printf("server accept the client...\n"); 

		// Function for chatting between client and server 
		func(connfd,k); 
	}

	// After chatting close the socket 
	close(sockfd); 
} 

char* base64convert( char* encoded, int len_str)
{
   	char* decoded_string; 

	decoded_string = (char*)malloc(sizeof(char) * 10000); 

	int i, j, k = 0; 

	// stores the bitstream. 
	int num = 0; 

	// count_bits stores current 
	// number of bits in num. 
	int count_bits = 0; 

	// selects 4 characters from 
	// encoded string at a time. 
	// find the position of each encoded 
	// character in char_set and stores in num. 
	for (i = 0; i < len_str; i += 4) { 
		num = 0, count_bits = 0; 
		for (j = 0; j < 4; j++) { 
			// make space for 6 bits. 
			if (encoded[i + j] != '=') { 
				num = num << 6; 
				count_bits += 6; 
			} 

			/* Finding the position of each encoded 
			character in char_set 
			and storing in "num", use OR 
			'|' operator to store bits.*/

			// encoded[i + j] = 'E', 'E' - 'A' = 5 
			// 'E' has 5th position in char_set. 
			if (encoded[i + j] >= 'A' && encoded[i + j] <= 'Z') 
				num = num | (encoded[i + j] - 'A'); 

			// encoded[i + j] = 'e', 'e' - 'a' = 5, 
			// 5 + 26 = 31, 'e' has 31st position in char_set. 
			else if (encoded[i + j] >= 'a' && encoded[i + j] <= 'z') 
				num = num | (encoded[i + j] - 'a' + 26); 

			// encoded[i + j] = '8', '8' - '0' = 8 
			// 8 + 52 = 60, '8' has 60th position in char_set. 
			else if (encoded[i + j] >= '0' && encoded[i + j] <= '9') 
				num = num | (encoded[i + j] - '0' + 52); 

			// '+' occurs in 62nd position in char_set. 
			else if (encoded[i + j] == '+') 
				num = num | 62; 

			// '/' occurs in 63rd position in char_set. 
			else if (encoded[i + j] == '/') 
				num = num | 63; 

			// ( str[i + j] == '=' ) remove 2 bits 
			// to delete appended bits during encoding. 
			else { 
				num = num >> 2; 
				count_bits -= 2; 
			} 
		} 

		while (count_bits != 0) { 
			count_bits -= 8; 

			// 255 in binary is 11111111 
			decoded_string[k++] = (num >> count_bits) & 255; 
		} 
	} 

	// place NULL character to mark end of string. 
	decoded_string[k] = '\0'; 
	local_len=strlen(decoded_string);

	return decoded_string; 
  }
