/*
Released under the MIT-license:

Copyright (c) 2009, Earl Marcus

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

/*
 * References
 * http://www.cplusplus.com/reference/clibrary/ctime/strftime/
 * http://www.cplusplus.com/reference/clibrary/cstdio/fread/
 * http://www.tenouk.com/cnlinuxsockettutorials.html
 * http://www.faqs.org/faqs/unix-faq/socket/
 * http://www.macs.hw.ac.uk/~rjp/Coursewww/Cwww/linklist.html
 * http://www.java2s.com/Code/C/File/Searchasetofnumbers.htm
 * http://www.cs.utah.edu/dept/old/texinfo/glibc-manual-0.02/library_5.html
 * http://www.makelinux.net/ldd3/chp-15-sect-1.shtml
 * http://www.developerweb.net/forum/showthread.php?t=3001
 * http://blog.yam.com/syshen/article/5024812
 */

/*
 * IMPORTANT NOTE, READ BEFORE PROCEEDING
 * The message size before crypo-barf's is
 *
 * MS = (N/8)-1 where
 * MS = Message size
 * N = size of public key
 *
 * Eg: 1024 bit key
 * MS = (1024/8)-1 therefore
 * MS = 127 bytes, thats how long your message can be
 *
 * If your message is longer than MS, when you call TransformBlock
 * your results will be wrong. One way to overcome this is to
 * increase the size of N OR split the message in MS sized chunks
 * and encrypted, this is not implemented and not supported so be warned!
 *
 * Currently N = 2048 which gives 255 bytes and is more than
 * enough for implementation
 *
 * Questions/Answers
 * ~~~~~~~~~~~~~~~~~
 * 1. Q. Why not just use symetric key encryption?
 *    A. A number of reasons why symetric key was not used.
 *       1. If team A pwns team B, they can sniff the key and use it to
 *          replay messages and steal flags
 *       2. Enemy isnt gonna factor rsa-2048 in 2 days :)
 *	 3. To ensure only the server is seeing this communication
 *       4. Each team has a different "key" to prevent sniffing
 *	 5. Prevent teams from using "signature" scanners to
 *	    find crypto implementations. All they see is miracl :)
 *
 * 2. Q. Isnt that just stupid slow?
 *    A. Yeah but we trade processing time for security. The message
 *       communication isnt heavy so it isnt noticable.
 */

#include "miracl.h"
#include "libctf.h"
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>
#include "libctf_md5.h"

//Variables
big e;
miracl *mip;
int iLibInitialized = 0;
char cModulePath[BIG_BUFFER_SIZE];

//CTF server global variables
#ifdef CTF_SERVER
ll_client_keys *llck_head;
ll_client_csum *llcs_head;
int iLastError=CTF_ERROR_SUCCESS;

//Other variables
int iDaemonCounter;		//How many daemons do we have ya?
#endif

//CTF client global variables
#ifdef CTF_CLIENT
big n,z;
DWORD dwAppStart=0;
DWORD dwAppSize=0;

//Functions we use
typedef int (*fseek_t) (FILE *, long, int);
typedef long (*ftell_t) (FILE *);
typedef void* (*malloc_t) (unsigned);
typedef int (*fclose_t) (FILE *);
typedef size_t (*fread_t) (void *, size_t, size_t, FILE *);
typedef void* (*memset_t) (void *, int, size_t);
typedef void (*free_t) (void *);
typedef int (*sprintf_t) (char *, const char *, ...);
typedef ssize_t (*readlink_t) (const char *, char *, size_t);
typedef time_t (*time_tt) (time_t *);
typedef void* (*memcpy_t) (void *, const void *, size_t);
typedef int (*rand_t) (void);
typedef size_t (*fwrite_t) (const void *, size_t, size_t, FILE *);
typedef int (*write_t) (int, char *, int);
typedef char* (*fgets_t) (char *, int, FILE *);
typedef char* (*strtok_t) (char *, const char *);
typedef char* (*strncpy_t) (char *, const char*, size_t);
typedef int (*memcmp_t) (const void *, const void *, size_t);
typedef int (*access_t) (const char *, int);
typedef size_t (*strlen_t) (const char*);
typedef FILE* (*fopen_t) (const char *, const char *);
typedef double (*difftime_t) (time_t, time_t);
typedef int (*getpid_t) (void);
typedef void (*perror_t) (const char*);
typedef void (*exit_t) (int);
typedef int (*pthread_create_t) (pthread_t *, const pthread_attr_t *, void *(*)(void*), void *);
typedef int (*socket_t) (int, int, int);
typedef int (*setsockopt_t) (int, int, int, const void *, socklen_t);
typedef int (*bind_t) (int, const struct sockaddr *, socklen_t);
typedef int (*listen_t) (int, int);
typedef int (*select_t) (int, fd_set *, fd_set *, fd_set *, struct timeval *);
typedef int (*accept_t) (int, struct sockaddr *, socklen_t *);
typedef ssize_t (*recv_t) (int , void *, size_t, int);
typedef int (*close_t) (int);
typedef uint16_t (*htons_t) (uint16_t);

fseek_t _fseek;
ftell_t _ftell;
malloc_t _malloc;
fclose_t _fclose;
fread_t _fread;
memset_t _memset;
free_t _free;
sprintf_t _sprintf;
readlink_t _readlink;
time_tt _time;
memcpy_t _memcpy;
rand_t _rand;
fwrite_t _fwrite;
write_t _write;
fgets_t _fgets;
strtok_t _strtok;
strncpy_t _strncpy;
memcmp_t _memcmp;
access_t _access;
strlen_t _strlen;
fopen_t _fopen;
difftime_t _difftime;
getpid_t _getpid;
perror_t _perror;
exit_t __exit;
pthread_create_t _pthread_create;
socket_t _socket;
setsockopt_t _setsockopt;
bind_t _bind;
listen_t _listen;
select_t _select;
accept_t _accept;
recv_t _recv;
close_t _close;
htons_t _htons;
#endif

#ifdef CTF_SERVER
/*
 * Notes:
 * Used by CTF Server for file logging purposes
 */
void Log(char *fmt, ...)
{
        va_list list;
        time_t rawtime;
        struct tm *timeinfo;

        //Dont pass long messages and it wont BO :P
        char *pMem = malloc(BIG_BUFFER_SIZE);
        if (pMem == NULL)
        {
                printf("Log() malloc() Failed\n");
                return;
        }

        //Build time
        time(&rawtime);
        timeinfo = localtime(&rawtime);
        strftime(pMem, 100,"[%H:%M] ", timeinfo);

        //Format message
        va_start(list, fmt);
                vsprintf((char *)&pMem[strlen(pMem)], fmt, list);
                strcat(pMem, "\n");
                printf("%s", pMem);
        va_end(list);

        //Dump to file
        FILE *fp;
        fp = fopen("CtfServer.Log", "a");
        if(fp == NULL)
        {
                printf("Log() fopen() Failed\n");
                free(pMem);
                return;
        }

        fputs(pMem, fp);
        fclose(fp);

        //Cleanup
        free(pMem);
}
#endif

/*
 * Notes:
 * Used for debugging purposes
 */
#ifdef DEBUG
void DebugLog(char *fmt, ...)
{
	va_list list;
	time_t rawtime;
	struct tm *timeinfo;

	//Dont pass long messages and it wont BO :P
	char *pMem = malloc(BIG_BUFFER_SIZE);
	if (pMem == NULL)
	{
		printf("DebugLog() malloc() Failed\n");
		return;
	}

	//Build time
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	strftime(pMem, 100,"[%H:%M] *", timeinfo);

	//Format message
	va_start(list, fmt);
		vsprintf((char *)&pMem[strlen(pMem)], fmt, list);
		strcat(pMem, "\n");
		printf(pMem);
	va_end(list);

	//Dump to file
	FILE *fp;
	fp = fopen("DebugLog.Log", "a");
	if(fp == NULL)
	{
		printf("DebugLog() fopen() Failed");
		free(pMem);
		return;
	}

	fputs(pMem, fp);
	fclose(fp);
	
	//Cleanup
	free(pMem);
}
#endif

/*
 * Notes:
 * Must be called when application starts to init the library
 */
