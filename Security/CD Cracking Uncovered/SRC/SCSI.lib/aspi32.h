/*-[SCSI.h]-------------------------------------------------------------------
 *
 *				Unified wrapper for ASPI32
 *				=====================================
 *
 *		ASPI32 is the low-level tool for communicating with
 * SCSI/ATAPI devices from application level, developed by Adaptec.
 *		ASPI driver must be installed (can be downloaded from the company
 * site), there are implementations for WinNT/9x, MS-DOS, UNIX, Novell
 *		Doesn't require administrative privileges for controlling device, which
 * creates a security breach!
 *
 * build 0x001 @ 29.05.2003
 ----------------------------------------------------------------------------*/

#ifndef __KPNC_ASPI32_H__
#define __KPNC_ASPI32_H__

#include <windows.h>
#include "scsidefs.h"
#include "wnaspi32.h"

//=[GLOBAL VARIABLES]====================================================
//
int (__cdecl *GetASPI32SupportInfoX) () = 0;;
int (__cdecl *SendASPI32CommandX)(LPSRB* SRB) = 0;

//=[BASE CONSTANTS]========================================================
#define ASPI32_DLL_BASE_NAME	"WNASPI32.DLL"
#define ASPI32_DLL_ALT_NAME		"WNASPI32.DLL"

#define ASPI_DATA_IN	SRB_DIR_IN
#define ASPI_DATA_OUT	SRB_DIR_OUT
#define ASPI_TIME_OUT	900


#define ASPI_MAX_ID				8				// MAX DEVICES.
#define MAX_INFO_LEN			48
#define ASPI_SRB_LEN			0x100
#define ASPI_SENSE_LEN			SENSE_LEN		// in WINASPI32.h
#define ASPI_MAX_SENSE_SIZE		0x100
#define ASPI_SENSE_IN_POS		0x0

//=[e_ASPI32_NOT_FOUND]=======================================================
//
//		Displaying message that ASPI32 is not installed and exiting
e_ASPI32_NOT_FOUND()
{

	fprintf(stderr,"-ERR:ASPI32 library not found\x7\n");
	_exit(0);
	return 0;
}

//=[_ASPI32_INIT]=============================================================
//
//		ASPI32 initialization, - ASPI32-DLL is loaded dynamically
// it is not needed to call this function, because it will be called
// automatically when first accessing wrappers
//
_ASPI32_INIT()
{
	HINSTANCE h;

	if (SendASPI32CommandX) return 1;

	if ((h = LoadLibrary(ASPI32_DLL_BASE_NAME)) == 0) e_ASPI32_NOT_FOUND();

	SendASPI32CommandX = (int (__cdecl *)(LPSRB*))GetProcAddress(h, "SendASPI32Command");
	GetASPI32SupportInfoX = (int (__cdecl *)()) GetProcAddress(h, "GetASPI32SupportInfo");

	return 3;
}

