// SimpleDecrypt.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

/* Type declarations of 32-bit and 8-bit unsigned integers */
typedef unsigned int uint32;
typedef unsigned char byte;

/* Structure to store the instance data (internal state) */
typedef struct t_instance {
    uint32 x[8];
    uint32 c[8];
    uint32 carry;
} t_instance;

unsigned char cOutput[50];

#define SERIAL_PART	1

//#define RELEASE_DEBUG	1

#if SERIAL_PART == 1
byte key[16] = {
        0x1d, 0x27, 0x2c, 0x6a, 0x2d, 0x8e, 0x3d, 0xfc,
        0xac, 0x14, 0x05, 0x6b, 0x78, 0xd6, 0x33, 0xa0
    };
//unsigned char Serial[20] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
unsigned char Serial[20] = { 0x5C, 0x56, 0x85, 0x44, 0x7F, 0xC6, 0xC7, 0xDF, 0x48, 0x1A, 0xF3, 0xB5, 0x44, 0xAC, 0x7D, 0xC2};
#elif SERIAL_PART == 2
unsigned char Serial[20] = {0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE};
#endif


void key_setup(t_instance *p_instance, const byte *p_key);
void cipher(t_instance *p_instance, const byte *p_src, byte *p_dest, size_t data_size);

uint32 __rotl(uint32 x, int rot) {
    return (x << rot) | (x >> (32 - rot));
}

/* Square a 32-bit number to obtain the 64-bit result and return
the upper 32-bit XOR the lower 32-bit */
uint32 g_func(uint32 x) {

    /* Construct high and low argument for squaring */
    uint32 a = x & 0xffff;
    uint32 b = x >> 16;

    /* Calculate high and low result of squaring */
    uint32 h = ((((a * a) >> 17) + (a * b)) >> 15) + (b * b);
    uint32 l = x * x;

    /* Return high XOR low */
    return (h ^ l);
}

/* Calculate the next internal state */
void next_state(t_instance *p_instance) {
    uint32 g[8], c_old[8], i;

    /* Save old counter values */
    for (i = 0; i < 8; i++)
        c_old[i] = p_instance->c[i];

    /* Calculate new counter values */
    p_instance->c[0] += 0x4d34d34d + p_instance->carry;
    p_instance->c[1] += 0xd34d34d3 + (p_instance->c[0] < c_old[0]);
    p_instance->c[2] += 0x34d34d34 + (p_instance->c[1] < c_old[1]);
    p_instance->c[3] += 0x4d34d34d + (p_instance->c[2] < c_old[2]);
    p_instance->c[4] += 0xd34d34d3 + (p_instance->c[3] < c_old[3]);
    p_instance->c[5] += 0x34d34d34 + (p_instance->c[4] < c_old[4]);
    p_instance->c[6] += 0x4d34d34d + (p_instance->c[5] < c_old[5]);
    p_instance->c[7] += 0xd34d34d3 + (p_instance->c[6] < c_old[6]);
    p_instance->carry = (p_instance->c[7] < c_old[7]);

    /* Calculate the g-functions */
    for (i = 0; i < 8; i++)
        g[i] = g_func(p_instance->x[i] + p_instance->c[i]);

    /* Calculate new state values */
    p_instance->x[0] = g[0] + __rotl(g[7], 16) + _rotl(g[6], 16);
    p_instance->x[1] = g[1] + __rotl(g[0],  8) + g[7];
    p_instance->x[2] = g[2] + __rotl(g[1], 16) + _rotl(g[0], 16);
    p_instance->x[3] = g[3] + __rotl(g[2],  8) + g[1];
    p_instance->x[4] = g[4] + __rotl(g[3], 16) + _rotl(g[2], 16);
    p_instance->x[5] = g[5] + __rotl(g[4],  8) + g[3];
    p_instance->x[6] = g[6] + __rotl(g[5], 16) + _rotl(g[4], 16);
    p_instance->x[7] = g[7] + __rotl(g[6],  8) + g[5];
}

