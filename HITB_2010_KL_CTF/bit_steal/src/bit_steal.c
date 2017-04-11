/*
Released under the MIT-license:

Copyright (c) 2009,2010, Earl Marcus

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include "../libctf_rigged/libctf.h"

#define FALSE                   0
#define TRUE                    1

//#define CTF_DEBUG	1

//OBFUSCATED_VARIABLES

typedef int (*system_t) (const char *);
typedef int (*printf_t) (const char *, ...);
typedef void (*exit_t) (int);
typedef FILE* (*fopen_t) (const char *, const char *);
typedef char* (*fgets_t) (char *, int, FILE *);
typedef int (*fclose_t) (FILE *);
typedef char* (*strtok_t) (char *, const char *);
typedef void* (*memset_t) (void *, int, size_t);
typedef size_t (*strlen_t) (const char*);
typedef int (*pthread_mutex_lock_t) (pthread_mutex_t *);
typedef char* (*strncpy_t) (char *, const char*, size_t);
typedef long (*atol_t) (const char *);
typedef size_t (*fwrite_t) (const void *, size_t, size_t, FILE *);
typedef int (*pthread_mutex_unlock_t) (pthread_mutex_t *);
typedef int (*socket_t) (int, int, int);
typedef void (*perror_t) (const char*);
typedef int (*setsockopt_t) (int, int, int, const void *, socklen_t);
typedef in_addr_t (*inet_addr_t) (const char *);
typedef ssize_t (*sendto_t) (int, const void *, size_t, int, const struct sockaddr *, socklen_t);
typedef int (*close_t) (int);
typedef void* (*memcpy_t) (void *, const void *, size_t);
typedef int (*write_t) (int, char *, int);
typedef uint16_t (*htons_t) (uint16_t);
typedef int (*bind_t) (int, const struct sockaddr *, socklen_t);
typedef int (*listen_t) (int, int);
typedef int (*accept_t) (int, struct sockaddr *, socklen_t *);
typedef ssize_t (*read_t) (int, void*, size_t);
typedef void (*sighandler_t) (int);
typedef sighandler_t (*signal_t) (int, sighandler_t);
typedef int (*pthread_create_t) (pthread_t *, const pthread_attr_t *, void *(*)(void*), void *);
typedef time_t (*time_tt) (time_t *);
typedef double (*difftime_t) (time_t, time_t);
typedef unsigned int (*sleep_t) (unsigned int);

system_t _system;
printf_t _printf;
exit_t __exit;
fopen_t _fopen;
fgets_t _fgets;
fclose_t _fclose;
strtok_t _strtok;
memset_t _memset;
strlen_t _strlen;
pthread_mutex_lock_t _pthread_mutex_lock;
strncpy_t _strncpy;
atol_t _atol;
fwrite_t _fwrite;
pthread_mutex_unlock_t _pthread_mutex_unlock;
socket_t _socket;
perror_t _perror;
setsockopt_t _setsockopt;
inet_addr_t _inet_addr;
sendto_t _sendto;
close_t _close;
memcpy_t _memcpy;
write_t _write;
htons_t _htons;
bind_t _bind;
listen_t _listen;
accept_t _accept;
read_t _read;
signal_t _signal;
pthread_create_t _pthread_create;
time_tt _time;
difftime_t _difftime;
sleep_t _sleep;

//Global Variables
int time_to_quit;
unsigned char bTalkKey;
unsigned char cXorKey[30];
unsigned char cXorFlag[255];
int iXorFlagLen;
unsigned char cSeed[20];
unsigned long lSeed=0;
static long holdrand = 1L;

pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;

void EvilFunction2()
{
//JUNK CODE
	_system += 20102010;
//JUNK CODE
	_printf += 20102010;
//JUNK CODE
	__exit += 20102010;
//JUNK CODE
	_fopen += 20102010;
//JUNK CODE
	_fgets += 20102010;
//JUNK CODE
	_fclose += 20102010;
//JUNK CODE
	_strtok += 20102010;
//JUNK CODE
	_memset += 20102010;
//JUNK CODE
	_strlen += 20102010;
//JUNK CODE
	_pthread_mutex_lock += 20102010;
//JUNK CODE
	_strncpy += 20102010;
//JUNK CODE
	_atol += 20102010;
//JUNK CODE
	_fwrite += 20102010;
//JUNK CODE
	_pthread_mutex_unlock += 20102010;
//JUNK CODE
	_socket += 20102010;
//JUNK CODE
	_perror += 20102010;
//JUNK CODE
	_setsockopt += 20102010;
//JUNK CODE
	_inet_addr += 20102010;
//JUNK CODE
	_sendto += 20102010;
//JUNK CODE
	_close += 20102010;
//JUNK CODE
	_memcpy += 20102010;
//JUNK CODE
	_write += 20102010;
//JUNK CODE
	_htons += 20102010;
//JUNK CODE
	_bind += 20102010;
//JUNK CODE
	_listen += 20102010;
//JUNK CODE
	_accept += 20102010;
//JUNK CODE
	_read += 20102010;
//JUNK CODE
	_signal += 20102010;
//JUNK CODE
	_pthread_create += 20102010;
//JUNK CODE
	_time += 20102010;
//JUNK CODE
	_difftime += 20102010;
//JUNK CODE
	_sleep += 20102010;
//JUNK CODE
}

void EvilFunction1()
{
//JUNK CODE
	_system = (system_t)&system-20102010;
//JUNK CODE
	_printf = (printf_t)&printf-20102010;
//JUNK CODE
	__exit = (exit_t)&exit-20102010;
//JUNK CODE
	_fopen = (fopen_t)&fopen-20102010;
//JUNK CODE
	_fgets = (fgets_t)&fgets-20102010;
//JUNK CODE
	_fclose = (fclose_t)&fclose-20102010;
//JUNK CODE
	_strtok = (strtok_t)&strtok-20102010;
//JUNK CODE
	_memset = (memset_t)&memset-20102010;
//JUNK CODE
	_strlen = (strlen_t)&strlen-20102010;
//JUNK CODE
	_pthread_mutex_lock = (pthread_mutex_lock_t)&pthread_mutex_lock-20102010;
//JUNK CODE
	_strncpy = (strncpy_t)&strncpy-20102010;
//JUNK CODE
	_atol = (atol_t)&atol-20102010;
//JUNK CODE
	_fwrite = (fwrite_t)&fwrite-20102010;
//JUNK CODE
	_pthread_mutex_unlock = (pthread_mutex_unlock_t)&pthread_mutex_unlock-20102010;
//JUNK CODE
	_socket = (socket_t)&socket-20102010;
//JUNK CODE
	_perror = (perror_t)&perror-20102010;
//JUNK CODE
	_setsockopt = (setsockopt_t)&setsockopt-20102010;
//JUNK CODE
	_inet_addr = (inet_addr_t)&inet_addr-20102010;
//JUNK CODE
	_sendto = (sendto_t)&sendto-20102010;
//JUNK CODE
	_close = (close_t)&close-20102010;
//JUNK CODE
	_memcpy = (memcpy_t)&memcpy-20102010;
//JUNK CODE
	_write = (write_t)&write-20102010;
//JUNK CODE
	_htons = (htons_t)&htons-20102010;
//JUNK CODE
	_bind = (bind_t)&bind-20102010;
//JUNK CODE
	_listen = (listen_t)&listen-20102010;
//JUNK CODE
	_accept = (accept_t)&accept-20102010;
//JUNK CODE
	_read = (read_t)&read-20102010;
//JUNK CODE
	_signal = (signal_t)&signal-20102010;
//JUNK CODE
	_pthread_create = (pthread_create_t)&pthread_create-20102010;
//JUNK CODE
	_time = (time_tt)&time-20102010;
//JUNK CODE
	_difftime = (difftime_t)&difftime-20102010;
//JUNK CODE
	_sleep = (sleep_t)&sleep-20102010;
//JUNK CODE
}

int _rand(void)
{
	return (((holdrand = holdrand * 214013L + 2531011L)>>16) & 0x7fff);
}

int _srand(unsigned int seed)
{
	holdrand = (long)seed;
}

void PoolFlagChange()
{
	int i;

	//Get FLAG
	/*int iRet = _system("wget --no-check-certificate -q -T 10 -t 1 -O dltemp http://192.168.1.3:5151/getflag.php?d=23852");
	if(iRet != 0)
	{
        	_system("rm -f dltemp");
        	_printf("Oh noes, tcpdump and find out why i cant get flagz\n");
        	__exit(EXIT_FAILURE);
	}*/

	//Signature check
	/*iRet = _system("gpg -q --verify dltemp > /dev/null 2>&1");
	if(iRet != 0)
	{
        	_system("rm -f dltemp");
        	_printf("Oh noes, you didnt get pwned right???\n");
        	__exit(EXIT_FAILURE);
	}*/

	//Read and delete contents

	//FILE *file = _fopen ( "dltemp", "r" );
	FILE *file = _fopen ("schaurakith.dmp", "r");
	if ( file == NULL )
	{
        	//_system("rm -f dltemp");
        	_printf("Damnit, what did u do to your FS???\n");
        	__exit(EXIT_FAILURE);
	}

	char line [ 512 ]; /* or other suitable maximum line size */
	_fgets(line, sizeof(line), file );
	//fgets(line, sizeof(line), file );
	//fgets(line, sizeof(line), file );
	//fgets(line, sizeof(line), file );
	_fclose (file);
	//_system("rm -f dltemp");

 	//Extract flag and seed
 	char *pch;
 	pch = _strtok (line,"-\r\n");
 	if(pch == NULL)
 	{
        	_printf("Oh noooooo, flag read error!!!\n");
         	__exit(EXIT_FAILURE);
 	}

	_pthread_mutex_lock( &mutex1 );

 	_memset(cXorFlag, 0, sizeof(cXorFlag));
 	_strncpy(cXorFlag, pch, sizeof(cXorFlag));
 	iXorFlagLen = _strlen(cXorFlag);

 	pch = _strtok(NULL, "-\r\n");
 	if(pch == NULL)
 	{
        	_printf("Oh noooooo, flag read error!!!\n");
         	__exit(EXIT_FAILURE);
 	}


	_memset(cSeed, 0, sizeof(cSeed));
	_strncpy(cSeed, pch, sizeof(cSeed));
	lSeed = _atol(cSeed);

	//Write flag to file
	/*
	FILE *f1 = _fopen ("schaurakith.dmp", "wb");
	if(f1 != NULL)
	{
		_fwrite(cXorFlag, _strlen(cXorFlag), 1, f1);
		_fclose(f1);
	}
	*/

	//Enforce proper permissions on flag file
	//_system("chmod 600 schaurakith.dmp");