//=[SEND_ASPI_CMD]============================================================
//
//		Send SCSI command to the device through ASPI and wait
// for completion (or return error code)
//
//	ARG:
//		adapter_id	-	controller number (0 - primary, 1 - secondary)
//		read_id		-	device ID (0 - master, 1 - slaeyer)
//		СDB			-	CDB block passed to the device
//		CDB_sz		-	CDB block size in bytes
//		SENSE		-	buffer for SENSE-info
//		buf			-	read buffer
//		buf_sz		-	buffer size (64 Kb max)
//		flags		-	== ASPI_DATA_IN  : device --> computer
//						== ASPI_DATA_OUT : compuiter  --> device
//
//	RET:
//						see SRB_Status (SS_COMP [0x1] if ALL OK)
//
//	
// 
#define _S_ERR_EVENT	"-ERR:CREATE EVENT\x7"
SEND_ASPI_CMD(int adapter_id, int read_id, char *CDB, int CDB_sz, char *SENSE, unsigned char *buf, int buf_sz, int flag)
{
	
	DWORD				ASPIStatus;
	DWORD				ASPIEventStatus;
	HANDLE				ASPICompletionEvent;
	SRB_ExecSCSICmd		*SRB;

	// initialization
	_ASPI32_INIT();
	// if (SENSE) ( strcpy(&SENSE[1], _S_ERR_EVENT) &&	(SENSE[0] = strlen(_S_ERR_EVENT)));

	// create even object for determining the fact of SCSI request termination
	if ((ASPICompletionEvent = CreateEvent(0,0,0,0)) == 0) return SS_ERR;

	// Allocating memory for SRB request
	SRB= (SRB_ExecSCSICmd*) malloc(sizeof(SRB_ExecSCSICmd));memset(SRB,0,sizeof(SRB_ExecSCSICmd));

	// init II
	if (SENSE) memset(SENSE, 'U', ASPI_MAX_SENSE_SIZE);

	// PREPARING SRB BLOCK
	SRB->SRB_Cmd		= SC_EXEC_SCSI_CMD;			// execute SCSI command
	SRB->SRB_HaId		= (unsigned char)adapter_id;// ID адаптера
	SRB->SRB_Flags		= flag | SRB_EVENT_NOTIFY;	// async mode
	SRB->SRB_Target		= read_id;					// device ID
	SRB->SRB_BufPointer	= buf;						// pointer to buffer
	SRB->SRB_BufLen		= buf_sz;					// buffer length
	SRB->SRB_SenseLen	= ASPI_SENSE_LEN;			// SENSE buffer length
	SRB->SRB_CDBLen		= (unsigned char)CDB_sz;	// size of ATAPI packet
	SRB->SRB_PostProc	= ASPICompletionEvent;		// return event
	memcpy(SRB->CDBByte, CDB, CDB_sz);				// pass CDB block

	// Execute SCSI command through ASPI
	ASPIStatus			= SendASPI32CommandX ( (LPSRB*) SRB );

	// Wait until command completion
	if (SRB->SRB_Status == SS_PENDING)
		ASPIEventStatus = WaitForSingleObject(ASPICompletionEvent,ASPI_TIME_OUT);
	
	// switch event status
	if (ASPIEventStatus == WAIT_OBJECT_0) 
	{
		ResetEvent(ASPICompletionEvent);
		//CloseHandle(ASPICompletionEvent);
	}

	// return SENSE info if necessary
	if (SENSE) memcpy(SENSE, SRB->SenseArea, SENSE_LEN);

	ASPIStatus = SRB->SRB_Status;
			CloseHandle(ASPICompletionEvent);
	
	free(SRB); 

	// return status code
	return ASPIStatus ;
}



#define MAX_ID					8


#define SCSI_INQUITY			0
#define NO_SENSE				0
#define SCSI_OK					1
#define READY					1

//=[ASPI_SEND_SCSI_INQUITY]===================================================
//
//		Output information about devices connected to computer
//
//
ASPI_SEND_SCSI_INQUITY(char *buf_dst, char *buf_src)
{

	#define _ALLOC_LEN	0xFF		// max. length of device response
	#define MAX_LUN		0x10

	int a, adapterid, targetid;
	//int 	my_devices = 0;
	unsigned char buf[_ALLOC_LEN];
	unsigned char str[_ALLOC_LEN];
	unsigned char CDB[ATAPI_CDB_SIZE]; 
	long real_len;

	BYTE	AdapterCount;
	DWORD	ASPI32Status;

	// initialization
	//------------------------------------------------------------------------
	_ASPI32_INIT(); memset(CDB, 0, ATAPI_CDB_SIZE);

	// getting status info
	//------------------------------------------------------------------------
	ASPI32Status = GetASPI32SupportInfoX();

	// getting number of controllers
	AdapterCount = (LOBYTE(LOWORD(ASPI32Status)));

	if (AdapterCount == 0) return 0;		// no adapters found

	// preparing CDB for INQUIRY
	//------------------------------------------------------------------------
	CDB[0] = 0x12;							// INQUIRY
	CDB[4] = _ALLOC_LEN;					// Allocation Length

	if (!buf_src && !buf_dst) printf("\n");

	// polling all adapter in loop
	for (adapterid = 0; adapterid < MAX_LUN; adapterid++)
	{
		// polling all devices on the current adapter
		for (targetid = 0; targetid < ASPI_MAX_ID; targetid++)
		{
			// sending the INQUIRY command to the device
			a = SEND_ASPI_CMD(	adapterid, targetid, (char *) CDB, ATAPI_CDB_SIZE,
								NO_SENSE, buf, _ALLOC_LEN, ASPI_DATA_IN);

			// was request successful?
			if (a == SS_COMP)
			{
				// determining the real length of device response
				real_len=(buf[4] > MAX_INFO_LEN) ? buf[4] : MAX_INFO_LEN;

				// retrieving the device ID
				memcpy(str, &buf[8], real_len); str[real_len] = 0;
			
				// IF DO...	<--- if the DO flag appears

				
				if (buf_src)
				{						// buf_src !=0: search drive
					if (strstr((char *)str, buf_src)) {
						sprintf(buf_dst,"%d.%d",adapterid,targetid);
						return 1;
					}

						
				}
					else
				{						// buf_src == 0: print info
					if (buf_dst)
						sprintf(buf_dst,"%d.%d <-- %s (%d)\n",adapterid,targetid,str,buf[0] & 0x1F);
					else
						printf("%d.%d <-- %s (%d)\n",adapterid, targetid, str, buf[0] & 0x1F);

				}	// if else buf_src

			}	// if a == SS_COMP
		}	// for target
	}	// for adapter

	return 1;
}


