// SETTINGS
//----------------------------------------------------------------------------
// You can change these settings as needed
//

#define _MY_POINT_			0xAB		// nonstandard  point -  key mark
#define _MY_TRACK_			2			// 1st track of the session with key mark
#define _MY_LBA_			0x2EE0		// LBA of _MY_TRACK_
int _MY_POINT_IN_SEC_	=	-2;			// for positioning on key mark
int _MY_KEY_BOUND_		=	55;			// how many sectors to scan

#define _MY_CD_TRY_			7			// number of attempts
#define _MY_TIME_TO_WAIT_	1000		// delay between positioning commands
#define _MY_TIME_TO_INC_	100			// increment of positioning delay.

#define _MY_MAX_NO_SEEK_		3		// max number of SEEK commands
										// after which MY_TIME_TO_WAIT
										// is increased by MY_TIME_TO_INC

#define _MY_DEBUG_NANE_		"debug.log"	// log file


// FUNDAMENTAL CONSTANTS (do not change!)
//----------------------------------------------------------------------------
#define GAP_SIZE			150			// pre-gap and post-gap length
#define LEAD_SIZE			4500		// Lead-In size
#define TOC_POINTER_SIZE	11			// Size of one entry ENTRY
#define QSUBCHANNEL_LEN		16			// size of subchannel data read

#define _offTRACK_NUM		0x6			// offset of TRCK NUM	in Q-sub
#define _offPOINT			0x7			// offset of PNT		in  Q-sub

#define _off_TOC_ENTRY		0x4			// offset of 1st ENTRY	in TOC
#define _off_TOC_TRACK_NUM	0x3			// offset of TRCK NUM	in TOC ENTRY
#define _off_TOC_MIN		0x8			// offset of MIN		in TOC ENTRY
#define _off_TOC_SEC		0x9			// offset of SEC		in TOC ENTRY
#define _off_TOC_FRA		0xA			// offset of FRAME		in TOC ENTRY

#define MAX_BUF_SIZE		0x1000		// buffer size for all cases


// RETURN CODES (can be changed, but do you actually need it?)
//----------------------------------------------------------------------------
#define _ERROR				-1			// error
#define _NEED_SKIP			-2			// mother function must die
#define _NOT_SUPPORT		-3			// required function not supported
#define _NOT_READY			0x0			// drive not ready

#define _HELLO_HACKER		0x999		// key disc not recognized
#define _HELLO_LEGAL_USER	0x666		// key disc recognized


// DEBUG PRING FUNCTIONS
//----------------------------------------------------------------------------
#define _DBG_	if (_DEBUG_PRINT_)		// print (if needed)
#define _FF_	fflush(debug)			// flushing buffers


// GLOBAL VARIABLES
//----------------------------------------------------------------------------
FILE *debug=0;							// debug print handle
int _DEBUG_PRINT_ = 0;					// != 0 - debug print enabled
