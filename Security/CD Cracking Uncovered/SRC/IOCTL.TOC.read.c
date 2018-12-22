/*----------------------------------------------------------------------------
 *
 *					READING AND DECODING TOC
 *					========================
 *
 * build 0x001 @ 26.05.2003
----------------------------------------------------------------------------*/
#include <windows.h>
#include <winioctl.h>
#include <stdio.h>

#define buf_len			(1024*1024)
main(int argc, char **argv)
{
	int		a;
	HANDLE	hCD;
	unsigned char	*buf;
	WORD	TOC_SIZE;
	BYTE	n_track;
	DWORD	x_size,b;

	// CHECKING ARGUMENTS
	if (argc < 2)
	{
		fprintf(stderr, "USAGE: IOCTL.TOC.read.exe \\\\.\\X:\n");
		return 0;
	}

	// TITLE
	fprintf(stderr,"simple TOC reader via IOCTL\n");


	// ALLOCATING MEMORY
	buf=malloc(buf_len);

	// OPENING THE DEVICE
	hCD=CreateFile(argv[1],GENERIC_READ,FILE_SHARE_READ, 0, OPEN_EXISTING, NULL, NULL);

	// EXITING IN CASE OF ERROR
	if (hCD == INVALID_HANDLE_VALUE)
		{ fprintf(stderr,"-ERR: %x\n",GetLastError()); return 0; }

	// PASSING THE CDROM_READ_TOC COMMAND TO THE DEVICE DRIVER
	if (DeviceIoControl(hCD,0x24000, 0,0,buf,buf_len,&x_size,0) != 0)
	{
		// GETTING рня LENGTH (it is written in the inverse order)
		*((BYTE *)(&TOC_SIZE)) = buf[1]; *((BYTE *)(&TOC_SIZE+1)) = buf[0];
		printf("TOC Data Length........%d\n",TOC_SIZE);

		// decoding the remaining info
		printf("First Session Number...%d\n",buf[2]); 
		printf("Last Session Number....%d\n\n",(n_track=buf[3]));
		for (a = 1; a <= n_track; a++)
		{
			printf("track %d\n{\n",a);
			printf("\treserved.............%x\n",buf[a * 8 - 4]);
			printf("\tADR|control..........%d\n",buf[a * 8 - 3]);
			printf("\ttrack number.........%d\n",buf[a * 8 - 2]);
			printf("\treserved.............%d\n",buf[a * 8 - 1]);
			printf("\treserved.............%d\n",buf[a * 8 + 0]);
			printf("\tmin..................%d\n",buf[a * 8 + 1]);
			printf("\tsec..................%d\n",buf[a * 8 + 2]);
			printf("\tfra..................%d\n",buf[a * 8 + 3]);
			printf("}\n\n");
		}
		
		// displaying TOC contents in raw form
		printf("\n\t\t\t* * * RAW * * *\n");
		for(a = 0; a < x_size; a++)
			printf("%02X%s",(unsigned char)buf[a],((a+1)%22)?" ":"\n");
		printf("\n\t\t\t* * *  *  * * *\n");
	}
}