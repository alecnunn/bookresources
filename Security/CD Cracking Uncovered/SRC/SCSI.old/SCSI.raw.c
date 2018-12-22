/*----------------------------------------------------------------------------
 *
 *				READING A SECTOR FROM CD-ROM IN RAW MODE VIA SPTI
 *				=================================================
 *
 * build 0x001 @ 27.05.2003
----------------------------------------------------------------------------*/
#include <windows.h>
#include <winioctl.h>
#include <stdio.h>
#include "SPTI.raw.h"

#define RAW_READ_CMD			0xBE	// ATAPI RAW READ

#define WHATS_READ				0xF8	// Sync & All Headers & User Data + EDC/ECC
#define PACKET_LEN				2352	// length of one sector

//#define WHATS_READ			0x10	// User Data
//#define PACKET_LEN			2048	// length of one sector

//-[DWORD READ_RAW_SECTOR_FROM_CD]---------------------------------------------
//		This function reads one or more sectors from CDROM in RAW mode,
// according to the flags passed to it
//
//	ARG:
//		driver		-	drive to open (for example,  "\\.\X:")
//		adapter_id	-	bus number (0 - primary, 1 - secondary)
//		read_id		-	device number (0 - master, 1 - slave)
//		buf			-	read buffer
//		buf_len		-	buffer size in bytes
//		StartSector	-	starting sector for reading (numbering from zero)
//		N_SECTOR	-	how many sectors to read \
//		flags		-	what to read (see the ATAPI specification)
//
//	RET:
//		!=0			-	success
//		==0			-	error
//
//	NOTE:
//		works only under NT/W2K/XP and requires administrative privileges
//-----------------------------------------------------------------------------
DWORD READ_RAW_SECTOR_FROM_CD(char *driver, int adapter_id, int read_id,
char *buf, int buf_len, DWORD StartSector, DWORD N_SECTOR, BYTE flags)
{
	HANDLE				hCD;
	SCSI_PASS_THROUGH	srb;
    SCSI_ADDRESS		sa;
	DWORD				returned, length, status;
	char buff[4000];
	memset(buff,0,1000);
	// OPENING THE DEVICE
	//------------------------------------------------------------------------
	// Attention! do not use "\\\\.\\SCSI0" or "\\\\.\\CdRom0"
	// you'll fail! (this is a common error of most beginners)
	hCD = CreateFile (	driver, GENERIC_WRITE|GENERIC_READ,
						FILE_SHARE_READ|FILE_SHARE_WRITE,0,OPEN_EXISTING,0,0);
	if (hCD == INVALID_HANDLE_VALUE)
	{
	    printf("-ERR: open device\n");
		return 0;
	}

	// FIRNUBG SRB
	//------------------------------------------------------------------------
	memset(&srb, 0, sizeof(SCSI_PASS_THROUGH));	// initialization

	// OPTIONS
	srb.Length				= sizeof(SCSI_PASS_THROUGH);
	srb.PathId				= 0;					// SCSI controller ID
	srb.TargetId			= adapter_id;			// target device ID
	srb.Lun 				= read_id;				// logical unit device ID
	srb.CdbLength			= 12;					// CDB packet length
	srb.SenseInfoLength		= 0;					// SenseInfo not needed
	srb.DataIn 				= SCSI_IOCTL_DATA_IN;	// we are going to read
	srb.DataTransferLength	= PACKET_LEN*N_SECTOR;	// length of data to be read
	srb.TimeOutValue 		= 200;					// TimeOut
	srb.DataBufferOffset	= buf;					// pointer to buffer
	srb.SenseInfoOffset 	= 0;					// SenseInfo not needed

	// CDB packet containing ATAPI commands
	srb.Cdb[0]				= RAW_READ_CMD;			// read raw sector
	srb.Cdb[1]				= 0x0;					// any disc format
	
	// number of the first sector for reading, first it is necessary to pass the most significant
	// byte of the most significant work, then least significant byte of the least significant work
	srb.Cdb[2]				= HIBYTE(HIWORD(StartSector));
	srb.Cdb[3]				= LOBYTE(HIWORD(StartSector));
	srb.Cdb[4]				= HIBYTE(LOWORD(StartSector));
	srb.Cdb[5]				= LOBYTE(LOWORD(StartSector));

	// number of sectors for reading
	srb.Cdb[6]				= LOBYTE(HIWORD(N_SECTOR));
	srb.Cdb[7]				= LOBYTE(LOWORD(N_SECTOR));
	srb.Cdb[8]				= HIBYTE(LOWORD(N_SECTOR));

	srb.Cdb[9]				= flags;				// what to read
	srb.Cdb[10]				= 0;					// Sub-Channel Data Bits
	srb.Cdb[11]				= 0;					// reserverd

	// passing SRB block to ATAPI device
	
	status = DeviceIoControl(hCD,  0x1b0011,
			&srb, sizeof(SCSI_PASS_THROUGH), &srb, 0, &returned, FALSE);

//	status = DeviceIoControl(hCD,  0x4100c,
// /* IOCTL_SCSI_GET_ADDRESS, */	0, 0, &buff, sizeof(1000), &returned, FALSE);

//    printf("%x\n",sa.PortNumber);
  //  printf("%x %x\n",buff[0], buff[1]);
    
	return 1;
}

#define FNAME	"raw.sector.dat"

main(int argc, char **argv)
{
	FILE *f;
	char *buf;

	if (argc<6)
	{
		fprintf(stderr,	"USAGE: SPTI.raw.exe \\\\.\\X: adapter_id device_id"\
						"start_sector N_sector\n"); return -1;
	}

	buf = malloc(PACKET_LEN*atol(argv[5]));
	if (READ_RAW_SECTOR_FROM_CD(argv[1], atol(argv[2]), atol(argv[3]),buf,
		PACKET_LEN*atol(argv[5]),atol(argv[4]),atol(argv[5]),WHATS_READ) != 0)
	{
		f=fopen(FNAME,"w");fwrite(buf,1,PACKET_LEN*atol(argv[5]),f);fclose(f);
	}

	return 0;
}