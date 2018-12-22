#include <windows.h>
#include <stdio.h>
#include "..\CD.h"
#include "..\SCSI.h"
#include "..\CDf.h"


#define DEF_FLAG	2	// RAW TOC

main(int argc, char** argv)
{
	#define argCD	(argv[1])
	#define argFLAG	((argc > 2)?(atol(argv[2])):DEF_FLAG)

	int a, b, len, char_per_line;
	unsigned char buf[0x1000];
	unsigned char CDB[ATAPI_CDB_SIZE];


	memset(buf, 0x55, 0x1000);
	memset(CDB, 0, ATAPI_CDB_SIZE);

	fprintf(stderr, "RAW TOC READER for SCSI/IDE CD-ROM by Kris Kaspersky\n");
	
	if (argc < 2)
	{
		fprintf(stderr, "USAGE:\n\tCD_RAW_TOC_READ.exe CD [flag]\n");	
		fprintf(stderr, " flag | description\n");
        fprintf(stderr, "------+-------------------------------------\n");
        fprintf(stderr, "   0  | TOC\n");
		fprintf(stderr, "   1  | Session Information\n");
		fprintf(stderr, "   2  | Full TOC\n");
		fprintf(stderr, "   3  | PMA\n");
		fprintf(stderr, "   4  | ATIP\n");
   		fprintf(stderr, "   5  | CD-Text\n");
		return 0;
	}


	a = cd_raw_toc_read(argCD,buf, 0x1000, argFLAG);
	if (a == SCSI_OK)
	{
		printf("* * * TOC * * *\n");
		
		switch(LOBYTE(argFLAG))
		{
			case 0  :  char_per_line = 8;  break;
			case 1  :  char_per_line = 8;  break;
			case 2  :  char_per_line = 11;
					   printf("session number\n");
					   printf(" | ADR/control\n");
					   printf(" |  | TNO\n");
					   printf(" |  |  | point\n");
					   printf(" |  |  |  | AM:AS:AF\n");
					   printf(" |  |  |  |  |  |  | zero\n");
					   printf(" |  |  |  |  |  |  |  | PM:PS:PF\n");
					   break;					
			case 3  :  char_per_line = 11; break;
			case 4  :  char_per_line = 24; break;
	  		case 5  :  char_per_line = 18; break;
			default :  char_per_line = 8;
		}

		len = buf[0]*0x100L + buf[1];
		printf("len = %X\n",len);
		for (a = 4; a < len; a += char_per_line)
		{
			for (b=0;  b < char_per_line; b++)
			{
				printf("%02X ",buf[a+b]);
			} printf("\n");
		}
	}
		else
	{
		printf("-ERR:....\x7\n");
		//return -1;
	}

	printf("\n* * * DISC INFO * * *\n");
	CDB[0] = 0x51; // READ DISC INFORMATION

	CDB[7] = HIBYTE(0x1000);
	CDB[8] = LOBYTE(0x1000);
	a = SEND_SCSI_CMD(argCD, CDB, ATAPI_CDB_SIZE, NO_SENSE, buf, 0x1000, SCSI_DATA_IN);
	if (a == SCSI_OK)
	{
		len = buf[0]*0x100L + buf[1];
		for (a = 2; a < len; a++)
		{
			printf("%02X ",buf[a]);

		}
	}


}