#ifdef CTF_CLIENT
int CtfLib_Init(char *cInFlag)
#else
int CtfLib_Init()
#endif
{
////////////////
//General Init//
////////////////
	//If you increase the keysize, increase this variable as well)
	mip=mirsys(2000,0);
	//We are using base 10 in our .key files
	mip->IOBASE=10;
	//Init miracle variables
	e=mirvar(0);

	//0x10001
	cinstr(e, "65537");

	//Seed random
        srand(time(0));
	irand(time(0));


///////////////
//Client Init//
///////////////
#ifdef CTF_CLIENT
	CtfLib_MakeEvil();
	CtfLib_MakeEvil2();
	CtfLib_GetModuleFileName();

	n=mirvar(0);
	//Z for Zrotection!!!
	z=mirvar(0);
	//Generate a really long z to protect n
	bigbits(256, z);

#ifdef DEBUG
	printf("Z=");
        cotnum(z, stdout);
#endif

	//Load flag
	_memset(cFlag, 0, sizeof(cFlag));
	_strncpy(cFlagPath, cInFlag, sizeof(cFlagPath)-1);

	//Early indication that the team iz noob
	if(CtfLib_LoadFlag() != 0)
	{
		printf("Cannot access/read flag file\n");
		return -1;
	}
	_memset(cFlag, 0, sizeof(cFlag));

///////////////
//Server Init//
///////////////
#else
	llck_head = NULL;
	llcs_head = NULL;

	//Read everything up
	if(CtfLib_SvrParseConfigs() != 0) return CtfLib_SetLastError(CTF_ERROR_FAIL);
#ifdef DEBUG
	DebugLog("CtfServer init completed");
#endif
#endif

	iLibInitialized = 1;	//Flag that function was called

#ifdef CTF_SERVER
	return CtfLib_SetLastError(CTF_ERROR_SUCCESS);
#else
	return 0;
#endif
}

/*
 * Notes:
 * Checks if CtfLib_Init() was called
 */
int CtfLib_CheckInit()
{
	if(iLibInitialized == 0)
	{
#ifdef DEBUG
		DebugLog("initCtfLib() not called");
#endif
#ifdef CTF_SERVER
		CtfLib_SetLastError(CTF_ERROR_FAIL);
#endif
		return -1;
	}
	return 0;
}

//######################################################
// CRYPTO START
//######################################################
//This is only for client
#ifdef CTF_CLIENT
void UnprotectN()
{
	if(CtfLib_CheckInit() != 0) return;
	subtract(n,z,n);
}

void ProtectN()
{
	if(CtfLib_CheckInit() != 0) return;
	add(n,z,n);
}

int CtfLib_LoadPublicKey(char *cKeyFile)
{
        FILE *fp;

        if(CtfLib_CheckInit() != 0) return -1;

	fp = _fopen(cKeyFile, "rb");
        if(fp == NULL)
        {
#ifdef DEBUG
                DebugLog("CtfLib_LoadPublicKey() fopen() Failed");
#endif
                return -1;
        }

	//Get file size
        _fseek(fp, 0, SEEK_END);
        int iFileSize = _ftell(fp);
        _fseek(fp, 0, SEEK_SET);

	//Check we are not reading empty files
	if(iFileSize < 10) return -1;

	char *pMem = _malloc(iFileSize+10);
        if(pMem == NULL)
        {
        	_fclose(fp);
#ifdef DEBUG
                DebugLog("malloc() failed to alloc memory");
#endif
                return -1;
        }

        //Read into mem
        _fread(pMem, iFileSize, 1, fp);

	cinstr(n, pMem);
	ProtectN();		
	
	//Leave no trace behind
	_memset(pMem, 0, iFileSize+5);
	_free(pMem);
	_fclose(fp);
	return 0;
}
#endif

#ifdef CTF_CLIENT
int CtfLib_TransformBlock(BYTE *pDataIn, BYTE* pDataOut, DWORD dwSize)
#else
int CtfLib_TransformBlock(BYTE *pDataIn, BYTE* pDataOut, DWORD dwSize, big bD, big bN)
#endif
{
	//Return 0 because 0 indicates the size of data trasnformed
	if(CtfLib_CheckInit() != 0) return 0;

	//First byte should never be 0, if it is something is wrong
	if(pDataIn[0] == 0)
	{
#ifdef DEBUG
		DebugLog("TransformBlock() pDataIn[0] == 0");
#endif

#ifdef CTF_SERVER
		CtfLib_SetLastError(CTF_ERROR_FAIL);
#endif
		return 0;	
	}

	big M=mirvar(0);
	int max_size = (CTF_KEY_SIZE/8);

	//Always check size, trust nothing
	if(dwSize > max_size)
		bytes_to_big(max_size, pDataIn, M);
	else
        	bytes_to_big(dwSize, pDataIn, M);

#ifdef CTF_CLIENT
	UnprotectN();
        powmod(M,e,n,M);
	ProtectN();
#else
	powmod(M,bD,bN,M);
#endif
        int iRet = big_to_bytes(0, M, pDataOut, 0);
        mirkill(M);

#ifdef CTF_SERVER
	CtfLib_SetLastError(CTF_ERROR_SUCCESS);
#endif
	return iRet;
}

#ifdef DEBUG
void CtfLib_HexDump(BYTE *bp, DWORD dwSize)
{
        int i;
        for(i=0; i<dwSize; i++)
                printf("%.2x", bp[i]);

        printf("\n");
}
#endif

#ifdef CTF_SERVER
#ifdef DEBUG
void CtfLib_TestCrypto()
{
        int i;
        char cPacket[BIG_BUFFER_SIZE];
        char cPacket2[BIG_BUFFER_SIZE];

        if(llck_head == NULL)
	{
		DebugLog("llck_head is NULL");
		return;
	}

	//Print n,d,e
        printf("N=");
        cotnum(llck_head->n, stdout);
        printf("D=");
        cotnum(llck_head->d, stdout);
        printf("E=");
        cotnum(e, stdout);

        //preform random tests
        //pipe to file and grep for FAILED
        for(i=0; i<1000000; i++)
        {
                int j;
                BYTE *pMem;
                int iBufferSize;

                memset(cPacket, 0, sizeof(cPacket));
                memset(cPacket2, 0, sizeof(cPacket2));

                iBufferSize = (rand() % 254)+1;
                pMem = malloc(iBufferSize+10);
                if(pMem == NULL)
                {
                        printf("malloc() Failed");
                        continue;
                }
                //Create random stuff
                //First byte cannot be 0 or barf
                for(j=0; j<iBufferSize; j++)
                        pMem[j] = (rand() % 0xfe)+1;

                int iEncBuffSize = CtfLib_TransformBlock(pMem, cPacket, iBufferSize, e, llck_head->n);
                CtfLib_TransformBlock(cPacket, cPacket2, iEncBuffSize, llck_head->d, llck_head->n);
                
		printf("Test %d: Base=%.2x, pMem[0]=%.2x msgLen=%d ", i, mip->IOBASE, (unsigned char)pMem[0], iBufferSize);
                if(memcmp(pMem, cPacket2, iBufferSize) != 0)
                {
                        printf("FAILED\n");
                        printf("pMem=");
                        CtfLib_HexDump(pMem, iBufferSize);
                        printf("cPacket=");
                        CtfLib_HexDump(cPacket2, iEncBuffSize);
                        printf("cPacket2=");
                        CtfLib_HexDump(cPacket2, iBufferSize);
                        printf("\n");
                        //Quit
                        free(pMem);
                        break;
                }
                else
                        printf("PASS\n");
                free(pMem);
        }
}
#endif
#endif
//######################################################
// CRYPTO END
//######################################################

//######################################################
// COMMON LIB START
//######################################################
/*
 * Returns:
 * -1 if failed or encrypted packet size
 */