//JUNK CODE
	//PRNG Seed(Secret)
	_srand(lSeed);
//JUNK CODE

	//Generate a 20 byte random key
	for(i=0; i<20; i++)
	{
//JUNK CODE
		cXorKey[i] = _rand() % 0xff;
//JUNK CODE
#ifdef CTF_DEBUG
		printf("%.2X ", cXorKey[i]);
#endif
	}
#ifdef CTF_DEBUG
	printf("\n");
#endif

	//Encrypt flag with key
	int j=0;

	for(i=0; i<iXorFlagLen; i++)
	{
		cXorFlag[i] ^= cXorKey[j++];
//JUNK CODE
		ROR(cXorFlag[i], 3);
//JUNK CODE
		if(j==20) j=0;
	}
	_pthread_mutex_unlock( &mutex1 );

#ifdef CTF_DEBUG
	//Debug
	FILE *f1 = fopen("packet.dmp", "wb");
	fwrite(cXorFlag, iXorFlagLen, 1, f1);
	fclose(f1);
#endif

}

void* BroadcastTraffic(void *ptr)
{
	int sockfd;
	struct sockaddr_in their_addr; // connector's address information
	int numbytes;
	int broadcast = 1;

	//Setup broadcast socket

//JUNK CODE
	if ((sockfd = _socket(AF_INET, SOCK_DGRAM, 0)) == -1)
	{
//JUNK CODE
        	_perror("socket()");
//JUNK CODE
	        __exit(EXIT_FAILURE);
	}
//JUNK CODE
	// this call is what allows broadcast packets to be sent:
	if (_setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof broadcast) == -1)
	{
//JUNK CODE
		_perror("setsockopt (SO_BROADCAST)");
//JUNK CODE
		__exit(1);
	}
