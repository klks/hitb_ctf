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
#include "miracl.h"

//Comment below to disable debugging
//#define DEBUG			1

//Uncomment this so library compiles without server code
//Dont fuck up and compile the daemons with this option enabled
//#define CTF_SERVER			1

#ifndef CTF_SERVER
	#define CTF_CLIENT		1
#endif

#define BIG_BUFFER_SIZE		20000
#define BYTE unsigned char
#define DWORD unsigned long
#define FALSE			0
#define TRUE			1
#define MIN(a,b) ((a)>(b)?(b):(a))
//Taken from http://cpansearch.perl.org/src/NISHANT/Twofish-1.0/Twofish/Twofish.h
#define	ROL(x,n) (((x) << ((n) & 0x1F)) | ((x) >> (32-((n) & 0x1F))))
#define	ROR(x,n) (((x) >> ((n) & 0x1F)) | ((x) << (32-((n) & 0x1F))))

//Key loading stuff
#define CTF_KEY_SIZE		2048	//Change this accordingly

//Error codes
#define CTF_ERROR_FAIL		-1
#define CTF_ERROR_SUCCESS	0

/*Nice but not implemented
#define CTF_ERROR_CRYPTO	1
#define CTF_ERROR_CONNECTION	2
#define CTF_ERROR_RECV		3
#define CTF_ERROR_SEND		4
*/

//Used by CtfLib_SvrEnumClients(...)
#define CTF_GETFLAG		1
#define CTF_SETFLAG		2
#define CTF_CHECKFLAG		3
#define CTF_GETHASHFILE		4
#define CTF_GETHASHMEM		5
#define CTF_CHECKHASHFILE	6
#define CTF_CHECKHASHMEM	7

//Packet related stuff
#define PACKET_TIMEOUT		5*60
#define CTF_ID			0x097FFCC7	//When stored on file will show CTFCTF09
#define PACKET_CHECKSUM_FILE	1	//Get binary checksum from file
#define PACKET_CHECKSUM_MEM	2	//Get binary checksum from memory
#define PACKET_GETFLAG		3	//Server is asking for flag
#define PACKET_SETFLAG		4	//Server is setting flag

#ifdef DEBUG
#define PACKET_MSGTEST		5	//Used for debugging
#endif

#define PACKET_HEADER_SIZE	16
//Packet structure breakdown
//[CTF_ID.4][TIMESTAMP.4][PACKET_ID.4][PACKET_SIZE.4][PACKET_MESSAGE.n]
struct PACKET_BLOCK
{
	DWORD dwCTF_ID;
	DWORD dwTimeStamp;
	DWORD dwPACKET_ID;
	DWORD dwPACKET_SIZE;
	BYTE bpMsg;
};
typedef struct PACKET_BLOCK *PPACKET_BLOCK;

#ifdef CTF_SERVER
///Server Stuff
//Linked list holding the client checksums
struct LL_CLIENT_CHECKSUM
{
	int iPort;
        char *pMemCsum;
	char *pFileCsum;
        struct LL_CLIENT_CHECKSUM *next;
};
typedef struct LL_CLIENT_CHECKSUM ll_client_csum;

//Linked list holding the client flags
struct LL_CLIENT_FLAGS
{
	int iPort;
	char *pFlag;
	struct LL_CLIENT_FLAGS *next;
};
typedef struct LL_CLIENT_FLAGS ll_client_flags;

//Linked list holding the client keys
struct LL_CLIENT_KEYS
{
        char *pTeamName;
	char *pClientIP;
        big d;
	big n;
	struct LL_CLIENT_FLAGS *flags;
        struct LL_CLIENT_KEYS *next;
};
typedef struct LL_CLIENT_KEYS ll_client_keys;

#else
//Client stuff
char cFlag[1000];			//Variable that holds the flag
char cFlagPath[BIG_BUFFER_SIZE];	//Varaible that holds where the file path to flag
#endif

//Functions have this format CtfLib_<function_name>
#ifdef CTF_CLIENT
void CtfLib_MakeEvil();
void CtfLib_MakeEvil2();
void CtfLib_GetModuleFileName();
void CtfLib_GetModuleHandle();
int CtfLib_Init(char *cFlag);               //Init, first thing to do in the morning
void CtfLib_MakeStandAloneSocket(int iPort);
void* CtfLib_StandAloneSocket(void *ptr);
int CtfLib_ClientProcess(int iSock, BYTE *pPacketData, DWORD dwPacketSize);
int CtfLib_LoadPublicKey(char *cKeyFile);
int CtfLib_LoadFlag();
void CtfLib_GetFlag(int iSock);
char* CtfLib_GetInsecureFlag();		//Dont use this if possible :D
void CtfLib_SetFlag(BYTE *cFlag, int iFlagSize);
void CtfLib_ClientInternalCheck(int iSock, int iType);
#else
int CtfLib_Init();               //Init, first thing to do in the morning
void CtfLib_SvrCleanup();
void CtfLib_SvrTest();
void CtfLib_SvrEnumClients2(ll_client_keys *client, int iPort, int iOp);
void CtfLib_SvrEnumClients(char *cIP, int iPort, int iOp);
int CtfLib_SvrParseConfigs();
int CtfLib_SvrRecvPacket(int iSock, BYTE *pOutData, big bD, big bN);
void CtfLib_SvrPropogateFlags(int iTries);
void CtfLib_SvrGenNewFlags(char *cFlagPath);
int CtfLib_SetLastError(int err);
int CtfLib_GetLastError();
#endif

//Debugging stuff comes here
#ifdef DEBUG
void CtfLib_HexDump(BYTE *bp, DWORD dwSize);
void DebugLog(char *fmt, ...);
void CtfLib_TestCrypto();
#endif
