/*----------------------------------------------------------------------------
 *
 *								crackme.N2.c
 *								============
 *
 *		CD-ROM protection against copying based on nonstandard point in the
 * second session of the TOC, which cannot be returned by the READ TOC command
 * (RAW format), because the drive returns only the points "known" to it.
 * However, nonstandard point can be read at the level of subchannel data
 * using SEEK/READ SUB-CHANNEL commands. 
 *		This protection cannot be copied by any of the protected CD copiers
 * that exist nowadays, because they all rely on the READ TOC command.
 * The protection cannot even be emulated, because most virtual drives prohibit
 * reading subchannel data from Lead-In. However, the drives that I have tested
 * did allow to do so!
 *
 * PREPARING THE IMAGE:
 *		Create a two-session disc, leave the second session open,
 * read the image using ConeCD. Then add a new ENTRY into the second session
 * having nonstandard point (ABh by default), burn the image to disc, then
 * start Nero and write another session and close it. If this is not done, then
 * by some unknown reason, some drives, including ASUS 50x CD-ROM, will see
 * only the first session, locking all attempts at positioning on the Lead-In of
 * the second session).
 *		Insert this disc into the drive and start crackme.N2.exe, - key disc
 * must be recognized. If this doesn't happen, start crackme.N2.exe with the
 * -debug command-line option followed by the log file name and send the resulting
 * file to developer, i.e. kk@sendmail.ru
 * (for example:crackme.N2.exe -debug C:\TEMP\debug.log)
 *
 * KNOWN PROBLEMS:
 *		1) precise positioning to the specified sector in Lead-In area is
 * impossible, therefore we have to scan a long area, which is too slow.
 * In theory, the algorithm can be improved, by using the "fork" method,
 * however, these are plans for the future
 *		2) NEC drives (and, possibly, some other models) incorrectly display
 * points by returning them according to the following formula: NEC_PNT := 100 + PNT - A0h,
 * therefore, it can be assumed that there are drives that use custom conversion
 * scheme which is not known beforehand;
 *
 * FAQ:
 *
 * à) HOW TO ADD A NEW ENTRY INTO THE CCD FILE
 *		- open a ccd file using any text editor,
 *		- find the TocEntries field and increase its value by 1
 *		- find the [Entry] section with Point == 0x2 and Session == 2
 *		- copy this [Entry] into the clipboard
 *		- move the cursor to the end of this entry Entry
 *		- Paste the Entry from the clipboard
 *		- Increase the number of the inserted Entry by one
 *		- renumber all further Entries
 *		- change the Point of the inserted Entry to 0xAB
 *		- increase the PMin field of the inserted Entry by 1
 *		- save changes into the file
 *
 * á) WHERE TO GET Clone CD
 *		- http://www.slysoft.com/download/SetupCloneCD.exe
 *		- many other Internet sites (use search engines!)
 *
 * â) HOW ABOUT bit-hack'a
 *		- This protection can be easily cracked in any debugger! If you want 
 *		  to take counter-measures, use anti-debugging techniques.
----------------------------------------------------------------------------*/
#ifndef __CRACKME_N2_KERNEL_H__
#define __CRACKME_N2_KERNEL_H__

#include <stdio.h>
#include "..\CD.h"
#include "..\SCSI.h"


// determining address of the middle of the Lead-In area,
// storing the key mark


#define _TOC_TRACK_NUM_	(BUF[a + _off_TOC_TRACK_NUM])
#define _TOC_MIN_		(BUF[a + _off_TOC_MIN])
#define _TOC_SEC_		(BUF[a + _off_TOC_SEC])
#define _TOC_FRA_		(BUF[a + _off_TOC_FRA])

