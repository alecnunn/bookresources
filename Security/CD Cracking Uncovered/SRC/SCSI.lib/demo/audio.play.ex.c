/*----------------------------------------------------------------------------
 *
 *							SIMPLE AUDIO PLAYER
 *							===================
 *
 * build 0x001 @ 27.05.2003
----------------------------------------------------------------------------*/
// Add processing for the stop situation
#include <stdio.h>
#include <windows.h>

#include "..\CD.h"
#include "..\SCSI.h"


#define BUF_SIZE		(SENSE_SIZE)

//=============================[ K E R N E L ]================================
// 
//		CD-PLAYER KERNEL
//		----------------
//
// CD-PLAYER kernel contains functions that directly control the drive, including
//  _CD_READ_TOC, _GET_CURRENT and _PLAY_CD_MSF
_CD_READ_TOC(char *cd, unsigned char *_TOC)
{
	int a;

	unsigned char CDB[ATAPI_CDB_SIZE];

	memset(CDB,0,ATAPI_CDB_SIZE);
	CDB[0] = READ_TOC;
	CDB[2] = W_TOC;
	CDB[7] = HIBYTE(MAX_TOC_SIZE);
	CDB[8] = LOBYTE(MAX_TOC_SIZE);
	
	SCSI_BEGIN(cd);
	a = SCSI_SEND_CMD(cd, CDB, ATAPI_CDB_SIZE, NO_SENSE, _TOC, MAX_TOC_SIZE, SCSI_DATA_IN);
	return a;
}


GET_CURRENT(char *cd, ULBA *lba, ULBA *track_lba, int *track)
{
	int a;

	unsigned char buf[MAX_TOC_SIZE];
	unsigned char CDB[ATAPI_CDB_SIZE];

	memset(CDB, 0, ATAPI_CDB_SIZE);

	CDB[0] = 0x42; // READ SUB
	CDB[2] = 1<<6;	// SubQ
	CDB[3] = 1;		// Format Sub
	CDB[7] = HIBYTE(MAX_TOC_SIZE);
	CDB[8] = LOBYTE(MAX_TOC_SIZE);

	if (SCSI_BEGIN(cd) != SCSI_OK) return ~SCSI_OK;	
	a = SCSI_SEND_CMD(cd, CDB, ATAPI_CDB_SIZE, NO_SENSE, buf, MAX_TOC_SIZE, SCSI_DATA_IN);

	if (a != SCSI_OK) {
		fprintf(stderr,"-ERR: READ SUB\x7\n"); return ~SCSI_OK; }

	*lba = buf[9] * 0x10000 + buf[10]*0x100 + buf[11];
	*track_lba  = buf[13] * 0x10000 + buf[14]*0x100 + buf[15];
	*track = buf[6];

	return SCSI_OK;
}


#define _DISPLAY_TOC		1
#define _GET_TRACK_LBA		2
#define _TRACK_RESOLVE		3

FACK_TOC(char *cd, int TRACK_LBA, int DO)
{
	int a;
	int M, S, F;
	int	len;
	int lba, lba_t;
	int track, old_track, track_c;

	unsigned char _TOC[MAX_TOC_SIZE];

	a = GET_CURRENT(cd, &lba, &lba_t, &track_c);


	if (a != SCSI_OK) track_c = 0;

	a =_CD_READ_TOC(cd, _TOC);
	if (a != SCSI_OK) {
		fprintf(stderr,"-ERR:CD-ROM NOT READY!\n");	return -1; }

	len = _TOC[0] * 0x100L + _TOC[1] + 2; //track = _TOC[2]; 

	for (a = 4; a < len; a += sizeof(struct CD_TOC))
	{
		// трек 
		RAW_LBAtoMSF(&M, &S, &F, &_TOC[a+4]);  track = _TOC[a + 2]; 
		lba = _TOC[a+5] * 0x10000 + _TOC[a+6]*0x100 + _TOC[a+7];

		switch(DO)
		{
			case _DISPLAY_TOC:

					printf("%c %02d --> %02d:%02d:%02d ", (track == track_c)?'*':' ',
					track, M, S, F);
					printf("(%s)\n",(_TOC[a+2]==0xAA)?
						"Lead-Out":(_TOC[a+1] & 16)?"Audio":"Non-Audio");
					break;

			case _GET_TRACK_LBA:	if (track >= TRACK_LBA) return lba;
									break;


			case _TRACK_RESOLVE:	if (lba > TRACK_LBA)
										return (track == 0xAA)?0xAA:--track;
									break;
									
			default: return -1;
		}
		// track++; no, track numbering may be irregular
	}

	// Returning data
	switch(DO)
	{
		case _DISPLAY_TOC:		return 1;
		case _GET_TRACK_LBA:	return lba;
		case _TRACK_RESOLVE:	return -1;
	}

	return -1;
}

