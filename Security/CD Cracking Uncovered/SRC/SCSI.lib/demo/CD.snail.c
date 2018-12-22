/*----------------------------------------------------------------------------
 *
 *								SLOWS DOWN CD
 *								=============
 *
 *		This program is intended to slow down or speed up CD drives.
 * The speed can be specified both in Kbytes per second and in "x"
 * (if lower than 176, it is always interpreted as "x"). If the specified
 * speed is zero, the program displays the list of speeds supported by
 * this drive.
 *
 * build 0x000 
----------------------------------------------------------------------------*/
#include <windows.h>
#include <stdio.h>

#include "..\CD.h"
#include "..\SCSI.h"

#define BUF_SIZE 100

#define MAX_TRY_SET_SPEED	16
get_cur_speed(char *cd)
{
	long a, b, stat;
	long mode_len;
	long my_try = 0;

	unsigned char buf[BUF_SIZE];
	unsigned char CDB[ATAPI_CDB_SIZE];
	unsigned char SENSE[SENSE_SIZE];
	memset(CDB, 0, ATAPI_CDB_SIZE); memset(buf, 0, BUF_SIZE);

	
	// preparing CDB
	CDB[0] = MODE_SENSE;
	CDB[2] = 0x2A;

	CDB[7] = HIBYTE(BUF_SIZE);
	CDB[8] = LOBYTE(BUF_SIZE);
	while(1)
	{
		stat = SEND_SCSI_CMD(cd, CDB, ATAPI_CDB_SIZE, SENSE, buf, BUF_SIZE, SCSI_DATA_IN);

		if (stat == SCSI_OK)
		{
		
		    mode_len = buf[0]; mode_len = mode_len*0x100+buf[1];
			if (mode_len < 20){fprintf(stderr,"-ERR:MODE_SENSE\x7\n");return -1;}
			return buf[14+8]*0x100L+buf[15+8];
		}
			else
		{
        	if (my_try++ == MAX_TRY_SET_SPEED) return -1; else 	Sleep(100);
		}	
	}
	return 0;	
}

set_cur_speed(char *cd, int speed)
{
	long a, b, stat;
	long mode_len;
	long my_try=0;

	unsigned char buf[BUF_SIZE];
	unsigned char CDB[ATAPI_CDB_SIZE];
	unsigned char SENSE[SENSE_SIZE];
	memset(CDB, 0, ATAPI_CDB_SIZE); memset(buf, 0, BUF_SIZE);
	
	// preparing CDB 
	CDB[0] = 0xBB;
	CDB[2] = HIBYTE(speed);
	CDB[3] = LOBYTE(speed);
	//printf("%s %x\n",cd, speed);
	
	while(1)
	{
		stat = SEND_SCSI_CMD(cd, CDB, ATAPI_CDB_SIZE, SENSE, buf, BUF_SIZE, SCSI_DATA_IN);
		if (stat == SS_COMP)
		{
			CDB[0] = 0xA8;
			CDB[2] = 0x0;
			CDB[3] = 0x0;
			CDB[9] = 0x0;
			if (SCSI_BEGIN(cd) == SCSI_OK)
				SEND_SCSI_CMD(cd, CDB, ATAPI_CDB_SIZE, SENSE, buf, BUF_SIZE, SCSI_DATA_IN);
			return 1;
		}
			else
		{
        	if (my_try++ == MAX_TRY_SET_SPEED)
        	{
        		printf("\x7");
        	 return -1;
        	 }
        	 Sleep(100);
		}
	}
	return 0;	

}

#define MIN_SPEED	1
#define MAX_SPEED	128
list_speed(char *cd)
{
	int a, b, c;

	for (a=MIN_SPEED; a <= MAX_SPEED; a++)
	{
		set_cur_speed(cd, a * 177);
		if (get_cur_speed(cd)/176 == a)
			printf("%d ",a);
	}
	printf("\n");
}

main(int argc, char **argv)
{
	long a, b, stat;
	long mode_len;
	unsigned char buf[BUF_SIZE];
	unsigned char CDB[ATAPI_CDB_SIZE];
	unsigned char SENSE[SENSE_SIZE];

	// Initializing buffer
	memset(CDB, 0, ATAPI_CDB_SIZE); memset(buf, 0, BUF_SIZE);
	
	// Defining arguments
	#define CD			argv[1]
	#define SPEED		atol(argv[2])
	#define GETONLY		(!(argv[2][0]>='0' && argv[2][0]<='9'))

	//#define isASPI		( ( (CD[0]!='\\') && ((trg+LUN)!=0) )?1:0 )

	if (argc == 2)
		return SCSI_SEND_CMD(ASPI_BUS_SCAN);


	if (argc<3)
	{
		fprintf(stderr,	"USAGE:\n");
		fprintf(stderr,	"\tCD.snail.exe \\\\.\\X: speed\n");
		fprintf(stderr,	"\tCD.snail.exe target.Lun speed\n");
		fprintf(stderr,	"\tspeed in x or Kbps, speed == 0: print supports speeds\n\n");
		fprintf(stderr,	"for example:\n");
		fprintf(stderr,	"\tCD.snail.exe \\\\.\\G: 16\n");
		fprintf(stderr,	"\tCD.snail.exe 1.1 176\n");
		return 0;
	}

	// TITLE
	fprintf(stderr,"cd snail for SCSI/ATAPI CD-ROM by Kris Kaspersky\n");


	
	//		buf[14+8]*0x100+buf[15+8], (buf[14+8]*0x100+buf[15+8])/176);

   	printf("current speed: %d Kbps (x%d); ",
				   	get_cur_speed(CD), get_cur_speed(CD)/176);
	
	if (GETONLY) return 1;
	if (SPEED)
	{
		printf("try to x%d; ",(SPEED<176)?SPEED:SPEED/176);

		set_cur_speed(CD, (SPEED<176)?SPEED*177:SPEED);
	   	printf("new speed: %d Kbps (x%d)\n",
					   	get_cur_speed(CD), get_cur_speed(CD)/176);
	}
	else
		list_speed(CD);
		
	return 0;
}