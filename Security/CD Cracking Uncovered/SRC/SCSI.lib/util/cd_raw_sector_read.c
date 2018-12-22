#include <windows.h>
#include <stdio.h>
#include "..\CD.h"
#include "..\SCSI.h"



#define	argCD		(argv[1])
#define argLBA		(atol(argv[2]))
#define argN		(atol(argv[3]))
#define FILE_NAME	"cd_raw_sector.dat"

main(int argc, char **argv)
{
	int a;
	FILE *f;
	long Start_Sec;
	unsigned long N_Sec;
		unsigned long stuff_size = 2352;
	int flags = 0xF8;
	unsigned char CDB[ATAPI_CDB_SIZE];
	unsigned char SENSE[SENSE_SIZE];
    unsigned char *buf;

	fprintf(stderr, "RAW SECTOR READER for SCSI/IDE CD-ROM by Kris Kaspersky\n");
	
	if (argc < 4)
	{
		printf("USAGE:\n\tcd_raw_sector_read.exe CD start_sec num_sec\n\n");
		printf("       SCSI_INQUITY via ASPI32\n");
		printf("-------------------------------------\n");
		SEND_SCSI_CMD("?.?", 0, 0, 0, 0, 0, 0);
		return 0;
	}

	Start_Sec = argLBA;
	N_Sec = argN;
	if ((N_Sec * stuff_size) > 64*1024)
	{
		fprintf(stderr,"-ERR: too big N_SEC\x7\n");
		return -1;

	} 
	buf = malloc(N_Sec * stuff_size * 2);

	memset(CDB, 0, ATAPI_CDB_SIZE); memset(buf, 0x0, N_Sec * stuff_size*2);
	memset(SENSE, 0x55, SENSE_SIZE);

	CDB[0] = 0xBE; // READ CD

	CDB[1] = 0x0; // any disk 

	CDB[2] = HIBYTE(HIWORD(Start_Sec));
	CDB[3] = LOBYTE(HIWORD(Start_Sec));
	CDB[4] = HIBYTE(LOWORD(Start_Sec));
	CDB[5] = LOBYTE(LOWORD(Start_Sec));

	CDB[6] = LOBYTE(HIWORD(N_Sec));
	CDB[7] = HIBYTE(LOWORD(N_Sec));
	CDB[8] = LOBYTE(LOWORD(N_Sec));

	CDB[9] = flags;
	printf("try to read sector from %d to %d in %s...",Start_Sec, Start_Sec+N_Sec-1, FILE_NAME);
	a = SEND_SCSI_CMD(argCD, CDB, ATAPI_CDB_SIZE, SENSE, buf, N_Sec * stuff_size, SCSI_DATA_IN);
	if (a != SCSI_OK)
	{
		printf("-ERROR\nSENSE:");
		for (a = 0; a < 14; a++) printf("%02X ",SENSE[a]); printf("\x7\n");
	}
		else
	{
		f = fopen(FILE_NAME, "wb");
		if (f == 0)
		{
			printf("-ERR: open file %s\n",FILE_NAME);
			return -1;
		}
		fwrite(buf, 1, N_Sec * stuff_size, f);
		fclose(f);
   		printf("+OK\n");
		return 1;
	}
	return -1;
}

