/*----------------------------------------------------------------------------
 *
 *				READING A SECTOR FROM THE CD-ROM IN RAW MODE VIA ASPI
 *				=====================================================
 *
 * build 0x001 @ 29.05.2003
----------------------------------------------------------------------------*/
#include <windows.h>
#include <stdio.h>
#include "scsidefs.h"
#include "wnaspi32.h"

void ASPI32Post (LPVOID);

#define F_NAME			"raw.sector.dat"

/* ASPI SRB packet length */
#define ASPI_SRB_LEN	0x100

#define RAW_READ_CMD	0xBE

#define WHATS_READ		0x10 //0xF8	// Sync & All Headers & User Data + EDC/ECC
#define PACKET_LEN		2352

//#define WHATS_READ	0x10	// User Data
//#define PACKET_LEN	2048


#define MY_CMD			RAW_READ_CMD

HANDLE hEvent;

//-[DWORD READ_RAW_SECTOR_FROM_CD]---------------------------------------------
//		This function reads one or more sectors from CDROM in RAW mode,
// according to the flags passed to it
//
//	ARG:
//		adapter_id	-	bus number (0 - primary, 1 - secondary)
//		read_id		-	device number (0 - master, 1 - slave)
//		buf			-	read buffer
//		buf_len		-	buffer size
//		StartSector	-	start sector
//		N_SECTOR	-	number of sectors to be read
//		flags		-	what to read (see ATAPI specification)
//
//	RET:
//					-	do not return anything
//
//	NOTE:
//		- The function returns control before completing the request,
//		  thefore the data buffer is empty for the moment of exiting.
//		  The buffer will be actually filled only when the 
//		  ASPI32Post function is called (you can modify it as desired)
//		  to signal about operation termination it is recommended to
//		  use events
//
//		- This function works under 9x/ME/NT/W2K/XP and doesn't require
//        administrative privileges. However, ASPI driver must be installed
//-----------------------------------------------------------------------------
READ_RAW_SECTOR_FROM_CD(int adapter_id,int read_id,char *buf,int buf_len,
							  int StartSector,int N_SECTOR,int flags)
{
	PSRB_ExecSCSICmd SRB;
	DWORD	ASPI32Status;

	// Allocating memory for SRB request
	SRB = malloc(ASPI_SRB_LEN); memset(SRB, 0, ASPI_SRB_LEN);

	// PREPARING SRB BLOCK
	SRB->SRB_Cmd		= SC_EXEC_SCSI_CMD;			// execute SCSI command
	SRB->SRB_HaId		= adapter_id;				// adapter ID
	SRB->SRB_Flags		= SRB_DIR_IN|SRB_POSTING;	// Async. data read
	SRB->SRB_Target		= read_id;					// device ID
	SRB->SRB_BufPointer	= buf;						// data buffer
	SRB->SRB_BufLen		= buf_len;					// buffer length
	SRB->SRB_SenseLen	= SENSE_LEN;				// SENSE buffer length
	SRB->SRB_CDBLen		= 12;						// ATAPI packet size

	SRB->CDBByte [0]	= MY_CMD;					// ATAPI command
	SRB->CDBByte [1]	= 0x0;						// CD format - any

						// first sector number
	SRB->CDBByte [2]	= HIBYTE(HIWORD(StartSector));
	SRB->CDBByte [3]	= LOBYTE(HIWORD(StartSector));
	SRB->CDBByte [4]	= HIBYTE(LOWORD(StartSector));
	SRB->CDBByte [5]	= LOBYTE(LOWORD(StartSector));

						// number of sectors to be read
	SRB->CDBByte [6]	= LOBYTE(HIWORD(N_SECTOR));
	SRB->CDBByte [7]	= HIBYTE(LOWORD(N_SECTOR)); 
	SRB->CDBByte [8]	= LOBYTE(LOWORD(N_SECTOR)); 

	SRB->CDBByte [9]	= flags;					// what to read?
	SRB->CDBByte [10]	= 0;						// subhcannel data not needed
	SRB->CDBByte [11]	= 0;						// reserved

	// Address of the procedure that will receive notifications
	SRB->SRB_PostProc	= (void *) ASPI32Post;

	// Sending SRB request to the device
	SendASPI32Command(SRB);

	// Returning before request accomplishment
	return 0;
}

//----------------------------------------------------------------------------
//		This callback function is called by ASPI driver itself and gets control
// when request is completed or in case of error.
// As parameter it gets the pointer to the instance of the
// PSRB_ExecSCSICmd structure containin all the required information (status,
// pointer to buffer, etc.)
//----------------------------------------------------------------------------
void ASPI32Post (void *Srb)
{
	FILE *f;

	// was our query successful?
	if ((((PSRB_ExecSCSICmd) Srb)->SRB_Status) == SS_COMP)
	{
		// YOU ARE FREE TO MODIFY THIS CODE AS DESIRED
		//-------------------------------------------------------
		// Writes sector contents into the file.
		// Attention: PSRB_ExecSCSICmd) Srb)->SRB_BufLen contains the buffer size
		// instead of the actual length of the read data. If the number of bytes
		// returned by the device is less that the buffer size, that its tail
		// will contain garbage! Here we use the SRB_BufLen field
		// only because when calling the SendASPI32Command function we carefully
		// track the matching between the buffer size and returned information 
		// length
		if (f=fopen(F_NAME, "w"))
		{
			// writing sector into a file
			fwrite(((PSRB_ExecSCSICmd) Srb)->SRB_BufPointer,1,
			((PSRB_ExecSCSICmd) Srb)->SRB_BufLen, f);
			fclose(f);
		}
		// beeping and unfreezing the flow to specify
		// that reading procedure has accomplished
		MessageBeep(0); SetEvent(hEvent);
		//--------------------------------------------------------
	}
}

main(int argc, char **argv)
{
	void *p; int buf_len, TIME_OUT = 4000;

	if (argc<5)
	{
		fprintf(stderr,"USAGE:\n\tRAW.CD.READ.EXE adapter_id"\
		", read_id, StartSector, n_sec\n"); return 0;
	}
	
	// Computing the buffer length and allocate memory for it
	// ATTENTION: in such a way it is possible to use no more than 64K
	// if you require larger buffers,
	// use the GetASPI32Buffer function
	buf_len = PACKET_LEN*atol(argv[4]); p = malloc(buf_len*2);
	memset(p,0x55,buf_len*2);

	// creating event
	if ((hEvent = CreateEvent(NULL,FALSE,FALSE,NULL)) == NULL) return -1;

	// reading one or more sectors from CD
	READ_RAW_SECTOR_FROM_CD(atol(argv[1]), atol(argv[2]),p,buf_len,
							atol(argv[3]), atol(argv[4]),WHATS_READ);

	// waiting for completion
	WaitForSingleObject(hEvent, TIME_OUT);

	return 0;
}
