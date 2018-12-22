/*----------------------------------------------------------------------------
 *
 *							SUBCODE
 *
 *		Positions to the specified LBA and reads subcode channel
 *
----------------------------------------------------------------------------*/
#include <windows.h>
#include <stdio.h>
#include "..\CD.h"
#include "..\SCSI.h"

//#include "CDf.h"

#define MAX_BUF_SIZE (10*1024)

main(int argc, char** argv)
{
	int  a, x;
	//long LBA;
	unsigned char buf[MAX_BUF_SIZE];
	unsigned char CDB[ATAPI_CDB_SIZE];
	unsigned char SENSE[SENSE_SIZE];

	memset(CDB, 0, ATAPI_CDB_SIZE);
	memset(SENSE, 0x55, SENSE_SIZE);

	#define CD	argv[1]
	#define LBA	atol(argv[2])
	#define argLBAmax ((argc > 3)?atol(argv[3]):LBA)

	fprintf(stderr,"seek CD-ROM & read Q-subcode by KK\n");
	if (argc <2)
	{
		fprintf(stderr,"USAGE seek_and_Q.exe CD LBA [LBAmax]\n");
		SEND_SCSI_CMD("?.?", 0, 0, 0, 0, 0, 0);
		return 0;
	}


	memset(CDB, 0, ATAPI_CDB_SIZE);
	CDB[0] = 0x42;	// READ SUBCODE CD
	CDB[2] = 1<<6;
	CDB[3] = 1;
	CDB[6] = 0;
	CDB[7] = HIBYTE(LOWORD(MAX_BUF_SIZE));
	CDB[8] = LOBYTE(LOWORD(MAX_BUF_SIZE));
    memset(buf, 0x00, MAX_BUF_SIZE);
	a = SEND_SCSI_CMD(CD, CDB, ATAPI_CDB_SIZE, NO_SENSE, buf, MAX_BUF_SIZE, SCSI_DATA_IN);
	if (a!=SCSI_OK)
	{
		fprintf(stderr,"-ERR:error READ SUBCODE\n"); return -1;
	}
	for(a = 0;  a < 16; a++) printf("%02X ",buf[a]); printf("\n");
	//printf("AUDIO STATUS: %x\n",buf[17]);
	//for (a=20; a < 20+buf[18]*0x100L+buf[19]; a++) printf("%02X ",buf[a]); printf("\n");


}