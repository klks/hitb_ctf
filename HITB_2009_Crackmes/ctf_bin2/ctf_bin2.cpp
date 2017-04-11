// ctf_bin2.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <windows.h>
#include "xor_table.h"
//#define DEBUG		1
//#define RELEASE_DEBUG	1

#ifdef RELEASE_DEBUG
char cEvilArray[50];
#endif

/*
BYTE bEncArray[]={0x04, 0x04, 0x04, 0x04, \	//0
				  0x01, 0x01, 0x01, 0x01, \	//1
				  0x00, 0x00, 0x00, 0x00, \	//2
				  0x01, 0x01, 0x01, 0x01, \	//3
				  0x02, 0x02, 0x02, 0x02, \	//4
				  0x03, 0x03, 0x03, 0x03, \	//5
				  0x02, 0x02, 0x02, 0x02, \	//6
				  0x03, 0x03, 0x03, 0x03, \	//7
				  0x04, 0x04, 0x04, 0x04, \	//8
				  0x00, 0x00, 0x00, 0x00,	//9
				};
*/

BYTE bEncArray[]={0x2F, 0xED, 0x0A, 0x9F, \
				  0xD4, 0x63, 0xD8, 0xC1, \
				  0xBA, 0x17, 0x4C, 0x3A, \
				  0x37, 0x63, 0x48, 0x0D, \
				  0xCB, 0xCB, 0x96, 0x9E, \
				  0xD1, 0x90, 0xE5, 0xB1, \
				  0x25, 0x84, 0xB1, 0xD0, \
				  0x71, 0x98, 0xE1, 0x55, \
				  0xF2, 0xFF, 0x65, 0x55, \
				  0xEF, 0x63, 0xE8, 0x3F	
				};

#ifdef DEBUG
void MakeXorArray()
{
	srand(GetTickCount());

	FILE *fp;
    fp = fopen("XorTable.txt", "w");
    if(fp == NULL)
    {
            printf("fopen() Failed");
            return;
    }

	fprintf(fp, "DWORD cXorArray[]={\n");
	for(int i=0; i<1000; i++)
	{
		fprintf(fp, "\t\t");
		for(int j=0; j<5; j++)
		{
			//Generate 4 random bytes
			BYTE pR[5];
			pR[0] = rand() & 0xFF;
			pR[1] = rand() & 0xFF;
			pR[2] = rand() & 0xFF;
			pR[3] = rand() & 0xFF;
			DWORD *dwp = (DWORD *)pR;

			fprintf(fp, "0x%.8x,", *dwp);
		}
		fprintf(fp, " \\\n");
	}
	fprintf(fp, "};");
	fflush(fp);
    fclose(fp);
}

void HexPrint(BYTE *bp)
{
        int i;
        for(i=0; i<4; i++)
                printf("%.2x", (unsigned char)bp[i]);

        printf("\n");
}

void CharPrint(BYTE *bp)
{
        int i;
        for(i=0; i<4; i++)
                printf("%c", (unsigned char)bp[i]);

        printf("\n");
}