PLAY_CD(char *cd, int Ms, int Ss, int Fs, int Me, int Se, int Fe, int flag)
{

	
	unsigned char CDB[ATAPI_CDB_SIZE];

	memset(CDB, 0, ATAPI_CDB_SIZE);

	CDB[0] = 0x47;

	CDB[3] = Ms;
	CDB[4] = Ss;
	CDB[5] = Fs;

	CDB[6] = Me;
	CDB[7] = Se;
	CDB[8] = Fe;

	if (SCSI_BEGIN(cd)!=SCSI_OK) return ~SCSI_OK;	
	return SCSI_SEND_CMD(cd, CDB, ATAPI_CDB_SIZE, NO_SENSE, NO_BUF, SCSI_DATA_IN);
}

PLAY_CD_LBA(char *CD, int start_LBA, int end_LBA)
{
	int Ms, Ss, Fs;
	int Me, Se, Fe;
	
	LBAtoMSF(&Ms, &Ss, &Fs, start_LBA);
	LBAtoMSF(&Me, &Se, &Fe, end_LBA);
	return PLAY_CD(CD, Ms, Ss, Fs, Me, Se, Fe, 0);
}


// Returns the position until which to play
PLAY_CD_TRACK(char *CD, int start_TRACK, int end_TRACK )
{
	int a;
	int start_LBA;
	int end_LBA;

	start_LBA = FACK_TOC(CD, start_TRACK, _GET_TRACK_LBA);
	end_LBA = FACK_TOC(CD, end_TRACK, _GET_TRACK_LBA) - 50;
	if ((start_LBA == -1) || (end_LBA == -1)) return ~SCSI_OK;
	a = PLAY_CD_LBA(CD, start_LBA, end_LBA);
	return (a == SCSI_OK)?--end_LBA:a;
}


#define _BEGIN_LIST_INT_	0xA0
#define _BEGIN_LIST_CHR_	'b'
PLAY_CD_LIST(char *CD, int *TRACK_LIST, int (*_user_cur_pos)(int cur_lba, int cur_track_lba, int cur_track))
{
	int a;
	int cur_wait;
	int f_c;
	ULBA cur_lba, cur_track_lba;
	ULBA to_lba, to_track_lba;
	int cur_track, to_track;
	int *begin_list = TRACK_LIST;
	int m,s,f;


	while (TRACK_LIST && *TRACK_LIST)
	{
		if (*TRACK_LIST == _BEGIN_LIST_INT_) TRACK_LIST = begin_list;
		else
		{
			a = PLAY_CD_TRACK(CD, *TRACK_LIST, (*TRACK_LIST)+1); cur_wait = 0;

	

			while(1)
			{
	
				if (GET_CURRENT(CD, &cur_lba, &cur_track_lba, &cur_track)==SCSI_OK)
				{				
					_user_cur_pos(cur_lba, cur_track_lba, cur_track);
					LBAtoMSF(&m, &s, &f, a);
					printf("a = %d:%d:%d\n",m,s,f);
					LBAtoMSF(&m, &s, &f, cur_lba);
					printf("cur_lba = %d:%d:%d\n",m,s,f);

					// if (cur_track > *TRACK_LIST) break;
					//if (cur_track_lba > 1000) break;	
					if ( (cur_wait > 300) && ((cur_lba + 150) >= a)) break;
				}	
					Sleep(30); cur_wait++;
				
			}
			TRACK_LIST++;
		}	
		
	}
}

// From command line to array

#define _CFG_DOT_	','
cmd2list(int *dst, unsigned char *src, int n)
{
	int a = 0;
	char *p = src;
	if (!(src) || !(dst)) return 0;
	while(*p)
	{
		switch(*p)
		{
			case _BEGIN_LIST_CHR_: dst[a] = _BEGIN_LIST_INT_; break;
			default:			dst[a] = strtol(p, &p, 10);
		}

		if (++a == n) return 0;
		if (*p== _CFG_DOT_) p++; else  return 0;
	}
	dst[a] = 0;
	return a;
}

// DO
// 1 - resume
PAUSE_RESUME(char *cd, int RESUME)
{
	unsigned char CDB[ATAPI_CDB_SIZE];
	CDB[0] = 0x4B;	// PAUSE/RESUME AUDIO
	CDB[8] = RESUME;
}


