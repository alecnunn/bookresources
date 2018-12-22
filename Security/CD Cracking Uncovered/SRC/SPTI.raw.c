/*----------------------------------------------------------------------------
 *
 *				READING SECTORS FROM CD-ROM IN RAW MODE USING SPTI
 *				==================================================
 *
 * This program operates only under Windows NT, It requires the user
 * to be granted administrative privileges
 *
 * build 0x001 @ 27.05.2003
----------------------------------------------------------------------------*/
#include <windows.h>
#include <stdio.h>

#define RAW_READ_CMD			0xBE	// ATAPI RAW READ

#define WHATS_READ				0xF8	// Sync & All Headers & User Data + EDC/ECC
#define PACKET_LEN				(2352)	// sector length

//#define WHATS_READ			0x10	// User Data
//#define PACKET_LEN			2048	// sector length

#define FNAME					"raw.sector.dat"


#define SCSI_IOCTL_DATA_IN				1			// from ntddscsi.h
#define IOCTL_SCSI_PASS_THROUGH			0x4D004		// from ntddscsi.h
#define IOCTL_SCSI_PASS_THROUGH_DIRECT	0x4D014		// from ntddscsi.h4d014
#define IOCTL_SCSI_GET_ADDRESS			0x41018

typedef struct _SCSI_PASS_THROUGH_DIRECT {					// from ntddscsi.h
    USHORT Length;
    UCHAR ScsiStatus;
    UCHAR PathId;
    UCHAR TargetId;
    UCHAR Lun;
    UCHAR CdbLength;
    UCHAR SenseInfoLength;
    UCHAR DataIn;
    ULONG DataTransferLength;
    ULONG TimeOutValue;
    UCHAR* DataBufferOffset;
    ULONG SenseInfoOffset;
    UCHAR Cdb[16];
} SCSI_PASS_THROUGH_DIRECT, *PSCSI_PASS_THROUGH_DIRECT;

typedef struct _SCSI_ADDRESS {						// from ntddscsi.h
    ULONG Length;
    UCHAR PortNumber;
    UCHAR PathId;
    UCHAR TargetId;
    UCHAR Lun;
}SCSI_ADDRESS, *PSCSI_ADDRESS;


//-[DWORD READ_RAW_SECTOR_FROM_CD]---------------------------------------------
// This	function reads one or more sectors from CD-ROM in RAW mode,
// according to the flags passed to it
//
//	ARG:
//		driver		-	device to open (for example,  "\\\\.\\X:" or "\\\\.\\CdRom0")
//		buf			-	read buffer
//		buf_len		-	buffer size in bytes
//		StartSector	-	from which sector to start reading (numbering starts from 0)
//		N_SECTOR	-	number of sectors to read
//		flags		-	what to read (see SCSI/ATAPI specification)
//
//	RET:
//		!=0			-	success
//		==0			-	error
//
//	NOTE:
//		works only under NT/W2K/XP and requires administrative privileges
//-----------------------------------------------------------------------------
DWORD READ_RAW_SECTOR_FROM_CD(char *driver, char *buf, int buf_len, DWORD StartSector, DWORD N_SECTOR, BYTE flags)
{
	HANDLE				hCD;
	SCSI_PASS_THROUGH	srb;
	DWORD				returned, length, status;

	// OPENING THE DEVICE
	hCD = CreateFile (	driver, GENERIC_WRITE|GENERIC_READ,
						FILE_SHARE_READ|FILE_SHARE_WRITE,0,OPEN_EXISTING,0,0);
	if (hCD == INVALID_HANDLE_VALUE) { printf("-ERR: open CD\n"); return 0;}

	// FORMING SRB
	//------------------------------------------------------------------------
	memset(&srb,0,sizeof(SCSI_PASS_THROUGH_DIRECT));// initialization

	srb.Length				= sizeof(SCSI_PASS_THROUGH_DIRECT);
	srb.PathId				= 0;					// SCSI controller ID
	srb.TargetId			= 6;					// ignored
	srb.Lun 				= 9;					// ignored
	srb.CdbLength			= 12;					// CDB packet length
	srb.SenseInfoLength		= 0;					// SenseInfo not needed
	srb.DataIn 				= SCSI_IOCTL_DATA_IN;	// we are going to read
	srb.DataTransferLength	= PACKET_LEN*N_SECTOR;	// data length for reading
	srb.TimeOutValue 		= 2000;					// TimeOut
	srb.DataBufferOffset	= buf;					// pointer to the buffer
	srb.SenseInfoOffset 	= 0;					// SenseInfo not needed

	// CDB packet containing ATAPI commands
	srb.Cdb[0]				= RAW_READ_CMD;			// read raw sector
	srb.Cdb[1]				= 0x0;					// any disc format
	
	// number of the first sector to be read, most significant byte of the
	// most significant word is passed first, then least significant byte of 
	// the least significant word
	srb.Cdb[2]				= HIBYTE(HIWORD(StartSector));
	srb.Cdb[3]				= LOBYTE(HIWORD(StartSector));
	srb.Cdb[4]				= HIBYTE(LOWORD(StartSector));
	srb.Cdb[5]				= LOBYTE(LOWORD(StartSector));

	// number of sectors for reading
	srb.Cdb[6]				= LOBYTE(HIWORD(N_SECTOR));
	srb.Cdb[7]				= HIBYTE(LOWORD(N_SECTOR));
	srb.Cdb[8]				= LOBYTE(LOWORD(N_SECTOR));

	srb.Cdb[9]				= flags;				// what to read
	srb.Cdb[10]				= 0;					// Sub-Channel Data Bits
	srb.Cdb[11]				= 0;					// reserverd

	// SENDING SRB BLOCK TO ATAPI DEVICE
	status = DeviceIoControl(hCD, IOCTL_SCSI_PASS_THROUGH,
			&srb, sizeof(SCSI_PASS_THROUGH), &srb, 0, &returned, FALSE);

	return 1;
}

#define argCD		(argv[1])
#define argFROM		(atol(argv[2]))
#define argN_SEC	(atol(argv[3]))

main(int argc, char **argv)
{
	FILE *f;
	char *buf;

	if (argc < 4)
	{
		fprintf(stderr,	"USAGE: SPTI.raw.sector.read.exe \\\\.\\X:"\
						"start_sector N_SEC\n"); return -1;
	}

	// ALLOCATING MEMORY 
	buf = malloc(PACKET_LEN * argN_SEC * 2);
	//                                 ^^^^
	// Under certain circumstances the drive can return more data than
	// we have requested

	// READING SECTORS INTO THE BUFFER
	if (READ_RAW_SECTOR_FROM_CD(argCD, buf,
		PACKET_LEN * argN_SEC, argFROM, argN_SEC, WHATS_READ) != 0)
	{
		// Writing sectors into the FNAME file on the hard disk
		f=fopen(FNAME,"w");fwrite(buf, 1, PACKET_LEN * argN_SEC,f);fclose(f);
	}

	return 0;
}