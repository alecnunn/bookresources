#include <stdio.h>
#include "..\CD.h"
#include "..\SCSI.h"

#define	MAX_BUF_SIZE	0x100
main()
{
	int a;
	unsigned char buf[MAX_BUF_SIZE];
	unsigned char CDB[ATAPI_CDB_SIZE];

	memset(CDB, 0, ATAPI_CDB_SIZE);

	CDB[0] = 0x5A;
	CDB[2] = 0x2A;
	CDB[7] = HIBYTE(MAX_BUF_SIZE);
	CDB[8] = LOBYTE(MAX_BUF_SIZE);
	a = SEND_SCSI_CMD("TEAC", CDB, 	ATAPI_CDB_SIZE, NO_SENSE, buf, MAX_BUF_SIZE, SCSI_DATA_IN);

	if (a != SCSI_OK)
	{
		printf("-ERR: SCSI_SEND_CMD\n");
	}
	else
	printf("Multi-Session:%s\n", ((buf[4+8] & 1<<6)?"YES":"NO"));


}