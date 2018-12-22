/*----------------------------------------------------------------------------
 *
 *								SCSI lib
 *								========
 *
 *		Library for working with SCSI devices by Kris Kaspersky
 * is a wrapper for ASPI32/SPTI interfaces plus the set of 
 * function prototypes for working with CD-R/CD-RW drives
 *
 * vesion 0.0	unified SEND_SCSI_CMD,
 *				scanning ASPI devices
 *
 * vesion 0.1	possibility of addressing devices by names (TEAC, NEC...)
------------------------------------------------------------------------------*/
// BUGFIX LIST
// 1) fixed an error - SCSI_BEGIN didn't process SCSI_NEED_SKIP

#ifndef __SCSI_LIB_H__
#define __SCSI_LIB_H__
#include <windows.h>
#include <stdio.h>
#include "SPTI.h"
#include "ASPI32.h"

#ifndef	SCSI_VER
	#define	SCSI_VER	0x0001
#endif

#ifndef SCSI_VER
	#define	SCSI_LIC	"GNU license"
#endif

#ifndef ONE_WAIT						// time to wait between attempts
	#define ONE_WAIT	600
#endif

#ifndef MAX_WAIT						// number of attempts
	#define MAX_WAIT	4
#endif

#ifndef	NO_SENSE
	#define NO_SENSE	0
#endif

#ifndef SENSE_SIZE
	#define SENSE_SIZE	((SPTI_MAX_SENSE_SIZE>ASPI_MAX_SENSE_SIZE)?ASPI_MAX_SENSE_SIZE:SPTI_MAX_SENSE_SIZE)
#endif

#ifndef NO_BUF
	#define	NO_BUF		0, 0
#endif

#ifndef ASPI_BUS_SCAN
	#define ASPI_BUS_SCAN	"?.?", 0, 0, 0, 0, 0, 0
#endif

#define	SCSI_NEED_SKIP	0xFF

#define SCSI_DATA_IN			66
#define SCSI_DATA_OUT			99


//=[TRG & LUN]================================================================
//
//		Defines the method of interpreting the cd key specified by ASPI rules
//
//	ARG:
//		cd		-	key in ASPI format (format is determined by these functions,
//					default value is TRG.LUN)
//
//	RET:
//					address of the int TRG or LUN types, for TRG and LUN, respectively
//
//	NOTE:
//		TRG and LUN functions can be redefined by the user
//		into the custom interpretation format
#ifndef TRG
	#define TRG(cd)		atol(&cd[0])
#endif

#ifndef LUN
	#define LUN(cd)		atol(&cd[2])
#endif

//=[WDV]====================================================================
//
//		checks if this key is the key specified in the SPTI format
//		(by default)
//
//	FIXME:
//		not use right
#ifndef isSPTI
	#define isSPTI(cd)	((cd[0]=='\\')?iSPTI : 0)
	#define iSPTI		6
#endif

//=[isASPI]===================================================================
//
//		defines the CD key format
//
//	ARG:
//		cd		-	string with CD key
//
//	RET:
//		== 0		key in the SPTI format
//		== 1		key in the ASPI format
//		>= 2		reserved for futuer use
//
//	NOTE:
//		The function can be redefined by the user. For example, it is possible
// to explicitly specify "ASPI" ,"SPTI" words or use any other
// syntax
#ifndef isASPI
	#define iASPI		9
	#define isASPI(cd)	((cd[1] == '.')?iASPI:0) //&& (TRG(cd) + LUN(cd))) ? iASPI : 0)
#endif



#ifndef getInterface
	#define getInterface(cd)	(isSPTI(cd)?isSPTI(cd):isASPI(cd))
#endif

