#include <stdio.h>

//JUNK_DEFINES

int ShellTheCode(char *cEncBuff)
{
	//OBFUSCATED_VARIABLES
	
	//JUNK_CODE
	for(int i=0; i<60;i++)
	{
		if(cEncBuff[i] > 30 && i < 10)
		{
			//JUNK_CODE
		}
		else if(i > 20 && cEncBuff[i] < 0xE0)
		{
			//JUNK_CODE
		}
		else if(cEncBuff[i] == i)
		{
			//JUNK_CODE
		}
		
	//Decrypt
	//JUNK_CODE
		cEncBuff[i] ^= 0x30;
	//JUNK_CODE
	
	//Encrypt
		cEncBuff[i] ^= 0x30;
	//JUNK_CODE
	}
	//JUNK_CODE
	return 0;
}

int main()
{
	unsigned long ulSize = 0;
	unsigned char *ucp = (unsigned char *)&ShellTheCode;
	
	//Find end of function
	for(;;)
	{
		//VC++ //mov     esp, ebp	//pop     ebp	//retn
		if(*ucp == 0x8B && *(ucp+1) == 0xE5 && *(ucp+2) == 0x5D && *(ucp+3) == 0xC3)
		{
			break;	//ret
		}
		if(*ucp == 0x8B && *(ucp+1) == 0xE5 && *(ucp+2) == 0x5D && *(ucp+3) == 0xC2) 
		{
			ulSize +=2;	//For remainder of ret
			break;	//ret n
		}
		ulSize += 1;
		ucp++;
	}
	ulSize += 4;
	FILE * pFile;
	pFile = fopen ("shellcode.txt","w");
	if (pFile!=NULL)
	{
		fwrite(&ShellTheCode, 1, ulSize, pFile);
		fclose (pFile);
		printf("Shellcode.txt generated\n");
		return 0;
	}
	return -1;
}