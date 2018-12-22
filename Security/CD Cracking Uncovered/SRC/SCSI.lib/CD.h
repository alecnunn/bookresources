/*----------------------------------------------------------------------------
 *
 *					Everything for working with CD-ROM/R/RW
 *					=======================================
 *
----------------------------------------------------------------------------*/
#ifndef __KPNC_CD_H__
#define __KPNC_CD_H__

#ifndef	BYTE
	#define BYTE char
#endif

#define ATAPI_CDB_SIZE					12

#define RAW_SECTOR_SIZE					2352
#define SECTOR_SIZE						2048



//=[FUNDAMENTALS]==============================================================
#define ONE_SECTOR		0x1


//=[SCSI/ATAPI CMD]===========================================================
//
//			SCSI/ATAPI commands for controlling CD-ROM/RW/R drives
//			-------------------------------------------------------
//
#define READ_CAPACITY					0x25
#define READ_HEADER						0x44
#define MODE_SENSE						0x5A
	#define CAPABILITIES						0x2A

#define READ_CD							0xBE
#define MECHANISM_STAT					0xBD


	#define F_USER_DATA						0x10
	#define W_USER_DATA						0x10

#define READ_TOC						0x43
	#define MAX_TOC_SIZE					0x1000
	#define W_TOC							0x0			// HI-LEW TOC
	#define W_SESSION						0x1			// information about sessions
	#define W_FULL_TOC						0x2			// RAW TOC



// LBA, ULBA
//---------------------------------------------------------------------------
// relative LBA
#ifndef RLBA					// defining relative (signed) LBA
	#define RLBA signed int
#endif

// unsigned LBA
#ifndef ULBA					// defining unsigned LBA
	#define ULBA unsigned int
#endif

//=[STRUCTURES]================================================================
#pragma pack(push, enter_CD)
#pragma pack(1)

//=[MSF]======================================================================
//
//	MSF		-	structures that stores minutes, seconds and frames,
//				(mainly used for absolute addressing)
#ifndef  MSF
	struct MSF
	{
			BYTE M;		// MIN
			BYTE S;		// SEC
			BYTE F;		// FRAME
	};
#endif

struct CD_TOC
{
	BYTE reserverd_1;
	BYTE ADR_control;
	BYTE track_number;
	BYTE reserverd;
	BYTE ADDR[4];
};
#pragma pack(pop, enter_CD)

RLBA MSFtoLBA(int M, int S, int F)
{
	return (M * 60 + S) * 75 + F - 150;

}

struct MSF LBAtoMSF(int *M, int *S, int *F, RLBA lba)
{
	struct MSF msf;

	*M = (lba  + 150L) / (60L*75);
	*S = ((lba + 150)  / 75L) % 60;
	*F = (lba  + 150)  % 75;

	msf.M = *M; msf.S = *S; msf.F = *F;

	return msf;
} 


struct MSF LBA0toMSF(int *M, int *S, int *F, RLBA lba)
{
	struct MSF msf;

	*M = lba / (60L*75);
	*S = (lba  / 75L) % 60;
	*F = lba  % 75;

	msf.M = *M; msf.S = *S; msf.F = *F;

	return msf;
} 


struct MSF RAW_LBAtoMSF(int *M, int *S, int *F, unsigned char *raw_lba)
{
	int 	a;
	RLBA	lba;


	lba 	= raw_lba[3] + raw_lba[2]*0x100L + raw_lba[1]*0x10000L + raw_lba[0]*0x1000000L;
	//printf(">>>%x\n>>>%x\n",((int *)raw_lba)[0], (unsigned int) lba);

	return LBAtoMSF(M,S,F, lba);


}

RLBA RAW_LBAtoLBA(unsigned char *raw_lba)
{
	int 	a;
	RLBA	lba;


	lba 	= raw_lba[3] + raw_lba[2]*0x100L + raw_lba[1]*0x10000L + raw_lba[0]*0x1000000L;
	//printf(">>>%x\n>>>%x\n",((int *)raw_lba)[0], (unsigned int) lba);

	return lba;

}



#undef BYTE
#endif //__KPNC_CD_H__