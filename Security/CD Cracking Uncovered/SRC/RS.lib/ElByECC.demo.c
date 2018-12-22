/*----------------------------------------------------------------------------
 *
 *							ElByECC.DLL demo
 *							================
 *
 * This program demonstrates working with ElByECC.DLL,
 * generating Reed-Solomon redundant codes on the basis of user data,
 * then intentionally invalidates them and carries out data recovery.
 *		Number of invalid bytes is passed in the first command-line
 * parameter (6 by default)
----------------------------------------------------------------------------*/
#include <stdio.h>
#include "ElByECC.h"								

#define _DEF_DMG	6
#define _DEF_PNT	0
#define _DEF_STP	1
#define _raw_X_STEP	 ((argc>2)?atol(argv[2]):_DEF_STP)
#define _raw_X_POINT ((argc>3)?atol(argv[3]):_DEF_PNT)

#define N_BYTES_DAMAGE	((argc>1)?atol(argv[1]):_DEF_DMG)
#define X_STEP			((_raw_X_POINT+_raw_X_STEP*N_BYTES_DAMAGE>USER_DATA_SIZE)?0 & printf("ERR"):_raw_X_STEP)
#define X_POINT		_raw_X_POINT

main(int argc, char **argv)
{
	int a;
	char stub_head[HEADER_SIZE];					// sector header
	char user_data[USER_DATA_SIZE];					// user data area

	struct RAW_SECTOR_MODE1 raw_sector_for_damage;	// sector for invalidation
	struct RAW_SECTOR_MODE1 raw_sector_for_compre;	// sector copy for checking.

	// TITLE
	//------------------------------------------------------------------------
	printf("= ElByECC.DLL usage demo example by KK\n");

	// initializing user data
	//------------------------------------------------------------------------
	printf("user data initialize...............");
	for (a = 0; a < USER_DATA_SIZE; a++) user_data[a] = a;	// user_data  init
	memset(stub_head, 0, HEADER_SIZE); stub_head[3] = 1;	// src header init
	printf("+OK\n");

	// generating Reed-Solomon codes on the basis of user data
	//-----------------------------------------------------------------------
	printf("RS-code generate...................");
	a = GenECCAndEDC_Mode1(user_data, stub_head, &raw_sector_for_damage);
	if (a == ElBy_SECTOR_ERROR) { printf("-ERROR!\x7\n"); return -1;}
	memcpy(&raw_sector_for_compre, &raw_sector_for_damage, RAW_SECTOR_SIZE);
	printf("+OK\n");

	// intentional invalidation of user data
	//------------------------------------------------------------------------
	printf("user-data %04d bytes damage........", N_BYTES_DAMAGE);
	for (a=0;a<N_BYTES_DAMAGE;a++) raw_sector_for_damage.USER_DATA[a*X_STEP]^=0xFF;
	if(!memcmp(&raw_sector_for_damage,&raw_sector_for_compre,RAW_SECTOR_SIZE))
		printf("-ERR: NOT DAMAGE YET\n"); else printf("+OK\n");

	// checking integrity of user data
	//------------------------------------------------------------------------
	printf("user-data check....................");
	a = CheckSector((struct RAW_SECTOR*)&raw_sector_for_damage,ElBy_TEST_ONLY);
	if (a==ElBy_SECTOR_OK){
		printf("-ERR:data not damage\x7\n");return -1;}printf(".data damge\n");

	// recovery of user data
	//------------------------------------------------------------------------
	printf("user-data recorver.................");
	a = CheckSector((struct RAW_SECTOR*)&raw_sector_for_damage, ElBy_REPAIR);
	if (a == ElBy_SECTOR_ERROR) {
		printf("-ERR: NOT RECORVER YET\x7\n"); return -1; } printf("+OK\n");

	// checking if recovery was successful
	//------------------------------------------------------------------------
	printf("user-data recorver check...........");
	if(memcmp(&raw_sector_for_damage,&raw_sector_for_compre,RAW_SECTOR_SIZE))
	 	printf("-ERR: NOT RECORVER YET\x7\n"); else printf("+OK\n");

	printf("+OK\n");
	return 1;
}