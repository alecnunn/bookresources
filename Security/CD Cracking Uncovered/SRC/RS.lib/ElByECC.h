/*-[ElByECC.DLL]--------------------------------------------------------------
 *
 *						Interface to the CloneCD kernel
 *						===============================
 *
 *		The kernel of the Clone CD protected CD copier (http://www.elby.ch/)
 * is concentrated in the ElByECC.dll file,  containing several functions for
 * low-level working with sectors (computing and restoring Reed-Solomon codes,
 * scrambling, etc.), which are useful not only for creating custom protected
 * CD copiers, but also for developing protected programs and various experi-
 * ments with CDs.
 * See also Ahead Nero kernel (NewTRF.DLL), which provides reacher set of
 * functions, but in comparison to Clone CD kernel is poorly optimized.
 *	
 *
 * VERSION: 0x001
----------------------------------------------------------------------------*/
#include <stdio.h>
#include <memory.h>

#ifndef __ElByECC_by_KPNC_H__
#define __ElByECC_by_KPNC_H__

#pragma pack( push, before_include1)
#pragma pack(1)					// we do not need data alignment

//============================================================================
//						FUNDAMENTAL DATA TYPES
//============================================================================

// BYTE, WORD, DWORD
//----------------------------------------------------------------------------
#ifndef	BYTE					// defining byte
	#define BYTE unsigned char
#endif

#ifndef WORD					// defining word
	#define WORD short unsigned int
#endif

#ifndef DWORD					// defining double word
	#define DWORD unsigned int
#endif

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

//============================================================================
//								CONSTANTS
//============================================================================
#ifndef RAW_SECTOR_SIZE			// raw sector size in bytes
	#define RAW_SECTOR_SIZE	2352
#endif

#ifndef SYNC_SIZE				// size of the sync group 00 FF FF .. FF FF 00
	#define SYNC_SIZE	12
#endif

#ifndef HEADER_SIZE				// header size
	#define HEADER_SIZE	4
#endif

#ifndef SUBHEADER_SIZE			// subheader size
	#define SUBHEADER_SIZE	4		// (Attention! there are two subheaders)
#endif

#ifndef USER_DATA_SIZE			// inser data area size...
	#define USER_DATA_SIZE	2048// ...in MODE1 (default)
#endif

#ifndef USER_DATA_SIZE_MODE1			// same, for MODE1.
	#define USER_DATA_SIZE_MODE1		2048
#endif

#ifndef USER_DATA_SIZE_MODE2			// same, for MODE2.
	#define USER_DATA_SIZE_MODE2		2336
#endif

#ifndef USER_DATA_SIZE_MODE2FORM1		// same, for MODE2.
	#define USER_DATA_SIZE_MODE2FORM1	2048
#endif

#ifndef USER_DATA_SIZE_MODE2FORM2		// same, for MODE2.
	#define USER_DATA_SIZE_MODE2FORM2	2324
#endif

#ifndef EDC_SIZE				// EDC field size (CRC)
	#define EDC_SIZE	4
#endif

#ifndef ZERO_SIZE				// length of intermediate area
	#define ZERO_SIZE	8
#endif

#ifndef P_SIZE					// number of P parity bytes
	#define P_SIZE		172
#endif

#ifndef Q_SIZE					// number of Q parity bytes
	#define Q_SIZE		104
#endif


//============================================================================
//								STRUCTURES
//============================================================================


//=[MSF]======================================================================
//
//	MSF		-	structure that stores minutes, seconds and frames,
//				(mainly used for absolute addressing)
#ifndef  MSF
	struct MSF
	{
			BYTE M;		// MIN
			BYTE S;		// SEC
			BYTE F;		// FRAME
	};
#endif

//=[BCD]======================================================================
//
//	BCD:
//		structure storing minutes, seconds and frames in BCD format,
//		this is exactly the form in which this information is stored
//		in sector headers!
#ifndef BCD
	struct BCD{
			BYTE M;		// MIN
			BYTE S;		// SEC
			BYTE F;		// FRAME
	};
