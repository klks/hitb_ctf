/*
Released under the MIT-license:

Copyright (c) 2012, Earl Marcus

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
#include "vm_processor.h"
#include "vm_opcodes.h"

#define FALSE                   0
#define TRUE                    1

//#define CTF_DEBUG	1

#define LISTEN_PORT	26498

//global variables
static pthread_mutex_t cs_mutex = PTHREAD_MUTEX_INITIALIZER;

int time_to_quit;

char *cFlag;
char challenge[2048];
char response[2048];
char good_bad_flag = 0;

void signal_handler(int sig)
{
 
    switch(sig)
    {
        case SIGHUP:
            break;
        case SIGINT:
        case SIGTERM:
            printf("Reactor Core is now shutting down...\n");
            time_to_quit = TRUE;
            break;
        default:
            break;
    }
}

void GenerateChallengeCode(char *cOutput)
{
    int i, j;
    for(i=0; i<3; i++)
    {
        for(j=0; j<3; j++)
        {
            unsigned char rchar = rand() % 255;
            sprintf(&cOutput[strlen(cOutput)], "%.2X", rchar);
        }
        if(i < 2)
            strcat(cOutput, "-");
    }
}

/*
Algo in C++

int checksum = 0;
for(int i=0; i<strlen(cChallenge); i++)
{
    checksum += cChallenge[i];
    checksum += 0xBABE;
    checksum <<= 2;
}
    AABBCCDD = checksum
    reordered to
    DDBB-AACC
*/
int VerifyChallengeResponse(char *cChallenge, char *cResponse)
{
    int iRet = -1;
    pthread_mutex_lock( &cs_mutex );
    
    int lenResponse = vm_strlen(cResponse);
    if(lenResponse != 9)
    {
        pthread_mutex_unlock( &cs_mutex );
        return iRet;
    }
    
    memcpy(challenge, cChallenge, vm_strlen(cChallenge));
    memcpy(response, cResponse, lenResponse);
    
    __asm__ __volatile__("call VM_ENTER\n");
    VM_MOV VM_REG2 VM_OFFSET
    __asm__ __volatile__("push $challenge\n");
    VM_MOV VM_REG3 VM_REG VM_REG2
        
    //REG2 - Pointer to cChallenge
    //REG4 - Pointer to serial
    //REG3 - Len of cChallenge
    //REG7 - Checksum of cChallenge
    //REG9 - General Purpose

    //Find length of cChallenge and do a checksum
    VM_XOR VM_REG7 VM_REG VM_REG7
    VM_XOR VM_REG8 VM_REG VM_REG8
    VM_XOR VM_REG10 VM_REG VM_REG10

    VM_EXIT
    
__asm__ __volatile__("checksum_loop:\n");
    __asm__ __volatile__("call VM_ENTER\n");
    VM_MOVZX VM_REG9 VM_REG3 __asm__ __volatile__(".byte 0x01\n");
    VM_INC VM_REG3
    VM_CMP VM_REG9 VM_REG VM_REG10
    VM_EXIT
    __asm__ __volatile__("call VM_GET_CMPBIT\n");
    __asm__ __volatile__("jz exit_checksum_loop\n");
    __asm__ __volatile__("call VM_ENTER\n");
    VM_ADD VM_REG7 VM_REG VM_REG9
    VM_ADD VM_REG7 VM_CONST __asm__ __volatile__(".byte 0x00\n .byte 0x00\n .byte 0xBA\n .byte 0xBE\n");
    VM_SHL VM_REG7 VM_CONST __asm__ __volatile__(".byte 0x00\n .byte 0x00\n .byte 0x00\n .byte 0x02\n");
    VM_EXIT
    __asm__ __volatile__("jmp checksum_loop");
    
__asm__ __volatile__("exit_checksum_loop:\n");
    __asm__ __volatile__("call VM_ENTER\n");

    //Verify Serial
    VM_MOV VM_REG4 VM_OFFSET
    __asm__ __volatile__("push $response\n");
    
    //Check if format is right (DDBB-AACC)
    VM_MOV VM_REG5 VM_REG VM_REG4
    VM_ADD VM_REG5 VM_CONST __asm__ __volatile__(".byte 0x00\n .byte 0x00\n .byte 0x00\n .byte 0x05");
    VM_MOVZX VM_REG5 VM_REG5 __asm__ __volatile__(".byte 0x01\n");
    VM_CMP VM_REG5 VM_CONST __asm__ __volatile__(".byte 0x00\n .byte 0x00\n .byte 0x00\n .byte 0x2D");
    VM_EXIT
    __asm__ __volatile__("jnz end_of_app\n");
    
    //Converted results stored in VM_REG6
    //Correctly Converted results stored in VM_REG8
__asm__ __volatile__("forming_loop:\n");
    __asm__ __volatile__("call VM_ENTER\n");
    VM_MOVZX VM_REG5 VM_REG4 __asm__ __volatile__(".byte 0x01\n");
    VM_INC VM_REG4
    VM_CMP VM_REG5 VM_REG VM_REG10
    VM_EXIT
    __asm__ __volatile__("call VM_GET_CMPBIT\n");
    __asm__ __volatile__("jz exit_forming_loop\n");
    __asm__ __volatile__("call VM_ENTER\n");
    //Check if -, true = jmp to forming loop
    VM_CMP VM_REG5 VM_CONST __asm__ __volatile__(".byte 0x00\n .byte 0x00\n .byte 0x00\n .byte 0x2D");
    VM_EXIT
    __asm__ __volatile__("call VM_GET_CMPBIT\n");
    __asm__ __volatile__("jz forming_loop\n");

    //Check if 0-9 & A-F
    __asm__ __volatile__("call VM_ENTER\n");
    VM_CMP VM_REG5 VM_CONST __asm__ __volatile__(".byte 0x00\n .byte 0x00\n .byte 0x00\n .byte 0x30");
    VM_EXIT
    __asm__ __volatile__("call VM_GET_CMPBIT\n");
    __asm__ __volatile__("jl end_of_app\n");

    __asm__ __volatile__("call VM_ENTER\n");
    VM_CMP VM_REG5 VM_CONST __asm__ __volatile__(".byte 0x00\n .byte 0x00\n .byte 0x00\n .byte 0x39");
    VM_EXIT
    __asm__ __volatile__("call VM_GET_CMPBIT\n");
    __asm__ __volatile__("jle sub_30\n");
    __asm__ __volatile__("jmp check_AF");
        
    //VM_REG5 is in 0-9
__asm__ __volatile__("sub_30:\n");
    __asm__ __volatile__("call VM_ENTER\n");
    VM_SUB VM_REG5 VM_CONST  __asm__ __volatile__(".byte 0x00\n .byte 0x00\n .byte 0x00\n .byte 0x30");
    VM_EXIT
    __asm__ __volatile__("jmp done_subbing\n");

__asm__ __volatile__("check_AF:\n");
    __asm__ __volatile__("call VM_ENTER\n");
    VM_CMP VM_REG5 VM_CONST __asm__ __volatile__(".byte 0x00\n .byte 0x00\n .byte 0x00\n .byte 0x46");
    VM_EXIT
    __asm__ __volatile__("call VM_GET_CMPBIT\n");
    __asm__ __volatile__("jg end_of_app\n");

    __asm__ __volatile__("call VM_ENTER\n");
    VM_CMP VM_REG5 VM_CONST __asm__ __volatile__(".byte 0x00\n .byte 0x00\n .byte 0x00\n .byte 0x41");
    VM_EXIT
    __asm__ __volatile__("call VM_GET_CMPBIT\n");
    __asm__ __volatile__("jl end_of_app\n");

    //VM_REG5 is in A-F
    __asm__ __volatile__("call VM_ENTER\n");
    VM_SUB VM_REG5 VM_CONST __asm__ __volatile__(".byte 0x00\n .byte 0x00\n .byte 0x00\n .byte 0x37");
    VM_EXIT

__asm__ __volatile__("done_subbing:\n");
    __asm__ __volatile__("call VM_ENTER\n");
    //Convert into VM_REG6
    VM_SHL VM_REG6 VM_CONST __asm__ __volatile__(".byte 0x00\n .byte 0x00\n .byte 0x00\n .byte 0x04");
    VM_ADD VM_REG6 VM_REG VM_REG5
    VM_EXIT
    __asm__ __volatile__("jmp forming_loop\n");

__asm__ __volatile__("exit_forming_loop:\n");

    __asm__ __volatile__("call VM_ENTER\n");
    //Reorder VM_REG6 (DDBB-AACC)
    //DD
    VM_MOV VM_REG8 VM_REG VM_REG6
    VM_SHL VM_REG8 VM_CONST __asm__ __volatile__(".byte 0x00\n .byte 0x00\n .byte 0x00\n .byte 0x08");
    VM_AND VM_REG8 VM_CONST __asm__ __volatile__(".byte 0x00\n .byte 0x00\n .byte 0xFF\n .byte 0x2D");

    //BB
    VM_MOV VM_REG3 VM_REG VM_REG6
    VM_SHR VM_REG3 VM_CONST __asm__ __volatile__(".byte 0x00\n .byte 0x00\n .byte 0x00\n .byte 0x10");
    VM_AND VM_REG3 VM_CONST __asm__ __volatile__(".byte 0x00\n .byte 0x00\n .byte 0x00\n .byte 0xFF");
    VM_ADD VM_REG8 VM_REG VM_REG3
    VM_SHL VM_REG8 VM_CONST __asm__ __volatile__(".byte 0x00\n .byte 0x00\n .byte 0x00\n .byte 0x08");

    //AA
    VM_MOV VM_REG3 VM_REG VM_REG6
    VM_SHR VM_REG3 VM_CONST __asm__ __volatile__(".byte 0x00\n .byte 0x00\n .byte 0x00\n .byte 0x18");
    VM_ADD VM_REG8 VM_REG VM_REG3
    VM_SHL VM_REG8 VM_CONST __asm__ __volatile__(".byte 0x00\n .byte 0x00\n .byte 0x00\n .byte 0x08");

    //CC
    VM_SHR VM_REG6 VM_CONST __asm__ __volatile__(".byte 0x00\n .byte 0x00\n .byte 0x00\n .byte 0x08");
    VM_AND VM_REG6 VM_CONST __asm__ __volatile__(".byte 0x00\n .byte 0x00\n .byte 0x00\n .byte 0xFF");
    VM_ADD VM_REG8 VM_REG VM_REG6

    //Check both serials
    VM_CMP VM_REG7 VM_REG VM_REG8
    VM_EXIT
    __asm__ __volatile__("call VM_GET_CMPBIT\n");
    __asm__ __volatile__("setz %al\n");
    __asm__ __volatile__("mov %%al, %0" :"=r"(good_bad_flag));
        
__asm__ __volatile__("end_of_app:\n");     

#ifdef CTF_DEBUG
    printf("VM_REG7 = 0x%.8X\n", GET_VM_REGISTER(OP_REG7));
    printf("VM_REG8 = 0x%.8X\n", GET_VM_REGISTER(OP_REG8));
#endif

    if(good_bad_flag)
        iRet = 0;
        
    pthread_mutex_unlock( &cs_mutex );
    return iRet;
}

