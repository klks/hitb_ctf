// mmx_bin.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
using namespace std;

//#define CTF_DEBUG 1

#ifdef CTF_DEBUG
__int64 Get_i64(__m64 n)
{
    union __m64__m64
    {
        __m64 m;
        __int64 i;
    } mi;

    mi.m = n;
    return mi.i;
}
#endif

unsigned char * Get_c64(__m64 m)
{
    union __m64__m64
    {
        __m64 m;
        unsigned char c[8];
    } mi;

	_mm_empty();
    mi.m = m;
	_mm_empty();
    return mi.c;
}

__m64 Get_m64(__int64 n)
{
    union __m64__m64
    {
        __m64 m;
        __int64 i;
    } mi;

	_mm_empty();
    mi.i = n;
	_mm_empty();
    return mi.m;
}

int _tmain(int argc, _TCHAR* argv[])
{
	int CPUInfo[4] = {-1};
	bool bMMXExtensions = false;
	char input_buffer[500];
	char cTemp[20];
	bool valid_password = false;

#ifdef CTF_DEBUG	//Our desired unlock code
	unsigned char password[18] = "e8y8KulZ9qXoZW94\x00";
#else
	unsigned char password[17] = {0x2C, 0x35, 0xF0, 0xDF, 0x0C, 0x2D, 0x9D, 0xA7, 0x49, 0xCA, 0xAA, 0x64, 0x95, 0xB4, 0x23, 0x34, 00};
#endif

	__cpuid(CPUInfo, 1);
	bMMXExtensions = (CPUInfo[3] & 0x800000) || false;

	if (!bMMXExtensions)
	{
		printf("You do not qualify\n");
		return 0;
	}

	printf("wargames.my CTF 2011 Bonus Binary 02 [KLKS\\TSK] (HITB Hardness Level 6/10)\n");

#ifdef CTF_DEBUG
	printf("Password is set to : ");
	memset(input_buffer, 0, sizeof(input_buffer));
	strcpy(input_buffer, "3565541561357657462135416462513699543372594632135168732135168464");
	printf("%s\n", input_buffer);
#else
	printf("Password please : ");
	memset(input_buffer, 0, sizeof(input_buffer));
	fgets(input_buffer, sizeof(input_buffer), stdin);
	input_buffer[strlen(input_buffer)-1] = 0;
#endif

	if(strlen(input_buffer) != 64)
	{
		printf("Nope try again later\n");
		return 0;
	}

	for(int i=0;i<(int)strlen(input_buffer);i++)
	{
		if(input_buffer[i] < '0' || input_buffer[i] > '9')
		{
			printf("Invalid password\n");
			return 0;
		}
	}
	__int64 a1=0;
	__m64 a,b,c,d, k, l, k1,s;
	__m64 x1,x2,x3,x4;
	__m64 j1,j2,j3,j4,cmp;

	//Input xxxxxxxxyyyyyyyy-zzzzzzzzaaaaaaaa-xxxxxxxxyyyyyyyy-zzzzzzzzaaaaaaaa
	//1111111111111111111111111111111111111111111111111111111111111111

	_mm_empty();

	memset(cTemp, 0, sizeof(cTemp));

	memcpy(cTemp, input_buffer, 16);
	sscanf(cTemp, "%I64d", &a1);
	a = Get_m64(a1);

	memcpy(cTemp, input_buffer+16, 16);
	sscanf(cTemp, "%I64d", &a1);
	b = Get_m64(a1);

	memcpy(cTemp, input_buffer+32, 16);
	sscanf(cTemp, "%I64d", &a1);
	c = Get_m64(a1);

	memcpy(cTemp, input_buffer+48, 16);
	sscanf(cTemp, "%I64d", &a1);
	d = Get_m64(a1);

	k = _mm_xor_si64 (a , c);
	l = _mm_xor_si64 (d , b);

	k1 = _mm_sub_pi32(a,b);
	s = _mm_sub_pi32(c,d);

	x1 = Get_m64(0xCAFEBABEDEADC0DE);
	x4 = Get_m64(0x168a68731d84613c);
	x3 = Get_m64(0x3b168b6516d4e564);
	x2 = Get_m64(0x2a58761c26e846f4);

	j1 = Get_m64(0x169b680898fc6961);
	j2 = Get_m64(0xc4eab4da6e01d30d);
	j3 = Get_m64(0xcad14d0348ccb7aa);
	j4 = Get_m64(0x2a5a41ebc16e41d4);

	x1 = _mm_xor_si64 (x1 , k);
	x2 = _mm_xor_si64 (x2 , l);
	x3 = _mm_xor_si64 (x3 , k1);
	x4 = _mm_xor_si64 (x4 , s);

#ifdef CTF_DEBUG
	//print the values for x1=j3,x2=j4,x3=j2,x4=j1
	__int64 iTmp = Get_i64(x3);
	printf("j1 =  Get_m64(0x%I64X);\n", iTmp);
	iTmp = Get_i64(x3);
	printf("j2 =  Get_m64(0x%I64X);\n", iTmp);
	iTmp = Get_i64(x1);
	printf("j3 =  Get_m64(0x%I64X);\n", iTmp);
	iTmp = Get_i64(x2);
	printf("j4 =  Get_m64(0x%I64X);\n", iTmp);
#endif

	unsigned long sum = 0;
	unsigned char *cp;
	int i;
	cmp = _mm_cmpeq_pi32(x1,j3);
	cp = Get_c64(cmp);
	for(i =0;i<8;i++)
	{
		sum += (unsigned long)*cp;
		cp++;
	}
	cmp = _mm_cmpeq_pi32(x3,j2);
	cp = Get_c64(cmp);
	for(i =0;i<8;i++)
	{
		sum += (unsigned long)*cp;
		cp++;
	}
	cmp = _mm_cmpeq_pi32(x2,j4);
	cp = Get_c64(cmp);
	for(i =0;i<8;i++)
	{
		sum += (unsigned long)*cp;
		cp++;
	}
	cmp = _mm_cmpeq_pi32(x4,j1);
	cp = Get_c64(cmp);
	for(i =0;i<8;i++)
	{
		sum += (unsigned long)*cp;
		cp++;
	}

	if(sum == 8160)
	{
		valid_password = true;
	}

	_mm_empty();

#ifdef CTF_DEBUG
	printf("SUM = %d\n", sum);
#endif

	if(valid_password)
	{
		_mm_empty();
		//Decode password using k,l,k1,s
		j1 = _mm_madd_pi16(k,k1);
		j2 = _mm_madd_pi16(l,s);

		cp = Get_c64(j1);
		unsigned char *cp2 = password;
		for(i =0;i<8;i++)
		{
			*cp2 ^= *cp;
			cp++;
			cp2++;
		}
		cp = Get_c64(j2);
		for(i =0;i<8;i++)
		{
			*cp2 ^= *cp;
			cp++;
			cp2++;
		}
		_mm_empty();

		printf("Well done, the unlock code is : ");
#ifdef CTF_DEBUG
		for(int i=0;i<16;i++)
		{
			printf("0x%.2X, ", (unsigned char)password[i]);
		}
#else
		printf("%s", password);
#endif
	}
	else
	{
		printf("Maybe some other time....");
	}
	return 0;
}

