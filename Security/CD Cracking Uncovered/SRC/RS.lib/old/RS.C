#include <stdio.h>
#include <windows.h>
#include "ElbyECC.h"

#define MAX_FIX_ERROR	52
#define OVER_ERROR		(MAX_FIX_ERROR+1)

#define N_ERROR     	MAX_FIX_ERROR
//stdcall GenECCAndEDC_Mode1(char *sector, int mode);

extern int __stdcall GenECCAndEDC_Mode1(char *userdata, int *header, char *sector);

#define x " ^^^^^^^^^^^^^^^^^^^^^^^ "
main()
{
	int a;
	HANDLE h;
	unsigned char secheader[SECHEADER_SIZE];
	unsigned char rawsector[RAWSECTOR_SIZE];
	unsigned char user_data[USER_DATA_SIZE];
	int (__stdcall *GenECCAndEDC_Mode1)  (char *userdata, char *header, char *sector);
	int (__stdcall *ReGenECCAndEDC_Mode1)(char *sector); 
	int (__stdcall *CheckECCAndEDC_Mode1)  (char *userdata,  char *sector);

	h=LoadLibrary("ElbyECC.dll");
	if (h==0)
	{
		printf("-ERR:LoadLibrary(\"ElbyECC.dll\") are fault!\n");
		return -1;  
	}

	GenECCAndEDC_Mode1 = GetProcAddress(h,  "GenECCAndEDC_Mode1");
    ReGenECCAndEDC_Mode1 = GetProcAddress(h, "ReGenECCAndEDC_Mode1");
	CheckECCAndEDC_Mode1 =GetProcAddress(h,  "CheckECCAndEDC_Mode1");
	memset(user_data, 0x96, USER_DATA_SIZE);

	GenECCAndEDC_Mode1(user_data, secheader, rawsector);

	//memset(&rawsector[pUSER_DATA],0,N_ERROR);
	// sector corruption

	print_sector(rawsector);

	// checking sector for integrity
	a = CheckECCAndEDC_Mode1(rawsector , CHECK_ONLY);
	if (a == SECTOR_OK)
		 printf("%sSECTOR OK%s\n",x,x); else printf("%sSECTOR ERROR%s\n",x,x);

	if (a == NEED_RECORVER)
		a = CheckECCAndEDC_Mode1(rawsector,CHECK_AND_RECORVER);

	if (a == SECTOR_OK)		
		printf("%sSECTOR WAS RECORVERED%s\n",x,x,print_sector(rawsector));
	else
		printf("%sSECTOR DAMAGED AND NOT RECORVER!%s\x7\n",x,x);

    // ReGenECCAndEDC_Mode1(rawsector);

}