#define isValidLBA(a)	((a>0x69)?1:0)
//=[GetKeyMarkLBA(char *CD)]==================================================
//
//		Defining LBA address of the area containing the key mark.
// This area is in the middle of the Lead-In area located in the session 
// specified by  _MY_TRACK_)
//		The address is determined on the basis of the address of the1st track
// of the required session and subtracting GAP_SIZE - LEAD_SIZE/2 from it,
// which ensures positioning on the middle of the Lead-In area.
//		Special parameter _MY_POINT_IN_SEC_ allows precise tuning on the key
// mark (to speed up the search), however, do not forget that different drives
// have different systematic errors
//
//	ARG:
//		CD				-	address/name of the CD-ROM drive
//
//	RET:
//		_NEED_SKIP		-	mother function must be terminated
//		_NOT_READY		-	drive not ready or TOC read error
//		_NOT_SUPPORT	-	No second seesion in the TOC
//
GetKeyMarkLBA(char *CD)
{
	int a, len;

	unsigned char BUF[MAX_TOC_SIZE];
	unsigned char CDB[ATAPI_CDB_SIZE]; memset(CDB, 0, ATAPI_CDB_SIZE);

	_DBG_ fprintf(debug, "# GetKeyMarkLBA(%s) was called\n",CD, _FF_);

	// READ TOC (format 2 - RAW TOC)
	//------------------------------------------------------------------------
	CDB[0] = 0x43;					// READ TOC
	CDB[2] = 2;						// RAW TOC
	CDB[7] = HIBYTE(MAX_TOC_SIZE);	// max. length...
	CDB[8] = LOBYTE(MAX_TOC_SIZE);	//					...of the buffer

	// waiting for the device to become ready
	_DBG_ fprintf(debug, "# call SCSI_BEGIN(%s)\n",CD, _FF_);
	a = SCSI_BEGIN(CD);	if (a == SCSI_NEED_SKIP) return _NEED_SKIP;
						// if the device is not ready, continue execution
						// processing error in SEND_SCSI_CMD
	

	// pass READ TOC command
	_DBG_ fprintf(debug, "# call SEND_SCSI_CMD(/* READ RAW TOC */)\n", _FF_);
	a = SEND_SCSI_CMD(CD, (char *)CDB, ATAPI_CDB_SIZE, NO_SENSE, BUF, MAX_TOC_SIZE, SCSI_DATA_IN);
	if (a != SCSI_OK) return _NOT_READY;	// drvie not ready or TOC read error

	// defining full TOC length along with the header
	len = BUF[0]*0x100L + BUF[1]; len += 2; _DBG_ fprintf(debug, "# TOC len == %d\n",len, _FF_);

	// finding TOC ENTRY with MY_TACK and defining its LBA
	for ( a = _off_TOC_ENTRY; a < len; a += TOC_POINTER_SIZE)
	{
		_DBG_ fprintf(debug, "# TOC TRACK NUM: %02X\n", _TOC_TRACK_NUM_, _FF_);
		if (_TOC_TRACK_NUM_ == _MY_TRACK_)		// this is our track
		{
			// determining LBA or our track
			a = MSFtoLBA(_TOC_MIN_, _TOC_SEC_, _TOC_FRA_);
			_DBG_ fprintf(debug, "# _MY_TRACK_ LBA - %d\n", a, _FF_);

			// calculating address of the area with the key mark inside
			return a - GAP_SIZE - LEAD_SIZE/2 + _MY_POINT_IN_SEC_;
		}
	}

	// if we are here, then there is no MY_TRACK in TOC and this
	// means that the drive sees only the first session
	return _NOT_SUPPORT;
}

#define _MY_TRACK_RAW_MSF_ 1	

char* ASPI_GETNEXT_ADDR_IN_LIST(char *CD, char *LIST)
{
	if (CD && *LIST)
	{
		memcpy(CD, LIST, 3); CD[4] = 0;
		while(*LIST!=0)	if (*(++LIST) == 0xA) return ++LIST; return 0;
	}

	return 0;
}

