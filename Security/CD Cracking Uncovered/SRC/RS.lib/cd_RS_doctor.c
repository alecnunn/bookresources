#include <string.h>
#include "ElByECC.h"

#define arg_sector_in	((argc>LAST_NOT_KEY)  ?argv[LAST_NOT_KEY]    :"_sector_in.dat")
#define arg_sector_out	((argc>LAST_NOT_KEY+1)?argv[LAST_NOT_KEY + 1]:"_sector_out.dat")

#define my_CHK	1
#define my_FIX  2
#define my_GEN  4
#define my_FRC  8      // force MODE
#define my_OUT  16

#define key_FIX "/F"
#define key_CHK "/C"
#define key_GEN "/R"

#define key_MODE1 		"-MODE1"		// FORCE MODE1
#define key_MODE2FORM1  "-MODE2FORM1"		// FORCE MODE2FORM1
main(int argc, char **argv)
{
	int 	a;
	int		LAST_NOT_KEY;
	int		my_DO;
	int		xMODE;
	FILE	*sector_in, *sector_out;
	struct	RAW_SECTOR raw_sector_in;
    struct	RAW_SECTOR raw_sector_out;

    LAST_NOT_KEY = 1;

    fprintf(stderr,"cd RS doctor ver 1.0 by Kris Kaspersky kk@sendmail.ru\n");
	if (argc == 1)
	{
		printf("\nUSAGE:\n");
		printf("\tcd_RS_doctor.exe [keys] sector_in\n");
		printf("\t/C\t\t - check sector (default)\n");
		printf("\t/F\t\t - fix sector in need\n");
		printf("\t/G\t\t - rebuild sector\n");
		printf("\t-MODE1\t\t - force MODE1\n");
		printf("\t-MODE2FORM1\t - force MODE2FORM1\n");

		printf("\nEXAMPLE:\n");
		printf("\tcd_RS_doctor sec.dat\t\t\t- check sector\n");
        printf("\tcd_RS_doctor /F sec.dat fixed_sec.dat\t- fix sector\n");
        printf("\tcd_RS_doctor /R sec.dat reb_sec.dat\t- rebuild sector\n");
        printf("\tcd_RS_doctor -MODE1 sector.dat\t\t- force MODE1 & check sector \n");
        printf("\tcd_RS_doctor /R -MODE1 sec.dat\t\t- force MODE1 & rebuild sector \n");


		return 0;
	} my_DO = 0;

	// default keys
	//-----------------------------------------------------------------------
    my_DO |= my_CHK;
    xMODE = ElBy_TRACK_MODE1; 

	// parsing command-line arguments
	for (a = 1; a < argc; a++)
	{		
		if (strcmp(argv[a], key_FIX) == 0) { my_DO |= my_FIX; my_DO |= my_OUT; LAST_NOT_KEY++;}
        if (strcmp(argv[a], key_GEN) == 0) { my_DO |= my_GEN; my_DO |= my_OUT; LAST_NOT_KEY++;}
        if (strcmp(argv[a], key_CHK) == 0) { my_DO |= my_CHK; LAST_NOT_KEY++;}						

        if (strcmp(argv[a], key_MODE1) == 0) { xMODE = ElBy_TRACK_MODE1; my_DO |= my_FRC; LAST_NOT_KEY++;}						
        if (strcmp(argv[a], key_MODE2FORM1) == 0) { xMODE = ElBy_TRACK_MODE2FORM1; my_DO |= my_FRC; LAST_NOT_KEY++;}						
	}


	// READING SECTOR_IN anyway
	//------------------------------------------------------------------------
	sector_in = fopen(arg_sector_in,"rb");

	if (sector_in == 0) {
		fprintf(stderr,"-ERR: open file %s\x7\n",arg_sector_in); return -1; }

	printf("+OK: open file %s\n",arg_sector_in);

	a = fread((void *)&raw_sector_in, 1, sizeof(raw_sector_in), sector_in);

	if (a != RAW_SECTOR_SIZE) {
	    fprintf(stderr,"-ERR: read file %s\x7\n",arg_sector_in); return -1; }

	printf("+OK: read file %s\n",arg_sector_in);		
	//-----------------------------------------------------------------------
	// end reading SECTOR_IN



	// OPENING SECTOR_OUT, if FIX
	//-----------------------------------------------------------------------
	if (my_DO & my_OUT)
	{
		sector_out = fopen(arg_sector_out,"wb");		

		if (sector_in == 0) {
			fprintf(stderr,"-ERR: open file %s\x7\n",arg_sector_out); return -1; }

		printf("+OK: open file %s\n",arg_sector_out);
	}


	// CHECKING THE SECTOR
	//-----------------------------------------------------------------------
	if (my_DO & my_CHK)
	{
		printf("checking sector....");
		if ((my_DO & my_FRC) == 0)
		{
			a = CheckSector(&raw_sector_in, ElBy_TEST_ONLY);
		}
			else
		{
			switch(xMODE)
			{
				case ElBy_TRACK_MODE1:
					a = CheckECCAndEDC_Mode1((struct RAW_SECTOR_MODE1*) &raw_sector_in, ElBy_TEST_ONLY);
					break;

				case ElBy_TRACK_MODE2FORM1:
					a = CheckECCAndEDC_Mode2Form1((struct RAW_SECTOR_MODE2FORM1*) &raw_sector_in, ElBy_TEST_ONLY);
					break;

				default:
				fprintf(stderr, "-ERR: unsuport mode %s\x7\n",ElBy_TRACK_TEXT(xMODE));
				a = ElBy_SECTOR_ERROR;
			}
        }
		printf("%s\n",(a == ElBy_SECTOR_OK)?"SECTOR OK":"SECTOR ERROR");
	}

	// CORRECTING SECTOR
	//-----------------------------------------------------------------------
	if (my_DO & my_FIX)
	{
		printf("try fixing sector....");
		if ((my_DO & my_FRC) == 0)
		{
			a = CheckSector(&raw_sector_in, ElBy_REPAIR);
		}
			else
		{
			switch(xMODE)
			{
				case ElBy_TRACK_MODE1:
					a = CheckECCAndEDC_Mode1((struct RAW_SECTOR_MODE1*) &raw_sector_in, ElBy_REPAIR);
					break;

				case ElBy_TRACK_MODE2FORM1:
					a = CheckECCAndEDC_Mode2Form1((struct RAW_SECTOR_MODE2FORM1*) &raw_sector_in, ElBy_REPAIR);
					break;

				default:
				fprintf(stderr, "-ERR: unsuport mode %s\x7\n",ElBy_TRACK_TEXT(xMODE));
				a = ElBy_SECTOR_ERROR;
			}

		}


		printf("%s\n",(a == ElBy_SECTOR_OK)?"REPAIR OK":"REPAIR ERROR");
		memcpy(&raw_sector_out, &raw_sector_in, sizeof(raw_sector_in));
	}


	// REBUILDING SECTOR
	//-----------------------------------------------------------------------
	if (my_DO & my_GEN)
	{
		printf("generate sector....");		
		if ((my_DO & my_FRC) == 0)
		{
	        a = ReGenECCAndEDC(&raw_sector_in);
	    }
			else
		{
		switch(xMODE)
		{
        			case ElBy_TRACK_MODE1:
       			a = ReGenECCAndEDC_Mode1(
       			((struct RAW_SECTOR_MODE1*) &raw_sector_in));
			break;

			case ElBy_TRACK_MODE2FORM1:
			    a = ReGenECCAndEDC_Mode2Form1(
       			((struct RAW_SECTOR_MODE2FORM1*) &raw_sector_in));
			break;
		/*
			case ElBy_TRACK_MODE2FORM1:
			    a = GenECCAndEDC_Mode2Form1(
       			((struct RAW_SECTOR_MODE2FORM1*) &raw_sector_in)->USER_DATA,
				&((struct RAW_SECTOR_MODE2FORM1*) &raw_sector_in)->ADDR,
				&((struct RAW_SECTOR_MODE2FORM1*) &raw_sector_in)->subheader,
                &((struct RAW_SECTOR_MODE2FORM1*) &raw_sector_out)[0]);
			break;
	   */

			default:
			printf("-ERR: not support mode\n");
			a = ElBy_SECTOR_ERROR;
		 }

		}
		printf("%s\n",(a == ElBy_SECTOR_OK)?"REBUILD OK":"REBUILD ERROR");
		memcpy(&raw_sector_out, &raw_sector_in, sizeof(raw_sector_in));

	}


	if (my_DO & my_OUT)
	{
		fwrite(&raw_sector_out, 1, sizeof(raw_sector_out), sector_out);
		fclose(sector_out);
	}
return 1;
}