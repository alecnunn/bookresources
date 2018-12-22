#include <stdio.h>
#include "..\CD.h"
#include "..\SCSI.h"

#define	MAX_BUF_SIZE	0x100
main()
{
	int a;
	unsigned char buf[MAX_BUF_SIZE];
	unsigned char CDB[ATAPI_CDB_SIZE];

	#define T_LBA	0000
	#define A_LBA	0000

	memset(CDB, 0, ATAPI_CDB_SIZE);

	CDB[0] = 0xA7; // SET READ AHEAD
	CDB[2] = HIBYTE(HIWORD(T_LBA));
	CDB[3] = LOBYTE(HIWORD(T_LBA));
    CDB[4] = HIBYTE(LOWORD(A_LBA));
    CDB[5] = LOBYTE(LOWORD(A_LBA));
	a = SEND_SCSI_CMD("TEAC", CDB, 	ATAPI_CDB_SIZE, NO_SENSE, buf, MAX_BUF_SIZE, SCSI_DATA_IN);

	if (a != SCSI_OK)
	{
		printf("-ERR: SCSI_SEND_CMD\n");
	}

}