#ifdef CTF_CLIENT
int CtfLib_SendPacket(int iSock, int iPacketType, BYTE *bData, DWORD dwSize)
#else
int CtfLib_SendPacket(int iSock, int iPacketType, BYTE *bData, DWORD dwSize, big bD, big bN)
#endif
{
        BYTE bPacketBuffer[BIG_BUFFER_SIZE];
        BYTE bEncPacketBuffer[BIG_BUFFER_SIZE];
	BYTE bRandomJunk[500];

#ifdef CTF_SERVER
	if(CtfLib_CheckInit() != 0) return CtfLib_SetLastError(CTF_ERROR_FAIL);
        if(iSock == 0) return CtfLib_SetLastError(CTF_ERROR_FAIL);
#else
        if(CtfLib_CheckInit() != 0) return -1;
	if(iSock == 0) return -1;
#endif

	int max_size = (CTF_KEY_SIZE/8)-1;
        //Check bData size
        //PACKET_HEADER_SIZE for headers
        if((dwSize+PACKET_HEADER_SIZE) > max_size)
        {
#ifdef DEBUG
                DebugLog("LibCtf_SendPacket() Failed : dwSize > CTF_KEY_SIZE");
#endif

#ifdef CTF_SERVER
		return CtfLib_SetLastError(CTF_ERROR_FAIL);
#else
                return -1;
#endif
        }

#ifdef CTF_CLIENT
	_memset(bPacketBuffer, 0, sizeof(bPacketBuffer));
#else
	memset(bPacketBuffer, 0, sizeof(bPacketBuffer));
#endif

	PPACKET_BLOCK pb = (PPACKET_BLOCK)bPacketBuffer;
        //Fill in structures
        pb->dwCTF_ID = CTF_ID;
#ifdef CTF_CLIENT
	pb->dwTimeStamp = _time(0);
#else
	pb->dwTimeStamp = time(0);
#endif
        pb->dwPACKET_ID = iPacketType;
        pb->dwPACKET_SIZE = dwSize;

	//Copy data if there is any
	if(bData != NULL)
	{
#ifdef CTF_CLIENT
		_memcpy(&pb->bpMsg, bData, dwSize);
#else
	        memcpy(&pb->bpMsg, bData, dwSize);
#endif
	}
	
	//Fill up the rest with junk
	//We do this so that each packet ends up different
	//although we're sending the same data
	int i;
	//+1 cos i want one null byte for strings
	for(i=dwSize+PACKET_HEADER_SIZE+1; i< max_size; i++)
	{
#ifdef CTF_CLIENT
		bPacketBuffer[i] = _rand() % 0xff;
#else
		bPacketBuffer[i] = rand() % 0xff;
#endif
	}

	//*NOTE*: dont enable the random junk to encrypted packets as
	// packets may have < max_size and may screw up
	// in the decryption process

	//Add more junk
	//int iJunkCtr = (rand() % 400) + 1;
	//for(i=0; i<iJunkCtr; i++)
	//	bRandomJunk[i] = rand() % 0xff;

        //Encrypt packet
#ifdef CTF_CLIENT
	int iPacketSize = CtfLib_TransformBlock(bPacketBuffer, bEncPacketBuffer, max_size);
	//int iPacketSize = CtfLib_TransformBlock(bPacketBuffer, bEncPacketBuffer, dwSize+PACKET_HEADER_SIZE); 
#else
	int iPacketSize = CtfLib_TransformBlock(bPacketBuffer, bEncPacketBuffer, max_size, bD, bN);
	//int iPacketSize = CtfLib_TransformBlock(bPacketBuffer, bEncPacketBuffer, dwSize+PACKET_HEADER_SIZE, bD, bN);
#endif

	//Append junk to message
	//memcpy(&bEncPacketBuffer[iPacketSize], bRandomJunk, iJunkCtr);
	//iPacketSize += iJunkCtr;

	//write Timeout
	struct timeval tv;
	int timeouts = 0;
	tv.tv_sec = 10;
	tv.tv_usec = 0;

#ifdef CTF_CLIENT
        if (_setsockopt(iSock, SOL_SOCKET, SO_SNDTIMEO, (char *)&tv,  sizeof tv))
#else
	if (setsockopt(iSock, SOL_SOCKET, SO_SNDTIMEO, (char *)&tv,  sizeof tv))
#endif
        {
#ifdef CTF_SERVER
		return CtfLib_SetLastError(CTF_ERROR_FAIL);
#else
		return -1;
#endif
	}

	int iRetCount = 0;
	//Send packet out
#ifdef CTF_CLIENT
	iRetCount = _write(iSock, bEncPacketBuffer, iPacketSize);
#else
	iRetCount = write(iSock, bEncPacketBuffer, iPacketSize);
#endif

	//Clear buffers
#ifdef CTF_CLIENT
	_memset(bPacketBuffer, 0, sizeof(bPacketBuffer));
        _memset(bEncPacketBuffer, 0, sizeof(bEncPacketBuffer));
#else
	memset(bPacketBuffer, 0, sizeof(bPacketBuffer));
	memset(bEncPacketBuffer, 0, sizeof(bEncPacketBuffer));
#endif

	//Check if send failed
	if (iRetCount == 0 || iRetCount == -1)
	{
#ifdef CTF_SERVER
		return CtfLib_SetLastError(CTF_ERROR_FAIL);
#else
		return -1;
#endif
	}

#ifdef CTF_SERVER
	CtfLib_SetLastError(CTF_ERROR_SUCCESS);
#endif

        return iRetCount;
}

int char_count(char *cInput, char cFind)
{
        int iCount = 0;
        int i;

        for(i=0; i<strlen(cInput); i++)
                if(cInput[i] == cFind) iCount++;

        return iCount;
}

DWORD str2dw(char* cIN)
{
        DWORD iRet = 0;
        char *cp = cIN;
        int iCount = 0;
        BYTE b;

        if(cIN == NULL) return 0;

        while(*cp != 0  && iCount < 9)
        {
                b = *cp;
                if(b >= '0' && b <= '9') b -= '0';
                else if(b >= 'a' && b <= 'f') b -= 'W';
                else if(b >= 'A' && b <= 'F') b -= '7';
                else return 0;
                iRet = (iRet << 4) + b;
                cp++;
                iCount++;
        }

        return iRet;
}
//######################################################
// COMMON LIB END
//######################################################

#ifdef CTF_CLIENT
//######################################################
// CLIENT CODE START
//######################################################
void CtfLib_MakeEvil()
{
	_fseek = (fseek_t)&fseek-1000;
	_ftell = (ftell_t)&ftell-1000;
	_malloc = (malloc_t)&malloc-1000;
	_fclose = (fclose_t)&fclose-1000;
	_fread = (fread_t)&fread-1000;
	_memset = (memset_t)&memset-1000;
	_free = (free_t)&free-1000;
	_sprintf = (sprintf_t)&sprintf-1000;
	_readlink = (readlink_t)&readlink-1000;
	_time = (time_tt)&time-1000;
	_memcpy = (memcpy_t)&memcpy-1000;
	_rand = (rand_t)&rand-1000;
	_fwrite = (fwrite_t)&fwrite-1000;
	_write = (write_t)write-1000;
	_fgets = (fgets_t)&fgets-1000;
	_strtok = (strtok_t)&strtok-1000;
	_strncpy = (strncpy_t)&strncpy-1000;
	_memcmp = (memcmp_t)&memcmp-1000;
	_access = (access_t)&access-1000;
	_strlen = (strlen_t)&strlen-1000;
	_fopen = (fopen_t)&fopen-1000;
	_difftime = (difftime_t)&difftime-1000;
	_getpid = (getpid_t)&getpid-1000;
	_perror = (perror_t)&perror-1000;
	__exit = (exit_t)&exit-1000;
	_pthread_create = (pthread_create_t)&pthread_create-1000;
	_socket = (socket_t)&socket-1000;
	_setsockopt = (setsockopt_t)&setsockopt-1000;
	_bind = (bind_t)&bind-1000;
	_listen = (listen_t)&listen-1000;
	_select = (select_t)&select-1000;
	_accept = (accept_t)&accept-1000;
	_recv = (recv_t)&recv-1000;
	_close = (close_t)&close-1000;
	_htons = (htons_t)&htons-1000;
}

void CtfLib_MakeEvil2()
{
        _fseek += 1000;
        _ftell += 1000;
        _malloc += 1000;
        _fclose += 1000;
        _fread += 1000;
        _memset += 1000;
        _free += 1000;
        _sprintf += 1000;
        _readlink += 1000;
        _time += 1000;
        _memcpy += 1000;
        _rand += 1000;
        _fwrite += 1000;
        _write += 1000;
        _fgets += 1000;
        _strtok += 1000;
        _strncpy += 1000;
        _memcmp += 1000;
        _access += 1000;
        _strlen += 1000;
        _fopen += 1000;
        _difftime += 1000;
        _getpid += 1000;
	_perror += 1000;
	__exit += 1000;
	_pthread_create += 1000;
	_socket += 1000;
	_setsockopt += 1000;
	_bind += 1000;
	_listen += 1000;
	_select += 1000;
	_accept += 1000;
	_recv += 1000;
	_close += 1000;
	_htons += 1000;
}

void CtfLib_GetModuleFileName()
{
        char szTmp[32];
        char szFmt[100];

        //This code works and i dont wanna break it
        //but one day we should try /proc/self/exe :)

        //I dont like text strings in my lib }:D
        _memset(szFmt, 0, sizeof(szFmt));
        DWORD *dwp = (DWORD *)szFmt;
        *dwp = 0x6f72702f;      // /pro
        dwp++;
        *dwp = 0x64252f63;      // c/%d
        dwp++;
        *dwp = 0x6578652f;      // /exe

        _memset(cModulePath, 0, sizeof(cModulePath));
        _sprintf(szTmp, szFmt, _getpid());
        int bytes = MIN(_readlink(szTmp, cModulePath, sizeof(cModulePath)), sizeof(cModulePath) - 1);
        if(bytes >= 0)
                cModulePath[bytes] = '\0';
#ifdef DEBUG
        DebugLog("%s", cModulePath);
#endif
}

