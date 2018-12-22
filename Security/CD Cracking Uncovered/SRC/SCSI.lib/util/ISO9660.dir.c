/*----------------------------------------------------------------------------
 *
 *						ISO9660/Juliet FS dir'ed & pincher
 *						==================================
 *
 *		Utility for viewing the contents of any of the CD sessions written in the,
 * ISO9660/Joliet format and copying of any specified file.
 * May be useful for analysis of some kinds of anti-copying mechanisms and
 * recovery of deleted files.
----------------------------------------------------------------------------*/
#include <windows.h>
#include <stdio.h>
#include "..\CD.h"
#include "..\SCSI.h"
#include "..\CDf.h"
#include "ISO9660.h"

#define TIME_ON_TRY	500
#define MAX_TRY		5


#define SID "CD001"

read_from_cd(char *CD, unsigned char *buf, long sector)
{
	int a;
	int stat;
	for (a = 0; a < MAX_TRY; a++)
	{

		stat=cd_raw_sector_read(CD, buf, SECTOR_SIZE, sector, ONE_SECTOR, W_USER_DATA);
		// fixme 
		//if (stat == SCSI_INQ) {printf("\x7"); return 1;}
		if (stat == SCSI_OK) return 1;
		Sleep(TIME_ON_TRY);
	}
	return -1;
}

display_Primary_Volume_Descriptor(char *CD, unsigned long argSector, unsigned char *buf)
{
	struct Primary_Volume_Descriptor *p;
	p = (struct Primary_Volume_Descriptor*) buf;
	printf("Standard Identifier...........");
		fwrite(p->Standard_Identifier, 1, sizeof(p->Standard_Identifier), stdout); 
	if (memcmp(p->Standard_Identifier, SID, strlen(SID))!=0)
	{
		printf(" (NOT \"%s\"!)\x7\n",SID);
		return -1;
	} printf("\n");
	
}

display_TOC(char *CD)
{
	int a;
	int len;
	unsigned char TOC[MAX_TOC_SIZE];
	struct CD_TOC *p;
	int stat;
	printf("track  StartLBA\n---------------\n");
	stat=cd_raw_toc_read(CD, TOC, MAX_TOC_SIZE, W_TOC);
	len = TOC[0]*0x100L+TOC[1];
	p = (struct CD_TOC*) &TOC[4];
	for (a = 2; a < len; a+= sizeof(struct CD_TOC))
	{
		printf("%3X  %10d\n", p->track_number,
			p->ADDR[0]*0x1000000+p->ADDR[1]*0x10000+p->ADDR[2]*0x100+p->ADDR[3]);
		p++;
	}
	return -1;
}

#define _SLASH	"/"
#define _BASE_FI_LEN	14
#define _MAX_J_S		255
cd_find_next(char *CD, struct Directory_Record *up_dir, char *full_path, int f_Juliet)
{
	int a, b;
	long len;
	long sector;
	int next;
	int xcount = 0;
	long block_size = SECTOR_SIZE;
	unsigned char buf[SECTOR_SIZE*2];
	char l_buf [_MAX_J_S];
	struct Directory_Record *in_dir;
	char *my_full=0;

	len    = up_dir->Data_Length_A;
	sector = up_dir->Location_Extent_A;




	if (read_from_cd(CD, buf, sector) < 1) return -1;

	in_dir = (struct Directory_Record*) &buf[0];

	for (a = 0; a < len;)
	{
		if (in_dir->LEN_DR==0)
		{
			if ((len - a) < block_size) break;
			if (read_from_cd(CD, buf, ++sector) < 1) return -1;
			in_dir = (struct Directory_Record*) &buf[0];
		}

		// displaying information about the files on screen
		// -------------------------------------------------------------------
		printf("%-10d|%-10d|",in_dir->Location_Extent_A, in_dir->Data_Length_A);

		// processing name \x0 (".") è \x1 ("..")
		if ((in_dir->LEN_FI == 1) && (*in_dir->File_Identifier==0)) printf(".");
		else if ((in_dir->LEN_FI == 1) &&(*in_dir->File_Identifier==1)) printf("..");

		// displaying other names on the screen as is
		else if (f_Juliet == 0) printf(in_dir->File_Identifier); 
			else wprintf(in_dir->File_Identifier+1);

		// displaying full path on the screen
		if (full_path && !f_Juliet)
		{
			for (b=0;b<(_BASE_FI_LEN-in_dir->LEN_FI); b++) printf(" ");
			my_full=malloc(in_dir->LEN_FI+strlen(full_path)+1+strlen(_SLASH));
			memcpy(my_full, full_path, strlen(full_path)+1);strcat(my_full, _SLASH);
			my_full[strlen(my_full)+in_dir->LEN_FI+1] = 0;
			memcpy(&my_full[strlen(my_full)],in_dir->File_Identifier,in_dir->LEN_FI);
			printf("\t(%s)", my_full);
		}

		printf("\n");

		xcount++;

		// processing subdirectories
		//if ((in_dir->File_Flags & 2) && (xcount > 2))
		if ((in_dir->File_Flags & 2) && (in_dir->Location_Extent_A > sector))
		{
			cd_find_next(CD, in_dir, my_full,f_Juliet);
		}

		// next record
		next = in_dir->LEN_DR;
		in_dir = (struct Directory_Record*) (((char *) in_dir) + next);
		a += next;
		
	}
	return 1;
}


