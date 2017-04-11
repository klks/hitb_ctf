// Client.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "../Release/xmlrpcpp.lib")
#include "../xmlrpcpp/XmlRpc.h"

char cEncBuff[1024];

//#define DEBUG	1
#define CTF_SVR	"127.0.0.1"

using namespace XmlRpc;

void decryptMessage(char *cEncBuff, char *cCleanKey)
{
	for(int i=0, j=0; i<(int)strlen(cEncBuff); i++,j++)
	{
		if(j == 16) j = 0;
		cEncBuff[i] ^= cCleanKey[j];
	}
}

void makeCleanKey(char* encKey, char* cleanKey)
{
	char *p = cleanKey;
	char c;
	for(int i=0; i<23; i++)
	{
		c = encKey[i];
		if(isdigit(c))
		{
			*p = c;
			p++;
		}
	}
}

bool verifyLuhn(char *cleanKey)
{
   int n, i, alternate, sum;

    n = strlen(cleanKey);

    if (n < 13|| n > 19)
        return false;

    for (alternate = 0, sum = 0, i = n - 1; i > -1; --i)
	{
        n = cleanKey[i] - '0';
        if (alternate)
		{
			n *= 2;
			if (n > 9)
				n = (n % 10) + 1;
		}
		alternate = !alternate;
        sum += n;
    }

    if(sum % 10 != 0) return false;
	return true;
}

int _tmain(int argc, _TCHAR* argv[])
{
	WSADATA HWSAdata;
	SOCKET s;
	SOCKADDR_IN lSockAddr;
	int lConnect;
	int iLen;
	char cEncKey[50];
	char cCleanKey[50];

	if (WSAStartup(MAKEWORD(2,2), &HWSAdata) != 0) { exit(1); }
	if ((s=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP))==INVALID_SOCKET){ exit(1); }

	memset(&lSockAddr,0, sizeof(lSockAddr));
    lSockAddr.sin_family = AF_INET;
    lSockAddr.sin_port = htons(60701);
    lSockAddr.sin_addr.s_addr = inet_addr(CTF_SVR);
	if((lConnect = connect(s,(SOCKADDR *)&lSockAddr,sizeof(SOCKADDR_IN))) != 0){ exit(1);}

	memset(cEncBuff, 0, sizeof(cEncBuff));
	memset(cEncKey, 0, sizeof(cEncKey));
	memset(cCleanKey, 0, sizeof(cCleanKey));
	iLen = recv(s, cEncBuff, sizeof(cEncBuff), 0);
	closesocket(s);

#ifdef DEBUG
	std::cout << "Encrypted message size : " << iLen << std::endl;
#endif
	
	XmlRpcClient c(CTF_SVR, 60702,"/KLKSRPC");
	XmlRpcValue result;
	XmlRpcValue noArg;
	c.execute("getKey", noArg, result);

#ifdef DEBUG
	std::cout << "Key from server : " << result << std::endl;
#endif

	if(result.size() != 23) { exit(1); }

	
	memcpy(cEncKey, result.getString().data(), 23);
	makeCleanKey(cEncKey, cCleanKey);
	if(!verifyLuhn(cCleanKey))
	{ 
#ifdef DEBUG
		std::cout << "Key : invalid" << std::endl;
#endif
		exit(1);
	}
#ifdef DEBUG
		std::cout << "Key : Valid" << std::endl;
#endif

#ifdef DEBUG
	std::cout << "cEncKey = " << cEncKey << " - " << strlen(cEncKey) << std::endl;
	std::cout << "cCleanKey = " << cCleanKey << " - " << strlen(cCleanKey) << std::endl;
#endif

	decryptMessage(cEncBuff, cCleanKey);
	std::cout << cEncBuff << std::endl;

	return 0;
}