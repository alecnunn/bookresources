/*----------------------------------------------------------------------------
 *
 *				READING SECTOR FROM CD-ROM IM RAW MODE VIA ASPI
 *				=================================================
 *
 * build 0x001 @ 29.05.2003
----------------------------------------------------------------------------*/
#include <windows.h>
//#include "CD.h"
//#include "SPTI.h"
#include "scsidefs.h"
#include "wnaspi32.h"

#define TRG(cd)	atol(&cd[0])
#define LUN(cd)	atol(&cd[2])
#define isASPI(cd) (((cd[0]!='\\') && (TRG(cd)+LUN(cd)))?1:0)


#define ASPI_SRB_LEN	0x100
#define ASPI_SENSE_IN_POS	0x0

#define MAX_SENSE_SIZE 0x100
//-[DWORD READ_RAW_SECTOR_FROM_CD]---------------------------------------------
//		This function reads one or more sectors from CDROM in RAW mode,
// according to flags passed to it
//
//	ARG:
//		adapter_id	-	bus number (0 - primary, 1 - secondary)
//		read_id		-	device number (0 - master, 1 - slave)
//		buf			-	read buffer
//		buf_len		-	buffer size in bytes
//		StartSector	-	start sector
//		N_SECTOR	-	sectors to read
//		flags		-	what to read (see ATAPI specification)
//
//	RET:
//					-	doesn't return anything
//
//	NOTE:
//		- this function works both 9x/ME/NT/W2K/XP and doesn't require 
//		  administrative privileges. However, ASPI driver must be installed
//-----------------------------------------------------------------------------
//SEND_SPTI_CMD(char *driver,char *SRB,int SRB_sz, char *SENSE, char *buf, int buf_sz, int flag, DWORD *returned)

#define ASPI_DATA_IN	SRB_DIR_IN      
#define ASPI_DATA_OUT	SRB_DIR_OUT
#define ASPI_TIME_OUT	90000
SEND_ASPI_CMD(int adapter_id, int read_id, char *CDB, int CDB_sz, char *SENSE, char *buf, int buf_sz, int flag)
{
	
	PSRB_ExecSCSICmd SRB;
	DWORD	ASPIStatus;
	DWORD ASPIEventStatus;

	HANDLE ASPICompletionEvent;

	if ((ASPICompletionEvent = CreateEvent(0,0,0,0)) == 0)
	{
		printf("-ERR:CreateEvent\n"); return -1;
	 }

	// Allocating memory for SRB request
	SRB = malloc(sizeof(SRB_ExecSCSICmd)); memset(SRB, 0, sizeof(SRB_ExecSCSICmd));
	//printf("3 pass %x\n",sizeof(SRB_ExecSCSICmd));

	// Preparing SRB block
	SRB->SRB_Cmd		= SC_EXEC_SCSI_CMD;			// execute SCSI command
	SRB->SRB_HaId		= adapter_id;				// adapter ID
	SRB->SRB_Flags		= flag | SRB_EVENT_NOTIFY;	
	SRB->SRB_Target		= read_id;					// device ID
	SRB->SRB_BufPointer	= buf;						// data read buffer
	SRB->SRB_BufLen		= buf_sz;					// buffer length
	SRB->SRB_SenseLen	= SENSE_LEN;				// SENSE buffer length
	SRB->SRB_CDBLen		= CDB_sz;					// ATAPI packet size
	SRB->SRB_PostProc	= ASPICompletionEvent;
	
	memcpy(SRB->CDBByte, CDB,  CDB_sz);
	ASPIStatus      = SendASPI32Command ( (LPSRB) SRB );
	
	if (SRB->SRB_Status == SS_PENDING )
	   ASPIEventStatus = WaitForSingleObject(ASPICompletionEvent, ASPI_TIME_OUT);
	
	if (ASPIEventStatus == WAIT_OBJECT_0) 
		  ResetEvent(ASPICompletionEvent);
	
	if (SENSE) memcpy(SENSE, SRB->SenseArea, SENSE_LEN);
	// Returning from the function before the request is accomplished
	
	return SRB->SRB_Status;
}


#define MAX_ID           		8
#define MAX_INFO_LEN			48
#define SCSI_INQUITY			0
#define SCSI_DATA_IN					SCSI_IOCTL_DATA_IN
#define SCSI_DATA_OUT					SCSI_IOCTL_DATA_OUT
#define NO_SENSE						0
#define SCSI_OK							1
#define READY							1


SEND_SCSI_INQUITY()
{
	int a, adapterid, targetid;
	unsigned char buf[0xFF];
	unsigned char str[0xFF];	
	unsigned char CDB[ATAPI_CDB_SIZE]; 
	long real_len;


	BYTE 	 AdapterCount;
	DWORD	ASPI32Status;

	ASPI32Status = GetASPI32SupportInfo();
  	AdapterCount = (LOBYTE(LOWORD(ASPI32Status)));
   	memset(CDB, 0, ATAPI_CDB_SIZE);
    CDB[0] = 0x12;
    CDB[4] = 0xFF;

  	if (AdapterCount)
  	{
		for (adapterid = 0; adapterid < AdapterCount; adapterid++)
		{
			for (targetid = 0; targetid < MAX_ID; targetid++)
			{
				a = SEND_ASPI_CMD(adapterid, targetid, CDB, ATAPI_CDB_SIZE, 0, buf, 0xFF, ASPI_DATA_IN);
				if (a == SS_COMP)
				{
					real_len=(buf[4]>MAX_INFO_LEN)?buf[4]:MAX_INFO_LEN;
					memcpy(str,&buf[8],real_len);str[real_len]=0;
					printf("%d.%d <-- %s\n",adapterid, targetid, str);
				}	
			}
		}	
	}
	return SCSI_INQUITY;	
}

#define SCSI_INQ	0x0
SEND_SCSI_CMD(char *cd, char *CDB, int CDB_sz, char *SENSE, char *buf, int buf_sz, int flag)
{
	int stat;
	if (cd[0]=='?')
	{
		SEND_SCSI_INQUITY();
		_exit(0);
		return 0;
	}
	if (isASPI(cd))
	{
			if (flag == SCSI_DATA_IN) flag = ASPI_DATA_IN;
				else if (flag == SCSI_DATA_OUT) flag = ASPI_DATA_OUT;
					else return -1;

	    	stat = SEND_ASPI_CMD(TRG(cd), LUN(cd), CDB, CDB_sz, SENSE, buf,	buf_sz, flag);
	 }
		else
	{   	
   			stat = SEND_SPTI_CMD(cd,  CDB, CDB_sz, SENSE, buf,	buf_sz, flag);
	}
	return stat;
}


SCSI_IS_READY(char *cd)
{
	unsigned char CDB[ATAPI_CDB_SIZE];
	memset(CDB,0, ATAPI_CDB_SIZE);
	return SEND_SCSI_CMD(cd, CDB, ATAPI_CDB_SIZE, NO_SENSE, 0, 0, SCSI_DATA_IN);
}

#define ONE_WAIT	1000
#define MAX_WAIT	3
SCSI_BEGIN(char *cd)
{
	int a;
	for (a = 0; a < MAX_WAIT; a++)
	{
		if (SCSI_IS_READY(cd) == SCSI_OK) return SCSI_OK;
		//printf("\x7");
		 Sleep(MAX_WAIT);
	}
	return ~SCSI_OK;
}
