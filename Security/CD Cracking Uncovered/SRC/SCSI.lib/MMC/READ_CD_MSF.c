#include <stdio.h>
#include "..\CD.h"
#include "..\SCSI.h"


#define DEF_MAX_TRY 3
READ_CD_MSF(char *buf, char *CD, int sector_type, int Ms, int Ss, int Fs, int Me, int Se, int Fe, int flags)
{
	int a;
	int count;
	unsigned char CDB[ATAPI_CDB_SIZE];
    memset(CDB, 0, ATAPI_CDB_SIZE); memset(buf, 0, RAW_SECTOR_SIZE);

	CDB[0] = 0xB9; // READ CD MSF
	CDB[1] = sector_type << 2;

	CDB[3] = Ms;
    CDB[4] = Ss;
    CDB[5] = Fs;

	CDB[6] = Me;
    CDB[7] = Se;
    CDB[8] = Fe;

    CDB[9] = flags;

    for (count = 0; count < DEF_MAX_TRY; count++)
    {
	    if (SCSI_BEGIN(CD)==SCSI_OK) 
	    {
		    a = SCSI_SEND_CMD(CD, CDB, ATAPI_CDB_SIZE, NO_SENSE, buf, RAW_SECTOR_SIZE, SCSI_DATA_IN);
		    if (a == SCSI_OK) return a;
		}
	}
	return ~SCSI_OK;
} 



#define DEF_CD	"TEAC"
#define DEF_FILE	"sector.dat"
#define argCD		((argc>1)?argv[1]:DEF_CD)

#define DEF_N_SEC	(75*4)
main(int argc, char **argv)
{
	int a, x;
	FILE *f;
	int Ms, Ss, Fs; int Me, Se, Fe;
	unsigned char BUF[RAW_SECTOR_SIZE];

	if (!(f = fopen(DEF_FILE, "wb")))
	{
		printf("-ERR: open file %s\x7\n",DEF_FILE);
		return -1;
	}

	for (x = 0; x < DEF_N_SEC; x++)
	{
		LBA0toMSF(&Ms, &Ss, &Fs, x);
        LBA0toMSF(&Me, &Se, &Fe, x+1);

		a = READ_CD_MSF(BUF, argCD, 0x0 /* any sector */, Ms, Ss, Fs, Me, Se, Fe, 0xF8 /* raw */);	
		printf("sector %02d:%02d:%02d read %s", Ms,Ss,Fs, (a==SCSI_OK)?"OK [ ":"ERROR\n");
		if (a == SCSI_OK)
		{
			for (a = 0; a < 16; a++) printf("%02X ",BUF[a]);  printf("]\n");
			fwrite(BUF, 1, RAW_SECTOR_SIZE, f);
		}       
	}
}
