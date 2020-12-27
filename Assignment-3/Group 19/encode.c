#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<errno.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<math.h>

int power_of_two[] = {128,64,32,16,8,4,2,1};

// The following function returns Base64 character corresponding to it's integer value 
char base64Character(int value)
{
	if(value <= 25){
		return((char)(value + 65));	// 'A'-'Z'
	}
	else if(value <= 51){
		return((char)(value + 71));	// 'a'-'z'
	}
	else if(value <= 61){
		return((char)(value - 4));	// '0'-'9'
	}
	else if(value == 62) {
		return('+');				// '+'
	}
	else {
		return('/');				// '/'
	}
}

// The following function takes a set of maximum three consecutive ascii characters and concat their 8 bit binary values together to form a 24 bit string. 
// If number of characters are less then 3 then for each missing character we pad output string by eight '0' characters, hence making total count of 
// 24 bits in output string.
void asciiToBinary(char* message,char* out)
{
	for(int i = 0; i < 3;i++){
		if(message[i] == '\0'){
			for(int j = i*8; j < (i+1)*8; j++)
				out[j] = '0';
		}
		else {
			int ascii_value = (int)message[i];
			for(int j = 0; j < 8; j++){
				if(ascii_value >= power_of_two[j]){
					ascii_value -= power_of_two[j];
					out[i*8 + j] = '1';
				}
				else {
					out[i*8 + j] = '0';
				}
			}
		}
	}
}

// The following function takes up the 24 bit string returned by asciiToBinary() function and convert each group of 6 bits into it's integer value and then to 
// it's corresponding Base64 character using base64Character() function. Finally returns the string formed by concating the 4 Base64 characters returned 
// by each 6 bits group.  
void binaryToBase64(char* message,int padding,int flag,char* out)
{
	int value;
	for(int i = 0; i < 4; i++){
		if(i >= 4-padding && flag){
			out[i] = '=';
			continue;
		}
		value = 0;
		for(int j = 0; j < 6; j++){
			value += ((int)(message[i*6 + j] - '0'))*power_of_two[j + 2];
		}
		out[i] = base64Character(value);
	}
}

// The following function is the outlining function which takes up a ASCII encoded string and return it's Base64 encoded string by the help of above mentioned functions.
char* asciiToBase64(char* message)
{
	int padding,flag = 0;
	int len = strlen(message);
	padding = 3 - ((len-1) % 3) - 1;

	char* output = (char *) malloc(sizeof(char)*2*len);
	char* str = (char*) malloc(sizeof(char)*4);
	str[4] = '\0';
	char* binary = (char*) malloc(sizeof(char)*25);
	binary[25] = '\0';
	char* base64 = (char*) malloc(sizeof(char)*5);
	base64[5] = '\0';
	int i,j;
	for(i = 0,j = 0; i < len; i += 3, j++){
		if(len - i <= 3)
		{
			flag = 1;
			for(int k = 0; k < 3; k++){
				if(k >= 3 - padding)
					str[k] = '\0';
				else
					str[k] = message[i + k];
			}
		}
		else {
			str[0] = message[i + 0]; 
			str[1] = message[i + 1];
			str[2] = message[i + 2];
		}
		asciiToBinary(str,binary);
		binaryToBase64(binary,padding,flag,base64);
		for(int k = 0; k < 4; k++){
			output[4*j + k] = base64[k];
		}
	}
	output[4*j] = '\0';
	return(output);
}