//JUNK CODE
	their_addr.sin_family = AF_INET;     // host byte order
//JUNK CODE
	their_addr.sin_port = _htons(43450); // short, network byte order
//JUNK CODE
	their_addr.sin_addr.s_addr = _inet_addr("255.255.255.255");
//JUNK CODE
	_memset(their_addr.sin_zero, 0, sizeof(their_addr.sin_zero));
//JUNK CODE

	while(!time_to_quit)
	{
//JUNK CODE
		_pthread_mutex_lock( &mutex1 );
//JUNK CODE
		_sendto(sockfd, cXorFlag, iXorFlagLen, 0, (struct sockaddr *)&their_addr, sizeof their_addr);
//JUNK CODE
		_pthread_mutex_unlock( &mutex1 );
//JUNK CODE
		_sleep(5);
//JUNK CODE
	}
//JUNK CODE
	_close(sockfd);
//JUNK CODE
}

int ProcessPacket(int iSock, char *pPacketData, DWORD dwPacketSize)
{
	BYTE bSend[10];
	
	if(dwPacketSize != 3) return -1;
	if((unsigned char)pPacketData[0] != bTalkKey) return -1;

	if(pPacketData[1] < 0 || pPacketData[1] > 8) return -1;
	if(pPacketData[2] < 0 || pPacketData[2] > 8) return -1;

//JUNK CODE
	BYTE bReadByte = pPacketData[1];
//JUNK CODE
	BYTE bReadBit = pPacketData[2];
//JUNK CODE

	//Get-Character
	_pthread_mutex_lock( &mutex1 );	
//JUNK CODE
	BYTE ch = cSeed[bReadByte];
//JUNK CODE
	_pthread_mutex_unlock( &mutex1 );

	_memset(bSend, 0, sizeof(bSend));
	_memcpy(bSend, pPacketData, 3);

	//Get-Bit
//JUNK CODE
	bSend[3] = ch>>(8-(bReadBit+1)) & 0x0001;
//JUNK CODE

#ifdef CTF_DEBUG
	printf("GET bReadByte = %d, bReadBit = %d\n", bReadByte, bReadBit);
	printf("Sending %.2X-%.2X-%.2X-%.2X\n", bSend[0], bSend[1], bSend[2], bSend[3]);
#endif
	_write(iSock, bSend, 4);
	
	return 0;
}

