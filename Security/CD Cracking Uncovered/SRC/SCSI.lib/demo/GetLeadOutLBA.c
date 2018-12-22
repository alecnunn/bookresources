#include <stdio.h>
#include "..\CD.h"
#include "..\SCSI.h"

main(int argc, char **argv)
{
	int a;
	int TOC_SIZE;
	unsigned char TOC[MAX_TOC_SIZE];
	unsigned char CDB[ATAPI_CDB_SIZE];
	memset(CDB, 0, ATAPI_CDB_SIZE);
	#define LEN_FI_LEN		2
	#define FIRT_TOC_EL		4
	#define TOC_FI_SIZE		8
	#define __LEAD_OUT__	0xAA

	#define argCD	((argc>1)?argv[1]:0)

	if (!argCD)
	{
		printf("USAGE: GetLeadOutLBA CD\n");
		return -1;
	}


	CDB[0] = 0x43;
	CDB[7] = 1;	

	a = SCSI_SEND_CMD(argCD,CDB, ATAPI_CDB_SIZE, NO_SENSE, TOC, MAX_TOC_SIZE, SCSI_DATA_IN);
	if (a == SCSI_OK)
	{
		TOC_SIZE = TOC[0]*0x100L + TOC[1] + LEN_FI_LEN;
		for (a = FIRT_TOC_EL; a < TOC_SIZE; a += TOC_FI_SIZE)
			if (TOC[a+2] == __LEAD_OUT__)
				printf("Lead-Out LBA - %d\n",
					TOC[a+4+3] + TOC[a+4+2]*0x100L + TOC[a+4+1]*0x10000L + TOC[a+4+0]*0x1000000L);

		
	}
	else
	{
		printf("-ERR READ_TOC\n");
		return -1;
	}


	return 1;

}