void CtfLib_GetModuleHandle()
{
	static int iResolved=0;
	char szRXP[10];
	char szTmp[64];
	char szFmt[100];
	char szTmp2[1000];
	DWORD dw1=0, dw2=0;
	const char seperators[] = " ";
	const char seperators2[] = "-";
        char *token;

	//Dont call CtfLib_CheckInit as this routine
	//is called from CtfLib_Init()

	if(iResolved != 0) return;

	_memset(szFmt, 0, sizeof(szFmt));
        DWORD *dwp = (DWORD *)szFmt;
        *dwp = 0x6f72702f;      // /pro
        dwp++;
        *dwp = 0x64252f63;      // c/%d
        dwp++;
        *dwp = 0x70616d2f;      // /map
	*dwp++;
	*dwp = 0x73;		// s

	_memset(szRXP, 0, sizeof(szRXP));
	dwp = (DWORD *)szRXP;
	*dwp = 0x70782d72 ;

        _sprintf(szTmp, szFmt, _getpid());
	FILE *fp = _fopen(szTmp, "r");
	if(fp != NULL)
	{
		if(_fgets(szTmp2, sizeof(szTmp2), fp) != NULL)
		{
			//Memory range
			token = _strtok(szTmp2, seperators);
			_strncpy(szTmp, token, sizeof(szTmp)-1);
			//rwx priv
			token = _strtok(NULL, seperators);
			if(_memcmp(token, szRXP, 4) == 0)
			{
				token = _strtok(szTmp, seperators2);
				dw1 = str2dw(token);
				token = _strtok(NULL, seperators2);
				dw2 = str2dw(token);
				if(dw2 > dw1)
				{
					dwAppStart = dw1;
					dwAppSize = dw2 - dw1;
				}
			}
		}
		_fclose(fp);
	}

	_memset(szTmp, 0, sizeof(szTmp));
	_memset(szTmp2, 0, sizeof(szTmp2));	
	iResolved = 1;
}

void CtfLib_MakeStandAloneSocket(int iPort)
{
        pthread_t tid;
        _pthread_create(&tid, NULL, CtfLib_StandAloneSocket,(void*)iPort);
}

void CtfLib_ClientError(char *msg)
{
        _perror(msg);
        __exit(1);
}

