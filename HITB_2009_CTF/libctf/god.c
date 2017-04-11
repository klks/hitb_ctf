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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include "libctf.h"
#include "miracl.h"

void error(char *msg)
{
        perror(msg);
        exit(-1);
}

void usage()
{
	printf("Usage: god.ctf [options] <param.1> <param.2> <param.n>\n");
	printf("[Options]\n");
	printf("\t-p <loopCount>\t\t\tPropogates flags to teams\n");
	printf("\t-m <flagOut>\t\t\tGenerate a new set of flags\n");

	//Checks
	printf("\t-cp <teamIP> <daemonPort>\tChecks for flag problems\n");
	printf("\t-cx <teamIP> <daemonPort>\tChecks file hash of daemon\n");
	printf("\t-cm <teamIP> <daemonPort>\tChecks mem hash of daemon\n");
	//printf("\t-cv <flagData>\t\tChecks if a flag is a valid flag\n");

	//Gets
	printf("\t-gf <teamIP> <daemonPort>\tGets the flag for a particular teams daemon\n");
	printf("\t-gx <teamIP> <daemonPort>\tGet file hash of daemon\n");
        printf("\t-gm <teamIP> <daemonPort>\tGet mem hash of daemon\n");
	
	//Sets
	printf("\t-sf <teamIP> <daemonPort>\tSets the flag for a particular teams daemon\n");

	printf("\n");
	printf("[Notes]\n");
	printf("\t<teamIP>\t\t\t\tSet to 0 to enumerate all IP\n");
	printf("\t<daemonPort>\t\t\tSet to 0 to enumerate all ports\n");
}

int main(int argc, char * argv[])
{
	if(argc < 2)
	{
		usage();
		return -1;
	}

	if(CtfLib_Init() != 0)
                error("CTF Library failed to init\n");

	//I should add more checking but i'm gonna
	//assume whoever uses god has a IQ > 10
	if(memcmp(argv[1], "-p", 2) == 0)
		CtfLib_SvrPropogateFlags(atoi(argv[2]));
	else if(memcmp(argv[1], "-m", 2) == 0)
		CtfLib_SvrGenNewFlags(argv[2]);

	//Checks
	else if(memcmp(argv[1], "-cp", 3) == 0)
		CtfLib_SvrEnumClients(argv[2], atoi(argv[3]), CTF_CHECKFLAG);
	else if(memcmp(argv[1], "-cx", 3) == 0)
                CtfLib_SvrEnumClients(argv[2], atoi(argv[3]), CTF_CHECKHASHFILE);
	else if(memcmp(argv[1], "-cm", 3) == 0)
                CtfLib_SvrEnumClients(argv[2], atoi(argv[3]), CTF_CHECKHASHMEM);

	//Gets
	else if(memcmp(argv[1], "-gf", 3) == 0)
		CtfLib_SvrEnumClients(argv[2], atoi(argv[3]), CTF_GETFLAG);
	else if(memcmp(argv[1], "-gx", 3) == 0)
                CtfLib_SvrEnumClients(argv[2], atoi(argv[3]), CTF_GETHASHFILE);
	else if(memcmp(argv[1], "-gm", 3) == 0)
                CtfLib_SvrEnumClients(argv[2], atoi(argv[3]), CTF_GETHASHMEM);

	//Sets
	else if(memcmp(argv[1], "-sf", 3) == 0)
		CtfLib_SvrEnumClients(argv[2], atoi(argv[3]), CTF_SETFLAG);

	else usage();

	CtfLib_SvrCleanup();
        return CtfLib_GetLastError();
}
