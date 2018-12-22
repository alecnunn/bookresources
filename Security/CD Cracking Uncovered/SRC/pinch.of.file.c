/*----------------------------------------------------------------------------
 *
 *								PINCH OF FILE
 *								=============
 *
 * Cuts fragments of any size from files (useful for file-by-file copying
 * of CDs containing files with incorrect lengths and starting sectors)
 * 
 *
 * Build 0x001 @ 09.07.2003
----------------------------------------------------------------------------*/
#include <stdio.h>

#define MAX_BUFF	0x6666					// buffer size

#define _MIN(a, b) ((a<b)?a:b)				// minimum of two numbers

main(int argc, char **argv)
{
	#define FIN				argv[1]			// defining arguments
	#define FOUT			argv[2]
	#define BEGIN_OFFSET	argv[3]
	#define END_OFFSET		argv[4]

   	long a;									// real_read_size
	FILE *fin, *fout;						// file handles
	char buf[MAX_BUFF];						// read buffer
	long begin, end, pinch_size;			// start, end and remainder
	char *stop;

	// TITLE
	fprintf(stderr,"file pincher v 0.0 by Kris Kaspersky\n");

	if (argc < 5)							// HELP (if needed)
	{
		fprintf(stderr,	"USAGE:\n\tpinch.of.file.exe file_in "\
						"file_out start_offset end_offset\n\n");
        fprintf(stderr, "for example:\n\t"\
        				" pinch.of.file.ex \"Z:\\mp3\\People Are People.mp3\""\
        				" C:\\my.mp3 1246 7923456\n"); return 0;
	}

	// opening the source file
	fin = fopen(FIN,"rb");
	if (fin == 0)  {fprintf(stderr,"-ERR open %s file\x7\n",FIN);return 0;}
	
	// opening the target file
	fout = fopen(FOUT,"wb");
	if (fout == 0) {fprintf(stderr,"-ERR open %s file\x7\n",FOUT);return 0;}

	// converting string arguments into numbers
	begin = strtol(BEGIN_OFFSET, &" ", 10);end = strtol(END_OFFSET, &stop, 10);
	pinch_size = end - begin;

	// displaying information
	printf("%s -> %s at %d to %d (%d total)\n",FIN,FOUT,begin,end,pinch_size);

	// moving the pointer
	if (fseek(fin, begin, SEEK_SET))
		{fprintf(stderr,"-ERR: fseek error\x7\n"); return -1;}

	// main read loop
	while(1)
	{
		// reading
		a = fread(buf, 1, _MIN(pinch_size, MAX_BUFF), fin);
		if (a == 0) {fprintf(stderr,"\n-ERR:fread error\x7\n"); return -1;}

		// writing
		if (fwrite(buf, 1, a, fout)==0)
			{ fprintf(stderr,"\n-ERR:fwrite error\x7\n"); return -1; }

		// is there anything to read?
		if ((pinch_size -= a)==0) break; 

		// displaying the progress
		fprintf(stderr,"\r%3.1f%% left to processed",
							100.0*((float)pinch_size/(end - begin)));
	} 
	fprintf(stderr,"\r.OK\t\t\t \n");


}