// it is our CD? 
int isMyDEV(char *CD, char *LIST)
{
	int a, len;
	char *p;
	unsigned char BUF[MAX_TOC_SIZE];
	unsigned char CDB[ATAPI_CDB_SIZE]; memset(CDB, 0, ATAPI_CDB_SIZE);
	_DBG_ fprintf(debug, "# isMyDEV(,%s) was called\n", LIST, _FF_);
	while((p=ASPI_GETNEXT_ADDR_IN_LIST(CD, LIST)))
	{
		LIST = p;
		_DBG_ fprintf(debug, "# checking %s \n",CD, _FF_);

		// READ TOC (format 2 - RAW TOC)
		//------------------------------------------------------------------------
		CDB[0] = 0x43;					// READ TOC
		CDB[2] = 2;						// RAW TOC
		CDB[7] = HIBYTE(MAX_TOC_SIZE);	// max length...
		CDB[8] = LOBYTE(MAX_TOC_SIZE);	//					...of the buffer

			
		// waiting for the device to become ready
		_DBG_ fprintf(debug, "# call SCSI_BEGIN(%s)\n",CD, _FF_);
		a = SCSI_IS_READY(CD);  //if (a == SCSI_NEED_SKIP) return _NEED_SKIP;
		if (a != SCSI_OK)	continue;	// going to the next drive in the list

		// pass the READ TOC command for exectuion
		_DBG_ fprintf(debug, "# call SEND_SCSI_CMD(/* READ RAW TOC */)\n", _FF_);
		a = SEND_SCSI_CMD(CD, (char *)CDB, ATAPI_CDB_SIZE, NO_SENSE, BUF, MAX_TOC_SIZE, SCSI_DATA_IN);
		if (a != SCSI_OK) continue; 	// drive not ready or TOC read error

		// determining full TOC length along with the header
		len = BUF[0]*0x100L + BUF[1]; len += 2; _DBG_ fprintf(debug, "# TOC len == %d\n",len, _FF_);
	
		// Finding TOC ENTRY with MY_TACK and determining its LBA
		for ( a = _off_TOC_ENTRY; a < len; a += TOC_POINTER_SIZE)
		{
			_DBG_ fprintf(debug, "# TOC TRACK NUM: %02X\n", _TOC_TRACK_NUM_, _FF_);
			if (_TOC_TRACK_NUM_ == _MY_TRACK_)		// this is our track
			{
				// determining LBA or our track	
				a = MSFtoLBA(_TOC_MIN_, _TOC_SEC_, _TOC_FRA_);
				_DBG_ fprintf(debug, "# _MY_TRACK_ LBA - %d : %d\n", a, _MY_LBA_, _FF_);

				if (a == _MY_LBA_) return _MY_LBA_;
			}
		}

	}	
	// if we are here, then there is no MY_TRACK in TOC and this
	// means that the drive sees only the first session
	return _NOT_READY;
}



#define _NORMAL		0
#define _DANGER		1
#define _SATANA		2
#define _ABNORMAL	3

//=[_chk_point(int point)]=============================================[ICQ]==
//
//		Analysis of the point retrieved from TOC read at subchannel level
// with the account of behavior of certain drives (for the moment, only NEC
// is supported)
//
//	ARG:
//		point		-	point as it is
//
//	RET:
//		_NORMAL		-	track number
//		_DANGER		-	nonstandard point, but no key mark
//		_SATANA		-	recognized key mark
//		_ABNORMAL	-	something unnatural
//
int _ICQ_chk_point(int point)
{
	if (point <= 99) return _NORMAL;
	switch(point)
	{
		// standard service points
		case 0xA0:
		case 0xA1:
		case 0xA2:
		case 0xB0:
		case 0xB1:
		case 0xB2:
		case 0xB4:
		case 0xC0:
		case 0xC1:						return _NORMAL;

		// service points returned by NEC
		case 0x64:
		case 0x65:
		case 0x66:						return _DANGER;
	
		// key mark
		
		case (100 + _MY_POINT_ - 0xA0):	// for _NEC
		case (_MY_POINT_):				return _SATANA;
	}
	return _ABNORMAL;
}


