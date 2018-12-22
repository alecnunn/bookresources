#include <stdio.h>
#include "..\CD.h"
#include "..\SCSI.h"
#include "crackme.N1.h"

#define MAX_ERROR_SN	3
#define TOC_POINTER_SIZE   11

#define _FLAG_C0_WAS_READ	1
#define _FLAG_C1_WAS_READ	2

#define _ERROR				-1
#define _HELLO_HACKER		0
#define _HELLO_LEGAL_USER	1
#define _NEED_SKIP			0x69

int _DEBUG_PRINT = 0;

// Read pointers C0 and C1
READ_C1(char *CD, char *C0, char *C1)
{
	int a, b, len;
	int flag_was_read = 0;

	unsigned char BUF[MAX_TOC_SIZE];
	unsigned char CDB[ATAPI_CDB_SIZE];

	if (_DEBUG_PRINT) printf("# READ_C1(%s, %s, %s) called\n",CD, C0, C1);

	memset(CDB, 0, ATAPI_CDB_SIZE);

	CDB[0] = 0x43;	// READ TOC
	CDB[2] = 2;		// RAW TOC
	CDB[6] = 0;
	CDB[7] = HIBYTE(MAX_TOC_SIZE);
	CDB[8] = LOBYTE(MAX_TOC_SIZE);

	if (_DEBUG_PRINT) printf("# call SCSI_BEGIN(%s)\n",CD);			
	a = SCSI_BEGIN(CD);
	if (a == SCSI_NEED_SKIP) return _NEED_SKIP; else if (a != SCSI_OK) return _ERROR;

	if (_DEBUG_PRINT) printf("# call SEND_SCSI_CMD(...)\n");
	a = SEND_SCSI_CMD(CD, CDB, ATAPI_CDB_SIZE, NO_SENSE, BUF, MAX_TOC_SIZE, SCSI_DATA_IN);
	if (a != SCSI_OK) return _ERROR;

	len = BUF[0]*0x100L + BUF[1]; // determine the length
	len += 2; // full TOC length including the header
	if (_DEBUG_PRINT) printf("# TOC len == %d\n",len);

	for ( a = 4;  a < len; a += TOC_POINTER_SIZE)
	{
		if (_DEBUG_PRINT) printf("# %02X\n", BUF[a + 3]);
		switch(BUF[a + 3])
		{
			case 0xC0:	if (_DEBUG_PRINT) { printf("# C0:"); for (b = 0; b < TOC_POINTER_SIZE; b++) printf("%02X ",BUF[a + b]); printf("\n"); }
                        if (C0) memcpy(C0, &BUF[a], TOC_POINTER_SIZE);
						flag_was_read |= _FLAG_C0_WAS_READ; break;

			case 0xC1:  if (_DEBUG_PRINT) { printf("# C1:"); for (b = 0; b < TOC_POINTER_SIZE; b++) printf("%02X ",BUF[a + b]); printf("\n");}
						if (C1 )memcpy(C1, &BUF[a], TOC_POINTER_SIZE);
			            flag_was_read |= _FLAG_C1_WAS_READ; break;
		}
	}

	if ( (flag_was_read != (_FLAG_C0_WAS_READ | _FLAG_C1_WAS_READ))) return _HELLO_HACKER;

	return _HELLO_LEGAL_USER;
}

CRYPT(unsigned char *C0, unsigned char *C1)
{
	int a;
	for (a = 0; a < TOC_POINTER_SIZE; a++) C0[a] ^= (C1[a] + 1);
}


BIN2ASCII(unsigned char *dst, unsigned char *src)
{
	int a;
	for (a = 0; a < TOC_POINTER_SIZE; a++)
	{
		sprintf(dst,"%02X ", *src); dst += 3; src++;
	}
}

ASCII2BIN(unsigned char *dst, unsigned char *src)
{
	int a;
	char *stop;

	for (a = 0; a < TOC_POINTER_SIZE; a++)
	{
		if (*src < 0x20) return  _ERROR;
		*dst++ = strtoul(src, &stop,0x10); src=stop;
	}
}

#define argCD	(argv[1])
#define argDEBUG	((argc>2)?strcmp(argv[2], "-debug")?0:1:0)
main(int argc, char** argv)
{

	int a;
	unsigned char buf[100000];
	unsigned  char buf1[100000];
	unsigned  char buf2[100000];

	printf(sTITLE);

	if (argc < 2) {
		printf("USAGE: crackme.N1.exe CD\n");
		printf(sPRESS);
		return 0;

	}

	_DEBUG_PRINT = argDEBUG;

	printf(sCHECKMARK);
	a = READ_C1(argv[1], buf, buf1);
	if (a == _NEED_SKIP) 	{ printf(sPRESS); return -1;}
	if (a == _ERROR)		{ printf(sERROR);	return -1; }
	if (a == _HELLO_HACKER)	{ printf(sHACKER);	return -1; }
	printf(sOK);

    CRYPT(buf, buf1);  BIN2ASCII(buf1, buf);
    if (_DEBUG_PRINT) printf("# SN:%s\n",buf1);
 
    for (a = 1; a <= MAX_ERROR_SN; a++)
    {
	    printf(sENTER); fgets(buf, 10000, stdin);
	    ASCII2BIN(buf2, buf); BIN2ASCII(buf, buf2);
	    if (_DEBUG_PRINT)  printf("entered SN: %s\n",buf);
	 	if (memcmp(buf, buf1, TOC_POINTER_SIZE))
	 	{
	 		printf(sNOT);
 			printf(sTRY);
		}
			else
		{
			printf(sYES);
			break;
		}
		if (a == MAX_ERROR_SN)
		{
			printf(sHACK_SN);
			printf(sPRESS);
			return -1;
		}

	}

	printf(sLEGAL);		

	printf(sPRESS);	
}
