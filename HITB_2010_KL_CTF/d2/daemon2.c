#include <stdio.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <time.h>
#include <string.h>
#include "../libctf_rigged/libctf.h"

//#define ROL(x,n) (((x) << ((n) & 0x1F)) | ((x) >> (32-((n) & 0x1F))))
//#define ROR(x,n) (((x) >> ((n) & 0x1F)) | ((x) << (32-((n) & 0x1F))))

//OBFUSCATED_VARIABLES

char *my_strcpy(char *dest, const char *src)
{
//JUNK CODE
   char *save = dest;
//JUNK CODE
   while(*dest++ = *src++);
//JUNK CODE
   return save;
}

void echo(unsigned char *str){
	int i,j;
	unsigned char buffer[512];
	
//JUNK CODE
	for(i=0, j=0; i<strlen(str); i++, j++)
	{
//JUNK CODE
		if( j > str[i] )
		{
			//printf("j: %.2x str[i]: %.2x", j, str[i]);
//JUNK CODE
			return;
		}
//JUNK CODE
		if(j > 0xF) j = 0;
//JUNK CODE
	}
//JUNK CODE
	my_strcpy(buffer, str);
//JUNK CODE
}

int main(int argc, char *argv[]){
	int sockfd, n;
	struct sockaddr_in servaddr,cliaddr;
	socklen_t len;
	unsigned char msg[1024];
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	
	//Fake LibCtf
	CtfLib_Init(0);
	
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(9999);
	bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

	srand(time(0));
	while(1){	
//JUNK CODE
		unsigned char magic_rand = (0xCC + (rand() % 16)) & 0xFF;
//JUNK CODE
		len = sizeof(cliaddr);
//JUNK CODE
		n = recvfrom(sockfd, msg, 1024, 0, (struct sockaddr *)&cliaddr, &len);
//JUNK CODE
		msg[n] = 0;
//JUNK CODE
		printf("magic_rand: %.2x\n", magic_rand);

		if(msg[0] == magic_rand){
//JUNK CODE
			echo(msg);
//JUNK CODE
		}
	}
	return 0;
}

