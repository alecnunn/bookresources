#ifndef __CRACKME_N2_CON_H__
#define __CRACKME_N2_CON_H__
#define sTITLE			"crackme.N2.demo by Kris Kaspersky (kk@sendmail.ru) для Рауфа Алиева\n-------------------------------------------------------------------\n\n"
#define sARG			"parsing the command line..."
#define sSCAN			"scanning ASPI bus..."
#define sSELECT			"drive found............"
#define sWAIT			"reading TOC............."

#define sNOT_READY		"drive not ready"
#define sNOT_FOUND		"-ERR: drive with key disc not found"
#define sNOT_READY_ERR	"-ERR: no disc in drive, drive not ready or doesn't exist\x7\n"

#define sREADY_OK		"+OK             \n"
#define sICQ			"searching key label"
#define sICQ_OK			".....+OK                                                                      \n"
#define sICQ_NOT		".....not found!                                                                  \n"


#define sNOT_SUPPORT	"-ERR: TOC read error: key disc damaged or not supported by the drive\x7\n" 
#define sERROR			"-ERR: surface defect,  drive damaged or incompatible with protection\n"

#define sHACKER			"\n\n"\
						"-ERR: this is unauthorized disc copy or drive is incompatible with protection\n"\
						"run the program with the -debug option and send the debug.log file\n"\
						"to the developer by the following address kk@sendmai.ru for analysis\x7\n"

#define sLEGAL			"\n\n"\
						"**[#]*************************************************************************\n"\
						"*                                                                            *\n"\
						"*               = Welcome legal userь =                                      *\n"\
						"*----------------------------------------------------------------------------*\n"\
						"*                                                                            *\n"\
                        "*     All checks passed, key disc recognized, key mark found and recognized. *\n"\
                        "* Now the main branch of the program gets control and displays               *\n"\
                        "* this demo text, which could be replaced by your protected program.         *\n"\
                        "* Try to copy this disc using any protected CD copier                        *\n"\
                        "* (Alcohol 120%%/Clone CD),  including disc emulation on a virtual drive.    *\n"\
                        "* If you are satisfied with the strength of this protection, contact         *\n"\
                        "* the author of this program for integrating this mechanism into your        *\n"\
                        "* software:   kk@sendmail.ru                                                 *\n"\
                        "*----------------------------------------------------------------------------*\n"\
                        "*                                                                            *\n"\
                        "*                        SAY NO TO BANNERS !                                 *\n"\
                        "*                                                                            *\n"\
                        "******************************************************************************\n"

#define sENTER			"\n\n\t     * to exit the program press  <ENTER> *\n"

#define sINPUT			"management console is activated\n$:"
#define sERR_ARG		"-ERR: unknown argument %s in command line\x7\n"

#define sUSAGE			"\rUSAGE:                       \n"\
						"      -cd CD                   : explicitly specify the drive and interface\n"\
						"                                 (ASPI/SPTI), see documentation for details\n"\
						"                                 also allowed is the combination ?.?\n\n"\
						"      -debug [debug_file_name] : enable debug printing\n"\
						"                                 (debug.log by default), \n"\
						"                                 stdout and stderr are also allowed\n\n"\
						"      -rlba nn                 : relative offset of the key mark on\n"\
						"                                 disc (for example,  -rlba 10 or -rlba 69)\n"\
						"                                 default value is -2\n\n"\
						"      -2lba nn                 : number of sectors to scan for key mark\n"\
						"                                 (55 - by default)\n\n"\
						"      -#3                      : management console mode\n"
				                                       
#endif //__CRACKME_N2_CON_H__