//=[_ICQ_display_progress]====================================================
//
//		CALLBACK function ensuring displaying progress on screen,
//	in the current implementation displays a dot with each iteration
//
//	ARG:
//		from		-	LBA address of the first scanned sector of the range
//		cur			-	LBA address of the sector currently scanned
//		to			-	LBA address of the last scanned sector of the range
//
//	RET:
//		none
/*
_ICQ_display_progress(int from, int cur, int to)
{
	
	printf("."); return from+cur+to;
}
*/


#define __POINT		BUF[_offPOINT]
#define __TRACK_NUM	BUF[_offTRACK_NUM]
//=[ICQ(char *CD, int LBA, int LBAmax)]=======================================
//
//		Searching for key mark on the disc carried out by reading TOC at the
// subchannel level - the lowest level supported by drives (and this support
// is undocumented)
//
//	ARG:
//		CD			-	drive name/address
//		LBA			-	address from which to start the search (see GetKeyMarkLBA)
//		LBAmax		-	address to which to continue the search
//
//	RET:
//		_HELLO_LEGAL_USER
//					-	key mark found
//		_HELLO_HACKER
//					-	key mark not found
//
//	NOTE:
//		1) it is necessary to scan at least 50 sectors
//		2) positioning/reading errors of subchannel data are ignored
ICQ(char *CD, int LBA, int LBAmax)
{
	int a, x;
	int no_seek_cnt = 0;					// seek error counter
	int no_qsub_cnt = 0;					// read error counter
	int TIME_TO_WAIT = _MY_TIME_TO_WAIT_;	// seek delay

	unsigned char BUF[MAX_TOC_SIZE];
	unsigned char OLD[MAX_TOC_SIZE];
	unsigned char CDB[ATAPI_CDB_SIZE];
	//unsigned char LIST[MAX_TOC_SIZE];

	_DBG_ fprintf(debug, "# ICQ(%s,%d,%d) was called\n", CD, LBA, LBAmax, _FF_);

	// TESTING ALL LBA ADDRESSES ONE BY ONE
	//------------------------------------------------------------------------
	for (x = LBA; x <= LBAmax; x++)
	{
		// displaying progress
		_ICQ_display_progress(LBA, x, LBAmax);

		// seeking delay
		// If waiting time is insufficient, then the drive simply won't 
		// position the head at all, and return the data from the current
		// position. This is the program's bottleneck, because the time
		// provided for the drive for positioning the head is large enough
		// (about 1 second). To speed up the program, it is necessary to 
		// use adaptive algorithms
		Sleep(TIME_TO_WAIT);

		// buffer initialization
		memset(CDB, 0, ATAPI_CDB_SIZE); memset(BUF, 0x00, MAX_BUF_SIZE);

		// SEEK
		//=-------------------------------------------------------------------
		CDB[0] = 0x2B;	// SEEK CD
		CDB[2] = HIBYTE(HIWORD(x));
		CDB[3] = LOBYTE(HIWORD(x));
		CDB[4] = HIBYTE(LOWORD(x));
		CDB[5] = LOBYTE(LOWORD(x));

		// WAITING FOR THE DRIVE TO BECOME READY
		// (SCSI_NEED_SKIP is skipped, because it was already processed in the 
		//	GetKeyMarkLBA function)
		_DBG_ fprintf(debug, "# SCSI_BEGIN() was called\n", _FF_);
		a = SCSI_BEGIN(CD); 	_DBG_ fprintf(debug, "# SCSI_BEGIN() was returned %d\n",a, _FF_);
		if (a != SCSI_OK) {	_DBG_ fprintf(debug, "# DEVICE NOT READY\n", _FF_);}

		// PASS THE SEEK COMMAND FOR EXECUTION
		_DBG_ fprintf(debug, "# SEND_SCSI_CMD(/* SEEK to sec %Xh LBA */) was called\n", x, _FF_);
		a = SEND_SCSI_CMD(CD, (char *)CDB, ATAPI_CDB_SIZE, NO_SENSE, BUF, MAX_BUF_SIZE, SCSI_DATA_IN);
		if (a != SCSI_OK) _DBG_ fprintf(debug,"# SEEK ERROR\n", _FF_);

		// reinitialization
		memset(CDB, 0, ATAPI_CDB_SIZE); memset(BUF, 0x00, MAX_BUF_SIZE);

		// READ SUBCANNEL
		//====================================================================
		CDB[0] = 0x42;	// READ SUBCODE CD
		CDB[2] = 1<<6;
		CDB[3] = 1;
		CDB[7] = HIBYTE(LOWORD(MAX_BUF_SIZE));
		CDB[8] = LOBYTE(LOWORD(MAX_BUF_SIZE));

		// WAITING FOR THE DRIVE TO BECOME READY
		_DBG_ fprintf(debug, "# SCSI_BEGIN() was called\n", _FF_);
		a = SCSI_BEGIN(CD); _DBG_ fprintf(debug, "# SCSI_BEGIN() was returned %d\n",a, _FF_);
		if (a != SCSI_OK) {	_DBG_ fprintf(debug, "# DEVICE NOT READY\n", _FF_);}

		// PASSING THE COMMAND TO READ SUBCHANNEL DATA FOR EXECUTION
		_DBG_ fprintf(debug, "# SEND_SCSI_CMD(/* READ SUBCHANNEL */) was called\n", _FF_);
		a = SEND_SCSI_CMD(CD, (char *)CDB, ATAPI_CDB_SIZE, NO_SENSE, BUF, MAX_BUF_SIZE, SCSI_DATA_IN);
		// a = 0; // XXX_IML initates subchannel data read error
		if (a != SCSI_OK)
		{	// subchannel data read error

			// debug print
			_DBG_ fprintf(debug,"# SUBCODE READ ERROR\n", _FF_);

			// trying to read sector _MY_MAX_NO_SEEK_ times,
			// any time decreasing the counter
			// WARNING: direct counter modification is here!
			if (++no_qsub_cnt < _MY_MAX_NO_SEEK_) x--; else no_qsub_cnt = 0; continue;
		} no_qsub_cnt = 0 ;

		// SUBCHANNEL DATA READ SUCCESSFULLY
		//---------------------------------------------------------------

		// debug print
		//----------------------------------------------------------------
		_DBG_
		{
			fprintf(debug,"#\t\t", _FF_);
			for(a = 0;  a < QSUBCHANNEL_LEN; a++) fprintf(debug, "%02X ", BUF[a], _FF_);
			fprintf(debug, "(%02Xh @ %02Xh)\n", __TRACK_NUM, __POINT, _FF_);
		}

		// SEEK SUCCESS CHECK
		//----------------------------------------------------------------
		// (if the head was positioned, new subchannel data must be read)
		if (memcmp(OLD, BUF, QSUBCHANNEL_LEN))
		{							// success
			no_seek_cnt = 0;
		}
			else
		{							// drive didn't position the head
			_DBG_ fprintf(debug, "# NO ACTUAL SEEK WASED ", _FF_);
			if (++no_seek_cnt > _MY_MAX_NO_SEEK_)
			{						// seek error counter reached maximum
									// increasing waiting time
				TIME_TO_WAIT+=_MY_TIME_TO_INC_; no_seek_cnt = 0;
				_DBG_ fprintf(debug, "inc %d on %d", TIME_TO_WAIT, _MY_TIME_TO_INC_, _FF_);
			} _DBG_ fprintf(debug, "\n", _FF_);
		}
		memcpy(OLD, BUF, QSUBCHANNEL_LEN);

		// seaching our key mark in subchannel data
		_DBG_ fprintf(debug, "# CALL _chk_point(%02Xh)\n", __POINT, _FF_);
		if (_ICQ_chk_point(__POINT) == _SATANA) return _HELLO_LEGAL_USER;
	}

	return _HELLO_HACKER;
}

#endif //__CRACKME_N2_KERNEL_H__