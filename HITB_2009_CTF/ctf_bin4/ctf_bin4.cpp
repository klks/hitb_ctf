//Super easy crackme, -10/10
#include <windows.h>
#include "resource.h"

//#define DEBUG	1

char cEncryptedSerial[] = "43E054986598269C67E3389519DE6a911b901cDA4dDB4eDC4fD8208C41D752D2438F748935D266D2378B488549836aD65b876cCA";
HGLOBAL pSample1=NULL;
char pSolution1[1000];

//C<)n&Yw+{rHFG|XD^)g8nY\0N2
//43E054986598269C67E3389519DE6a911b901cDA4dDB4eDC4fD8208C41D752D2438F748935D266D2378B488549836aD65b876cCA

void GenerateRandomString()
{
	DWORD dwLen = (rand() % 20) + 10;

	pSample1 = GlobalAlloc(GPTR, dwLen+10);
	char *cp = (char *)pSample1;

	for(DWORD i=0; i<dwLen; i++)
	{
		*cp = (char)(rand() % 93) + 33;
		cp++;
	}
}
#ifdef DEBUG
DWORD str2dw(char* cIN)
{
        DWORD iRet = 0;
        char *cp = cIN;
        int iCount = 0;
        BYTE b;

        if(cIN == NULL) return 0;

        while(*cp != 0  && iCount < 9)
        {
                b = *cp;
                if(b >= '0' && b <= '9') b -= '0';
                else if(b >= 'a' && b <= 'f') b -= 'W';
                else if(b >= 'A' && b <= 'F') b -= '7';
                else return 0;
                iRet = (iRet << 4) + b;
                cp++;
                iCount++;
        }

        return iRet;
}

void ReverseGeneratedSerial(HWND hDlg)
{
	char cDecryptedString[1000];
	char cTemp[10];

	ZeroMemory(cDecryptedString, sizeof(cDecryptedString));

	int iRealLength = strlen(cEncryptedSerial)/4;

	for(int i=0; i<iRealLength; i++)
	{
		BYTE p1, p2;

		//Extract first 4 digits
		ZeroMemory(cTemp, sizeof(cTemp));
		cTemp[0] = cEncryptedSerial[(i*4)];
		cTemp[1] = cEncryptedSerial[(i*4)+1];
		p1 = str2dw(cTemp);
		ZeroMemory(cTemp, sizeof(cTemp));
		cTemp[0] = cEncryptedSerial[(i*4)+2];
		cTemp[1] = cEncryptedSerial[(i*4)+3];
		p2 = str2dw(cTemp);

		p2 += i;
		p1 -= i;
		p2 ^= 0xDA;
		p1 += 0xFC;
		p2 -= 0x37;
		p1 ^= 0x7F;
		cDecryptedString[i] = p1 + p2;
	}

	SetWindowText(GetDlgItem(hDlg, IDC_EDIT3), cDecryptedString);
}
#endif

void GenerateSerial()
{
	char *cp;
	DWORD dwChecksum = 0;

	GenerateRandomString();

	int iLen = strlen((char *)pSample1);

	cp = (char *)pSample1;
	for(int i=0; i<iLen; i++)
	{
		BYTE p1 = *cp;
		BYTE p2 = p1 & 0x0F;
		p1 = p1 & 0xF0;
		p1 ^= 0x7F;
		p2 += 0x37;
		p1 -= 0xFC;
		p2 ^= 0xDA;
		p1 += i;
		p2 -= i;
		wsprintf(&pSolution1[strlen(pSolution1)], "%.2x%.2X", p1, p2);
		cp++;
	}
}

BOOL CALLBACK DialogProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam){
	switch(uMsg) {
	case WM_TIMER:
		GenerateSerial();
		SetWindowText(GetDlgItem(hDlg, IDC_EDIT1), (char *)pSample1);
		SetWindowText(GetDlgItem(hDlg, IDC_EDIT2), pSolution1);
#ifdef DEBUG
		ReverseGeneratedSerial(hDlg);
#endif
		KillTimer(hDlg, 0xC7F);
		break;

	case WM_CLOSE:
		GlobalFree(pSample1);
		EndDialog(hDlg, TRUE);
		break;

	case WM_INITDIALOG:
		SetTimer(hDlg, 0xC7F, 1, NULL);
		SetWindowText(GetDlgItem(hDlg, IDC_EDIT4), cEncryptedSerial);
		break;

		/*
	case WM_COMMAND:
		switch(LOWORD(wParam)) {
		case IDM_EXIT:
			EndDialog(hDlg, TRUE);
			break;
		}
		break;
	*/

	default:
		return FALSE;
	}
	return TRUE;
}

int WINAPI WinMain(HINSTANCE hInstance,	HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	srand(GetTickCount());
	DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, (DLGPROC)DialogProc);
	return 0;
}