/* key_setup */
void key_setup(t_instance *p_instance, const byte *p_key) {
    uint32 k0, k1, k2, k3, i;

    /* Generate four subkeys */
    k0 = *(uint32*)(p_key + 0);
    k1 = *(uint32*)(p_key + 4);
    k2 = *(uint32*)(p_key + 8);
    k3 = *(uint32*)(p_key + 12);

    /* Generate initial state variables */
    p_instance->x[0] = k0;
    p_instance->x[2] = k1;
    p_instance->x[4] = k2;
    p_instance->x[6] = k3;
    p_instance->x[1] = (k3 << 16) | (k2 >> 16);
    p_instance->x[3] = (k0 << 16) | (k3 >> 16);
    p_instance->x[5] = (k1 << 16) | (k0 >> 16);
    p_instance->x[7] = (k2 << 16) | (k1 >> 16);

    /* Generate initial counter values */
    p_instance->c[0] = _rotl(k2, 16);
    p_instance->c[2] = _rotl(k3, 16);
    p_instance->c[4] = _rotl(k0, 16);
    p_instance->c[6] = _rotl(k1, 16);
    p_instance->c[1] = (k0 & 0xFFFF0000) | (k1 & 0xFFFF);
    p_instance->c[3] = (k1 & 0xFFFF0000) | (k2 & 0xFFFF);
    p_instance->c[5] = (k2 & 0xFFFF0000) | (k3 & 0xFFFF);
    p_instance->c[7] = (k3 & 0xFFFF0000) | (k0 & 0xFFFF);

    /* Reset carry flag */
    p_instance->carry = 0;

    /* Iterate the system four times */
    for (i = 0; i < 4; i++)
        next_state(p_instance);

    /* Modify the counters */
    for (i = 0; i < 8; i++)
        p_instance->c[(i + 4) & 0x7] ^= p_instance->x[i];
}

/* Encrypt or decrypt a block of data */
void cipher(t_instance *p_instance, const byte *p_src, byte *p_dest,
    size_t data_size) {
    uint32 i;

    for (i = 0; i < data_size; i += 16) {
        next_state(p_instance);  /* Iterate the system */

        /* Encrypt 16 bytes of data */
        *(uint32*)(p_dest + 0) = *(uint32*)(p_src + 0) ^
                                 (p_instance->x[0]) ^
                                 (p_instance->x[5] >> 16) ^
                                 (p_instance->x[3] << 16);

        *(uint32*)(p_dest + 4) = *(uint32*)(p_src + 4) ^
                                 (p_instance->x[2]) ^
                                 (p_instance->x[7] >> 16) ^
                                 (p_instance->x[5] << 16);

        *(uint32*)(p_dest + 8) = *(uint32*)(p_src + 8) ^
                                 (p_instance->x[4]) ^
                                 (p_instance->x[1] >> 16) ^
                                 (p_instance->x[7] << 16);

        *(uint32*)(p_dest + 12) = *(uint32*)(p_src + 12) ^
                                  (p_instance->x[6]) ^
                                  (p_instance->x[3] >> 16) ^
                                  (p_instance->x[1] << 16);

        /* Increment pointers to source and destination data */
        p_src += 16;
        p_dest += 16;
    }
}

void EvadeOS()
{
	OSVERSIONINFO osvi;
	ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&osvi);

	if(osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 1)
	{
      printf("Windows 7 Detected....needs win98\n");
	  ExitProcess(0);
	}
	if(osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 0)
	{
		printf("Windows Vista Detected...needs winXP\n");
		ExitProcess(0);
	}
	if(osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 2)
	{
		printf("Windows Server 2003 Detected... needs Win2008 Server\n");
		ExitProcess(0);
	}
	if(osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 1)
	{
		printf("Windows XP Detected...needs windows7\n");
		ExitProcess(0);
	}
	if(osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0)
	{
		printf("Windows 2000 Detected...needs WinNT\n");
		ExitProcess(0);
	}
	return;
}

DWORD WINAPI DecryptThreadProc(LPVOID lpParameter)
{
    t_instance state;
	
#ifdef RELEASE_DEBUG
	key_setup(&state, key);
    cipher(&state, Serial, cOutput, sizeof(Serial));

	printf("unsigned char Serial[20] = {");

    for (int i = 0; i < 16; i++) {
        printf(" 0x%02X,", cOutput[i]);
    }

	printf("};\n");
#else
	key_setup(&state, key);
    cipher(&state, Serial, cOutput, sizeof(Serial));
#endif
	return 0;
}

int _tmain(int argc, _TCHAR* argv[])
{
	HANDLE hThreadHandle;
	DWORD dwThreadID;

	ZeroMemory(cOutput, sizeof(cOutput));
	EvadeOS();
	hThreadHandle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)DecryptThreadProc, 0, 0, &dwThreadID);
	WaitForSingleObject(hThreadHandle, INFINITE);
	CloseHandle(hThreadHandle);
	printf("Did you see that?\n");
	return 0;
}