#endif


//=[HEADER]===================================================================
//
// HEADER:
//		structure for storing sector header
//
#ifndef HEADER
	struct HEADER
	{
		struct BCD	ADDR;
		BYTE		MODE;
	};
#endif

//=[SUBHEADER]================================================================
//
// SUBHEADER:
//		subheader of MODE2FORM1 and MODE2FORM2 modes
//
#ifndef SUBHEADER
	struct SUBHEADER
	{
			BYTE	FILE_NUM;			// to which file it belongs?
			BYTE	CHANNE_NUM;			// to which channel it belongs?
			BYTE	SUB_MODE;			// mode
			BYTE	CODING_INFO;		// coding information
	};
#endif

//=[RAW_SECTOR]===============================================================
//
// RAW_SECTOR:
//		simply raw sector
//
#ifndef RAW_SECTOR
	struct RAW_SECTOR
	{
		BYTE	RAW[RAW_SECTOR_SIZE];
	};
#endif

//=[RAW_SECTOR_MODE1]=========================================================
//
//	RAW_SECTOR_MODE1:
//		structure storing MODE1 sector as is
//
#ifndef RAW_SECTOR_MODE1
	struct RAW_SECTOR_MODE1
	{
			BYTE		SYNC[SYNC_SIZE];			// sync group
			struct		BCD ADDR;					// absolute sector address
			BYTE		MODE;						// sector type
			BYTE		USER_DATA[USER_DATA_SIZE];	// user data
			BYTE		EDC[EDC_SIZE];				// checksum
			BYTE		ZERO[ZERO_SIZE];			// zeroes (not used)
			BYTE		P[P_SIZE];					// P parity bytes
			BYTE		Q[Q_SIZE];					// Q parity bytes
	};
#endif

//=[RAW_SECTOR_MODE2]=========================================================
//
//	RAW_SECTOR_MODE2:
//		structure storing MODE2 sector as is
//
#ifndef RAW_SECTOR_MODE2
	struct RAW_SECTOR_MODE2
	{
			BYTE		SYNC[SYNC_SIZE];			// sync group
			struct		BCD ADDR;					// absoulte sector address
			BYTE		MODE;						// sector type
			BYTE		USER_DATA[USER_DATA_SIZE_MODE2];
													// user data
	};
#endif

//=[RAW_SECTOR_MODE2FORM1]====================================================
//
//	RAW_SECTOR_MODE2FORM1:
//		structures storing the MODE2FORM1 sector as is
//
#ifndef RAW_SECTOR_MODE2FORM1
	struct RAW_SECTOR_MODE2FORM1
	{
		BYTE		SYNC[SYNC_SIZE];			// sync group
		struct		BCD ADDR;					// absolute sector address
		BYTE		MODE;						// sector type
		struct		SUBHEADER subheader;		// main subheader
		struct		SUBHEADER subheader_2;		// secondary subheader
		BYTE		USER_DATA[USER_DATA_SIZE_MODE2FORM1];
												// user data
		BYTE		EDC[EDC_SIZE];				// checksum
		BYTE		P[P_SIZE];					// P parity bytes
		BYTE		Q[Q_SIZE];					// Q parity bytes
	};
#endif

//=[RAW_SECTOR_MODE2FORM2]====================================================
//
//	RAW_SECTOR_MODE2FORM2:
//		stryctyre stirubg tge MODE2FORM2 sector as is
//
#ifndef RAW_SECTOR_MODE2FORM2
	struct RAW_SECTOR_MODE2FORM2
	{
		BYTE		SYNC[SYNC_SIZE];			// sync group
		struct		BCD ADDR;					// absolute sector address
		BYTE		MODE;						// sector type
		struct		SUBHEADER subheader;		// main subheader
		struct		SUBHEADER subheader_2;		// secondary subheader
		BYTE		USER_DATA[USER_DATA_SIZE_MODE2FORM2];
												// user data
		BYTE		EDC[EDC_SIZE];				// checksum
	};
