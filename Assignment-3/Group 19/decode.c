#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<errno.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<math.h>

int powers_of_two[] = {128,64,32,16,8,4,2,1};

// The following function returns integer value corresponding to a Base64 character 
int base64Value(char c)
{
	if(c >= 'A' && c <= 'Z' ){
		return((int)(c-'A'));			// 0-25
	}
	else if(c >= 'a' && c <= 'z' ){
		return((int)(c-'a'+26));		// 26-51
	}
	else if(c >= '0' && c <= '9' ){
		return((int)(c-'0'+52));		// 52-61
	}
	else if(c == '+') {
		return(62);						// 62
	}
	else {
		return(63);						// 63
	}
}

// The following function takes a set of four consecutive Base64 characters and concat their 6 bit binary values after obtaining their corresponding 
// integer values from base64Value() function, together to form a 24 bit string. If there exists '=' in the four character string then for each '=' character 
// we pad output string by six '0' characters, hence making total count of 24 bits in output string.
void base64ToBinary(char* message,char* out)
{
	for(int i = 0; i < 4; i++)
	{
		if(message[i] == '='){
			for(int j = i*6; j < (i+1)*6; j++)
				out[j]='0';
		}
		else {
			int base64_value = base64Value(message[i]);
			for(int j = 0; j < 6; j++){
				if(base64_value >= powers_of_two[j+2]){
					base64_value -= powers_of_two[j+2];
					out[i*6+j] = '1';
				}
				else {
					out[i*6+j] = '0';
				}
			}
		}
	}
}

// The following function takes up the 24 bit string returned by base64ToBinary() function and convert each group of 8 bits into it's integer value and then to 
// it's corresponding ASCII character. Finally returns the string formed by concating the 3 ASCII characters returned by each 8 bits group. 
void binaryToASCII(char* message,int padding,int flag,char* out)
{
	int value;
	for(int i = 0; i < 3; i++){
		if(i >= 3-padding && flag){
			break;
		}
		value = 0;
		for(int j = 0; j < 8; j++){
			value += ((int)(message[i*8 + j]-'0'))*powers_of_two[j];
		}
		out[i] = (char)value;
	}
}	

// The following function is the outlining function which takes up a Base64 encoded string and return it's ASCII encoded string by the help of above mentioned functions.
char* base64ToASCII(char* message)
{
	int padding = 0,flag = 0,last = 0;
	int len = strlen(message);
	char* output = (char*) malloc(sizeof(char)*len);
	char* str = (char*) malloc(sizeof(char)*5);
	str[5] = '\0';
	char* binary = (char*) malloc(sizeof(char)*25);
	binary[25] = '\0';
	char* ascii = (char*) malloc(sizeof(char)*4);
	ascii[4] = '\0';
	for(int i = 0,j = 0; i < len; i += 4,j++){
		for(int k = 0; k < 4; k++){
			if(message[i + k] == '='){
				flag = 1;
				padding++;
			}
			str[k] = message[i + k];
		}
		base64ToBinary(str,binary);
		binaryToASCII(binary,padding,flag,ascii);
		for(int k = 0; k < 3;k++){
			if(flag && k >= 3-padding){
				break;
			}
			last = 3*j + k + 1;
			output[3*j + k] = ascii[k];
		}
	}
	output[last] = '\0';
	return(output);
}