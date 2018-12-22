/*----------------------------------------------------------------------------
 *
 *					READS CD-ROM SECTORS IN COOKED MODE
 *					===================================
 *
 *		this program operates only under Windows NT, and doesn't require
 * administrative privileges
 *
 * Build 0x001 @ 19.05.03
---------------------------------------------------------------------------- */
#include <windows.h>
#include <winioctl.h>
#include <stdio.h>

// DEFAULT PARAMETERS
#define DEF_FN				"sector"
#define DEF_TO				0x666
#define DEF_FROM			0x000
#define CDROM_SECTOR_SIZE	2048		// for MODE1/MODE2FORM1 only!

// COMMAND-LINE ARGUMENTS
#define argCD		(argv[1])
#define argFN		((argc > 2)?argv[2]      :DEF_FN)
#define argFROM		((argc > 3)?atol(argv[3]):DEF_FROM)
#define argTO		((argc > 4)?(atol(argv[4]) > argFROM)?atol(argv[4]):argFROM:DEF_TO)

main(int argc, char **argv)
{
	int				a;
	FILE			*f;
	HANDLE			hCD;
	char			*buf;
	DWORD			x_read;
	char			buf_n[1024];

	// CHECKING ARGUMENTS
	if (argc<2)
	{
			printf("USAGE: cooked.sector.read.exe PhysCD [filename] [from] [to]\n");
			printf("\tPhysCD   - physical name of CD (\"\\\\.\\G:\")\n");
			printf("\tfilename - file name to store follow sector\n");
			printf("\tfrom     - start sector\n");
			printf("\tto       - end sector\n");
			return 0;
	}

	// TITLE
	fprintf(stderr,"cooked sector reader for NT\n");

	// ALLOCATING MEMORY
	buf = malloc(CDROM_SECTOR_SIZE); if (!buf) {printf("-ERR: low memory\n");return -1;}

	// OPENING A DEVICE
	hCD=CreateFile(argCD, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
	if (hCD == INVALID_HANDLE_VALUE) {
		printf("-ERR: error CreateFile(%s,....)\n", argCD); return -1;
	}

	// INFO
	printf("read sector from %04d to %04d in %s file\n", argFROM, argTO, argFN);

	// POSITIONING THE POINTER TO THE FIRST READABLE SECTOR
	SetFilePointer (hCD, CDROM_SECTOR_SIZE * argFROM, NULL, FILE_BEGIN);

	// READING SECTORS ONE BY ONE
	for (a = argFROM; a <= argTO; a++)
	{
		// reading the next sector
		if (ReadFile(hCD, buf, CDROM_SECTOR_SIZE, &x_read, NULL) && x_read)
		{
			// writing the read sector into a file
			sprintf(buf_n,"%s[%04d].dat",argFN, a);
			if (f = fopen(buf_n,"wb")){fwrite(buf, 1, x_read, f); fclose(f);}
			printf("sector [%04d.%04d] read\r",a, argTO);
		}
			else
		{
			printf("sector %04d read error\n",a);
		}
	}
}