#endif

//
//============================================================================
//									FUNCTIONS
//============================================================================
//

// DEFINITIONS OF MAIN FLAGS AND RETURN CODES
//----------------------------------------------------------------------------
#define ElBy_TEST_ONLY		0				// test only
#define	ElBy_REPAIR			1				// test and corredt if necessary.

#define ElBy_SECTOR_OK		1				// sector ОК
#define ElBy_SECTOR_ERROR	0				// sector with error
#define ElBy_NEED_REPAIR	0				// sector needs repair.


// track types
//----------------------------------------------------------------------------
#define ElBy_TRACK_MODE0		3
#define ElBy_TRACK_MODE1		0
#define ElBy_TRACK_MODE2		NOT_SUPPORT
#define ElBy_TRACK_MODE2FORM1	1
#define ElBy_TRACK_MODE2FORM2	2
#define ElBy_TRACK_ERR			4

#define ElBy_TRACK_TEXT(a)	((a == ElBy_TRACK_MODE0)		? "MODE0":\
							 (a == ElBy_TRACK_MODE1)		? "MODE1":\
							 (a == ElBy_TRACK_MODE2)		? "MODE2":\
							 (a == ElBy_TRACK_MODE2FORM1)	? "MODE2FORM1":\
							 (a == ElBy_TRACK_MODE2FORM2)	? "MODE2FORM2":\
							 "UNKNOWN MODE")

#define NOT_SUPPORT			-1				// this mode is not supported

// FUNCTION PROTOTYPES AND THEIR BRIEF DESCRIPTIONS
//============================================================================


// functions fo converting data types
//----------------------------------------------------------------------------
extern		BCDtoMSF(struct BCD *bcd, struct MSF *msf);
extern		MSFtoBCD(struct MSF *msf, struct BCD *bcd);
extern		LBAtoMSF(RLBA LBA, struct MSF *msf);
extern		ULBAtoMSF(ULBA LBA, struct MSF *msf);
extern RLBA	MSFtoLBA(struct MSF *msf);
extern ULBA	MSFtoULBA(struct MSF *msf);
extern		LBAtoBCD(RLBA lba, struct BCD *bcd);
extern		ULBAtoBCD(ULBA lba, struct BCD *bcd);
extern RLBA	BCDtoLBA(struct BCD *bcd);
extern ULBA	BCDtoULBA(struct BCD *bcd);

//=[IdentifyTrack]============================================================
//
//		Defines the type of the track passed by MODE and submode, requiring
// the presence of correct SYNC
//		The type is determined according to the Yellow Book (using the 
// entire MODE byte).
// Therefore, sector run-in/run-out are considered sectors of unknown type,
// which results in incorrect operation of the CheckSector/CheckSectors functions,
// because it calls the IdentifyTrack function for determining track type!
// Determine the track type manyually by calling forcing functions with specifying
// the track type in the prototype. They operate correctly, 
// ---------------------------------------------------------------------------
//
// ARG:
//		sector			-	pointer to sector the type of which must be defined
//
// RET:
//		0				-	MODE1
//		1				-	MODE2FORM1
//		2				-	MODE2FORM2
//		3				-	MODE0
//		4				-	NO SYNC (NOT VALID SECTOR)
//
// REM:
//		1) CloneCD  assumes that MODE2 always has FORM1/FORM2
//		2) If SubMode and 4 trailing bytes !=0, there is an error in MODE0
//
extern IdentifyTrack(struct RAW_SECTOR *sector);



