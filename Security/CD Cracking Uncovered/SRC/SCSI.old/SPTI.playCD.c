/*----------------------------------------------------------------------------
 *
 *				SIMPLE AUDIO PLAYER. PLAYS AUDIO VIA SPTI
 *				============================================
 *
 * build 0x001 @ 27.05.2003
----------------------------------------------------------------------------*/

#include <stdio.h>
#include "SPTI.h"

main(int argc, char **argv)
{
	int a;
	FILE *f;
	char *buf;
    char SENSE[200]; char buff[400];
	char SRB[ATAPI_SRB_SIZE]; memset(SRB, 0, ATAPI_SRB_SIZE);
	memset(SENSE, 'U', 200);	memset(buff, 'U', 400);

	


	#define CD		argv[1]
	#define RESUME	atol(argv[2])
	#define startM  atol(argv[2])
	#define startS	atol(argv[3])
	#define startF	atol(argv[4])
	#define endM	atol(argv[5])
	#define endS	atol(argv[6])
	#define endF	atol(argv[7])
/*
	if (argc<3)
	{
		fprintf(stderr,	"USAGE: SPTI.play.exe \\\\.\\X: startM startS startF "\
						"endM endS endF\n(note: \"SPTI.play.exe \\\\.\\X: 0|1"\
						"will PAUSE/RESUME AUDIO\n");  return 0;
	}
	
	if (argc<4)
	{
		SRB[0] = 0x4B;	// PAUSE/RESUME AUDIO
		SRB[8] = RESUME;
	} 	else

	if (argc == 8)
	{
*/
		SRB[0] = 0x47; //0xB4;	// PLAY CD

		SRB[1] = 0; //(1<<1);

		SRB[2] = 0;
		
		SRB[3] = 0x22; //startM; SRB[4] = startS; SRB[5] = startF;
		SRB[4] = 0x2;//
		SRB[5] = 0x0; //

		SRB[6] = 0x10;
		SRB[7] = 0x10;
		SRB[8] = 0x7;

		SRB[9] = 0;
		SRB[10] =  0;
		SRB[11] = 0x0;

//	} else { fprintf(stderr, "-ERR in arguments\n"); return -1;}

	SEND_SPTI_CMD(CD, SRB, ATAPI_SRB_SIZE, 70, 30,  buff, 300, 1, &a);

	//memset(SRB, 0, 12); SRB[0] = 0x3; SRB[4] = 0x20;
	//SEND_SPTI_CMD(CD, SRB, ATAPI_SRB_SIZE, 50, 0x66,  buff, 0x66, 1);

	for (a = 0; a < 100; a++)
	{
		printf("%02x ",(unsigned char)buff[a+70]);
	}

	return 0;
}                             