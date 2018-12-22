/*----------------------------------------------------------------------------
 *     ::::::::::::::::::::::::::::::::::::::::::::::::::::
 *       ####   #####           #   ####   #     #  #####      CD anti
 *      #    #   #   #         ##  #    #  ##   ##  #     
 *      #        #   #        # #  #       # # # #  #          -copying
 *      #       #####        #  #  #       #  #  #  ###   
 *      #        #  #       #####  #       #     #  #         mechanism  
 *      #    #   #   #     #    #  #    #  #     #  #     
 *       ####    #    #   #     #   ####   #     #  #####        2.0 beta
 *     ::::::::::::::::::::::::::::::::::::::::::::::::::::
----------------------------------------------------------------------------*/
//
// HOW TO CREATE A PROTECTED DISC
// ------------------------------
//		Use CloneCD, burn the supplied image to disc and make sure that
//	protection recognizes the disc as key disc. Then try to copy it using
//	any copier (end-user of protected CD copier).
//	I encourage you to crack this protection!
//		For more details see crackme.N2.kernel.h. The developer of this
//	protection can be contacted by kk@sendmail.ru
//
//		Attention: when using tab space other than four this listing is unreadable...
//
//	Kris Kaspersky
//	-------------- 
#define __CONSOLE__						// console version
										// for protect_main, or, to be more precise
										// for functions of the crackme.N2.user.h file
										// where user interface is located.
#include <stdio.h>
#include "crackme.N2.main.h"			// connect the protection module
										// to the project

// COMMAND LINE OPTIONS
//----------------------------------------------------------------------------
#define keyDEBUG		"-debug"		// debug info [file_name]
#define keyCD			"-cd"			// specify the drive with key disc
#define keyIN_SEC		"-rlba"			// relative mark offset
#define keyBOUND		"-2lba"			// max. number of scanned sectors
#define keyINPUT		"-#3"			// service console


// Macros simplifying command-line parsing
//----------------------------------------------------------------------------
#define argYetOne(a)	((argc > (++a)) ? 1 : 0)
#define is_arg(a, s)	((strcmp(argv[(a)], (s))) ? 0 : argYetOne(a) ? 1 : 0 )

// Demonstration of the protect_main call
//----------------------------------------------------------------------------
main(int argc, char** argv)
{
	int		a;
	char	*CD, *buf;
	char	*debug_file_name = _MY_DEBUG_NANE_;

	// Initialization
	//------------------------------------------------------------------------
	buf = malloc(MAX_BUF_SIZE); CD  = malloc(MAX_BUF_SIZE); *CD = 0;

	// TITLE
	//------------------------------------------------------------------------
	printf(sTITLE);

	// Parsing the command-line options
	//------------------------------------------------------------------------
	if ((argc>1)) { printf(sARG);
	for (a = 1; a < argc; a++)
	{
		if (!strcmp("-?",argv[a]) || !strcmp("/?",argv[a]) ||	// -HELP
			!strcmp("-h",argv[a]) || !strcmp("/h",argv[a]))
		{
			printf(sUSAGE);	return 0;
		} else

		if (!strcmp(argv[a], keyDEBUG))							// -debug
		{
			_DEBUG_PRINT_ = ~0;
			if (argYetOne(a)) debug_file_name = argv[a];		// gbg_FN
		} else

		if (is_arg(a, keyCD))									// -cd
				CD = argv[a]; else

		if (is_arg(a, keyIN_SEC))								// -rlba
			_MY_POINT_IN_SEC_ = atol(argv[a]); else

		if (is_arg(a, keyBOUND))								// -2lba
			_MY_KEY_BOUND_ = atol(argv[a]); else

		if (!strcmp(argv[a], keyINPUT))							// -#3
		{
			printf(sINPUT); fgets(buf, MAX_BUF_SIZE, stdin);

			// Splitting the string into array of substrings
			argv = (char**) malloc(MAX_BUF_SIZE); argc = 0;

			while(*buf)
			{
				argv[++argc] = (char *) malloc(MAX_BUF_SIZE); a = 0;
				while(*buf > 0x20) argv[argc][a++] = *buf++; argv[argc][a] = 0;
				while(*buf && *buf <= 0x20) buf++;
			} argc++; a = 0; continue;		// XXX_HACK modifying the loop counter
											// must be a = 1, but we move it backward
		}
			else
		{
			printf(sERR_ARG, argv[a]);
			return -1;
		}
	} printf(sREADY_OK); }

	// CALLING THE MAIN PROTECTION FUNCTION
	//---------------------------------------------------------------------
	// In this demo version the function simply returns the code of genuine
	// key disc (the disc is either original or not, otherwise some error has
	// occurred). This can be easily cracked by bit hack (even if special
	// anti-debugging measures have been taken). Therefore, in commercial
	// version this function returns x-bit key that can be used for decrypting
	// critical sections of the program
	a = protect_main(CD, debug_file_name, _MY_POINT_IN_SEC_, _MY_KEY_BOUND_);

	// Processing the results of checking the key disc
	//---------------------------------------------------------------------
	switch(a)
	{
		// code  _ERROR not supported
		// case _ERROR:			printf(sERROR);  break;
		case _HELLO_HACKER:		printf(sHACKER); break;
		case _HELLO_LEGAL_USER:	printf(sLEGAL);  break;
	}

	// waiting for the user to press <ENTER> and exit the program
	// (specially for those that do not know how to start console applications)
	printf(sENTER); fgets(buf, MAX_BUF_SIZE, stdin);

	return 0;
}