ASPI_SEND_SCSI_INQUITY_EX(char *LIST, char *buf_dst, char *buf_src, int DEV_TYPE)
{

	#define _ALLOC_LEN	0xFF		// max length of the device response
	#define MAX_LUN		0x10

	int a, adapterid, targetid;
	//int 	my_devices = 0;
	unsigned char buf[_ALLOC_LEN];
	unsigned char str[_ALLOC_LEN];
	unsigned char CDB[ATAPI_CDB_SIZE]; 
	long real_len;

	BYTE	AdapterCount;
	DWORD	ASPI32Status;

	// initialization
	//------------------------------------------------------------------------
	_ASPI32_INIT(); memset(CDB, 0, ATAPI_CDB_SIZE); if (LIST) *LIST = 0;


	// retrieving the status info
	//------------------------------------------------------------------------
	ASPI32Status = GetASPI32SupportInfoX();

	// getting the number of controllers
	AdapterCount = (LOBYTE(LOWORD(ASPI32Status)));

	if (AdapterCount == 0) return 0;		// no controllers found

	// preparing CDB for INQUIRY
	//------------------------------------------------------------------------
	CDB[0] = 0x12;							// INQUIRY
	CDB[4] = _ALLOC_LEN;					// Allocation Length

	if (!buf_src && !buf_dst && !LIST) printf("\n");
	//if (!buf_src && !buf_dst) printf("\n");

	// polling all adapters in the loop
	for (adapterid = 0; adapterid < MAX_LUN; adapterid++)
	{
		// polling all devices on the current adapter
		for (targetid = 0; targetid < ASPI_MAX_ID; targetid++)
		{
			// semdomg tje INQUIRY command to the device
			a = SEND_ASPI_CMD(	adapterid, targetid, (char *) CDB, ATAPI_CDB_SIZE,
								NO_SENSE, buf, _ALLOC_LEN, ASPI_DATA_IN);

			// was request successful?
			if (a == SS_COMP)
			{
				// determining the actual length of the device response
				real_len=(buf[4] > MAX_INFO_LEN) ? buf[4] : MAX_INFO_LEN;

				// retrieving device ID
				memcpy(str, &buf[8], real_len); str[real_len] = 0;
			
				// IF DO...	<--- если появится флаг DO
				if (LIST)
				{
					if ((buf[0] & 0x1F) == DEV_TYPE)
					{
						sprintf(&LIST[strlen(LIST)],"%d.%d <-- %s\n",adapterid,targetid, str);
					}
				}
				else
				{

					if (buf_src)
					{						// buf_src !=0: search drive
						if (strstr((char *)str, buf_src)) {
							sprintf((char* )buf_dst,"%d.%d",adapterid,targetid);
							return 1;
						}

						
					}
						else
					{						// buf_src == 0: print info
						if (buf_dst)
							sprintf(buf_dst,"%d.%d <-- %s (%d)\n",adapterid,targetid,str,buf[0] & 0x1F);
						else
							printf("%d.%d <-- %s (%d)\n",adapterid, targetid, str, buf[0] & 0x1F);
					}	// if else buf_src

				}	
			}	// if a == SS_COMP
		}	// for target
	}	// for adapter

	return 1;
}



#define SCSI_INQ	0x0

#endif //__KPNC_ASPI32_H__