//=[SEND_SCSI_CMD]============================================================
//
//		Sends various SCSI commands to the device using the chosen interface
// and waits for their accomplishment (unified wrapper for SEND_SPTI_CMD and SEND_ASPI_CMD)
//
//	ARG:
//		cd		-	the key specifying the target device and defining interaction
//					interface;
//	
//		CDB		-	CDB packet sent to device
//
//		CDB_sz	-	size of CDB packet in bytes
//
//		SENSE	-	buffer for SENSE-info of the size SENSE_SIZE
//
//		buf		-	buffer for data exhcnage with device
//
//		buf_sz	-	buffer size
//
//		flag	-	data direction
//
// RET:
//		== SCSI_OK	success
//		!= SCSI_OK	errors occurred (see SENSE)
//
SEND_SCSI_CMD(char *cd, char *CDB, int CDB_sz, char *SENSE, unsigned char *buf, int buf_sz, int flag)
{
	int stat;
	int	my_interface;

	// scanning the bus and displaying identifiers for all devices
	//------------------------------------------------------------------------
	if (cd[0] == '?') { ASPI_SEND_SCSI_INQUITY(0,0); return SCSI_NEED_SKIP; }

	// defining the interface for interaction
	//------------------------------------------------------------------------
	my_interface = getInterface(cd);

	if (!my_interface && strlen(cd)>2) 
	{
			 ASPI_SEND_SCSI_INQUITY(cd, cd); my_interface = iASPI;
	}

	if (my_interface == iASPI)
	{											// ASPI

		// setting flags to ASPI
		switch(flag)
		{
			case SCSI_DATA_IN:	flag = ASPI_DATA_IN; break;
			case SCSI_DATA_OUT:	flag = ASPI_DATA_OUT; break;
			default:			return -1;
		}

		// passing the command to the device through ASPI

		stat = SEND_ASPI_CMD(TRG(cd), LUN(cd), CDB, CDB_sz, SENSE, buf, buf_sz, flag);
		return stat;
	}
	
	if (my_interface == iSPTI)
	{											// SPTI

		// setting flags to SPTI
		switch(flag)
		{
			case SCSI_DATA_IN:	flag = SPTI_DATA_IN; break;
			case SCSI_DATA_OUT:	flag = SPTI_DATA_OUT; break;
			default:			return -1;
		}

		// passing the command to the device through SPTI
		stat = SEND_SPTI_CMD(cd, CDB, CDB_sz, SENSE, buf,buf_sz, flag);
		return stat;
	}

	// returning status
	return 0;
}

//=[ALT]======================================================================
//
//		alternative function names
SCSI_SEND_CMD(char *cd, char *CDB, int CDB_sz, char *SENSE, unsigned char *buf, int buf_sz, int flag)
{
	return SEND_SCSI_CMD(cd, CDB, CDB_sz, SENSE, buf, buf_sz, flag);
}

ATAPI_SEND_CMD(char *cd, char *CDB, int CDB_sz, char *SENSE, unsigned char *buf, int buf_sz, int flag)
{
	return SEND_SCSI_CMD(cd, CDB, CDB_sz, SENSE, buf, buf_sz, flag);
}

IDE_SEND_CMD(char *cd, char *CDB, int CDB_sz, char *SENSE, unsigned char *buf, int buf_sz, int flag)
{
	return SEND_SCSI_CMD(cd, CDB, CDB_sz, SENSE, buf, buf_sz, flag);
}

//=[SCSI_IS_READY]============================================================
//
//		testing the device for readiness
//
//	ARG:
//		cd		-	device key
//	RET:
//		== SCSI_OK	device ready
//		!= SCSI_OK	device not ready
SCSI_IS_READY(char *cd)
{
	unsigned char CDB[ATAPI_CDB_SIZE]; memset(CDB,0, ATAPI_CDB_SIZE);
	return SEND_SCSI_CMD(cd, (char *)CDB, ATAPI_CDB_SIZE, NO_SENSE, NO_BUF, SCSI_DATA_IN);
}


//=[SCSI_BEGIN]===============================================================
//
//		wait until the device is ready
//
//	ARG:
//		cd		-	device key
//	RET:
//		== SCSI_OK	device ready
//		!= SCSI_OK	device not ready
//
//	NOTE:
//		waiting interval is specified through MAX_WAIT and ONE_WAIT (see above)
//
SCSI_BEGIN(char *cd)
{
	int a, status;
	for (a = 0; a < MAX_WAIT; a++)
	{
		status = SCSI_IS_READY(cd);
		if (( status == SCSI_OK) || (status == SCSI_NEED_SKIP)) return status;
		Sleep(ONE_WAIT); if (a) printf("\x7");
	}

	return ~SCSI_OK;
}

#endif //__SCSI_LIB_H__