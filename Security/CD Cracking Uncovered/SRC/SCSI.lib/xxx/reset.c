#include <stdio.h>
#include "..\CD.h"
#include "..\SCSI.h"

main()
{
	unsigned char CDB[ATAPI_CDB_SIZE];
	memset(CDB,0,ATAPI_CDB_SIZE);

	SEND_SCSI_CMD("TEAC", CDB, ATAPI_CDB_SIZE, NO_SENSE, 0, 0, SCSI_DATA_IN);

}
