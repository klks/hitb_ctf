// Client.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <Windows.h>

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "../Release/xmlrpcpp.lib")
#include "../xmlrpcpp/XmlRpc.h"


#define CTF_SVR	"127.0.0.1"
//#define CTF_DEBUG 1

using namespace XmlRpc;

char bEncryptedBuffer[64] = {0};
static const std::string base64_chars = 
             "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
             "abcdefghijklmnopqrstuvwxyz"
             "0123456789+/";

static inline bool is_base64(unsigned char c) {
  return (isalnum(c) || (c == '+') || (c == '/'));
}

std::string base64_decode(std::string const& encoded_string) {
  int in_len = encoded_string.size();
  int i = 0;
  int j = 0;
  int in_ = 0;
  unsigned char char_array_4[4], char_array_3[3];
  std::string ret;

  while (in_len-- && ( encoded_string[in_] != '=') && is_base64(encoded_string[in_])) {
    char_array_4[i++] = encoded_string[in_]; in_++;
    if (i ==4) {
      for (i = 0; i <4; i++)
        char_array_4[i] = base64_chars.find(char_array_4[i]);

      char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
      char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
      char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

      for (i = 0; (i < 3); i++)
        ret += char_array_3[i];
      i = 0;
    }
  }

  if (i) {
    for (j = i; j <4; j++)
      char_array_4[j] = 0;

    for (j = 0; j <4; j++)
      char_array_4[j] = base64_chars.find(char_array_4[j]);

    char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
    char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
    char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

    for (j = 0; (j < i - 1); j++) ret += char_array_3[j];
  }

  return ret;
}

int _tmain(int argc, _TCHAR* argv[])
{
	std::cout << "HITB CTF 2011 - CantTouchThis v0.1 [KLKS/TSK]" << std::endl;

	XmlRpcClient c(CTF_SVR, 60888,"/KLKSRPC");

	XmlRpcValue result;
	XmlRpcValue noArg;
	if(!c.execute("CantTouchThis", noArg, result))
	{
#ifdef CTF_DEBUG
		std::cout << "InternalError, Execution Failed!!!" << std::endl;
#endif
		std::cout << "InternalError, you suck!!!" << std::endl;
		return 0;
	}

	unsigned long retSize = result.size();

#ifdef CTF_DEBUG
		std::cout << "Execution Successful, retSize = " << retSize << std::endl;
#endif

	if(retSize < 0)
	{
#ifdef CTF_DEBUG
		std::cout << "InternalError, Size Returned Failed!!!" << std::endl;
#endif
		std::cout << "InternalError, you suck!!!" << std::endl;
		return 0;
	}

#ifdef CTF_DEBUG
		std::cout << "Server returned : " << result.getString().data() << std::endl;
#endif

	LPVOID pExecutableMem = VirtualAlloc(NULL, retSize*2, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if(pExecutableMem == NULL)
	{
#ifdef CTF_DEBUG
		std::cout << "InternalError, VirtualAlloc Failed!!!" << std::endl;
#endif
		std::cout << "InternalError, you suck!!!" << std::endl;
		return 0;
	}

	std::string decoded = base64_decode(result.getString());
	std::copy(decoded.begin(), decoded.end(), (char *)pExecutableMem);

	__asm
	{
		push offset bEncryptedBuffer
		mov edx, pExecutableMem
		call edx
	}

	std::cout << "You made it this far, did you find out what happened?????" << std::endl;
	return 0;
}

