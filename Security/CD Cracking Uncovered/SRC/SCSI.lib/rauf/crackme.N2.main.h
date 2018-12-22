/*----------------------------------------------------------------------------
 *
 *							protect_main
 *							============
 *
 *		Main function of the protection carrying out the search of the drive
 *	with the key disc loaded and checking it for the presence of the key mark.
 *	All this is accompanied by displaying the debug info (if needed),
 *	"fine-tuning" to the required software/hardware environment and displaying
 *	interface (see crackme.N2.user.h)
 *
 *	ARG:
 *		argCD		-	name/address/interface  of the drive with the key disc
 *						written in the SCSI.lib syntax (i.e., 1.1 or \\.\X:)
 *						If argCD == 0 or *argCD == 0, the protection attempts 
 *						to find the key disc on its own
 *						(this is its main operating mode). The forced mode is
 *						intended for debugging purposes.
 *
 *		arg_debug	-	Log file for debug print,
 *						if arg_debug == 0 or *arg_debug == 0, the default naeme
 *						 _MY_DEBUG_NANE_ is used. Also allowed names are stdout and stderr
 *
 *		rlba		-	offset of the key mark on the disc in relation to the value 
 *                      returned by the GetKeyMarkLBA function
 *
 *		to_lba		-	how many sectors to scan when searching for the key mark
----------------------------------------------------------------------------*/
#ifndef __CRACKME_N2_MAIN_H__
#define __CRACKME_N2_MAIN_H__

#include "crackme.N2.cfg.h"
#include "crackme.N2.user.h"
#include "crackme.N2.kernel.h"


#define keySTDOUT		"stdout"
#define keySTDERR		"stderr"

int protect_main(char *argCD, char *arg_debug, int rlba, int to_lba)
{
	int a, x;
	unsigned char buf[MAX_BUF_SIZE];
	unsigned char CD[MAX_BUF_SIZE];
	unsigned char LIST[MAX_BUF_SIZE];
	char *debug_file_name;

	_MY_POINT_IN_SEC_	=	rlba;	
	_MY_KEY_BOUND_		=	to_lba;

	// opening log file
	//------------------------------------------------------------------------
	if ((arg_debug) && (*arg_debug)) debug_file_name = arg_debug;
									else debug_file_name = _MY_DEBUG_NANE_;

	if (!strcmp(debug_file_name, keySTDOUT)) debug = stdout;
		else if (!strcmp(debug_file_name, keySTDERR)) debug = stderr;
			else if (_DEBUG_PRINT_) debug = fopen(debug_file_name, "w");
	
	if (debug == 0) debug = stderr;

	if ((argCD) && (*argCD))
	{
		strcpy((char *)CD, argCD);
	}
		else
	{
		_pm_scan(); ASPI_SEND_SCSI_INQUITY_EX((char* )LIST, 0, 0, 0x5);
		a = isMyDEV((char*) CD, (char *)LIST);
		if (a == _NOT_READY) {
			_pm_print_not_found(); return _NOT_READY;
		} _pm_print_OK();
	}

	_pm_print_select((char *)CD);
	// fgets(buf,100,stdin); //XXX_FACK
	
	// DETERMINING ADDRESS OF THE LEAD-IN WITH KEY MARK INSIDE
	//========================================================================
	for (x = _pm_print_wait(); x < _MY_CD_TRY_; x++)
	{
		 _ICQ_display_progress(0, x, _MY_CD_TRY_);
		a = GetKeyMarkLBA((char *)CD); if (a != _NOT_READY) break; 
		//printf(((x == _MY_CD_TRY_)?
		//				sNOT_READY_ERR : "%s [%d/%d]\r"), sNOT_READY, x, _MY_CD_TRY_);
	}

	switch(a)
	{
		case _NEED_SKIP:	return a;
		case _NOT_READY:	_pm_print_not_ready(); return a;
		case _NOT_SUPPORT:	_pm_print_not_support();return a; 
		default :			_pm_print_TOC_OK();
	}

	// SEARCHING FOR THE KEY MARK IN THE SPECIFIED AREA
	//========================================================================
	_pm_print_ICQ(); a = ICQ((char *)CD, a, a + _MY_KEY_BOUND_);
	if (a == _HELLO_HACKER)
		_pm_print_hello_hacker();
	else
	    _pm_print_hello_legal_user();

	return a;
}

#endif //__CRACKME_N2_MAIN_H__