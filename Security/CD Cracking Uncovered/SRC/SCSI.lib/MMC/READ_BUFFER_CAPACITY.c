#include <stdio.h>
#include "..\CD.h"
#include "..\SCSI.h"

#define	MAX_BUF_SIZE	0x100
main()
{
	int a;
	unsigned char buf[MAX_BUF_SIZE];
	unsigned char CDB[ATAPI_CDB_SIZE];
    unsigned char X1[8];
	int *x1 = &X1[0];
	int *x2 = &X1[4];
	


	memset(CDB, 0, ATAPI_CDB_SIZE);

	CDB[0] = 0x5C; // READ BUFFER CAPACITY
    CDB[7] = HIBYTE(MAX_BUF_SIZE);
    CDB[8] = LOBYTE(MAX_BUF_SIZE);
	a = SEND_SCSI_CMD("TEAC", CDB, 	ATAPI_CDB_SIZE, NO_SENSE, buf, MAX_BUF_SIZE, SCSI_DATA_IN);

	if (a != SCSI_OK)
	{
		printf("-ERR: SCSI_SEND_CMD\n");
	}
	for (a = 0; a < 8; a++) X1[a] = buf[11-a];

	printf("%d \n%d\n", *x1/1024, *x2/1024);

}