//=[CheckSector]==============================================================
//
//							CheckSector family
//							=====================
//
//		To check the sector integrity and try to recover it if necessary using
// Reed-Solomon redundant codes (for MODE2FORM2 mode only the checksum is
// checked, in MODE0 & MODE2 modes even this check is missing)
//		The sector must be loaded into the memory, no alignment is needed,
// when checking a group of sectors, they must be located in memory sequentially
// at a distance raw_sector_size, which can exceed the size of a single sector.
//----------------------------------------------------------------------------
//
//	ARG:
//		*sector			-	pointer to the sector of appropriate type
//
//		raw_sector_size	-	sector size with tail (if any)
//
//		DO				-	== ElBy_TEST_ONLY	: check only
//							== ElBy_REPAIE		: check and repair 
//
//		N_SEC			-	number of sectors to check
//
//	RET:
//		ElBy_SECTOR_OK		sector is OK or was successfully recovered
//		ElBy_SECTOR_ERROR	sector is invalid or cannot be recovered
//
extern CheckSector(struct RAW_SECTOR *sector, int DO);
extern CheckSectors(struct RAW_SECTOR *sector, int raw_sector_size, int N_SEC, int DO);
extern CheckECCAndEDC_Mode1(struct RAW_SECTOR_MODE1 *sector , int DO);
extern CheckECCAndEDC_Mode2Form1(struct RAW_SECTOR_MODE2FORM1 *sector , int DO);
extern CheckECCAndEDC_Mode2Form2(struct RAW_SECTOR_MODE2FORM1 *sector);



//=[GenerateSector]==============================================================
//
//		Generating sectors of various types for all cases, based on the specified
// USER_DATA, MODE and ADDR (see also the GEN family)
// ---------------------------------------------------------------------------
//	ARG:
//		raw_sector_dst	-	destination sector
//							ATTENTION! USER_DATA is not always initialized, therefore
//							you must do it manually! See the DO description
//
//		DO				-	What to to?
//							== 0:	build SYNC and HEADER_ADDR based on the
//									passed address with MODE0 and fill
//									USER_DATA with zeroes;
//
//							== 1:	build sector in MODE1, calculate
//									EDC/ECC by USER_DATA. USER_DATA  itself
//									must not be copied into the sector;
//
//							== 2:	build SYNC and HEADER_ADDR in MODE2,
//									leaving everything else intact;
//
//							== 3:	build sector in MODE2FORM1, compute
//									EDC/ECC by USER, do not copy USER_DATA 
//									into the destination sector.
//
//							== 4:	build sector in MODE2FORM, compute
//									EDC by USER, do not copy USER itself;
//
//							== 5:	build BAD sector in MODE1 (SYNC&HEADER
//									& UUUUUUU);
//
//							== 6:	build BAD sector in MODE2 (SYNC&HEADER
//									& UUUUUU)
//	
//		LBA				-	LBA sector address
//
//		USER_DATA		-	pointer to USER_DATA by which EDC/ECC is 
//							calculated
//							ATTENTION! If least significant byte of address != 0, then field M
//							of the address is OE'ed with A0h!
//
// RET:
//
//		ElBy_SECTOR_OK		everything OK
//
//		ElBy_SECTOR_ERROR	sector generation error
//
extern GenerateSector	(struct RAW_SECTOR *raw_sector_dst, int DO,
						 RLBA lba, char *user_data);


//=[GenECCandEDC_ModeX]=======================================================
//
//					GenECCamdEDC_ModeX (GEN) family
//					------------------------------------
//
//		compute EDC/ECC fields for USER_DATA and add SYNC+HEADER
//----------------------------------------------------------------------------
// ARG:
//		userdata		-	sector's user data
//		header			-	header (address/mode)
//		subheader		-	subheader (two instances) for copying
//		sector_dst		-	generated sector
//
// RET:
//		ElBy_SECTOR_OK		everything OK
//		ElBy_SECTOR_ERROR	sector was not built
extern GenECCAndEDC_Mode1(char *userdata_src, char *header_src,
						struct RAW_SECTOR_MODE1 *raw_sector_mode1_dst);