void *LoginCheck(int client_sock)
{
    unsigned char buffer[1024];
    unsigned char authMsg[1024];
    unsigned char chlgSerial[100];

    int iRet;

    bzero(buffer, sizeof(buffer));
    bzero(authMsg, sizeof(authMsg));

    struct timeval tv;
    tv.tv_sec = 3;  /* 3 Secs Timeout */
    setsockopt(client_sock, SOL_SOCKET, SO_RCVTIMEO,(struct timeval *)&tv,sizeof(struct timeval));
    setsockopt(client_sock, SOL_SOCKET, SO_SNDTIMEO,(struct timeval *)&tv,sizeof(struct timeval));

    //Generate an challenge request
    GenerateChallengeCode(chlgSerial);

    //Send user login Request
    char msgLogin[] = "Reactor Core Remote Authentication Module\n";
    char msgReqResp[] = "Please transmit response...\n";
    char msgFailed[] = "Authentication Failed!!!...\n";
    sprintf(authMsg, "Challenge : %s\n", chlgSerial);

    //Welcome the user
    iRet = write(client_sock, msgLogin, strlen(msgLogin));
    //Show him the auth challenge
     iRet = write(client_sock, authMsg, strlen(authMsg));
    //Request for a response
    iRet = write(client_sock, msgReqResp, strlen(msgReqResp));

    //Wait for response
    int rlen = read(client_sock, buffer, sizeof(buffer)-1);
    if(rlen > 0)
    {
        if(VerifyChallengeResponse(chlgSerial, buffer) != -1)
        {
            write(client_sock, cFlag, strlen(cFlag));
        }
        else
        {
            write(client_sock, msgFailed, strlen(msgFailed));
        }
    }
    close(client_sock);
}

