#include <windows.h>
#include <stdio.h>
#include "CD.h"
#include "SPTI.h"
#include "ASPI32.h"

main()
{
	int a;
	unsigned char CDB[ATAPI_CDB_SIZE];
	unsigned char SENSE[MAX_SENSE_SIZE];

	memset(CDB, 0, ATAPI_CDB_SIZE);

	CDB[0] = 0xB4; //PLAY CD
	CDB[1] = 2;
	CDB[3] = 0;
	CDB[4] = 2;
	CDB[5] = 0;
	CDB[6] = 2;
	CDB[7] = 2;
	CDB[8] = 2;
	CDB[9] = 1;
	a = SEND_SCSI_CMD("1.1", CDB, ATAPI_CDB_SIZE, SENSE, CDB, 10, SCSI_DATA_IN);
	if (a != SCSI_OK)
	{
		for (a = 0; a < 22; a++) printf("%02X ", SENSE[a]);
	}


}