/*----------------------------------------------------------------------------
 *
 *				READING SECTORS FROM CDDA IN RAW MODE (USING CDFS)
 *				==================================================
 *
 * build 0x001 @ 26.05.2003
----------------------------------------------------------------------------*/
#include <windows.h>
#include <winioctl.h>
#include <stdio.h>

typedef enum _TRACK_MODE_TYPE {
	YellowMode2,
	XAForm2,
	CDDA
} TRACK_MODE_TYPE, *PTRACK_MODE_TYPE;

typedef struct __RAW_READ_INFO {
	LARGE_INTEGER	DiskOffset;
	ULONG			SectorCount;
	TRACK_MODE_TYPE	TrackMode;
} RAW_READ_INFO, *PRAW_READ_INFO;

#define CDROM_RAW_SECTOR_SIZE	2352
#define CDROM_SECTOR_SIZE		2048


//--[ReadCDDA]-----------------------------------------------------------------
//				
//				Reads sector in raw mode from CDDA discs
//				========================================
//	ARG:
//		drive		 - device name (example: "\\\\.\\X:")
//		start_sector - number of the first readable sector
//		n_sec		 - number of sectors to read
//
//	RET:
//		== 0		 - error
//		!= 0		 - pointer to buffer containing read sectors
//
//	NOTE:
//		1) This function supports only discs of the types supporting CDFS
//		driver (the one that it uses). Built-in Windows NT driver supports
//		only CDDA discs
//----------------------------------------------------------------------------
char* ReadCDDA(char *drive, int start_sector, int n_sec)
{
	int		a;
	HANDLE	hCD;
	DWORD	x_size;
	char	*szDrive;
	BOOL	fResult = 0;
	unsigned char	*buf;
	RAW_READ_INFO	rawRead;



	// PREPARING THE RAW_READ_INFO STRUCTURE, passed to the CD-ROM driver
	rawRead.TrackMode			= CDDA;
	rawRead.SectorCount			= n_sec;
	rawRead.DiskOffset.QuadPart	= start_sector * CDROM_SECTOR_SIZE;

	// ALLOCATING MEMORY
	buf = malloc(CDROM_RAW_SECTOR_SIZE * n_sec);

	// GETTING DEVICE DESCRIPTOR
	hCD = CreateFile(drive,GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,0,0);
	if (hCD != INVALID_HANDLE_VALUE)

	// PASSING THE IOCTL_CDROM_RAW_READ COMMAND TO THE DEVICE DRIVER
	fResult = DeviceIoControl(	hCD, 0x2403E /* IOCTL_CDROM_RAW_READ */,
								&rawRead, sizeof(RAW_READ_INFO),
								buf, CDROM_RAW_SECTOR_SIZE * n_sec,
								&x_size, (LPOVERLAPPED) NULL);

	// DISPLAYING THE RESULT (IF ANY)
	if (fResult)
		for (a = 0; a <= x_size; ++a) printf("%02X%s",buf[a],(a%24)?" ":"\n");
	else
		printf("-ERROR"); printf("\n");

	// EXITING
	CloseHandle(hCD); return (fResult)?buf:0;
}

main(int argc, char **argv)
{
	if (argc<4)
	{
		fprintf(stderr,"USAGE: IOCTL.CDDA.raw.exe \\\\.\\X: StartSec NumSec\n");
		return 0;
	} ReadCDDA(argv[1], atol(argv[2]), atol(argv[3]));
	return 0;
}
