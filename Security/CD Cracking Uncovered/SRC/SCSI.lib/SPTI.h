/*----------------------------------------------------------------------------
 *
 *								SPTI wrapper
 *								============
 *
 *		Ensures unified wrapper for SPTI interface by implementing the
 * SEND_SCSI_CMD function with alternative name  SCSI_SEND_CMD,  sending
 * commands to the device through SPTI.
 *		SPTI interface is available only in Windows NT/2000/XP and requires
 * administrative privileges, otherwise it returns error
 *
----------------------------------------------------------------------------*/
#ifndef __KPNC_SPTI_H__
#define __KPNC_SPTI_H__
#include <windows.h>
#include <stdio.h>

// MAIN CONFIGURATION PARAMETERS
//----------------------------------------------------------------------------
#ifndef SCSI_TIME_OUT					// Time-out
	#define SCSI_TIME_OUT				90000		// ***
#endif

//=[FACK_ERR]=================================================================
//
//		For displaying error messages the FACK_ERR(s,x) function is used,
// where s - is the string in printf format, and x - additional argument of
// any type
//		If __GUI__ is defined, MessageBox will be used for displaying error 
// messages (to achieve this, it is necessaryd to include USER32.lib), otherwise 
// the function will carry out console output)
//		If desired, customize the FACK_ERR function
#ifndef FACK_ERR
	#ifdef __GUI__
		#define FACK_ERR(s,x) MessageBox(0, _NEO_BUF, 0, (sprintf(_NEO_BUF,s,x)&0)|MB_OK);
	#else
		#define FACK_ERR(s,x) fprintf(stderr,"%s %s\n",s, x)
	#endif
#endif


#define SPTI_SCSI_OK					1

#define SCSI_IOCTL_DATA_OUT				0
#define SCSI_IOCTL_DATA_IN				1
#define SCSI_IOCTL_DATA_UNSPECIFIED		2

#define SPTI_DATA_IN					SCSI_IOCTL_DATA_IN
#define SPTI_DATA_OUT					SCSI_IOCTL_DATA_OUT

#define SPTI_SENSE_IN_POS				(sizeof(SCSI_PASS_THROUGH))
#define SPTI_MAX_SENSE_SIZE				0x100		// ***

#define IOCTL_SCSI_PASS_THROUGH_DIRECT	0x4D014		// from ntddscsi.h

typedef struct _SCSI_PASS_THROUGH					// from ntddscsi.h
{
	USHORT	Length;
	UCHAR	ScsiStatus;
	UCHAR	PathId;
	UCHAR	TargetId;
	UCHAR	Lun;
	UCHAR	CdbLength;
	UCHAR	SenseInfoLength;
	UCHAR	DataIn;
	ULONG	DataTransferLength;
	ULONG	TimeOutValue;
	UCHAR*	DataBufferOffset;					// as SCSI_PASS_THROUGH_DIRECT
	ULONG	SenseInfoOffset;
	UCHAR	Cdb[16];
} SCSI_PASS_THROUGH, *PSCSI_PASS_THROUGH;

#ifndef _NEO_BUF
	char _NEO_BUF[0x1000];
#endif


//=[SEND_SPTI_CMD]============================================================
//
//		send SCSI command to the device through SPTI interface and
// wait for completion (requires administrative privileges)
//
//	ARG:
//		driver	-	device (for example, "\\\\.\\X:" or "\\\\.\\CrRom0")
//		CDB		-	pointer to CDB packet
//		CDB_sz	-	size of CDB packet in bytes
//		SENSE	-	pointer to SENSE buffer (0 if SENSE-info is not needed)
//		buf		-	read/write buffer
//		buf_sz	-	buffer size
//		flag	-	SCSI_DATA_OUT/SCSI_IOCTL_DATA_IN/SCSI_DATA_UNSPECIFIED
//
//	RET:
//		== SCSI_OK	success
//		!=	SCSI_OK	errors (see SENSE)
//
SEND_SPTI_CMD(char *driver, char *CDB, int CDB_sz, char *SENSE, unsigned char *buf, int buf_sz, int flag)
{

	HANDLE					hDevice;
	SCSI_PASS_THROUGH		srb;
	DWORD					status;
	unsigned char			sense_buf[SPTI_SENSE_IN_POS + SPTI_MAX_SENSE_SIZE];
	DWORD					returned;

	// èinitialization
	//------------------------------------------------------------------------
	memset(sense_buf, 'U', SPTI_SENSE_IN_POS + SPTI_MAX_SENSE_SIZE);

	if (CDB_sz > 16) {
		fprintf(stderr, "-ERR: invalid SRB_sz\n");		return -1; }

	// opening device
	//------------------------------------------------------------------------
	hDevice = CreateFile (	driver, GENERIC_READ | GENERIC_WRITE,
							FILE_SHARE_READ | FILE_SHARE_WRITE,
							0, OPEN_EXISTING, 0, 0);

	// error handling
	//-----------------------------------------------------------------------
	if (hDevice == INVALID_HANDLE_VALUE)
	{
		FACK_ERR("-ERR: open device",driver);
		//fprintf(stderr, "-ERR: open device %s\n",driver); 
		return -1; }

	// forming srb
	//------------------------------------------------------------------------
	memset(&srb, 0, sizeof(SCSI_PASS_THROUGH));	// initialization

	// OPTIONS
	srb.Length				= sizeof(SCSI_PASS_THROUGH);
	srb.PathId				= 6;					// SCSI controller ID
	srb.TargetId			= 6;					// target device ID
	srb.Lun 				= 6;					// logical unit device ID
	srb.CdbLength			= (unsigned char)CDB_sz;// CDB packet length
	srb.SenseInfoLength		= SPTI_MAX_SENSE_SIZE;	// SenseInfo length
	srb.DataIn 				= (unsigned char)flag;	// data flow direction
	srb.DataTransferLength	= buf_sz;				// buffer size
	srb.TimeOutValue 		= SCSI_TIME_OUT;		// Out
	srb.DataBufferOffset	= buf;					// pointer to the buffer
	srb.SenseInfoOffset 	= SPTI_SENSE_IN_POS;	// sense offset SENSE in the flow
	memcpy(srb.Cdb, CDB, CDB_sz);					// CDB

	// sending SRB block to SCSI/ATAPI device
	status = DeviceIoControl(hDevice, IOCTL_SCSI_PASS_THROUGH_DIRECT,
							&srb, sizeof(SCSI_PASS_THROUGH),
							sense_buf, SPTI_MAX_SENSE_SIZE, &returned, FALSE);

	if (!status) return ~SPTI_SCSI_OK;

	CloseHandle(hDevice);							// close handle

	// defining the presence of SENSE_INFO
	//------------------------------------------------------------------------
	if ((returned > SPTI_SENSE_IN_POS) && SENSE)
	{
		memcpy(SENSE, &sense_buf[SPTI_SENSE_IN_POS], SPTI_MAX_SENSE_SIZE);
		return ~SPTI_SCSI_OK;
	}
		return SPTI_SCSI_OK;
		//((SCSI_PASS_THROUGH*)sense_buf)->ScsiStatus
		// returns != SCSI_OK even if operation is successful

}




#endif //__KPNC_SPTI_H__