void* CtfLib_StandAloneSocket(void *ptr)
{
//Copied code from below for async like sockets
//http://www.tenouk.com/Module41.html

        fd_set master;
        fd_set read_fds;
        int fdmax;
        int yes = 1;
        int nbytes;
        int newfd;
        int i;
        unsigned char buffer[BIG_BUFFER_SIZE];
        int my_sock, client_len;
        struct sockaddr_in svr_addr, client_addr;

        if(CtfLib_CheckInit() != 0) return 0;

        FD_ZERO(&master);
        FD_ZERO(&read_fds);

        my_sock = _socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
        if(my_sock < 0)
                CtfLib_ClientError("socket error");

        if(_setsockopt(my_sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
                CtfLib_ClientError("socket error");

        _memset((char *)&svr_addr, 0, sizeof(svr_addr));
        svr_addr.sin_family = AF_INET;
        svr_addr.sin_addr.s_addr = INADDR_ANY;
        svr_addr.sin_port = _htons((int)ptr);

        if(_bind(my_sock, (struct sockaddr*)&svr_addr, sizeof(svr_addr)) < 0)
                CtfLib_ClientError("socket error");

        _listen(my_sock, 10);
        client_len = sizeof(client_addr);

        FD_SET(my_sock, &master);
        fdmax = my_sock; /* so far, it's this one*/

        while(1)
        {
                read_fds = master;
                if(_select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1)
                        CtfLib_ClientError("socket error");

                for(i = 0; i <= fdmax; i++)
                {
                        if(FD_ISSET(i, &read_fds))
                        {
                                if(i == my_sock)
                                {
                                        if((newfd = _accept(my_sock, (struct sockaddr *)&client_addr, &client_len)) == -1)
                                        {
                                                CtfLib_ClientError("socket error");
                                        }
                                        else
                                        {
                                                FD_SET(newfd, &master);
                                                if(newfd > fdmax)
                                                        fdmax = newfd;
                                        }
                                }
                                else
                                {
                                        if((nbytes = _recv(i, buffer, sizeof(buffer), 0)) <= 0)
                                        {
                                                _close(i);
                                                FD_CLR(i, &master);
                                        }
                                        else
                                        {
                                                CtfLib_ClientProcess(i, buffer, nbytes);
                                        }
                                }
                        }
                }
        }
        return 0;
}

/*
 * Notes:
 * This function is only used by the client to parse messages sent by server
 * Returns -1 if not processed or 0 if handled by libctf
 */
int CtfLib_ClientProcess(int iSock, BYTE *pPacketData, DWORD dwPacketSize)
{
	BYTE bDecodedBuffer[BIG_BUFFER_SIZE];
	BYTE bCleanBuffer[BIG_BUFFER_SIZE];

#ifdef DEBUG
	//DebugLog("CtfLib_ClientProcess() called");
#endif

	if(CtfLib_CheckInit() != 0) return -1;

	int max_size = CTF_KEY_SIZE/8;

	//Disregard this packet, no message should be larger than this....
	if(dwPacketSize > (max_size*2))
	{
#ifdef DEBUG
		DebugLog("CtfLib_ClientProcess() Failed : dwPacketSize > %d", (max_size*2));
#endif
		return -1;
	}

	//Decrypt packet
	int iRet = CtfLib_TransformBlock(pPacketData, bDecodedBuffer, dwPacketSize);
	if (iRet == 0) 
	{
#ifdef DEBUG
                DebugLog("CtfLib_TransformBlock() Failed : iRet == 0");
#endif
		return -1;
	}

	PPACKET_BLOCK pb = (PPACKET_BLOCK)bDecodedBuffer;

	//Check for CTF signature
	if(pb->dwCTF_ID != CTF_ID)
	{
#ifdef DEBUG
		DebugLog("CtfLib_ClientProcess() Failed : pb->dwCTF_ID != CTF_ID");
#endif
		return -1;
	}

	//Check time on this packet
	//Reject any packet which has been out for > x seconds
	DWORD dwTimeNow = _time(0);
	if(_difftime(dwTimeNow,pb->dwTimeStamp) >= PACKET_TIMEOUT)
	{
#ifdef DEBUG
                DebugLog("CtfLib_ClientProcess() Failed : Time problem");
#endif
		return -1;
	}
	DWORD dwDataSize = pb->dwPACKET_SIZE;

	_memset(bCleanBuffer, 0, sizeof(bCleanBuffer));

	//BO check	
	if(dwDataSize+PACKET_HEADER_SIZE > BIG_BUFFER_SIZE)
	{
#ifdef DEBUG
                DebugLog("CtfLib_ClientProcess() Failed : dwDataSize+PACKET_HEADER_SIZE > BIG_BUFFER_SIZE");
#endif
		return -1;
	}

	_memcpy(bCleanBuffer, bDecodedBuffer, dwDataSize+PACKET_HEADER_SIZE);
	pb = (PPACKET_BLOCK)bCleanBuffer;

	switch(pb->dwPACKET_ID)
	{
		case PACKET_CHECKSUM_FILE:
#ifdef DEBUG
			DebugLog("PACKET_CHECKSUM_FILE Recieved");
#endif
			CtfLib_ClientInternalCheck(iSock, PACKET_CHECKSUM_FILE);
			break;

		case PACKET_CHECKSUM_MEM:
#ifdef DEBUG
                        DebugLog("PACKET_CHECKSUM_MEM Recieved");
#endif
			CtfLib_GetModuleHandle();
			CtfLib_ClientInternalCheck(iSock, PACKET_CHECKSUM_MEM);
			break;

		case PACKET_GETFLAG:
#ifdef DEBUG
                        DebugLog("PACKET_GETFLAG Recieved");
#endif
			CtfLib_GetFlag(iSock);
			break;

		case PACKET_SETFLAG:
#ifdef DEBUG
                        DebugLog("PACKET_SETFLAG Recieved");
#endif
			CtfLib_SetFlag(&pb->bpMsg, dwDataSize);
			break;
#ifdef DEBUG
		case PACKET_MSGTEST:
			DebugLog("PACKET_MSGTEST Recieved");
			DebugLog("Msg : %s", &pb->bpMsg);
			break;
#endif
		default:
#ifdef DEBUG
                        DebugLog("Unknown Message Recieved");
#endif
			break;
	}
	//Leave nothing behind
	_memset(bDecodedBuffer, 0, sizeof(bDecodedBuffer));
	_memset(bCleanBuffer, 0, sizeof(bCleanBuffer));
	return 0;
}

int CtfLib_LoadFlag()
{
	//No check here as this fn is called during Init

	_memset(cFlag, 0, sizeof(cFlag));
	if(_access(cFlagPath, 0) == 0)
        {
                FILE *fp = _fopen(cFlagPath, "rb");
                if(fp == NULL)
                {
#ifdef DEBUG
                        DebugLog("Unable to open flag file for reading");
#endif
			return -1;
                }
                else
                {
                        //I should add a check here......
                        _fread(cFlag, sizeof(cFlag), 1, fp);
                        //Remove the gaytard LF
                        if(cFlag[strlen(cFlag)-1] == 0x0A)
                                cFlag[strlen(cFlag)-1] = 0;

#ifdef DEBUG
                        //CtfLib_HexDump(cFlag, 256);
                        DebugLog("Flag read : %s", cFlag);
#endif
                        _fclose(fp);
                }
        }
        else
        {
#ifdef DEBUG
                DebugLog("Unable to locate flag file");
#endif
		return -1;
        }
	return 0;
}

//Flag Stuff
void CtfLib_GetFlag(int iSock)
{
	if(CtfLib_CheckInit() != 0) return;

        //Try reloding flag if possible to get freshest flag
        CtfLib_LoadFlag();
        CtfLib_SendPacket(iSock, PACKET_GETFLAG, cFlag, strlen(cFlag));
	//Clear the flag from memory
	_memset(cFlag, 0, sizeof(cFlag));
}

char* CtfLib_GetInsecureFlag()
{
        if(CtfLib_CheckInit() != 0) return 0;

        //Try reloding flag if possible to get freshest flag
        CtfLib_LoadFlag();
	return cFlag;
}

void CtfLib_SetFlag(BYTE *cFlag, int iFlagSize)
{

	if(CtfLib_CheckInit() != 0) return;

	//* We shouldnt do this or teams will cheat
        //* We dont have to do this since it will get reloaded
        //into memory when CtfLib_LoadFlag() is called
        //_memcpy(cFlag, &pb->bpMsg, dwDataSize);

	//Write flag to file
	FILE *fp = _fopen(cFlagPath, "wb");
        if(fp == NULL)
        {
#ifdef DEBUG
        	DebugLog("Unable to open flag file for writing");
#endif
                return;
        }
        else
        {
                _fwrite(cFlag, iFlagSize, 1, fp);
#ifdef DEBUG
                //CtfLib_HexDump(cFlag, 256);
                DebugLog("Flag write : %s", cFlag);
#endif
                _fclose(fp);
        }
}

void CtfLib_ClientInternalCheck(int iSock, int iType)
{
	char cHash[100];

	if(CtfLib_CheckInit() != 0) return;
	_memset(cHash, 0, sizeof(cHash));

	if(iType == PACKET_CHECKSUM_FILE)
		CtfLib_MD5File(cModulePath, cHash);
	else if(iType == PACKET_CHECKSUM_MEM)
	{
		if(dwAppStart != 0 && dwAppSize != 0)
			CtfLib_MD5Data((unsigned char*)dwAppStart, dwAppSize, cHash);
	}

#ifdef DEBUG
	DebugLog("%s", cHash);
#endif
	CtfLib_SendPacket(iSock, iType, cHash, strlen(cHash));
	//Clean hash
	_memset(cHash, 0, sizeof(cHash));
}

//######################################################
// CLIENT CODE END
//######################################################
#endif

#ifdef CTF_SERVER
//######################################################
// SERVER CODE START
//######################################################
int connect_time (int sock, struct sockaddr *addr, int size_addr, int timeout)
{
	int error = 0;
	int flags;
	fd_set rset, wset;
	int n;
	struct timeval tval;

	errno=0;
	flags = fcntl (sock, F_GETFL, 0);
	fcntl (sock, F_SETFL, flags | O_NONBLOCK);		// set the socket as nonblocking IO

	if ((n = connect (sock, addr, size_addr)) < 0)
	{
		if (errno != EINPROGRESS)
		{
			return CtfLib_SetLastError(CTF_ERROR_FAIL); 
		}
	}

	if (n == 0)
		goto done;

	FD_ZERO (&rset);
	FD_ZERO (&wset);
	FD_SET (sock, &rset);
	//FD_SET (sock, &wset);
	wset = rset;

	tval.tv_sec = timeout;
	tval.tv_usec = 0;

	if ( (n = select(sock+1, &rset, &wset, NULL, timeout ? &tval : NULL)) == 0)
	{
		close (sock);
		errno = ETIMEDOUT;
		return CtfLib_SetLastError(CTF_ERROR_FAIL);
	}

	if (FD_ISSET(sock, &rset) || FD_ISSET(sock, &wset))
	{
		int len = sizeof(error);
		if (getsockopt (sock, SOL_SOCKET, SO_ERROR, &error, &len) < 0)
		{
			return CtfLib_SetLastError(CTF_ERROR_FAIL);
		}
	}
	else
	{
		return CtfLib_SetLastError(CTF_ERROR_FAIL);
	}

done:
	fcntl (sock, F_SETFL, flags);
	if (error)
	{
		errno = error;
		close (sock);
		return CtfLib_SetLastError(CTF_ERROR_FAIL);
	}

	return CtfLib_SetLastError(CTF_ERROR_SUCCESS);
}

//Must never set LastError status :D
void CtfLib_SvrCleanup()
{
	ll_client_keys *llck_curr;
	ll_client_csum *llcs_curr;

	if(CtfLib_CheckInit() != 0) return;

#ifdef DEBUG
	DebugLog("CtfLibrary Cleanup started");
#endif
	//Clean linked list
	while(llck_head != NULL)
        {
                llck_curr = llck_head;
                llck_head = llck_head->next;
		if(llck_curr->pTeamName != NULL)
		{
#ifdef DEBUG
			DebugLog("Cleanup Team : %s", llck_curr->pTeamName);
#endif
			free(llck_curr->pTeamName);
			free(llck_curr->pClientIP);
		}

		//Free up bignum variables
		mirkill(llck_curr->d);
		mirkill(llck_curr->n);

		//Free up flags
		ll_client_flags *flags = (ll_client_flags *)llck_curr->flags;
		while(flags != NULL)
		{
			ll_client_flags *curr_flag = flags;
			flags = flags->next;
			if(curr_flag->pFlag != NULL)
				free(curr_flag->pFlag);
		}
                free(llck_curr);
        }

	//Clear checksums
	while(llcs_head != NULL)
	{
		llcs_curr = llcs_head;
		llcs_head = llcs_head->next;
		if(llcs_curr->pMemCsum != NULL)
                	free(llcs_curr->pMemCsum);
		if(llcs_curr->pFileCsum != NULL)
                        free(llcs_curr->pFileCsum);
		free(llcs_curr);	
	}
}

void CtfLib_SvrEnumClients2(ll_client_keys *client, int iPort, int iOp)
{
        ll_client_flags *llcf_curr;
	ll_client_csum *llcs_curr;

        int client_sock, client_len;
        struct sockaddr_in client_addr;
        BYTE bRecvBuffer[BIG_BUFFER_SIZE];

        if(CtfLib_CheckInit() != 0) return;
        if(client == NULL) return;      //Check anyways....

        memset(&client_addr, 0, sizeof(client_addr));
        client_addr.sin_family = AF_INET;
        client_addr.sin_addr.s_addr = inet_addr(client->pClientIP);

        llcf_curr = client->flags;
        while(llcf_curr != NULL)
        {
                if(iPort == 0 || llcf_curr->iPort == iPort)
                {
			if ((client_sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
		        {
                		Log("CtfLib_SvrEnumClients2() socket() failed");
		                return;
        		}

                        client_addr.sin_port = htons(llcf_curr->iPort);
#ifdef DEBUG
			DebugLog("!!! Trying %s(%s:%d) !!!", client->pTeamName, client->pClientIP, llcf_curr->iPort);
#endif
                        if(connect_time(client_sock, (struct sockaddr *)&client_addr, sizeof(client_addr), 5) < 0)
                        {
				Log("!!! %s(%s:%d) DOWN!!!", client->pTeamName, client->pClientIP, llcf_curr->iPort);
                                llcf_curr = llcf_curr->next;
				CtfLib_SetLastError(CTF_ERROR_FAIL);
                                continue;
                        }

			//Always wash your hands before you devour
			memset(bRecvBuffer, 0, sizeof(bRecvBuffer));
// iOp Start
			if(iOp == CTF_CHECKFLAG || iOp == CTF_GETFLAG)
			{
	                        CtfLib_SendPacket(client_sock, PACKET_GETFLAG, NULL, 0, client->d, client->n);
        	                int iRet = CtfLib_SvrRecvPacket(client_sock, bRecvBuffer, client->d, client->n);
                	        if(iRet != -1)
	                        {
        	                        PPACKET_BLOCK pb = (PPACKET_BLOCK)bRecvBuffer;
					//BO check	
					if(pb->dwPACKET_SIZE+PACKET_HEADER_SIZE > BIG_BUFFER_SIZE)
					{
						Log("CtfLib_SvrEnumClients2() BO Check Failed (FLAG) %s(%s:%d)", client->pTeamName, client->pClientIP, llcf_curr->iPort);
						CtfLib_SetLastError(CTF_ERROR_FAIL);
						continue;
					}
                	                //Null terminate it
	                                bRecvBuffer[PACKET_HEADER_SIZE+pb->dwPACKET_SIZE] = 0;
        	                        if(pb->dwPACKET_ID == PACKET_GETFLAG)
                	                {
						if(iOp == CTF_GETFLAG)
						{
							Log("%s(%s:%d) Flag : %s", client->pTeamName, client->pClientIP, llcf_curr->iPort, &pb->bpMsg);
							CtfLib_SetLastError(CTF_ERROR_SUCCESS);
						}
						else
						{
		                                        if(strcmp(&pb->bpMsg, llcf_curr->pFlag) != 0)
							{
        		                                        Log("!!! %s(%s:%d) PWNED (%s)!!!", client->pTeamName, client->pClientIP, llcf_curr->iPort, &pb->bpMsg);
								CtfLib_SetLastError(CTF_ERROR_FAIL);
							}
							else
							{
								CtfLib_SetLastError(CTF_ERROR_SUCCESS);
							}
						}
                	                }
	                                else
					{
						Log("!!! %s(%s:%d) Wrong Reply!!!", client->pTeamName, client->pClientIP, llcf_curr->iPort);
						CtfLib_SetLastError(CTF_ERROR_FAIL);
					}
        	                }
				else
				{
					Log("!!! %s(%s:%d) Crypto Problem!!!", client->pTeamName, client->pClientIP, llcf_curr->iPort);
					CtfLib_SetLastError(CTF_ERROR_FAIL);
				}
			}
			else if(iOp == CTF_GETHASHMEM || iOp == CTF_CHECKHASHMEM ||\
				iOp == CTF_GETHASHFILE || iOp ==CTF_CHECKHASHFILE)
			{
				if(iOp == CTF_GETHASHMEM || iOp == CTF_CHECKHASHMEM)
					CtfLib_SendPacket(client_sock, PACKET_CHECKSUM_MEM, NULL, 0, client->d, client->n);
				else
					CtfLib_SendPacket(client_sock, PACKET_CHECKSUM_FILE, NULL, 0, client->d, client->n);

                                int iRet = CtfLib_SvrRecvPacket(client_sock, bRecvBuffer, client->d, client->n);
				if(iRet != -1)
                                {
                                        PPACKET_BLOCK pb = (PPACKET_BLOCK)bRecvBuffer;
					//BO check
					if(pb->dwPACKET_SIZE+PACKET_HEADER_SIZE > BIG_BUFFER_SIZE)
					{
						Log("CtfLib_SvrEnumClients2() BO Check Failed (HASH) %s(%s:%d)", client->pTeamName, client->pClientIP, llcf_curr->iPort);
						CtfLib_SetLastError(CTF_ERROR_FAIL);
						continue;
					}

                                        //Null terminate it
                                        bRecvBuffer[PACKET_HEADER_SIZE+pb->dwPACKET_SIZE] = 0;
                                        if(pb->dwPACKET_ID == PACKET_CHECKSUM_MEM ||\
					   pb->dwPACKET_ID == PACKET_CHECKSUM_FILE)
                                        {
                                                if(iOp == CTF_GETHASHMEM)
                                                        Log("%s(%s:%d) Mem Hash : %s", client->pTeamName, client->pClientIP, llcf_curr->iPort, &pb->bpMsg);
						else if(iOp ==  CTF_GETHASHFILE)
							Log("%s(%s:%d) File Hash : %s", client->pTeamName, client->pClientIP, llcf_curr->iPort, &pb->bpMsg);
                                                else
                                                {
							llcs_curr = llcs_head;
							int iFound = 0;
							while(llcs_curr != NULL)
							{
								if(llcs_curr->iPort == llcf_curr->iPort)
								{
									iFound = 1;
									if(iOp == CTF_CHECKHASHMEM)
									{
										if(strcmp(llcs_curr->pMemCsum,&pb->bpMsg) != 0)
										{
		                                                                	Log("!!! %s(%s:%d) Invalid Mem Hash (%s)!!!", client->pTeamName, client->pClientIP, llcf_curr->iPort, &pb->bpMsg);
											CtfLib_SetLastError(CTF_ERROR_FAIL);
										}
										else
										{
											CtfLib_SetLastError(CTF_ERROR_SUCCESS);
										}
									}
									else
									{
										if(strcmp(llcs_curr->pFileCsum,&pb->bpMsg) != 0)
										{
											Log("!!! %s(%s:%d) Invalid File Hash (%s)!!!", client->pTeamName, client->pClientIP, llcf_curr->iPort, &pb->bpMsg);
											CtfLib_SetLastError(CTF_ERROR_FAIL);
										}
										else
										{
											CtfLib_SetLastError(CTF_ERROR_SUCCESS);
										}
									}
								}
								llcs_curr = llcs_curr->next;
							}
							if(iFound = 0)
							{
								Log("!!! No entry found for port %d!!!", llcf_curr->iPort);
								CtfLib_SetLastError(CTF_ERROR_FAIL);
							}
                                                }
                                        }
                                        else
					{
						Log("!!! %s(%s:%d) Wrong Reply!!!", client->pTeamName, client->pClientIP, llcf_curr->iPort);
						CtfLib_SetLastError(CTF_ERROR_FAIL);
					}
                                }
				else
				{
					Log("!!! %s(%s:%d) Crypto Problem!!!", client->pTeamName, client->pClientIP, llcf_curr->iPort);
					CtfLib_SetLastError(CTF_ERROR_FAIL);
				}
			}
			else if(iOp == CTF_SETFLAG)
			{
				//Dont call SetLastError as SendPacket already does it
				CtfLib_SendPacket(client_sock, PACKET_SETFLAG, llcf_curr->pFlag, strlen(llcf_curr->pFlag), client->d, client->n);
                                Log("Flag %s set for %s(%s:%d)", llcf_curr->pFlag, client->pTeamName, client->pClientIP, llcf_curr->iPort);
			}
//iOp End
                        close(client_sock);
                }
                llcf_curr = llcf_curr->next;
	}
}

void CtfLib_SvrEnumClients(char *cIP, int iPort, int iOp)
{
        ll_client_keys *llck_curr;

        if(CtfLib_CheckInit() != 0) return;

        llck_curr = llck_head;
        while(llck_curr != NULL)
        {
                if(cIP == NULL || strcmp(cIP, "0") == 0)
                        CtfLib_SvrEnumClients2(llck_curr, iPort, iOp);
                else if(strcmp(llck_curr->pClientIP, cIP) == 0)
                        CtfLib_SvrEnumClients2(llck_curr, iPort, iOp);
                llck_curr = llck_curr->next;
        }
}

//This is risky, we gotta assume it was called from CtfLib_Init()
int CtfLib_SvrParseConfigs()
{
	const char seperators[] = "=,\r\n";
	char *token;
	ll_client_keys *llck_curr;
	ll_client_flags *llcf_curr;
	ll_client_csum *llcs_curr;

	char cLineBuffer[BIG_BUFFER_SIZE];
	char *pTeamName=NULL;
	char *pIPAddr=NULL;
	char *pFlag=NULL;
	char *pMemCS=NULL;
	char *pFileCS=NULL;
	big n,d;

	int iFileSize;
	FILE *fp, *fp2;
	char *pMem;

	if(access("ctfsvr_configs/general.conf", 0) != 0)
	{
		Log("access() failed on ctfsvr_configs/general.conf");
		return CtfLib_SetLastError(CTF_ERROR_FAIL);
	}

	if(access("ctfsvr_configs/keys/ctf_keys.txt", 0) != 0)
        {
                Log("access() failed on ctfsvr_configs/keys/ctf_keys.txt");
                return CtfLib_SetLastError(CTF_ERROR_FAIL);
        }

	if(access("ctfsvr_configs/ctf_svrflags.txt", 0) != 0)
        {
                Log("access() failed on ctfsvr_configs/ctf_svrflags.txt");
                return CtfLib_SetLastError(CTF_ERROR_FAIL);
        }

	if(access("ctfsvr_configs/ctf_checksum.txt", 0) != 0)
        {
                Log("access() failed on ctfsvr_configs/ctf_checksum.txt");
                return CtfLib_SetLastError(CTF_ERROR_FAIL);
        }

	//Read general.conf
#ifdef DEBUG
	DebugLog("Parsing general.conf");
#endif
	fp = fopen("ctfsvr_configs/general.conf", "rb");
	if(fp == NULL)
	{
		Log("fopen() failed on ctfsvr_configs/general.conf");
		return CtfLib_SetLastError(CTF_ERROR_FAIL);
	}
	while(1)
	{
		memset(cLineBuffer, 0, sizeof(cLineBuffer));
		if(fgets(cLineBuffer, sizeof(cLineBuffer), fp) == NULL) break;
		if(cLineBuffer[0] == ';' || cLineBuffer[0] == '#') continue;	//Skip comments
		token = strtok(cLineBuffer, seperators);

		if(memcmp(token, "daemon_count", 12) == 0)
		{
			token = strtok(NULL, seperators);
			if(token == NULL)
			{
				Log("Invalid value in daemon_count");
				goto READ_ERROR;
			}
			iDaemonCounter = atoi(token);
		}
		break;
	}
	fclose(fp);

	//Read ctf_keys.txt
#ifdef DEBUG
	DebugLog("Parsing ctf_keys.txt");
#endif
	fp = fopen("ctfsvr_configs/keys/ctf_keys.txt", "rb");
        if(fp == NULL)
        {
		Log("fopen() failed on ctfsvr_configs/keys/ctf_keys.txt");
                return CtfLib_SetLastError(CTF_ERROR_FAIL);
        }
        while(1)
        {
                memset(cLineBuffer, 0, sizeof(cLineBuffer));
                if(fgets(cLineBuffer, sizeof(cLineBuffer), fp) == NULL) break;
		if(cLineBuffer[0] == ';' || cLineBuffer[0] == '#') continue;	//Skip comments
		if(char_count(cLineBuffer, ',') != 3)
		{
			Log("Invalid number of parameters in line");
			Log("Line : %s", cLineBuffer);
			goto READ_ERROR;
		}

		//Extract, verify then copy

		//First field - team name
		token = strtok(cLineBuffer, seperators);
		pTeamName = malloc(strlen(token)+10);
                if(pTeamName == NULL)
                {
                        Log("malloc(pTeamName) failed");
                        goto READ_ERROR;
                }

		strcpy(pTeamName, token);
#ifdef DEBUG
		DebugLog("Reading information for team %s", token);
#endif
		//Second field - IP Address
		token = strtok(NULL, seperators);
		pIPAddr = malloc(strlen(token)+10);
		if(pIPAddr == NULL)
		{
			Log("malloc(pIPAddr) failed");
			goto READ_ERROR;
		}
		strcpy(pIPAddr, token);

		//Third field - public key
		token = strtok(NULL, seperators);
#ifdef DEBUG
		DebugLog("Reading %s", token);
#endif

		if(access(token, 0) != 0)
		{
                	Log("access() failed on %s", token);
			goto READ_ERROR;
		}

		//public key
		fp2 = fopen(token, "rb");
		if(fp2 == NULL)
		{
			Log("fopen() failed on %s", token);
			goto READ_ERROR;
		}

		//Get file size
                fseek(fp2, 0, SEEK_END);
                iFileSize = ftell(fp2);
                fseek(fp2, 0, SEEK_SET);

		//Alloc mem
                pMem = malloc(iFileSize+10);
                if(pMem == NULL)
                {
                        fclose(fp2);
                        Log("malloc(pMem) failed");
			goto READ_ERROR;
                }

                //Read into mem
                fread(pMem, iFileSize, 1, fp2);

                //Read into variable
                n=mirvar(0);
                cinstr(n, pMem);

		free(pMem);
		fclose(fp2);

		//Fourth field - private key
                token = strtok(NULL, seperators);
#ifdef DEBUG
		DebugLog("Reading %s", token);
#endif

		if(access(token, 0) != 0)
                {
			mirkill(n);
                        Log("access() failed on %s", token);
			goto READ_ERROR; 
                }

                fp2 = fopen(token, "rb");
                if(fp2 == NULL)
                {
			mirkill(n);
                        Log("fopen() failed on %s", token);
			goto READ_ERROR;
                }

		//Get file size
                fseek(fp2, 0, SEEK_END);
                iFileSize = ftell(fp2);
                fseek(fp2, 0, SEEK_SET);

                //Alloc mem
                pMem = malloc(iFileSize+10);
                if(pMem == NULL)
                {
                        fclose(fp2);
                        Log("malloc(pMem) failed");
                        goto READ_ERROR;
                }

                //Read into mem
                fread(pMem, iFileSize, 1, fp2);

                //Read into variable
                d=mirvar(0);
                cinstr(d, pMem);

                free(pMem);
                fclose(fp2);

		//Prepare linked list
		llck_curr = (ll_client_keys *)malloc(sizeof(ll_client_keys));
		if(llck_curr == NULL)
		{
			free(pIPAddr);
			free(pTeamName);
			mirkill(n);
	                mirkill(d);
        	        pIPAddr = NULL;
                	pTeamName = NULL;
			Log("malloc(llck_curr) failed");
			goto READ_ERROR;
		}

		llck_curr->flags = NULL;
		llck_curr->next = llck_head;
		llck_head = llck_curr;

		//Copy data over
		llck_curr->pTeamName = pTeamName;
		llck_curr->pClientIP = pIPAddr;
		llck_curr->flags = NULL;
		llck_curr->d = mirvar(0);
		copy(d, llck_curr->d);
		llck_curr->n = mirvar(0);
		copy(n, llck_curr->n);

		mirkill(n);
		mirkill(d);
		pIPAddr = NULL;
		pTeamName = NULL;
	}
	fclose(fp);

        //Read ctf_svrflags.txt
#ifdef DEBUG
	DebugLog("Parsing ctf_svrflags.txt");
#endif
        fp = fopen("ctfsvr_configs/ctf_svrflags.txt", "rb");
        if(fp == NULL)
        {
		Log("fopen() failed on ctfsvr_configs/ctf_svrflags.txt");
                return CtfLib_SetLastError(CTF_ERROR_FAIL);
        }

	while(1)
        {
                memset(cLineBuffer, 0, sizeof(cLineBuffer));
                if(fgets(cLineBuffer, sizeof(cLineBuffer), fp) == NULL) break;
                if(cLineBuffer[0] == ';' || cLineBuffer[0] == '#') continue;    //Skip comments
		if(char_count(cLineBuffer, ',') != 2)
                {
                        Log("Invalid number of parameters in line");
                        Log("Line : %s", cLineBuffer);
                        goto READ_ERROR;
                }

                //Extract, verify then copy

		//Verify this ip exists
		int iFound = 0;
		token = strtok(cLineBuffer, seperators);
		llck_curr = llck_head;
		while(llck_curr != NULL)
		{
			//We found a match
			if(strcmp(llck_curr->pClientIP,token) == 0)
			{
				//Get Port
				token = strtok(NULL, seperators);
				int iPort = atoi(token);

				//Get Flag
				token = strtok(NULL, seperators);
				pFlag = malloc(strlen(token)+10);
		                if(pFlag == NULL)
                		{
                		        Log("malloc(pFlag) failed");
		                        goto READ_ERROR;
		                }
				strcpy(pFlag, token);
				
				//Prepare linked list
                		llcf_curr = (ll_client_flags *)malloc(sizeof(ll_client_flags));
				if(llcf_curr == NULL)
				{
					free(pFlag);
					Log("malloc(llcf_curr) failed");
					goto READ_ERROR;
				}

				llcf_curr->next = (ll_client_flags *)llck_curr->flags;
				llck_curr->flags = llcf_curr;

				//Copy data over
				llcf_curr->iPort=iPort;
				llcf_curr->pFlag=pFlag;

				iFound = 1;
			}
			llck_curr = llck_curr->next;
		}
		if(iFound == 0)
		{
			Log("Unable to find an entry for %s", token);
			goto READ_ERROR;
		}
        }
        fclose(fp);

	//Read ctf_checksum.txt
#ifdef DEBUG
        DebugLog("Parsing ctf_checksum.txt");
#endif
        fp = fopen("ctfsvr_configs/ctf_checksum.txt", "rb");
        if(fp == NULL)
        {
                Log("fopen() failed on ctfsvr_configs/ctf_svrflags.txt");
                return CtfLib_SetLastError(CTF_ERROR_FAIL);
        }

	while(1)
        {
                memset(cLineBuffer, 0, sizeof(cLineBuffer));
                if(fgets(cLineBuffer, sizeof(cLineBuffer), fp) == NULL) break;
                if(cLineBuffer[0] == ';' || cLineBuffer[0] == '#') continue;    //Skip comments
                if(char_count(cLineBuffer, ',') != 2)
                {
                        Log("Invalid number of parameters in line");
                        Log("Line : %s", cLineBuffer);
                        goto READ_ERROR;
                }

                //Extract, verify then copy
                token = strtok(cLineBuffer, seperators);
		int iPort = atoi(token);

		//Check if port exists
		llcs_curr = llcs_head;
		while(llcs_curr != NULL)
		{
			if(llcs_curr->iPort == iPort)
			{
				Log("Entry already exists for port %d", iPort);
				goto READ_ERROR;
			}
			llcs_curr = llcs_curr->next;
		}

		pMemCS = pFileCS = NULL;
		//Get Memory Checksum
                token = strtok(NULL, seperators);
                pMemCS = malloc(strlen(token)+10);
                if(pMemCS == NULL)
                {
                	Log("malloc(pMemCS) failed");
                        goto READ_ERROR;
                }
                strcpy(pMemCS, token);

		//Get File Checksum
                token = strtok(NULL, seperators);
                pFileCS = malloc(strlen(token)+10);
                if(pFileCS == NULL)
                {
                        Log("malloc(pFileCS) failed");
                        goto READ_ERROR;
                }
                strcpy(pFileCS, token);

		//Prepare linked list
                llcs_curr = (ll_client_csum *)malloc(sizeof(ll_client_csum));
                if(llcs_curr == NULL)
                {
                	Log("malloc(llcs_curr) failed");
                        goto READ_ERROR;
		}

		llcs_curr->next = llcs_head;
                llcs_head = llcs_curr;

                //Copy data over
                llcs_curr->iPort=iPort;
                llcs_curr->pMemCsum=pMemCS;
		llcs_curr->pFileCsum=pFileCS;
	}

	return CtfLib_SetLastError(CTF_ERROR_SUCCESS);
READ_ERROR:
	if(pIPAddr != NULL) free(pIPAddr);
	if(pTeamName != NULL) free(pTeamName);
	if(pMemCS != NULL) free(pMemCS);
	if(pFileCS != NULL) free(pFileCS);
	fclose(fp);
	return CtfLib_SetLastError(CTF_ERROR_FAIL);
}

int CtfLib_SvrRecvPacket(int iSock, BYTE *pOutData, big bD, big bN)
{
	struct timeval tv;
	int timeouts = 0;
	tv.tv_sec = 10;
	tv.tv_usec = 0;

	BYTE bEncPacketBuffer[BIG_BUFFER_SIZE];
	BYTE bMessageBuffer[BIG_BUFFER_SIZE];

        if(CtfLib_CheckInit() != 0) return CtfLib_SetLastError(CTF_ERROR_FAIL);

	memset(bMessageBuffer, 0, sizeof(bMessageBuffer));

	if (setsockopt(iSock, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv,  sizeof tv))
	{
#ifdef DEBUG
                DebugLog("CtfLib_SvrRecvPacket() setsockopt() failed");
#endif
		return CtfLib_SetLastError(CTF_ERROR_FAIL);
	}

	int iPacketSize = read(iSock, bEncPacketBuffer, sizeof(bEncPacketBuffer));
	if(iPacketSize <= 0)
	{
#ifdef DEBUG
		DebugLog("CtfLib_SvrRecvPacket() read() failed");
#endif
		return CtfLib_SetLastError(CTF_ERROR_FAIL);
	}

	//Decrypt packet
        CtfLib_TransformBlock(bEncPacketBuffer, bMessageBuffer, iPacketSize, bD, bN);

	PPACKET_BLOCK pb = (PPACKET_BLOCK)bMessageBuffer;
        //Check for CTF signature
        if(pb->dwCTF_ID != CTF_ID)
        {
                Log("CtfLib_SvrRecvPacket() Failed : pb->dwCTF_ID != CTF_ID");
                return CtfLib_SetLastError(CTF_ERROR_FAIL);
        }

	DWORD iMsgSize = PACKET_HEADER_SIZE + pb->dwPACKET_SIZE;
	//BO check	
	if(iMsgSize > BIG_BUFFER_SIZE)
	{
		Log("CtfLib_SvrRecvPacket() BO Check Failed");
		return CtfLib_SetLastError(CTF_ERROR_FAIL);
	}
	memcpy(pOutData, bMessageBuffer, iMsgSize);
	return iMsgSize;
}

void CtfLib_SvrPropogateFlags(int iTries)
{
	ll_client_keys *llck_curr;
	if(CtfLib_CheckInit() != 0) return;

	Log("Propogating flag %d times to each team", iTries);

	int i;

	for(i=1; i<=iTries; i++)
	{
		llck_curr = llck_head;
        	while(llck_curr != NULL)
	        {
			Log("Team : %s(%s) Count : %d of %d", llck_curr->pTeamName, llck_curr->pClientIP, i, iTries);
			CtfLib_SvrEnumClients(llck_curr->pClientIP, 0, CTF_SETFLAG);
                	llck_curr = llck_curr->next;
		}
		sleep(5);
	}
}

void CtfLib_SvrGenNewFlags(char *cFlagPath)
{
	ll_client_keys *llck_curr;
        ll_client_flags *llcf_curr;
	char cNewFlag[BIG_BUFFER_SIZE];
	big x;

	if(CtfLib_CheckInit() != 0) return;

	//Fail if cFlagPath exists
	if(access("ctfsvr_configs/ctf_svrflags.txt", 0) != 0)
        {
                Log("access() failed on %s, please make sure file does NOT exist...", cFlagPath);
		CtfLib_SetLastError(CTF_ERROR_FAIL);
                return;
        }

	//Open file for writing
        FILE *fp = fopen(cFlagPath, "w");
        if(fp == NULL)
        {
                Log("fopen() failed on %s", cFlagPath);
		CtfLib_SetLastError(CTF_ERROR_FAIL);
                return;
        }

	x=mirvar(0);
	//Enumerate through each team
	llck_curr = llck_head;
	while(llck_curr != NULL)
	{
		//Enumerate through each flag
		llcf_curr = llck_curr->flags;
                while(llcf_curr != NULL)
		{
			//Generate new flag
			bigdig(60,10,x);
			cotstr(x, cNewFlag);
			//Write to file
			fprintf(fp, "%s,%d,%s\n", llck_curr->pClientIP, llcf_curr->iPort, cNewFlag);
			llcf_curr = llcf_curr->next;
		}
		llck_curr = llck_curr->next;
	}
	mirkill(x);

	//Cleanup
	fclose(fp);
	Log("New flags generated");
}

int CtfLib_SetLastError(int err)
{
	iLastError = err;
	return err;
}

int CtfLib_GetLastError()
{
	return iLastError;
}

#ifdef DEBUG
//Test each function
void CtfLib_SvrTest()
{
	ll_client_keys *llck_curr;
	ll_client_flags *llcf_curr;
	BYTE bRecvBuffer[BIG_BUFFER_SIZE];
	int iRet;

	if(CtfLib_CheckInit() != 0) return;

	//Setup socket stuff
        int client_sock, client_len;
        struct sockaddr_in client_addr;

	if ((client_sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
	{
		Log("CtfLib_SvrTest() socket() failed");
		return;
        }


	memset(&client_addr, 0, sizeof(client_addr));
	client_addr.sin_family = AF_INET;

	//Log("Generating random event");

	//Enumerate through every client
	llck_curr = llck_head;

	while(llck_curr != NULL)
        {
		//Log("CtfLib_SvrTest() Testing %s", llck_curr->pClientIP);
		//Enumerate through every flag we have
		llcf_curr = (ll_client_flags *)llck_curr->flags;
		while(llcf_curr != NULL)
		{
			//Log("Attempting to connect to %s:%d", llck_curr->pClientIP, llcf_curr->iPort);
			client_addr.sin_addr.s_addr = inet_addr(llck_curr->pClientIP);
			client_addr.sin_port = htons(llcf_curr->iPort);
			if(connect(client_sock, (struct sockaddr *)&client_addr, sizeof(client_addr)) < 0)
			{
				//Log("CtfLib_SvrTest() connect() failed to %s:%d", llck_curr->pClientIP, llcf_curr->iPort);
			}
			else
			{
				int action = (rand() % 10) + 1;
				switch(action)
			        {
			                case PACKET_CHECKSUM_FILE:
						//Log("Sending PACKET_CHECKSUM_FILE");
						CtfLib_SendPacket(client_sock, PACKET_CHECKSUM_FILE, NULL, 0, llck_curr->d, llck_curr->n);
			                        break;
			                case PACKET_CHECKSUM_MEM:
						//Log("Sending PACKET_CHECKSUM_MEM");
						CtfLib_SendPacket(client_sock, PACKET_CHECKSUM_MEM, NULL, 0, llck_curr->d, llck_curr->n);
			                        break;
			                case PACKET_GETFLAG:
						//Log("Sending PACKET_GETFLAG");
						CtfLib_SendPacket(client_sock, PACKET_GETFLAG, NULL, 0, llck_curr->d, llck_curr->n);
						iRet = CtfLib_SvrRecvPacket(client_sock, bRecvBuffer, llck_curr->d, llck_curr->n);
						if(iRet != -1)
						{
							PPACKET_BLOCK pb = (PPACKET_BLOCK)bRecvBuffer;
							if(pb->dwPACKET_ID == PACKET_GETFLAG)
							{
								//CtfLib_HexDump(bRecvBuffer, 256);
								bRecvBuffer[PACKET_HEADER_SIZE+pb->dwPACKET_SIZE] = 0;
								Log("Flag : %s", &pb->bpMsg);
							}
							else
							{
								Log("dwPACKET_ID mismatch");
							}
						}
						else 
							Log("PACKET_GETFLAG Failed");
			                        break;
			                case PACKET_SETFLAG:
						//Log("Sending PACKET_SETFLAG");
						CtfLib_SendPacket(client_sock, PACKET_SETFLAG, "New Flag 123456", strlen("New Flag 123456"), llck_curr->d, llck_curr->n);
			                        break;
					case PACKET_MSGTEST:
						//Log("Sending PACKET_MSGTEST");
						CtfLib_SendPacket(client_sock, PACKET_MSGTEST, "Hello World", strlen("Hello World"), llck_curr->d, llck_curr->n);
						break;
			                default:        //Send random junk
						{
							char cRandBuffer[BIG_BUFFER_SIZE];
							int iRandPacket = rand() % 240;
							int i;
							for(i=0; i<iRandPacket; i++)
								cRandBuffer[i] = rand() % 0xff;

						//Log("Sending Random Junk");
						
						CtfLib_SendPacket(client_sock, iRandPacket, cRandBuffer, iRandPacket, llck_curr->d, llck_curr->n);
						}
                        			break;
			        }
				close(client_sock);
			}
			llcf_curr = llcf_curr->next;
		}
                llck_curr = llck_curr->next;
	}
}
#endif
//######################################################
// SERVER CODE END
//######################################################
#endif