extern GenECCAndEDC_Mode2Form1(char *userdata_src, char *header_src,
						char *subheader_src, // внимание! two subheaher need
						struct RAW_SECTOR_MODE2FORM1 *raw_sector_mode2form1_dst);

extern GenECCAndEDC_Mode2Form2(char *userdata_src, char *header_src,
						char *subheader_src,// внимание! two subheaher need
						struct RAW_SECTOR_MODE2FORM2 *raw_sector_mode2form2_dst);


//=[ReGenECCandEDC_ModeX]=====================================================
//
//						ReGenECCandEDC_ModeX family
//						===========================
//
//		Recalculate EDC/ECC fields for USER_DATA. Similar to the GEN family,
// however, in contrast to it, this family is oriented towards ready sectors,
// in contrast to GENs, which are oritented towards creating sectors from scratch.
//----------------------------------------------------------------------------
// ARG:
//		rawsector		-	pointer to sector
//
// RET:
//		ElBy_SECTOR_OK		everything OK
//		ElBy_SECTOR_ERROR	sector was not built
//
extern ReGenECCAndEDC				(struct RAW_SECTOR *raw_sector);
extern ReGenECCAndEDC_Mode1			(struct RAW_SECTOR_MODE1 *raw_sector_mode1);
extern ReGenECCAndEDC_Mode2Form1	(struct RAW_SECTOR_MODE2FORM1 *raw_sector_mode2form1);



//=[scrambler]================================================================
//
//						SCRAMBLER family
//						================
//
//	Carries out sector scrambling/descrambling, which, by the way,
//  doesn't make any difference, because scrambling function is symmetric.
//
//		Functions with the Raw prefix do not require the SYNC presence,
// other functions require it. SYNC itself is never scrambled
//----------------------------------------------------------------------------
//
// ARG:
//		raw_sector		-	sector for scrambling 
//		N_SEC			-	number of sectors for scrambling
//							(ATTENTION! ElByECC.DLL ver 4.0.0.0 contains a bug,
//							which crashes RawScrambleSectors when N_SEC == 0.
//							ScrambleSectors/UnScrambleSectors are free from this bug)
//
//		raw_sec_size	-	size of one sector
//
// RET:
//		ElBy_SECTOR_OK		everything ОК (Raw doesn't return a result)
//		ElBy_SECTOR_ERROR	error (Raw doesn't return a result)
//
extern RawScrambleSector(struct RAW_SECTOR *raw_sector);
extern RawScrambleSectors(struct RAW_SECTOR *raw_sector, int N_SEC, int raw_sec_size);
extern ScrambleSector(struct RAW_SECTOR *raw_sector);
extern ScrambleSectors(struct RAW_SECTOR *raw_sector, int N_SEC, int raw_sector_size);
extern UnScrambleSector(struct RAW_SECTOR *raw_sector);
extern UnScrambleSectors(struct RAW_SECTOR *raw_sector, int N_SEC, int raw_sector_size);


//=[GenHeadAndSyc]============================================================
//
//		Build sector zero for Lead-In area
//----------------------------------------------------------------------------
// ARG:
//		raw_sector_dst	-	destination sector
//
//		f_buil			-	== 0,1: build header
//							>= 2  : do not build header
//
//		lba				-	LBA address
//
//		f_lba				== 0 : subtract 150 sectors from lba
//							!= 0 : take lba as is
//
//
// NOTE:
//		actually writes only SYNC & ADDR into HEADER, converting from LBA to M:S:F
//
extern GenHeadAndSyc_4_RawDatenLeadin_0Sect(struct RAW_SECTOR *raw_sector_dst,int f_build, ULBA lba, int f_lba);

// Sync-template
//extern SyncByteTemplate;

#pragma pack( pop, before_include1 )

#define LEN_SCREEN	80
print_hex(unsigned char *p, int len)
{
	int a;
	for (a = 0; a < len; a++)
	{
		printf("%02X ", p[a]);
		if ((a+1) % ((LEN_SCREEN - 1)/3) == 0) printf("\n");
	}printf("\n");
	return 0;
}

