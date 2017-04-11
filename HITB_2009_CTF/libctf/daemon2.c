/*
 * Released under the MIT-license:
 *
 * Copyright (c) 2009, Earl Marcus
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 * */
/*
 *  * References
 *   * http://www.linuxhowtos.org/C_C++/socket.htm
 *    */

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
        exit(0);
}

void usage()
{
	printf("Usage: client.ctf <publickey_file> <port> <flag_file>\n");
	exit(0);
}

int main(int argc, char * argv[])
{

	//Install SIGINT handler
        signal(SIGINT, handle_exit);
       	if(argc != 4) usage();

	CtfLib_Init(argv[3]);
       	CtfLib_LoadPublicKey(argv[1]);
	
	unsigned char buffer[BIG_BUFFER_SIZE];
	int my_sock, client_sock, client_len;
	struct sockaddr_in svr_addr, client_addr;
	my_sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(my_sock < 0)
		error("socket() Failed");
	
	bzero((char *)&svr_addr, sizeof(svr_addr));
	svr_addr.sin_family = AF_INET;
	svr_addr.sin_addr.s_addr = INADDR_ANY;
	svr_addr.sin_port = htons(atoi(argv[2]));

	if(bind(my_sock, (struct sockaddr*)&svr_addr, sizeof(svr_addr)) < 0)
		error("bind() Failed");
	
	listen(my_sock, 5);
	client_len = sizeof(client_addr);
	while(1)
	{
		client_sock = accept(my_sock, (struct sockaddr*)&client_addr, &client_len);
		if(client_sock < 0)
			error("accpet() Failed");

		bzero(buffer, sizeof(buffer));
		int rlen = read(client_sock, buffer, sizeof(buffer)-1);
		if(rlen < 0)
			error("read() Failed");

		CtfLib_ClientProcess(client_sock, buffer, rlen);
		close(client_sock);
	}
	return 0;
}
