#include <stdio.h>
#include "..\CD.h"
#include "..\SCSI.h"

#define pregap_size		150
#define postgap_size	150
GetTrackLen(int N, unsigned char *TOC)
{
	int a;
	int len;
	struct CD_TOC *p, *p_next;

	len = TOC[0]*0x100L + TOC[1] + 2;
	TOC += 4; // skipping TOC length and First/Last Track Num fields

	for (a = 0; a < len; a += sizeof(struct CD_TOC))
	{
		p 		= (struct CD_TOC*) &TOC[a];
		p_next	= (struct CD_TOC*) &TOC[a + sizeof(struct CD_TOC)];

		if ((p->track_number == N) && ((len - a) > sizeof(struct CD_TOC)))
		{
			return  (RAW_LBAtoLBA(p_next->ADDR) - RAW_LBAtoLBA(p->ADDR) -pregap_size - postgap_size);
			//tracklen.c 
		}
	}

	return -1;
		
}


main()
{
	int a;
	unsigned char BUF[MAX_TOC_SIZE];
	unsigned char CDB[ATAPI_CDB_SIZE];

	memset(CDB, 0, ATAPI_CDB_SIZE);
	CDB[0] = 0x43; // READ TOC
	CDB[7] = HIBYTE(MAX_TOC_SIZE);
    CDB[8] = LOBYTE(MAX_TOC_SIZE);

    a = SCSI_SEND_CMD("TEAC", CDB, ATAPI_CDB_SIZE, NO_SENSE, BUF, MAX_TOC_SIZE, SCSI_DATA_IN);
	printf("%d \n", GetTrackLen(1, BUF));




}