print_sector(struct RAW_SECTOR *raw_sector)
{
	int	xMODE;

	struct RAW_SECTOR_MODE1			*raw_sector_mode1;
	struct RAW_SECTOR_MODE2			*raw_sector_mode2;
	struct RAW_SECTOR_MODE2FORM1	*raw_sector_mode2form1;
	struct RAW_SECTOR_MODE2FORM2	*raw_sector_mode2form2;

	raw_sector_mode2 = (struct RAW_SECTOR_MODE2*) raw_sector;
	raw_sector_mode1 = (struct RAW_SECTOR_MODE1*) raw_sector;

	raw_sector_mode2form1 = (struct RAW_SECTOR_MODE2FORM1*) raw_sector;
	raw_sector_mode2form2 = (struct RAW_SECTOR_MODE2FORM2*) raw_sector;

	xMODE = IdentifyTrack(raw_sector);

	printf("\nsector TYPE: %s{\n", ElBy_TRACK_TEXT(xMODE));
	// printing the header (it is universal for all types of sectors)
	printf("\nSYNC:\n");		print_hex((unsigned char *) raw_sector_mode2, SYNC_SIZE);
	printf("\nHEADER:\n");		print_hex((unsigned char *) &raw_sector_mode2->ADDR, HEADER_SIZE);
	
	switch(xMODE)
	{
		case	ElBy_TRACK_ERR:
		case	ElBy_TRACK_MODE0:
		case	ElBy_TRACK_MODE2:
					printf("\nUSER DATA:\n"); print_hex((unsigned char *)&raw_sector_mode2->USER_DATA, USER_DATA_SIZE_MODE2);
					break;

		case	ElBy_TRACK_MODE1:
					printf("\nUSER DATA:\n"); print_hex((unsigned char *)&raw_sector_mode1->USER_DATA, USER_DATA_SIZE);
					printf("\nEDC:\n"); print_hex((unsigned char *)&raw_sector_mode1->EDC, EDC_SIZE);
					printf("\nZERO:\n"); print_hex((unsigned char *)&raw_sector_mode1->ZERO, ZERO_SIZE);
					printf("\nP-CODE:\n"); print_hex((unsigned char *)&raw_sector_mode1->P, P_SIZE);
					printf("\nQ-CODE:\n"); print_hex((unsigned char *)&raw_sector_mode1->Q, Q_SIZE);
					break;

		case	ElBy_TRACK_MODE2FORM1:
					printf("\nSUB-HEADER:\n"); print_hex((unsigned char *)&raw_sector_mode2form1->subheader, SUBHEADER_SIZE*2);
					printf("\nUSER DATA:\n"); print_hex((unsigned char *)&raw_sector_mode2form1->USER_DATA, USER_DATA_SIZE);
					printf("\nEDC:\n"); print_hex((unsigned char *)&raw_sector_mode2form1->EDC, EDC_SIZE);
					printf("\nP-CODE:\n"); print_hex((unsigned char *)&raw_sector_mode2form1->P, P_SIZE);
					printf("\nQ-CODE:\n"); print_hex((unsigned char *)&raw_sector_mode2form1->Q, Q_SIZE);
					break;

		case	ElBy_TRACK_MODE2FORM2:
					printf("\nSUB-HEADER:\n"); print_hex((unsigned char *)&raw_sector_mode2form1->subheader, SUBHEADER_SIZE*2);
					printf("\nUSER DATA:\n"); print_hex((unsigned char *)&raw_sector_mode2form2->USER_DATA, USER_DATA_SIZE_MODE2FORM2);
					printf("\nEDC:\n"); print_hex((unsigned char *)&raw_sector_mode2form2->EDC, EDC_SIZE);
					break;
	}
	printf("} sector TYPE: %s\n", ElBy_TRACK_TEXT(xMODE));
	return 0;
}

#endif //__ElByECC_by_KPNC_H__

