#include <stdio.h>
#include "..\CD.h"
#include "..\SCSI.h"



#define Media_Catalog_Number 2
main()
{
	int a;
	int len;
	int MCVAL;
	int SubChannel_Data_Format_Code;
	unsigned char *p;
	unsigned char BUF[MAX_TOC_SIZE];
	unsigned char CDB[ATAPI_CDB_SIZE];



	memset(CDB, 0, ATAPI_CDB_SIZE);

	CDB[0] = 0x42; // READ SUB
	CDB[2] = 1 << 6;
	CDB[3] = 0x2; // UPC
	CDB[6] = 0;
	CDB[7] = 0x1; // LEN
	a = SEND_SCSI_CMD("TEAC", CDB, ATAPI_CDB_SIZE, NO_SENSE, BUF, MAX_TOC_SIZE, SCSI_DATA_IN);
	if (a != SCSI_OK) {
		printf("-ERR: SENS SCSI CMD\n"); return -1;
	}

	len = BUF[2]*0x100L + BUF[3];

	if (len < 20) {
		printf("-ERR: non-standart Bar-Code or ISRC\x7\n");
		return -1;
	} len -= 2; // Subtracting own value from the length

	p = &BUF[4];	// To starting position

    SubChannel_Data_Format_Code = p[0];

    switch(SubChannel_Data_Format_Code)
    {
    	case Media_Catalog_Number:
    			MCVAL = p[4];
   				printf("Media Catalog Number (MCN):");
    			if (MCVAL & (1<<7))
    			{
    				for (a = 5; a < 18; a++) printf("%02Xh ",p[a]); 	printf("\n");
    			}
    				else
				{
					printf("NONE\n");
				}
				break;
    }

	for (a = 0; a < 100; a++) printf("%02Xh ",BUF[a]);


}
