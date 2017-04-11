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
 * http://www.linuxhowtos.org/C_C++/socket.htm
 * http://www.c.happycodings.com/Gnu-Linux/code18.html
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include "libctf.h"

void error(char *msg)
{
        perror(msg);
        exit(1);
}

void handle_exit(int sig_num)
{
	CtfLib_SvrCleanup();
	exit(0);
}

int main(int argc, char * argv[])
{
        if(CtfLib_Init() != 0)
		error("CTF Library failed to init\n");

	//Install SIGINT handler
	signal(SIGINT, handle_exit);

	while(1)
	{
		//CtfLib_SvrProcess();
		CtfLib_SvrTest();
		//sleep(1);
	}
        return 0;
}