void TestDecrypt(BYTE *cTextArray)
{
	BYTE cDecodedArray[50];

	ZeroMemory(cDecodedArray, sizeof(cDecodedArray));

	//T0
	char *cp = (char *)cTextArray;
	cp += 8;
	DWORD *dwp_T00 = (DWORD *)cp;
	cp = (char *)cTextArray;
	cp += 36;
	DWORD *dwp_T01 = (DWORD *)cp;

	//T1
	cp = (char *)cTextArray;
	cp += 4;
	DWORD *dwp_T10 = (DWORD *)cp;
	cp = (char *)cTextArray;
	cp += 12;
	DWORD *dwp_T11 = (DWORD *)cp;

	//T2
	cp = (char *)cTextArray;
	cp += 16;
	DWORD *dwp_T20 = (DWORD *)cp;
	cp = (char *)cTextArray;
	cp += 24;
	DWORD *dwp_T21 = (DWORD *)cp;

	//T3
	cp = (char *)cTextArray;
	cp += 20;
	DWORD *dwp_T30 = (DWORD *)cp;
	cp = (char *)cTextArray;
	cp += 28;
	DWORD *dwp_T31 = (DWORD *)cp;

	//T4
	cp = (char *)cTextArray;
	DWORD *dwp_T40 = (DWORD *)cp;
	cp = (char *)cTextArray;
	cp += 32;
	DWORD *dwp_T41 = (DWORD *)cp;

//061E-8FEC-0D9E-9EBA-4B69-A28A-5632-DBBE-E721-6AF4
//0937-4217-3927-2198-1796-3105-2906-4987-2584-0483	//Loops when they will get decrypted
// T40- T10- T00- T11- T20- T30- T21- T31- T41- T01
// 0  - 1  - 2  - 3  - 4  - 5  - 6  - 7  - 8  - 9  - Dword
// 0  - 4  - 8  - 12 - 16 - 20 - 24 - 28 - 32 - 36 - Byte

	for(int i=0; i<5000; i++)
	{
		//T0
		*dwp_T00 ^= cXorArray_0[i];
		*dwp_T01 ^= cXorArray_0[i];
		if(i == 3927)
		{
			printf("T0 - %d - ", i);
			CharPrint((BYTE*)dwp_T00);
			memcpy(&cDecodedArray[8], (unsigned char*)dwp_T00, 4);
		}
		if(i == 483)
		{
			printf("T0 - %d - ", i);
			CharPrint((BYTE*)dwp_T01);
			memcpy(&cDecodedArray[36], (unsigned char*)dwp_T01, 4);
		}

		
		//T1
		*dwp_T10 ^= cXorArray_1[i];
		*dwp_T11 ^= cXorArray_1[i];
		if(i == 4217)
		{
			printf("T1 - %d - ", i);
			CharPrint((BYTE*)dwp_T10);
			memcpy(&cDecodedArray[4], (unsigned char*)dwp_T10, 4);
		}
		if(i == 2198)
		{
			printf("T1 - %d - ", i);
			CharPrint((BYTE*)dwp_T11);
			memcpy(&cDecodedArray[12], (unsigned char*)dwp_T11, 4);
		}

		//T2
		*dwp_T20 ^= cXorArray_2[i];
		*dwp_T21 ^= cXorArray_2[i];
		if(i == 1796)
		{
			printf("T2 - %d - ", i);
			CharPrint((BYTE*)dwp_T20);
			memcpy(&cDecodedArray[16], (unsigned char*)dwp_T20, 4);
		}
		if(i == 2906)
		{
			printf("T2 - %d - ", i);
			CharPrint((BYTE*)dwp_T21);
			memcpy(&cDecodedArray[24], (unsigned char*)dwp_T21, 4);
		}

		//T3
		*dwp_T30 ^= cXorArray_3[i];
		*dwp_T31 ^= cXorArray_3[i];
		if(i == 3105)
		{
			printf("T3 - %d - ", i);
			CharPrint((BYTE*)dwp_T30);
			memcpy(&cDecodedArray[20], (unsigned char*)dwp_T30, 4);
		}
		if(i == 4987)
		{
			printf("T3 - %d - ", i);
			CharPrint((BYTE*)dwp_T31);
			memcpy(&cDecodedArray[28], (unsigned char*)dwp_T31, 4);
		}

		//T4
		*dwp_T40 ^= cXorArray_4[i];
		*dwp_T41 ^= cXorArray_4[i];
		//Check and print
		if(i == 937)
		{
			printf("T4 - %d - ", i);
			CharPrint((BYTE*)dwp_T40);
			memcpy(cDecodedArray, (unsigned char*)dwp_T40, 4);
		}
		if(i == 2584)
		{
			printf("T4 - %d - ", i);
			CharPrint((BYTE*)dwp_T41);
			memcpy(&cDecodedArray[32], (unsigned char*)dwp_T41, 4);
		}
		//Dump entry to file
		FILE *fp;
		fp = fopen("DecryptDump.txt", "a");
		for(int i=0; i<40; i++)
			isprint((unsigned char)cTextArray[i]) ? fprintf(fp, "%.c", (unsigned char)cTextArray[i]):fprintf(fp, ".");
		fprintf(fp, "\n");
		fflush(fp);
		fclose(fp);
	}

	printf("Decoded String : %s", cDecodedArray);
}