_user_cur_pos(cur_lba, cur_track_lba, cur_track)
{
	int M,S,F;
	LBAtoMSF(&M, &S, &F, cur_lba);			
	printf("DISK - %02d:%02d:%02d\t\t",M, S, F);
	LBAtoMSF(&M, &S, &F, cur_track_lba);			
	printf("TRACK (%02d) - %02d:%02d:%02d\r",cur_track, M, S, F);
}



main(int argc, char **argv)
{
	int a,b; FILE *f;

    unsigned char SENSE[SENSE_SIZE];		
	
	int	lba_begin, lba_end, lba_c, lba_tc;		// start play and end play
	int Ms,Ss,Fs, Me,Se,Fe, Mc, Sc, Fc;			// M:S:F for start play and end play	
	int Ts, Te, Tc;								// TrackStart/TrackEnd
	int demo_track[]={13,2, 12,13, 8,11,9,10,5};
	int track_list[1000];

	#define	no_actual_arg	1
	#define arg_TOC			2
	#define arg_min_track	3
	#define arg_max_track   4
	#define arg_msf			8

	#define arg_is_cur		((argc < 3)?0:((argv[2][0]=='?') || (argv[2][0]=='*'))?1:0)
	#define arg_is_cur_c	((argc < 3)?0:(argv[2][0]=='*')?1:0)

	#define CD		argv[1]
	#define RESUME	atol(argv[2])
	
	#define startM  atol(argv[2])
	#define startS	atol(argv[3])
	#define startF	atol(argv[4])
	#define endM	atol(argv[5])
	#define endS	atol(argv[6])
	#define endF	atol(argv[7])

	#define argTs	atol(argv[2])
	#define argTL	(argv[2])
	#define argTe	(argc<arg_max_track)?-1:atol(argv[3])
	//printf("argc = %d\n",argc);

	//PLAY_CD_TRACK("TEAC", 8, 10);
	if (argc == arg_min_track)
	{
		if (strchr(argTL, _CFG_DOT_))
		{
			cmd2list(track_list, argTL, 100);
			//for (a = 0; a< 10; a++) printf("%x %x\n",a ,track_list[a]);
			PLAY_CD_LIST(CD, track_list, _user_cur_pos);
			return 0;
		}

	}

	if (argc == no_actual_arg) {
		fprintf(stderr,	"USAGE: audio.play.exe CD\n"); return 0; }

	if (argc == arg_TOC) { FACK_TOC(CD,0, _DISPLAY_TOC); return 1; }

	if (arg_is_cur)
	{
		do{
			a = GET_CURRENT(CD, &lba_c, &lba_tc, &Tc);
			if (a = SCSI_OK)
			{
				LBAtoMSF(&Mc, &Sc, &Fc, lba_c);			
				printf("DISK - %02d:%02d:%02d\t\t",Mc, Sc, Fc);
				LBAtoMSF(&Mc, &Sc, &Fc, lba_tc);			
				printf("TRACK (%02d) - %02d:%02d:%02d\r",Tc, Mc, Sc, Fc);
				Sleep(30);
			}
		}while(arg_is_cur_c);
		return 0;
	}

	if ((argc >= arg_min_track) && (argc <= arg_max_track))
	{															// PLAY TRACKS
		
		lba_begin = FACK_TOC(CD, argTs, _GET_TRACK_LBA);
		lba_end  = FACK_TOC(CD, argTe, _GET_TRACK_LBA);
		LBAtoMSF(&Ms, &Ss, &Fs, lba_begin);
    	LBAtoMSF(&Me, &Se, &Fe, lba_end);
    	printf("PLAY FROM: %02d:%02d:%02d\n",Ms,Ss,Fs);
    	printf("PLAY TO  : %02d:%02d:%02d\n",Me,Se,Fe);
		PLAY_CD(CD, Ms, Ss, Fs, Me, Se, Fe,0 );		
		return 1;
	}

	if (argc == arg_msf)
	{
		Ms = startM; Ss = startS; Fs = startF;
		Me = endM;   Se = endS;   Fe = endF;

		//  in local
    	printf("PLAY FROM: %02d:%02d:%02d\n",Ms,Ss,Fs);
    	printf("PLAY TO  : %02d:%02d:%02d\n",Me,Se,Fe);
		PLAY_CD(CD, Ms, Ss, Fs, Me, Se, Fe,0 );		
		return 1;		
	}

	return 0;
}