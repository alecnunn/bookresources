/*----------------------------------------------------------------------------
 *
 *				INTERACTING WITH IDE drives using SCSI miniport
 *				===============================================
 *
 *	Build 0x001 03.06.2003
----------------------------------------------------------------------------*/
#include <windows.h>
#include <stdio.h>

#define	IDE_ATA_IDENTIFY	0xEC	// ATA command for drive identification

//#define	SENDIDLENGTH		512+4	// max size of the DeviceIoControl buffer
#define  SENDIDLENGTH  sizeof (SENDCMDOUTPARAMS) + IDENTIFY_BUFFER_SIZE
#define	IDENTIFY_BUFFER_SIZE	512

// STRUCTURES DEFINED IN THE SCSI.H file and placed here specially for those 
// who haven't got NT DDK
#define	FILE_DEVICE_SCSI				0x0000001b
#define	IOCTL_SCSI_MINIPORT				0x0004D008
#define	IOCTL_SCSI_MINIPORT_IDENTIFY	((FILE_DEVICE_SCSI << 16) + 0x0501)

#pragma pack(1)
typedef struct _SRB_IO_CONTROL
{
	ULONG HeaderLength;			// sizeof(SRB_IO_CONTROL)
	UCHAR Signature[8];			// minidriver signature
	ULONG Timeout;				// max. waiting time in seconds
	ULONG ControlCode;			// command code
	ULONG ReturnCode;			// return code
	ULONG Length;				// length of the entire buffer
} SRB_IO_CONTROL, *PSRB_IO_CONTROL;

typedef struct _IDEREGS
{
	BYTE bFeaturesReg;			// IDE Features register
	BYTE bSectorCountReg;		// IDE SectorCount register
	BYTE bSectorNumberReg;		// IDE SectorNumber register
	BYTE bCylLowReg;			// IDE CylLowReg register
	BYTE bCylHighReg;			// IDE CylHighReg register
	BYTE bDriveHeadReg;			// IDE DriveHead register
	BYTE bCommandReg;			// command register
	BYTE bReserved;				// reserved
} IDEREGS, *PIDEREGS, *LPIDEREGS;


typedef struct _SENDCMDINPARAMS
{
	DWORD		cBufferSize;	// buffer size in bytes or zero
	IDEREGS		irDriveRegs;	// structure containing values of IDE registers
	BYTE		bDriveNumber;	// physical disc number, counting from zero
	BYTE		bReserved[3];	// reserved
	DWORD		dwReserved[4];	// reserved
	BYTE		bBuffer[1];		// input buffer starts here
} SENDCMDINPARAMS, *PSENDCMDINPARAMS, *LPSENDCMDINPARAMS;

typedef struct _DRIVERSTATUS
{
   BYTE  bDriverError;  //  Error code from driver, or 0 if no error.
   BYTE  bIDEStatus;    //  Contents of IDE Error register.
                        //  Only valid when bDriverError is SMART_IDE_ERROR.
   BYTE  bReserved[2];  //  Reserved for future expansion.
   DWORD  dwReserved[2];  //  Reserved for future expansion.
} DRIVERSTATUS, *PDRIVERSTATUS, *LPDRIVERSTATUS;


typedef struct _SENDCMDOUTPARAMS
{
   DWORD         cBufferSize;   //  Size of bBuffer in bytes
   DRIVERSTATUS  DriverStatus;  //  Driver status structure.
   BYTE          bBuffer[1];    //  Buffer of arbitrary length in which to store the data read from the                                                       // drive.
} SENDCMDOUTPARAMS, *PSENDCMDOUTPARAMS, *LPSENDCMDOUTPARAMS;


int ATAPI_MINIPORT_DEMO(void)
{
	int		a;
	HANDLE	h;
	char	*buf;
	int		LU = 0;
	DWORD	returned;
	int		controller;
	char	ScsiPort [16];
	char	buffer [sizeof (SRB_IO_CONTROL) + SENDIDLENGTH];

	SRB_IO_CONTROL	*p	= (SRB_IO_CONTROL  *) buffer;
	SENDCMDINPARAMS	*pin= (SENDCMDINPARAMS *) (buffer + sizeof (SRB_IO_CONTROL));

	// polling both IDE controllers in a loop
	for (controller = 0; controller < 2; controller++)
	{
		// forming ScsiPort for each controller
		sprintf (ScsiPort, "\\\\.\\Scsi%d:", controller);

		// opening the respective ScsiPort
		h= CreateFile (ScsiPort,GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);

		if (h == INVALID_HANDLE_VALUE) { // EXITING IN CASE OF ERROR
			printf ("-ERR: Unable to open ScsiPort%d\n",controller); return -1;
		}
	
		// polling both devices on each IDE controller
		for (LU = 0; LU < 2; LU++)
		{
			// initializing the input buffer
			memset (buffer, 0, sizeof (buffer));

			// PREPARING THE SRB_IO_CONTROL STRUCTURE
			// intended for the miniport driver
			p -> Timeout		= 10000;					// waiting
			p -> Length			= SENDIDLENGTH;				// max. length
			p -> HeaderLength	= sizeof (SRB_IO_CONTROL);	// header size
			p -> ControlCode	= IOCTL_SCSI_MINIPORT_IDENTIFY;
			// ^^^ code of the command passed to the driver

			// Signaturer. For ATAPI.SYS it is "SCSIDISK"
			strncpy ((char *) p -> Signature, "SCSIDISK", 8);

			// PREPARING THE SENDCMDINPARAMS STRUCTURE
			// containing ATA commands passed to the IDE drive
			pin -> bDriveNumber				= LU;
			pin -> irDriveRegs.bCommandReg	= IDE_ATA_IDENTIFY;

			// SENDING THE CONTROL REQUEST TO THE MINIPORT DRIVER
			if (DeviceIoControl (h, IOCTL_SCSI_MINIPORT, buffer,
			sizeof (SRB_IO_CONTROL) + sizeof (SENDCMDINPARAMS) - 1,
            buffer, sizeof (SRB_IO_CONTROL) + SENDIDLENGTH,&returned, 0))
					if (buffer[98]!=0)
					{	// in response you'll get the row with IDE
						// drive identifier, which we display on the screen
						for (a = 98; a < 136; a+=2 )
                        //for (a = 0; a < 436; a+=2 )
						printf("%c%c",buffer[a+1],buffer[a]); printf("\n");
					}
		}
		CloseHandle (h);		// close the handle for this SCSI miniport
	}
	return 0;
}

                                                    

int main()                                             
{
	ATAPI_MINIPORT_DEMO();
   return 0;
}