cd_find_first(char *CD, long sector, int f_Juliet)
{
	int a;
	char root[]="/root";
	unsigned char buf[SECTOR_SIZE*2];
	if (read_from_cd(CD, buf, sector + f_Juliet + PVD_sector) < 1) return -1;

	//cd_find_next(CD, ((struct Primary_Volume_Descriptor*) buf)->Directory_Record_Root_Directory, 0);
	printf("start      size       name\n--------------------------\n");

	cd_find_next(CD, &buf[156], root, f_Juliet);
	return 1;
}

cd2hdd(char *CD, char* filename, long StartLBA, long len)
{

	long real_size=0;
	long sector;
	FILE *f;
	unsigned char buf[SECTOR_SIZE*2];
	
	if ((f = fopen(filename, "wb"))==0) return -1;

	if (len == 0) return 0;

	sector = StartLBA;

	while(len)
	{
		printf("bytes left %15d\r", len);
		if (read_from_cd(CD, buf, sector++)<1) return -1;
		real_size = (len>SECTOR_SIZE)?SECTOR_SIZE:len;

		fwrite(buf, 1, real_size, f);
		len -= real_size;
	}
		printf("+OK\t\t\t\t\n");
	fclose(f); return 1;
}

#define argCD		argv[1]
#define argSector	atol(argv[2])
#define argFileName	argv[2]
#define argJuliet	argv[3]
#define argStartLBA	atol(argv[3])
#define argLen		atol(argv[4])
main(int argc, char** argv)
{
	long f_Juliet = 0;
	unsigned char buf[SECTOR_SIZE*2];
	printf("ISO9660.DIR'ed for SCSI/IDE CD-ROM by Kris Kaspersky\n");	
	

	if (argc < 2)
	{
		fprintf(stderr, "USAGE:\t\"ISO9660.dir.exe CD\"                          for display TOC\n");
		fprintf(stderr, "\t\"ISO9660.dir.exe CD start_LBA [-Juliet]\"      for dir'ed\n");
		fprintf(stderr, "\t\"ISO9660.dir.exe CD file_name start_LBA size\" for dir'ed\n\n");
		fprintf(stderr, "\tCD      - is adapterID.targetID or \\\\.\\X:\n");
		fprintf(stderr, "\t-Juliet - select Joliet file system\n\n");
		printf("       SCSI_INQUITY via ASPI32\n");
		printf("-------------------------------------\n");
		SEND_SCSI_CMD("?.?", 0, 0, 0, 0, 0, 0);
		return 0;
	}

	//
	if (argc == 2)
	{
		display_TOC(argCD);
		return 0;
	}

	if (argc == 4)
	{
		if (strstr(argJuliet, "Joliet")) f_Juliet = 1;
	}

	if (argc > 2 && argc < 5)
	{
		if (read_from_cd(argCD, buf, PVD_sector+argSector) < 1) return 0;

		if (display_Primary_Volume_Descriptor(argCD, argSector, buf) == -1)
		return 0;
		cd_find_first(argCD, argSector, f_Juliet);
		return 0;
	}

	if (argc == 5)
	{
		cd2hdd(argCD, argFileName, argStartLBA, argLen);
		return 0;
	}
	
	return 0;
}