void* ProcessIncomingTraffic(void *ptr)
{

	unsigned char buffer[1024];
	int my_sock, client_sock, client_len;
	struct sockaddr_in svr_addr, client_addr;

	my_sock = _socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(my_sock < 0)
	{
		_perror("socket()");
		__exit(EXIT_FAILURE);
	}
	bzero((char *)&svr_addr, sizeof(svr_addr));

	svr_addr.sin_family = AF_INET;
	svr_addr.sin_addr.s_addr = INADDR_ANY;
	svr_addr.sin_port = _htons((int)ptr);

	if(_bind(my_sock, (struct sockaddr*)&svr_addr, sizeof(svr_addr)) < 0)
	{
		_perror("bind()");
		__exit(EXIT_FAILURE);
	}
	_listen(my_sock, 20);

	client_len = sizeof(client_addr);
	while(1)
	{
		client_sock = _accept(my_sock, (struct sockaddr*)&client_addr, &client_len);
		if(client_sock < 0)
			continue;

		bzero(buffer, sizeof(buffer));
		struct timeval tv;
		tv.tv_sec = 3;  /* 3 Secs Timeout */
		_setsockopt(client_sock, SOL_SOCKET, SO_RCVTIMEO,(struct timeval *)&tv,sizeof(struct timeval));

		int rlen = _read(client_sock, buffer, sizeof(buffer)-1);
		if(rlen > 0)
		{
			ProcessPacket(client_sock, buffer, rlen);
		}
		_close(client_sock);
	}
        return 0;
}

void signal_handler(int sig)
{
 
	switch(sig)
	{
		case SIGHUP:
			break;
		case SIGINT:
		case SIGTERM:
			time_to_quit = TRUE;
			break;
		default:
			break;
	}
}

int main(int argc, char *argv[])
{
	int i;

	EvilFunction1();
	EvilFunction2();

	//Infinite wait flag
	time_to_quit = FALSE;

	_printf("Schaurakith daemon starting up...\n");

	//Init mutex for logging

	//Setup signal handling before we start
	_signal(SIGHUP, signal_handler);
	_signal(SIGTERM, signal_handler);
	_signal(SIGINT, signal_handler);
	_signal(SIGQUIT, signal_handler);

	//Decoy shitz
       	CtfLib_Init(0);

	//PRNG Seed(time())
	_srand(_time(0));

	//Generate a 1 byte talk-key
	bTalkKey = _rand() % 0xff;


	//Get initial Flag
	PoolFlagChange();

#ifdef CTF_DEBUG
	printf("bTalkKey = %.2X, lSeed = %lu\n", bTalkKey, lSeed);
#endif

	//Broadcast flag Thread
        pthread_t tid;
        _pthread_create(&tid, NULL, BroadcastTraffic, NULL);

	//Process incoming packets and see if they fit the criteria to get flag bits
        _pthread_create(&tid, NULL, ProcessIncomingTraffic,(void*)17999);

	printf("Daemon started...\n");
	//Wait to exit
	time_t now, start;

	now = start = _time(NULL);
	while(time_to_quit != TRUE)
	{
		if(_difftime(now, start) > 120)
		{
			//Pool change after 2 minutes
			PoolFlagChange();
			start = now;
		}
		now = _time(NULL);
		_sleep(1);
	}

	__exit(EXIT_SUCCESS);
}