void* RemoteManagementInterface(int listen_port)
{
    int my_sock, client_sock, client_len;
    struct sockaddr_in svr_addr, client_addr;

    my_sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(my_sock < 0)
    {
        perror("Reactor Core Error when calling socket();");
        exit(EXIT_FAILURE);
    }
    bzero((char *)&svr_addr, sizeof(svr_addr));

    svr_addr.sin_family = AF_INET;
    svr_addr.sin_addr.s_addr = INADDR_ANY;
    svr_addr.sin_port = htons(listen_port);

    if(bind(my_sock, (struct sockaddr*)&svr_addr, sizeof(svr_addr)) < 0)
    {
        perror("Reactor Core Error when calling bind()");
        exit(EXIT_FAILURE);
    }
    listen(my_sock, 20);

    client_len = sizeof(client_addr);
    while(1)
    {
        client_sock = accept(my_sock, (struct sockaddr*)&client_addr, &client_len);
        if(client_sock < 0)
            continue;

        pthread_t tid;
        //Create a thread to handle login packet processing
        pthread_create(&tid, NULL, LoginCheck, (void*)client_sock);

    }
    return 0;
}

void main()
{
    srand(time(0));
    printf("HITB 2012 CTF Weapons of Mass Destruction - Fallout Apocalypse\n");
    VM_INIT();
    printf("  - v1.2 By KLKS\n");
    printf("Reactor Core #4 starting up...\n");

    //Infinite wait flag
    time_to_quit = FALSE;

    //Setup signal handling before we start
    signal(SIGHUP, signal_handler);
    signal(SIGTERM, signal_handler);
    signal(SIGINT, signal_handler);
    signal(SIGQUIT, signal_handler);
    signal(SIGPIPE, SIG_IGN);
    
    //Make sure we are not running as root
    uid_t uid=getuid(), euid=geteuid();
#ifdef CTF_DEBUG
    printf("UID:%d, EUID:%d\n", uid, euid);
#endif
    
    if (uid == 0 || uid!=euid)
    {
        printf("Reactor Core will not run with its current privileges...\n");
        exit(EXIT_FAILURE);
    }
    
    //Read FLAG into memory
    long fsize;
    FILE *f = fopen("flag.txt", "rb");
    if (!f)
    {
        printf("Reactor Core cannot read flag.txt data, too much defense?...\n");
        exit(EXIT_FAILURE);
    }
    //Get file size
    fseek(f, 0, SEEK_END);
    fsize = ftell(f);
    
    //Get file contents now
    cFlag = malloc(fsize+10);
    if(cFlag == NULL)
    {
        fclose(f);
        printf("Reactor Core cannot read flag.txt data, too little space...\n");
        exit(EXIT_FAILURE);
    }
    bzero(cFlag, fsize+10);
    rewind(f);
    fread(cFlag, 1, fsize, f);
    fclose(f);
#ifdef CTF_DEBUG
    printf("Flag : %s\n", cFlag);
#endif

    pthread_t tid;
    //Create a thread to handle Remote Logins
    pthread_create(&tid, NULL, RemoteManagementInterface,(void*)LISTEN_PORT);

    sleep(2);
    printf("Reactor Core is now stabilized...\n");

    //Wait to exit
    time_t now, start;
    char cTemp[1024];
    
    while(time_to_quit != TRUE)
    {
        sleep(1);
    }

    VM_CLEANUP();
    exit(EXIT_SUCCESS);
}

