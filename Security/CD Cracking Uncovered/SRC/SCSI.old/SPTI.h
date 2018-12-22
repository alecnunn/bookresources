#include <windows.h>

#define SCSI_IOCTL_DATA_OUT          	0
#define SCSI_IOCTL_DATA_IN           	1
#define SCSI_IOCTL_DATA_UNSPECIFIED		2

#define SPTI_DATA_IN					SCSI_IOCTL_DATA_IN
#define SPTI_DATA_OUT					SCSI_IOCTL_DATA_OUT

#define IOCTL_SCSI_PASS_THROUGH 		0x4D004		// from ntddscsi.h
#define IOCTL_SCSI_PASS_THROUGH_DIRECT	0x4D014		// from ntddscsi.h4d014
#define IOCTL_SCSI_GET_ADDRESS 			0x41018

typedef struct _SCSI_PASS_THROUGH {			// from ntddscsi.h
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
} SCSI_PASS_THROUGH, *PSCSI_PASS_THROUGH;

typedef struct _SCSI_ADDRESS {
    ULONG Length;
    UCHAR PortNumber;
    UCHAR PathId;
    UCHAR TargetId;
    UCHAR Lun;
}SCSI_ADDRESS, *PSCSI_ADDRESS;


#define SENSE_IN_POS	(sizeof(SCSI_PASS_THROUGH))
#define MAX_SENSE_SZ	0xFFF

#define SPTI_SCSI_ERR		4
#define SPTI_SCSI_OK		1

// driver   - device (for example, "\\\\.\\X:" or "\\\\.\\CrRom0")
// SRB	    - pointer to SRB/ATAPI packet
// SRB_sz   - size of the SRB/ATAPI packet in bytes
// SENSE    - pointer to the SENSE buffer (0 if SENSE-info is not needed)
// SENSE_sz - size of the SENSE buffer
// buf      - read/write buffer
// buf_sz	- buffer size
// flag     - SCSI_IOCTL_DATA_OUT/SCSI_IOCTL_DATA_IN/SCSI_IOCTL_DATA_UNSPECIFIED
SEND_SPTI_CMD(char *driver,char *SRB,int SRB_sz, char *SENSE, char *buf, int buf_sz, int flag)
{
	#define SCSI_TIME_OUT	200
	HANDLE					hDevice;
	SCSI_PASS_THROUGH		srb;
	DWORD					length, status;	
	unsigned char 			sense_buf[SENSE_IN_POS+MAX_SENSE_SZ];
	DWORD					returned;	
	memset(sense_buf, 'U', SENSE_IN_POS+MAX_SENSE_SZ);

	// OPENING THE DEVICE
	hDevice = CreateFile (	driver, GENERIC_READ|GENERIC_WRITE,
						FILE_SHARE_READ|FILE_SHARE_WRITE,0,OPEN_EXISTING,0,0);
	if (hDevice == INVALID_HANDLE_VALUE)
	{
		fprintf(stderr, "-ERR: open device %s\n",driver);
		return -1;
	}

	if (SRB_sz > 16)
	{
		fprintf(stderr, "-ERR: invalid SRB_sz\n");
		return -1;
	}

	// FORMING SRB
	//------------------------------------------------------------------------
	memset(&srb, 0, sizeof(SCSI_PASS_THROUGH));	// initialization

	// OPTIONS
	srb.Length				= sizeof(SCSI_PASS_THROUGH);
	srb.PathId				= 2;					// SCSI controller ID
	srb.TargetId			= 2;          			// target device ID  
	srb.Lun 				= 2;           			// logical unit device ID
	srb.CdbLength			= SRB_sz;				// CDB packet length
	srb.SenseInfoLength		= MAX_SENSE_SZ;			// SenseInfo length
	srb.DataIn 				= flag;              	// we are going to read
	srb.DataTransferLength	= buf_sz;				// size of data that we are going to read/write
	srb.TimeOutValue 		= SCSI_TIME_OUT;		// TimeOut
	srb.DataBufferOffset	= buf;					// pointer to buffer
	srb.SenseInfoOffset 	= SENSE_IN_POS;				// 

	memcpy(srb.Cdb, SRB, SRB_sz);

	// SENDING SRB block to SCSI/ATAPI device
	status = DeviceIoControl(hDevice, IOCTL_SCSI_PASS_THROUGH,
			&srb, sizeof(SCSI_PASS_THROUGH), sense_buf, MAX_SENSE_SZ, &returned, FALSE);

	CloseHandle(hDevice);

	if ((returned > SENSE_IN_POS) && SENSE)
	{
		memcpy(SENSE, &sense_buf[SENSE_IN_POS], MAX_SENSE_SZ);
		return SPTI_SCSI_ERR;
	}
	else
	    return SPTI_SCSI_OK;

	return status;
}