void MakeEncMessage()
{
BYTE cTextArray[41]="061E8FEC0D9E9EBA4B69A28A5632DBBEE7216AF4";	//SHA-160 for "Kingdoms divided soon fall."
//061E-8FEC-0D9E-9EBA-4B69-A28A-5632-DBBE-E721-6AF4
//0937-4217-3927-2198-1796-3105-2906-4987-2584-0483	//Loops when they will get decrypted
// T40- T10- T00- T11- T20- T30- T21- T31- T41- T01
// 0  - 1  - 2  - 3  - 4  - 5  - 6  - 7  - 8  - 9  - Dword
// 0  - 4  - 8  - 12 - 16 - 20 - 24 - 28 - 32 - 36 - Byte

	printf("Good String : %s\n", cTextArray);

	//T0
	char *cp = (char *)cTextArray;
	cp += 8;
	DWORD *dwp_T00 = (DWORD *)cp;
	cp = (char *)cTextArray;
	cp += 36;
	DWORD *dwp_T01 = (DWORD *)cp;

	//T1
	cp = (char *)cTextArray;
	cp += 4;
	DWORD *dwp_T10 = (DWORD *)cp;
	cp = (char *)cTextArray;
	cp += 12;
	DWORD *dwp_T11 = (DWORD *)cp;

	//T2
	cp = (char *)cTextArray;
	cp += 16;
	DWORD *dwp_T20 = (DWORD *)cp;
	cp = (char *)cTextArray;
	cp += 24;
	DWORD *dwp_T21 = (DWORD *)cp;

	//T3
	cp = (char *)cTextArray;
	cp += 20;
	DWORD *dwp_T30 = (DWORD *)cp;
	cp = (char *)cTextArray;
	cp += 28;
	DWORD *dwp_T31 = (DWORD *)cp;

	//T4
	cp = (char *)cTextArray;
	DWORD *dwp_T40 = (DWORD *)cp;
	cp = (char *)cTextArray;
	cp += 32;
	DWORD *dwp_T41 = (DWORD *)cp;

	for(int i=4999; i>=0; i--)
	{
		//T0
		if(i <= 3927)
		{
			*dwp_T00 ^= cXorArray_0[i];
		}
		if(i <= 483)
		{
			*dwp_T01 ^= cXorArray_0[i];
		}

		//T1
		if(i <= 4217)
		{
			*dwp_T10 ^= cXorArray_1[i];
		}
		if(i <= 2198)
		{
			*dwp_T11 ^= cXorArray_1[i];
		}

		//T2
		if(i <= 1796)
		{
			*dwp_T20 ^= cXorArray_2[i];
		}
		if(i <= 2906)
		{
			*dwp_T21 ^= cXorArray_2[i];
		}

		//T3
		if(i <= 3105)
		{
			*dwp_T30 ^= cXorArray_3[i];
		}
		if(i <= 4987)
		{
			*dwp_T31 ^= cXorArray_3[i];
		}

		//T4
		if(i <= 937)
		{
			*dwp_T40 ^= cXorArray_4[i];
		}
		if(i <= 2584)
		{
			*dwp_T41 ^= cXorArray_4[i];
		}
	}

	printf("Encoded String : ");
	for(int i=0; i<40; i++)
		printf("%.2X ", (unsigned char)cTextArray[i]);

	printf("\n");

	//Sucky code :(
	//TestDecrypt(cTextArray);
}
#else
//DWORD dwLoopCounter = 5000;
int dwLoopCounter = 5000;
HANDLE hEvents[5];

typedef NTSTATUS (CALLBACK *ULPRET)(HANDLE, ULONG, PVOID, ULONG);

void thread_detach()
{
	HINSTANCE hLib;
	ULPRET _NtSetInformationThread;

	hLib = LoadLibrary("ntdll.dll");
	if(hLib) {

		_NtSetInformationThread = (ULPRET)GetProcAddress(hLib, "NtSetInformationThread");
	
		_NtSetInformationThread(GetCurrentThread(), 0x11, 0, 0);
	}

	//MessageBox(NULL, "Debugger Detached", "Debugger Detached", MB_OK);
}

DWORD WINAPI ThreadProc_0(LPVOID lpParameter)
{
	char *cp = (char *)bEncArray;
	cp += 8;
	DWORD *dwp_T00 = (DWORD *)cp;
	cp = (char *)bEncArray;
	cp += 36;
	DWORD *dwp_T01 = (DWORD *)cp;

	for(int i=0; i<dwLoopCounter; i++)
	{
		WaitForSingleObject(hEvents[0], INFINITE);
		*dwp_T00 ^= cXorArray_0[i];
		*dwp_T01 ^= cXorArray_0[i];
#ifdef RELEASE_DEBUG
		if(i == 3927)
			memcpy(&cEvilArray[8], (unsigned char*)dwp_T00, 4);
		if(i == 483)
			memcpy(&cEvilArray[36], (unsigned char*)dwp_T01, 4);
#endif
		SetEvent(hEvents[1]);
	}
	return 0;
}

DWORD WINAPI ThreadProc_1(LPVOID lpParameter)
{
	char *cp = (char *)bEncArray;
	cp += 4;
	DWORD *dwp_T10 = (DWORD *)cp;
	cp = (char *)bEncArray;
	cp += 12;
	DWORD *dwp_T11 = (DWORD *)cp;

	for(int i=0; i<dwLoopCounter; i++)
	{
		WaitForSingleObject(hEvents[1], INFINITE);
		*dwp_T10 ^= cXorArray_1[i];
		*dwp_T11 ^= cXorArray_1[i];
#ifdef RELEASE_DEBUG
		if(i == 4217)
			memcpy(&cEvilArray[4], (unsigned char*)dwp_T10, 4);
		if(i == 2198)
			memcpy(&cEvilArray[12], (unsigned char*)dwp_T11, 4);
#endif
		SetEvent(hEvents[2]);
	}
	return 0;
}

