/*----------------------------------------------------------------------------
 *
 *								LBA2MSF CONVERTER
 *								=================
 *
 *		Reports absolute (MSF) address of the sector by its LBA address.
 * Works only on CD-DATA, CDDA is not supported (no LBA addressing)
 * 
 *		NOTE: using ASPI with incorrect argument may freeze the system.
 *
 * build 0x001 @ 11.07.2003
----------------------------------------------------------------------------*/
#include <windows.h>
#include <stdio.h>

#include "CD.h"
#include "SPTI.h"
#include "ASPI32.h"

#define BUF_SIZE 100
#define _RADIX(a) (((a[strlen(a)-1]=='h')||(a[1]=='x'))?0x10:10)
main(int argc, char **argv)
{
	long a, b, stat;
	unsigned char buf[BUF_SIZE];
	unsigned char CDB[ATAPI_CDB_SIZE];
	unsigned char SENSE[MAX_SENSE_SZ];

	// иницилизируем буфера
	memset(CDB, 0, ATAPI_CDB_SIZE); memset(buf, 0, BUF_SIZE);
    memset(buf, 0x55, MAX_SENSE_SIZE);
	
	// определ€ем аргументы
	#define CD			argv[1]
	#define trg			atol(&argv[1][0])
	#define LUN			atol(&argv[1][2])
	#define isASPI		( ( (CD[0]!='\\') && ((trg+LUN)!=0) )?1:0 )
	#define startLBA 	strtol(argv[2], (char **) &buf, _RADIX(argv[2]))
	#define endLBA 		strtol(argv[3], (char **) &buf, _RADIX(argv[3]))

	if (argc<4)				
	{
		fprintf(stderr,	"USAGE:\n");
		fprintf(stderr,	"\tread.header.exe \\\\.\\X:\t\tstartLBA endLBA\n");
		fprintf(stderr,	"\tread.header.exe target.Lun\tstartLBA endLBA\n\n");
		fprintf(stderr,	"for example:\n");
		fprintf(stderr,	"\tread.header.exe \\\\.\\G: 0 669\n");
		fprintf(stderr,	"\tread.header.exe 1.1 0 669\n");
		return 0;
	}

	// TITLE
	fprintf(stderr,"READ HEADER (44h) SCSI/ATAPI commad demo by KK\n");

	// testing sectors one by one
	for (a = startLBA; a <= endLBA; a++)
	{
		// preparing CDB
		CDB[0] = READ_HEADER;

		CDB[1] = 0xFF; // <-- LBA2MSF

		CDB[2] = HIBYTE(HIWORD(a));
		CDB[3] = LOBYTE(HIWORD(a));
		CDB[4] = HIBYTE(LOWORD(a));
		CDB[5] = LOBYTE(LOWORD(a));

		CDB[6] = 0x0;
		CDB[7] = 0x4;
		CDB[8] = 0x2;

		if ((endLBA - startLBA)>=100)
			fprintf(stderr,"\r%3.1f left to process\r", 100.0 * ((float)(a-startLBA))/(endLBA-startLBA));

		if (isASPI)
	    	stat = SEND_ASPI_CMD(trg, LUN, CDB, ATAPI_CDB_SIZE, SENSE, buf, BUF_SIZE, 1);
		else
   			stat = SEND_SPTI_CMD(CD, CDB, ATAPI_CDB_SIZE, SENSE, buf, BUF_SIZE, 1);	

		if (stat == SS_COMP)
		{
			printf("LBA: %04Xh --> MSF:%02X %02X %02X (MODE - %x [%s])\n",
  			a, buf[5], buf[6], buf[7], buf[0],
  			(buf[0])?buf[0]==1?"L-EC symbols":(buf[0]==2)?"User Data":"unknown":"zero");
		}
			else
		{
			fprintf(stderr, "-ERR:\x7");
				for (b = 0; b < 20; b++) printf("%02x ",SENSE[b]); break;	
		}
	}
	return 0;
}