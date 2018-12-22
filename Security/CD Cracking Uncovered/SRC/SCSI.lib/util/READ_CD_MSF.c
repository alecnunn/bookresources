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
    CDB[10] = 0x1; // RAW SUB

    for (count = 0; count < DEF_MAX_TRY; count++)
    {
	    if (SCSI_BEGIN(CD)==SCSI_OK) 
	    {
		    a = SCSI_SEND_CMD(CD, CDB, ATAPI_CDB_SIZE, NO_SENSE, buf, RAW_SECTOR_SIZE + 96, SCSI_DATA_IN);
		    if (a == SCSI_OK) return a;
		}
	}
	return ~SCSI_OK;
} 



#define DEF_CD	"TEAC"
#define DEF_FILE	"sector.dat"
#define argCD		((argc>1)?argv[1]:DEF_CD)

#define DEF_N_SEC	(75*4)


#define DEF_BLOCK_SIZE	1

main(int argc, char **argv)
{

	int a, x;
	FILE *f,*f_sub;
	int  BLOCK_SIZE = DEF_BLOCK_SIZE;
	unsigned char ZERO16[16];
	RLBA LBA_FROM, LBA_TO;
	int F_EOL	= 1;
	int F_Qimit = 0;


	int Ms, Ss, Fs; int Me, Se, Fe;
	unsigned char BUF[RAW_SECTOR_SIZE + 96];
	memset(BUF, 0, RAW_SECTOR_SIZE + 96);
	memset(ZERO16, 0, 16);

	LBA_FROM = 0; LBA_TO = 150;

	if (!(f = fopen(DEF_FILE, "wb")) || !(f_sub = fopen("sector.sub","wb")))
	{
		printf("-ERR: open file %s\x7\n",DEF_FILE);
		return -1;
	}

	for (x = LBA_FROM; x < LBA_TO; x += BLOCK_SIZE)
	{
		LBAtoMSF(&Ms, &Ss, &Fs, x);
        LBAtoMSF(&Me, &Se, &Fe, ((LBA_TO - x) > BLOCK_SIZE) ? x + BLOCK_SIZE:LBA_TO);

		printf("\rread %02d:%02d:%02d/%02d:%02d:%02d ",  Ms,Ss,Fs, Me, Se, Fe); 
		a = READ_CD_MSF(BUF, argCD, 0x0 /* any sector */, Ms, Ss, Fs, Me, Se, Fe, 0xF8 /* raw */);	

		printf("%s", (a==SCSI_OK)?"ok [ ":"error\n\t\t\t\t\t");
		if (a == SCSI_OK) {
			for (a = 0; a < 16; a++) printf("%02X ",BUF[a]);  printf("]");
			fwrite(BUF, 1, RAW_SECTOR_SIZE, f);
			if (F_Qimit == 1)
			{
				fwrite(ZERO16, 1, 16, f_sub);
				fwrite(&BUF[RAW_SECTOR_SIZE], 1, 96 - 16, f_sub);
			}
			else
			{
				fwrite(&BUF[RAW_SECTOR_SIZE], 1, 96, f_sub);
			}
		}       
	}
}