DWORD WINAPI ThreadProc_2(LPVOID lpParameter)
{
	char *cp = (char *)bEncArray;
	cp += 16;
	DWORD *dwp_T20 = (DWORD *)cp;
	cp = (char *)bEncArray;
	cp += 24;
	DWORD *dwp_T21 = (DWORD *)cp;

	for(int i=0; i<dwLoopCounter; i++)
	{
		WaitForSingleObject(hEvents[2], INFINITE);
		*dwp_T20 ^= cXorArray_2[i];
		*dwp_T21 ^= cXorArray_2[i];
#ifdef RELEASE_DEBUG
		if(i == 1796)
			memcpy(&cEvilArray[16], (unsigned char*)dwp_T20, 4);
		if(i == 2906)
			memcpy(&cEvilArray[24], (unsigned char*)dwp_T21, 4);
#endif
		SetEvent(hEvents[3]);
	}
	return 0;
}

DWORD WINAPI ThreadProc_3(LPVOID lpParameter)
{
	char *cp = (char *)bEncArray;
	cp += 20;
	DWORD *dwp_T30 = (DWORD *)cp;
	cp = (char *)bEncArray;
	cp += 28;
	DWORD *dwp_T31 = (DWORD *)cp;

	for(int i=0; i<dwLoopCounter; i++)
	{
		WaitForSingleObject(hEvents[3], INFINITE);
		*dwp_T30 ^= cXorArray_3[i];
		*dwp_T31 ^= cXorArray_3[i];
#ifdef RELEASE_DEBUG
		if(i == 3105)
			memcpy(&cEvilArray[20], (unsigned char*)dwp_T30, 4);
		if(i == 4987)
			memcpy(&cEvilArray[28], (unsigned char*)dwp_T31, 4);
#endif
		SetEvent(hEvents[4]);
	}
	return 0;
}

DWORD WINAPI ThreadProc_4(LPVOID lpParameter)
{
	char *cp = (char *)bEncArray;
	DWORD *dwp_T40 = (DWORD *)cp;
	cp = (char *)bEncArray;
	cp += 32;
	DWORD *dwp_T41 = (DWORD *)cp;

	for(int i=0; i<dwLoopCounter; i++)
	{
		WaitForSingleObject(hEvents[4], INFINITE);
		*dwp_T40 ^= cXorArray_4[i];
		*dwp_T41 ^= cXorArray_4[i];
#ifdef RELEASE_DEBUG
		if(i == 937)
			memcpy(cEvilArray, (unsigned char*)dwp_T40, 4);
		if(i == 2584)
			memcpy(&cEvilArray[32], (unsigned char*)dwp_T41, 4);
#endif
		SetEvent(hEvents[0]);
	}
	return 0;
}
#endif

int _tmain(int argc, _TCHAR* argv[])
{
	thread_detach();
#ifdef DEBUG
	//Create XOR Array
	//MakeXorArray();	//Dont uncomment unless you want to generate a new XOR table
	MakeEncMessage();
	TestDecrypt(bEncArray);
#else
	HANDLE hThreadHandles[5];
	DWORD dwThreadID[5];

#ifdef RELEASE_DEBUG
	ZeroMemory(cEvilArray, sizeof(cEvilArray));
#endif
	printf("A wise man once said \"We learn little from victory but much from defeat.\"\n");
	//Create Events
	hEvents[0] = CreateEvent(NULL, FALSE, TRUE, "T0"); 
	hEvents[1] = CreateEvent(NULL, FALSE, FALSE, "T1"); 
	hEvents[2] = CreateEvent(NULL, FALSE, FALSE, "T2"); 
	hEvents[3] = CreateEvent(NULL, FALSE, FALSE, "T3"); 
	hEvents[4] = CreateEvent(NULL, FALSE, FALSE, "T4"); 

	//Create Threads
	hThreadHandles[0] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadProc_0, 0, 0, &dwThreadID[0]);
	hThreadHandles[1] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadProc_1, 0, 0, &dwThreadID[1]);
	hThreadHandles[2] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadProc_2, 0, 0, &dwThreadID[2]);
	hThreadHandles[3] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadProc_3, 0, 0, &dwThreadID[3]);
	hThreadHandles[4] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadProc_4, 0, 0, &dwThreadID[4]);

	//Wait for them all to complete
	WaitForMultipleObjects(5, hThreadHandles, TRUE, INFINITE);

#ifdef RELEASE_DEBUG
	printf("%s\n", cEvilArray);
#endif

	printf("Thanks for playing!\n");
#endif
	return 0;
}