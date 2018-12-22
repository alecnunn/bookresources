// recovers sector on the basis of RS codes

#include <stdio.h>
#include <windows.h>
#include "ElbyECC.h"

#define MAX_FIX_ERROR	52
#define OVER_ERROR		(MAX_FIX_ERROR+1)

#define N_ERROR     	MAX_FIX_ERROR
//stdcall GenECCAndEDC_Mode1(char *sector, int mode);

extern int __stdcall GenECCAndEDC_Mode1(char *userdata, int *header, char *sector);

#define x " ^^^^^^^^^^^^^^^^^^^^^^^ "
main(int argc, char **argv)
{
	int a;
	HANDLE h;
	FILE *f_src, *f_dst;
	unsigned char secheader[SECHEADER_SIZE];
	unsigned char rawsector[RAWSECTOR_SIZE];
	unsigned char user_data[USER_DATA_SIZE];

	int (__stdcall *GenECCAndEDC_Mode1)  	(char *userdata, char *header, char *sector);
	int (__stdcall *ReGenECCAndEDC_Mode1)	(char *sector); 
	int (__stdcall *CheckECCAndEDC_Mode1)  (char *userdata,  char *sector);

	h=LoadLibrary("ElbyECC.dll");
	if (h==0)
	{
		printf("-ERR:LoadLibrary(\"ElbyECC.dll\") are fault!\n");
		return -1;  
	}

	f_src = fopen("sector_src.dat","rb");
	if (f_src == 0) {
		fprintf(stderr, "-ERR: open scr file\x7\n"); return -1; }
	f_dst = fopen("sector_dst.dat","wb");
	if (f_dst == 0) {
		fprintf(stderr, "-ERR: open dst file\x7\n"); return -1; }

	fread(rawsector, 1, 2352, f_src);

	GenECCAndEDC_Mode1   = GetProcAddress(h,  "GenECCAndEDC_Mode1");
    ReGenECCAndEDC_Mode1 = GetProcAddress(h,  "ReGenECCAndEDC_Mode1");
	CheckECCAndEDC_Mode1 = GetProcAddress(h,  "CheckECCAndEDC_Mode1");

	
	// checking sector for integrity
	a = CheckECCAndEDC_Mode1(rawsector , CHECK_ONLY);
	if (a == SECTOR_OK)
		 printf("%sSECTOR OK%s\n",x,x); else printf("%sSECTOR ERROR%s\n",x,x);

	if (a == NEED_RECORVER)
	{
		if (argc < 4)
		a = CheckECCAndEDC_Mode1(rawsector,CHECK_AND_RECORVER);

		if (argc == 4)
		{
			//memcpy(secheader,rawsector,16);
			//memcpy(user_data,&rawsector[16],2048);
	        a = ReGenECCAndEDC_Mode1(rawsector);
	    }
	}

	if (a == SECTOR_OK)		
		printf("%sSECTOR WAS RECORVERED%s\n",x,x,print_sector(rawsector));
	else
		printf("%sSECTOR DAMAGED AND NOT RECORVER!%s\x7\n",x,x);

	fwrite(rawsector, 1, 2352, f_dst);
	fclose(f_dst);
}
